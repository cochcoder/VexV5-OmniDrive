# VEX V5 Omnidrive Code

A comprehensive repository providing VEX V5 Omnidrive robot control code with detailed explanations and examples.

## Table of Contents
- [What is an Omnidrive?](#what-is-an-omnidrive)
- [How Omnidrive Works](#how-omnidrive-works)
- [Project Structure](#project-structure)
- [Hardware Setup](#hardware-setup)
- [Installation](#installation)
- [Usage](#usage)
- [Code Explanation](#code-explanation)
- [Controller Mapping](#controller-mapping)
- [Mathematical Explanation](#mathematical-explanation)
- [Customization](#customization)
- [License](#license)

## What is an Omnidrive?

An **omnidrive** (also called holonomic drive) is a robot drivetrain that uses special **omniwheels** or **mecanum wheels** to enable movement in any direction without changing the robot's orientation. Unlike traditional drivetrains that must turn to change direction, an omnidrive can:

- Move forward and backward
- Strafe (move sideways) left and right
- Rotate in place
- Combine all three movements simultaneously

This provides superior maneuverability, making it ideal for VEX robotics competitions where precise positioning is crucial.

## How Omnidrive Works

### Wheel Configuration

An omnidrive typically uses 4 omniwheels arranged in a square pattern:

```
        FRONT
     FL _____ FR
       |     |
       |     |
       |_____|
     BL       BR
```

- **FL** = Front Left motor
- **FR** = Front Right motor  
- **BL** = Back Left motor
- **BR** = Back Right motor

### Movement Mechanics

Each omniwheel has small rollers at an angle, allowing it to roll in its primary direction while sliding in the perpendicular direction. By controlling the speed and direction of each motor independently, we can achieve:

1. **Forward/Backward**: All wheels spin in the same direction
2. **Strafing**: Left and right sides spin in opposite directions
3. **Rotation**: Front and back pairs spin in opposite directions
4. **Diagonal/Combined**: Mathematical combination of the above

## Project Structure

```
VexV5-OmniDrive/
├── src/
│   ├── main.cpp              # Main program entry point
│   └── robot-config.cpp      # Robot configuration and omnidrive function
├── include/
│   ├── vex.h                 # VEX library header
│   └── robot-config.h        # Robot configuration declarations
├── makefile                  # Build configuration
├── vex-project.json          # VEX project metadata
├── LICENSE                   # AGPL-3.0 license
└── README.md                 # This file
```

## Hardware Setup

### Required Components

1. **VEX V5 Brain** - Robot controller
2. **VEX V5 Controller** - For user input
3. **4x VEX V5 Motors** - One for each wheel
4. **4x Omniwheels** - Special wheels with perpendicular rollers
5. **Motor cables** - To connect motors to brain

### Wiring Configuration

Connect motors to the V5 Brain ports as follows:

- **Port 1**: Front Left motor
- **Port 2**: Front Right motor  
- **Port 3**: Back Left motor
- **Port 4**: Back Right motor

> **Note**: The Front Right and Back Right motors are configured with `reverse: true` in the code to account for their physical orientation.

## Installation

### Prerequisites

- VEXcode V5 (or compatible IDE)
- VEX V5 Robot Brain with latest firmware
- USB cable for downloading to robot

### Steps

1. **Clone or download this repository**
   ```bash
   git clone https://github.com/cochcoder/VexV5-OmniDrive.git
   ```

2. **Open the project**
   - Open VEXcode V5
   - Select "Open" and navigate to this project folder
   - Or use File → Open Project and select `vex-project.json`

3. **Configure your robot**
   - Verify motor ports match your physical setup
   - Adjust motor reversals in `robot-config.cpp` if needed

4. **Build and download**
   - Click the "Build" button to compile
   - Connect your V5 Brain via USB
   - Click "Download" to upload the program

## Usage

### Driver Control Mode

Once the program is running on your robot:

1. **Left Joystick**:
   - **Y-axis** (up/down): Forward and backward movement
   - **X-axis** (left/right): Strafing (sideways movement)

2. **Right Joystick**:
   - **X-axis** (left/right): Robot rotation

3. **Combined Movement**: Move both joysticks simultaneously for complex movements (e.g., moving diagonally while rotating)

### Autonomous Mode

The code includes a simple autonomous routine that demonstrates:
- Moving forward for 2 seconds
- Strafing right for 2 seconds  
- Rotating for 1 second

You can customize this in the `autonomous()` function in `main.cpp`.

## Code Explanation

### The Omnidrive Function

The heart of this project is the `omnidrive()` function in `robot-config.cpp`:

```cpp
void omnidrive(int forward, int strafe, int rotate)
```

**Parameters:**
- `forward`: Forward/backward movement (-100 to 100)
- `strafe`: Left/right strafing (-100 to 100)  
- `rotate`: Rotation speed (-100 to 100)

### How It Works

The function calculates individual motor speeds using these formulas:

```cpp
frontLeftSpeed  = forward + strafe + rotate
frontRightSpeed = forward - strafe - rotate
backLeftSpeed   = forward - strafe + rotate
backRightSpeed  = forward + strafe - rotate
```

### Why These Formulas?

Let's break down what each component does:

1. **Forward Component**: Adding `forward` to all motors makes them all spin the same direction, moving the robot forward or backward.

2. **Strafe Component**: 
   - Adding `strafe` to left motors and subtracting from right motors (or vice versa) creates opposing forces
   - This causes the robot to slide sideways due to the omniwheel rollers

3. **Rotate Component**:
   - Adding `rotate` to left motors and subtracting from right motors creates a rotation
   - Front-left and back-left spin one direction, front-right and back-right spin the opposite

### Speed Limiting

The function includes automatic speed limiting:

```cpp
if (maxSpeed > 100) {
    // Scale all speeds proportionally
    frontLeftSpeed = (frontLeftSpeed * 100) / maxSpeed;
    // ... same for other motors
}
```

This ensures that when combining movements (e.g., moving forward while strafing), the motor speeds never exceed 100%, while maintaining the correct ratio between motors to preserve the direction of movement.

## Controller Mapping

### VEX Controller Layout

```
       L-Joystick              R-Joystick
          ↑ Axis3                 ↑
    Axis4 ← + → Axis4       Axis2 ← + → Axis1
          ↓                       ↓
```

### Default Mapping

- **Axis 3** (Left Y): Forward/Backward
- **Axis 4** (Left X): Strafe Left/Right
- **Axis 1** (Right X): Rotate Left/Right

### Deadband

The code includes a 10-unit deadband on all joysticks:

```cpp
if (abs(forward) < 10) forward = 0;
```

This prevents unintended movement from joystick drift or minor imperfections.

## Mathematical Explanation

### Vector Composition

Omnidrive movement is based on **vector addition**. Each desired movement creates a velocity vector for each wheel:

For a wheel at position (x, y) from the robot's center:
- Forward velocity contributes equally to all wheels
- Strafe velocity depends on x-position (left vs right)  
- Rotation velocity depends on perpendicular distance from center

### Simplified 4-Wheel Formula

For a square omnidrive configuration, the formulas simplify to:

```
V_FL = V_forward + V_strafe + V_rotate
V_FR = V_forward - V_strafe - V_rotate
V_BL = V_forward - V_strafe + V_rotate
V_BR = V_forward + V_strafe - V_rotate
```

Where:
- V = velocity
- FL/FR/BL/BR = Front-Left, Front-Right, Back-Left, Back-Right

### Why Opposite Signs?

- **Strafe**: Left and right sides must spin opposite directions to create sideways motion
- **Rotate**: Diagonal pairs must spin opposite to create rotation without translation

## Customization

### Adjusting Motor Ports

Edit `robot-config.cpp` to change motor port assignments:

```cpp
motor FrontLeft = motor(PORT1, ratio18_1, false);
// Change PORT1 to your desired port
```

### Changing Gear Ratios

Modify the gear ratio if using different motor cartridges:

```cpp
motor FrontLeft = motor(PORT1, ratio18_1, false);
//                              ^^^^^^^^
// Options: ratio18_1, ratio36_1, ratio6_1
```

### Reversing Motors

If your motors spin the wrong direction, toggle the boolean parameter:

```cpp
motor FrontLeft = motor(PORT1, ratio18_1, false);
//                                         ^^^^^
// Change false to true, or true to false
```

### Adjusting Speed Scaling

For slower, more precise control, scale the inputs in the `usercontrol()` function:

```cpp
// Reduce to 75% speed
forward = forward * 0.75;
strafe = strafe * 0.75;
rotate = rotate * 0.75;
```

### Custom Controller Mapping

Remap controls by changing which axis is read:

```cpp
int forward = Controller1.Axis2.position();  // Use Axis2 instead
int strafe = Controller1.Axis1.position();   // Use Axis1 instead
// etc...
```

## License

This project is licensed under the GNU Affero General Public License v3.0 (AGPL-3.0) - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Feel free to:
- Report bugs
- Suggest enhancements  
- Submit pull requests

## Support

For questions or issues:
- Open an issue on GitHub
- Consult VEX Robotics forums
- Check VEXcode documentation

---

**Happy Building! 🤖**