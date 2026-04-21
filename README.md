# Heaths Projects

A collection of projects completed for school and hobbies.

**Links:**
<br/>Youtube: https://www.youtube.com/@bubbacreates
<br/>Portfolio: https://sites.google.com/view/heath-marchese/

**Email:**
<br/>Email: marchese.heath@gmail.com

## Hobby Projects

### [Robot Arm, Apr. '26](../main/Robot_Arm)

* *Project type: Arduino IDE (.ino)*

This project implements a real-time ESP32-based robotic arm controller with multi-tasking architecture, combining servo control, stepper motor positioning, and live OLED visualization. It is designed as a modular embedded system for controlling a 5-DOF robotic arm using potentiometer-based manual input.

Built using FreeRTOS on ESP32, the system separates sensing, actuation, and display into concurrent tasks for smooth and responsive operation.

The system is driven by analog and digital inputs:
- Potentiometer Control – Reads multiple analog inputs to directly map user movement to joint angles.
- Servo Actuation – Controls elbow, swivel, and gripper joints using PWM-driven servo motors.
- Stepper Base Control – Implements a 4-step sequence driver for precise rotational base positioning.
- OLED Interface – Displays real-time joint angles and system status using the U8g2 graphics library.

The system architecture is organized into parallel tasks:
- Data Acquisition Task – Continuously reads potentiometers, maps values to joint angles, and handles save-state input.
- Motor Driver Task – Updates servo positions and steps the base motor toward target angles.
- Display Task – Renders live joint data and system status on a SH1106 OLED display.
- Boot Animation – Provides a startup sequence with a graphical loading bar.

Key features include:
- FreeRTOS-based multitasking for concurrent control loops
- Multi-joint servo coordination with smooth analog mapping
- Stepper motor micro-stepping logic for base rotation
- OLED real-time telemetry display with formatted output
- Modular hardware abstraction across servos, stepper, and sensors

This project demonstrates embedded robotics control, real-time multitasking on microcontrollers, and hardware-software integration for multi-degree-of-freedom motion systems.

It is currently a work in progress, and future development includes implementing inverse kinematics (IK) to allow end-effector position-based control, enabling the robotic arm to compute joint angles automatically rather than relying solely on manual potentiometer input

### [Mini Drill Press, Nov. '25](../main/Mini_Drill_Press)

* *Project type: Arduino IDE (.ino)*

This project implements a smart Arduino-based mini drill press controller featuring real-time motor control, temperature monitoring, automatic cooling, and an animated OLED interface for system feedback.

Built using embedded C++ and multiple hardware peripherals, it combines motor actuation, sensor input processing, and graphical display rendering into a unified control system.

The system is driven by analog sensing and timed control loops:
- Motor Control – Uses a potentiometer input to dynamically adjust motor speed via an L298N motor driver with PWM control.
- Temperature Monitoring – Reads a thermistor to track system temperature and determine cooling requirements.
- Automatic Cooling – Activates a fan via PWM when temperature exceeds a defined threshold.
- Interrupt-Based Fan Sensing – Measures fan RPM using hardware interrupts for real-time speed tracking.
- OLED Display Interface – Provides system status and animations using a SH1106 OLED and the U8g2 graphics library.

The interface includes two operational states:
- Home Mode – Displays system status, runtime, temperature readings, and idle state information.
- Drilling Mode – Shows live motor speed, temperature data, and an animated drill bit visualization during operation.

Key features include:
- Real-time PWM motor speed control via potentiometer input
- Closed-loop thermal response system with automatic fan activation
- Interrupt-driven fan RPM measurement
- Frame-based animation rendering on OLED display
- Non-blocking timing using millis()-based scheduling for smooth multitasking
- Modular display rendering using state-driven UI logic

This project demonstrates embedded systems design, real-time control theory, and hardware-software integration, combining motor control, thermal management, and graphical feedback into a compact electromechanical control system.

### [Hand-held Entertainment Hub, Oct. '25](../main/Handheld_Entertainment_Hub)

* *Project type: Arduino IDE (.ino)*

This project implements a portable multifunction gaming console built on an Arduino platform, featuring an OLED-based graphical interface and a button-driven menu system. Designed using the U8g2 graphics library, it combines multiple mini-games and utilities into a single interactive handheld device, including a dice roller, reaction timer, slot machine, Dino game, Magic 8-Ball, digital pet, and music player.

The system is structured around a hierarchical menu interface with real-time input handling and task scheduling for responsive gameplay and smooth display updates.
- Menu Navigation – Multi-level UI (Main, Games, Fun, Dice) controlled via physical buttons
- Dice Roller – Animated dice rolls with selectable polyhedral dice (D2–D20) and critical roll detection
- Mini-Games Suite – Includes memory sequence game, reaction timer, slot machine, and Dino runner
- Interactive Utilities – Magic 8-Ball responses, digital pet simulation, and simple tone-based music player
- Persistent Storage – EEPROM-based saving of high scores, reaction times, and user stats
- Real-Time Graphics – OLED rendering with custom bitmaps, animations, and dynamic UI updates
- Embedded Systems Design – Modular architecture combining input handling, game logic, audio, and display control

This project demonstrates embedded systems development, real-time UI design, game logic implementation, and efficient resource management on constrained hardware, blending entertainment and interaction into a unified portable device.

### [DnD DM Terminal, Sep. '24](../main/DnD_Terminal)

* *Project type: Arduino IDE (.ino)*

This project implements a portable digital dice roller for tabletop games using an Arduino, featuring a graphical user interface on an OLED display. Built with the U8g2 graphics library and GEM menu framework, it provides an interactive and visually engaging way to simulate dice rolls for games like Dungeons & Dragons.

The system is driven by a button-controlled menu interface:
- Menu Navigation – Users navigate between pages and select dice types (D2–D20) using physical buttons.
- Dice Selection – A structured menu allows dynamic selection of different dice sizes.
- Roll Animation – Simulates dice rolling through animated bitmap transitions before displaying the final result.
- Special Outcomes – Detects and highlights critical rolls (e.g., natural 1 or natural 20) with unique graphics.

Key features include:
- Real-time OLED graphics rendering with custom bitmap assets
- Event-driven menu navigation system using button inputs
- Pseudo-random dice generation with animated feedback
- Modular design separating UI, input handling, and rendering logic

This project demonstrates embedded UI design, real-time graphics handling, and user interaction on resource-constrained hardware, blending software structure with a game-focused application.

<br/>Demo: https://youtube.com/shorts/bn4hZjJ3evo

### [Digital Electronics Number Converter, Jul. '24](../main/Digital_Converter)

* *Project type: Virtual Studio Code (.c)*

This project implements a C-based number system converter and binary arithmetic toolkit featuring an interactive command-line interface. It supports conversions between decimal, binary, two’s complement, and IEEE 754 floating-point formats, along with binary addition and analysis utilities.

Built as a modular console application, it provides an educational and functional tool for exploring low-level numerical representations and computer arithmetic.

The system is driven by a menu-based interface:
- Menu Navigation – Users select between multiple conversion and computation modes via a command-line menu.
- Decimal Conversions – Converts decimal values into binary (including fractional representation) and two’s complement form.
- IEEE 754 Support – Encodes decimal values into IEEE 754 format and decodes IEEE 754 bit strings back to decimal.
- Binary Operations – Performs binary-to-decimal conversion and binary addition with carry handling.
- Fractional Handling – Supports fractional binary conversion and parsing using fixed-point logic.

Key features include:
- Full binary representation support (integer and fractional values)
- IEEE 754 single-precision floating-point encoding/decoding
- Two’s complement generation using bitwise inversion and carry addition
- Binary arithmetic with step-by-step carry propagation
- Modular function design separating conversion, arithmetic, and formatting logic

This project demonstrates core systems programming concepts, including numerical representation, bit-level manipulation, and floating-point arithmetic, while reinforcing how data is represented and processed at the machine level.

### [Trinket Box, Jul. '24](../main/Trinket_Box)

* *Project type: Arduino IDE (.ino)*

This project implements a portable Arduino-based multimedia “trinket box” featuring an interactive LCD menu system, audio playback from an SD card, and a built-in Chrome Dino–style mini game with persistent high score tracking using EEPROM.

Built using the Arduino ecosystem and multiple hardware modules, it combines user input, audio output, and real-time game logic into a unified embedded system experience.

The system is driven by a rotary encoder and button interface:
- Menu Navigation – Users scroll through and select options using a rotary encoder with interrupt-driven input handling.
- Audio Playback – Plays .WAV files stored on an SD card using the TMRpcm library with speaker output.
- LCD Interface – Displays menus, status screens, and game visuals on a 16x2 I2C LCD.
- EEPROM Persistence – Stores and retrieves the Dino game high score across power cycles.

The Dino game includes:
- Side-scrolling obstacle logic with randomized spacing
- Jump mechanics controlled via a single button input
- Collision detection between character and obstacles
- Score tracking with increasing difficulty over time
- Game over and retry screen with countdown prompt

Key features include:
- Interrupt-based rotary encoder input for responsive menu control
- SD card integration for external audio file playback
- Custom LCD character sprites for player and obstacles
- EEPROM-based non-volatile storage for high score retention
- Real-time game loop with timing, collision detection, and state management

This project demonstrates embedded systems integration, real-time interaction design, and multi-peripheral coordination, combining audio, input devices, persistent memory, and simple game logic on resource-constrained hardware.

### [Science Calculator, Nov. '20](../main/Science_Calculator)

* *Project type: Spyder (.py)*

This project implements a modular scientific calculator suite with a graphical user interface using Python’s Tkinter framework, integrating physics, chemistry, mathematics, and unit conversion tools into a single interactive desktop application.

Built around a multi-page GUI architecture, it provides a structured and extensible platform for solving a wide range of STEM-related problems.

The system is driven by a frame-based navigation interface:
- Menu Navigation – Users switch between subject areas (Chemistry, Physics, Mathematics, Conversions) using a centralized Tkinter frame controller.
- Chemistry Tools – Includes thermodynamics calculators such as reaction property analysis and ideal gas law solving with unit handling.
- Mathematics Tools – Provides quadratic equation solving and vector operations such as projection and addition.
- Physics & Conversions – Structured placeholders and modules for pressure, volume, and temperature conversions.
- Interactive Inputs – Uses dynamic entry fields, radio buttons, and validation logic to collect and process user data.

Key features include:
- Multi-frame Tkinter architecture for clean UI state management
- Integration with external computation modules (Calculator, Conversions)
- Dynamic form generation for chemical equations and vector inputs
- Unit selection and validation system for scientific calculations
- Modular design separating UI, computation logic, and validation workflows

This project demonstrates GUI development, modular software design, and scientific computation handling in Python, combining interactive user interfaces with mathematical and physical problem-solving tools in a unified desktop application.

## School Projects

### [Smart Parking Occupancy Tracker (SPOT) - Capstone Project, Fall '25](../main/Capstone_Project)

* *Project type: Arduino IDE (.ino)*

This project implements a distributed IoT parking and sensing system built on ESP32 Zigbee devices, combining embedded sensor acquisition, mesh networking, and cloud telemetry via Azure IoT Hub. The system is composed of a Zigbee Coordinator (gateway), Zigbee End Device (sensor node), and a secure authentication layer that dynamically generates SAS tokens for cloud communication.

The architecture enables real-time distance sensing using a VL53L0X time-of-flight sensor, Zigbee-based data aggregation, and cloud reporting without requiring device reflashing after power loss.
- Zigbee Coordinator (Gateway) – Aggregates sensor data from end devices and forwards telemetry to Azure IoT Hub via MQTT
- Zigbee End Device (Sensor Node) – Captures time-of-flight distance measurements and transmits them over Zigbee mesh networking
- Time-of-Flight Sensing – Uses VL53L0X sensor for real-time distance-to-object measurements (e.g., vehicle detection)
- Dynamic Cloud Authentication – Generates Azure SAS tokens at runtime using HMAC-SHA256, eliminating manual reconfiguration after reboot
- UART Bridge Communication – Transfers processed sensor values between coordinator and gateway layer
- FreeRTOS Tasking (End Device) – Concurrent tasks handle sensing, display updates, and button input without blocking execution
- OLED Diagnostics Interface – Displays system status, battery indicators, sensor readings, and device ID
- Adaptive Network Management – Automatically reopens Zigbee network if no end devices are detected

Key features include:
- Fully wireless Zigbee mesh sensor-to-gateway communication
- Secure, auto-refreshing Azure IoT authentication (no reflashing required)
- Real-time environmental sensing with calibrated distance-to-occupancy mapping
- Modular embedded design separating sensing, networking, UI, and cloud layers
- Fault-tolerant reconnection logic for both Zigbee and cloud connectivity

This project demonstrates embedded IoT system design, multi-protocol networking (Zigbee + MQTT), real-time sensor processing, and secure cloud integration using dynamic authentication on resource-constrained microcontrollers.

<br/>Demo: https://youtu.be/wk3OdglrdMY

### [ROS2 Color-Based Robotic Sorting - MXET 400 Project, Spring '25](../main/MXET_400_Project)

* *Project type: ROS2 (.py)*

This project implements a ROS 2–based robotic automation system that detects object color and performs autonomous pick-and-place operations using a UR3e robotic arm. The system integrates real-time sensing, decision-making, and motion planning to sort objects into designated locations.

The architecture is divided into multiple ROS 2 nodes:
- Color Detection Node – Interfaces with an I2C color sensor to read RGB values, classify object color (red, green, blue, or none), and publish results to ROS topics.
- Primary Robot Controller (R1) – Executes pick-and-place routines and complex motion sequences using MoveIt 2, including cube handling and stack manipulation via action-based control.
- Secondary Robot Controller (R2) – Subscribes to color and digital input data to trigger autonomous sorting, directing the robot to place objects into color-specific bins.

Key system capabilities include:
- Real-time sensor integration and color classification
- ROS 2 pub/sub communication for modular and scalable design
- MoveIt 2 motion planning for precise robotic arm control
- Service-based gripper control using digital I/O
- Event-driven automation based on external inputs and sensor feedback

The system demonstrates a full robotics pipeline—from perception to action—combining embedded sensing, middleware communication, and industrial robot control for intelligent material handling applications.

<br/>Demo: https://youtu.be/dgTApvBPEFw

### [Drone Pitch Control - ESET 462 Project, Spring '25](../main/ESET_462_Project)

* *Project type: Arduino IDE (.ino)*

This project implements a real-time control system on an Arduino-compatible platform using FreeRTOS to stabilize motion based on sensor feedback. It integrates an MPU6050 accelerometer/gyroscope with an L298N dual motor driver to demonstrate closed-loop control using a PID algorithm.

In addition to the embedded system design, the physical structure of the system was fully modeled and 3D printed, creating a complete mechatronic platform that combines mechanical design with real-time control.

The system is structured using multiple concurrent FreeRTOS tasks:
- Input Task – Reads user input from a potentiometer to set the desired angle.
- Gyro Task – Processes IMU data to calculate the system’s pitch angle.
- Control Task – Implements a PID controller to compute corrective action based on error between desired and actual angles.
- Motor Task – Adjusts motor speeds in real time to maintain stability.

Inter-task communication is handled using queues, ensuring efficient and synchronized data flow between sensing, control, and actuation layers.

Key features include:
- Real-time multitasking with FreeRTOS
- Sensor fusion and angle estimation from IMU data
- Closed-loop PID control for dynamic stabilization
- Differential motor control for precise motion correction
- Custom-designed and 3D printed mechanical system

This project highlights embedded systems concepts such as real-time scheduling, feedback control, and hardware interfacing, while also demonstrating the integration of mechanical design and rapid prototyping for robotics applications.

<br/>Demo: https://www.youtube.com/watch?v=3KDRZVBq2bk

### [Mobile Robotics Labwork - MXET 300 Lab Contributions, Fall '24](../main/MXET_300)

* *Project type: Virtual Studio Code (.py) and SCUTTLE Robotics Kit*

A collection of work done for my Mechantronics labs. The work includes labs that cover many topics on establishing a working mobile robot along with my course project.

### [Motorized Double Pendulum - MXET 375 Project, Summer '24](../main/MXET_375_Project)

* *Project type: Arduino IDE (.ino)*

This project implements an interactive motor-driven double pendulum system using an Arduino, combining real-time position tracking with user input and visual feedback. It integrates a rotary encoder, L298N motor driver, and I2C LCD display to monitor and control angular motion.

The system operates as follows:
- A push button initiates the motion sequence and system startup.
- A DC motor drives the pendulum through a predefined motion profile using timed forward and reverse movements.
- A rotary encoder tracks angular displacement, allowing real-time updates to the second joint position.
- A 16x2 LCD displays both joint angles in radians, while serial output logs time-stamped data for analysis.

Key features include:
- Real-time angle tracking using interrupt-driven encoder input
- Coordinated motor control for staged motion sequences
- Live visualization of system state via LCD and serial monitoring
- Custom formatting for clean numerical display and data logging

This project demonstrates practical applications of embedded systems in motion control, human-machine interaction, and dynamic system visualization, making it well-suited for robotics and control systems experimentation.

<br/>Demo: https://youtu.be/3PyOYEPMs_4

### [MSP432 'Repeat After Me' Game - ESET 349 Project, Summer '24](../main/ESET_349_Project)

* *Project type: Keil IDE (.s)*

This project is a fully low-level implementation of a memory-based “repeat the pattern” game written in ARM assembly for a microcontroller platform. It directly interfaces with GPIO, timers, LEDs, buttons, and a piezo buzzer without relying on high-level libraries.

The game logic presents an increasing sequence of LED patterns that the player must replicate using corresponding input buttons:
- Each round adds a new step to the pattern, increasing difficulty progressively
- LEDs and buzzer tones provide audiovisual feedback for each sequence step
- Player input is validated in real time, with timeouts and error detection
- Win and loss conditions trigger distinct LED animations and sound patterns

Key features include:
- Direct register-level GPIO control for LEDs and button inputs
- Timer-based delays and sound generation using Timer_A
- Efficient memory handling with pointer-based pattern traversal
- Fully modular assembly functions for delays, sound, and light sequences

This project demonstrates a deep understanding of embedded systems at the hardware level, including real-time interaction, peripheral configuration, and state-driven program flow entirely in assembly language.

### [MSP432 Multi-Function Embedded System - ESET 269 Project, Spring '24](../main/ESET_269_Project)

* *Project type: Keil IDE (.c)*

This is a menu-driven embedded application built on the MSP432 microcontroller that demonstrates core hardware interfacing concepts, including UART communication, GPIO control, and ADC-based temperature sensing.

Through a serial console interface, users can select between multiple features:
- RGB LED Control: Configure color combinations, blink duration, and cycle count using GPIO outputs and timer-based delays.
- Digital Input Monitoring: Read and report the state of onboard push buttons using GPIO inputs with pull-up resistors.
- Temperature Measurement: Capture internal temperature sensor data via the ADC and convert it to both Celsius and Fahrenheit.

The system uses low-level register manipulation for peripherals such as Timer32, SysTick, UART (EUSCI_A0), and ADC14, providing hands-on experience with embedded systems programming and real-time hardware interaction.

### [Discrete Analog Piano - ESET 350 Project, Spring '24](../main/ESET_350_Project)

* *Project type: Analog Circuit*

This project presents the design and implementation of a fully analog square-wave synthesizer capable of generating musical notes using fundamental analog electronics principles. Built around an op-amp relaxation oscillator, the system produces a range of frequencies corresponding to piano keys, creating an interactive and hardware-based music generator.

The synthesizer operates through several key stages:
- Oscillator Stage – A Schmitt trigger–based relaxation oscillator generates square waves, with frequency determined by an RC time constant and selectable resistor network.
- Input Selection – Individual “keys” switch between resistor values to produce different musical notes across a defined frequency range.
- Buffer Stage – A voltage follower isolates the oscillator to prevent loading effects and maintain signal integrity.
- Amplification Stage – A class AB amplifier boosts the signal to drive a speaker, with a potentiometer controlling output volume.

Key features include:
- Continuous analog signal generation without digital processing
- Frequency tuning based on calculated resistor values for musical notes
- Hardware-based user interaction through button inputs
- Experimental validation using waveform analysis and frequency measurement

Testing confirmed that the system produces stable square waves across multiple frequencies with generally low error relative to ideal note values, demonstrating effective oscillator design and analog signal conditioning.

This project highlights core concepts in analog electronics, including oscillators, feedback systems, amplification, and real-world signal analysis, while bridging engineering design with musical application.

<br/>Demo: https://youtube.com/shorts/6o9ZUImQ6eg

## Other

### 3D Printing

* *Project type: 3D Modeling (.stl)*

I like to 3D print things and sometimes I remember to get pictures.

<br/>3D Printing Images: https://www.dropbox.com/scl/fo/oq1wqhtzj5fmjfgn7iqhv/ALXrbN7vOnTL8d6Vaw_4Xzw?rlkey=fd2yv3mldlqyzqb6exmruhfus&st=xppggboq&dl=0

### Music

* *Project type: Music (.mp3)*

I will sometimes *try* to make music for fun. You are welcome to listen to it anytime.

<br/>Youtube: https://www.youtube.com/@bubbacreates