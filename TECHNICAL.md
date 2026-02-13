# Omnidrive Technical Documentation

## Detailed Technical Explanation

This document provides an in-depth technical explanation of how the omnidrive system works, including the mathematics and physics behind it.

## Omnidrive Kinematics

### Coordinate System

We use a robot-centric coordinate system:
- **X-axis**: Points to the right of the robot
- **Y-axis**: Points forward from the robot
- **θ (theta)**: Rotation angle (positive = counterclockwise)

### Wheel Velocity Calculations

For an omnidrive robot with 4 wheels in a square configuration, each wheel's velocity is a combination of three components:

1. **Translation Velocity** (Vx, Vy): Linear movement in X and Y directions
2. **Rotational Velocity** (ω): Angular velocity around the robot's center

### Derivation of Motor Formulas

Consider the robot geometry:
```
        Y (forward)
        ↑
        |
        |        FL (+,+)    FR (+,-)
        |         *            *
        |         |            |
        |         |            |
        |         *            *
        |        BL (-,+)    BR (-,-)
        |
        └─────────────────→ X (right)
```

Each wheel's position relative to the robot center:
- FL: ( L/2,  W/2)
- FR: ( L/2, -W/2)
- BL: (-L/2,  W/2)
- BR: (-L/2, -W/2)

Where L = length between front/back, W = width between left/right

### Velocity Components

For each wheel, the total velocity is:
```
V_wheel = V_translation + V_rotation_tangential
```

#### Translation Component
All wheels contribute equally to forward/backward (Y) motion.
Strafing (X) causes left wheels and right wheels to rotate in opposite directions.

#### Rotation Component  
Rotation creates tangential velocities perpendicular to the radius from center to wheel.
For square configuration, this adds/subtracts equally from diagonal pairs.

### Final Formulas

Combining these components, we get:
```
V_FL = Vy + Vx + ω×R
V_FR = Vy - Vx - ω×R
V_BL = Vy - Vx + ω×R
V_BR = Vy + Vx - ω×R
```

Where:
- Vy = forward velocity
- Vx = strafe velocity
- ω = rotational velocity
- R = distance from center to wheel (effectively cancels out in normalized control)

In our implementation:
- `forward` parameter = Vy
- `strafe` parameter = Vx
- `rotate` parameter = ω

## Movement Examples

### Example 1: Forward Movement
```
Input: forward=100, strafe=0, rotate=0

V_FL = 100 + 0 + 0 = 100
V_FR = 100 - 0 - 0 = 100
V_BL = 100 - 0 + 0 = 100
V_BR = 100 + 0 - 0 = 100

Result: All wheels spin forward → Robot moves forward
```

### Example 2: Strafe Right
```
Input: forward=0, strafe=50, rotate=0

V_FL = 0 + 50 + 0 = 50
V_FR = 0 - 50 - 0 = -50
V_BL = 0 - 50 + 0 = -50
V_BR = 0 + 50 - 0 = 50

Result: Left wheels forward, right wheels backward → Robot slides right
```

### Example 3: Rotate Clockwise
```
Input: forward=0, strafe=0, rotate=40

V_FL = 0 + 0 + 40 = 40
V_FR = 0 - 0 - 40 = -40
V_BL = 0 - 0 + 40 = 40
V_BR = 0 + 0 - 40 = -40

Result: Left wheels forward, right wheels backward → Robot rotates clockwise
```

### Example 4: Diagonal Movement (Forward-Right)
```
Input: forward=70, strafe=70, rotate=0

V_FL = 70 + 70 + 0 = 140
V_FR = 70 - 70 - 0 = 0
V_BL = 70 - 70 + 0 = 0
V_BR = 70 + 70 - 0 = 140

After scaling (max > 100):
V_FL = (140 × 100) / 140 = 100
V_FR = (0 × 100) / 140 = 0
V_BL = (0 × 100) / 140 = 0
V_BR = (140 × 100) / 140 = 100

Result: FL and BR spin at 100% → Robot moves diagonally forward-right at 45°
```

### Example 5: Combined Movement (Forward + Strafe + Rotate)
```
Input: forward=50, strafe=30, rotate=20

V_FL = 50 + 30 + 20 = 100
V_FR = 50 - 30 - 20 = 0
V_BL = 50 - 30 + 20 = 40
V_BR = 50 + 30 - 20 = 60

Result: Complex movement - forward-right while rotating
```

## Speed Limiting Algorithm

When combining movements, motor speeds can exceed the maximum (-100 to 100 range). The algorithm handles this:

```cpp
// Find maximum absolute speed
maxSpeed = max(|V_FL|, |V_FR|, |V_BL|, |V_BR|)

// If any motor exceeds limit, scale all proportionally
if (maxSpeed > 100) {
    V_FL = (V_FL × 100) / maxSpeed
    V_FR = (V_FR × 100) / maxSpeed
    V_BL = (V_BL × 100) / maxSpeed
    V_BR = (V_BR × 100) / maxSpeed
}
```

**Why proportional scaling?**
- Maintains the ratio between wheel speeds
- Preserves the intended direction of movement
- Prevents motor damage from over-voltage
- Ensures predictable robot behavior

## Omniwheel Physics

### How Omniwheels Work

Standard omniwheels have:
- **Main rollers**: Large central wheel that provides primary drive
- **Perpendicular rollers**: Small rollers at 90° that allow passive sliding

```
        ___
       /   \    ← Main roller (powered)
    --*-----*--  ← Perpendicular rollers (passive)
       \___/
```

When the motor spins:
- Power is transmitted through the main roller
- The robot can slide perpendicular to the main direction via the small rollers
- This allows simultaneous movement in two directions

### Force Resolution

For an omnidrive to work:
1. Each wheel generates a force vector in its drive direction
2. Perpendicular forces are passively absorbed by roller sliding
3. Net force vectors from all 4 wheels combine to create desired motion
4. Proper motor speed ratios ensure forces sum to the intended direction

## Advanced Considerations

### Field-Centric vs Robot-Centric

This implementation uses **robot-centric** control:
- Forward/strafe relative to robot's current orientation
- Simpler to implement and understand
- Intuitive for drivers

**Field-centric** control (advanced):
- Uses gyroscope to maintain absolute directions
- Forward always moves toward same field direction regardless of robot rotation
- Requires additional sensors and coordinate transformations

### Encoder Feedback

For autonomous precision, add encoders:
- Measure actual wheel rotation
- Calculate real robot position using odometry
- Implement PID control for accurate movements
- Compensate for wheel slip

### PID Control

Add PID loops for:
- **Position control**: Move exact distances
- **Heading control**: Maintain or reach specific angles
- **Velocity control**: Achieve consistent speeds despite load changes

## Troubleshooting

### Robot Doesn't Move Straight
**Causes:**
- Motors not properly reversed in configuration
- Wheels not aligned at 90° angles
- Motor speeds not calibrated equally

**Solutions:**
- Toggle motor reversal: `motor(PORT, ratio, !current_reversal)`
- Physically check wheel alignment
- Add per-motor calibration multipliers

### Robot Rotates When Trying to Strafe
**Causes:**
- Incorrect motor reversal settings
- Wheels installed at wrong angles

**Solutions:**
- Verify each motor's reversal setting matches physical orientation
- Check formula signs match your wheel configuration

### Jerky or Unresponsive Movement
**Causes:**
- No deadband on controller inputs
- Battery voltage low
- Motor friction/binding

**Solutions:**
- Increase deadband threshold (currently 10)
- Charge battery fully
- Check mechanical assembly for friction

### Movement Direction is Wrong
**Causes:**
- Motor ports connected differently than code expects
- Controller axes mapped incorrectly

**Solutions:**
- Update port numbers in `robot-config.cpp`
- Adjust axis mappings in `main.cpp` usercontrol function

## Performance Optimization

### Reducing Latency
```cpp
wait(20, msec);  // 50Hz update rate (default)
wait(10, msec);  // 100Hz update rate (more responsive)
```

Lower values = more responsive, but higher CPU usage.

### Velocity Ramping
Add acceleration limiting to prevent wheel slip:
```cpp
int targetSpeed = calculated_speed;
int currentSpeed = motor.velocity(percent);
int maxAccel = 10;  // Max change per cycle

if (targetSpeed > currentSpeed + maxAccel) {
    targetSpeed = currentSpeed + maxAccel;
} else if (targetSpeed < currentSpeed - maxAccel) {
    targetSpeed = currentSpeed - maxAccel;
}
```

### Current Limiting
Protect motors during high torque situations:
```cpp
motor.setMaxTorque(100, percent);  // Limit to 100%
motor.setVelocity(speed, percent);
```

## References

- VEX Robotics Official Documentation
- "Modern Robotics: Mechanics, Planning, and Control" by Lynch & Park
- VEX Forum discussions on omnidrive kinematics
- "Introduction to Autonomous Robots" by Correll et al.

---

*This technical documentation is part of the VexV5-OmniDrive repository.*
