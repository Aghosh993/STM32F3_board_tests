/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Modified by Fernando Cortes <fermando.corcam@gmail.com>
 * modified by Guillermo Rivera <memogrg@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/flash.h>

/*
 A basic routine to adjust system clock settings to get SYSCLK
 to 64 MHz, and have AHB buses at 64 MHz, and APB Bus at 32 MHz (its max speed)
 Copied from:
 https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f3/stm32f3-discovery/adc/adc.c
 */

static void set_system_clock(void)
{
	rcc_clock_setup_hsi(&hsi_8mhz[CLOCK_64MHZ]);
}

/*
	Sets up Timer 1 to output a 1 kHz PWM signal on PA8 and PA9.
	Timer is set to up-counting, with output staying ON from timer
	counter reset at 0 to the counter reaching the OC (Output Compare)
	value. The output is then switched off until next reset of the timer
	counter.
 */

static void setup_timer1_pwm(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_TIM1);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP,
	                    GPIO_OSPEED_50MHZ, GPIO8 | GPIO9);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);
	gpio_set_af(GPIOA, GPIO_AF6, GPIO8 | GPIO9);

	rcc_periph_clock_enable(RCC_TIM1);
	timer_reset(TIM1);
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
	           TIM_CR1_DIR_UP);
	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM1);
	timer_enable_oc_output(TIM1, TIM_OC1);
	timer_enable_oc_output(TIM1, TIM_OC2);
	timer_enable_break_main_output(TIM1);

	timer_set_oc_value(TIM1, TIM_OC1, 16000);
	timer_set_oc_value(TIM1, TIM_OC2, 24000);

	timer_set_period(TIM1, 32000);

	timer_set_counter(TIM1,0U);
	timer_enable_counter(TIM1);
}

static void setup_timer2_pwm(void)
{
	// rcc_periph_clock_enable(RCC_GPIOA);
	// gpio_set_output_options(GPIOA, GPIO_OTYPE_PP,
	//                     GPIO_OSPEED_50MHZ, GPIO1 | GPIO2);
	// gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO1 | GPIO2);
	// gpio_set_af(GPIOA, GPIO_AF1, GPIO1 | GPIO2);

	// timer_reset(TIM2);
	// rcc_periph_clock_enable(RCC_TIM2);

	// timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
	//            TIM_CR1_DIR_UP);
	// timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_PWM1);
	// timer_set_oc_mode(TIM2, TIM_OC3, TIM_OCM_PWM1);

	// timer_set_oc_value(TIM2, TIM_OC2, 16000);
	// timer_set_oc_value(TIM2, TIM_OC3, 24000);
	// timer_set_period(TIM2, 32000);

	// timer_enable_preload(TIM2);
	// timer_enable_oc_preload(TIM2, TIM_OC2);
	// timer_enable_oc_preload(TIM2, TIM_OC3);
	// timer_set_oc_slow_mode(TIM2, TIM_OC2);
	// timer_set_oc_slow_mode(TIM2, TIM_OC3);

	// timer_enable_counter(TIM2);
	// timer_enable_oc_output(TIM2, TIM_OC2);
	// timer_enable_oc_output(TIM2, TIM_OC3);

	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP,
	                    GPIO_OSPEED_50MHZ, GPIO1 | GPIO2);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO1 | GPIO2);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO1 | GPIO2);

	timer_reset(TIM2);
	rcc_periph_clock_enable(RCC_TIM2);

	timer_continuous_mode(TIM2);

	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
	           TIM_CR1_DIR_UP);
	timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM2, TIM_OC3, TIM_OCM_PWM1);

	timer_set_period(TIM2, 64000);
	timer_set_prescaler(TIM2, 1);

	timer_enable_preload(TIM2);

	timer_set_oc_polarity_high(TIM2, TIM_OC2);
	timer_set_oc_polarity_high(TIM2, TIM_OC3);
	timer_enable_oc_output(TIM2, TIM_OC2);
	timer_enable_oc_output(TIM2, TIM_OC3);

	timer_generate_event(TIM2, TIM_EGR_UG);

	timer_enable_counter(TIM2);
}

static void setup_timer3_pwm(void)
{
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_output_options(GPIOC, GPIO_OTYPE_PP,
	                    GPIO_OSPEED_50MHZ, GPIO8 | GPIO9);
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);
	gpio_set_af(GPIOC, GPIO_AF2, GPIO8 | GPIO9);

	timer_reset(TIM3);
	rcc_periph_clock_enable(RCC_TIM3);

	timer_continuous_mode(TIM3);

	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
	           TIM_CR1_DIR_UP);
	timer_set_oc_mode(TIM3, TIM_OC3, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM3, TIM_OC4, TIM_OCM_PWM1);

	timer_set_period(TIM3, 64000);
	timer_set_prescaler(TIM3, 1);

	timer_enable_preload(TIM3);

	timer_set_oc_polarity_high(TIM3, TIM_OC3);
	timer_set_oc_polarity_high(TIM3, TIM_OC4);
	timer_enable_oc_output(TIM3, TIM_OC3);
	timer_enable_oc_output(TIM3, TIM_OC4);

	timer_generate_event(TIM3, TIM_EGR_UG);

	timer_enable_counter(TIM3);
}

static void timer4_master_setup(void)
{
	timer_reset(TIM4);
	rcc_periph_clock_enable(RCC_TIM4);

	timer_continuous_mode(TIM4);

	timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
	           TIM_CR1_DIR_UP);
	timer_set_period(TIM4, 1);
	timer_enable_update_event(TIM4);
	timer_set_master_mode(TIM4, TIM_CR2_MMS_UPDATE);
	timer_enable_counter(TIM4);
}

static void setup_sync_timer2_timer3_pwm(void)
{
	timer4_master_setup();

	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP,
	                    GPIO_OSPEED_50MHZ, GPIO1 | GPIO2);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO1 | GPIO2);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO1 | GPIO2);

	timer_reset(TIM2);
	rcc_periph_clock_enable(RCC_TIM2);

	timer_continuous_mode(TIM2);

	// timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
	//            TIM_CR1_DIR_UP);
	timer_slave_set_mode(TIM2, TIM_SMCR_SMS_ECM1);
	timer_slave_set_trigger(TIM2, TIM_SMCR_TS_ITR0);
	timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM2, TIM_OC3, TIM_OCM_PWM1);

	timer_set_period(TIM2, 64000);
	timer_set_prescaler(TIM2, 1);

	timer_enable_preload(TIM2);

	timer_set_oc_polarity_high(TIM2, TIM_OC2);
	timer_set_oc_polarity_high(TIM2, TIM_OC3);
	timer_enable_oc_output(TIM2, TIM_OC2);
	timer_enable_oc_output(TIM2, TIM_OC3);

	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_output_options(GPIOC, GPIO_OTYPE_PP,
	                    GPIO_OSPEED_50MHZ, GPIO8 | GPIO9);
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);
	gpio_set_af(GPIOC, GPIO_AF2, GPIO8 | GPIO9);

	timer_reset(TIM3);
	rcc_periph_clock_enable(RCC_TIM3);

	timer_continuous_mode(TIM3);

	// timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
	//            TIM_CR1_DIR_UP);
	timer_slave_set_mode(TIM3, TIM_SMCR_SMS_ECM1);
	timer_slave_set_trigger(TIM3, TIM_SMCR_TS_ITR0);
	timer_set_oc_mode(TIM3, TIM_OC3, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM3, TIM_OC4, TIM_OCM_PWM1);

	timer_set_period(TIM3, 64000);
	timer_set_prescaler(TIM3, 1);

	timer_enable_preload(TIM3);

	timer_set_oc_polarity_high(TIM3, TIM_OC3);
	timer_set_oc_polarity_high(TIM3, TIM_OC4);
	timer_enable_oc_output(TIM3, TIM_OC3);
	timer_enable_oc_output(TIM3, TIM_OC4);

	timer_generate_event(TIM2, TIM_EGR_UG);
	timer_generate_event(TIM3, TIM_EGR_UG);

	timer_enable_counter(TIM2);
	timer_enable_counter(TIM3);
}


int main(void)
{
	int i = 0;
	set_system_clock();
	// setup_timer1_pwm();
	setup_timer2_pwm();
	setup_timer3_pwm();
	// setup_sync_timer2_timer3_pwm();
	int duty_to_set = 0;

	while (1) 
	{
		if(timer_get_flag(TIM3, TIM_SR_UIF))
		{
			timer_clear_flag(TIM3, TIM_SR_UIF);

			timer_set_oc_value(TIM3, TIM_OC3, 48000);
			timer_set_oc_value(TIM3, TIM_OC4, 32000);
		}

		if(timer_get_flag(TIM2, TIM_SR_UIF))
		{
			timer_clear_flag(TIM2, TIM_SR_UIF);

			timer_set_oc_value(TIM2, TIM_OC2, 48000);
			timer_set_oc_value(TIM2, TIM_OC3, 32000);
		}
		++duty_to_set;
		if(duty_to_set >= 64000)
		{
			duty_to_set = 0;
		}
		for(i=0;i<2000;++i)
		{
			__asm__("nop");
		}

	}

	return 0;
}
