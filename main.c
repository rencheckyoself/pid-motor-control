#include <stdio.h>
#include "NU32.h"          // constants, functions for startup and UART

#include "currentcontrol.h"
#include "encoder.h"
#include "isense.h"
#include "utilities.h"

#define BUF_SIZE 200

// GLOBAL VARIABLES //

// FUNCTION PROTOTYPES //
void initialize_peripherals();

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
      case 'f': // set pwm duty cycle
      {
        int n = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &n);

        set_mode(PWM);
        set_pwm(n); // Set pwm command and direction
        break;
      }
      case 'g': // set current control gains
      {
        float kp=0, ki=0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f", &kp, &ki);
        set_cc_gains(kp, ki);
        break;
      }
      case 'h': // get current control gains
      {
        float cc_gains[2];
        get_cc_gains(cc_gains);

        sprintf(buffer, "%f \n", cc_gains[0]);
        NU32_WriteUART3(buffer);
        sprintf(buffer, "%f \n", cc_gains[1]);
        NU32_WriteUART3(buffer);

        break;
      }
      case 'i': // set current control gains
      {
        float kp=0, ki=0, kd=0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f %f", &kp, &ki, &kd);
        set_pos_gains(kp, ki, kd);
        break;
      }
      case 'j': // get current control gains
      {
        float pos_gains[3];
        get_pos_gains(pos_gains);

        sprintf(buffer, "%f \n", pos_gains[0]);
        NU32_WriteUART3(buffer);
        sprintf(buffer, "%f \n", pos_gains[1]);
        NU32_WriteUART3(buffer);
        sprintf(buffer, "%f \n", pos_gains[2]);
        NU32_WriteUART3(buffer);

        break;
      }
      case 'k': // Test current gains
      {
        set_mode(ITEST);

        while(get_mode() != IDLE) { }; // wait for test to finish

        // return data
        int ref = 0, act = 0;

        int i = 0;
        for(i = 0; i < 99; i ++)
        {
          get_itest_data(&ref, &act, i);
          sprintf(buffer,"%d %d\n", ref, act);
          NU32_WriteUART3(buffer);
        }

        float cc_gains[2];
        get_cc_gains(cc_gains);
        sprintf(buffer, "%f \n", cc_gains[0]);
        NU32_WriteUART3(buffer);
        sprintf(buffer, "%f \n", cc_gains[1]);
        NU32_WriteUART3(buffer);

        break;
      }
      case 'l': // Go to anlge
      {
        int deg = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &deg);

        set_holding_position(deg);
        set_mode(HOLD);

        break;
      }
      case 'm': // load trajectory
      {

        break;
      }
      case 'n': // load trajectory
      {

        break;
      }
      case 'o': // execute trajectory
      {

        break;
      }
      case 'p': // Disable Motor
      {
        set_mode(IDLE);
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

  // Initialise all current control Peripherals
  init_curcont();

  // Initialise all position control Peripherals
  init_poscont();
}
