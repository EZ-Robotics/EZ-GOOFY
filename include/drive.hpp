#pragma once
#include "api.h"

extern pros::Controller master;

void set_tank(int input_l, int input_r);

void set_tank_hold();
void set_tank_brake();
void set_tank_coast();

int get_left_drive_sensor();
int get_right_drive_sensor();
void reset_drive_sensor();

void angle_calculate_task(void*);
extern pros::Task angle_calculate;

void angle_pid_task(void*);
extern pros::Task angle_pid;

void wait_turn();
void set_angle_pid(int target, int speed, float input_kp);

void drive_pid_task(void*);
extern pros::Task drive_pid;

void wait_drive();
void set_drive_pid(int target, int target_heading, int speed, float input_kp, float input_kd);
