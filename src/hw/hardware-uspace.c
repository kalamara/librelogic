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


#include <unistd.h>
#include <sys/io.h>
#include <sys/types.h>
#include <stdio.h>

#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "util.h"
#include "plc_iface.h"

uint32_t Io_base = 0;
uint8_t Wr_offs = 0;
uint8_t Rd_offs = 0;

struct hardware Uspace;

int usp_config(void * conf)
{
    conf_uspace_t u = (conf_uspace_t)conf;
    Io_base = u->base;
    Wr_offs = u->write;
    Rd_offs = u->read;
    Uspace.label = u->label;
        
    return PLC_OK;
}

int usp_enable() /* Enable bus communication */
{
	int uid = getuid(); /* get User id */
	int r = seteuid(0); /* set User Id to root (0) */
	if (r < 0 || geteuid() != 0){
		fprintf(stderr, "FATAL ERROR: UNABLE TO CHANGE TO ROOT\n");
		return PLC_ERR;
	}
	if (iopl(3)){
/* request bus WR i/o permission */
		fprintf(stderr, "FATAL ERROR: UNABLE TO GET I/O PORT PERMISSION\n");
		perror("iopl() ");
		r = seteuid(uid);
		return PLC_ERR;
	}
	r = seteuid(uid); /* reset User Id */
	outb(0, Io_base + Wr_offs); //clear outputs port
	printf("io card enabled\n");
	return PLC_OK;
}

int usp_disable() /* Disable bus communication */
{
	int uid = getuid(); /* get User id */
	int r = setuid(0); /* set User Id to root (0) */
	if (r < 0 || getuid() != 0){
		fprintf(stderr, "Unable to change id to root\nExiting\n");
		return PLC_ERR;
	}
	if (iopl(0)){ /* Normal i/o prevelege level */
		perror("iopl() ");
		r = setuid(uid);
		return PLC_ERR;
	}
	r = setuid(uid); /* reset User Id */
    return PLC_OK;
}

int usp_fetch()
{
    return 0;
}

int usp_flush()
{
    return 0;
}

void usp_dio_read(unsigned int n, BYTE* bit)
{	//write input n to bit
	unsigned int b;
	BYTE i;
	i = inb(Io_base + Rd_offs + n / BYTESIZE);
	b = (i >> n % BYTESIZE) % 2;
	*bit = (BYTE) b;
}

void usp_dio_write(const BYTE * buf, unsigned int n, unsigned char bit)
{	//write bit to n output
	BYTE q;
	q = buf[n / BYTESIZE];
	q |= bit << n % BYTESIZE;
	outb(q, Io_base + Wr_offs + n / BYTESIZE);
}

void usp_dio_bitfield(const BYTE * write_mask, BYTE * bits)
{	//simultaneusly write output bits defined my mask and read all inputs
    /*FIXME
    int i;
	for (i = 0; i < Dq; i++)
		outb(bits[i] & write_mask[i], Base + Wr_offs + i);
	for (i = 0; i < Di; i++)
        bits[i] = inb(Base + Rd_offs + i);*/
}

void usp_data_read(unsigned int index, uint64_t* value)
{
    return; //unimplemented for user space
}


void usp_data_write(unsigned int index, uint64_t value)
{
    return; //unimplemented for user space
}

struct hardware Uspace = {
    HW_USPACE,
    0, //errorcode
    "",
    usp_enable,// enable
    usp_disable, //disable
    usp_fetch, //fetch
    usp_flush, //flush
    usp_dio_read, //dio_read
    usp_dio_write, //dio_write
    usp_dio_bitfield, //dio_bitfield
    usp_data_read, //data_read
    usp_data_write, //data_write
    usp_config, //hw_config
};

