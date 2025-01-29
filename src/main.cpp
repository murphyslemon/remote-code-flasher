#include <cstdio>
#include <cstring>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"

#include "hardware/timer.h"
#include "event_groups.h"
#include "ipstack/IPStack.h"
//#include "uart/PicoOsUart.h"

#define HTTP_SERVER         "127.0.0.1"
#define BUFSIZE             2048
#define WIFI_SSID           "franks_galaxy"
#define WIFI_PASSWORD       "veef2267"

#if 1
#define UART_NR 0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#else
#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#endif

#define BAUD_RATE 115200
#define STOP_BITS 1

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
    //stdio_init_all();
    //auto uart{std::make_shared<PicoOsUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE, STOP_BITS)};
    //uart->send("\n\rBoot\n\r");
    printf("\nBoot\n");
    xEventGroupSetBits(*init_complete_event, BIT_0);
    xEventGroupSetBits(*init_complete_event, BIT_1);
    xEventGroupSetBits(*init_complete_event, BIT_2);
    //print bits in binary form
    printf("bits: ");
    print_binary(xEventGroupGetBits(*init_complete_event));
    while (true) {
        vTaskDelay(1000);
    }
}

void tcp_server_task(void *pvParameters) {
    auto init_complete_event = (EventGroupHandle_t*) pvParameters;
    printf("i amasndbfgkabsdv\n");
    xEventGroupWaitBits(*init_complete_event, BIT_1 , pdFALSE, pdFALSE, portMAX_DELAY);
    vTaskDelay(1000);
    printf("i am here2\n");
    const char *msg = "Hello, Frank!";
    printf("\nconnecting...\n");
    auto *buffer = new unsigned char[BUFSIZE];
    IPStack ipstack(WIFI_SSID, WIFI_PASSWORD);

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
    stdio_init_all();
    sleep_ms(1000);
    printf("Hello, world!\n");
    // create freeRTOS event group bits
    EventGroupHandle_t init_complete_event = xEventGroupCreate();
    // create init task
    xTaskCreate(init_task, "init", 1024, &init_complete_event, 1, NULL);
    xTaskCreate(tcp_server_task, "TCP", 6000, &init_complete_event, 1, NULL);
    vTaskStartScheduler();
    // never reached
    while (true) {};
    return 0;
}
