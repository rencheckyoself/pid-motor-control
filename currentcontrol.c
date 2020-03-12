#include "NU32.h"
#include "currentcontrol.h"
#include "utilities.h"

static const int pr5max = 15999;
static const int pr2max = 3999;

static volatile int current_duty_cycle = 0;
static volatile int current_direction = 0;

static volatile float gains[2] = {0,0};

static volatile int itest_cnt = 0, itest_sp = 200;

static volatile float itest_eint = 0;

static volatile int itest_ref[100], itest_act[100];

static void brake_mode()
{
  set_pwm(0);
  OC2RS = 0;
}

void set_pwm(int pwm)
{

  if (pwm < 0) // Reverse
  {
    if (pwm < -100) pwm = -100;

    current_duty_cycle = pwm * -1;
    current_direction = 1;
  }
  else // Forward
  {
    if (pwm > 100) pwm = 100;

    current_duty_cycle = pwm;
    current_direction = 0;
  }
}

static void issue_pwm()
{
  OC2RS = (int) (current_duty_cycle * (pr2max+1))/100; // set duty cycle
  MOTOR_DIR = current_direction;
}

static float calc_control(float setpoint, float actual)
{
  float u = 0; // Calculated Control
  float error = setpoint - actual;

  itest_eint += error; // dt is factored into Ki

  u = gains[0]*error + gains[1]*itest_eint;

  if(u > 100) u = 100;
  if(u < -100) u = -100;

  return u;
}

void set_cc_gains(float kp_new, float ki_new)
{
  gains[0] = kp_new;
  gains[1] = ki_new;
}

void get_cc_gains(float* gain_arr)
{
  gain_arr[0] = gains[0];
  gain_arr[1] = gains[1];
}

void get_itest_data(int *ref_val, int *act_val, int index)
{
  *ref_val = itest_ref[index];
  *act_val = itest_act[index];
}

void __ISR(_TIMER_5_VECTOR, IPL5SOFT) Controller(void)
{
  switch (get_mode()) {
    case IDLE:
    {
      brake_mode();
      break;
    }
    case PWM:
    {
      issue_pwm();
      break;
    }
    case ITEST:
    {
      // get current sensor value
      int cur_val = read_adc_ma(15);

      // get setpoint
      switch (itest_cnt)
      {
        case 0:
        {
          itest_sp = 200;
          itest_cnt++;
          break;
        }
        case 24:
        {
          itest_sp = -200;
          itest_cnt++;
          break;
        }
        case 49:
        {
          itest_sp = 200;
          itest_cnt++;
          break;
        }
        case 74:
        {
          itest_sp = -200;
          itest_cnt++;
          break;
        }
        case 99:
        {
          set_mode(IDLE);
          itest_cnt = 0;
          break;
        }
        default:
        {
          itest_cnt++;
        }
      }

      // Calculate Control
      int pwm_cmd = calc_control(itest_sp, cur_val);

      set_pwm(pwm_cmd);
      issue_pwm();

      itest_ref[itest_cnt] = itest_sp;
      itest_act[itest_cnt] = cur_val;


      break;
    }
    case HOLD:
    {
      break;
    }
    case TRACK:
    {
      break;
    }
  }

  IFS0bits.T5IF = 0;
}


void init_curcont()
{
  // Motor Digital I/O Init
  TRISEbits.TRISE6 = 0;
  MOTOR_DIR = 1;

  // 5kHz Timer on Timer 5 for ISR
  T5CONbits.TCKPS = 0;     // Timer5 prescaler N=1
  PR5 = pr5max;            // 80Mhz/5kHz = (PR5 + 1)N
  TMR5 = 0;                // initial TMR5 count is 0

  // Generate 20kHz PWM Signal
  // Timer 2 Init
  T2CONbits.TCKPS = 0;    // Timer2 prescaller N=1
  PR2 = pr2max;            // 80Mhz/20kHz = (PR2 + 1)N
  TMR2 = 0;                // initial TMR2 count is 0

  // Output Compare Init
  OC2CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC2CONbits.OCTSEL = 0;   // Select timer 2
  OC2RS = 0;             // duty cycle = OC2RS/(PR2+1) = 25%
  OC2R = 0;              // initialize before turning OC2 on; afterward it is read-only

  // Turn on Peripherals
  T2CONbits.ON = 1;        // turn on Timer2
  T5CONbits.ON = 1;        // turn on Timer5
  OC2CONbits.ON = 1;       // turn on OC2

  // Set 5kHz Interrupt
  IPC5bits.T5IP = 5;              // INT step 4: priority for Timer5
  IFS0bits.T5IF = 0;              // INT step 5: clear interrupt flag
  IEC0bits.T5IE = 1;              // INT step 6: enable interrupt
}
