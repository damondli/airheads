

/*

OLD. SEE WORD DOCUMENT.

We probably do not need a PID or similar controller on the
elevators or rudder, since there is high friction. We can 
direct the motor to turn until it reaches the desired position.

The PID controller will apply to the angle of the plane.
The control output will be the position of the elevator or
rudder.

Because we are controlling both pitch and yaw, this is a 
two-dimensional control system. We will first assume that
the two are independent.

The Python code for ME 418 PI controller of one motor has

MOTOR MODULE
    @class MotorDriver
        Attributes: MotorA, MotorB (both of class Motor)
    @class Motor
        Methods: 
            Set voltage percent
            Enable
            Disable

ENCODER MODULE
    @class Encoder
        Methods:
            Update
            Get position
            Set position
            Zero
            Get velocity
            Counts to rad
            Get position rad
            Get velocity rad

CONTROLLER MODULE
    @class PIController
        Methods:
            Set gains
            Run (once)
            Get PI components



Editing this for our purposes, we have

DRV8871
    @class DRV8871
        Methods:
            Set duty
            Enable
            Disable
    Objects: MotorE, MotorR

POTENTIOMETER
    @class Pot
        Methods:
            Zero
            Get angle (returns rad)
            Set angle (receives rad)
    Objects: PotE, PotR

CONTROLLER
    @class Controller
        Methods:
            Set gains
            Calculate (once)
    Objects: ControllerE, ControllerR

IMU
    @class IMU
        Methods:
            Get angles
            Get angular velocities
    Objects: planeIMU

ULTRASONIC
    @class Ultrasonic
        Methods:
            Get distance
    Objects: planeUSS



*/