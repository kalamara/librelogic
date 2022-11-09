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

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "data.h" 
#include "util.h"

FILE *ErrLog = NULL;

void plc_log(const char *msg, ...) {
    va_list arg;
    time_t now;
    time(&now);
    char msgstr[MAXSTR];
    memset(msgstr, 0, MAXSTR);
    va_start(arg, msg);
    vsprintf(msgstr, msg, arg);
    va_end(arg);
    if (!ErrLog)
        ErrLog = fopen(LOG, "w+");
    if (ErrLog) {
        fprintf(ErrLog, "%s", msgstr);
        fprintf(ErrLog, ":%s", ctime(&now));
        fflush(ErrLog);
    }
    printf("%s\n", msgstr);
}

void plc_close_log() {
    if (ErrLog)
        fclose(ErrLog);
}

/*******************debugging tools***************/

double Mean = 0;
double M2 = 0;
unsigned long Loop = 0;

void compute_variance(double x) {
    if (Loop == 0) { //overflow
        Mean = 0;
        M2 = 0;
    }
    Loop++;
    double delta = x - Mean;
    Mean += delta / (double) Loop;
    M2 += delta * (x - Mean);
}

void get_variance(double *mean, double *var) {
    *mean = Mean;
    if (Loop > 1)
        *var = M2 / (double) (Loop - 1);
}

unsigned long get_loop() {
    return Loop;
}
