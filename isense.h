#ifndef ISENSE__HG
#define ISENSE__HG

#define ADC_PIN 0
#define SAMPLE_TIME 10  // 10 core timer ticks = 250 ns

// initialize adc
void init_adc();

// value read by the adc averaged across avg reads
int read_adc_value(unsigned int avg);

// value read by the adc averaged across avg reads converted to mA
int read_adc_ma(unsigned int avg);

#endif
