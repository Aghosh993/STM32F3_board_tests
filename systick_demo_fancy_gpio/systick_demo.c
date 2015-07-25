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
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

/*
	Shamelessly stolen from I2C example in libopencm3-examples,
	but sharing is caring, right? Right? Okay.
 */

#define LBLUE GPIOE, GPIO8
#define LRED GPIOE, GPIO9
#define LORANGE GPIOE, GPIO10
#define LGREEN GPIOE, GPIO11
#define LBLUE2 GPIOE, GPIO12
#define LRED2 GPIOE, GPIO13
#define LORANGE2 GPIOE, GPIO14
#define LGREEN2 GPIOE, GPIO15

static int led_counter;

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

static void gpio_setup(void)
{
	/* Enable GPIOE clock. */
	rcc_periph_clock_enable(RCC_GPIOE);

	/* Set GPIO12 (in GPIO port E) to 'output push-pull'. */
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
}

/*
	Set up 1 kHz Systick interrupt as system-wide time base.

	Adapted from: 
	https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f1/obldc/systick/systick.c
 */

static void systick_setup(void)
{
	/*
		64 MHz SYSCLK /8 = 8 MHz Systick clock
	 */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

	/*
		Overflow at 8 MHz / (7999+1U) = 1 kHz:
	 */
	systick_set_reload(7999);

	systick_interrupt_enable();

	/* Start counting. */
	systick_counter_enable();
}

/*
	Overrides the WEAK declaration of systick_tick_handler() in nvic.h:
 */

void sys_tick_handler(void)
{
	++led_counter;
	if(led_counter == 500)
	{
		led_counter = 0;
		gpio_toggle(GPIOE, GPIO12);	/* LED on/off */
	}
}

/*
	At the moment, libopencm3 appears not to have macros/functions
	explicitly defined to globally disable/enable interrupts.
	Thus, the following two wrapper functions for some assembler are
	required to achieve this.

	Taken from:
	http://permalink.gmane.org/gmane.comp.lib.libopencm3/29
 */

/*
	Globally ENABLE interrupts:
 */

static inline void reset_primask(void)
{
	asm volatile ("cpsie i");
}

/*
	Globally DISABLE interrupts:
 */

static inline void set_primask(void)
{
	asm volatile ("cpsid i");
}

int main(void)
{
	set_primask();
	led_counter = 0;

	set_system_clock();
	gpio_setup();
	systick_setup();

	reset_primask();

	/* Do nothing, Systick ISR will toggle I/O lines as necessary*/
	while (1);

	return 0;
}
