#include "main.h"

pros::ADIDigitalIn auton_select(8);
const int BLUE = 0;
const int RED  = 1;
int auton;

/*

3 - right drive
7 - right drive
20 - right drive
10 - left drive TRUE
17 - left drive TRUE
19 - left drive TRUE

8 - tray

11 - arm

12 - right roller TRUE
13 - left roller


*/

void initialize() {
	pros::delay(500);
	master.clear();
	pros::lcd::initialize();
	reset_drive_sensor();

	set_tank_hold();

	reset_tray_sensor();

	tray_pid_task.suspend();
	tray_controller.suspend();
	arm_pid_task.suspend();
	arm_controller.suspend();
	angle_calculate.suspend();
	angle_pid.suspend();
	drive_pid.suspend();

	auton = auton_select.get_value();

	//pros::delay(500);
}

void disabled() {}

void competition_initialize() {}

void
red() {
	drive_pid.resume();
	angle_calculate.resume();
	//angle_pid.resume();

	//Push preload cube forward and drive back
	set_drive_pid(9, 0, 50, 0.5, 0.1);
	wait_drive();
	pros::delay(250);

	set_drive_pid(-4, 0, 50, 0.5, 0.1);
	wait_drive();
	pros::delay(250);

	//Unfold
	arm_pid_task.resume();
	tray_pid_task.resume();
	set_arm_pid(700);
	pros::delay(1000);
	set_arm_pid(0);
	set_tray_pid(0);
	arm_pid_task.suspend();
	tray_pid_task.suspend();
	set_tray(-30);
	pros::delay(150);
	set_arm(-60);
	pros::delay(750);
	reset_arm_sensor();
	set_tray(0);
	pros::delay(100);
	reset_tray_sensor();
	arm_pid_task.resume();
	tray_pid_task.resume();

	//Drive forward to grab first row
	set_drive_pid(35, 0, 30, 0.5, 0.1);
	set_rollers(127);
	wait_drive();
	pros::delay(500);

	//Drive back a little to prepare for turn
	set_drive_pid(-12, 0, 40, 0.5, 0.1);
	set_rollers(127);
	wait_drive();
	pros::delay(500);
	set_rollers(7);

	//Turn
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(50, 100, 6);
	wait_turn();
	pros::delay(500);
	angle_pid.suspend();

	//Drive back to get inline with the second row
	set_drive_pid(-33, 50, 50, 0.9, 0.1);
	drive_pid.resume();
	wait_drive();
	pros::delay(250);

	//Turn to face second row
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(0, 100, 6);
	wait_turn();
	pros::delay(500);
	angle_pid.suspend();

	//Intake half of the cubes on second row
	set_drive_pid(17, 0, 30, 0.9, 0.1);
	set_rollers(127);
	drive_pid.resume();
	wait_drive();
	pros::delay(750);

	//Raise arm tiny bit
	set_arm_pid(300);
	pros::delay(500);

	//Drive in a little bit
	set_drive_pid(7, 0, 20, 0.9, 0.1);
	while (get_cube_sensor()>1900) {
			set_rollers(80);
	}
	set_rollers(50);
	wait_drive();
	pros::delay(250);

	//Drive back a tiny bit
	set_drive_pid(-7, 0, 30, 0.9, 0.1);
	wait_drive();
	pros::delay(500);
	set_arm_pid(0);
	pros::delay(500);

	//Drive back a tiny bit
	set_rollers(127);
	set_drive_pid(23, 0, 30, 0.9, 0.1);
	wait_drive();
	pros::delay(1500);

	//Bring cubes down
	while (get_cube_sensor()>1900) {
			set_rollers(-40);
	}
	set_rollers(0);
	set_bottom_clamp(true);
	set_top_clamp(true);
	pros::delay(150);

	//Bring tray up a little
	set_tray_pid(1500);
	pros::delay(1500);

	//Turn to face zone
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(145, 80, 6);
	wait_turn();
	pros::delay(750);
	angle_pid.suspend();

	//Drive to zone
	set_drive_pid(40, 145, 50, 0.4, 3);
	drive_pid.resume();
	wait_drive();
	pros::delay(500);

	//Center with zone
	set_drive_pid(12, 145, 20, 0.4, 3);
	pros::delay(1500);

	//Score stack
	tray_pid_task.suspend();
	while (get_tray_sensor()<2100) {
			set_tray(127);
	}
	while (get_tray_sensor()<4200){
			set_rollers(-15);
			set_tray(60);
	}
	set_tray_pid(4200);
	tray_pid_task.resume();
	pros::delay(500);

	//Undo clamps, top first
	set_top_clamp(false);
	pros::delay(200);
	set_bottom_clamp(false);
	pros::delay(500);

	//Drive back
	set_drive_pid(-12, 145, 50, 0.5, 0.1);
	wait_drive();
	pros::delay(250);
	set_rollers(0);

	//Bring tray back
	set_tray_pid(0);
	pros::delay(1000);
}

void
blue() {
	drive_pid.resume();
	angle_calculate.resume();
	//angle_pid.resume();

	//Push preload cube forward and drive back
	set_drive_pid(9, 0, 50, 0.5, 0.1);
	wait_drive();
	pros::delay(250);

	set_drive_pid(-4, 0, 50, 0.5, 0.1);
	wait_drive();
	pros::delay(250);

	//Unfold
	arm_pid_task.resume();
	tray_pid_task.resume();
	set_arm_pid(700);
	pros::delay(1000);
	set_arm_pid(0);
	set_tray_pid(0);
	arm_pid_task.suspend();
	tray_pid_task.suspend();
	set_tray(-30);
	pros::delay(150);
	set_arm(-60);
	pros::delay(750);
	reset_arm_sensor();
	set_tray(0);
	pros::delay(100);
	reset_tray_sensor();
	arm_pid_task.resume();
	tray_pid_task.resume();

	//Drive forward to grab first row
	set_drive_pid(35, 0, 30, 0.5, 0.1);
	set_rollers(127);
	wait_drive();
	pros::delay(500);

	//Drive back a little to prepare for turn
	set_drive_pid(-12, 0, 40, 0.5, 0.1);
	set_rollers(127);
	wait_drive();
	pros::delay(500);
	set_rollers(7);

	//Turn
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(-50, 100, 6);
	wait_turn();
	pros::delay(500);
	angle_pid.suspend();

	//Drive back to get inline with the second row
	set_drive_pid(-30, -50, 50, 0.9, 0.1);
	drive_pid.resume();
	wait_drive();
	pros::delay(250);

	//Turn to face second row
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(0, 100, 6);
	wait_turn();
	pros::delay(500);
	angle_pid.suspend();

	//Intake half of the cubes on second row
	set_drive_pid(17, 0, 30, 0.9, 0.1);
	set_rollers(127);
	drive_pid.resume();
	wait_drive();
	pros::delay(750);

	//Raise arm tiny bit
	set_arm_pid(300);
	pros::delay(500);

	//Drive in a little bit
	set_drive_pid(7, 0, 20, 0.9, 0.1);
	while (get_cube_sensor()>1900) {
			set_rollers(80);
	}
	set_rollers(50);
	wait_drive();
	pros::delay(250);

	//Drive back a tiny bit
	set_drive_pid(-7, 0, 30, 0.9, 0.1);
	wait_drive();
	pros::delay(500);
	set_arm_pid(0);
	pros::delay(500);

	//Drive back a tiny bit
	set_rollers(127);
	set_drive_pid(23, 0, 30, 0.9, 0.1);
	wait_drive();
	pros::delay(1500);

	//Bring cubes down
	while (get_cube_sensor()>1900) {
			set_rollers(-40);
	}
	set_rollers(0);
	set_bottom_clamp(true);
	set_top_clamp(true);
	pros::delay(150);

	//Bring tray up a little
	set_tray_pid(1500);
	pros::delay(1500);

	//Turn to face zone
	drive_pid.suspend();
	angle_pid.resume();
	set_angle_pid(-145, 80, 6);
	wait_turn();
	pros::delay(750);
	angle_pid.suspend();

	//Drive to zone
	set_drive_pid(40, -145, 50, 0.4, 3);
	drive_pid.resume();
	wait_drive();
	pros::delay(500);

	//Center with zone
	set_drive_pid(12, -145, 20, 0.4, 3);
	pros::delay(1500);

	//Score stack
	tray_pid_task.suspend();
	while (get_tray_sensor()<2100) {
			set_tray(127);
	}
	while (get_tray_sensor()<4200){
			set_rollers(-15);
			set_tray(60);
	}
	set_tray_pid(4200);
	tray_pid_task.resume();
	pros::delay(500);

	//Undo clamps, top first
	set_top_clamp(false);
	pros::delay(200);
	set_bottom_clamp(false);
	pros::delay(500);

	//Drive back
	set_drive_pid(-12, -145, 50, 0.5, 0.1);
	wait_drive();
	pros::delay(250);
	set_rollers(0);

	//Bring tray back
	set_tray_pid(0);
	pros::delay(1000);
}


void autonomous() {
	reset_drive_sensor();
 /*
	angle_calculate.resume();
	angle_pid.resume();
	pros::delay(200);
	set_angle_pid(45, 100, 5);
	wait_turn();
	pros::delay(750);
	*/

	blue();
}

void opcontrol() {
	angle_calculate.suspend();
	drive_pid.suspend();
	angle_pid.suspend();

	set_bottom_clamp(false);
	set_top_clamp(false);

	tray_controller.resume();
	tray_pid_task.resume();
	//arm_pid_task.resume();
	arm_controller.resume();
	while (true) {
		set_tank(master.get_analog(ANALOG_LEFT_Y), master.get_analog(ANALOG_RIGHT_Y));
		roller_control();

		pros::lcd::print(2, "left encoder: %i", get_left_drive_sensor());
		pros::lcd::print(3, "right encoder: %i", get_right_drive_sensor());

		pros::delay(20);
	}
}


/*

void
red() {
    drive_pid.resume();
    angle_calculate.resume();
    //angle_pid.resume();

    //Push preload cube forward and drive back
    set_drive_pid(9, 0, 50, 0.5, 0.1);
    wait_drive();
    pros::delay(250);

    set_drive_pid(-4, 0, 50, 0.5, 0.1);
    wait_drive();
    pros::delay(250);

    //Unfold
    arm_pid_task.resume();
    tray_pid_task.suspend();
    set_tray(10);
    set_arm_pid(700);
    pros::delay(1000);
    tray_pid_task.resume();
    set_tray_pid(1200);
    pros::delay(600);
    arm_pid_task.resume();
    set_arm_pid(300);
    pros::delay(300);
    set_arm_pid(0);
    set_tray_pid(0);
    pros::delay(1400);
    arm_pid_task.suspend();
    tray_pid_task.suspend();
    set_tray(-30);
    pros::delay(150);
    set_arm(-50);
    pros::delay(750);
    reset_arm_sensor();
    set_tray(0);
    pros::delay(100);
    reset_tray_sensor();
    arm_pid_task.resume();
    tray_pid_task.resume();

    //Drive forward to grab first row
    set_drive_pid(39, 0, 30, 0.5, 0.1);
    set_rollers(127);
    wait_drive();
    pros::delay(500);

    //Drive back a little to prepare for turn
    set_drive_pid(-18, 0, 40, 0.5, 0.1);
    set_rollers(127);
    wait_drive();
    pros::delay(500);
    set_rollers(7);


    //Turn
    drive_pid.suspend();
    angle_pid.resume();
    set_angle_pid(-52, 100, 6);
    wait_turn();
    pros::delay(100);
    angle_pid.suspend();

    //Drive back to get inline with the second row
    set_drive_pid(-29, -52, 50, 0.9, 0.1);
    drive_pid.resume();
    wait_drive();
    pros::delay(250);

    //Turn to face second row
    drive_pid.suspend();
    angle_pid.resume();
    set_angle_pid(4, 100, 6);
    wait_turn();
    pros::delay(100);
    angle_pid.suspend();

    //Intake cubes on second row
    set_drive_pid(40, 4, 30, 0.9, 0.1);
    set_rollers(127);
    drive_pid.resume();
    wait_drive();
    pros::delay(500);

    //Drive back a little
    set_drive_pid(-24, 4, 50, 0.5, 0.1);
    wait_drive();
    pros::delay(250);

    //Outtake until rollers hit line sensor
    set_rollers(127);
    pros::delay(1000);
    set_rollers(7);
    pros::delay(500);

    while (get_cube_sensor()>1900) {
        set_rollers(-40);
    }
    set_rollers(0);
    set_bottom_clamp(true);
    set_top_clamp(true);
    pros::delay(150);

    //Move tray a little
    set_tray_pid(1500);
    pros::delay(1000);

    //Turn to face zone
    drive_pid.suspend();
    angle_pid.resume();
    set_angle_pid(135, 50, 6);
    wait_turn();
    set_rollers(7);
    pros::delay(100);
    angle_pid.suspend();

    //Drive almost to zone
    set_drive_pid(12, 135, 50, 0.5, 0.1);
    drive_pid.resume();
    wait_drive();
    pros::delay(250);
    set_rollers(0);

    //Compoete drive to zone
    set_drive_pid(12, 135, 30, 0.5, 0.1);
    pros::delay(750);

    //Score stack
    tray_pid_task.suspend();
    while (get_tray_sensor()<2100) {
        set_tray(127);
    }
    while (get_tray_sensor()<4200){
        set_rollers(-15);
        set_tray(60);
    }
    set_tray_pid(4200);
    tray_pid_task.resume();
    pros::delay(500);

    //Undo clamps, top first
    set_top_clamp(false);
    pros::delay(200);
    set_bottom_clamp(false);
    pros::delay(500);

    //Drive back
    set_drive_pid(-42, 135, 50, 0.5, 0.1);
    wait_drive();
    pros::delay(250);
    set_rollers(0);

    //Turn to face stacks
    drive_pid.suspend();
    angle_pid.resume();
    set_angle_pid(-70, 50, 6);
    wait_turn();
    pros::delay(100);
    angle_pid.suspend();

    //Drive forward a little
    set_drive_pid(24, -70, 50, 0.5, 0.1);
    drive_pid.resume();
    set_tray_pid(0);
    wait_drive();
    pros::delay(250);
}

void
blue() {
    drive_pid.resume();
    angle_calculate.resume();
    //angle_pid.resume();

    //Push preload cube forward and drive back
    set_drive_pid(9, 0, 50, 0.5, 0.1);
    wait_drive();
    pros::delay(250);

    set_drive_pid(-4, 0, 50, 0.5, 0.1);
    wait_drive();
    pros::delay(250);

    //Unfold
    arm_pid_task.resume();
    tray_pid_task.suspend();
    set_tray(10);
    set_arm_pid(700);
    pros::delay(1000);
    tray_pid_task.resume();
    set_tray_pid(1200);
    pros::delay(600);
    arm_pid_task.resume();
    set_arm_pid(300);
    pros::delay(300);
    set_arm_pid(0);
    set_tray_pid(0);
    pros::delay(1400);
    arm_pid_task.suspend();
    tray_pid_task.suspend();
    set_tray(-30);
    pros::delay(150);
    set_arm(-50);
    pros::delay(750);
    reset_arm_sensor();
    set_tray(0);
    pros::delay(100);
    reset_tray_sensor();
    arm_pid_task.resume();
    tray_pid_task.resume();

    //Drive forward to grab first row
    set_drive_pid(39, 0, 30, 0.5, 0.1);
    set_rollers(127);
    wait_drive();
    pros::delay(500);

    //Drive back a little to prepare for turn
    set_drive_pid(-18, 0, 40, 0.5, 0.1);
    set_rollers(127);
    wait_drive();
    pros::delay(500);
    set_rollers(7);


    //Turn
    drive_pid.suspend();
    angle_pid.resume();
    set_angle_pid(52, 100, 6);
    wait_turn();
    pros::delay(100);
    angle_pid.suspend();

    //Drive back to get inline with the second row
    set_drive_pid(-32, 52, 50, 0.9, 0.1);
    drive_pid.resume();
    wait_drive();
    pros::delay(250);

    //Turn to face second row
    drive_pid.suspend();
    angle_pid.resume();
    set_angle_pid(4, 100, 6);
    wait_turn();
    pros::delay(100);
    angle_pid.suspend();

    //Intake cubes on second row
    set_drive_pid(40, -4, 30, 0.9, 0.1);
    set_rollers(127);
    drive_pid.resume();
    wait_drive();
    pros::delay(500);

    //Drive back a little
    set_drive_pid(-24, -4, 50, 0.5, 0.1);
    wait_drive();
    pros::delay(250);

    //Outtake until rollers hit line sensor
    set_rollers(127);
    pros::delay(1000);
    set_rollers(7);
    pros::delay(500);

    while (get_cube_sensor()>1900) {
        set_rollers(-40);
    }
    set_rollers(0);
    set_bottom_clamp(true);
    set_top_clamp(true);
    pros::delay(150);

    //Move tray a little
    set_tray_pid(1500);
    pros::delay(1000);

    //Turn to face zone
    drive_pid.suspend();
    angle_pid.resume();
    set_angle_pid(-135, 50, 6);
    wait_turn();
    set_rollers(7);
    pros::delay(100);
    angle_pid.suspend();

    //Drive almost to zone
    set_drive_pid(12, -135, 50, 0.5, 0.1);
    drive_pid.resume();
    wait_drive();
    pros::delay(250);
    set_rollers(0);

    //Compoete drive to zone
    set_drive_pid(12, -135, 30, 0.5, 0.1);
    pros::delay(750);

    //Score stack
    tray_pid_task.suspend();
    while (get_tray_sensor()<2100) {
        set_tray(127);
    }
    while (get_tray_sensor()<4200){
        set_rollers(-15);
        set_tray(60);
    }
    set_tray_pid(4200);
    tray_pid_task.resume();
    pros::delay(500);

    //Undo clamps, top first
    set_top_clamp(false);
    pros::delay(200);
    set_bottom_clamp(false);
    pros::delay(500);

    //Drive back
    set_drive_pid(-42, -135, 50, 0.5, 0.1);
    wait_drive();
    pros::delay(250);
    set_rollers(0);

    //Turn to face stacks
    drive_pid.suspend();
    angle_pid.resume();
    set_angle_pid(70, 50, 6);
    wait_turn();
    pros::delay(100);
    angle_pid.suspend();

    //Drive forward a little
    set_drive_pid(24, 70, 50, 0.5, 0.1);
    drive_pid.resume();
    set_tray_pid(0);
    wait_drive();
    pros::delay(250);
}

*/
