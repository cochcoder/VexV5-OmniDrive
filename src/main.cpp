/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX Robotics                                              */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Omnidrive Robot Example                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// FrontLeft            motor         1               
// FrontRight           motor         2               
// BackLeft             motor         3               
// BackRight            motor         4               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include "robot-config.h"

using namespace vex;

// A global instance of competition
competition Competition;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.     */
/*  Do them in the following function.  You must return from this function  */
/*  or the autonomous and usercontrol tasks will not be started.  This      */
/*  function is only called once after the V5 has been powered on and       */
/*  not every time that the robot is disabled.                              */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of  */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.  */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // Example autonomous routine: Move forward for 2 seconds
  omnidrive(50, 0, 0);  // Move forward at 50% speed
  wait(2, seconds);
  
  // Strafe right for 2 seconds
  omnidrive(0, 50, 0);  // Strafe right at 50% speed
  wait(2, seconds);
  
  // Rotate clockwise for 1 second
  omnidrive(0, 0, 30);  // Rotate at 30% speed
  wait(1, seconds);
  
  // Stop
  omnidrive(0, 0, 0);
}

/*---------------------------------------------------------------------------*/
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase   */
/*  of a VEX Competition.                                                    */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.  */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // User control code here, inside the loop
  while (1) {
    // Get joystick values
    // Left stick Y-axis controls forward/backward movement
    // Left stick X-axis controls left/right strafing
    // Right stick X-axis controls rotation
    
    int forward = Controller1.Axis3.position();   // Forward/backward (-100 to 100)
    int strafe = Controller1.Axis4.position();    // Left/right strafing (-100 to 100)
    int rotate = Controller1.Axis1.position();    // Rotation (-100 to 100)
    
    // Apply deadband to prevent drift from stick imperfections
    if (abs(forward) < 10) forward = 0;
    if (abs(strafe) < 10) strafe = 0;
    if (abs(rotate) < 10) rotate = 0;
    
    // Call omnidrive function with controller inputs
    omnidrive(forward, strafe, rotate);
    
    // Don't hog the CPU
    wait(20, msec);
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
