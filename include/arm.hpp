#pragma once
#include "api.h"

//Variables for arm
// - score and descore positions for every tower height
extern const int DSCORE_LOW;
extern const int SCORE_LOW;
extern const int DSCORE_MID;
extern const int SCORE_MID;
extern const int SCORE_HIGH;
// - the arm holds itself down to stop from raising while intaking cubes,
// - PASSIVE_ARM_DOWN is used when the intake button is pressed, and ACTIVE_ARM_DOWN
// - is used when the intake button is pressed
extern const int PASSIVE_ARM_DOWN;
extern const int ACTIVE_ARM_DOWN;

void set_arm(int input);

int get_arm_sensor();
void reset_arm_sensor();

void arm_pid(void*);
extern pros::Task arm_pid_task;
void set_arm_pid(int input);

void arm_control(void*);
extern pros::Task arm_controller;
