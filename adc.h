#ifndef ADC_H_
#define ADC_H_

#include <iostream>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdint>

#define I2C_DELAY 100000

using namespace std;

void init();

uint16_t get_adc_value(uint8_t channel);

#endif