#include "NU32.h"
#include "positioncontrol.h"
#include "utilities.h"

void set_pos_gains(float kp_new, float ki_new)
{
  gains[0] = kp_new;
  gains[1] = ki_new;
}

void get_pos_gains(float* gain_arr)
{
  gain_arr[0] = gains[0];
  gain_arr[1] = gains[1];
}
