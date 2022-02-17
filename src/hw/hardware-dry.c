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


#include "data.h"
#include "instruction.h"
#include "rung.h"

#include "util.h"

#include "config.h"
#include "schema.h"

#include "hardware.h"

int dry_config(const config_t conf)
{
    return PLC_OK;    
}

int dry_enable() /* Enable bus communication */
{
    return PLC_OK;
}

int dry_disable() /* Disable bus communication */
{

    return PLC_OK;
}

int dry_fetch()
{
    
    return 0;
}

int dry_flush()
{
    return 0;
}

void dry_dio_read(unsigned int n, BYTE* bit)
{	
    return;
}

void dry_dio_write(const unsigned char *buf, unsigned int n,  BYTE bit)
{	
    return;
}

void dry_dio_bitfield(const BYTE* mask, BYTE *bits)
{	
    return;
}

void dry_data_read(unsigned int index, uint64_t* value)
{
    return;
}


void dry_data_write(unsigned int index, uint64_t value)
{
    
    return; 
}

struct hardware Dry = {
    HW_DRY,
    0, //errorcode
    "no hardware",
    dry_enable,// enable
    dry_disable, //disable
    dry_fetch, //fetch
    dry_flush, //flush
    dry_dio_read, //dio_read
    dry_dio_write, //dio_write
    dry_dio_bitfield, //dio_bitfield
    dry_data_read, //data_read
    dry_data_write, //data_write
    dry_config, //hw_config
};

