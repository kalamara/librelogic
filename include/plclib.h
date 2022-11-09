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

#ifndef _PLCLIB_H_
#define _PLCLIB_H_
/**
 *@file plclib.h
 *@brief PLC core virtual machine
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "data.h"
#include "instruction.h"
#include "rung.h"

#include "plc_iface.h"

#define MILLION  1000000
#define THOUSAND 1000

#define MAXRUNG  256

#define FLOAT_PRECISION 0.000001

typedef enum {
    ST_STOPPED,
    ST_RUNNING
} STATUSES;

typedef enum {
    N_ERR = -20, // ERROR CODES are negative
    PLC_ERR_OVFLOW,
    PLC_ERR_TIMEOUT,
    PLC_ERR_HARDWARE,
    PLC_ERR_BADOPERATOR,
    PLC_ERR_BADCOIL,
    PLC_ERR_BADINDEX,
    PLC_ERR_BADOPERAND,
    PLC_ERR_BADFILE,
    PLC_ERR_BADCHAR,
    PLC_ERR_BADPROG,
} ERRORCODES;

typedef enum {
    IE_PLC,
    IE_BADOPERATOR,
    IE_BADCOIL,
    IE_BADINDEX,
    IE_BADOPERAND,
    IE_BADFILE,
    IE_BADCHAR, N_IE
} IL_ERRORCODES;

typedef enum {
    LANG_LD,
    LANG_IL,
    LANG_ST
} LANGUAGES;

typedef enum { // boolean function blocks supported
    BOOL_DI,      // digital input
    BOOL_DQ,      // digital output
    BOOL_COUNTER, // pulse of counter
    BOOL_TIMER,   // output of timer
    BOOL_BLINKER, // output of blinker
    N_BOOL
} BOOL_FB;

/**
 * @brief what changed since the last cycle
 */
typedef enum {
    CHANGED_I      = 0x1,
    CHANGED_O      = 0x2,
    CHANGED_M      = 0x4,
    CHANGED_T      = 0x8,
    CHANGED_S      = 0x10,
    CHANGED_STATUS = 0x20
} CHANGE_DELTA;

/***********************plc_t*****************************/
/**
 * @brief The digital_input struct
 */
typedef struct digital_input {
    BIT(I);      // contact value
    BIT(RE);     // rising edge
    BIT(FE);     // falling edge
    BIT(EDGE);   // true if value changed
    BIT(MASK);   // true if forced 1
    BIT(N_MASK); // true if forced 0
    char *nick;  // [NICKLEN]; // nickname
} *di_t;

/**
 * @brief The digital_output struct
 */
typedef struct digital_output {
    BIT(Q);      // contact
    BIT(SET);    // set
    BIT(RESET);  // reset
    BIT(MASK);   // true if forced true
    BIT(N_MASK); // true if forced false
    char *nick;  // [NICKLEN];//nickname
} *do_t;

/**
 * @brief The analog_io  struct
 */
typedef struct analog_io {
    double V;    // data value
    double min;  // range for conversion to/from raw data
    double max;
    double mask; // forced value mask
    char *nick;  // [NICKLEN]; // nickname
} *aio_t;

/**
 * @brief The timer struct.
 * struct which represents  a timer state at a given cycle
 */
typedef struct timer {
    long S;       // scale; S=1000=>increase every 1000 cycles. STEP= 10 msec=> increase every 10 sec
    long sn;      // internal counter used for scaling
    long V;       // value
    BIT(Q);       // output
    long P;       // Preset value
    BIT(ONDELAY); // 1=on delay, 0 = off delay
    BIT(START);   // start command: must be on to count
    BIT(RESET);   // down command: sets V = 0
    //BIT(MASK);  // true if timer is forced to up or down
    char *nick;   // [NICKLEN];
} *dt_t;

/**
 * @brief The blink struct
 * struct which represents a blinker
 */
typedef struct blink {
    BIT(Q);     // output
    long S;     // scale; S=1000=>toggle every 1000 cycles. STEP= 10 msec=> toggle every 10 sec
    long sn;    // internal counter for scaling
    char *nick; // [NICKLEN];
} *blink_t;

/**
 * @brief The mvar struct
 * struct which represents a memory register / counter
 */
typedef struct mvar {
    uint64_t V;  // TODO: add type
    BIT(RO);     // 1 if read only;
    BIT(DOWN);   // 1: can be used as a down counter
    BIT(PULSE);  // pulse for up/downcounting
    BIT(EDGE);   // edge of pulse
    BIT(SET);    // set pulse
    BIT(RESET);  // reset pulse
    //BIT(MASK); // true if pulse is set
    char *nick;  // [NICKLEN]; // nickname
} *mvar_t;

/**
 * @brief The mreal struct
 * struct which represents a real number memory register
 */
typedef struct mreal {
    double V;   // TODO: add type
    BIT(RO);    // 1 if read only;
    char *nick; // [NICKLEN]; // nickname
} *mreal_t;

/**
 * @brief The PLC_regs struct
 * The struct which contains all the software PLC registers
 */
// TODO: should masks and edges be packed inside di_t, dq_t?
typedef struct PLC_regs {
    hardware_t hw;
    // hardware interface
    BYTE *inputs;         // digital input values buffer
    uint64_t *real_in;    // analog raw input values buffer
    BYTE *outputs;        // digital output values buffer
    uint64_t *real_out;   // analog raw output values buffer
    
    BYTE command;         // serial command from plcpipe
    BYTE response;        // response to named pipe
    
    BYTE update;          // binary mask of state update
    int status;           // 0 = stopped, 1 = running, negative = error
    
    BYTE ni;              // number of bytes for digital inputs
    di_t di;              // digital inputs
    
    BYTE nq;              // number of bytes for digital outputs
    do_t dq;              // the digital outputs

    BYTE nai;             // number of analog input channels
    aio_t ai;             // the analog inputs
    
    BYTE naq;             // number of analog output channels
    aio_t aq;             // the analog outputs

    BYTE nt;              // number of timers
    dt_t t;               // the timers
    
    BYTE ns;              // number of blinkers
    blink_t s;            // the blinkers
    
    BYTE nm;              // number of memory counters
    mvar_t m;             // the memory
    
    BYTE nmr;             // number of memory registers
    mreal_t mr;           // the memory

    rung_t *rungs;
    BYTE rungno;          // 256 rungs should suffice
    
    long step;            // cycle time in milliseconds
    struct PLC_regs *old; // pointer to previous state
} *plc_t;

/**
 * @brief parse IL program
 * @param a unique program identifier
 * @param the program as an allocated buffer of allocated strings
 * @param the plc to store the generated microcode to
 * @return plc with updated status
 */
plc_t parse_il_program(const char *name, const char lines[][MAXSTR], plc_t p);

/**
 * @brief parse LD program
 * @param the program as an allocated buffer of allocated strings
 * @param the plc to store the generated microcode to
 * @return plc with updated status
 */
plc_t parse_ld_program(const char *name, const char lines[][MAXSTR], plc_t p);

/**
 * @brief add a new rung to a plc
 * @param a unique identifier
 * @param the plc
 * @return reference to the new rung
 */
rung_t plc_mk_rung(const char *name, plc_t p);

/**
 * @brief get rung reference from plc
 * @param p a plc
 * @param r the rung reference
 * @param idx the index
 * @return reference to rung or NULL
 */
rung_t plc_get_rung(const plc_t p, unsigned int idx);

/**
 * @brief custom project init code as plugin
 * @return OK or error
 */
int plc_project_init();

/**
 * @brief custom project task code as plugin
 * @param pointer to PLC registers
 * @return OK or error
 */
int plc_project_task(plc_t p);

/**
 * @brief force operand with value
 * @param the plc
 * @param the operand type
 * @param the operand index
 * @param the value
 * @return new plc state, or NULL in error
 */
plc_t plc_force(plc_t p, int op, BYTE i, char *val);

/**
 * @brief unforce operand
 * @param the plc
 * @param the operand type
 * @param the operand index
 * @param new plc state, or null in error
 */
plc_t plc_unforce(plc_t p, int op, BYTE i);

/**
 * @brief is an operand forced
 * @param reference to plc
 * @param operand type
 * @param input index
 * @return true if forced, false if not, error if out of bounds
 */
int plc_is_forced(plc_t p, int op, BYTE i);

/**
 * @brief copy constructor
 * @param source plc
 * @return newly allocated copy
 */
plc_t plc_copy(const plc_t plc);

/**
 * @brief dtor
 */
void plc_clear(plc_t plc);

// configurators

/**
 * @brief assign name to a plc register variable
 * @param plc instance   
 * @param the type of variable (IL_OPERANDS enum value)
 * @param variable index
 * @param variable name
 * @see also data.h
 * @return plc instance with saved change or updated error status
 */
plc_t plc_declare_variable(const plc_t p, int var, BYTE idx, const char *val);

/**
 * @brief assign initial value to a plc register variable
 * @param plc instance   
 * @param the type of variable (IL_OPERANDS enum value)
 * @param variable index
 * @param variable initial value (serialized float or long eg.
 * "5.35" or "12345")
 * @see also data.h
 * @return plc instance with saved change or updated error status
 */
plc_t plc_init_variable(const plc_t p, int var, BYTE idx, const char *val);

/**
 * @brief configure a plc register variable as readonly
 * @param plc instance   
 * @param the type of variable (IL_OPERANDS enum value)
 * @param variable index
 * @param serialized readonly flag (true if "TRUE", false otherwise)
 * @see also data.h
 * @return plc instance with saved change or updated error status
 */
plc_t plc_configure_variable_readonly(const plc_t p, int var, BYTE idx, const char *val);

/**
 * @brief assign upper or lower limit to an analog input or output
 * @param plc instance   
 * @param the type of io (IL_OPERANDS enum value)
 * @param variable index
 * @param the limit value (serialized float eg. "5.35")
 * @param upper limit if true, otherwise lower limit
 * @see also data.h
 * @return plc instance with saved change or updated error status
 */
plc_t plc_configure_io_limit(const plc_t p, int io, BYTE idx, const char *val, BYTE max);

/**
 * @brief configure a register as up or down counter
 * @param plc instance   
 * @param variable index
 * @param serialized direction flag (true if "DOWN", false otherwise)
 * @return plc instance with saved change or updated error status
 */
plc_t plc_configure_counter_direction(const plc_t p, BYTE idx, const char *val);
/**
 * @brief configure a timer scale
 * @param plc instance   
 * @param timer index
 * @param serialized long (eg 100000)
 * @see also timer_t
 * @return plc instance with saved change or updated error status
 */
plc_t plc_configure_timer_scale(const plc_t p, BYTE idx, const char *val);

/**
 * @brief configure a timer preset
 * @param plc instance   
 * @param timer index
 * @param serialized long (eg 100000)
 * @see also timer_t
 * @return plc instance with saved change or updated error status
 */
plc_t plc_configure_timer_preset(const plc_t p, BYTE idx, const char *val);

/**
 * @brief configure a timer delay mode
 * @param plc instance   
 * @param timer index
 * @param serialized mode flag (true if "ON", false otherwise)
 * @see also timer_t
 * @return plc instance with saved change or updated error status
 */
plc_t plc_configure_timer_delay_mode(const plc_t p, BYTE idx, const char *val);

/**
 * @brief configure a pulse scale
 * @param plc instance   
 * @param pulse index
 * @param serialized long (eg 100000)
 * @see also blink_t
 * @return plc instance with saved change or updated error status
 */
plc_t plc_configure_pulse_scale(const plc_t p, BYTE idx, const char *val);

#endif /* _PLCLIB_H_ */
