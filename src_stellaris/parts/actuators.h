#ifndef ACTUATORS_H_
#define ACTUATORS_H_

void actuators_servos_init(void);
void actuators_servo_raise(unsigned char index);
void actuators_servo_lower(unsigned char index);
void actuator_pwm_init(void);
void actuator_pwm_zero(void);
void actuator_pwm_50percent(void);
void actuator_pwm_30percent(void);


#endif /* ACTUATORS_H_ */
