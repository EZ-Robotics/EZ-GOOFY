#pragma once
#include "api.h"

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
