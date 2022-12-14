#  ME507 Airheads Glider Project

<div class="title_screenshot">

![Figure 1. Glider](img/Glider.png)

</div>

This repository documents the development of a flight stabilization system for a glider.
To access our written report for this project, click [here](Airheads_Term_Project_Report_ME507.pdf).

## Background

This is a term project for Cal Poly's ME507 class. The scope of our project consists of:

- Background/Research and Identifying Specifications
- Hardware Design
- Custom PCB Design
- Software Design
- Internet of Things Implementation
- Proper Documentation 


## Hardware Design

To make this project more challenging and more fitting for ME507, we decided to use micro servo body DC motors instead of servos. To implement some form of feedback for the position of the motors, a motor mount was designed to couple a potentiometer to the shaft of the motor.

![Figure 2. Custom Motor Bracket](img/Mount.png)

## Custom PCB Design

Our custom PCB is designed to fit on an ESP32 feather board.

![Figure 3. Custom PCB Design](img/PCB_Board.png)

The board supports:
- Two DRV8871 Motor Driver Chips
- Two Potentiometers
- One HC-SR04 Ultrasonic Sensor
- One LIS3MDL + LSM6DSOX IMU Breakout Board

## Software Design

The software includes the following tasks:
- PID Flight Controller
- Ultrasonic Sensor
- Elevator Motor
- Rudder Motor
- Webserver
- IMU

The tasks are organized as shown below in the task diagram.

![Figure 4. Task Diagram](img/Task.png)

Our controller task is structured as a finite state machine with three states.

![Figure 5. Finite State Machine](img/FSM.png)

Every task runs "simultaneously" though cooperative multitasking with FreeRTOS.

## Internet of Things (IoT)

To implement the IoT, the ESP32's Wi-Fi module is used to host webpage where the user can interact with the flight control system.

![Figure 6. Webpage Control Panel](img/Webpage.png)

Through the webpage, we have the capability to activate, deactivate, and calibrate the flight control system. With more time, we would like to enable our program to support GET or POST requests to accept the user's typed input. This way we can seamlessly update our PID gains without recompiling and uploading our program for each iteration. 

## Documentation and Report

For full documentation of our code, please visit [here](https://damondli.github.io/airheads/).
Our written report for this project can be accessed [here](Airheads_Term_Project_Report_ME507.pdf).
