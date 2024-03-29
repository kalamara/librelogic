cmake_minimum_required(VERSION 3.18)

set(CMAKE_BUILD_TYPE Debug)

project("logic")

include_directories(
	${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}
)
	
ADD_LIBRARY(${PROJECT_NAME}
    SHARED  
    ${PROJECT_SOURCE_DIR}/util.c
    ${PROJECT_SOURCE_DIR}/vm/data.c
    ${PROJECT_SOURCE_DIR}/vm/instruction.c
    ${PROJECT_SOURCE_DIR}/vm/rung.c
    ${PROJECT_SOURCE_DIR}/vm/plclib.c
    ${PROJECT_SOURCE_DIR}/vm/parser-il.c
    ${PROJECT_SOURCE_DIR}/vm/parser-ld.c
    ${PROJECT_SOURCE_DIR}/vm/parser-tree.c
    ${PROJECT_SOURCE_DIR}/vm/codegen.c
    ${PROJECT_SOURCE_DIR}/hw/hardware.c
    ${PROJECT_SOURCE_DIR}/hw/hardware-dry.c
)
#message("Using " ${HW} " hardware...")
if(${HW} STREQUAL "SIM")
    message("Using simulated hardware")    
    add_compile_definitions(SIM)
    target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/hw/hardware-sim.c)
elseif(${HW} STREQUAL "USPACE")
    message("Controlling PCI bus hardware from user space")    
    add_compile_definitions(USPACE)
    target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/hw/hardware-uspace.c)
elseif(${HW} STREQUAL "COMEDI")
    find_library(COMEDI comedi)
    if(COMEDI)
        message("COMEDI drivers found")    
        add_compile_definitions(COMEDI)
    else(COMEDI)
        message(FATAL_ERROR "COMEDI not found!")
    endif(COMEDI)
    target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/hw/hardware-comedi.c)
    target_link_libraries(${PROJECT_NAME} PUBLIC -lcomedi) 

elseif(${HW} STREQUAL "GPIOD")
    find_library(GPIOD gpiod)
    if(GPIOD)
        message("GPIOD found")    
        add_compile_definitions(GPIOD)
    else(GPIOD)
        message(FATAL_ERROR "GPIOD not found!")
    endif(GPIOD)
    target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/hw/hardware-gpiod.c)
    target_link_libraries(${PROJECT_NAME} PUBLIC -lgpiod) 

endif()

link_directories(
    ${PROJECT_BINARY_DIR}
#    ${PROJECT_BINARY_DIR}/librelogic
)	

#minimal console plc for performance testing, etc.
add_executable(plclite
    ${PROJECT_SOURCE_DIR}/plclite.c 
)
add_dependencies(plclite ${PROJECT_NAME})
target_link_options(
	plclite PUBLIC -Wl,--gc-sections -Wl,--print-memory-usage 
)
target_link_libraries(
        plclite PUBLIC -llogic
) 



