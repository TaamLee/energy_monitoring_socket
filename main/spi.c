#include "spi.h"
#include "string.h"
spi_device_handle_t spi_device;

void init_spi()
{
    esp_err_t ret; 
    // cấu hình giao diện SPI
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GPIO_MOSI_PIN,
        .miso_io_num = GPIO_MISO_PIN,
        .sclk_io_num = GPIO_SCLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        //.max_transfer_sz = 32
    };
    
    ret = spi_bus_initialize(VSPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);

    // Khai báo các thông số cho SPI
    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .clock_speed_hz = 150000,           // Tốc độ SPI là 10MHz
        .mode = 1,                                    // Chế độ SPI là 1
        .spics_io_num = GPIO_CS_PIN,                           // GPIO 14 được sử dụng để điều khiển chip select
        .queue_size = 3,
        .cs_ena_posttrans = 1, // Keep the CS low 3 cycles after transaction
    };

    // Thêm thiết bị SPI vào bus
    ret = spi_bus_add_device(VSPI_HOST, &dev_cfg, &spi_device);
    assert(ret == ESP_OK);

}
