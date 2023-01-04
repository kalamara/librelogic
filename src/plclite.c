#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>

#include <plclib.h>
      
#include "plclite.h" 
#ifdef SIM
//SIM configuration
#endif //SIM

#ifdef COMEDI
//COMEDI configuration

#endif //COMEDI

#ifdef USPACE
//uspace configuration
#endif //USPACE

#ifdef GPIOD
int Inputs[N_GPIO_IN] = {16,20,21};
int Outputs[N_GPIO_OUT] = {3};
struct config_gpiod GpiodConf = {
    .chipname = "gpiochip0",
    .in_lines = Inputs,
    .in_size = N_GPIO_IN,
    .out_lines = Outputs,
    .out_size = N_GPIO_OUT,
};
#endif //GPIOD

const char * Usage = "Usage: plclite [-p config file] \n \
    Options:\n \
    -h displays this help message\n \
    -p uses a program fle";
plc_t Plc;

void dump(){
    printf("\n%s %d\n" , "PLC status:", Plc->status);

    int i = 0;
    printf("\n%s\n" , "inputs:");
    while(i <  N_GPIO_IN){
        printf("%d", Plc->di[i++].I);
    }

    i = 0;
    printf("\n%s\n" , "outputs:");
    while(i <  N_GPIO_OUT){
        printf("%d", Plc->dq[i++].Q);
    }

    i = 0;
    printf("\n%s\n" , "memory regs:");
    while(i <  N_MEM){
        printf("%ld ", Plc->m[i++].V);
    }

    i = 0;
    printf("\n%s\n" , "timers:");
    while(i <  N_TIM){
        printf("%ld %d \n", Plc->t[i].V, Plc->t[i++].Q);
    }
}

/**
* @brief graceful shutdown
*/
void sigkill() {
    printf("%s\n",  "program interrupted");
    plc_clear(Plc);
    exit(0);
}


int main(int argc, char **argv)
{
    int prog = 0;
    char * progstr = PROGRAM;
    char * cvalue = NULL;
    opterr = 0;
    int c;

    signal(SIGINT, sigkill);
    signal(SIGTERM, sigkill);

    while ((c = getopt (argc, argv, "hp:")) != -1){
        switch (c) {
        case 'h':
         printf("%s\n", Usage);
         return 1;
        break;
        case 'p':
        cvalue = optarg;
        break;
        case '?':
         printf("%s\n", Usage);
        if (optopt == 'p'){
             printf( 
            "Option -%c requires an argument\n", optopt);
        } else if (isprint (optopt)){
            printf( 
            "Unknown option `-%c'\n", optopt);
        } else{
            printf( 
            "Unknown option character `\\x%x'\n",
            optopt);
        }
        return 1;
        default:
           return -1;
        }
    }

    if(cvalue == NULL){
        cvalue = progstr;
    }

    hardware_t hw = NULL;
#ifdef GPIOD
    hardware_t hw = plc_get_hardware(HW_GPIOD); 
    hw->configure(&GpiodConf);
#endif // GPIOD
    Plc = plc_new(
    1, 
    1, 
    0,0,
    N_TIM,
    0,
    N_MEM,
    0,
    STEP,
    hw);

    printf("%s\n", "PLC initialized:");
    dump();
//initialize PLC
    Plc = plc_load_program_file(cvalue, Plc);
//init cli
    Plc = plc_start(Plc);
    for(;;){
        if(Plc->update){
            dump();
            Plc->update=0;
        }
        Plc = plc_func(Plc);
    }    
    return 0;
}

