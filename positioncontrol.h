#ifndef POSCONT__HG
#define POSCONT__HG

//Set HOLD position of motor
void set_holding_position(int deg);

// Retrieve the current gain values
void get_cc_gains(float* gain_arr);

// Set new gain values
void set_pos_gains(float kp_new, float ki_new, float kd_new);

// Set up 200Hz ISR for position control
void init_poscont();
#endif
