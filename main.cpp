//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

//Butons
DigitalIn driver_occupancy(D4);
DigitalIn passenger_occupancy(D5);
DigitalIn ignition_BTN(D3);

//Swiches
DigitalIn driver_seatbelt(D8);
DigitalIn passenger_seatbelt(D9);


DigitalOut good_passenger_LED(LED1);
DigitalOut ignition(LED2);
/*
Driver seatbelt sensor 
Passenger seatbelt sensor 
*/

//Alarm buzzer 
DigitalInOut alarm_buzzer(PE_10);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();

void car();
void print_to_uart(string);

void run();

//=====[Main function, the program entry point after power on or reset]========


int main()
{
    inputsInit();

    run();
}



void inputsInit()
{
    driver_occupancy.mode(PullDown);
    passenger_occupancy.mode(PullDown);
    // driver_seatbelt.mode(PullDown);
    // passenger_seatbelt.mode(PullDown);

    ignition_BTN.mode(PullDown);

    alarm_buzzer.mode(OpenDrain);
    alarm_buzzer.input();
}


void run(){
    //PART 1
    //while the button is not presseed wait till the button is pressed.
    while (driver_occupancy == OFF){ }

    //display the message, “Welcome to enhanced alarm system model 218-W24”.
    uartUsb.write( "Welcome to enhanced alarm system model 218-W24\r\n", 48 );

    //PART 2

    //Light the green LED only when both seats are occupied and both seatbelts are fastened.
    while (ignition_BTN == OFF){ 
        if ((driver_occupancy && passenger_occupancy) && (driver_seatbelt && passenger_seatbelt)){
            good_passenger_LED = ON;
        } else {
            good_passenger_LED = OFF;
        }
    }

    //PART 3

    if (good_passenger_LED = ON){

        //normal ignition occurs. 
        ignition = ON;

        //Light the blue LED and extinguish the green LED. 
        good_passenger_LED = OFF;

        //Display the message, “Engine started.”  
        uartUsb.write( "Engine started.\r\n", 15+2);

    } else {

        // Sound the alarm buzzer; 
        alarm_buzzer.output(); 

        // display the message, “Ignition inhibited” 
        uartUsb.write( "Ignition inhibited\r\n", 18+2);

        // and display all the reasons why the ignition was inhibited: 
        if (!driver_occupancy){// driver_occupancy
            uartUsb.write( "Driver seat not occupied\r\n", 19+6+2);

        } if (!passenger_occupancy){// pasenger_occupancy
            uartUsb.write( "Passenger seat not occupied\r\n", 19+9+2);

        } if (!driver_seatbelt){// driver_seatbelt
            uartUsb.write( "Driver seatbelt not fastened\r\n", 22+6+2);

        } if (!passenger_seatbelt){// passenger_seatbelt
            uartUsb.write( "Passenger seatbelt not fastened\r\n", 22+9+2);
        }
    }
}
