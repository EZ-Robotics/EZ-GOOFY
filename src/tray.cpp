#include "main.h"

//Motor initialize
pros::Motor tray(8);
pros::ADIDigitalOut bottom_clamp(2);
pros::ADIDigitalOut top_clamp(1);
pros::ADIAnalogIn line_sensor(8);

//Set tray
void
set_tray(int input) {
  tray.move(input);
}
void
set_bottom_clamp(bool input) {
  bottom_clamp.set_value(input);
}
void
set_top_clamp(bool input) {
  top_clamp.set_value(input);
}

//Tray sensor
int
get_tray_sensor() {
  return tray.get_position();
}
void
reset_tray_sensor() {
  tray.set_zero_position(0);
}
int
get_cube_sensor() {
  return line_sensor.get_value();
}

//Tray PID
int target;
void
tray_pid(void*) {
  while (true) {
    set_tray((target-get_tray_sensor())*0.5);
    pros::delay(10);
  }
}
pros::Task tray_pid_task(tray_pid, nullptr, "tray pid");
void
set_tray_pid(int input) {
  target = input;
}

void
tray_control(void*) {
  bool is_pid = false;
  int tray_target = 0;
  tray_pid_task.resume();
  int arm_state = 0;
  int a = 0;
  while (true) {
    //Unfold code (only used for skills, untuned for new arm so dont use)
    if (master.get_digital(DIGITAL_X)) {
      tray_pid_task.suspend();
      set_tray(10);
      while (master.get_digital(DIGITAL_X)) {
        pros::delay(1);
      }
      tray_pid_task.resume();
      set_tray_pid(700);
      pros::delay(750);
      set_tray_pid(0);
      pros::delay(1000);
      tray_pid_task.suspend();
      set_tray(-50);
      pros::delay(1000);
      set_tray(0);
      pros::delay(50);
      reset_tray_sensor();
      tray_pid_task.resume();
    }

    //Reset tray to 0
    if (master.get_digital(DIGITAL_Y)) {
      //tray_pid_task.resume();
      arm_state = 0;
      while (get_arm_sensor()>950) {
        pros::delay(1);
      }
      tray_target = 0;
      set_tray_pid(tray_target);
      pros::delay(300);
      set_top_clamp(false);
      is_pid = true;
    }
    //If an arm button is pressed, make sure the tray is at 0 so they don't colide (failsafe)
    else if (master.get_digital(DIGITAL_R1) || master.get_digital(DIGITAL_R2)) {
      //tray_pid_task.resume();
      arm_state = 1;
      set_top_clamp(true);
      tray_target = 0;
      set_tray_pid(tray_target);
      is_pid = true;
    }

    //"Manual" tray out and clamp on the stack (or unclamp if shift)
    if (master.get_digital(DIGITAL_DOWN)) {
      if (is_pid) {
        tray_pid_task.suspend();
        is_pid = false;
      }
      if (check_shift() && a == 0) {
        a = 1;
      }
      //pros::delay(100);
      set_bottom_clamp(a==1?false:true);
      if (arm_state != 1)
        //set_top_clamp(a==1?false:true);
        set_top_clamp(false);
      if (get_tray_sensor()<TRAY_SLOW_DOWN) {
        set_tray(TRAY_FAST_SPEED);
      } else if (get_tray_sensor()<TRAY_SUPER_SLOW_DOWN) {
        set_tray(TRAY_SLOW_SPEED);
      } else {
        set_tray(TRAY_SUPER_SLOW_SPEED);
      }
      tray_target = get_tray_sensor();
    }
    //Bring tray back and unclamp the stack (or clamp if shift)
    else if (master.get_digital(DIGITAL_B)) {
      if (is_pid) {
        tray_pid_task.suspend();
        is_pid = false;
      }
      if (check_shift() && a == 0) {
        a = 1;
      }
      //pros::delay(100);
      set_bottom_clamp(a==1?true:false);
      if (arm_state != 1)
        set_top_clamp(a==1?true:false);
      set_tray(-127);
      tray_target = get_tray_sensor();
    }
    //Sensor reset for tray (failsafe if robot is turned on with the tray in the wrong position)
    else if (master.get_digital(DIGITAL_UP)) {
      tray_pid_task.suspend();
      set_tray(-40);
      while (master.get_digital(DIGITAL_UP)) {
        pros::delay(1);
      }
      set_tray(0);
      pros::delay(50);
      reset_tray_sensor();
      tray_pid_task.resume();
      tray_target = 0;
    }
    //Hold the tray where it is when you let go of a button, and make sure it can't go past TRAY_MAX or 0
    else {
      a=0;
      if (!is_pid) {
        tray_pid_task.resume();
        is_pid = true;
      } else {
        tray_target = tray_target < 0 ? 0 : (tray_target > TRAY_MAX ? TRAY_MAX : tray_target);
        set_tray_pid(tray_target);
      }
    }
    pros::delay(20);
  }
}
pros::Task tray_controller(tray_control, nullptr, "tray control");
