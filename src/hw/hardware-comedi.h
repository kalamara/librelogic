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

#ifndef _HARDWARE_COMEDI_H_
#define _HARDWARE_COMEDI_H_

#include <string.h>

#include "util.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plc_iface.h"

struct hardware Comedi;

#ifdef COMEDI
#include <comedilib.h>

static comedi_t *it;
uint32_t Comedi_file;
uint8_t Comedi_subdev_i;
uint8_t Comedi_subdev_q;
uint8_t Comedi_subdev_ai;
uint8_t Comedi_subdev_aq;

int com_config(void *conf) {
    conf_comedi_t c = (conf_comedi_t) conf;

    Comedi_file = c->file;
    
    Comedi_subdev_i = c->sub_i;
    Comedi_subdev_q = c->sub_q;
    Comedi_subdev_ai = c->sub_adc;
    Comedi_subdev_aq = c->sub_dac;
    
    Comedi.label = (char*) c->label;
    
    return PLC_OK;
}

int com_enable() // Enable bus communication
{
    int r = 0;

    char filestr[MEDSTR];
    memset(filestr, 0, MEDSTR);
    sprintf(filestr, "/dev/comedi%d", Comedi_file);
    printf("%s\n", filestr);
    if ((it = comedi_open(filestr)) == NULL)
        r = -1;
    //printf("io card enabled\n");
    return r;
}

int com_disable() // Disable bus communication
{
    comedi_close(it);
    return PLC_OK;
}

int com_fetch() { // COMEDI has already fetched them for you
    return 0;
}

int com_flush() {
    return 0;
}

void com_dio_read(unsigned int index, PLC_BYTE *value) { // write input n to bit
    unsigned int b;
    comedi_dio_read(it, Comedi_subdev_i, index, &b);
    *value = (PLC_BYTE) b;
}

void com_dio_write(const PLC_BYTE *value, unsigned int n, unsigned char bit) { // write bit to n output
    comedi_dio_write(it, Comedi_subdev_q, n, bit);
}

void com_dio_bitfield(const unsigned char *mask, unsigned char *bits) { // simultaneously write output bits defined my mask and read all inputs
    /*    FIXME int i;
     unsigned int w, b;
     w = (unsigned int) (*mask);
     b = (unsigned int) (*bits);
     comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);*/
}

void com_data_read(unsigned int index, uint64_t *value) {
    lsampl_t data;
    comedi_data_read(it, Comedi_subdev_ai, index, 0, // unsigned int range,
            AREF_GROUND, // unsigned int aref,
            &data);
    *value = (uint64_t) data;
}

void com_data_write(unsigned int index, uint64_t value) {
    lsampl_t data = (lsampl_t)(value % 0x100000000);
    comedi_data_write(it, Comedi_subdev_aq, index, 0, // unsigned int range,
            AREF_GROUND, // unsigned int aref,
            data);
}

struct hardware Comedi = {
        HW_COMEDI,
        0,                // errorcode
        "Comedi driver",
        com_enable,       // enable
        com_disable,      // disable
        com_fetch,        // fetch
        com_flush,        // flush
        com_dio_read,     // dio_read
        com_dio_write,    // dio_write
        com_dio_bitfield, // dio_bitfield
        com_data_read,    // data_read
        com_data_write,   // data_write
        com_config,       // hw_config
};

#else 

struct hardware Comedi;

#endif //COMEDI

#endif /* _HARDWARE_COMEDI_H_ */
