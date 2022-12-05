#  ME507 Airhead's Glider Project

<div class="title_screenshot">

![Screenshot of Doxygen Awesome CSS](img/screenshot.png)

</div>

This repository documents the development of a flight stabilization system for a glider.

## Background

This is a term project for Cal Poly's ME507 class. The scope of our project consists of:

- Background/Research and Identifying Specifications
- Hardware Design
- Custom PCB Design
- Software Design
- IoT Implementation
- Proper Documentation 


## Hardware Design

To make this project more challenging and more fitting for ME507, we decided to use micro servo body DC motors. To implement some form of feedback for the position of the motors, a motor mount was designed to couple a potentiometer to the shaft of the motor.

## Custom PCB Design

Our custom PCB is designed to fit on an ESP32 feather board.

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

Our program uses cooperative multitasking with FreeRTOS to "simultaneously" run every task. For full documentation of our code, please visit [here](https://damondli.github.io/).
