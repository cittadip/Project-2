## Automatic Headlight System:
By Adam Otsuka, Philip Cittadino

### Project Description:
The goal of the project was to create an automatic headlight system which contained all the safety features from Project 1. \
(Link to Project 1: https://github.com/4damo5/218-Project-1)

Project 2: \
Before the user can even access the automatic headlight system, the vehicle must be started under strict conditions.
These conditions include both the passenger and the driver being seated in the vehicle, with both their seatbelts fastened.
Under these conditions, the user can successfully start the vehicle, making the automatic headlight system possible.


The user can select from three headlight modes: ON, OFF, or AUTO. If ON is selected, both headlights will turn on. If OFF is selected, the headlights will turn off. If AUTO is selected, a light sensor will determine whether it is daylight or dusk.

If daylight is detected, the headlights will turn off after a 2-second delay. If dusk is detected, they will turn on after a 1-second delay. If the light sensor detects a level between these two thresholds, it will maintain its previous state.

Additionally, the project includes automatic high beams, with a switch to toggle between high-beam and low-beam modes. One illuminated LED indicates low-beam mode, while two illuminated LEDs indicate high-beam mode. The high beams follow the same behavior as the headlights described earlier.

### Project Results:
As mentioned previously, a headlight system was added to the smart ignition system to enhance vehicle safety. The driver and passenger must be seated with their seatbelts fastened in order to start the vehicle. If the user fails to meet these conditions, appropriate error messages will be displayed, and an alarm will sound. However, the user will be allowed to try again without needing to reset the system.

Once the engine has started, the user can activate the headlight system and choose between three modes: ON, OFF, or AUTO. Selecting ON will turn on both the headlights and the high beams. If OFF is selected, the headlights and high beams will turn off. If AUTO is selected, the system will use a photoresistor to determine whether the headlights and high beams should be turned ON or OFF.

Table 1 represents the results of our ignition system.

<img width="594" alt="Screenshot 2025-02-01 at 8 33 30 AM" src="https://github.com/user-attachments/assets/b437108f-8d49-446c-91c4-4b84abcd688d" />

Table 1. Ignition results.



Table 2 represents the results of our headlights and high-beam system.

<img width="586" alt="Screenshot 2025-02-01 at 8 34 06 AM" src="https://github.com/user-attachments/assets/86614a7d-6343-4a02-860e-e94446b57b7b" />

Table 2. Headlights and high-beam system results. 
