# VEX V5 OmniDrive

This project demonstrates how to program a **four-motor omnidirectional drivetrain** for a VEX V5 robot using C++ and the VEXcode API.

Unlike a normal tank or arcade drivetrain, an omnidirectional drivetrain can move:

* Forward and backward
* Left and right
* Diagonally
* Rotate clockwise or counterclockwise
* Translate and rotate at the same time

The drivetrain is controlled using three joystick axes on `Controller1`.

> **Note:** The exact motor signs and directions in this README depend on the physical orientation of the motors and omni wheels. A drivetrain built differently may require some motor directions to be reversed.

---

## Contents

* [Drivetrain Layout](#drivetrain-layout)
* [Controller Layout](#controller-layout)
* [Omnidrive Motor Math](#omnidrive-motor-math)
* [Understanding the Math](#understanding-the-math)
* [Forward Movement](#forward-movement)
* [Strafing](#strafing)
* [Rotation](#rotation)
* [Combining Movements](#combining-movements)
* [Speed Limiting](#speed-limiting)
* [Deadband](#deadband)
* [Motor Stop Flags](#motor-stop-flags)
* [Shoulder Button Controls](#shoulder-button-controls)
* [Controller Loop](#controller-loop)
* [Original Example Code](#original-example-code)
* [Recommended Code Structure](#recommended-code-structure)
* [Untested Speed Normalization](#untested-speed-normalization)
* [Possible Six-Wheel Implementation](#possible-six-wheel-implementation)
* [Testing the Drive](#testing-the-drive)
* [Possible Improvements](#possible-improvements)
* [Quick Reference](#quick-reference)

---

# Drivetrain Layout

The original code assumes four independently controlled drive motors.

A simplified layout looks like this:

```text
              FRONT

         FL           FR
           \         /
            \       /
             ROBOT
            /       \
           /         \
         BL           BR

               BACK
```

The drive motor names are:

| Position    | Variable |
| ----------- | -------- |
| Front Left  | `FL`     |
| Front Right | `FR`     |
| Back Left   | `BL`     |
| Back Right  | `BR`     |

The robot also has three mechanism motors:

| Device           | Variable |
| ---------------- | -------- |
| Conveyor 1       | `conv1`  |
| Conveyor 2       | `conv2`  |
| Output mechanism | `out`    |

---

# Controller Layout

The omnidrive uses three controller axes:

```cpp
Controller1.Axis3
Controller1.Axis4
Controller1.Axis1
```

Their intended functions are:

| Controller Input | Movement           |
| ---------------- | ------------------ |
| `Axis3`          | Forward / backward |
| `Axis4`          | Left / right       |
| `Axis1`          | Rotation           |

Conceptually:

```text
Left Joystick

         Forward
            ↑
            |
Left  ←-----+-----→  Right
            |
            ↓
         Backward


Right Joystick

Rotate Left  ←-----→  Rotate Right
```

All three inputs can be used at once.

For example:

```text
Axis3 + Axis4
```

allows diagonal movement.

```text
Axis3 + Axis1
```

allows the robot to move forward while rotating.

```text
Axis3 + Axis4 + Axis1
```

allows the robot to translate in any direction while simultaneously rotating.

---

# Omnidrive Motor Math

The most important part of the drivetrain is converting the three joystick inputs into four individual motor speeds.

The original code uses:

```cpp
int FrontLeftSpeed =
    Controller1.Axis3.position()
    + Controller1.Axis4.position()
    + Controller1.Axis1.position();

int FrontRightSpeed =
    Controller1.Axis3.position() * -1
    + Controller1.Axis4.position()
    + Controller1.Axis1.position();

int BackRightSpeed =
    Controller1.Axis3.position() * -1
    - Controller1.Axis4.position()
    + Controller1.Axis1.position();

int BackLeftSpeed =
    Controller1.Axis3.position()
    - Controller1.Axis4.position()
    + Controller1.Axis1.position();
```

This can be written more simply as:

```text
FL =  Axis3 + Axis4 + Axis1
FR = -Axis3 + Axis4 + Axis1
BR = -Axis3 - Axis4 + Axis1
BL =  Axis3 - Axis4 + Axis1
```

If the controller inputs are renamed:

```text
F = Forward
S = Strafe
R = Rotation
```

the equations become:

```text
FL =  F + S + R
FR = -F + S + R
BR = -F - S + R
BL =  F - S + R
```

This process is called **motor mixing**.

---

# Understanding the Math

Each wheel contributes differently to the robot's overall movement.

Instead of assigning one joystick directly to one motor, the program combines the requested:

```text
Forward movement
+
Sideways movement
+
Rotation
```

into a command for every wheel.

For example:

```cpp
FrontLeftSpeed = Forward + Strafe + Rotation;
```

means the front-left wheel contributes to all three types of movement.

The positive and negative signs tell each wheel which direction it must turn to create the requested motion.

---

# Forward Movement

Suppose the driver pushes only `Axis3` forward:

```text
Axis3 = 100
Axis4 = 0
Axis1 = 0
```

The equations become:

```text
FL =  100
FR = -100
BR = -100
BL =  100
```

Although two values are negative, the wheels can still all push the robot forward because the motors and omni wheels are mounted in different orientations.

---

# Strafing

Suppose the driver pushes `Axis4` fully right:

```text
Axis3 = 0
Axis4 = 100
Axis1 = 0
```

The drivetrain receives:

```text
FL =  100
FR =  100
BR = -100
BL = -100
```

This combination causes the robot to move sideways rather than rotate.

---

# Rotation

Suppose only `Axis1` is used:

```text
Axis3 = 0
Axis4 = 0
Axis1 = 100
```

The resulting values are:

```text
FL = 100
FR = 100
BR = 100
BL = 100
```

Because of the orientation of the wheels, this causes the robot to rotate.

---

# Combining Movements

The main advantage of omnidrive is that these movements can be combined.

For example:

```text
Axis3 = 50
Axis4 = 50
Axis1 = 0
```

produces:

```text
FL = 100
FR = 0
BR = -100
BL = 0
```

The robot moves diagonally.

Adding rotation:

```text
Axis3 = 50
Axis4 = 30
Axis1 = 20
```

allows the robot to move diagonally while rotating at the same time.

---

# Speed Limiting

The original example reduces all calculated speeds to approximately 60%:

```cpp
int FrontLeftSpeedFinal = FrontLeftSpeed * 0.6;
int FrontRightSpeedFinal = FrontRightSpeed * 0.6;
int BackRightSpeedFinal = BackRightSpeed * 0.6;
int BackLeftSpeedFinal = BackLeftSpeed * 0.6;
```

For example:

```text
100 × 0.6 = 60
```

The motor is therefore commanded to run at approximately:

```text
60%
```

instead of:

```text
100%
```

The multiplier can be adjusted.

Slower:

```cpp
speed * 0.4;
```

Faster:

```cpp
speed * 0.8;
```

Full power:

```cpp
speed * 1.0;
```

However, there is an important problem with simply multiplying the calculated values.

---

## Motor Speeds Can Exceed 100%

Because three joystick inputs are added together, a calculated motor speed can theoretically reach:

```text
100 + 100 + 100 = 300
```

With the original `0.6` multiplier:

```text
300 × 0.6 = 180
```

The code can therefore still request:

```text
180%
```

even though the normal motor percentage range is only:

```text
-100% to +100%
```

A better solution is discussed later in the [Untested Speed Normalization](#untested-speed-normalization) section.

---

# Deadband

Joysticks do not always return exactly zero when released.

For example, a centered joystick may report:

```text
1
-2
3
```

Without a deadband, these small values could cause the motors to:

* Twitch
* Hum
* Slowly move
* Waste battery power

The original code treats values between approximately `-5` and `5` as zero.

For example:

```cpp
if (FrontLeftSpeedFinal < 5 && FrontLeftSpeedFinal > -5) {
    FL.stop();
}
```

This is equivalent to checking:

```text
-5 < speed < 5
```

A cleaner version could use:

```cpp
if (std::abs(FrontLeftSpeedFinal) < 5) {
    FL.stop();
}
```

---

# Motor Stop Flags

The original program uses four boolean variables:

```cpp
bool FrontLeftNeedsToBeStopped_Controller1 = true;
bool FrontRightNeedsToBeStopped_Controller1 = true;
bool BackLeftNeedsToBeStopped_Controller1 = true;
bool BackRightNeedsToBeStopped_Controller1 = true;
```

These keep track of whether a motor has already received a stop command.

For example:

```cpp
if (FrontLeftSpeedFinal < 5 && FrontLeftSpeedFinal > -5) {

    if (FrontLeftNeedsToBeStopped_Controller1) {

        FL.stop();

        FrontLeftNeedsToBeStopped_Controller1 = false;
    }

} else {

    FrontLeftNeedsToBeStopped_Controller1 = true;
}
```

The first time the wheel enters the deadband:

```cpp
FL.stop();
```

is sent.

Then:

```cpp
FrontLeftNeedsToBeStopped_Controller1 = false;
```

records that the motor has already been stopped.

This prevents the program from unnecessarily sending the same stop command every 20 milliseconds.

When the driver moves the joystick again:

```cpp
FrontLeftNeedsToBeStopped_Controller1 = true;
```

is restored.

---

# Sending Speeds to the Motors

If the wheel is outside the deadband, the speed is sent to the motor.

For example:

```cpp
if (FrontLeftNeedsToBeStopped_Controller1) {

    FL.setVelocity(FrontLeftSpeedFinal, percent);

    FL.spin(forward);
}
```

The same pattern is used for every wheel:

```cpp
if (FrontLeftNeedsToBeStopped_Controller1) {
    FL.setVelocity(FrontLeftSpeedFinal, percent);
    FL.spin(forward);
}

if (FrontRightNeedsToBeStopped_Controller1) {
    FR.setVelocity(FrontRightSpeedFinal, percent);
    FR.spin(forward);
}

if (BackLeftNeedsToBeStopped_Controller1) {
    BL.setVelocity(BackLeftSpeedFinal, percent);
    BL.spin(forward);
}

if (BackRightNeedsToBeStopped_Controller1) {
    BR.setVelocity(BackRightSpeedFinal, percent);
    BR.spin(forward);
}
```

Negative velocity values automatically reverse the requested direction.

For example:

```cpp
FL.setVelocity(60, percent);
FL.spin(forward);
```

and:

```cpp
FL.setVelocity(-60, percent);
FL.spin(forward);
```

produce opposite motor directions.

---

# Shoulder Button Controls

The drivetrain loop also controls the conveyor and output mechanisms.

---

## Conveyor Controls

The left shoulder buttons control:

```cpp
conv1
conv2
```

### L2

```cpp
if (Controller1.ButtonL2.pressing()) {

    conv1.spin(reverse);
    conv2.spin(forward);

    Controller1LeftShoulderControlMotorsStopped = false;
}
```

### L1

```cpp
else if (Controller1.ButtonL1.pressing()) {

    conv1.spin(forward);
    conv2.spin(reverse);

    Controller1LeftShoulderControlMotorsStopped = false;
}
```

### Neither Button

```cpp
else if (!Controller1LeftShoulderControlMotorsStopped) {

    conv1.stop();
    conv2.stop();

    Controller1LeftShoulderControlMotorsStopped = true;
}
```

---

## Output Controls

The right shoulder buttons control:

```cpp
out
```

### R2

```cpp
if (Controller1.ButtonR2.pressing()) {

    out.spin(forward);

    Controller1RightShoulderControlMotorsStopped = false;
}
```

### R1

```cpp
else if (Controller1.ButtonR1.pressing()) {

    out.spin(reverse);

    Controller1RightShoulderControlMotorsStopped = false;
}
```

### Neither Button

```cpp
else if (!Controller1RightShoulderControlMotorsStopped) {

    out.stop();

    Controller1RightShoulderControlMotorsStopped = true;
}
```

---

# Controller Loop

The controller logic runs inside:

```cpp
int rc_auto_loop_function_Controller1() {

    while (true) {

        // Controller code

        wait(20, msec);
    }

    return 0;
}
```

The basic sequence is:

```text
Read controller
      ↓
Calculate wheel speeds
      ↓
Apply speed limiting
      ↓
Apply deadband
      ↓
Process mechanism controls
      ↓
Send commands to drive motors
      ↓
Wait 20 milliseconds
      ↓
Repeat
```

---

## Why Wait 20 ms?

The line:

```cpp
wait(20, msec);
```

prevents the loop from running unnecessarily fast.

With a 20 ms delay:

```text
1000 ms ÷ 20 ms = 50
```

the drivetrain updates approximately:

```text
50 times per second
```

which is more than enough for driver control.

---

# Original Example Code

Below is a cleaned-up version of the original example.

```cpp
// Controller configuration

bool Controller1RightShoulderControlMotorsStopped = true;
bool Controller1LeftShoulderControlMotorsStopped = true;


// Variables used for controlling drive motors
bool FrontLeftNeedsToBeStopped_Controller1 = true;
bool FrontRightNeedsToBeStopped_Controller1 = true;
bool BackLeftNeedsToBeStopped_Controller1 = true;
bool BackRightNeedsToBeStopped_Controller1 = true;


// Monitoring inputs from Controller1
int rc_auto_loop_function_Controller1() {

    while (true) {

        if (RemoteControlCodeEnabled) {

            // ----------------------------------------
            // Calculate omnidrive motor velocities
            // ----------------------------------------

            int FrontLeftSpeed =
                Controller1.Axis3.position()
                + Controller1.Axis4.position()
                + Controller1.Axis1.position();

            int FrontRightSpeed =
                Controller1.Axis3.position() * -1
                + Controller1.Axis4.position()
                + Controller1.Axis1.position();

            int BackRightSpeed =
                Controller1.Axis3.position() * -1
                - Controller1.Axis4.position()
                + Controller1.Axis1.position();

            int BackLeftSpeed =
                Controller1.Axis3.position()
                - Controller1.Axis4.position()
                + Controller1.Axis1.position();


            // ----------------------------------------
            // Limit speeds to approximately 60%
            // ----------------------------------------

            int FrontLeftSpeedFinal =
                FrontLeftSpeed * 0.6;

            int FrontRightSpeedFinal =
                FrontRightSpeed * 0.6;

            int BackRightSpeedFinal =
                BackRightSpeed * 0.6;

            int BackLeftSpeedFinal =
                BackLeftSpeed * 0.6;


            // ----------------------------------------
            // Front Left Deadband
            // ----------------------------------------

            if (
                FrontLeftSpeedFinal < 5 &&
                FrontLeftSpeedFinal > -5
            ) {

                if (FrontLeftNeedsToBeStopped_Controller1) {

                    FL.stop();

                    FrontLeftNeedsToBeStopped_Controller1 = false;
                }

            } else {

                FrontLeftNeedsToBeStopped_Controller1 = true;
            }


            // ----------------------------------------
            // Front Right Deadband
            // ----------------------------------------

            if (
                FrontRightSpeedFinal < 5 &&
                FrontRightSpeedFinal > -5
            ) {

                if (FrontRightNeedsToBeStopped_Controller1) {

                    FR.stop();

                    FrontRightNeedsToBeStopped_Controller1 = false;
                }

            } else {

                FrontRightNeedsToBeStopped_Controller1 = true;
            }


            // ----------------------------------------
            // Back Left Deadband
            // ----------------------------------------

            if (
                BackLeftSpeedFinal < 5 &&
                BackLeftSpeedFinal > -5
            ) {

                if (BackLeftNeedsToBeStopped_Controller1) {

                    BL.stop();

                    BackLeftNeedsToBeStopped_Controller1 = false;
                }

            } else {

                BackLeftNeedsToBeStopped_Controller1 = true;
            }


            // ----------------------------------------
            // Back Right Deadband
            // ----------------------------------------

            if (
                BackRightSpeedFinal < 5 &&
                BackRightSpeedFinal > -5
            ) {

                if (BackRightNeedsToBeStopped_Controller1) {

                    BR.stop();

                    BackRightNeedsToBeStopped_Controller1 = false;
                }

            } else {

                BackRightNeedsToBeStopped_Controller1 = true;
            }


            // ----------------------------------------
            // Conveyor Controls
            // ----------------------------------------

            if (Controller1.ButtonL2.pressing()) {

                conv1.spin(reverse);
                conv2.spin(forward);

                Controller1LeftShoulderControlMotorsStopped = false;

            } else if (Controller1.ButtonL1.pressing()) {

                conv1.spin(forward);
                conv2.spin(reverse);

                Controller1LeftShoulderControlMotorsStopped = false;

            } else if (!Controller1LeftShoulderControlMotorsStopped) {

                conv1.stop();
                conv2.stop();

                Controller1LeftShoulderControlMotorsStopped = true;
            }


            // ----------------------------------------
            // Output Controls
            // ----------------------------------------

            if (Controller1.ButtonR2.pressing()) {

                out.spin(forward);

                Controller1RightShoulderControlMotorsStopped = false;

            } else if (Controller1.ButtonR1.pressing()) {

                out.spin(reverse);

                Controller1RightShoulderControlMotorsStopped = false;

            } else if (!Controller1RightShoulderControlMotorsStopped) {

                out.stop();

                Controller1RightShoulderControlMotorsStopped = true;
            }


            // ----------------------------------------
            // Send velocities to the drivetrain
            // ----------------------------------------

            if (FrontLeftNeedsToBeStopped_Controller1) {

                FL.setVelocity(
                    FrontLeftSpeedFinal,
                    percent
                );

                FL.spin(forward);
            }


            if (FrontRightNeedsToBeStopped_Controller1) {

                FR.setVelocity(
                    FrontRightSpeedFinal,
                    percent
                );

                FR.spin(forward);
            }


            if (BackLeftNeedsToBeStopped_Controller1) {

                BL.setVelocity(
                    BackLeftSpeedFinal,
                    percent
                );

                BL.spin(forward);
            }


            if (BackRightNeedsToBeStopped_Controller1) {

                BR.setVelocity(
                    BackRightSpeedFinal,
                    percent
                );

                BR.spin(forward);
            }
        }


        // Approximately 50 updates per second
        wait(20, msec);
    }


    return 0;
}


task rc_auto_loop_task_Controller1(
    rc_auto_loop_function_Controller1
);
```

---

# Recommended Code Structure

As a project becomes larger, drivetrain logic should ideally be separated from controller logic.

Instead of putting all the drivetrain calculations directly into the controller loop, a function can be created:

```cpp
void driveOmni(
    double forward,
    double strafe,
    double rotation
) {

    double frontLeft =
        forward + strafe + rotation;

    double frontRight =
        -forward + strafe + rotation;

    double backRight =
        -forward - strafe + rotation;

    double backLeft =
        forward - strafe + rotation;


    FL.setVelocity(frontLeft, percent);
    FR.setVelocity(frontRight, percent);
    BR.setVelocity(backRight, percent);
    BL.setVelocity(backLeft, percent);


    FL.spin(forward);
    FR.spin(forward);
    BR.spin(forward);
    BL.spin(forward);
}
```

Then the controller loop becomes:

```cpp
while (true) {

    driveOmni(
        Controller1.Axis3.position(),
        Controller1.Axis4.position(),
        Controller1.Axis1.position()
    );

    wait(20, msec);
}
```

This structure makes it much easier to:

* Modify drivetrain behavior
* Add autonomous movement
* Add normalization
* Add speed modes
* Add acceleration limiting
* Debug problems

---

# Untested Speed Normalization

> **Experimental / Untested**
>
> The following normalization code has not been tested on the physical robot. Verify motor directions and behavior before using it during competition.

The original code can calculate values greater than `100`.

For example:

```text
Forward  = 100
Strafe   = 100
Rotation = 100
```

could produce:

```text
FL = 300
```

Simply clipping that value to `100` would work, but clipping each motor independently can change the ratio between the wheels.

For example:

```text
Original values:

FL = 150
FR = 75
BR = 50
BL = 100
```

Clipping would produce:

```text
FL = 100
FR = 75
BR = 50
BL = 100
```

The ratios have changed.

That means the actual direction of the robot may also change.

A better solution is **normalization**.

---

## How Normalization Works

First find the largest absolute motor speed.

For:

```text
FL = 150
FR = 75
BR = 50
BL = 100
```

the largest value is:

```text
150
```

Since `150` exceeds the maximum allowed value of `100`, every motor is scaled by:

```text
100 / 150
```

or approximately:

```text
0.667
```

The resulting values are approximately:

```text
FL = 100
FR = 50
BR = 33
BL = 67
```

The motor ratios remain the same.

---

## Example Normalization Function

A possible implementation is:

```cpp
void normalizeDriveSpeeds(
    double &frontLeft,
    double &frontRight,
    double &backLeft,
    double &backRight
) {

    double maximum = std::abs(frontLeft);

    maximum = std::max(
        maximum,
        std::abs(frontRight)
    );

    maximum = std::max(
        maximum,
        std::abs(backLeft)
    );

    maximum = std::max(
        maximum,
        std::abs(backRight)
    );


    if (maximum > 100.0) {

        double scale = 100.0 / maximum;

        frontLeft *= scale;
        frontRight *= scale;
        backLeft *= scale;
        backRight *= scale;
    }
}
```

This requires:

```cpp
#include <algorithm>
#include <cmath>
```

---

## Using Normalization With OmniDrive

The drivetrain code could become:

```cpp
void driveOmni(
    double forward,
    double strafe,
    double rotation
) {

    double frontLeft =
        forward + strafe + rotation;

    double frontRight =
        -forward + strafe + rotation;

    double backRight =
        -forward - strafe + rotation;

    double backLeft =
        forward - strafe + rotation;


    normalizeDriveSpeeds(
        frontLeft,
        frontRight,
        backLeft,
        backRight
    );


    FL.setVelocity(frontLeft, percent);
    FR.setVelocity(frontRight, percent);
    BR.setVelocity(backRight, percent);
    BL.setVelocity(backLeft, percent);


    FL.spin(forward);
    FR.spin(forward);
    BR.spin(forward);
    BL.spin(forward);
}
```

---

## Normalization With a 60% Speed Limit

If the robot should still be limited to approximately `60%`, normalize the speeds first and apply the speed limit afterward.

```cpp
void driveOmni(
    double forward,
    double strafe,
    double rotation
) {

    double frontLeft =
        forward + strafe + rotation;

    double frontRight =
        -forward + strafe + rotation;

    double backRight =
        -forward - strafe + rotation;

    double backLeft =
        forward - strafe + rotation;


    normalizeDriveSpeeds(
        frontLeft,
        frontRight,
        backLeft,
        backRight
    );


    constexpr double DRIVE_SPEED = 0.60;


    frontLeft *= DRIVE_SPEED;
    frontRight *= DRIVE_SPEED;
    backRight *= DRIVE_SPEED;
    backLeft *= DRIVE_SPEED;


    FL.setVelocity(frontLeft, percent);
    FR.setVelocity(frontRight, percent);
    BR.setVelocity(backRight, percent);
    BL.setVelocity(backLeft, percent);


    FL.spin(forward);
    FR.spin(forward);
    BR.spin(forward);
    BL.spin(forward);
}
```

The largest possible output is now approximately:

```text
60%
```

rather than potentially exceeding `100%`.

---

## Why Normalize Before Applying the Speed Limit?

Consider:

```text
FL = 200
FR = 100
BR = 50
BL = 100
```

Normalization produces:

```text
FL = 100
FR = 50
BR = 25
BL = 50
```

Then applying a `0.60` multiplier produces:

```text
FL = 60
FR = 30
BR = 15
BL = 30
```

The relative relationship between the four motors is preserved.

This should produce more predictable movement when several joystick directions are combined.

---

# Possible Six-Wheel Implementation

> **Experimental / Design Dependent**
>
> A six-wheel omnidirectional drivetrain cannot automatically use the exact same equations as the four-wheel drivetrain. The correct equations depend heavily on the physical placement and orientation of the additional wheels.

A possible six-motor layout could look like:

```text
                 FRONT

          FL             FR

          ML             MR

          BL             BR

                  BACK
```

where:

| Position     | Variable |
| ------------ | -------- |
| Front Left   | `FL`     |
| Middle Left  | `ML`     |
| Back Left    | `BL`     |
| Front Right  | `FR`     |
| Middle Right | `MR`     |
| Back Right   | `BR`     |

However, the **wheel orientation matters more than the number of wheels**.

---

## Important Mechanical Consideration

A normal four-wheel X-drive places omni wheels at angles so they can produce both:

```text
Forward/backward force
```

and:

```text
Left/right force
```

Adding two normal traction wheels in the middle could prevent sideways movement.

The extra wheels therefore need to be chosen and positioned so that they do not mechanically fight the strafing movement.

Possible solutions include:

* Additional omni wheels
* A carefully designed six-wheel holonomic layout
* Center omni wheels whose rollers allow sideways movement
* Six independently angled omni-wheel modules

The equations must match the actual wheel geometry.

---

# Simple Experimental Six-Wheel Layout

One possible design would keep the four corner wheels responsible for the normal X-drive motion and add two center omni wheels primarily for forward/backward movement and rotation.

Conceptually:

```text
                 FRONT


        FL                    FR
           \                /
            \              /

        ML        ROBOT       MR

            /              \
           /                \
        BL                    BR


                  BACK
```

The existing corner equations remain:

```text
FL =  F + S + R
FR = -F + S + R

BL =  F - S + R
BR = -F - S + R
```

The middle wheels could potentially use:

```text
ML =  F + R
MR = -F + R
```

The middle wheels would therefore contribute to:

* Forward movement
* Backward movement
* Rotation

but would not directly contribute to strafing.

---

## Example Six-Wheel Code

An experimental implementation might look like:

```cpp
void driveOmni6(
    double forward,
    double strafe,
    double rotation
) {

    // Corner wheels

    double frontLeft =
        forward + strafe + rotation;

    double frontRight =
        -forward + strafe + rotation;

    double backLeft =
        forward - strafe + rotation;

    double backRight =
        -forward - strafe + rotation;


    // Experimental center wheels

    double middleLeft =
        forward + rotation;

    double middleRight =
        -forward + rotation;


    FL.setVelocity(frontLeft, percent);
    ML.setVelocity(middleLeft, percent);
    BL.setVelocity(backLeft, percent);

    FR.setVelocity(frontRight, percent);
    MR.setVelocity(middleRight, percent);
    BR.setVelocity(backRight, percent);


    FL.spin(forward);
    ML.spin(forward);
    BL.spin(forward);

    FR.spin(forward);
    MR.spin(forward);
    BR.spin(forward);
}
```

Again, this is only a possible starting point.

Actual motor signs may need to change.

---

# Six-Wheel Speed Normalization

With six wheels, normalization should include all six motor values.

An example function is:

```cpp
void normalizeDriveSpeeds6(
    double &frontLeft,
    double &middleLeft,
    double &backLeft,
    double &frontRight,
    double &middleRight,
    double &backRight
) {

    double maximum = std::abs(frontLeft);

    maximum = std::max(
        maximum,
        std::abs(middleLeft)
    );

    maximum = std::max(
        maximum,
        std::abs(backLeft)
    );

    maximum = std::max(
        maximum,
        std::abs(frontRight)
    );

    maximum = std::max(
        maximum,
        std::abs(middleRight)
    );

    maximum = std::max(
        maximum,
        std::abs(backRight)
    );


    if (maximum > 100.0) {

        double scale = 100.0 / maximum;

        frontLeft *= scale;
        middleLeft *= scale;
        backLeft *= scale;

        frontRight *= scale;
        middleRight *= scale;
        backRight *= scale;
    }
}
```

The complete experimental function could then be:

```cpp
void driveOmni6(
    double forward,
    double strafe,
    double rotation
) {

    double frontLeft =
        forward + strafe + rotation;

    double frontRight =
        -forward + strafe + rotation;


    double middleLeft =
        forward + rotation;

    double middleRight =
        -forward + rotation;


    double backLeft =
        forward - strafe + rotation;

    double backRight =
        -forward - strafe + rotation;


    normalizeDriveSpeeds6(
        frontLeft,
        middleLeft,
        backLeft,
        frontRight,
        middleRight,
        backRight
    );


    constexpr double DRIVE_SPEED = 0.60;


    frontLeft *= DRIVE_SPEED;
    middleLeft *= DRIVE_SPEED;
    backLeft *= DRIVE_SPEED;

    frontRight *= DRIVE_SPEED;
    middleRight *= DRIVE_SPEED;
    backRight *= DRIVE_SPEED;


    FL.setVelocity(frontLeft, percent);
    ML.setVelocity(middleLeft, percent);
    BL.setVelocity(backLeft, percent);

    FR.setVelocity(frontRight, percent);
    MR.setVelocity(middleRight, percent);
    BR.setVelocity(backRight, percent);


    FL.spin(forward);
    ML.spin(forward);
    BL.spin(forward);

    FR.spin(forward);
    MR.spin(forward);
    BR.spin(forward);
}
```

The controller loop would then use:

```cpp
while (true) {

    driveOmni6(
        Controller1.Axis3.position(),
        Controller1.Axis4.position(),
        Controller1.Axis1.position()
    );

    wait(20, msec);
}
```

---

# More Accurate Six-Wheel Motor Mixing

For a more advanced drivetrain, every wheel should ideally be treated as its own movement vector.

Instead of assuming:

```text
FL
FR
ML
MR
BL
BR
```

have specific equations, each wheel can have coefficients describing how strongly it contributes to:

```text
Forward
Strafe
Rotation
```

The general equation is:

```text
WheelSpeed =
    Forward × ForwardCoefficient
  + Strafe  × StrafeCoefficient
  + Rotation × RotationCoefficient
```

For example:

```cpp
double wheelSpeed =
    forward * forwardCoefficient
    + strafe * strafeCoefficient
    + rotation * rotationCoefficient;
```

A six-wheel robot could therefore have something conceptually similar to:

```text
FL = F( 1.0) + S( 1.0) + R(1.0)

ML = F( 1.0) + S( 0.0) + R(1.0)

BL = F( 1.0) + S(-1.0) + R(1.0)

FR = F(-1.0) + S( 1.0) + R(1.0)

MR = F(-1.0) + S( 0.0) + R(1.0)

BR = F(-1.0) + S(-1.0) + R(1.0)
```

This is essentially what the experimental six-wheel equations above are doing.

For a custom chassis, the coefficients could be changed to better match the actual wheel angles.

---

# Why Six Wheels Could Be Useful

A properly designed six-wheel holonomic drivetrain could potentially provide:

* More traction
* Greater pushing force
* Better weight distribution
* More motor power
* Better acceleration
* More support for a heavy robot

However, there are tradeoffs.

Possible disadvantages include:

* More weight
* More motors
* More current draw
* More complicated programming
* Greater mechanical friction
* More opportunities for wheels to fight each other
* Less space for other mechanisms

A six-wheel drivetrain should therefore be designed mechanically and mathematically together rather than simply adding two motors to a four-wheel drivetrain.

---

# Testing the Drive

Testing should be done one movement at a time.

---

## Test 1 — Forward

Move only:

```text
Axis3
```

Expected result:

```text
Robot moves directly forward and backward.
```

If the robot rotates or strafes, check motor directions.

---

## Test 2 — Strafing

Move only:

```text
Axis4
```

Expected result:

```text
Robot moves directly left and right.
```

If the robot rotates, one or more drive motors may be reversed incorrectly.

---

## Test 3 — Rotation

Move only:

```text
Axis1
```

Expected result:

```text
Robot rotates in place.
```

---

## Test 4 — Diagonal Movement

Move both:

```text
Axis3
Axis4
```

Expected result:

```text
Robot moves diagonally.
```

---

## Test 5 — Translation and Rotation

Use:

```text
Axis3
Axis4
Axis1
```

at the same time.

The robot should be able to translate while rotating.

---

# Testing Six-Wheel Drive

If experimenting with six-wheel omnidrive, test the center wheels separately before running all six motors.

For example:

```cpp
ML.setVelocity(30, percent);
MR.setVelocity(-30, percent);

ML.spin(forward);
MR.spin(forward);
```

Verify that the center wheels push the robot in the intended direction.

Then test:

1. Forward movement
2. Backward movement
3. Rotation
4. Strafing
5. Diagonal movement
6. Strafing while rotating

Pay especially close attention during strafing.

If the center wheels:

* Drag
* Skip
* Fight the corner wheels
* Prevent sideways movement

then the mechanical wheel arrangement likely needs to be changed.

---

# Possible Improvements

Once the basic drivetrain works, several features could improve it.

---

## Adjustable Deadband

Instead of repeating:

```cpp
if (speed < 5 && speed > -5)
```

create:

```cpp
double applyDeadband(
    double value,
    double deadband
) {

    if (std::abs(value) < deadband) {
        return 0;
    }

    return value;
}
```

Use it like:

```cpp
forward = applyDeadband(forward, 5);
strafe = applyDeadband(strafe, 5);
rotation = applyDeadband(rotation, 5);
```

---

## Adjustable Drive Speed

Create a constant:

```cpp
constexpr double DRIVE_SPEED = 0.60;
```

Then:

```cpp
frontLeft *= DRIVE_SPEED;
frontRight *= DRIVE_SPEED;
backLeft *= DRIVE_SPEED;
backRight *= DRIVE_SPEED;
```

This makes the maximum speed easy to change.

---

## Precision Mode

A button could reduce drivetrain speed.

For example:

```cpp
double driveSpeed = 0.60;

if (Controller1.ButtonA.pressing()) {
    driveSpeed = 0.30;
}
```

This would give:

```text
Normal mode:
60%

Precision mode:
30%
```

---

## Turbo Mode

Another button could allow full speed:

```cpp
double driveSpeed = 0.60;

if (Controller1.ButtonA.pressing()) {

    driveSpeed = 0.30;

} else if (Controller1.ButtonB.pressing()) {

    driveSpeed = 1.00;
}
```

This could produce:

```text
Precision = 30%
Normal    = 60%
Turbo     = 100%
```

---

## Acceleration Limiting

Instead of instantly changing:

```text
0%
```

to:

```text
100%
```

the software could gradually increase the motor command.

This is sometimes called:

* Slew-rate limiting
* Acceleration limiting
* Ramp limiting

Possible benefits include:

* Smoother control
* Less wheel slip
* Reduced sudden current draw
* Better driver precision

---

## Joystick Response Curves

Raw joystick input is linear.

For example:

```text
Joystick 20% → Motor 20%
Joystick 50% → Motor 50%
Joystick 100% → Motor 100%
```

A nonlinear curve could provide more precise control near the center while retaining maximum speed.

For example:

```cpp
double curveJoystick(double input) {

    double normalized = input / 100.0;

    return normalized
        * normalized
        * normalized
        * 100.0;
}
```

This produces a cubic joystick curve.

Small joystick movements become gentler while large movements still reach full power.

---

# How OmniDrive Works Physically

An omnidirectional drivetrain works because each wheel generates force in a specific direction.

Instead of thinking about:

```text
Left motors
vs.
Right motors
```

like a tank drive, think about every wheel contributing a movement vector.

The driver requests:

```text
Forward force
+
Sideways force
+
Rotational force
```

The motor mixing equations determine how much of each request should be sent to each wheel.

That is the core idea behind holonomic and omnidirectional drive programming.

---

# Quick Reference

## Controller

```text
Axis3 = Forward / backward

Axis4 = Left / right

Axis1 = Rotation
```

---

## Four-Wheel Drive Equations

```text
FL =  F + S + R

FR = -F + S + R

BR = -F - S + R

BL =  F - S + R
```

Where:

```text
F = Forward

S = Strafe

R = Rotation
```

---

## Experimental Six-Wheel Equations

```text
FL =  F + S + R

ML =  F + R

BL =  F - S + R


FR = -F + S + R

MR = -F + R

BR = -F - S + R
```

These equations are only a starting point and depend on the physical drivetrain layout.

---

## Mechanism Controls

```text
L2 = Conveyor direction 1

L1 = Conveyor direction 2

R2 = Output forward

R1 = Output reverse
```

---

## Deadband

```text
Approximately -5 to +5
```

---

## Original Speed Multiplier

```text
60%
```

---

## Controller Update Rate

```text
20 ms per loop

≈ 50 updates per second
```

---

# Summary

The basic four-wheel omnidrive equations are:

```text
FL =  F + S + R

FR = -F + S + R

BR = -F - S + R

BL =  F - S + R
```

These equations combine:

```text
Forward / backward movement
+
Left / right movement
+
Rotation
```

into four individual motor commands.

The most important improvement to the original implementation is **speed normalization**.

Instead of allowing mixed joystick inputs to create commands above `100%`, normalization scales all motors together while preserving their relative ratios.

A six-wheel implementation is also possible, but the equations should be designed around the actual orientation of the wheels. Simply adding another pair of motors does not automatically create a better omnidirectional drivetrain.

The main concept to remember is:

> **The joystick does not directly control individual motors. The requested movement is mathematically mixed into every wheel so that all of the wheels work together to create the desired robot motion.**
