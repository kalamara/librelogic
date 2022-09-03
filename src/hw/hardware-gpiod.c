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

static struct gpiod_chip *Chip;

static struct gpiod_line ** InLines = NULL;
static unsigned int MaxIn = 0;

static struct gpiod_line ** OutLines = NULL;
static unsigned int MaxOut = 0;

int gpiod_config(void * conf)
{
    conf_gpiod_t c = (conf_gpiod_t )conf;
    // Open GPIO chip
    Chip = gpiod_chip_open_by_name(c->chipname);
    if(!Chip){
        plc_log("Failed to open GPIOD chip"); 
        
        return PLC_ERR;    
    }
    MaxIn = c->in_size;
    MaxOut = c->out_size;
    
    InLines = calloc(MaxIn, sizeof(void *));
    OutLines = calloc(MaxOut, sizeof(void *));
    
    unsigned int i = 0;
    unsigned int q = 0;
    for(; i < MaxIn ; i++) {// Open GPIO lines
        InLines[i] = gpiod_chip_get_line(Chip, c->in_lines[i]);    

        if(!InLines[i]){
            plc_log("Could not get line %d ",  c->in_lines[i]); 
        
            return PLC_ERR;    
        }
    }
    
    for(; q < MaxOut ; q++) {
        OutLines[q] = gpiod_chip_get_line(Chip, c->out_lines[q]);    

        if(!OutLines[q]){
            plc_log("Could not get line %d ",  c->out_lines[q]); 
        
            return PLC_ERR;    
        }
    }

    return PLC_OK;
}

int gpiod_enable() /* Enable */
{
    unsigned int i = 0;
    unsigned int q = 0;
    int ok = 0; 
    // Open lines for output
    for(; q < MaxOut ; q++) {//  
        ok = gpiod_line_request_output(OutLines[q], "librelogic", 0);
        if(ok < 0){
            plc_log("Could not get output %d ", q); 
        
            return PLC_ERR;    
        }
    } 

    // Open lines for input
    for(; i < MaxIn ; i++) {//  
        ok = gpiod_line_request_input(InLines[i], "librelogic");  
        if(ok < 0){
            plc_log("Could not get input %d ", i); 
        
            return PLC_ERR;    
        }
    }
    return PLC_OK;
}

int gpiod_disable() /* Disable */
{
    unsigned int i = 0;
    unsigned int q = 0;
    for(; i < MaxIn ; i++) {// Release GPIO lines
        gpiod_line_release(InLines[i]);    
    }
    
    for(; q < MaxOut ; q++) {
        gpiod_line_release(OutLines[q]);
    }

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
    int v = gpiod_line_get_value(InLines[n]);
    *bit = (BYTE)v;

    return;
}

void gpiod_dio_write(const unsigned char *buf, unsigned int n,  BYTE bit)
{	
    gpiod_line_set_value(OutLines[n], bit);

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

