/*
 * control.h
 *
 *  Created on: Mar 10, 2013
 *      Author: ZWM
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#define CONTROL_TIMER_FREQ 1000

void control_init(void);
void control_start(void);
void control_stop(void);
void control_go_to_origin(void);

void control(void);

#endif /* CONTROL_H_ */
