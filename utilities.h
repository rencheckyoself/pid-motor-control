#ifndef UTILS__HG
#define UTILS__HG

typedef enum mode {IDLE, PWM, ITEST, HOLD, TRACK} mode;

// Sets the mode
void set_mode(mode new_mode);

// gets the mode
mode get_mode(void);

#endif
