#ifndef _CONFIG_H_
#define  _CONFIG_H_
typedef enum {
    IN_1,
    IN_2,
    IN_3,
    N_GPIO_IN,
} GPIO_IN;

typedef enum {
    OUT_1,
    N_GPIO_OUT,
} GPIO_OUT;

typedef enum {
    M_1,
    M_2,
    M_3,
    M_4, 
    M_5,
    M_6,
    M_7,
    M_8,
    N_MEM,
} MEM_IN;

typedef enum {
    TIM_1,
    TIM_2,
    TIM_3,
    N_TIM,
} TIM_OUT;

#define PROGRAM "program.ld"
#define STEP 1000 

#endif  //_CONFIG_H_
