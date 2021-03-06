/*
 * display.c
 *
 *  Created on: Jan 31, 2022
 *      Author: Miroslav
 */


#include "display.h"

uint8_t seven_seg[] =
{ 0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84};

void display(){
	//GPIOC->ODR = seven_seg[1];
	HAL_TIM_Base_Start_IT(&htim1);
}

uint32_t counter = 0;
uint32_t quarter_minut = 10;
uint32_t quarter_sec = 0;

uint32_t possession_sec = 24;
uint32_t possession_desetinka = 0;

uint8_t time_left[4] = {0, 0, 0, 0};
uint8_t curr = 0;

extern uint8_t showTime;

#define ARR 9
uint32_t volatile overflow = 0;
uint16_t volatile start = 0;
uint16_t volatile end = 0;
uint32_t volatile time = 0;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM1){
		if(++counter == 100){
				counter = 0;
				if(quarter_sec == 0){
					quarter_sec = 59;
					if(quarter_minut == 0){
								quarter_minut = 10;
					}else --quarter_minut;
				}
				else --quarter_sec;
				if(possession_sec == 0){
					possession_sec = 24;
				}else --possession_sec;
			}

			if(possession_desetinka == 0){
				possession_desetinka = 99;
			}
			else --possession_desetinka;

			if(showTime == 0){
				time_left[2] = quarter_sec / 10;
				time_left[3] = quarter_sec % 10;
				time_left[0] = quarter_minut / 10;
				time_left[1] = quarter_minut % 10;
			}else {
				time_left[2] = possession_desetinka / 10;
				time_left[3] = possession_desetinka % 10;
				time_left[0] = possession_sec / 10;
				time_left[1] = possession_sec % 10;
			}

			curr = ( curr + 1 ) % 4;
			GPIOC->ODR &= ~0xF00;
			GPIOC->ODR &= ~0xFF;
			GPIOC->ODR |= seven_seg[time_left[curr]];
			GPIOC->ODR |= ( 0x1 ) << (8 + curr);
	}else if(htim->Instance == TIM3){
		++overflow;
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM3){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
			start = TIM3->CCR1;
			overflow = 0;
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
			end = TIM3->CCR2;
			time = (end + ( overflow * ( ARR + 1 ))) - start;
			if(time >= 2000){
				quarter_minut = 10;
				quarter_sec = 0;
				possession_desetinka = 0;
				possession_sec = 24;
			}
			overflow = 0;
		}
	}
}

