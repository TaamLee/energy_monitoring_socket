#ifndef _ADE7753_H_
#define _ADE7753_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "spi.h"

static const char *TAG = "ADE7753";




// Define pin connections for the ADE7753
#define ADE7753_CS_PIN 5
#define ADE7753_IRQ_PIN  2
#define CALIBRATION_FACTOR_EXAMPLE 0.98
#define ADE7753_MASK_SAGCYC 0x1C
#define ADE7753_WRITE 0x80

// Define hằng số tỉ lệ
#define kv 0.000000171849155                      
#define ki 0.00000039122624397277
#define ke 0.00041576242446899414
#define kt 2.2*pow(10,-6)

//constants
#define GAIN_1    0x00
#define GAIN_2    0x01
#define GAIN_4    0x02
#define GAIN_8    0x04
#define GAIN_16   0x08
#define INTEGRATOR_ON 1
#define INTEGRATOR_OFF 0
#define FULLSCALESELECT_0_5V    0x00
#define FULLSCALESELECT_0_25V   0x01
#define FULLSCALESELECT_0_125V  0x02


// Register addresses for the ADE7753
#define ADE7753_STATUS_REG      0x0B
#define ADE7753_MODE_REG        0x09
#define ADE7753_CONFIG_REG      0x0B
#define ADE7753_TEMP_REG        0x0E
#define ADE7753_LINECYC_REG     0x1C
#define ADE7753_LAST_RWDATA_REG 0x60
#define ADE7753_WRITE_FLAG      0x80
#define ADE7753_FREQ_REG        0x08
#define ADE7753_AENERGY_REG     0x02
#define ADE7753_CALIBRATION_REG 0x20

//Register address

//------Name--------Address---------Lenght
#define ADE7753_WAVEFORM_REG 	  0x01//			24
#define AENERGY 	  0x02//			24
#define RAENERGY  	0x03//			24
#define ADE7753_LAENERGY_REG 	  0x04//			24
#define VAENERGY 	  0x05//			24
#define RVAENERGY 	0x06//			24
#define LVAENERGY 	0x07//			24
#define LVARENERGY 	0x08//			24
#define ADE7753_MODE_REG 		    0x09//			16
#define ADE7753_IRQEN_REG 		  0x0A//			16
#define STATUSR 		0x0B//			16
#define RSTSTATUS 	0x0C//			16
#define CH1OS 		  0x0D//			8
#define CH2OS 		  0x0E//			8
#define ADE7753_GAIN_REG 		    0x0F//			8
#define PHCAL 		  0x10//			6
#define APOS 		    0x11//			16
#define WGAIN 		  0x12//			12
#define WDIV 		    0x13//			8
#define CFNUM 		  0x14//			12
#define CFDEN 		  0x15//			12
#define IRMS 		    0x16//			24
#define VRMS 		    0x17//			24
#define IRMSOS 		  0x18//			12
#define VRMSOS 		  0x19//			12
#define VAGAIN 		  0x1A//			12
#define VADIV 		  0x1B//			8
#define LINECYC 	  0x1C//			16
#define ZXTOUT 		  0x1D//			12
#define SAGCYC 		  0x1E//			8
#define SAGLVL 		  0x1F//			8
#define IPKLVL 		  0x20//			8
#define VPKLVL 		  0x21//			8
#define IPEAK 		  0x22//			24
#define RSTIPEAK 	  0x23//			24
#define VPEAK 		  0x24//			24
#define RSTVPEAK  	0x25//			24
#define TEMP 		    0x26//			8
#define PERIOD 		  0x27//			16
#define TMODE 		  0x3D//			8
#define CHKSUM 		  0x3E//			6
#define DIEREV 		  0X3F//			8

#define CYCEND  1<<2
#define		ZX		0x0010



// Function prototypes
uint8_t read_8bits_register(uint8_t reg_addr);
uint16_t read_16bits_register(uint8_t reg_addr);
uint32_t read_24bits_register(uint8_t reg_addr);
void write_8bits_register(uint8_t reg, uint8_t value);
void write_16bits_register(uint8_t reg, uint16_t value);
uint16_t getInterruptStatus(void);
uint16_t getResetInterruptStatus(void);
float get_IRMS(int N);
float get_VRMS(int N);
double get_power_consumption(void);
void ade7753_init();

#endif /* _ADE7753_H_ */