#include "main.h"

//Variables for tray
// - tray uses TRAY_FAST_SPEED until it reaches TRAY_SLOW_DOWN
// - then it uses TRAY_SLOW_DOWN until it reaches TAY_MAX and stops it from going farther
#define TAY_MAX          4500 //Max position of tray
#define TRAY_SLOW_DOWN   2100 //Position for the tray to go slow
#define TRAY_SLOW_SPEED  60   //Slow speed
#define TRAY_FAST_SPEED  127  //Fast speed

//Variables for arm
// - score and descore positions for every tower height
#define DSCORE_LOW 850
#define SCORE_LOW  1000
#define DSCORE_MID 1150
#define SCORE_MID  1300
#define SCORE_HIGH 1560
// - the arm holds itself down to stop from raising while intaking cubes,
// - PASSIVE_ARM_DOWN is used when the intake button is pressed, and ACTIVE_ARM_DOWN
// - is used when the intake button is pressed
#define PASSIVE_ARM_DOWN -10
#define ACTIVE_ARM_DOWN  -30

//Motor initialize
pros::Motor arm(11);

//Set arm
void
set_arm(int input) {
  arm.move(input);
}

//Arm sensor
int
get_arm_sensor() {
  return arm.get_position();
}
void
reset_arm_sensor() {
  arm.set_zero_position(0);
}

//Arm PID
int arm_target;
void
arm_pid(void*) {
  while (true) {
    set_arm((arm_target-get_arm_sensor())*0.5);
    pros::delay(10);
  }
}
pros::Task arm_pid_task(arm_pid, nullptr, "arm_pid");
void
set_arm_pid(int input) {
  arm_target = input;
}

//Check if tray is not at resting position
bool
is_tray_at_pos() {
  pros::delay(10);
  if (get_tray_sensor()>400) {
    return true;
  }
  return false;
}

//Outtake until a cube is detected by the line sensor
void
grab_cube() {
  if (!is_tray_at_pos()) {
    int timer = 0;
    set_rollers(127);
    pros::delay(100);
    while (timer<150) {
      if (get_cube_sensor()>2100) {
        set_rollers(-80);
      } else {
        set_rollers(0);
      }
      timer++;
      pros::delay(1);
    }
  }
  set_rollers(0);
}

//Don't register a shift if the intake button is also being pressed (failsafe)
bool
check_arm_shift() {
  if (master.get_digital(DIGITAL_L2)) {
    if (master.get_digital(DIGITAL_L1) || master.get_digital(DIGITAL_RIGHT)) {
      return false;
    }
    else {
      return true;
    }
  }
  return false;
}

//Arm control
void
arm_control(void*) {
  //arm_pid_task.resume();
  const int UP = 1;
  const int DOWN = 0;
  int arm_state = DOWN;
  bool is_pid = false;
  set_arm_pid(0);
  int timer = 0;
  while (true) {
    //Bring arm to mid tower score/descore
    if (master.get_digital(DIGITAL_R1)) {
      timer = 0;
      if (!is_pid) {
        arm_pid_task.resume();
        is_pid = true;
      }
      pros::delay(arm_state==DOWN?100:0);
      arm_state = UP;
      //pros::delay(is_tray_at_pos()?0:200);
      //grab_cube();
      set_arm_pid(check_arm_shift()?DSCORE_MID:SCORE_MID);
    }
    //Bring arm to high tower score/descore
    else if (master.get_digital(DIGITAL_R2)) {
      timer = 0;
      if (!is_pid) {
        arm_pid_task.resume();
        is_pid = true;
      }
      pros::delay(arm_state==DOWN?100:0);
      arm_state = UP;
      //pros::delay(is_tray_at_pos()?0:200);
      //grab_cube();
      set_arm_pid(check_arm_shift()?DSCORE_LOW:SCORE_LOW);
    }
    //High tower position (unsure if this works lol)
    else if (master.get_digital(DIGITAL_Y) && check_shift()) {
      timer = 0;
      if (!is_pid) {
        arm_pid_task.resume();
        is_pid = true;
      }
      pros::delay(arm_state==DOWN?100:0);
      arm_state = UP;
      //pros::delay(is_tray_at_pos()?0:200);
      //grab_cube();
      set_arm_pid(SCORE_HIGH);
    }
    //Bring arm and tray to 0
    else if (master.get_digital(DIGITAL_Y)) {
      timer = 0;
      if (!is_pid) {
        arm_pid_task.resume();
        is_pid = true;
      }
      arm_state = DOWN;
      set_arm_pid(0);
    }
    //Hold the arm down so it doesn't raise while intaking cubes
    else {
      if (get_arm_sensor()<50 && arm_state == DOWN) {
        if (is_pid) {
          arm_pid_task.suspend();
          is_pid = false;
        }
        set_arm(is_intaking ? ACTIVE_ARM_DOWN : PASSIVE_ARM_DOWN);
        reset_arm_sensor();
      }
    }

    //Unfold button (only used in skills, not retuned for the new arm)
    if (master.get_digital(DIGITAL_X)) {
      arm_pid_task.suspend();
      set_arm(100);
      while (master.get_digital(DIGITAL_X)) {
        pros::delay(1);
      }
      arm_pid_task.resume();
      set_arm_pid(300);
      pros::delay(300);
      set_arm_pid(0);
      pros::delay(1000);
      arm_pid_task.suspend();
      set_arm(-50);
      pros::delay(500);
      reset_arm_sensor();
      arm_pid_task.resume();
    }

    pros::delay(20);
  }
}
pros::Task arm_controller(arm_control, nullptr, "arm control");
