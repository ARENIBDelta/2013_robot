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
#include "parts/init.h"
#include "parts/actuators.h"

#define CONTROL_DO_STEP_OR_DIE_OR_PAUSE(a,b,c) \
turret_set_angle(a);                      \
if (!control_do_step(a, b, c)) {          \
	control_stop_1();                     \
	control_stop_2();                     \
	while(1);                             \
}                                         \
else {                                    \
	if (movement_is_paused()) {           \
		control_stop_steps(b, c, 1);      \
		while(movement_is_paused())       \
		movement_stay_put(0, 0, BASE_Z);  \
		control_start_steps2(b, c);       \
	}                                     \
}                                         \

unsigned char is_blue = 0;
const unsigned char do_init = 1;

void main(void) {
	//Config système
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
	FPUEnable();
	FPULazyStackingEnable();

    //configure_uart_bt();

	actuators_servos_init(); //A demarrer avant la tourelle car timer partagé
	actuators_servo_lower(0);
	actuators_servo_lower(1);

	actuator_pwm_init();
	actuator_pwm_zero();

	//Config propulsion
    config_pwms();
    motor_set_pwm_limits_all(40);
    control_init();

	if (do_init) {
		init_init();
		while(!init_is_ready());
		SysCtlDelay(SysCtlClockGet()/10);
		is_blue = init_is_blue();
	}

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

	if (do_init) {
		while(!init_is_ready());
		SysCtlDelay(SysCtlClockGet()/100);
	}

	//Démarrage propulsion
	motor_set_pwm_limits_all(30);
	control_enable_1();
    control_enable_2();

	control_do_lines(
		0, 0,
		0, 0,
		170, BASE_Z,
		0, 0,
		0, 0,
		170, BASE_Z,
		STEPS
	);

	actuators_servos_init(); //A demarrer avant la tourelle car timer partagé
	actuators_servo_lower(0);
	actuators_servo_lower(1);

	//Init et démarrage tourelle
    turret_init();
	turret_set_angle(90);

	if (do_init) {
		while(!init_tirette_go()) {
			movement_stay_put(0, 0, BASE_Z);
		}
	}

	motor_set_pwm_limits_all(99);

	control_start_steps2(180, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(180, 90, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 0, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, 25);

	control_do_half_step(0, 0, 25);

	actuators_servo_raise(0);
	actuators_servo_raise(1);

	float angle = 0 * M_PI  / 180;
	unsigned int count = 200;
	while(count--) {
		movement_stay_put(25*cos(angle), 25*sin(angle), BASE_Z);
	}

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(180, 90, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);
	actuators_servo_lower(0);
	actuators_servo_lower(1);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 0, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, 25);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, 25);

	control_do_half_step(0, 0, 25);

	actuators_servo_raise(0);
	actuators_servo_raise(1);

	angle = 0 * M_PI  / 180;
	count = 200;
	while(count--) {
		movement_stay_put(25*cos(angle), 25*sin(angle), BASE_Z);
	}

	control_stop_steps(180, 25, 1);
	actuators_servo_lower(0);
	actuators_servo_lower(1);
	while(1)
		movement_stay_put(0, 0, BASE_Z);

}
