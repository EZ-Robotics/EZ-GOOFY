#include "main.h"

////////////////
//  AUTON VARIABLES
////////////////
const int ARM_DOWN_POWER  = -40;  //Power the arm holds down at while intaking
const int TRAY_DOWN_POWER = -20;  //Power the arm holds down at while intaking
const int LINE_SENSOR     = 1900; //Desired value for cube to go to

////////////////
//  ARM VARIABLES
////////////////
// - score and descore positions for every tower height
const int DSCORE_LOW = 850;
const int SCORE_LOW  = 1000;
const int DSCORE_MID = 1150;
const int SCORE_MID  = 1300;
const int SCORE_HIGH = 1560;
// - the arm holds itself down to stop from raising while intaking cubes,
// - PASSIVE_ARM_DOWN is used when the intake button is pressed, and ACTIVE_ARM_DOWN
// - is used when the intake button is pressed
const int PASSIVE_ARM_DOWN = -20;
const int ACTIVE_ARM_DOWN  = -45;

////////////////
//  TRAY VARIABLES
////////////////
// - tray uses TRAY_FAST_SPEED until it reaches TRAY_SLOW_DOWN
// - then it uses TRAY_SLOW_DOWN until it reaches TAY_MAX and stops it from going farther
const int TRAY_MAX							  = 4500; //Max position of tray
const int TRAY_SLOW_DOWN 			  = 2000; //Position for the tray to go slow
const int TRAY_SUPER_SLOW_DOWN  = 3300; //Poisition for tray super slow
const int TRAY_FAST_SPEED       = 127;  //Fast speed
const int TRAY_SLOW_SPEED       = 50;   //Medium speed
const int TRAY_SUPER_SLOW_SPEED = 30;   //Slow speed

////////////////
//  ROLLER VARIABLES
////////////////
const int ROLLER_TIMEOUT 		= 250; //Timeout for rollers when bringing a cube into the intake
const int SLOW_ROLLER_SPEED = 60;  //Speed for rollers for slow intake button
const int PASSIVE_POWER 		= 7;   //Power set to rollers when nothing is being done to hold cubes in

pros::ADIAnalogIn auton_select(7);
const int R6 = 1;
const int R3 = 2;
const int B3 = 3;
const int B6 = 4;
int auton;

void initialize() {
	pros::delay(500);
	master.clear();
	pros::lcd::initialize();
	reset_drive_sensor();

	if (auton_select.get_value()>0 && auton_select.get_value()<330) {
		auton = R6;
	}
	else if (auton_select.get_value()>330 && auton_select.get_value()<1800) {
		auton = R3;
	}
	else if (auton_select.get_value()>1800 && auton_select.get_value()<3300) {
		auton = B3;
	}
	else if (auton_select.get_value()>3300 && auton_select.get_value()<4100) {
		auton = B6;
	}

	set_tank_hold();

	reset_tray_sensor();

	tray_pid_task.suspend();
	tray_controller.suspend();
	arm_pid_task.suspend();
	arm_controller.suspend();
	angle_calculate.suspend();
	angle_pid.suspend();
	drive_pid.suspend();
}

void disabled() {}
void competition_initialize() {}

void
red_6() {
	reset_drive_sensor();

	angle_calculate.resume();

	//Hold arm and tray back
	set_tray(TRAY_DOWN_POWER);
	set_arm(ARM_DOWN_POWER);

	//Intake and grab cubes
	set_rollers(127);
	set_drive_pid(36, 0, 40, 0.4, 3);
	drive_pid.resume();
	wait_drive();

	//Reset tray
	set_tray(0);
	tray_pid_task.resume();
	set_tray_pid(0);

	//Turn to grab 6th cube
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(25, 80, 6);
	wait_turn();
	pros::delay(250);
	angle_pid.suspend();

	//Intake and grab cubes
	drive_pid.resume();
	set_rollers(127);
	set_drive_pid(8, 25, 30, 0.4, 3);
	drive_pid.resume();
	wait_drive();

	//Turn to face 0 deg
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(0, 60, 6);
	wait_turn();
	pros::delay(250);
	angle_pid.suspend();

	//Drive back a little
	drive_pid.resume();
	set_drive_pid(-36, 0, 60, 0.4, 3);

	while (get_cube_sensor()>LINE_SENSOR) {
			set_rollers(-80);
	}
	set_rollers(0);
	set_bottom_clamp(true);
	pros::delay(150);

	wait_drive();
	pros::delay(250);
	set_rollers(-7);

	//Turn to face zone
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(-110, 60, 6);
	wait_turn();
	pros::delay(250);
	angle_pid.suspend();

	//Drive to the zone
	set_tank(40, 60);
	pros::delay(750);
	set_tank(20, 20);

	//Score stack
	tray_pid_task.suspend();
	while (get_tray_sensor()<TRAY_SLOW_DOWN) {
		set_tray(TRAY_FAST_SPEED);
	}
	while(get_tray_sensor()<TRAY_SUPER_SLOW_DOWN) {
		set_tray(TRAY_SLOW_SPEED);
	}
	while(get_tray_sensor()<TRAY_MAX) {
		set_tray(TRAY_SUPER_SLOW_SPEED);
	}
	tray_pid_task.resume();
	set_tray_pid(4500);
	set_bottom_clamp(false);
	set_tank(-10, -10);
	pros::delay(250);
	set_rollers(-127);
	pros::delay(250);

	//Back up
	set_tank(-127, -127);
	pros::delay(250);
	set_tray_pid(0);
	set_tank(-10, -10);
	pros::delay(500);
	set_tank(0, 0);
}

void red_3() {
	reset_drive_sensor();

	angle_calculate.resume();

	//Hold arm and tray back
	set_tray(TRAY_DOWN_POWER);
	set_arm(ARM_DOWN_POWER);

	//Intake and grab cubes
	set_rollers(127);
	set_drive_pid(20, 0, 40, 0.4, 3);
	drive_pid.resume();
	wait_drive();

	//Reset tray
	set_tray(0);
	tray_pid_task.resume();
	set_tray_pid(0);

	//Turn to face zone
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(90, 80, 6);
	wait_turn();
	pros::delay(250);
	angle_pid.suspend();

	//Drive forward to zone
	drive_pid.resume();
	set_drive_pid(18, 90, 40, 0.4, 3);
	wait_drive();

	//Turn to face zone
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(120, 80, 6);
	wait_turn();
	pros::delay(250);

	//Outtake a little
	while (get_cube_sensor()>LINE_SENSOR) {
			set_rollers(-80);
	}
	set_rollers(0);
	set_bottom_clamp(true);
	pros::delay(250);

	//Drive forward to zone
	angle_pid.suspend();
	drive_pid.resume();
	set_drive_pid(10, 120, 30, 0.4, 3);
	pros::delay(500);

	//Score stack
	tray_pid_task.suspend();
	while (get_tray_sensor()<TRAY_SLOW_DOWN) {
		set_tray(TRAY_FAST_SPEED);
	}
	while(get_tray_sensor()<TRAY_SUPER_SLOW_DOWN) {
		set_tray(TRAY_SLOW_SPEED);
	}
	while(get_tray_sensor()<TRAY_MAX) {
		set_tray(TRAY_SUPER_SLOW_SPEED);
	}
	tray_pid_task.resume();
	set_bottom_clamp(false);
	set_tray_pid(4500);
	set_rollers(-127);
	pros::delay(500);

	//Drive back
	set_drive_pid(-15, 120, 30, 0.4, 3);
	pros::delay(500);
	set_tray_pid(0);
	wait_drive();
	set_rollers(0);
}

void blue_3() {
	reset_drive_sensor();

	angle_calculate.resume();

	//Hold arm and tray back
	set_tray(TRAY_DOWN_POWER);
	set_arm(ARM_DOWN_POWER);

	//Intake and grab cubes
	set_rollers(127);
	set_drive_pid(20, 0, 40, 0.4, 3);
	drive_pid.resume();
	wait_drive();

	//Reset tray
	set_tray(0);
	tray_pid_task.resume();
	set_tray_pid(0);

	//Turn to face zone
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(-90, 80, 6);
	wait_turn();
	pros::delay(250);
	angle_pid.suspend();

	//Drive forward to zone
	drive_pid.resume();
	set_drive_pid(18, -90, 40, 0.4, 3);
	wait_drive();

	//Turn to face zone
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(-120, 80, 6);
	wait_turn();
	pros::delay(250);

	//Outtake a little
	while (get_cube_sensor()>LINE_SENSOR) {
			set_rollers(-80);
	}
	set_rollers(0);
	set_bottom_clamp(true);
	pros::delay(250);

	//Drive forward to zone
	angle_pid.suspend();
	drive_pid.resume();
	set_drive_pid(10, -120, 30, 0.4, 3);
	pros::delay(500);

	//Score stack
	tray_pid_task.suspend();
	while (get_tray_sensor()<TRAY_SLOW_DOWN) {
		set_tray(TRAY_FAST_SPEED);
	}
	while(get_tray_sensor()<TRAY_SUPER_SLOW_DOWN) {
		set_tray(TRAY_SLOW_SPEED);
	}
	while(get_tray_sensor()<TRAY_MAX) {
		set_tray(TRAY_SUPER_SLOW_SPEED);
	}
	tray_pid_task.resume();
	set_bottom_clamp(false);
	set_tray_pid(4500);
	set_rollers(-127);
	pros::delay(500);

	//Drive back
	set_drive_pid(-15, -120, 30, 0.4, 3);
	pros::delay(500);
	set_tray_pid(0);
	wait_drive();
	set_rollers(0);
}

void
blue_6() {
	reset_drive_sensor();

	angle_calculate.resume();

	//Hold arm and tray back
	set_tray(TRAY_DOWN_POWER);
	set_arm(ARM_DOWN_POWER);

	//Intake and grab cubes
	set_rollers(127);
	set_drive_pid(36, 0, 40, 0.4, 3);
	drive_pid.resume();
	wait_drive();

	//Reset tray
	set_tray(0);
	tray_pid_task.resume();
	set_tray_pid(0);

	//Turn to grab 6th cube
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(-25, 80, 6);
	wait_turn();
	pros::delay(250);
	angle_pid.suspend();

	//Intake and grab cubes
	drive_pid.resume();
	set_rollers(127);
	set_drive_pid(8, -25, 30, 0.4, 3);
	drive_pid.resume();
	wait_drive();

	//Turn to face 0 deg
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(0, 60, 6);
	wait_turn();
	pros::delay(250);
	angle_pid.suspend();

	//Drive back a little
	drive_pid.resume();
	set_drive_pid(-36, 0, 60, 0.4, 3);

	while (get_cube_sensor()>LINE_SENSOR) {
			set_rollers(-80);
	}
	set_rollers(0);
	set_bottom_clamp(true);
	pros::delay(150);

	wait_drive();
	pros::delay(250);
	set_rollers(-7);

	//Turn to face zone
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(110, 60, 6);
	wait_turn();
	pros::delay(250);
	angle_pid.suspend();

	//Drive to the zone
	set_tank(60, 40);
	pros::delay(750);
	set_tank(20, 20);

	//Score stack
	tray_pid_task.suspend();
	while (get_tray_sensor()<TRAY_SLOW_DOWN) {
		set_tray(TRAY_FAST_SPEED);
	}
	while(get_tray_sensor()<TRAY_SUPER_SLOW_DOWN) {
		set_tray(TRAY_SLOW_SPEED);
	}
	while(get_tray_sensor()<TRAY_MAX) {
		set_tray(TRAY_SUPER_SLOW_SPEED);
	}
	tray_pid_task.resume();
	set_tray_pid(4500);
	set_bottom_clamp(false);
	set_tank(-10, -10);
	pros::delay(250);
	set_rollers(-127);
	pros::delay(250);

	//Back up
	set_tank(-127, -127);
	pros::delay(250);
	set_tray_pid(0);
	set_tank(-10, -10);
	pros::delay(500);
	set_tank(0, 0);
}

void autonomous() {
	if (auton == R3)
		red_3();

	if (auton == R6)
		red_6();

	if (auton == B3)
		blue_3();

	if (auton == B6)
		blue_6();
}

void opcontrol() {
	angle_calculate.suspend();
	drive_pid.suspend();
	angle_pid.suspend();

	set_bottom_clamp(false);
	set_top_clamp(false);

	tray_controller.resume();
	tray_pid_task.resume();
	arm_controller.resume();
	while (true) {
		set_tank(master.get_analog(ANALOG_LEFT_Y), master.get_analog(ANALOG_RIGHT_Y));
		roller_control();

		pros::delay(20);
	}
}
