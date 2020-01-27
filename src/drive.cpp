#include "main.h"
#include "math.h"

pros::Controller master(CONTROLLER_MASTER);

int
clip_num(float input, float max, float min) {
  if (input > max)
    return max;
  else if (input < min)
    return min;
  return input;
}

pros::ADIEncoder RIGHT_SENSOR (3, 4, false);
pros::ADIEncoder LEFT_SENSOR (5, 6, true);

//Motor initialize
pros::Motor left_back(10, MOTOR_GEARSET_6, true);
pros::Motor left_front(18, MOTOR_GEARSET_6, true);
pros::Motor left_top(19, MOTOR_GEARSET_6, true);
pros::Motor right_back(7, MOTOR_GEARSET_6);
pros::Motor right_front(16, MOTOR_GEARSET_6);
pros::Motor right_top(20, MOTOR_GEARSET_6);

//Set drive
void
set_tank(int input_l, int input_r) {
  left_back.move(input_l);
  left_front.move(input_l);
  left_top.move(input_l);
  right_back.move(input_r);
  right_front.move(input_r);
  right_top.move(input_r);
}

//Set brake types
void
set_tank_hold() {
  left_back.set_brake_mode(MOTOR_BRAKE_HOLD);
  left_front.set_brake_mode(MOTOR_BRAKE_HOLD);
  right_back.set_brake_mode(MOTOR_BRAKE_HOLD);
  right_front.set_brake_mode(MOTOR_BRAKE_HOLD);
  right_top.set_brake_mode(MOTOR_BRAKE_HOLD);
  left_top.set_brake_mode(MOTOR_BRAKE_HOLD);
}
void
set_tank_brake() {
  left_back.set_brake_mode(MOTOR_BRAKE_BRAKE);
  left_front.set_brake_mode(MOTOR_BRAKE_BRAKE);
  right_back.set_brake_mode(MOTOR_BRAKE_BRAKE);
  right_front.set_brake_mode(MOTOR_BRAKE_BRAKE);
  right_top.set_brake_mode(MOTOR_BRAKE_BRAKE);
  left_top.set_brake_mode(MOTOR_BRAKE_BRAKE);
}
void
set_tank_coast() {
  left_back.set_brake_mode(MOTOR_BRAKE_COAST);
  left_front.set_brake_mode(MOTOR_BRAKE_COAST);
  right_back.set_brake_mode(MOTOR_BRAKE_COAST);
  right_front.set_brake_mode(MOTOR_BRAKE_COAST);
  right_top.set_brake_mode(MOTOR_BRAKE_COAST);
  left_top.set_brake_mode(MOTOR_BRAKE_COAST);
}

int
get_left_drive_sensor() {
  //return left_back.get_position();
  return LEFT_SENSOR.get_value();
}
int
get_right_drive_sensor() {
  //return right_back.get_position();
  return RIGHT_SENSOR.get_value();
}
void
reset_drive_sensor() {
  RIGHT_SENSOR.reset();
  LEFT_SENSOR.reset();
}

//Tick to Inch Conversion (divide ticks by tick_per_inch)
const float wheel_size = 2.75; //3.5
const float tick_per_rev = 360; //300 * (7/3)
const float circumference = wheel_size*M_PI;
const float tick_per_inch = (tick_per_rev/circumference);

//Angle calculation based on encoder values
float angle = 0.0;
void
angle_calculate_task(void*) {
  //More constants for calculating angle of robot
  const float base_size = 5; //12.6125
  const float base_circumference = base_size*M_PI;
  const float inch_per_deg = base_circumference/180;
  float error;

  while (true) {
    error = (get_right_drive_sensor()-get_left_drive_sensor())/tick_per_inch;
    angle = error/inch_per_deg;
    pros::delay(10);
  }
}
pros::Task angle_calculate(angle_calculate_task, nullptr, "angle calculate");

//PID Controllers for angle of robot
int target_angle = 0;
float angle_kp = 1;
int angle_max_speed = 0;
void
angle_pid_task(void*) {
  int error;
  int last_time, last_error;
  float der;
  int output;
  long dT;
  while (true) {
    dT = pros::millis() - last_time;

    error = target_angle - angle;

    der = (error-last_error);

    output = (error*angle_kp) + (der*25);
    printf("angle %f\n", angle);

    output = clip_num(output, angle_max_speed, -angle_max_speed);

    set_tank(output, -output);

    last_time = pros::millis();
    last_error = error;

    pros::delay(10);
  }
}
pros::Task angle_pid(angle_pid_task, nullptr, "angle pid");

//Delay for turns
void
wait_turn() {
  pros::delay(20);
  int i;
  while (true) {
    pros::delay(20);
    if (abs(target_angle-angle)<2) {
      return;
    }
    if (abs(target_angle-angle)<5) {
      i++;
      if (i>1000/20) {
        return;
      }
    } else {
      i = 0;
    }
  }
}

void
set_angle_pid(int target, int speed, float input_kp) {
  target_angle = target;
  angle_max_speed = speed;
  angle_kp = input_kp;
}

//Drive PID with active straight code
// - it makes sure the angle of the robot is what it should be all the way through the movements,
// - turning if needed to keep it going straight
int l_target_encoder;
int r_target_encoder;
float drive_kp = 0.5;
float drive_kd = 0.2;
int max_speed = 0;
int heading = 0;
void
drive_pid_task(void*) {
  int left_error, right_error, heading_error;
  int last_time, last_l_error, last_r_error, last_heading_error;
  float l_der, r_der, h_der;
  int right_output, left_output, heading_output;;
  long dT;
  while (true) {
    dT = pros::millis() - last_time;

    left_error    = l_target_encoder - get_left_drive_sensor();
    right_error   = r_target_encoder - get_right_drive_sensor();
    heading_error = heading-angle;

    l_der = dT==0 ? 0 : (left_error-last_l_error)/dT;
    r_der = dT==0 ? 0 : (right_error-last_r_error)/dT;
    h_der = dT==0 ? 0 : (heading_error-last_heading_error)/dT;

    left_output    = (left_error*drive_kp)  + (l_der*drive_kd);
    right_output   = (right_error*drive_kp) + (r_der*drive_kd);
    heading_output = (heading_error*5)      + (h_der*10);

    left_output  = clip_num(left_output,  max_speed, -max_speed);
    right_output = clip_num(right_output, max_speed, -max_speed);

    if (pros::millis()<750) {
      set_tank(0, 0);
    } else {
      set_tank(left_output+heading_output, right_output-heading_output);
    }
    //set_tank(left_output, right_output);

    last_time    = pros::millis();
    last_l_error = left_error;
    last_r_error = right_error;
    last_heading_error = heading_error;

    //printf("heading error %f\n", heading-angle);

    pros::delay(10);
  }
}
pros::Task drive_pid(drive_pid_task, nullptr, "drive_pid");

//Wait for drive
void
wait_drive() {
  pros::delay(20);
  int i = 0;
  while (true) {
    pros::delay(20);
    if (abs(l_target_encoder-get_left_drive_sensor())<40 && abs(r_target_encoder-get_right_drive_sensor())<40) {
      return;
    }
    if (abs(l_target_encoder-get_left_drive_sensor())<150 && abs(r_target_encoder-get_right_drive_sensor())<150) {
      i++;
      if (i>1000/20) {
        return;
      }
    } else {
      i = 0;
    }
  }
}

void
set_drive_pid(int target_in, int target_heading, int speed, float input_kp, float input_kd) {
  l_target_encoder = get_left_drive_sensor()  + (target_in*tick_per_inch);
  r_target_encoder = get_right_drive_sensor() + (target_in*tick_per_inch);
  drive_kp  = input_kp;
  drive_kd  = input_kd;
  max_speed = speed;
  heading   = target_heading;
}
