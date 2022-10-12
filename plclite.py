#!/usr/bin/python

"""PLC - lite for running logic on Raspberry pi"""
VERSION = '0.0'

USAGE = '''Usage: plclite [-p config file]
    Options:
    -h displays this help message
    -p uses a program fle";

'''

PROGRAM = 'program.ld'
STEP = 1000
INPUTS = [16,20,21]
OUTPUTS = [3]

import sys
sys.path.append('build-lib')

# librelogic.py
from _librelogic import ffi, lib
hardware_conf = ffi.new("struct config_gpiod *",
{   
    'chipname' : ffi.new("char[]", "gpiochip0".encode('ascii')),
    'in_lines' : ffi.new("uint32_t[]", INPUTS),
    'in_size'  : len(INPUTS),
    'out_lines': ffi.new("uint32_t[]", OUTPUTS),
    'out_size' : len(OUTPUTS),
    'label' : ffi.new("char[]", "gpiod".encode('ascii')),
}
)

def dump(plc):
    instr = "In:  "  
    for i in range(0,len(INPUTS)):
        instr = instr + str(lib.plc_get_di_val(plc, i)) + " "
    print(instr);

    qstr = "Out: "  
    for q in range(0,len(OUTPUTS)):
        qstr = qstr + str(lib.plc_get_dq_val(plc, q)) + " "
    print(qstr);
        

def logic(prog):
    hw_gpiod = lib.plc_get_hardware(lib.HW_GPIOD); 
    hw_gpiod.configure(hardware_conf);
    plc = lib.plc_new( 1, 1, 0, 0, 1, 0, 2, 0, STEP, hw_gpiod); 
    
    return lib.plc_load_program_file(ffi.new("char[]", prog.encode('ascii')), plc);

def main(args=None):
    import getopt

    p_file = PROGRAM
    
    if args is None:
        args = sys.argv[1:]
    try:
        opts, args = getopt.gnu_getopt(args, 'hvp:',
                                       ['help', 'version',
                                        'program='
                                       ])
        for o,a in opts:
            if o in ('-h', '--help'):
                print(USAGE)
                return 0
            elif o in ('-v', '--version'):
                print(VERSION)
                return 0
            elif o in ('-p', '--program'):
                p_file = a
            
    except getopt.GetoptError:
        e = sys.exc_info()[1]     # current exception
        sys.stderr.write(str(e)+"\n")
        sys.stderr.write(USAGE+"\n")
        return 1
        
    plc = lib.plc_start(logic(p_file))
        
    while(lib.plc_is_running(plc) == 1):
        plc = lib.plc_func(plc)
        if lib.plc_is_updated(plc) > 0:
            dump(lib.plc_reset_update(plc))

if __name__ == '__main__':
    sys.exit(main())
