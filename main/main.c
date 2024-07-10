#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "driver/uart.h"
#include "gps_uart.h"

void app_main(void)
{
    for(;;)
    {
        init_gps_uart();
        read_gps_uart();
        read_gps_uart();
    }
}