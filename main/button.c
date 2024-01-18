#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "button.h"

extern nvs_handle_t my_handle;
static TaskHandle_t button_handle;


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    xTaskNotifyFromISR(button_handle,0,0,NULL);
}

static void gpio_task_example(void* arg)
{
    for(;;) {
        xTaskNotifyWait(0,0,NULL,portMAX_DELAY );
        int32_t dummy =0;
        int err = nvs_set_i32(my_handle, "energy", dummy);
        printf((err != ESP_OK) ? "Failed to save energy = 0 !\n" : "save_energy = 0 done\n");
        printf("interrupt happened \n");
        esp_restart();
    }
}
void init_button(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //interrupt of risinag edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    //start gpio task
    xTaskCreatePinnedToCore(gpio_task_example, "gpio_task_example", 2048, NULL, 10, &button_handle,1);
    
    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_handler, (void*) GPIO_INPUT_IO);

}