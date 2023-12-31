#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <arpa/inet.h>
#include "lwip/opt.h"


#include "lwip/err.h"
#include "lwip/sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_err.h"

#include "ade_new.h"
#include "wifi_conection.h"
#include "mqtt.h"
#include "oled_1306.h"

#define CONFIG_SDA_GPIO 21
#define CONFIG_SCL_GPIO 22
#define CONFIG_RESET_GPIO 33

typedef enum{
  IRMS_VALUE = 0,
  VRMS_VALUE ,
  POWER_VALUE 
}ID_e;
static const char *TAG2 = "main";
typedef struct{
  float data; // the value of irms,vrms,power
  ID_e id; // this Id indicates which the value it does
}Data_t;

QueueHandle_t data_queue; // hold data to send to cloud
QueueHandle_t displaying_queue; // hold data to display to oled
SemaphoreHandle_t xsema_handle;
float irms,vrms;
double at_power;
double last_energy;

nvs_handle_t my_handle;

//i2c variables
SSD1306_t dev;


void get_irms_task(void* parameters)
{
  Data_t data_q;
  uint8_t flag = 0;
  while(1){
    if( xSemaphoreTake( xsema_handle, 0 ))
    {
      irms = get_IRMS(10);
      flag = 1;
      xSemaphoreGive( xsema_handle );
    }
    if (flag)
    {
      printf("irms = %.2f\n",irms);
      //send irms to queue
      data_q.data = irms;
      data_q.id = IRMS_VALUE;
      xQueueSend(data_queue,&data_q,(TickType_t) 0);
      xQueueSend(displaying_queue,&data_q,(TickType_t) 0);
      flag = 0;
      vTaskDelay(2000/portTICK_PERIOD_MS);
    }
  }
}
void get_vrms_task(void* parameters)
{
  Data_t data_q;
  uint8_t flag = 0;
  while(1){
    if( xSemaphoreTake( xsema_handle, 0 ))
    {
      vrms = get_VRMS(10);
      flag = 1;
      xSemaphoreGive( xsema_handle );
    }
    if (flag)
    {
      printf("vrms = %.2f \n",vrms);
      //send vrms to queue
      data_q.data = vrms;
      data_q.id = VRMS_VALUE;
      xQueueSend(data_queue,&data_q,(TickType_t) 0);
      xQueueSend(displaying_queue,&data_q,(TickType_t) 0);
      flag = 0;
      vTaskDelay(2000/portTICK_PERIOD_MS);
    }
  }
}
void get_energy_task(void* paragrams)
{  
  Data_t data_q;
  uint8_t flag = 0;
  while(1)
  {
    if( xSemaphoreTake( xsema_handle, 0 ))
    {
      at_power = get_power_consumption();
      flag = 1;
      xSemaphoreGive( xsema_handle );
    }
    if (flag)
    {
      printf("power = %.2f  \n",at_power);
      //send active power to queue
      data_q.data = at_power;
      data_q.id = POWER_VALUE;
      xQueueSend(data_queue,&data_q,(TickType_t) 0);
      xQueueSend(displaying_queue,&data_q,(TickType_t) 0);
      flag = 0;
      vTaskDelay(2000/portTICK_PERIOD_MS);
    }
  }
    
}
void send_to_cloud_task(void* parameters)
{
  Data_t data_dequeue;
  char data_string[15];
  while(1){
    if (xQueueReceive(data_queue,&data_dequeue,0 ) == pdPASS)
    {
      if (data_dequeue.id == IRMS_VALUE){
        //send IRMS to thingboard
        Send_data_to_cloud(data_dequeue.data,"irms");
      }
      else if (data_dequeue.id == VRMS_VALUE){
        //send VRMS to thingboard
        Send_data_to_cloud(data_dequeue.data,"vrms");
      }
      else if (data_dequeue.id == POWER_VALUE){
        //send POWER CONSUMPTION to thingboard
        Send_data_to_cloud(data_dequeue.data,"power");
      }
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}
void display_task(void* parameters)
{
  Data_t data_dequeue;
  char data_string[15];
  while(1){
    if (xQueueReceive(displaying_queue,&data_dequeue,portMAX_DELAY ) == pdPASS)
    {
      sprintf(data_string,"%.2f       ",data_dequeue.data);
      if (data_dequeue.id == IRMS_VALUE)
        ssd1306_display_text(&dev, 0, data_string , strlen(data_string),6, false);
      else if (data_dequeue.id == VRMS_VALUE)
        ssd1306_display_text(&dev, 1, data_string , strlen(data_string),6, false);
      else
        ssd1306_display_text(&dev, 2, data_string , strlen(data_string),6, false);
    }

  }
}
void storing_energy_task(void* parameters)
{
  while(1){
        int err = nvs_set_i32(my_handle, "energy", at_power);
        printf((err != ESP_OK) ? "Failed to save energy!\n" : "save_energy_done\n");
        vTaskDelay(60000/portTICK_PERIOD_MS);
  }
}
void init_oled (void)
{
  i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, -1);
  ssd1306_init(&dev, 128, 64);
  ssd1306_clear_screen(&dev, false);
  ssd1306_contrast(&dev, 0xff);

  ssd1306_display_text(&dev, 0, "Irms =" , 6,0, false);
  ssd1306_display_text(&dev, 1, "Vrms =" , 6,0, false);
  ssd1306_display_text(&dev, 2, "KWh =" , 5,0, false);
}

void app_main(void)
{
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  //open NVS storge
  ret = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (ret != ESP_OK) {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
  }
  else{
    printf("open NVS successful!\n");
    //get actice power before power off
    int err = nvs_get_i32(my_handle, "energy", &at_power);
    switch (err) {
    case ESP_OK:
        printf("Done\n");
        printf("energy = %lf\n", at_power);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        printf("The value is not initialized yet!\n");
        break;
    default :
        printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }

  //create queue
  data_queue = xQueueCreate( 10, sizeof(Data_t) );
  displaying_queue = xQueueCreate( 10, sizeof(Data_t) );
  //create semaphore
  xsema_handle = xSemaphoreCreateBinary();
  xSemaphoreGive(xsema_handle);

  //init wifi and smartconfig
  initialise_wifi();
  //init ade7753
  ade7753_init();
  //init oled
  init_oled();
  //mqtt init
  mqtt_app_start();

  xTaskCreatePinnedToCore( get_irms_task, "get_irms_task", 4*1024, NULL,4,NULL, 1 );
  xTaskCreatePinnedToCore( get_vrms_task, "get_vrms_task", 4*1024, NULL,4,NULL, 1 );
  xTaskCreatePinnedToCore( get_energy_task, "get_energy_task", 4*1024, NULL,4,NULL, 1 );
  xTaskCreatePinnedToCore( send_to_cloud_task, "send_to_cloud_task", 5*1024, NULL,5,NULL, 0 );
  xTaskCreatePinnedToCore( display_task, "display_task", 4*1024, NULL,4,NULL, 1 );
  xTaskCreatePinnedToCore( storing_energy_task, "display_task", 4*1024, NULL,4,NULL, 1 );
  
}
