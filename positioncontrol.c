#include <string.h>

#include "NU32.h"
#include "encoder.h"
#include "currentcontrol.h"
#include "positioncontrol.h"
#include "utilities.h"

static volatile float gains[3] = {10, 0.0002, 500};
static const pr4max = 49999;
static const max_current = 762;

static volatile float hold_eint = 0;
static volatile float hold_eprev = 0;
static volatile int hold_sp = 0;

static volatile int traj_pos = 0;
static volatile int traj_size = 0;
static volatile int ref_traj[BUF_SIZE];
static volatile int act_traj[BUF_SIZE];
static volatile float track_eint = 0;
static volatile float track_eprev = 0;

static int hold_calc_control(float setpoint, float actual)
{
  int u = 0; // Calculated current Control in mA
  float error = setpoint - actual;

  hold_eint += error; // dt is factored into Ki
  float derror = error - hold_eprev;

  hold_eprev = error;

  u = gains[0]*error + gains[1]*hold_eint + gains[2]*derror;

  if(u > max_current) u = max_current;
  if(u < -max_current) u = -max_current;

  return u;
}

static int track_calc_control(float setpoint, float actual)
{
  int u = 0; // Calculated current Control in mA
  float error = setpoint - actual;

  track_eint += error; // dt is factored into Ki
  float derror = error - track_eprev;

  track_eprev = error;

  u = gains[0]*error + gains[1]*hold_eint + gains[2]*derror;

  if(u > max_current) u = max_current;
  if(u < -max_current) u = -max_current;

  return u;
}

// return a value of the actual trajectory
int get_traj_val(int index)
{
  return act_traj[index];
}

void set_ref_traj(float *traj, int traj_len)
{
  int i = 0;

  traj_size = traj_len;

  for(i = 0; i < traj_len; i++)
  {
    ref_traj[i] = traj[i];
  }

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
  switch (get_mode())
  {
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
    case TRACK:
    {
      if(traj_pos < traj_size)
      {
        // get current position
        int act_pos = encoder_angle();

        // Calculate control
        int cur_sp = track_calc_control(ref_traj[traj_pos], act_pos);

        // set current setpoint
        set_track_setpoint(cur_sp);

        act_traj[traj_pos] = act_pos;

        traj_pos++;
      }
      else
      {
        set_mode(IDLE);
        traj_pos = 0;
        track_eint = 0;
        track_eprev = 0;
      }
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
