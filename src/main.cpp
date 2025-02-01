#include <cstdio>
#include <cstring>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/stdio_uart.h"

#include "hardware/timer.h"
#include "event_groups.h"
#include "ipstack/IPStack.h"
//#include "uart/PicoOsUart.h"

#define HTTP_SERVER         "192.168.162.155"
#define BUFSIZE             2048
#define WIFI_SSID           "franks_galaxy"
#define WIFI_PASSWORD       "veef2267"

#define UART_ID uart0   // Use UART0
#define TX_PIN 0        // Replace with your TX pin
#define RX_PIN 1        // Replace with your RX pin
#define BAUD_RATE 115200

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_4 (1 << 4)

extern "C" {
uint32_t read_runtime_ctr(void) {
    return timer_hw->timerawl;
}
}

void print_binary(uint32_t num) {
    for (int i = 31; i >= 0; i--) {
        printf("%c", (num & (1 << i)) ? '1' : '0');
    }
    printf("\n");
}

void init_task(void *param) {
    auto init_complete_event = (EventGroupHandle_t*) param;
    stdio_uart_init_full(UART_ID, BAUD_RATE, TX_PIN, RX_PIN);
    printf("\nBoot\n");
    xEventGroupSetBits(*init_complete_event, BIT_0);
    //printf("bits: ");
    //print_binary(xEventGroupGetBits(*init_complete_event));
    while (true) {
        vTaskDelay(100);
    }
}

void tcp_server_task(void *pvParameters) {
    auto init_complete_event = (EventGroupHandle_t*) pvParameters;
    xEventGroupWaitBits(*init_complete_event, BIT_0 , pdFALSE, pdFALSE, portMAX_DELAY);
    //stdio_uart_init_full(UART_ID, BAUD_RATE, TX_PIN, RX_PIN);
    printf("TCP Server\n");
    vTaskDelay(1000);
    const char *msg = "Hello, Frank!";
    auto *buffer = new unsigned char[BUFSIZE];
    IPStack ipstack(WIFI_SSID, WIFI_PASSWORD);
    printf("bogady boo!\n");
    while(true) {
        int rc = ipstack.connect(HTTP_SERVER, 50372);
        if (rc == 0) {
            ipstack.write((unsigned char *) (msg), strlen(msg), 1000);
            auto rv = ipstack.read(buffer, BUFSIZE, 2000);
            buffer[rv] = 0;
            printf("rv=%d\n%s\n", rv, buffer);
            ipstack.disconnect();
        }
        else {
            printf("rc from TCP connect is %d\n", rc);
        }
    }
}

int main(void) {
    // create freeRTOS event group bits
    EventGroupHandle_t init_complete_event = xEventGroupCreate();
    // create init task
    TaskHandle_t init_task_handle;
    UBaseType_t uxCore1AffinityMask;
    xTaskCreate(init_task, "init", 1024, &init_complete_event, tskIDLE_PRIORITY + 1, &init_task_handle);
    uxCore1AffinityMask = ( 0x03); // should be uxCore1AffinityMask = ( ( 1 << 1 )); for core 1
    vTaskCoreAffinitySet( init_task_handle, uxCore1AffinityMask );

    TaskHandle_t tcp_server_task_handle;
    UBaseType_t uxCore0AffinityMask;
    xTaskCreate(tcp_server_task, "TCP", 4096, &init_complete_event, tskIDLE_PRIORITY + 2, &tcp_server_task_handle);
    uxCore0AffinityMask = 0x03;
    vTaskCoreAffinitySet( tcp_server_task_handle, uxCore0AffinityMask );

    vTaskStartScheduler();
    // never reached
    while (true) {};
    return 0;
}
