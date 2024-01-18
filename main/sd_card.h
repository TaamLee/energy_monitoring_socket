
#define PIN_NUM_MISO  27
#define PIN_NUM_MOSI  26
#define PIN_NUM_CLK   25
#define PIN_NUM_CS    33
#define MOUNT_POINT "/sdcard"


void init_sdcard(void);
void data_logger(float irms,float vrms,double at_power,struct tm *time);