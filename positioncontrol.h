#ifndef POSCONT__HG
#define POSCONT__HG

// Retrieve the current gain values
void get_cc_gains(float* gain_arr);

// Set new gain values
void set_cc_gains(float kp_new, float ki_new);


#endif
