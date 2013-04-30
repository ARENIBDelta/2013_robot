#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

#define tics_us SysCtlClockGet() / 1000000
#define tics_100ns SysCtlClockGet() / 10000000
#define tics_ms SysCtlClockGet() / 1000
#define default_pulse 0
#define PULSE_US 1
#define PULSE_100NS 2
#define PULSE_MS 3

void config_pwms(void);
void set_pwm_width(
		uint32_t timer_base, uint32_t timer,
		uint32_t pulse_duration, uint8_t pulse_unit
);

#endif /* PWM_H_ */
