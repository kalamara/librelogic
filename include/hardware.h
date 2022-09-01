#ifndef _HARDWARE_H_
#define _HARDWARE_H_
/**
 *@file hardware.h
 *@brief hardware interface
*/
#include <inttypes.h>

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
    int  in_size; //bytes
    int  out_size; 
    int  adc_size;
    int  dac_size;
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
 * hardware ctor factory
 */
hardware_t plc_get_hardware(int type);


#endif //_HARDWARE_H_
