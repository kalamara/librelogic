cmake_minimum_required(VERSION 3.18)

set(CMAKE_BUILD_TYPE Debug)

project("logic-apps")

include_directories(
	${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}
)
	

link_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/../build-lib
)	

#minimal console plc for performance testing, etc.
add_executable(plclite
    ${PROJECT_SOURCE_DIR}/plclite.c 
)
#add_dependencies(plclite logic)
target_link_options(
	plclite PUBLIC -Wl,--gc-sections -Wl,--print-memory-usage 
)
target_link_libraries(
        plclite PUBLIC -llogic
) 

#st lexer
add_executable(stlex
    ${PROJECT_SOURCE_DIR}/../src/vm/lex.yy.c
)



