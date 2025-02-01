// This program simulates an enhanced vehicle alarm system with various safety
// features. It uses input signals to detect driver and passenger presence,
// seatbelt fastening, and ignition button presses, and outputs status via LEDs
// and UART messages.
//**CHANGE DESCRIPTION**

//=====[Libraries]=============================================================

#include "arm_book_lib.h"
#include "mbed.h"
#include <string>

//=====[Defines]===============================================================

// uart macros
#define UART_INTRO_KEY 0
#define UART_ENGINE_KEY 1
#define UART_ERROR_KEY 2

// headlight macros
#define HEADLIGHT_OFF 3
#define HEADLIGHT_AUTO 4
#define HEADLIGHT_ON 5

// headlight delay macros
#define HEADLIGHT_ON_DELAY 1000
#define HEADLIGHT_OFF_DELAY 2000

// light sensor macros
#define DUSK 8

// potentiometer macros
#define LEFT_LIMIT 6
#define RIGHT_LIMIT 3

//=====[Declaration and initialization of public global objects]===============

DigitalIn highbeamsSwitch(D1);
DigitalIn driverPresent(D2);
DigitalIn passengerPresent(D3);
DigitalIn driverSeatbelt(D4);
DigitalIn passengerSeatbelt(D5);
DigitalIn ignitionButton(BUTTON1);
AnalogIn headlightKnob(A0);
AnalogIn lightReader(A1);

DigitalOut greenIndicator(LED1);
DigitalInOut sirenPin(PE_10);
DigitalOut blueIndicator(LED2);
DigitalOut headlights(D6);
DigitalOut highbeams(D7);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);
// UART interface for communication with a computer or external device
// USBTX and USBRX represent transmit and receive pins, respectively
// 115200 is the baud rate for serial communication

//=====[Declaration and initialization of public global variables]=============

int introComplete = false; // Tracks whether the welcome message for the driver
                           // has been displayed

float potentiometerReading = 0.0;
float potentiometerReadingScaled = 0.0;

float lightReading = 0.0;

// variables used in tracking the falling edge of the ignition button
bool ignitionButtonState = OFF; 
bool ignitionPrevious = ON;
bool waitForRelease = true;

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();
void ignitionCase();
void driverIntroduction();
void drivingState();
void headlightState();
void headlightAuto();
void headlightOff();
void headlightOn();
int headlightMode();
void ignitionState();
void uartCommands(int cmd);

//=====[Implementations of public functions]===================================

void inputsInit() {
  driverPresent.mode(PullDown);
  passengerPresent.mode(PullDown);
  driverSeatbelt.mode(PullDown);
  passengerSeatbelt.mode(PullDown);
  ignitionButton.mode(PullDown);
}

void outputsInit() {
  sirenPin.mode(OpenDrain);
  sirenPin.input();

  blueIndicator = OFF;  // Blue LED: Indicates engine running
  greenIndicator = OFF; // Green LED: Indicates safe state for operation
}

/*
Tracks the falling edge of the ignition button
Allows for the ignition button to turn the engine off on release
*/
void ignitionState() {
  if (ignitionButton && !ignitionPrevious) {
    if (waitForRelease) {
      ignitionButtonState = !ignitionButtonState;
      waitForRelease = false;
    }
  }
  if (!ignitionButton && ignitionPrevious) {
    waitForRelease = true;
  }
  ignitionPrevious = ignitionButton;
}

/*
activates when the ignition button is pressed and acts based on a case-to-case
basis Ex. uart messages are printed based on which buttons are not pressed to
start the engine if all buttons are pressed the engine will start
*/
void ignitionCase() {
  // maybe change the if statement to look for a button release
  ignitionState();
  if (ignitionButtonState) {
    // Check if all required conditions are met for engine start
    if (driverPresent && passengerPresent && driverSeatbelt &&
        passengerSeatbelt) {
      uartCommands(UART_ENGINE_KEY);

      // this loop does 1 loop of all the functions that would occur if when the
      // engine is on before checking if the button was pushes and released
      // again
      do {
        // Turn off the green indicator and activate the blue indicator
        greenIndicator = OFF;
        //  keep on blue led
        blueIndicator = ON;
        sirenPin.input();
        //  also check for headlights settings here
        ignitionState();
        headlightState();
      } while (ignitionButtonState);

      // if engine is off the headlights and blue indicator should be turned off
      blueIndicator = OFF;
      headlightOff();
    } else {
      // Display ignition failure message and reasons via UART
      // Report individual reasons for failure
      uartCommands(UART_ERROR_KEY);

      // The alarm will keep sounding until the engine is properly
      // started eg. passenger/driver present and seatbelts on
      do {
        sirenPin.output();
        sirenPin = LOW; // Activate siren signal
        drivingState();
      } while (!(driverPresent && passengerPresent && driverSeatbelt &&
                 passengerSeatbelt));
      ignitionButtonState = OFF;
    }
  }
}

/*
prints the introduction string in uart when the driver detected for the first
time
*/
void driverIntroduction() {
  if (driverPresent && !introComplete) {
    uartCommands(UART_INTRO_KEY);
    introComplete = true;
  }
}

/*
a list of UART serial prints depending on the situation the car is in
@param cmd the uart that is selected to be printed
*/
void uartCommands(int cmd) {
  switch (cmd) {
  case UART_INTRO_KEY:
    uartUsb.write("\nWelcome to enhanced alarm system model 218-W24", 46);
    break;

  case UART_ENGINE_KEY:
    uartUsb.write("\nEngine started.", 15);
    break;
  case UART_ERROR_KEY:
    uartUsb.write("\nIgnition inhibited", 19);
    uartUsb.write("\nReasons:", 9);

    if (!driverPresent) {
      uartUsb.write("\nDriver not present.", 20);
    }
    if (!passengerPresent) {
      uartUsb.write("\nPassenger not present.", 23);
    }
    if (!driverSeatbelt) {
      uartUsb.write("\nDriver Seatbelt not fastened.", 30);
    }
    if (!passengerSeatbelt) {
      uartUsb.write("\nPassenger Seatbelt not fastened.", 33);
    }
    break;
  }
}

/*
tells the program what state the code is in; if green led should be on/off
and/or ignition pressed

The green indicator lights up when all safety conditions (driver and
passenger present, seatbelts fastened) are met, indicating that the vehicle
is in a safe state to operate.
*/
void drivingState() {
  driverIntroduction();
  if (driverPresent && passengerPresent && driverSeatbelt &&
      passengerSeatbelt) {
    greenIndicator = ON; // All conditions met, safe state
  } else {
    greenIndicator = OFF; // One or more conditions not met, unsafe state
  }
}

/*
takes the potentiometer data and turns that into the select headlight mode
left = off, mid = auto, right = on
@return state of headlight in the form of a constant int variable
*/
int headlightMode() {
  int state;
  float reading = headlightKnob.read();
  float val = reading / 0.1;

  if (val >= LEFT_LIMIT) {
    state = HEADLIGHT_OFF;
  } else if (val > RIGHT_LIMIT && val <= LEFT_LIMIT) {
    state = HEADLIGHT_AUTO;
  } else {
    state = HEADLIGHT_ON;
  }

  return state;
}

/*
uses the headlight mode and does its according job
auto - depending on light turns on/off the headlight
on - turns on headlight
off - turns off headlight
*/
void headlightState() {
  switch (headlightMode()) {
  case HEADLIGHT_AUTO:
    headlightAuto();
    break;
  case HEADLIGHT_OFF:
    headlightOff();
    break;
  case HEADLIGHT_ON:
    headlightOn();
    break;
  }
}

/*
toggles between turning the headlights on or off depending on how dark it is
*/
void headlightAuto() {
  // if the light sensor is below dusk level;
  lightReading = lightReader.read();
  lightReading = lightReading / .1;

  if (lightReading < DUSK) {
      delay(HEADLIGHT_ON_DELAY);
    headlightOn();
  } else {
      delay(HEADLIGHT_OFF_DELAY);
    headlightOff();
  }
}

/*
turns the headlights off
turns highbeams off 
*/
void headlightOff() {
  headlights = OFF;
  highbeams = OFF;
}

/*
turns headlights on
turns on high beams if the manual high beam switch is on
*/
void headlightOn() {
  headlights = ON;

  if (highbeamsSwitch) {
    highbeams = ON;
  }
  else {
      highbeams = OFF;
  }
}

//=====[Main function, the program entry point after power on or reset]========

int main() {
  inputsInit();  // Initialize all input pins and their modes
  outputsInit(); // Initialize output pins and their default states

  // Main loop: Continuously check and update system states
  while (true) {
    drivingState(); // Update driving indicators based on safety inputs
    ignitionCase(); // Handle ignition logic and system response
  }
}

//     char str[100] = "";
//     sprintf ( str, "%.2f \r\n",
//                     lightReading );
//     int stringLength = strlen(str);
//      uartUsb.write(str, stringLength);
