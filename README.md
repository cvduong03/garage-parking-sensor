# Garage Parking Sensor

My parents' garage is small and requires most cars to be parked within 1 foot of the wall to comfortably close the garage door. 
The Garage Parking Sensor is a simple device designed to help my parents park their brand new Lexus in the garage. 

The device consists of an HC-SR04 ultrasonic sensor to measure distance, an addressable WS2812 RGB LED Strip to act as a visual aid, and an ESP32 microcontroller to connect the components and handle the logic. It is buildable on a breadboard with some jumper wires.

## Functionality
As the car approaches the ultrasonic sensor placed at the wall, the measurements are fed into a range of conditional statements which are reflected by the colors below:
- More than 6 feet: Solid green (distant/clear to go)
- Less than 6 feet: Solid yelow (approaching/proceed with caution)
- Less than 3 feet: Solid orange (near/prepare to stop)
- Less than 1 feet: Solid red (close/stop)
- Less than 6 inches: Flash red (too close/stop immediately)

The LED strip will display the appropriate color and turn off after 10 seconds of no change in distance.

### Notes

The ultrasonic sensor used may struggle on rounded or irregular shaped objects at greater distances. I recommend trying different or better distance measuring sensors as the logic should be easily adaptable to work with them.
