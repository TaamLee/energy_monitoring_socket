#ifndef _SPI_H_
#define _SPI_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/spi_master.h"

extern spi_device_handle_t spi_device;
#define SPI_WRITE   1
#define SPI_READ    0
#define GPIO_MOSI_PIN 23
#define GPIO_MISO_PIN 19
#define GPIO_SCLK_PIN 18
#define GPIO_CS_PIN 5

void init_spi();

#endif 