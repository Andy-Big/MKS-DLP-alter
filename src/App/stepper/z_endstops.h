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



#define ENDSTOPS_ENABLED  zEndstops.enabled









enum EndstopEnum : char {
  Z_MIN,
  Z_MAX
};

class Endstops {
  public:
      typedef uint8_t esbits_t;

  private:
    static bool enabled, enabled_globally;
    static esbits_t live_state;
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
    __INLINE static bool abort_enabled()
	{
      return enabled;
    }

    static inline bool global_enabled()
	{
		return enabled_globally;
	}

    /**
     * Periodic call to poll endstops if required. Called from temperature ISR
     */
    static void poll();

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
    __INLINE static esbits_t state()
	{
      return live_state;
    }

    /**
     * Report endstop hits to serial. Called from loop().
     */
    static void event_handler();

    // Enable / disable endstop checking globally
    static void enable_globally(const bool onoff=true);

    // Enable / disable endstop checking
    static void enable(const bool onoff=true);

    // Disable / Enable endstops based on ENSTOPS_ONLY_FOR_HOMING and global enable
    static void not_homing();

    __INLINE static void validate_homing_move()
	{
		hit_on_purpose();
	}

    // Clear endstops (i.e., they were hit intentionally) to suppress the report
    __INLINE static void hit_on_purpose()
	{
		hit_state = 0;
	}

    static void resync();

};

extern Endstops endstops;

