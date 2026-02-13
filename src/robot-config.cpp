/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       robot-config.cpp                                          */
/*    Author:       VEX Robotics                                              */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Robot configuration and omnidrive implementation          */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
motor FrontLeft = motor(PORT1, ratio18_1, false);
motor FrontRight = motor(PORT2, ratio18_1, true);
motor BackLeft = motor(PORT3, ratio18_1, false);
motor BackRight = motor(PORT4, ratio18_1, true);

// VEXcode generated functions

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Function:     omnidrive                                                 */
/*    Description:  Controls an omnidrive robot with 4 wheels                 */
/*                                                                            */
/*    Parameters:                                                             */
/*      forward:  Forward/backward movement (-100 to 100)                     */
/*      strafe:   Left/right strafing movement (-100 to 100)                  */
/*      rotate:   Rotation speed (-100 to 100)                                */
/*                                                                            */
/*    How it works:                                                           */
/*                                                                            */
/*    Omnidrive Wheel Configuration:                                          */
/*                                                                            */
/*         FRONT                                                              */
/*      FL _____ FR                                                           */
/*        |     |                                                             */
/*        |     |       FL = Front Left                                       */
/*        |_____|       FR = Front Right                                      */
/*      BL       BR     BL = Back Left                                        */
/*                      BR = Back Right                                       */
/*                                                                            */
/*    Each motor's speed is calculated using:                                 */
/*    - Forward/backward component                                            */
/*    - Strafing component (with opposite signs for left/right sides)        */
/*    - Rotation component (with opposite signs for left/right sides)        */
/*                                                                            */
/*    Motor Speed Formulas:                                                   */
/*    FL = forward + strafe + rotate                                          */
/*    FR = forward - strafe - rotate                                          */
/*    BL = forward - strafe + rotate                                          */
/*    BR = forward + strafe - rotate                                          */
/*                                                                            */
/*    These formulas create the proper wheel velocities to achieve:          */
/*    - Forward/backward motion when only 'forward' is non-zero               */
/*    - Strafing (sideways) motion when only 'strafe' is non-zero            */
/*    - Rotation when only 'rotate' is non-zero                               */
/*    - Combined motion when multiple inputs are provided                     */
/*                                                                            */
/*----------------------------------------------------------------------------*/

void omnidrive(int forward, int strafe, int rotate) {
  // Calculate individual motor speeds using omnidrive kinematics
  // These formulas create the vector sum of movement in all directions
  
  int frontLeftSpeed = forward + strafe + rotate;
  int frontRightSpeed = forward - strafe - rotate;
  int backLeftSpeed = forward - strafe + rotate;
  int backRightSpeed = forward + strafe - rotate;
  
  // Find the maximum absolute value to scale down if necessary
  // This prevents any motor from exceeding 100% speed
  int maxSpeed = abs(frontLeftSpeed);
  if (abs(frontRightSpeed) > maxSpeed) maxSpeed = abs(frontRightSpeed);
  if (abs(backLeftSpeed) > maxSpeed) maxSpeed = abs(backLeftSpeed);
  if (abs(backRightSpeed) > maxSpeed) maxSpeed = abs(backRightSpeed);
  
  // Scale down all motors proportionally if any exceed 100%
  // This maintains the direction of movement while preventing motor saturation
  if (maxSpeed > 100) {
    frontLeftSpeed = (frontLeftSpeed * 100) / maxSpeed;
    frontRightSpeed = (frontRightSpeed * 100) / maxSpeed;
    backLeftSpeed = (backLeftSpeed * 100) / maxSpeed;
    backRightSpeed = (backRightSpeed * 100) / maxSpeed;
  }
  
  // Set motor velocities
  FrontLeft.spin(forward, frontLeftSpeed, percent);
  FrontRight.spin(forward, frontRightSpeed, percent);
  BackLeft.spin(forward, backLeftSpeed, percent);
  BackRight.spin(forward, backRightSpeed, percent);
}
