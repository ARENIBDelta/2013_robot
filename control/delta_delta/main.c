#define PART_LM4F120B2QR
#include <inc/hw_types.h>
#include <driverlib/fpu.h>
#include <driverlib/sysctl.h>
#include <stdint.h>
#include <stdlib.h>

#include "parts/movement.h"
#include "parts/control.h"
#include "parts/turret.h"
#include "platform/pwm.h"
#include "platform/uartbt.h"

#define CONTROL_DO_STEP_OR_DIE(a,b,c) \
if (!control_do_step(a, b, c)) {      \
	control_stop_1();                 \
	control_stop_2();                 \
	while(1);                         \
}

void main(void) {
	//Config système
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
	FPUEnable();
	FPULazyStackingEnable();

    //configure_uart_bt();

	//Config propulsion
    config_pwms();
    motor_set_pwm_limits_all(40);
    control_init();

    //Init
    control_go_to_origin_1();
    control_go_to_origin_2();
	while(!control_reached_1 || !control_reached_2) {
		SysCtlDelay(1000);
	}
	control_disable_1();
	control_stop_1();
	control_disable_2();
	control_stop_2();

	//Démarrage propulsion
	motor_set_pwm_limits_all(99);
	control_enable_1();
    control_enable_2();

	control_do_lines(
		0, 0,
		0, 0,
		170, BASE_Z-DZ,
		0, 0,
		0, 0,
		170, BASE_Z,
		STEPS
	);

	//Init et démarrage tourelle
    turret_init();
	turret_set_angle(0);

	//Script
	control_start_steps(0, 30, 0);
	while(1) {
		CONTROL_DO_STEP_OR_DIE(0, 90, 30);
		if (movement_is_paused()) {
			control_stop_steps(90, 30, 1);
			while(movement_is_paused())
				movement_stay_put(0, 0, BASE_Z);
			control_start_steps(90, 30, 1);
		}

		turret_set_angle(90);

		CONTROL_DO_STEP_OR_DIE(90, 90, 30);
		if (movement_is_paused()) {
			control_stop_steps(90, 30, 1);
			while(movement_is_paused())
				movement_stay_put(0, 0, BASE_Z);
			control_start_steps(90, 30, 1);
		}

		CONTROL_DO_STEP_OR_DIE(90, 180, 30);
		if (movement_is_paused()) {
			control_stop_steps(180, 30, 1);
			while(movement_is_paused())
				movement_stay_put(0, 0, BASE_Z);
			control_start_steps(180, 30, 1);
		}

		turret_set_angle(180);

		CONTROL_DO_STEP_OR_DIE(180, 180, 30);
		if (movement_is_paused()) {
			control_stop_steps(180, 30, 1);
			while(movement_is_paused())
				movement_stay_put(0, 0, BASE_Z);
			control_start_steps(180, 30, 1);
		}

		CONTROL_DO_STEP_OR_DIE(180, 270, 30);
		if (movement_is_paused()) {
			control_stop_steps(270, 30, 1);
			while(movement_is_paused())
				movement_stay_put(0, 0, BASE_Z);
			control_start_steps(270, 30, 1);
		}

		turret_set_angle(270);

		CONTROL_DO_STEP_OR_DIE(270, 270, 30);
		if (movement_is_paused()) {
			control_stop_steps(270, 30, 1);
			while(movement_is_paused())
				movement_stay_put(0, 0, BASE_Z);
			control_start_steps(270, 30, 1);
		}

		CONTROL_DO_STEP_OR_DIE(270, 0, 30);
		if (movement_is_paused()) {
			control_stop_steps(0, 30, 1);
			while(movement_is_paused())
				movement_stay_put(0, 0, BASE_Z);
			control_start_steps(0, 30, 1);
		}

		turret_set_angle(0);

		CONTROL_DO_STEP_OR_DIE(0, 0, 30);
		if (movement_is_paused()) {
			control_stop_steps(0, 30, 1);
			while(movement_is_paused())
				movement_stay_put(0, 0, BASE_Z);
			control_start_steps(0, 30, 1);
		}
	}

}
