#include "NU32.h"
#include "encoder.h"
#include "currentcontrol.h"
#include "positioncontrol.h"
#include "utilities.h"

static volatile float gains[3] = {0,0,0};
static const pr4max = 49999;
static volatile float hold_eint = 0;

static volatile int hold_sp = 0;

static float hold_calc_control(float setpoint, float actual)
{
  float u = 0; // Calculated Control
  float error = setpoint - actual;

  hold_eint += error; // dt is factored into Ki

  u = gains[0]*error + gains[1]*hold_eint;

  if(u > 100) u = 100;
  if(u < -100) u = -100;

  return u;
}

void set_holding_position(int deg)
{
  hold_sp = deg;
}

void set_pos_gains(float kp_new, float ki_new, float kd_new)
{
  gains[0] = kp_new;
  gains[1] = ki_new;
  gains[2] = kd_new;
}

void get_pos_gains(float* gain_arr)
{
  gain_arr[0] = gains[0];
  gain_arr[1] = gains[1];
  gain_arr[2] = gains[2];
}

void __ISR(_TIMER_4_VECTOR, IPL6SOFT) PositionController(void)
{
  switch (get_mode()) {
    case HOLD:
    {
      // get current position
      int act_pos = encoder_angle();

      // Calculate control
      int cur_sp = hold_calc_control(hold_sp, act_pos);

      // set current setpoint
      set_hold_setpoint(cur_sp);

      break;
    }
  }

  IFS0bits.T4IF = 0;

}

void init_poscont()
{
  // 200Hz ISR
  // Timer 4 init
  T4CONbits.TCKPS = 3;    // Timer4 prescaler N=32
  PR4 = pr4max;               // 80MHz/200Hz = (PR4 + 1)N
  TMR4 = 0;                   // init T4 count to 0

  TRISDbits.TRISD9 = 0;
  LATDbits.LATD9 = 1;

  T4CONbits.ON = 1;

  IPC4bits.T4IP = 6;
  IFS0bits.T4IF = 0;
  IEC0bits.T4IE = 1;
}
