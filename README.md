#  ME507 Airheads Glider Project

<div class="title_screenshot">
<p align="center">
<img src="img/Glider.png">
</p>
</div>

This repository documents the development of a flight stabilization system for a glider.

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

<p align="center">
![Custom Motor Bracket](img/Mount.png)
</p>

## Custom PCB Design

Our custom PCB is designed to fit on an ESP32 feather board.

![Custom PCB](img/PCB_Board.png)

The board supports:
- Two DRV8871 Motor Driver Chips
- Two [Potentiometers](/class_potentiometer.html)
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

Our program uses cooperative multitasking with FreeRTOS to "simultaneously" run every task.

## Internet of Things (IoT)

To implement the IoT, the ESP32's Wi-Fi module is used to host webpage where the user can interact with the flight control system.

![Webpage Control Panel](img/Webpage.png)

## Documentation

For full documentation of our code, please visit [here](https://damondli.github.io/).
