/*
 * Copyright 2022 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/librelogic-esp32 *
 *
 * This is based on other projects:
 *    Free PLC logic library  (https://github.com/kalamara/librelogic)
 *    Others (see individual files)
 *
 *    please contact their authors for more information.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <stddef.h>
#include <stdint.h>

#include "plc_iface.h"
#include "util.h"
#ifdef HW_PORT_INCLUDE
#include "plc_port.h"
#endif
#include "hardware-dry.h"

hardware_t plc_get_hardware(int type) {
    switch (type) {
        case HW_GPIOD:
#ifdef GPIOD
            #include "hardware-gpiod.h"
            plc_log("Using GPIOD");
            return &Gpiod;
#else
            return NULL;
#endif
        case HW_COMEDI:
#ifdef COMEDI
            #include "hardware-comedi.h"
            return &Comedi;
#else
            return NULL;
#endif
        case HW_USPACE:
#ifdef USPACE
            #include "hardware-uspace.h"
            return &Uspace;
#else
            return NULL;
#endif

        case HW_SIM:
#ifdef SIM
            #include "hardware-sim.h"
            return &Sim;
#else
            return NULL;
#endif


        case HW_PORT:
#ifdef HW_PORT_INCLUDE
            #include HW_PORT_INCLUDE
            return HW_PORT_RETURN;
#else
            return NULL;
#endif

        default:
            return &Dry;
    }
}
