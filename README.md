# librelogic
Free PLC logic library

# OVERVIEW

LibreLogic is a library that can emulate a Programmable Logic Controller (PLC).

PLC's are real time controllers whose function is to periodically read inputs, run several real time tasks, and control outputs, in a steady time period.

PLC's are the standard platform for automation applications, and can they can be programmed in one or more of the 4 programming languages as defined by IEC116131-3: 
Instruction List, Ladder Diagram, Function Block Diagram, or Structured Text.

LibreLogic emulates this function: In a configurable time cycle, it will read the 
inputs from the hardware, run a task as programmed by the user, and send the 
appropriate outputs back to the card. 

Apart from inputs and outputs, LibreLogic also holds an internal "address space"
of a user-defined number of memory variables which you may use in your programs.

It also supports Timer and Blinking Timer (Pulse) registers, whose number and function 
can as well be configured by the user.

# Hardware
LibreLogic is designed to support several IO hardware. 
On Linux, it can work with GPIOD, Comedi, and in user space. 
Additionally, in case no hardware is available there is File Simulation mode and Dry mode.

## GPIOD
In case your machine has GPIOs (eg. on a RaspBerry pi) you can just install the GPIOD package:

libgpdiod-dev

See also
https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git

This mode only supports digital I/O.

## Comedi
In Comedi mode, all you need to do is install and set up the apropriate Comedi driver for your card, and provide the setup values to librelogic frm the caller aplication.
Consult www.comedi.org for a list of compatible cards, and instructions on comedi.

## User space
In user-space mode, you need to know the base IO address space of your cards, 
and which area of it the card uses for reading (read offset) and writing (write 
offset). For pci cards, run 

> lspci -vvv 

to find out the base address. The rest should be found in the cards' manual.

## Simulation
In File Simulation mode, LibreLogic can be configured to read input bytes from 
an ASCII text file and send outputs to another text file.

## Dry
In Dry mode no hardware is used, inputs are updated manually but the logic is executed and produces outputs. This mode is for debugging.

# Unit testing
LibreLogic comes with a harness of hundreds of unit tests, however these tests require CUnit to run.
See http://cunit.sourceforge.net/


# PROGRAMMING

You can edit a text file with a task in LD or IL in your favorite text editor and then load it into LibreLogic (see below).

## Ladder Diagram
.ld files are in Ladder diagram format.
The version of LD LibreLogic supports, consists of the following operators and 
operands.

### Operators
These are the accepted symbols that can exist along with the operands.

'-' propagates a boolean state horizontally, from left to right. 
Thus, it works as a logical "AND". 

'+' changes line and can join states of up to 3 different lines, 
like a logical "OR" .

'|' propagates a state vertically, both ways between aligned "+" nodes.

'!' negates the state of the following operand like a logical "NOT"

'(' open contact. this propagates a state directly to an output. the following operand must be an output.

')' negate contact. this propagates the opposite of a state to an output. the following operand must be an output.

'[' set coil. if this is ON, thestate of an output is set. the following operand must be an output.

']' reset coil. if this is ON, thestate of an output is reset. the following operand must be an output.

';' end of line. anything after that is considered "comments".

Blank characters interrupt lines, so be careful.

### Input operands 
These can appear anywhere in a line before a '(', followed by a valid 
non negative index. Valid values of indexes are dependant on the operand and
the configuration. This means, that if you have 16 timers and 64 inputs, you
can write t14 and i62, but not t45 or i567.
In every cycle, their values are polled and propagated to the diagram. 
Accepted symbols (case sensitive) are:

'i' digital input state

'q' digital output state

'r' rising edge of digital input

'f' falling edge of digital input

'm' pulse state of counter

't' output of timer

'b' output of blinker


### Output operands
These symbols must follow operator '(' and be followed by a valid index.
Each output operand should appear only once.

'Q' digital output

'M' pulse of counter

'T' timer 


## LD Example : Triple Majority Circuit

    ; triple majority circuit
    ; https://en.wikipedia.org/wiki/Triple_modular_redundancy

     i0---i1----+---------(Q0
     i2---i0----+
     i1---i2----+

##  Instruction List

Alternatively, you can define a IEC61131-3 compatible Instruction List program in .il files. 
Currently, subroutines are not implemented. 
Just like standard Instruction List, all instructions store their result to an internal Accumulator register, while "ST" stores the Accumulator's value to its operand.
A line of an IL program, shall follow the format:

    [label:]<operator>[<modifier>[%<operand\><byte>[</bit>]]|<label>][;comment]
Supported Operators are:
    ) close parenthesis (pop instruction from stack)
    S set output
    R reset output
    AND
    OR
    XOR
    LD load
    ST store
    ADD
    SUB	subtract
    MUL	multiply
    DIV	divide
    GT	>
    GE	>=
    NE	<>
    EQ	==
    LE	<=
    LT	<
    JMP	jump to label
Modifier symbols recognized are:
    (	open parenthesis (push instruction to stack)
    !	negate
    ?	conditional
Operands are the same as in LD, with the difference that it is assumed that they are Words (unsigned Integers), unless noted otherwise with the symbol '/'. As defined in the IEC standard, each instruction supports its own set of data types and modifiers, according to the following scheme:

Instruction |Modifiers      |Data Types
----------- | ------------- | -----------
)           |N/A            |N/A
S 	    |N/A	    |BOOL
R 	    |N/A	    |BOOL
AND 	|!,(	    |BOOL/WORD
OR	    |!,(	    |BOOL/WORD
XOR	    |!,(	    |BOOL/WORD
LD 	    |!		    |BOOL/WORD
ST 	    |!		    |BOOL/WORD
ADD	    |(		    |BOOL/WORD
SUB	    |(		    |BOOL/WORD
MUL	    |(		    |BOOL/WORD
DIV	    |(		    |BOOL/WORD
GT	    |(		    |BOOL/WORD
GE	    |(		    |BOOL/WORD
NE	    |(		    |BOOL/WORD
EQ	    |(		    |BOOL/WORD
LE	    |(		    |BOOL/WORD
LT	    |(		    |BOOL/WORD
JMP	    |?		    |CHARACTER STRING

## IL Example : GCD

   ; greatest common divisor
   ; https://en.wikipedia.org/wiki/Euclidean_algorithm

    LD %i0
    ST %m0
    EQ %m3
    JMP?end
    
    LD %i1
    ST %m1
    EQ %m3
    JMP?end
    
    while:LD %m0    ; A
    EQ %m1          ; A == B
    JMP?endwhile    ; while(A != B)
        LD %m0
        LT %m1      ; A < B
        JMP?reverse
        LD %m0
        SUB %m1     ; A - B
        ST %m0      ; A = A - B
    JMP while
    reverse:LD %m1  ; B
        SUB %m0     ; B - A
        ST %m1      ; B = B - A
    JMP while
    endwhile:LD %m0 ; 
    ST %q0 ; output gcd 
    end: LD %m3;

# USE CASE: LibreLogic on the Raspberry pi
Hardware: Raspberry pi 3

OS: Raspbian 11

## Installing required packets:
apt-get install ninja-build cmake python3 python3-dev gcc gpiod libgpiod-dev

## Python environment:
pip install cffi

## Wiring
Connect buttons on GPIO 16, 20, 21 and a LED to GPIO 3

## Build librelogic
Clone librelogic repository

### Configure
./Config.sh

### Build
cmake --build build-lib

### Build python interface
python cffi_build.py

### run python example
This will load the default program, program.ld
which is the majority circuit above:


     i0---i1----+---------(Q0
     i2---i0----+
     i1---i2----+



     python plclite.py &
     Using GPIOD
     IN 0 => GPIO 16
     IN 1 => GPIO 20
     IN 2 => GPIO 21
     OUT 0 => GPIO 3
     Loading LD code from program.ld...
     program.ld
     0.LD  i0/2
     1.AND (i0/1
     2.)
     3.OR (i0/0
     4.AND (i0/2
     5.)
     6.OR (i0/1
     7.AND (i0/0
     8.)
     9.)
     10.)
     11.ST  Q0/0
     
     In:  0 0 0 
     Out: 0 


### confirm GPIOs are used:

     gpioinfo
     gpiochip0 - 54 lines:
     	line   0:      unnamed       unused   input  active-high 
     	line   1:      unnamed       unused   input  active-high 
     	line   2:      unnamed       unused   input  active-high 
     	line   3:      unnamed "librelogic"  output  active-high [used]
     	line   4:      unnamed       unused   input  active-high 
     	line   5:      unnamed       unused   input  active-high 
     	line   6:      unnamed       unused   input  active-high 
     	line   7:      unnamed       unused   input  active-high 
     	line   8:      unnamed       unused   input  active-high 
     	line   9:      unnamed       unused   input  active-high 
     	line  10:      unnamed       unused   input  active-high 
     	line  11:      unnamed       unused   input  active-high 
     	line  12:      unnamed       unused   input  active-high 
     	line  13:      unnamed       unused   input  active-high 
     	line  14:      unnamed       unused   input  active-high 
     	line  15:      unnamed       unused   input  active-high 
     	line  16:      unnamed "librelogic"   input  active-high [used]
     	line  17:      unnamed       unused   input  active-high 
     	line  18:      unnamed       unused   input  active-high 
     	line  19:      unnamed       unused   input  active-high 
     	line  20:      unnamed "librelogic"   input  active-high [used]
     	line  21:      unnamed "librelogic"   input  active-high [used]

## Congratulations, you are running ladder logic on the Raspberry pi!

When the state of a button is changed, the LED flashes according the logic in the program.ld:
It is ON when and only when any 2 buttons are pressed!

# API reference 
TBD
