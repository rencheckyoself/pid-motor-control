#include <stdio.h>
#include "NU32.h"          // constants, functions for startup and UART

#include "encoder.h"
#include "utilities.h"
#include "isense.h"

#define BUF_SIZE 200

#define MOTOR_DIR LATEbits.LATE6

// GLOBAL VARIABLES //
static const int pr5max = 15999;
static const int pr2max = 3999;

unsigned int adc_sample_convert(int pin);
unsigned int calc_control(float setpoint, float actual);
void initialize_peripherals();

void __ISR(_TIMER_5_VECTOR, IPL5SOFT) Controller(void)
{

  OC1RS = 1000;             // duty cycle = OC1RS/(PR2+1) = 25%

  MOTOR_DIR = !MOTOR_DIR;

  switch (get_mode()) {
    case IDLE:
      // brake_mode();
      break;
    case PWM:
      // Set based on f Function
      break;
    case ITEST:

      break;
    case HOLD:

      break;
    case TRACK:

      break;
  }

  IFS0bits.T5IF = 0;
}

int main(void)
{
  NU32_Startup();          // cache on, interrupts on, LED/button init, UART init
  char buffer[BUF_SIZE];
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;

  __builtin_disable_interrupts(); // INT step 2: disable interrupts

  initialize_peripherals();

  __builtin_enable_interrupts();  // INT step 7: enable interrupts at CPU

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      // case 'd':                      // dummy command for demonstration purposes
      // {
      //   int n = 0;
      //   NU32_ReadUART3(buffer,BUF_SIZE);
      //   sscanf(buffer, "%d", &n);
      //   sprintf(buffer,"%d\r\n", n + 1); // return the number + 1
      //   NU32_WriteUART3(buffer);
      //   break;
      // }
      case 'a': // read raw adc value
      {
        sprintf(buffer, "%d\n", read_adc_value(3));
        NU32_WriteUART3(buffer);
        break;
      }
      case 'b': // read mA adc value
      {
        sprintf(buffer, "%d\n", read_adc_ma(10));
        NU32_WriteUART3(buffer);
        break;
      }
      case 'c': // read the raw encoder value
      {
        sprintf(buffer, "%d\n", encoder_counts());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'd': // read the deg. encoder value
      {
        sprintf(buffer, "%d\n", encoder_angle());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'e': // reset raw encoder value
      {
        encoder_reset();
        break;
      }
      case 'q': // quit
      {
        // handle q for quit. Later you may want to return to IDLE mode here.
        set_mode(IDLE);

        break;
      }
      case 'r': // Get current mode
      {
        sprintf(buffer, "%d\n", get_mode());
        NU32_WriteUART3(buffer);
        break;
      }
      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }

  return 0;
}

// unsigned int adc_sample_convert(int pin)
// {    // sample & convert the value on the given
//      // adc pin the pin should be configured as an
//      // analog input in AD1PCFG
//     unsigned int elapsed = 0, finish_time = 0;
//     AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
//     AD1CON1bits.SAMP = 1;                  // start sampling
//     elapsed = _CP0_GET_COUNT();
//     finish_time = elapsed + SAMPLE_TIME;
//     while (_CP0_GET_COUNT() < finish_time) {
//       ;                                   // sample for more than 250 ns
//     }
//     AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
//     while (!AD1CON1bits.DONE) {
//       ;                                   // wait for the conversion process to finish
//     }
//     return ADC1BUF0;                      // read the buffer with the result
// }

void initialize_peripherals()
{

  // Initialize Encoders (SPI4)
  encoder_init();

  // Set the mode
  set_mode(IDLE);

  //Initialize ADC
  init_adc();

  // Motor Digital I/O Init
  TRISEbits.TRISE6 = 0;
  MOTOR_DIR = 1;

  // 5kHz Timer on Timer 5 for ISR
  T5CONbits.TCKPS = 0;     // Timer5 prescaler N=1
  PR5 = pr5max;            // 80Mhz/5kHz = (PR5 + 1)N
  TMR5 = 0;                // initial TMR5 count is 0

  // Generate 20kHz PWM Signal
  // Timer 2 Init
  T2CONbits.TCKPS = 0;    // Timer2 prescaller N=1
  PR2 = pr2max;            // 80Mhz/20kHz = (PR2 + 1)N
  TMR2 = 0;                // initial TMR2 count is 0

  // Output Compare Init
  OC2CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC2CONbits.OCTSEL = 0;   // Select timer 2
  OC1RS = 1000;             // duty cycle = OC1RS/(PR2+1) = 25%
  OC1R = 1000;              // initialize before turning OC1 on; afterward it is read-only

  // Turn on Peripherals
  T2CONbits.ON = 1;        // turn on Timer2
  T5CONbits.ON = 1;        // turn on Timer5

  // Set 5kHz Interrupt
  IPC5bits.T5IP = 5;              // INT step 4: priority for Timer5
  IFS0bits.T5IF = 0;              // INT step 5: clear interrupt flag
  IEC0bits.T5IE = 1;              // INT step 6: enable interrupt
}
