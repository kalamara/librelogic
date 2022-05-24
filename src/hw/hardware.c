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

#include "hardware.h"
#include <stddef.h>

extern struct hardware Comedi;
extern struct hardware Uspace;
extern struct hardware Sim;
extern struct hardware Dry;

hardware_t get_hardware( int type){
    switch(type){
        case HW_COMEDI:
#ifdef COMEDI        
            return &Comedi;
#else
            return NULL;
#endif             
        case HW_USPACE:
#ifdef USPACE        
            return &Uspace;
#else
            return NULL;
#endif             

        case HW_SIM: 
#ifdef SIM        
        	return &Sim;
#else
            return NULL;
#endif             

        default: 
            return &Dry;
    }
}


