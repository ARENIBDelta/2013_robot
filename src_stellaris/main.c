#define PART_LM4F120B2QR
#include <inc/hw_types.h>
#include <driverlib/fpu.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <stdint.h>
#include <stdlib.h>

#define REMOTE_CONTROL  0
#define STEP_LENGTH     20

#include "parts/movement.h"
#include "parts/control.h"
#include "parts/turret.h"
#include "platform/pwm.h"
#include "platform/uartbt.h"
#include "parts/init.h"
#include "parts/actuators.h"
#include "parts/match_timer.h"

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

inline unsigned char control_do_step_or_die_or_pause_with_timeout(float angle_src, float angle_dst, char length) {
	turret_set_angle(angle_src);
	if (!control_do_step(angle_src, angle_dst, length)) {
		control_stop_1();
		control_stop_2();
		while(1);
	}
	else {
		if (movement_is_paused()) {
			unsigned char count = 255;
			control_stop_steps(angle_dst, length, 1);
			while(movement_is_paused() && count) {
				movement_stay_put(0, 0, BASE_Z);
				//SysCtlDelay(SysCtlClockGet()/100);
				count--;
			}
			if (!count)
				return 0;
			control_start_steps2(angle_dst, length);
		}
	}
	return 1;
}

unsigned char is_blue = 0;
const unsigned char do_init = 1;

#if REMOTE_CONTROL
unsigned char  remote_state = 0;
unsigned short current_angle1 = 0;
unsigned short current_angle2 = 0;
unsigned short go_angle = 0;
unsigned short new_order = 0;
#endif

#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>

extern unsigned long __STACK_END;
void reset_stack(unsigned long stack_addr);

void main_match_ended(void) {
	reset_stack((unsigned int) &__STACK_END);

    motor_set_pwm_limits_all(25);

    while(1) {
    	movement_stay_put(0, 0, BASE_Z);
    }
}

unsigned char turret_watch(unsigned short robot_angle, unsigned short turret_angle) {
	turret_set_angle(turret_angle);
	float angle = robot_angle * M_PI  / 180;
	unsigned int count = 255;
	while(count--) {
		movement_stay_put(STEP_LENGTH*cos(angle), STEP_LENGTH*sin(angle), BASE_Z);
	}
	return movement_is_paused();
}

void go_forward(unsigned char steps) {
	unsigned char i;
	unsigned char steps_done = 0;
	unsigned char trans_pos = 0;
	unsigned char state = 0;

start:
	switch(state) {
	case 10:
		control_start_steps2(180, STEP_LENGTH);
		state = 11;
		break;
	case 11:
		if (!control_do_step_or_die_or_pause_with_timeout(180, 180, STEP_LENGTH)) {
			state = 20;
		}
		trans_pos++;
		if (trans_pos >= 5)
			state = 12;
		break;
	case 12:
		if (turret_watch(0, 90)) {
			control_do_step_or_die_or_pause_with_timeout(180, 0, STEP_LENGTH);
			state = 21;
			break;
		}
		else {
			control_do_step_or_die_or_pause_with_timeout(180, 90, STEP_LENGTH);
		}
		state = 0;
		break;
	case 20:
		control_start_steps2(0, STEP_LENGTH);
		state = 21;
		break;
	case 21:
		if (!control_do_step_or_die_or_pause_with_timeout(0, 0, STEP_LENGTH)) {
			state = 10;
		}
		trans_pos--;
		if (!trans_pos)
			state = 22;
		break;
	case 22:
		if (turret_watch(180, 90)) {
			control_do_step_or_die_or_pause_with_timeout(0, 180, STEP_LENGTH);
			state = 11;
			break;
		}
		else {
			control_do_step_or_die_or_pause_with_timeout(0, 90, STEP_LENGTH);
		}
		state = 0;
		break;
	case 0:
	default:
		if (!control_do_step_or_die_or_pause_with_timeout(90, 90, STEP_LENGTH)) {
			if (!trans_pos) {
				state = 10;
			}
			else {
				state = 20;
			}
		}
		steps_done++;
		break;
	}
	if (steps_done == steps) {
		return;
	}
	else {
		goto start;
	}
}

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

#if REMOTE_CONTROL
	configure_uart_bt();
#endif

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

	//match_timer_init();
	//match_timer_start();

	motor_set_pwm_limits_all(99);

#if REMOTE_CONTROL

	while(1) {
		switch(remote_state) {
		case 3:
			control_stop_steps(current_angle1, STEP_LENGTH, 1);
			remote_state = 0;
			new_order = 0;
			break;
		case 2:
			CONTROL_DO_STEP_OR_DIE_OR_PAUSE(current_angle1, current_angle2, STEP_LENGTH);
			current_angle1 = current_angle2;
			IntMasterDisable();
			if (new_order) {
				current_angle2 = go_angle;
				remote_state = 2;
			}
			else {
				remote_state = 3;
			}
			IntMasterEnable();
			new_order = 0;
			break;
		case 1:
			control_start_steps2(current_angle1, STEP_LENGTH);
			new_order = 0;
			remote_state = 2;
			break;
		default:
		case 0:
			movement_stay_put(0, 0, BASE_Z);
			IntMasterDisable();
			if (new_order) {
				current_angle1 = go_angle;
				current_angle2 = go_angle;
				remote_state = 1;
			}
			IntMasterEnable();
			new_order = 0;
			break;
		}

		new_order = 0;
	}

#else

	control_start_steps2(135, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(135, 90, STEP_LENGTH);

	go_forward(7);

	control_stop_steps(90, STEP_LENGTH, 1);

	while(1) {
		movement_stay_put(0, 0, BASE_Z);
	}

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 0, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, STEP_LENGTH);

	control_do_half_step(0, 0, STEP_LENGTH);

	actuators_servo_raise(0);
	actuators_servo_raise(1);

	float angle = 0 * M_PI  / 180;
	unsigned int count = 200;
	while(count--) {
		movement_stay_put(STEP_LENGTH*cos(angle), STEP_LENGTH*sin(angle), BASE_Z);
	}

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(180, 90, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);
	actuators_servo_lower(0);
	actuators_servo_lower(1);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);
	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 90, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(90, 0, STEP_LENGTH);

	CONTROL_DO_STEP_OR_DIE_OR_PAUSE(0, 0, STEP_LENGTH);

	control_do_half_step(0, 0, STEP_LENGTH);

	actuators_servo_raise(0);
	actuators_servo_raise(1);

	angle = 0 * M_PI  / 180;
	count = 200;
	while(count--) {
		movement_stay_put(STEP_LENGTH*cos(angle), STEP_LENGTH*sin(angle), BASE_Z);
	}

	control_stop_steps(180, STEP_LENGTH, 1);
	actuators_servo_lower(0);
	actuators_servo_lower(1);
	while(1)
		movement_stay_put(0, 0, BASE_Z);

#endif

}
