# CPE301-Final-Project
Full Documentation

https://docs.google.com/document/d/1xEC2ZcBTXbJL_ikaYuqPgeiJFZhcmx397U9M6ES3sX8/edit?usp=sharing

1.Introduction

The final project for CPE 301 aims to create and design a evaporation cooling system or swamp cooler. It will use Arduino code in tantum with the MEGA Arduino Controller Board. The system uses a DHT11 Temperature and Humidity Sensor, Water Level Detection Sensor Module, an liquid crystal display module (LCD), a fan with its 3-6V Motor, one button, and four LEDs of the colors red, blue, green,yellow. The cooler system works by monitoring the water level and temperature of the system.

2.Objective

The objective of this project is to create a swamp cooler that switches between states. The system would print an error message when the water level is too low and needs to be refilled. The temperature sensor would record and display the temperature and humidity on the LCD screen. The system would go through multiple different stages of disabled, idle, error and running. The system should start in the disabled state where a yellow LED would be lit up and the start button should be monitored. The idle state should have the green LED be on and the temperature and humidity should be continuously monitored. The error state happens when the water level in the system is too low and would have the red LED lit with the fan unable to be used. A message would also be displayed on the LCD screen stating that there is an error and water needs to be refilled. Finally the running state should happen when the temperature is beyond the threshold. The fan and the blue LED should be on in this state. The state changes once the numbers of the program are at a normal state.

3.Design

This project was designed initially through the process of brainstorming the code. The inital code was made to have a basis of what the project might look like. Afterwards, we met up and made the inital design of the circuit board to test the code and see what works and does not work in the code as well as the circuit. For the design of the circuit, we started off grabbing each of the important component that is needed for this project, which were the water level detector sensor, the temperature and humidity censor, the LCD screen, the fan and stepper motor along side the other smaller components such as LEDs, resistors, and wires. We first made inital connections with the board and components. After some testing on what worked we did some revisions with both the circuity and code. After more testing and tinkering, we ended with the final build of the project. After, we made all the documents and gathered the information on the other documents that were helpful and used in this project.

Relevant Libraries

https://www.arduino.cc/reference/en/libraries/dht-sensor-library/ 

https://github.com/adafruit/DHT-sensor-library 

https://github.com/hasenradball/DS3231-RTC#the-special-rtclibnow-function- 

