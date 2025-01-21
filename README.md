# iCub Eye Tracking System

A real-time gaze control system for the iCub humanoid robot that tracks red objects using coordinated eye-head movements.

## Author
**Mohamed Nasser Gaafer**  
Robotics Programming Frameworks and IoT  
Scuola Superiore Sant'Anna, Pisa  
Course Instructors: Egidio Falotico, Ugo Albanese

## Overview

This system enables the iCub robot to track moving red objects by:
- Processing camera input to detect red objects
- Controlling eye movements using PID control
- Coordinating head movements based on eye position
- Providing real-time visualization of tracking performance

## Control System

### Eye Control
Uses PID control for precise positioning:

$$
\begin{aligned}
\Delta\theta(t) &= K_p e(t) + K_i \int e(t)dt + K_d \frac{de(t)}{dt} \\
\text{where: } K_p &= 0.2, \quad K_i = 0.01, \quad K_d = 0.05
\end{aligned}
$$

- Error calculated in image coordinates (pixels)
- Anti-windup limit: ±10.0
- Position threshold: 0.2°
- Error threshold: 1.0 pixels

### Head Control
Simple proportional control based on eye position:

$$
\begin{aligned}
v_{neck}(t) &= -K_{head} \theta_{eye}(t) \\
\text{where: } K_{head} &= 1.2
\end{aligned}
$$

- Compensatory movement opposite to eye direction
- Activates when eyes deviate >0.1° from center
- Separate control for pitch and yaw axes

## Setup

### Prerequisites
- Ubuntu 22.04
- YARP 3.7
- iCub Main 1.25.0
- Qt5 for visualization

### Installation
1. Clone repository
2. Build with CMake
3. Configure YARP network

### Usage
1. Start YARP server: `yarpserver`
2. Launch simulator: `iCub_SIM`
3. Run tracking: `./gaze_control`
4. Stop system: `./shutdown.sh`

## Implementation
- Real-time image processing at 50Hz
- Modular design with separate threads for control and visualization
- Qt-based plotting for performance monitoring

## Results

### Visual Tracking Performance
The system demonstrates:
- Stable tracking of red objects in real-time
- Natural eye-head coordination
- Smooth pursuit behavior with head following eyes
- Real-time performance visualization

![Tracking Demo](docs/screenshots/preview_compressed.gif)

### Performance Analysis
Real-time visualization of tracking errors and motor positions:

![Error Plot](docs/screenshots/error_plot.png)
![Position Plot](docs/screenshots/position_plot.png)

