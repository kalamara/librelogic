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

#ifndef _UTIL_H_
#define _UTIL_H_

/**logging, metrics, debugging stuff */

#define LOG "plcemu.log"
void plc_log(const char *msg, ...);
void plc_close_log();

/*******************debugging tools****************/
void dump_label(char *label, char *dump);
void compute_variance(double x);
void get_variance(double *mean, double *var);
unsigned long get_loop();

#endif /* _UTIL_H */
