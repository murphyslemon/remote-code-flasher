cmake_minimum_required(VERSION 3.12)

#Set board type
set(PICO_BOARD pico_w)

# Name project
SET(ProjectName franks_testing)

# Set any variables required for importing libraries
SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)

# Import those libraries
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# Define project
project(${ProjectName})

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(src)

target_include_directories(${ProjectName} PRIVATE
        ${FREERTOS_KERNEL_PATH}/include
        ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040
        ${PICO_SDK_PATH}/src/common/pico_base/include
        ${PICO_SDK_PATH}/src/rp2_common/hardware_uart/include
        ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio/include
        ${PICO_SDK_PATH}/src/rp2_common/hardware_clocks/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_stdlib/include
        src
        src/ipstack
        src/uart
        ${PICO_SDK_PATH}/lib/mbedtls/include
)