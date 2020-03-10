#include "NU32.h"          // constants, functions for startup and UART
#include "isense.h"

static unsigned int adc_sample_convert()
{    // sample & convert the value on the given
     // adc pin the pin should be configured as an
     // analog input in AD1PCFG
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = ADC_PIN;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) {
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}

int read_adc_ma(unsigned int avg)
{
  return 5.645 * read_adc_value(avg) - 2789.1;
}

int read_adc_value(unsigned int avg)
{
  int reading = 0;
  int i = 0;
  if(avg > 50) avg = 50;

  for(i = 0; i < avg; i++)
  {
    reading += adc_sample_convert();
  }

  return reading/avg;
}

void init_adc()
{
  AD1CON1bits.ASAM = 0;                   // turn on manual sampling
  AD1PCFGbits.PCFG0 = ADC_PIN;                  // AN0 is an adc pin
  AD1CON3bits.ADCS = 2;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb =
  //                           2*3*12.5ns = 75n
  AD1CON1bits.ADON = 1;                   // turn on A/D converter
}
