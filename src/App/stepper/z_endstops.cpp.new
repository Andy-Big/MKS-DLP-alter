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

// private:

bool Endstops::enabled, Endstops::enabled_globally; // Initialized by settings.load()
volatile uint8_t Endstops::hit_state;

Endstops::esbits_t Endstops::live_state = 0;

// Initialized by settings.load()

/**
 * Class and Instance Methods
 */

void Endstops::init()
{
} // Endstops::init

// Called at ~1KHz from Temperature ISR: Poll endstop state if required
void Endstops::poll()
{
    update();
}

void Endstops::enable_globally(const bool onoff)
{
  enabled_globally = enabled = onoff;
  resync();
}

// Enable / disable endstop checking
void Endstops::enable(const bool onoff)
{
  enabled = onoff;
  resync();
}

// Disable / Enable endstops based on ENSTOPS_ONLY_FOR_HOMING and global enable
void Endstops::not_homing()
{
  enabled = enabled_globally;
}

// Get the stable endstop states when enabled
void Endstops::resync() {
  if (!abort_enabled()) return;     // If endstops/probes are disabled the loop below can hang

  // Wait for Temperature ISR to run at least once (runs at 1KHz)
  update();
}

// The following routines are called from an ISR context. It could be the temperature ISR, the
// endstop ISR or the Stepper ISR.

// Check endstops - Could be called from Temperature ISR!
void Endstops::update() {

  /**
   * Check and update endstops
   */
      __SET_BIT(live_state, Z_MIN, (HAL_GPIO_ReadPin(Z_MIN_GPIO_Port, Z_MIN_Pin) != cfgzMotor.z_min_endstop_inverting));
      __SET_BIT(live_state, Z_MAX, (HAL_GPIO_ReadPin(Z_MAX_GPIO_Port, Z_MAX_Pin) != cfgzMotor.z_max_endstop_inverting));

  // Signal, after validation, if an endstop limit is pressed or not

  if (zStepper.axis_is_moving()) {
    if (zStepper.motor_direction())
	{ // Z -direction. Gantry down, bed up.

//        PROCESS_ENDSTOP_Z(MIN);
      if (TEST(state(), Z_MIN))
	  {
        SBI(hit_state, Z_MIN);
        zPlanner.endstop_triggered();
      }
    }
    else { // Z +direction. Gantry up, bed down.
      if (TEST(state(), Z_MAX))
	  {
        SBI(hit_state, Z_MAX);
        zPlanner.endstop_triggered();
      }
    }
  }
} // Endstops::update()

