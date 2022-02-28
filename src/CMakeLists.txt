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

include_directories(
	${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}
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
    ${PROJECT_SOURCE_DIR}/hw/hardware.c
)

if(${HW} STREQUAL "SIM")
    target_sources(${PROJECT_NAME}, ${PROJECT_SOURCE_DIR}/hw/hardware-sim.c)
elseif(${HW} STREQUAL "USPACE")
    target_sources(${PROJECT_NAME}, ${PROJECT_SOURCE_DIR}/hw/hardware-uspace.c)
elseif(${HW} STREQUAL "COMEDI")
    target_sources(${PROJECT_NAME}, ${PROJECT_SOURCE_DIR}/hw/hardware-comedi.c)
else() #DRY
    target_sources(${PROJECT_NAME} PUBLIC 
        ${PROJECT_SOURCE_DIR}/hw/hardware-dry.c)
endif()





