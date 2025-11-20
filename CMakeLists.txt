# CMakeLists.txt for STM32F407 + SE050 + mbedTLS TLS Client Example

cmake_minimum_required(VERSION 3.15)

# Project information
project(stm32_se050_tls_client C ASM)
set(PROJECT_VERSION 1.0.0)

# Toolchain configuration
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Required tools
find_program(ARM_GCC_TOOLCHAIN arm-none-eabi-gcc)
if(NOT ARM_GCC_TOOLCHAIN)
    message(FATAL_ERROR "ARM GCC toolchain not found")
endif()

# Set toolchain executables
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# Target microcontroller settings
set(MCU_FAMILY STM32F4xx)
set(MCU_LINE STM32F407xx)
set(MCU STM32F407VG)

# Compilation flags
set(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99 -Wall -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Linker script
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld)

# Include directories
include_directories(
    Core
    board
    Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
    Drivers/CMSIS/Core/Include
    Drivers/STM32${MCU_FAMILY}_HAL_Driver/Inc
    Drivers/STM32${MCU_FAMILY}_HAL_Driver/Inc/Legacy
    Middlewares/mbedtls/include
    Middlewares/plug-and-trust/hostlib/hostLib/libCommon/infra
    Middlewares/plug-and-trust/hostlib/hostLib/inc
    Middlewares/plug-and-trust/sss/inc
)

# Source files
file(GLOB_RECURSE CORE_SOURCES 
    "Core/*.c"
)

file(GLOB_RECURSE BOARD_SOURCES 
    "board/*.c"
)

file(GLOB_RECURSE HAL_SOURCES 
    "Drivers/STM32${MCU_FAMILY}_HAL_Driver/Src/*.c"
)

file(GLOB_RECURSE CMSIS_SOURCES 
    "Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Source/*.c"
)

file(GLOB_RECURSE MBEDTLS_SOURCES 
    "Middlewares/mbedtls/library/*.c"
)

file(GLOB_RECURSE PNT_SOURCES 
    "Middlewares/plug-and-trust/hostlib/hostLib/libCommon/infra/*.c"
    "Middlewares/plug-and-trust/hostlib/hostLib/se05x/*.c"
    "Middlewares/plug-and-trust/sss/src/*.c"
)

# All source files
set(SOURCES
    ${CORE_SOURCES}
    ${BOARD_SOURCES}
    ${HAL_SOURCES}
    ${CMSIS_SOURCES}
    ${MBEDTLS_SOURCES}
    ${PNT_SOURCES}
)

# Create executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Linker flags
target_link_libraries(${PROJECT_NAME}.elf -T${LINKER_SCRIPT} -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs)

# Generate binary output files
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMENT "Generating binary and hex files"
)

# Print size
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Memory usage:"
)