/* SPI Master Half Duplex EEPROM example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "spi_eeprom.h"


/*
 This code demonstrates how to use the SPI master half duplex mode to read/write a AT932C46D EEPROM (8-bit mode).
*/

#  define DEVICE_HOST    SPI2_HOST

#  define PIN_NUM_MISO 13
#  define PIN_NUM_MOSI 11
#  define PIN_NUM_CLK  12
#  define PIN_NUM_CS   10

static const char TAG[] = "main";

void app_main(void)
{
    esp_err_t ret;
    ESP_LOGI(TAG, "Initializing bus SPI%d...", DEVICE_HOST+1);
    spi_bus_config_t buscfg={
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    //Initialize the SPI bus
    ret = spi_bus_initialize(DEVICE_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    eeprom_config_t eeprom_config = {
        .cs_io = PIN_NUM_CS,
        .host = DEVICE_HOST,
        .miso_io = PIN_NUM_MISO,
    };
    
#ifdef CONFIG_EXAMPLE_INTR_USED
    eeprom_config.intr_used = true;
    gpio_install_isr_service(0);
#endif

    eeprom_handle_t eeprom_handle;

    ESP_LOGI(TAG, "Initializing device...");
    
    // "I (370) gpio: GPIO[10]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 " statemnet is here!
    ret = spi_eeprom_init(&eeprom_config, &eeprom_handle);
    ESP_ERROR_CHECK(ret);

    ret = spi_eeprom_write_enable(eeprom_handle);
    ESP_ERROR_CHECK(ret);
    // no need to write
    
    const char test_str[] = "Hello World!";


    uint8_t test_buf[32] = "";
    for (int i = 0; i < sizeof(test_str); i++) {

        ret = spi_eeprom_read(eeprom_handle, i, &test_buf[i]);
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGI(TAG, "Read: %s", test_buf);

    ESP_LOGI(TAG, "Example finished.");

    while (1) {
        // Add your main loop handling code here.
        for (int i = 0; i < sizeof(test_buf); i++) {
            ret = spi_eeprom_read(eeprom_handle, i, &test_buf[i]);
            ESP_ERROR_CHECK(ret);
        }
        ESP_LOGI(TAG, "Read: %s", test_buf);
        vTaskDelay(100);
    }
}
