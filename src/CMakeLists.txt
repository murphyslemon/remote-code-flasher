add_executable(${ProjectName}
        ipstack/IPStack.cpp
        ipstack/tls_common.c
        Fmutex.cpp
        main.cpp
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName}
    pico_stdlib
    FreeRTOS-Kernel-Heap4
    pico_cyw43_arch_lwip_sys_freertos
    pico_mbedtls
    pico_lwip_mbedtls
    )

pico_add_extra_outputs(${ProjectName})

# Disable usb output, enable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
