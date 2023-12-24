//MQTT define
#define MQTT_URI    "mqtt://demo.thingsboard.io"
#define ACCESS_TOKEN "f5jllvxrrrxhiy8sce51"
static const char *TAG1 = "MQTT_EXAMPLE";


void mqtt_app_start(void);
void Send_data_to_cloud(float data,char* topic);