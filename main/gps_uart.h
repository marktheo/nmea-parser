#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "driver/uart.h"
#include "esp_log.h"

void init_gps_uart(void);
void read_gps_uart(void);
void save_gps_uart(void);

const uart_port_t gps_uart = UART_NUM_0;
const int gps_uart_buffer_size = 1024;
const int gps_uart_buffer_gga_commas[6] = {16, 27, 29, 41, 43, 45};

uint8_t gps_uart_buffer_gga[78] = {0};

char gps_uart_buffer_gga_latitude_degree[2] = {0};
char gps_uart_buffer_gga_latitude_decimal[8] = {0};

int gps_uart_buffer_gga_latitude_degrees = 0;
double gps_uart_buffer_gga_latitude_decimals = 0;

char gps_uart_buffer_gga_longitude_degree[3] = {0};
char gps_uart_buffer_gga_longitude_decimal[8] = {0};

int gps_uart_buffer_gga_longitude_degrees = 0;
double gps_uart_buffer_gga_longitude_decimals = 0;

void init_gps_uart(void)
{
    uart_config_t gps_uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_REF_TICK,
    };

    ESP_ERROR_CHECK(uart_param_config(gps_uart, &gps_uart_config));
    ESP_ERROR_CHECK(uart_set_pin(gps_uart, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_delete(gps_uart));
    ESP_ERROR_CHECK(uart_driver_install(gps_uart, gps_uart_buffer_size * 2, 0, 0, NULL, 0));
}

void read_gps_uart(void)
{
    uint8_t *gps_uart_buffer = (uint8_t *) malloc ((gps_uart_buffer_size) * sizeof(uint8_t));
    char* gps_uart_buffer_gga_pointer = NULL;

    int gps_uart_buffer_gga_index_alpha = 0;
    int gps_uart_buffer_gga_index_omega = 0;
    
    int gps_uart_buffer_len = uart_read_bytes(gps_uart, gps_uart_buffer, gps_uart_buffer_size, 1000 / portTICK_PERIOD_MS);
    gps_uart_buffer_gga_pointer = strstr((char *)gps_uart_buffer, "$GNGGA,");

    if (gps_uart_buffer_len > 0 && gps_uart_buffer_gga_pointer != NULL)
    {
        gps_uart_buffer_gga_index_alpha = (int)(gps_uart_buffer_gga_pointer) - (int)(gps_uart_buffer);
        
        for (int i = 0; i < 80; i++)
        {
            if(gps_uart_buffer[gps_uart_buffer_gga_index_alpha + i] == '*')
            {
                gps_uart_buffer_gga_index_omega = gps_uart_buffer_gga_index_alpha + i + 2;
                break;
            }
        }

        for (int i = 0; i <= gps_uart_buffer_gga_index_omega - gps_uart_buffer_gga_index_alpha; i++)
        {
            gps_uart_buffer_gga[i] = gps_uart_buffer[gps_uart_buffer_gga_index_alpha + i];
        }
        
        free(gps_uart_buffer);
        
        if(gps_uart_buffer_gga[gps_uart_buffer_gga_commas[0]] != ',')
        {
            read_gps_uart();
        }

        else if(gps_uart_buffer_gga[gps_uart_buffer_gga_commas[1]] != ',')
        {
            read_gps_uart();
        }

        else if(gps_uart_buffer_gga[gps_uart_buffer_gga_commas[2]] != ',')
        {
            read_gps_uart();
        }

        else if(gps_uart_buffer_gga[gps_uart_buffer_gga_commas[3]] != ',')
        {
            read_gps_uart();
        }

        else if(gps_uart_buffer_gga[gps_uart_buffer_gga_commas[4]] != ',')
        {
            read_gps_uart();
        }

        else if(gps_uart_buffer_gga[gps_uart_buffer_gga_commas[5]] != ',')
        {
            read_gps_uart();
        }

        else
        {
            ESP_LOGI("GPS", "Sentence[%d]: %s\n", gps_uart_buffer_gga_index, gps_uart_buffer_gga);
        }
    }

    else
    {
        free(gps_uart_buffer);
        read_gps_uart();
    }
}

void save_gps_uart(void)
{
    gps_uart_buffer_gga_latitude_degree[0] = gps_uart_buffer_gga[17];
    gps_uart_buffer_gga_latitude_degree[1] = gps_uart_buffer_gga[18];
    gps_uart_buffer_gga_latitude_degrees = atoi(gps_uart_buffer_gga_latitude_degree);

    gps_uart_buffer_gga_latitude_decimal[0] = gps_uart_buffer_gga[19];
    gps_uart_buffer_gga_latitude_decimal[1] = gps_uart_buffer_gga[20];
    gps_uart_buffer_gga_latitude_decimal[2] = gps_uart_buffer_gga[21];
    gps_uart_buffer_gga_latitude_decimal[3] = gps_uart_buffer_gga[22];
    gps_uart_buffer_gga_latitude_decimal[4] = gps_uart_buffer_gga[23];
    gps_uart_buffer_gga_latitude_decimal[5] = gps_uart_buffer_gga[24];
    gps_uart_buffer_gga_latitude_decimal[6] = gps_uart_buffer_gga[25];
    gps_uart_buffer_gga_latitude_decimal[7] = gps_uart_buffer_gga[26];
    gps_uart_buffer_gga_latitude_decimals = strtod(gps_uart_buffer_gga_latitude_decimal, NULL);
    gps_uart_buffer_gga_latitude_decimals /= 60;

    gps_uart_buffer_gga_longitude_degree[0] = gps_uart_buffer_gga[30];
    gps_uart_buffer_gga_longitude_degree[1] = gps_uart_buffer_gga[31];
    gps_uart_buffer_gga_longitude_degree[2] = gps_uart_buffer_gga[32];
    gps_uart_buffer_gga_longitude_degrees = atoi(gps_uart_buffer_gga_longitude_degree);

    gps_uart_buffer_gga_longitude_decimal[0] = gps_uart_buffer_gga[33];
    gps_uart_buffer_gga_longitude_decimal[1] = gps_uart_buffer_gga[34];
    gps_uart_buffer_gga_longitude_decimal[2] = gps_uart_buffer_gga[35];
    gps_uart_buffer_gga_longitude_decimal[3] = gps_uart_buffer_gga[36];
    gps_uart_buffer_gga_longitude_decimal[4] = gps_uart_buffer_gga[37];
    gps_uart_buffer_gga_longitude_decimal[5] = gps_uart_buffer_gga[38];
    gps_uart_buffer_gga_longitude_decimal[6] = gps_uart_buffer_gga[39];
    gps_uart_buffer_gga_longitude_decimal[7] = gps_uart_buffer_gga[40];
    gps_uart_buffer_gga_longitude_decimals = strtod(gps_uart_buffer_gga_longitude_decimal, NULL);
    gps_uart_buffer_gga_longitude_decimals /= 60;

    ESP_LOGI("GPS UART", "Latitude: %.8f", gps_uart_buffer_gga_latitude_degrees + gps_uart_buffer_gga_latitude_decimals);
    ESP_LOGI("GPS UART", "Longitude: %.8f", gps_uart_buffer_gga_longitude_degrees + gps_uart_buffer_gga_longitude_decimals);
}