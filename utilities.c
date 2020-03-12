
#include "utilities.h"

static volatile mode current_mode = IDLE;

void set_mode(mode new_mode)
{
  current_mode = new_mode;
}

mode get_mode(void)
{
  return current_mode;
}
