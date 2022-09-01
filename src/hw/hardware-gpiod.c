/*******************************************************************************
LibreLogic : a free PLC library
Copyright (C) 2022, Antonis K. (kalamara AT ceid DOT upatras DOT gr)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gpiod.h>
#include "data.h"
#include "instruction.h"
#include "rung.h"

#include "util.h"
#include "hardware.h"

int gpiod_config(void * conf)
{
    conf_gpiod_t c = (conf_gpiod_t )conf;
//    const char *chipname = "gpiochip0";
    struct gpiod_chip *chip;
    // Open GPIO chip
    chip = gpiod_chip_open_by_name(c->chipname);//hipname);
    if(chip){
        return PLC_OK;    
    }
    plc_log("Failed to open GPIOD chip"); 
    return PLC_ERR;
}

int gpiod_enable() /* Enable bus communication */
{
    return PLC_OK;
}

int gpiod_disable() /* Disable bus communication */
{

    return PLC_OK;
}

int gpiod_fetch()
{
    
    return 0;
}

int gpiod_flush()
{
    return 0;
}

void gpiod_dio_read(unsigned int n, BYTE* bit)
{	
    return;
}

void gpiod_dio_write(const unsigned char *buf, unsigned int n,  BYTE bit)
{	
    return;
}

void gpiod_dio_bitfield(const BYTE* mask, BYTE *bits)
{	
    return;
}

void gpiod_data_read(unsigned int index, uint64_t* value)
{
    return;
}


void gpiod_data_write(unsigned int index, uint64_t value)
{
    
    return; 
}

struct hardware Gpiod = {
    HW_GPIOD,
    0, //errorcode
    "GPIOD",
    gpiod_enable,// enable
    gpiod_disable, //disable
    gpiod_fetch, //fetch
    gpiod_flush, //flush
    gpiod_dio_read, //dio_read
    gpiod_dio_write, //dio_write
    gpiod_dio_bitfield, //dio_bitfield
    gpiod_data_read, //data_read
    gpiod_data_write, //data_write
    gpiod_config, //hw_config
};

