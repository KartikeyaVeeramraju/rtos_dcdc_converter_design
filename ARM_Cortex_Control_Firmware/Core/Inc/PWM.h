/*
 * PWM.h
 *
 *  Created on: Apr 10, 2020
 *      Author: kartikeya
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#define V3_3 1
#define V2_5 2

void start_pwm(uint8_t channel);
void stop_pwm(uint8_t channel);

void update_duty(uint8_t channel, uint16_t duty);

#endif /* INC_PWM_H_ */
