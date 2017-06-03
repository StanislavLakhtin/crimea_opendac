include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)

# Find the cross compiler
find_program(ARM_CC arm-none-eabi-gcc
            ${TOOLCHAIN_DIR}/bin)
find_program(ARM_CXX arm-none-eabi-g++
            ${TOOLCHAIN_DIR}/bin)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy
            ${TOOLCHAIN_DIR}/bin)
find_program(ARM_SIZE_TOOL arm-none-eabi-size
            ${TOOLCHAIN_DIR}/bin)
find_program(ARM_AS arm-none-eabi-as
            ${TOOLCHAIN_DIR}/bin)
find_program(ARM_AR arm-none-eabi-ar
            ${TOOLCHAIN_DIR}/bin)

SET(CMAKE_C_COMPILER ${ARM_CC})
SET(CMAKE_CXX_COMPILER ${ARM_CXX})
SET(CMAKE_C_ARCHIVE_CREATE ${ARM_AR})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)