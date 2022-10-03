/**
 *@file plc_iface.h
 *@brief interface towards python CFFI.
 * NOTE: this file is not preproccessed, so no # macros (#define, #include, etc.)
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
    int  base;
    int  write; 
    int  read;
    const char * label;
} * conf_uspace_t;  

typedef struct config_sim {
    const char * ifname;
    const char * ofname;
    unsigned int  in_size; //bytes
    unsigned int  out_size; 
    unsigned int  adc_size;
    unsigned int  dac_size;
    const char * label;
} * conf_sim_t;

typedef struct config_comedi {
    int  file;
    int  sub_i;
    int  sub_q;
    int  sub_adc;
    int  sub_dac;
    const char * label;
} * conf_comedi_t;

typedef struct config_gpiod {
    const char * chipname;
    const int * in_lines;
    const int * out_lines;
    unsigned int  in_size; 
    unsigned int  out_size; 
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
 * hardware ctor factory
 */
hardware_t plc_get_hardware(int type);
