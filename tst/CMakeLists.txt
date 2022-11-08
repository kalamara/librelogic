cmake_minimum_required(VERSION 3.18)

set(CMAKE_BUILD_TYPE Debug)
set(REMOTE-UI OFF)

project("librelogic-test")

find_library(CUNIT cunit)

include_directories(
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/../src
	${PROJECT_SOURCE_DIR}/../include
)
	
# test targets
if(CUNIT)
    add_executable(test_vm
        ${PROJECT_SOURCE_DIR}/ut-vm.c
        ${PROJECT_SOURCE_DIR}/vm-stubs.c
        ${PROJECT_SOURCE_DIR}/../src/vm/data.c
        ${PROJECT_SOURCE_DIR}/../src/vm/instruction.c
        ${PROJECT_SOURCE_DIR}/../src/vm/rung.c
        ${PROJECT_SOURCE_DIR}/../src/vm/plclib.c
        ${PROJECT_SOURCE_DIR}/../src/vm/parser-il.c
        ${PROJECT_SOURCE_DIR}/../src/vm/parser-ld.c
        ${PROJECT_SOURCE_DIR}/../src/vm/parser-tree.c
        ${PROJECT_SOURCE_DIR}/../src/vm/codegen.c
    )

    target_link_libraries(
    test_vm PUBLIC ${CUNIT} -lgcov -fsanitize=address
    )	
endif(CUNIT)    





