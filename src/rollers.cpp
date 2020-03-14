#include "main.h"

//Motor initialize
pros::Motor right_roller(11, true);
pros::Motor left_roller(15);
pros::Motor right_roller_2(16);
pros::Motor left_roller_2(6, true);

//Set rollers
void
set_rollers(int input) {
  right_roller.move(input);
  left_roller.move(input);
  right_roller_2.move(input);
  left_roller_2.move(input);
}

//Set brake type rollers
void
set_rollers_hold() {
  right_roller.set_brake_mode(MOTOR_BRAKE_HOLD);
  left_roller.set_brake_mode(MOTOR_BRAKE_HOLD);
  right_roller_2.set_brake_mode(MOTOR_BRAKE_HOLD);
  left_roller_2.set_brake_mode(MOTOR_BRAKE_HOLD);
}
void
set_rollers_coast() {
  right_roller.set_brake_mode(MOTOR_BRAKE_COAST);
  left_roller.set_brake_mode(MOTOR_BRAKE_COAST);
  right_roller_2.set_brake_mode(MOTOR_BRAKE_COAST);
  left_roller_2.set_brake_mode(MOTOR_BRAKE_COAST);
}

//Check if shift key is pressed
bool
check_shift() {
  if (master.get_digital(DIGITAL_L2)) {
    return true;
  }
  return false;
}

bool is_intaking = false;
bool run = false;
int timer;
void
roller_control() {
  if (master.get_digital(DIGITAL_L1)) {
    set_rollers(check_shift()?-127:127);
    is_intaking = check_shift() ? false : true;
  }
  else if (master.get_digital(DIGITAL_RIGHT)) {
    set_rollers(check_shift()?-SLOW_ROLLER_SPEED:SLOW_ROLLER_SPEED);
  }
  else if (master.get_digital(DIGITAL_R1) || master.get_digital(DIGITAL_R2)) {
    run = true;
    if (get_cube_sensor()>1900) {
      if (get_arm_sensor()<300) {
        set_rollers(-80);
      }
      timer++;
      if (timer > ROLLER_TIMEOUT/20) {
        run = false;
      }
    }
  }
  else {
    is_intaking = false;
    if (run) {
      if (get_cube_sensor()>1900) {
        if (get_arm_sensor()<300) {
          set_rollers(-80);
        }
        timer++;
        if (timer > ROLLER_TIMEOUT/20) {
          run = false;
        }
      }
      else {
        timer = 0;
        set_rollers(0);
        run = false;
      }
    }
    else if (get_tray_sensor()>2000) {
      set_rollers(-15);
      timer = 0;
    }
    else if (get_arm_sensor()>100) {
      set_rollers(0);
      set_rollers_hold();
      timer = 0;
    }
    else {
      set_rollers(PASSIVE_POWER);
      timer = 0;
    }
  }
}
