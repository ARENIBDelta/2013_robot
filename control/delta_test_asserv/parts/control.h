/*
 * control.h
 *
 *  Created on: Mar 10, 2013
 *      Author: ZWM
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#define CONTROL_TIMER_FREQ 100

void control_init(void);
void control_enable_1(void);
void control_enable_2(void);
void control_disable_1(void);
void control_disable_2(void);
void motor_set_pwm_limits_all(char duty_cycle_max);
void control_go_to_origin_1(void);
void control_go_to_origin_2(void);
void control_set_goal_1(unsigned short alpha, unsigned short beta, unsigned short gamma);
void control_set_goal_2(unsigned short alpha, unsigned short beta, unsigned short gamma);
void control_stop_1(void);
void control_stop_2(void);

void control(void);

#endif /* CONTROL_H_ */
