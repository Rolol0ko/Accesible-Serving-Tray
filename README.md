# Accessible Serving Tray
### GNG2101 – Switch-Adapted Serving Tray

An accessible, motorized serving tray designed to make moving items across a surface easier for individuals with limited mobility or difficulty reaching.

This project uses a push-button-controlled stepper motor mechanism to move a serving tray between two endpoints. The system is designed around simple operation, accessible controls, and safe, reliable movement.

## Project Overview

The Accessible Serving Tray is an assistive technology project developed for GNG2101. Its purpose is to provide a more accessible way for users to move items without having to manually reach across an entire table or serving surface.

A large push button controls the tray's movement. Each press moves the tray towards the opposite endpoint, allowing the user to alternate between two preset positions.

## Project Objectives

The project aims to:

1. Develop an accessible serving mechanism that reduces the need for manual reaching.
2. Implement simple, intuitive switch-based controls.
3. Design a reliable motorized tray movement system.
4. Incorporate endpoint detection and position referencing.
5. Reduce unnecessary motor heating through automatic de-energization.
6. Apply embedded programming and electromechanical design principles to an assistive technology application.

## Features

- **Accessible button control:** A large push button provides a simple input for operating the tray.
- **Automatic bidirectional movement:** Each button press moves the tray towards the opposite endpoint.
- **Limit switch detection:** Two limit switches identify the CLOSE and FAR positions.
- **Manual positioning:** The tray can be moved by hand while the motor is de-energized.
- **Automatic position zeroing:** The system establishes its reference position when an endpoint limit switch is detected.
- **Motor power management:** The motor is de-energized when stationary to reduce unnecessary holding current and motor heating.
- **Smooth movement:** AccelStepper provides controlled step timing, acceleration, and deceleration.
- **Button debouncing:** Software filtering helps prevent unintended multiple activations.

## Safety Considerations

- The motor is de-energized when stationary to reduce motor heating.
- The motor loses holding torque when de-energized. The tray should therefore be mechanically secure when the motor is disabled.
- Limit switches are used to detect the endpoints and stop normal travel.
- The A4988 current limit must be configured appropriately for the motor.
- The motor power supply should be disconnected before modifying electrical connections.
- The mechanism should be tested at low speed and without obstructions before normal use.

## Future Improvements
Potential areas for future development include:

- Adjustable movement speed.
- Improved mechanical stability and tray guidance.
- More comprehensive fault detection.
- Emergency stop functionality.
- Improved accessibility through alternative input methods.
- Mechanical safeguards to prevent unintended tray movement.

## Hardware Components

| Component | Purpose |
|---|---|
| Arduino Nano | Main system controller |
| NEMA 17 stepper motor | Drives the tray mechanism |
| A4988 stepper motor driver | Controls motor movement |
| Large push button | User movement input |
| Close & far limit switch | Detects the endpoints of travel |
| External power supply | Supplies power to the stepper motor and arduino|
| Mechanical tray assembly | Supports and guides tray movement |

## Software and Libraries

The project is programmed using the Arduino framework.

- [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/) – Controls stepper motor movement, speed, acceleration, and deceleration.
- Arduino IDE
- Arduino standard libraries (`Arduino.h`)

## System Operation

### 1. Startup and Manual Positioning

When the Arduino starts:

- The motor driver outputs are disabled.
- The stepper motor is de-energized.
- The user manually slides the tray.
- The system waits for either the CLOSE or FAR limit switch to activate.
- Once an endpoint is detected, the Arduino establishes a reference position.

### 2. Normal Operation

Once the tray's position has been established:

1. The user presses the large push button.
2. The motor driver is enabled.
3. The stepper motor begins moving the tray towards the opposite endpoint.
4. The Arduino continuously monitors the directional limit switches.
5. When the destination limit switch activates, the motor stops.
6. The motor driver outputs are disabled to remove holding current.

### 3. Reversing Direction

After reaching an endpoint, another button press moves the tray in the opposite direction.

This process repeats between CLOSE and FAR.

## Pin Configuration

The Arduino software uses the following pin assignments:

| Arduino Pin | Connection |
|---|---|
| D2 | Large push button |
| D3 | CLOSE limit switch |
| D4 | FAR limit switch |
| D5 | A4988 ENABLE |
| D8 | A4988 DIR |
| D9 | A4988 STEP |

The button/limit switches are configured using Arduino's internal pull-up resistors. The arduino expects the switches to read LOW when activated.

## Installation and Setup

### 1. Install Arduino IDE

Download and install the [Arduino IDE](https://www.arduino.cc/en/software).

### 2. Install AccelStepper

1. Open Arduino IDE.
2. Navigate to **Sketch → Include Library → Manage Libraries**.
3. Search for `AccelStepper`.
4. Install the AccelStepper library.

### 3. Open the Project Code

Open the Arduino source code from this repository.

### 4. Configure the Hardware

Connect the Arduino, A4988 driver, NEMA 17 motor, push button, and limit switches according to the pin configuration above.

Use an appropriate external motor power supply and configure the A4988 current limit according to the motor specifications.

### 5. Upload and Test

1. Connect the Arduino to your computer.
2. Select the appropriate Arduino board and COM port.
3. Upload the program.
4. Open the Serial Monitor at 9600 baud.
5. Manually position the tray until an endpoint is detected.
6. Press the large button to initiate movement.


## Motor Configuration

The current software uses the following initial AccelStepper settings:

| Parameter | Value |
|---|---:|
| Maximum speed | 200 steps/second |
| Acceleration | 150 steps/second² |
| Minimum pulse width | 5 microseconds |
| Button debounce | 50 milliseconds |
| Limit switch debounce | 10 milliseconds |

These values can be adjusted in the Arduino source code to accommodate the mechanical load and desired movement characteristics.