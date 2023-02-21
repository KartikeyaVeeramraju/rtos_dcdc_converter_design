/*
 * PWM.c
 *
 *  Created on: Apr 10, 2020
 *      Author: kartikeya
 */

#include "main.h"
#include "stm32f4xx_it.h"
#include "Manager.h"
#include "PWM.h"
//#include "stm32f4xx_hal_tim.h"

#define TIM1Base_ADDR (uint32_t)0x40010000
#define TIM8Base_ADDR (uint32_t)0x40010400

//Register Offsets
#define CR1 0x00
#define CR2 0x04
#define CCR4 0x40 //capture compare register channel 4
#define CCER 0x20
#define BDTR 0x44
#define CCR4 0x40

#define ENABLE_BIT 0x0001
#define UDIS_BIT (uint16_t)(1<<1)
#define CH4_OUT_BIT (uint16_t) (1<<12)
#define MASTER_OP_ENABLE_BIT (1<<15)



uint32_t *p; //pointer pointing to the address of registers


void start_pwm(uint8_t channel)
{
	if(channel == V3_3)
	{
		//latch onto the address of CR1 in Timer 1
		  //start the timer
		  p = (uint32_t)(TIM1Base_ADDR+CR1);
		  *p = (*p	|	ENABLE_BIT);
		  //enable CH4 compare
		  p = (uint32_t)(TIM1Base_ADDR+CCER);
		  *p = (*p	|	CH4_OUT_BIT);
		  //enable main output bit
		  p = (uint32_t)(TIM1Base_ADDR+BDTR);
		  *p = (*p	|	 MASTER_OP_ENABLE_BIT);
	}
	else if(channel == V2_5)
	{
		  //start the timer

		  p = (uint32_t)(TIM8Base_ADDR+CR1);
		  *p = (*p	|	ENABLE_BIT);
		  //enable CH4 compare
		  p = (uint32_t)(TIM8Base_ADDR+CCER);
		  *p = (*p	|	CH4_OUT_BIT);
		  //enable main output bit
		  p = (uint32_t)(TIM8Base_ADDR+BDTR);
		  *p = (*p	|	 MASTER_OP_ENABLE_BIT);
	}
}

void stop_pwm(uint8_t channel)
{
	if(channel == V3_3)
	{
		  //enable main output bit
		p = (uint32_t)(TIM1Base_ADDR+BDTR);
		*p = (*p	&	 (~MASTER_OP_ENABLE_BIT));
		p = TIM1Base_ADDR+CR1;
		*p = (*p	&	(~ENABLE_BIT));
	}
	else if(channel == V2_5)
	{
		//enable main output bit
		p = (uint32_t)(TIM8Base_ADDR+BDTR);
		*p = (*p	&	 (~MASTER_OP_ENABLE_BIT));
		p = TIM8Base_ADDR+CR1;
		*p = (*p	&	(~ENABLE_BIT));
	}
}

void update_duty(uint8_t channel, uint16_t duty)
{
		if(channel == V3_3)
		{
			p = (uint32_t)(TIM1Base_ADDR+CCR4);
			*p = duty;
		}
		if(channel == V2_5)
		{
			p = (uint32_t)(TIM8Base_ADDR+CCR4);
			*p = duty;
		}
}



