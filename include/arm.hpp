#pragma once
#include "api.h"

void set_arm(int input);

int get_arm_sensor();
void reset_arm_sensor();

void arm_pid(void*);
extern pros::Task arm_pid_task;
void set_arm_pid(int input);

void arm_control(void*);
extern pros::Task arm_controller;
