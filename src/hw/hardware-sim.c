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


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
        
#include "data.h"
#include "instruction.h"
#include "rung.h"

#include "util.h"

#include "plc_iface.h"

#define ASCIISTART 0x30

FILE * Ifd = NULL;
FILE * Qfd = NULL;
char * BufIn = NULL;
char * BufOut = NULL;
char * AdcIn = NULL;
char * AdcOut = NULL;

unsigned int Ni = 0;
unsigned int Nq = 0;
unsigned int Nai = 0;
unsigned int Naq = 0;

struct hardware Sim;

int sim_config(void * conf)
{
    int r = PLC_OK;    
    conf_sim_t c =(conf_sim_t)conf;
    const char * istr =  c->ifname;
    if(istr){
        if(!(Ifd=fopen(istr, "r+"))){
                plc_log("Failed to open simulation input from %s", istr);
                r = PLC_ERR;
        } else {
                plc_log("Opened simulation input from %s", istr);
        }
    }
    const char * ostr = c->ofname;
    if(ostr){
         if(!(Qfd=fopen(ostr, "w+"))){
             plc_log("Failed to open simulation output to %s", ostr);
             r = PLC_ERR;
        } else {
            plc_log("Opened simulation output to %s", ostr);
        }
    }
    
    Ni = c->in_size; 
    Nq = c->out_size;
    Nai = c->adc_size;
    Naq = c->dac_size;
    Sim.label = c->label;
        
    return r;    
}

int sim_enable() /* Enable bus communication */
{
    int r = PLC_OK;
    /*open input and output streams*/
    
    if(!(BufIn = (char * )malloc(Ni))){
        r = PLC_ERR;
    } else {
        memset(BufIn, 0, Ni);
    }
    if(!(BufOut = (char * )malloc(Nq))){
        r = PLC_ERR;
    } else {
        memset(BufOut, 0, Nq);
    }
    if(!(AdcIn = (char * )malloc( LONG_BYTES * Nai))){
        r = PLC_ERR;
    } else {
        memset(AdcIn, 0, LONG_BYTES * Nai);
    }
    if(!(AdcOut = (char * )malloc( LONG_BYTES * Naq))){
        r = PLC_ERR;
    } else {
        memset(AdcOut, 0, LONG_BYTES * Naq);
    }
    return r;
}

int sim_disable() /* Disable bus communication */
{
    int r = PLC_OK;
    /*close streams*/
    if(Ifd && !fclose(Ifd)){
        r = PLC_ERR;
    }
    plc_log("Closed simulation input");
    if(Qfd && !fclose(Qfd)){
        r = PLC_ERR;
    }
    plc_log("Closed simulation output"); 
    if(BufIn){
        free(BufIn);
        BufIn = NULL;
    }
    if(BufOut){
        free(BufOut);
        BufOut = NULL;
    }
    return r;
}

int sim_fetch()
{
    unsigned int digital = Ni;
    unsigned int analog = Nai;
    int bytes_read = 0;
    if(Ifd){
        bytes_read = fread(BufIn, 
                        sizeof(BYTE), 
                        digital, 
                        Ifd ? Ifd : stdin);
        int i = 0;
        for(; i < bytes_read; i++){
            if(BufIn[i] >= ASCIISTART){
                BufIn[i] -= ASCIISTART;
            }
        }
        bytes_read += fread(AdcIn, 
                        sizeof(BYTE), 
                        LONG_BYTES*analog, 
                        Ifd ? Ifd : stdin);
   
        if(bytes_read < digital + LONG_BYTES*analog){
        //plc_log("failed to read from %s, reopening", SimInFile);
                if(feof(Ifd)){
                    rewind(Ifd);
                } else {
                sim_disable();
                sim_enable();
                }
        }
    }
    return bytes_read;
}

int sim_flush()
{
    int bytes_written = 0;
    unsigned int digital = Nq;
    unsigned int analog = Naq;
    if(Qfd){
        bytes_written = fwrite(BufOut, 
                        sizeof(BYTE), 
                        digital, 
                        Qfd);
        bytes_written += fwrite(AdcOut, 
                            sizeof(BYTE), 
                            analog * LONG_BYTES, 
                            Qfd );
        fputc('\n', Qfd );
        fflush(Qfd);
    }
    return bytes_written;
}

void sim_dio_read(unsigned int n, BYTE* bit)
{	//write input n to bit
    unsigned int b, position;
    position = n / BYTESIZE;
    BYTE i = 0;
    if(strlen(BufIn) > position){
    /*read a byte from input stream*/
        i = BufIn[position];
    }
    b = (i >> n % BYTESIZE) % 2;
    *bit = (BYTE) b;
}

void sim_dio_write(const unsigned char *buf, unsigned int n,  BYTE bit)

{	//write bit to n output
	BYTE q;
    unsigned int position = n / BYTESIZE;
    q = buf[position];
    q |= bit << n % BYTESIZE;
    /*write a byte to output stream*/
    q+=ASCIISTART; //ASCII
   // plc_log("Send %d to byte %d", q, position);
     if(strlen(BufOut) >= position){
         BufOut[position] = q;
     }
}

void sim_dio_bitfield(const BYTE* mask, BYTE *bits)
{	//simultaneusly write output bits defined by mask and read all inputs
    /* FIXME
    int i=0;
    unsigned int w = (unsigned int) (*mask);
    unsigned int b = (unsigned int) (*bits);
    comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);*/
}

void sim_data_read(unsigned int index, uint64_t* value)
{
    unsigned int pos = index*LONG_BYTES;
    int i = LONG_BYTES - 1;
    *value = 0;
    if(strlen(AdcIn) > pos) {
       uint64_t mult = 1;
       for(; i >= 0 ; i--){
            *value += (uint64_t)AdcIn[pos + i] * mult;
            mult *= 0x100;
        }    
    }
}


void sim_data_write(unsigned int index, uint64_t value)
{
    unsigned int pos = index*LONG_BYTES;
    sprintf(AdcOut + pos, "%lx", value);
    return; 
}

struct hardware Sim = {
    HW_SIM,
    0, //errorcode
    "simulated hardware",
    sim_enable,// enable
    sim_disable, //disable
    sim_fetch, //fetch
    sim_flush, //flush
    sim_dio_read, //dio_read
    sim_dio_write, //dio_write
    sim_dio_bitfield, //dio_bitfield
    sim_data_read, //data_read
    sim_data_write, //data_write
    sim_config, //hw_config
};

