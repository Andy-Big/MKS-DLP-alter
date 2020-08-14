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
#pragma once

/**
 * endstops.h - manages endstops
 */

#include "main.h"







class Endstops;

extern Endstops			zEndstops;



#define ENDSTOPS_ENABLED	zEndstops.enabled

#define ZE_MAX_Pin			GPIO_PIN_2
#define ZE_MAX_GPIO_Port	GPIOC
#define ZE_MAX_EXTI_IRQn	EXTI2_IRQn

#define ZE_MIN_Pin			GPIO_PIN_0
#define ZE_MIN_GPIO_Port	GPIOC
#define ZE_MIN_EXTI_IRQn	EXTI0_IRQn






#ifdef __cplusplus
extern "C"
{
#endif
void	EndstopsInterrupt();
#ifdef __cplusplus
}
#endif




enum EndstopEnum : uint8_t {
  Z_MIN = 0x01,
  Z_MAX = 0x02
};

class Endstops {
  public:
      typedef uint8_t esbits_t;

  private:
    static bool enabled;
    static uint8_t live_state;
    static volatile uint8_t hit_state;      // Use X_MIN, Y_MIN, Z_MIN and Z_MIN_PROBE as BIT index

  public:
    Endstops() {};

    /**
     * Initialize the endstop pins
     */
    static void init();

    /**
     * Are endstops or the probe set to abort the move?
     */
    __INLINE static bool is_enabled()
	{
      return enabled;
    }

    __INLINE static void enable()
	{
      enabled = 1;
	  update();
    }

    __INLINE static void disable()
	{
      enabled = 0;
	  live_state = 0;
	  hit_state = 0;
    }

    /**
     * Update endstops bits from the pins. Apply filtering to get a verified state.
     * If abort_enabled() and moving towards a triggered switch, abort the current move.
     * Called from ISR contexts.
     */
    static void update();

    /**
     * Get Endstop hit state.
     */
    __INLINE static uint8_t trigger_state()
	{
		return hit_state;
	}

    /**
     * Get current endstops state
     */
    __INLINE static uint8_t state()
	{
      return live_state;
    }

    /**
     * Report endstop hits to serial. Called from loop().
     */
    static void event_handler();

};

extern Endstops endstops;

