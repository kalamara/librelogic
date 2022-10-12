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

NOTE: this file is not preproccessed, so no # macros (#define, #include, etc.)
*/

typedef enum{
    HW_DRY,
    HW_SIM,
    HW_COMEDI,
    HW_USPACE,//TODO: update with current linux kernels
    HW_GPIOD,
    HW_IIO, //TODO Linux industrial I/O
    HW_USB, //TODO FAR IN THE FUTURE
    N_HW
}HARDWARES;

typedef struct config_uspace {
    uint32_t  base;
    uint8_t  write; 
    uint8_t  read;
    const char * label;
} * conf_uspace_t;  

typedef struct config_sim {
    const char * ifname;
    const char * ofname;
    uint32_t  in_size; //bytes
    uint32_t  out_size; 
    uint32_t  adc_size;
    uint32_t  dac_size;
    const char * label;
} * conf_sim_t;

typedef struct config_comedi {
    uint32_t  file;
    uint8_t  sub_i;
    uint8_t  sub_q;
    uint8_t  sub_adc;
    uint8_t  sub_dac;
    const char * label;
} * conf_comedi_t;

typedef struct config_gpiod {
    const char * chipname;
    const uint32_t * in_lines;
    const uint32_t * out_lines;
    uint32_t  in_size; 
    uint32_t  out_size; 
    const char * label;
} * conf_gpiod_t;


typedef int (*helper_f)(); //generic helper functions only return an error code

typedef void(*dio_rd_f)(unsigned int, unsigned char*);
typedef void(*dio_wr_f)(const unsigned char *, unsigned int , unsigned char );
typedef void(*dio_bit_f)(const unsigned char*, unsigned char *);
typedef void(*data_rd_f)(unsigned int, uint64_t* );
typedef void(*data_wr_f)(unsigned int, uint64_t );
typedef int(*config_f)(void * );

typedef struct hardware{
    int type;
    int status;
    const char * label;

/**
 * @brief Enable bus communication
 * @return error code
 */
    helper_f enable;
/**
 * @brief Disable bus communication
 * @return error code
 */
    helper_f disable;
/**
 * @brief fetch all input bytes if necessary
 * @return error code
 */
    helper_f fetch;
/**
 * @brief flush all output bytes if necessary
 * @return error code
 */
    helper_f flush;
/**
 * @brief read digital input
 * @param index
 * @param value
 */
    dio_rd_f dio_read;
/**
 * @brief write bit to digital output
 * @param current output buffer
 * @param n index
 * @param bit
 */
    dio_wr_f dio_write;
/**
 * @brief read / write binary mask to digital input / output
 * @param mask
 * @param bits
 */
    dio_bit_f dio_bitfield;
/**
 * @brief read analog sample
 * @param the index
 * @param the raw value 
 */
    data_rd_f data_read;
/**
 * @brief write analog sample
 * @param the index
 * @param the raw value 
 */
    data_wr_f data_write;

/**
 * @brief apply a configuration to create a hardware instance 
 * @param the configuration
 */
   config_f configure;
} * hardware_t;

/**
 * Forward declaration plc_t
 */
typedef struct PLC_regs * plc_t;

/**
 * @brief start PLC 
 * @param the plc
 * @return plc with updated status
 */
plc_t plc_start(plc_t p);

/**
 * @brief stop PLC 
 * @param the plc
 * @return plc with updated status
 */
plc_t plc_stop(plc_t p);

/**
 * @brief load a PLC program
 * @param the local filename (path relative to config file) 
 * @param the plc
 * @return plc with updated status
 */
plc_t plc_load_program_file(const char * path, plc_t plc);
          
/**
 * @brief PLC initialization executed once
 * @param ref to plc
 * @return OK or error
 */
int plc_init(plc_t p);

/**
 * @brief PLC realtime loop
 * Anything in this function normally (ie. when not in error)
 * satisfies the realtime @conditions:
 * 1. No disk I/O
 * 2. No mallocs
 * This way the time it takes to execute is predictable
 * Heavy parts can timeout
 * @param the PLC
 * @return PLC with updated state
 */
plc_t plc_func( plc_t p);

/**
 * @brief construct a new plc with a configuration
 * @param number of digital inputs
 * @param number of digital outputs
 * @param number of analog inputs 
 * @param number of analog outputs
 * @param number of timers
 * @param number of pulses
 * @param number of integer memory variables
 * @param number of real memory variables
 * @param cycle time in milliseconds
 * @param hardware identifier        

 * @return configured plc
 */
plc_t plc_new(
    int di, 
    int dq,
    int ai,
    int aq,
    int nt, 
    int ns,
    int nm,
    int nr,
    int step,
    hardware_t hw);

/**
 * @brief hardware ctor factory
 * @param hardware type (enum HARDWARES)
 * @return handle to hardware instance
 */
hardware_t plc_get_hardware(int type);

/**
 * @brief get digital input value
 * @param the PLC
 * @param input index
 * @return 0 or 1
 */
unsigned char plc_get_di_val(plc_t p, unsigned int i);

/**
 * @brief get digital output value
 * @param the PLC
 * @param output index
 * @return 0 or 1
 */
unsigned char plc_get_dq_val(plc_t p, unsigned int i);

/**
 * @brief get memory register value
 * @param the PLC
 * @param register index
 * @return value
 */
unsigned int plc_get_m_val(plc_t p, unsigned int i);

/**
 * @brief get timer value
 * @param the PLC
 * @param t index
 * @return val
 */
unsigned int plc_get_t_val(plc_t p, unsigned int i);

/**
 * @brief get timer output
 * @param the PLC
 * @param input index
 * @return 0 or 1
 */
unsigned char plc_get_t_out(plc_t p, unsigned int i);

/**
 * @brief is plc running?
 * @param the PLC
 * @return 1 if running, 0 if stopped, negative if error
 */
int plc_is_running(plc_t p);

/**
 * @brief is plc updated?
 * @param the PLC
 * @return update mask if updated, 0 otherwise
 */
unsigned char plc_is_updated(plc_t p);

/**
 * @brief reset update mask
 * @param the PLC
 * @return plc handle
 */
plc_t plc_reset_update(plc_t p);
