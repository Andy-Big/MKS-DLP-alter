/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * endstops.cpp - A singleton object to manage endstops
 */

#include "z_endstops.h"
#include "z_planner.h"
#include "z_stepper.h"
#include "config.h"

	 
	 
	 
	 
Endstops			zEndstops;





// interrupt routine
void	EndstopsInterrupt()
{
	zEndstops.update();
}






// private:

bool				Endstops::enabled = 1; // Initialized by settings.load()
volatile uint8_t	Endstops::hit_state = 0;

uint8_t				Endstops::live_state = 0;

// Initialized by settings.load()

/**
 * Class and Instance Methods
 */

void Endstops::init()
{
	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(ZE_MIN_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ZE_MIN_EXTI_IRQn);

	HAL_NVIC_SetPriority(ZE_MAX_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ZE_MAX_EXTI_IRQn);

	enabled = 1;
} // Endstops::init


// Check endstops - Could be called from Temperature ISR!
void Endstops::update()
{
	if (enabled)
	{
		__SET_BIT(live_state, Z_MIN, (HAL_GPIO_ReadPin(ZE_MIN_GPIO_Port, ZE_MIN_Pin) != cfgzMotor.min_endstop_inverting));
		__SET_BIT(live_state, Z_MAX, (HAL_GPIO_ReadPin(ZE_MAX_GPIO_Port, ZE_MAX_Pin) != cfgzMotor.max_endstop_inverting));

		// Signal, after validation, if an endstop limit is pressed or not

		if (zStepper.axis_is_moving())
		{
			if (zStepper.motor_direction())
			{ // Z -direction. Gantry down, bed up.

				//        PROCESS_ENDSTOP_Z(MIN);
				if (TEST(state(), Z_MIN))
				{
					SBI(hit_state, Z_MIN);
					zPlanner.endstop_triggered();
				}
			}
			else
			{ // Z +direction. Gantry up, bed down.
				if (TEST(state(), Z_MAX))
				{
					SBI(hit_state, Z_MAX);
					zPlanner.endstop_triggered();
				}
			}
		}
	}
} // Endstops::update()

