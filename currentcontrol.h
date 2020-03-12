#ifndef CURRENTCONTROL__HG
#define CURRENTCONTROL__HG

#define MOTOR_DIR LATEbits.LATE6

// Set the pwm duty cycle
void set_pwm(int pwm);

// initialize current controller
void init_curcont();

// Retrieve the current gain values
void get_cc_gains(float* gain_arr);

// Set new gain values
void set_cc_gains(float kp_new, float ki_new);

#endif
