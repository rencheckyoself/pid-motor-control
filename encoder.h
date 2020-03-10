#ifndef ENCODER__HG
#define ENCODER__HG

#define ENCODER_MID_POINT 32768

// Initialize the encoder module.
void encoder_init();

// Read the encoder angle in ticks.
int encoder_ticks();

// Read the encoder angle in units of 0.1 degrees.
int encoder_angle();

// Reset the encoder position.
void encoder_reset();

// Read the encoder counts
int encoder_counts();

#endif
