#define ZENOH_ROUTER_ADDRESS "serial/UART_1#baudrate=115200"
#define ZENOH_NODE_NAME "mi_node"

#include "picorosso.h"
PicoRosso picorosso;

#include "ticker.h"
Ticker ticker;

extern "C"
{
    void app_main(void);
}

void app_main()
{
    // PicoRosso initalization //////////////////////////////
    picorosso.setup(ZENOH_NODE_NAME, ZENOH_ROUTER_ADDRESS);

    ticker.setup("tick");

    while (true)
    {
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}