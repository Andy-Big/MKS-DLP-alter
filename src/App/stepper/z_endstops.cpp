/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * endstops.cpp - A singleton object to manage endstops
 */

#include "z_endstops.h"
#include "z_stepper.h"
#include "config.h"

// TEST_ENDSTOP: test the old and the current status of an endstop
#define TEST_ENDSTOP(ENDSTOP) (TEST(current_endstop_bits & old_endstop_bits, ENDSTOP))

Endstops		zEndstops;

// public:

bool Endstops::enabled, Endstops::enabled_globally; // Initialized by settings.load()
volatile char Endstops::endstop_hit_bits; // use X_MIN, Y_MIN, Z_MIN and Z_MIN_PROBE as BIT value

  uint8_t Endstops::current_endstop_bits = 0, Endstops::old_endstop_bits = 0;

/**
 * Class and Instance Methods
 */

void Endstops::init() {

//      SET_INPUT_PULLUP(Z_MIN_PIN);
//      SET_INPUT_PULLUP(Z_MAX_PIN);


} // Endstops::init

void Endstops::report_state()
{
  if (endstop_hit_bits)
  {
    hit_on_purpose();
  }
} // Endstops::report_state


// Check endstops - Called from ISR!
void Endstops::update() {

  // UPDATE_ENDSTOP_BIT: set the current endstop bits for an endstop to its status
//     #define UPDATE_ENDSTOP_BIT(AXIS, MINMAX) __SET_BIT(current_endstop_bits, Z_MINMAX), (READ(Z_MINMAX_PIN)) != Z_MINMAX_ENDSTOP_INVERTING))

  // COPY_BIT: copy the value of SRC_BIT to DST_BIT in DST
//	#define COPY_BIT(bits, COPY_BIT, BIT) __SET_BIT(bits, BIT, TEST(bits, COPY_BIT))

  /**
   * Define conditions for checking endstops
   */

  if (zStepper.current_block->steps > 0)
  {
    if (zStepper.motor_direction())
	{ // Z -direction. Gantry down, bed up.
       __SET_BIT(current_endstop_bits, Z_MIN, (HAL_GPIO_ReadPin(Z_MIN_GPIO_Port, Z_MIN_Pin) != cfgzMotor.z_min_endstop_inverting));
      if (TEST_ENDSTOP(Z_MIN) && zStepper.current_block->steps > 0)
	  {
        SBI(endstop_hit_bits, Z_MIN);
        zStepper.endstop_triggered();
      }

    }
    else
	{ // Z +direction. Gantry up, bed down.
       __SET_BIT(current_endstop_bits, Z_MAX, (HAL_GPIO_ReadPin(Z_MAX_GPIO_Port, Z_MAX_Pin) != cfgzMotor.z_max_endstop_inverting));
      if (TEST_ENDSTOP(Z_MAX) && zStepper.current_block->steps > 0)
	  {
        SBI(endstop_hit_bits, Z_MAX);
        zStepper.endstop_triggered();
      }
    }
  }

  old_endstop_bits = current_endstop_bits;

} // Endstops::update()
