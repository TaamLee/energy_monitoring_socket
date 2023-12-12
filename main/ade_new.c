#include "ade_new.h"
uint32_t sum_irms = 0;
uint32_t sum_vrms = 0;
uint32_t energy_mill;
double energy =0;
uint8_t read_8bits_register(uint8_t reg_addr)
{
    uint8_t tx_data[2] = { reg_addr & 0x7F, 0 };  // Gửi byte đầu tiên là địa chỉ thanh ghi, byte thứ hai và ba để nhận dữ liệu
    uint8_t rx_data[2] = { 0 };               // Lưu trữ dữ liệu đọc được từ thanh ghi
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.flags = 0;
    t.length = 16;
    t.tx_buffer = tx_data;
    t.rxlength = 0;
    t.rx_buffer = rx_data;

    esp_err_t ret = spi_device_transmit(spi_device, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI transaction failed: %s", esp_err_to_name(ret));
        return 0;
    }
    return rx_data[1];
}
uint16_t read_16bits_register(uint8_t reg_addr)
{
    uint8_t tx_data[3] = { reg_addr & 0x7F, 0, 0 };  // Gửi byte đầu tiên là địa chỉ thanh ghi, byte thứ hai và ba để nhận dữ liệu
    uint8_t rx_data[3] = { 0 };               // Lưu trữ dữ liệu đọc được từ thanh ghi
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.flags = 0;
    t.length = 24;
    t.tx_buffer = tx_data;
    t.rxlength = 0;
    t.rx_buffer = rx_data;

    esp_err_t ret = spi_device_transmit(spi_device, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI transaction failed: %s", esp_err_to_name(ret));
        return 0;
    }

    uint16_t reg_value_16 = (uint16_t)(rx_data[1] << 8) | (uint16_t)(rx_data[2] & 0xFF);
    return reg_value_16;
}
uint32_t read_24bits_register(uint8_t reg_addr)
{
    uint8_t tx_data[4] = { reg_addr & 0x7F, 0, 0, 0 };  // Gửi byte đầu tiên là địa chỉ thanh ghi, byte thứ hai và ba để nhận dữ liệu
    uint8_t rx_data[4] = { 0 };               // Lưu trữ dữ liệu đọc được từ thanh ghi
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.flags = 0;
    t.length = 32;
    t.tx_buffer = tx_data;
    t.rxlength = 0;
    t.rx_buffer = rx_data;

    esp_err_t ret = spi_device_transmit(spi_device, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI transaction failed: %s", esp_err_to_name(ret));
        return 0;
    }

    uint32_t reg_value_32 = (uint32_t)(rx_data[1] << 16) | (uint32_t)(rx_data[2] << 8)| (uint32_t)(rx_data[3] & 0xFF);
    return reg_value_32;
}

void write_8bits_register(uint8_t reg, uint8_t value) {
    spi_transaction_t spi_transaction = {
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = {reg | ADE7753_WRITE_FLAG, value},
        .length = 16
    };
    esp_err_t ret = spi_device_transmit(spi_device, &spi_transaction);
        if (ret != ESP_OK) {
        printf("Lỗi khi truyền/nhận dữ liệu qua SPI: %d\n", ret);
    }
}
void write_16bits_register(uint8_t reg, uint16_t value) {
    spi_transaction_t spi_transaction = {
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = {reg | ADE7753_WRITE_FLAG,(uint8_t)(value >> 8), (uint8_t)(value & 0xFF)},
        .length = 24
    };
    esp_err_t ret = spi_device_transmit(spi_device, &spi_transaction);
        if (ret != ESP_OK) {
        printf("Lỗi khi truyền/nhận dữ liệu qua SPI: %d\n", ret);
    }
}
uint16_t getInterruptStatus(void){
    return read_16bits_register(ADE7753_STATUS_REG);
}
uint16_t getResetInterruptStatus(void){
    return read_16bits_register(RSTSTATUS);
}
float get_IRMS(int N){
    for (int i  = 0 ; i <10 ; i++)
    {
     sum_irms = sum_irms + read_24bits_register(IRMS);
    }
    float irms = (((float)sum_irms * 0.5)/(float)1868467)/(float)10 * 10.00;
    sum_irms = 0;
    return irms;
    // unsigned long IRMS_VAL = 0;
    // uint32_t lastupdate = 0;
    // for (int i = 0; i<N; i++){
    //     getResetInterruptStatus(); //clear interrupts
    //     lastupdate = xTaskGetTickCount();
    //     while(!(getInterruptStatus() & (ZX))){ 
    //         if((xTaskGetTickCount()-lastupdate)>100){
    //           return -1; //return on Timeout
    //         }
    //     }
    //     IRMS_VAL += read_24bits_register(IRMS);
    // }
    // return (((float)IRMS_VAL * 0.5)/(float)1900000)/(float)10 * 10.00; 
}
float get_VRMS(int N){
    for (int i  = 0 ; i <10 ; i++)
    {
     sum_vrms = sum_vrms + read_24bits_register(VRMS);
    }
    float vrms = (((float)sum_vrms * 0.5)/(float)1561400)/(float)10 * 781.00;
    sum_vrms = 0;
    if(vrms < 100)
        return 0;
    return vrms;
    // unsigned long VRMS_VAL = 0;
    // uint32_t lastupdate = 0;
    // for (int i = 0; i<N; i++){
    //     getResetInterruptStatus(); //clear interrupts
    //     lastupdate = (uint32_t)xTaskGetTickCount();
    //     while(!(getInterruptStatus() & (ZX))){ 
    //         if((xTaskGetTickCount()-lastupdate)>100){
    //           return -1; //return on Timeout
    //         }
    //     }
    //     VRMS_VAL += read_24bits_register(VRMS);
    // }
    // return (((float)VRMS_VAL * 0.5)/(float)1561400)/(float)10 * 781.00; 
}
double get_power_consumption()
{
    float power;
    //getResetInterruptStatus();
    while(!(getInterruptStatus() & (CYCEND)));
    power = read_24bits_register(ADE7753_LAENERGY_REG);
    power *= ke;
    energy=energy+(power*(xTaskGetTickCount()-energy_mill)/3600000.0);
    energy_mill = xTaskGetTickCount();
    printf("active power = %.2f \n",power);
    return energy;
}
void ade7753_init()
{
    //init SPI
    init_spi();
    //set MODE register
    write_16bits_register(ADE7753_MODE_REG,0x8C);
    //set gain
    write_8bits_register(ADE7753_GAIN_REG,GAIN_1);
    // set Cyc
    write_16bits_register(LINECYC, 0x78);
}