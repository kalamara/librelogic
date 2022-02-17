cmake_minimum_required(VERSION 3.22)

set(CMAKE_BUILD_TYPE Debug)

project("logic")

find_library(COMEDI comedi)
if(COMEDI)
    add_compile_definitions(COMEDI)
else(COMEDI)
    message(WARNING "COMEDI drivers not found")    
endif(COMEDI)

find_library(PTHREAD pthread)
if(NOT PTHREAD)
    message(FATAL ERROR "PTHREAD not found")
endif(NOT PTHREAD)  
      
#find_library(NACS nacs-yml)

include_directories(
	${PROJECT_SOURCE_DIR}/include
)
	
ADD_LIBRARY(${PROJECT_NAME}
    SHARED 
    ${PROJECT_SOURCE_DIR}/vm/data.c
    ${PROJECT_SOURCE_DIR}/vm/instruction.c
    ${PROJECT_SOURCE_DIR}/vm/rung.c
    ${PROJECT_SOURCE_DIR}/vm/plclib.c
    ${PROJECT_SOURCE_DIR}/vm/parser-il.c
    ${PROJECT_SOURCE_DIR}/vm/parser-ld.c
    ${PROJECT_SOURCE_DIR}/vm/parser-tree.c
    ${PROJECT_SOURCE_DIR}/vm/codegen.c
    
    #${PROJECT_SOURCE_DIR}/src/hw/hardware.c
    #${PROJECT_SOURCE_DIR}/src/hw/hardware-comedi.c
    #${PROJECT_SOURCE_DIR}/src/hw/hardware-sim.c
    #${PROJECT_SOURCE_DIR}/src/hw/hardware-uspace.c
    #${PROJECT_SOURCE_DIR}/src/hw/hardware-dry.c
)






