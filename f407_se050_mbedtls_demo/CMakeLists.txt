cmake_minimum_required(VERSION 3.15)
project(f407_se050_mbedtls_demo C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Os -g")
add_compile_options(${MCU_FLAGS})
add_link_options(${MCU_FLAGS})

include_directories(include middleware/sss mbedtls/include)
file(GLOB SRC "src/*.c" "middleware/sss/src/*.c" "mbedtls/library/*.c")
add_executable(f407_se050_demo ${SRC})

set(LINKER_SCRIPT "STM32F407VG_FLASH.ld")
target_link_options(f407_se050_demo PRIVATE "-T${LINKER_SCRIPT}")
