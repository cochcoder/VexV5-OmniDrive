# VexV5-OmniDrive

// Controller configuration 

// define variables used for controlling motors based on controller inputs
bool Controller1RightShoulderControlMotorsStopped = true;
bool Controller1LeftShoulderControlMotorsStopped = true;

// variables used for controlling motors based on controller inputs
bool FrontLeftNeedsToBeStopped_Controller1 = true;
bool FrontRightNeedsToBeStopped_Controller1 = true;
bool BackLeftNeedsToBeStopped_Controller1 = true;
bool BackRightNeedsToBeStopped_Controller1 = true;


// monitoring inputs from Controller1
int rc_auto_loop_function_Controller1() {
  // process the controller input every 20 milliseconds
  // update the motors based on the input values
  while(true) {
    if(RemoteControlCodeEnabled) {
      
      // calculate the drivetrain motor velocities from the controller joystick axies
      // FL = Axis3 + Axis4 + Axis1
      // FR = Axis3 * -1 + Axis4 + Axis1
      // BR = Axis3 * -1 - Axis4 + Axis1
      // BL = Axis3 - Axis4 + Axis1
      int FrontLeftSpeed = Controller1.Axis3.position() + Controller1.Axis4.position() + Controller1.Axis1.position();
      int FrontRightSpeed = Controller1.Axis3.position() * -1 + Controller1.Axis4.position() + Controller1.Axis1.position();
      int BackRightSpeed = Controller1.Axis3.position() * -1 - Controller1.Axis4.position() + Controller1.Axis1.position();
      int BackLeftSpeed = Controller1.Axis3.position() - Controller1.Axis4.position() + Controller1.Axis1.position();

      int FrontLeftSpeedFinal = FrontLeftSpeed * 0.6;
      int FrontRightSpeedFinal = FrontRightSpeed * 0.6;
      int BackRightSpeedFinal = BackRightSpeed * 0.6;
      int BackLeftSpeedFinal = BackLeftSpeed * 0.6;
      
      // check if the value is inside of the deadband range
      if (FrontLeftSpeedFinal < 5 && FrontLeftSpeedFinal > -5) {
        // check if the left motor has already been stopped
        if (FrontLeftNeedsToBeStopped_Controller1) {
          // stop the left drive motor
          FL.stop();
          // tell the code that the left motor has been stopped
          FrontLeftNeedsToBeStopped_Controller1 = false;
        }
      } else {
        // reset the toggle so that the deadband code knows to stop the left motor nexttime the input is in the deadband range
        FrontLeftNeedsToBeStopped_Controller1 = true;
      }
      // check if the value is inside of the deadband range
      if (FrontRightSpeedFinal < 5 && FrontRightSpeedFinal > -5) {
        // check if the right motor has already been stopped
        if (FrontRightNeedsToBeStopped_Controller1) {
          // stop the right drive motor
          FR.stop();
          // tell the code that the right motor has been stopped
          FrontRightNeedsToBeStopped_Controller1 = false;
        }
      } else {
        // reset the toggle so that the deadband code knows to stop the right motor next time the input is in the deadband range
        FrontRightNeedsToBeStopped_Controller1 = true;
      }
      // check if the value is inside of the deadband range
      if (BackLeftSpeedFinal < 5 && BackLeftSpeedFinal > -5) {
        // check if the right motor has already been stopped
        if (BackLeftNeedsToBeStopped_Controller1) {
          // stop the right drive motor
          BL.stop();
          // tell the code that the right motor has been stopped
          BackLeftNeedsToBeStopped_Controller1 = false;
        }
      } else {
        // reset the toggle so that the deadband code knows to stop the right motor next time the input is in the deadband range
        BackLeftNeedsToBeStopped_Controller1 = true;
      }
      // check if the value is inside of the deadband range
      if (BackRightSpeedFinal < 5 && BackRightSpeedFinal > -5) {
        // check if the right motor has already been stopped
        if (BackRightNeedsToBeStopped_Controller1) {
          // stop the right drive motor
          BR.stop();
          // tell the code that the right motor has been stopped
          BackRightNeedsToBeStopped_Controller1 = false;
        }
      } else {
        // reset the toggle so that the deadband code knows to stop the right motor next time the input is in the deadband range
        BackRightNeedsToBeStopped_Controller1 = true;
      }
      // Configure shoulder buttons
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
      // only tell the left drive motor to spin if the values are not in the deadband range
      if (FrontLeftNeedsToBeStopped_Controller1) { 
        FL.setVelocity(FrontLeftSpeedFinal, percent);
        FL.spin(forward);
      }
      // only tell the right drive motor to spin if the values are not in the deadband range
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
    }
    // wait before repeating the process
    wait(20, msec);
  }
  return 0;
  }

task rc_auto_loop_task_Controller1(rc_auto_loop_function_Controller1);

int main() {}
