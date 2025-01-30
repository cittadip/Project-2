// This program simulates an enhanced vehicle alarm system with various safety
// features. It uses input signals to detect driver and passenger presence,
// seatbelt fastening, and ignition button presses, and outputs status via LEDs
// and UART messages.
//**CHANGE DESCRIPTION**

//=====[Libraries]=============================================================

#include "arm_book_lib.h"
#include "mbed.h"

//=====[Defines]===============================================================

#define UART_INTRO_KEY 0
#define UART_ENGINE_KEY 1
#define UART_ERROR_KEY 2

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
bool ignitionState = OFF;
float potentiometerReading = 0.0;
float potentiometerReadingScaled = 0.0;
float lightReading = 0.0;
bool ignitionPrevious = OFF;

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
string potentiometerValue();

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

void buttonReleased() {
    if (ignitionButton == ON && ignitionPrevious = ignitionButton) {
        unsigned long start = millis();
    }
    if (ignitionButton == OFF && ignitionPrevious == ON) {
         
    }

    ignitionPrevious = ignitionButton;
}

void ignitionState() {
    if (ignitionButton) {
        ignitionPrevious = ignitionButton;
        delay(20);
    }
    if (ignitionButton == ignitionPrevious) {
        ignitionState = !ignitionState;
    }
}

/*
activates when the ignition button is pressed and acts based on a case-to-case
basis Ex. uart messages are printed based on which buttons are not pressed to
start the engine if all buttons are pressed the engine will start
*/
void ignitionCase() {
    // maybe change the if statement to look for a button release
  if (ignitionButton) {
    // Check if all required conditions are met for engine start
    if (driverPresent && passengerPresent && driverSeatbelt &&
        passengerSeatbelt) {
      uartCommands(UART_ENGINE_KEY);
        delay(500);
      

      //** CHANGE THIS**
      // implement case to look for key press and release

      //maybe another do while loop
        do {
            // Turn off the green indicator and activate the blue indicator
            greenIndicator = OFF;
            //  keep on blue led 
            blueIndicator = ON;
            sirenPin.input();
            //  also check for headlights settings here
            ignitionState();
            headlightState();
         } 
         while (!ignitionState);
        // if engine is off the headlights should be turned off
    } else {
      // Display ignition failure message and reasons via UART
      // Report individual reasons for failure
      uartCommands(UART_ERROR_KEY);

      do {
      sirenPin.output();
      sirenPin = LOW; // Activate siren signal
      }
      while (!(driverPresent && passengerPresent && driverSeatbelt &&
             passengerSeatbelt));
      }
    }
  }

/*
prints the introduction string in uart when the driver detected for the first time
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
  } 
  else {
    greenIndicator = OFF; // One or more conditions not met, unsafe state
  }
}

typedef struct Controller {
//    do all the potentiometer initializations either at the top or near here
   int pin;
   int val;
   string state;

   reading = pin.read();
   val =  reading * 3.3 / 0.01;

   if (val <= 30) { 
     state = "off";
   }
    else if (val > 30 || val < = 60) {
    state = "auto";
    }
    else { 
    state = "on";
    }
} Controller;

string potentiometerValue() {
     Controller headlightMode;
     headlightMode.pin = headlightKnob;
     return headlightMode.state;
}

void headlightState() {
    switch (potentiometerValue()) {
    case "auto":
        headlightAuto();
        break;
    case "off":
        headlightOff();
        break;
    case "on":
        headlightsOn();
        break;
    }
}

void headlightAuto() {
    // if the light sensor is below dusk level;
    lightReading = lightReader.read();
    lightReading = lightReading / .01;

        if (lightReading < 7) {
            headlightsOn();
        }
        else {
            headlightOff();
        }
}

void headlightOff() {
    delay(2000);
    headlightLeft = OFF;
    headlightRight = OFF;
}

void headlightOn() {
    delay(1000);
    headlightLeft = ON;
    headlightRight = ON;
}

//=====[Main function, the program entry point after power on or reset]========

int main() {
  inputsInit();  // Initialize all input pins and their modes
  outputsInit(); // Initialize output pins and their default states

  // Main loop: Continuously check and update system states
  while (true) {
    drivingState();       // Update driving indicators based on safety inputs
    ignitionCase();       // Handle ignition logic and system response
  }
}
