#pragma once

extern const int ROLLER_TIMEOUT; //Timeout for rollers when bringing a cube into the intake
extern const int SLOW_ROLLER_SPEED; //Speed for rollers for slow intake button
extern const int PASSIVE_POWER; //Power set to rollers when nothing is being done to hold cubes in

extern bool is_intaking;

void set_rollers(int input);

void set_rollers_hold();
void set_rollers_coast();

void roller_control();

bool check_shift();
