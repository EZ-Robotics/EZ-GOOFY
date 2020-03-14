#pragma once
#include "api.h"

//Variables for tray
// - tray uses TRAY_FAST_SPEED until it reaches TRAY_SLOW_DOWN
// - then it uses TRAY_SLOW_DOWN until it reaches TAY_MAX and stops it from going farther
extern const int TRAY_MAX; //Max position of tray
extern const int TRAY_SLOW_DOWN; //Position for the tray to go slow
extern const int TRAY_SUPER_SLOW_DOWN; //Poisition for tray super slow
extern const int TRAY_FAST_SPEED; //Fast speed
extern const int TRAY_SLOW_SPEED; //Medium speed
extern const int TRAY_SUPER_SLOW_SPEED; //Slow speed

void set_tray(int input);
void set_bottom_clamp(bool input);
void set_top_clamp(bool input);

int get_tray_sensor();
void reset_tray_sensor();
int get_cube_sensor();

void tray_pid(void*);
extern pros::Task tray_pid_task;

void set_tray_pid(int input);

void tray_control(void*);
extern pros::Task tray_controller;
