#ifndef POSCONT__HG
#define POSCONT__HG

// return a value of the actual trajectory
int get_traj_val(int index);

//Set HOLD position of motor
void set_holding_position(int deg);

// Retrieve the current gain values
void get_pos_gains(float* gain_arr);

// Set new gain values
void set_pos_gains(float kp_new, float ki_new, float kd_new);

// Set up 200Hz ISR for position control
void init_poscont();

// Set a reference trajectory
void set_ref_traj(float *traj, int traj_len);
#endif
