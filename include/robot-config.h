/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       robot-config.h                                            */
/*    Author:       VEX Robotics                                              */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Robot configuration declarations                          */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

using namespace vex;

extern brain Brain;

// VEXcode devices
extern controller Controller1;
extern motor FrontLeft;
extern motor FrontRight;
extern motor BackLeft;
extern motor BackRight;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void );

/**
 * Controls an omnidrive robot with 4 wheels
 * 
 * @param forward Forward/backward movement (-100 to 100)
 * @param strafe Left/right strafing movement (-100 to 100)
 * @param rotate Rotation speed (-100 to 100)
 */
void omnidrive(int forward, int strafe, int rotate);

#endif
