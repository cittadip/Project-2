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

#define UART_INTRO_KEY 0
#define UART_ENGINE_KEY 1
#define UART_ERROR_KEY 2

#define HEADLIGHT_OFF 3
#define HEADLIGHT_AUTO 4
#define HEADLIGHT_ON 5

//=====[Declaration and initialization of public global objects]===============

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
DigitalOut headlightLeft(D6);
DigitalOut headlightRight(D7);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);
// UART interface for communication with a computer or external device
// USBTX and USBRX represent transmit and receive pins, respectively
// 115200 is the baud rate for serial communication

//=====[Declaration and initialization of public global variables]=============

int introComplete = false; // Tracks whether the welcome message for the driver
                           // has been displayed
bool ignitionButtonState = OFF;
float potentiometerReading = 0.0;
float potentiometerReadingScaled = 0.0;
float lightReading = 0.0;
bool ignitionPrevious = ON;

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
int potentiometerValue();
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
bool waitForRelease = false; 

void ignitionState() {
    if (!ignitionButton && ignitionPrevious) {
        if (!waitForRelease) {
            ignitionButtonState = !ignitionButtonState;
            waitForRelease = true;
      }
    
}
if (ignitionButton == 1 && ignitionPrevious == 0) {
          waitForRelease = false;
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
  if (!ignitionButtonState) {
    // Check if all required conditions are met for engine start
    if (driverPresent && passengerPresent && driverSeatbelt &&
        passengerSeatbelt) {
      uartCommands(UART_ENGINE_KEY);

      // maybe another do while loop
      do {
        // Turn off the green indicator and activate the blue indicator
        greenIndicator = OFF;
        //  keep on blue led
        blueIndicator = ON;
        sirenPin.input();
        //  also check for headlights settings here
        ignitionState();
        headlightState();
      } while (!ignitionButtonState);
      blueIndicator = OFF;
      headlightOff();
      // if engine is off the headlights should be turned off
    } else {
      // Display ignition failure message and reasons via UART
      // Report individual reasons for failure
      uartCommands(UART_ERROR_KEY);

      do {
        sirenPin.output();
        sirenPin = LOW; // Activate siren signal
        drivingState();
      } while (!(driverPresent && passengerPresent && driverSeatbelt &&
                 passengerSeatbelt));
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
*/
void uartCommands(int cmd) {
  switch (cmd) {
  case 0:
    uartUsb.write("\nWelcome to enhanced alarm system model 218-W24", 46);
    break;

  case 1:
    uartUsb.write("\nEngine started.", 15);
    break;
  case 2:
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
  default:
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

int potentiometerValue() {
  int state;
  float reading = headlightKnob.read();
  float val = reading / 0.1;

  if (val >= 6) {
    state = HEADLIGHT_OFF;
  } else if (val > 3 && val <= 6) {
    state = HEADLIGHT_AUTO;
  } else {
    state = HEADLIGHT_ON;
  }

  return state;
}

void headlightState() {
  switch (potentiometerValue()) {
  case HEADLIGHT_AUTO:
    headlightAuto();
    break;
  case HEADLIGHT_OFF:
  delay(2000);
    headlightOff();
    break;
  case HEADLIGHT_ON:
  delay(1000);
    headlightOn();
    break;
  }
}

void headlightAuto() {
  // if the light sensor is below dusk level;
  lightReading = lightReader.read();
  lightReading = lightReading / .1;

  if (lightReading < 8) {
    headlightOn();
  } else {
    headlightOff();
  }
}

void headlightOff() {
  
  headlightLeft = OFF;
  headlightRight = OFF;
}

void headlightOn() {
  
  headlightLeft = ON;
  headlightRight = ON;
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
