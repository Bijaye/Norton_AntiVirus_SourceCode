/*
Copyright Notice
================
BOCHS is Copyright 1994,1995,1996 by Kevin P. Lawton.

BOCHS is commercial software.

For more information, read the file 'LICENSE' included in the bochs
distribution.  If you don't have access to this file, or have questions
regarding the licensing policy, the author may be contacted via:

    US Mail:  Kevin Lawton
              528 Lexington St.
              Waltham, MA 02154

    EMail:    bochs@world.std.com
*/



#include "bochs.h"


#define BX_TICK_USECONDS 1


bx_timer_t bx_timer[BX_MAX_TIMERS];
unsigned   bx_num_timers = 0;
unsigned   bx_min_timer_index = 0;
Bit32u     bx_num_ticks = 1;



  static inline void
new_min_timer(void)
{
  /* convenience function to determine the timer with the lowest
   * remaining time left; the next to go off
   */
  unsigned i;
  Bit32u   min_time;
  Boolean  found;

  min_time = 0xffffffff;
  found = 0;
  for (i=0; i<bx_num_timers; i++) {
    if (bx_timer[i].active && (found==0 || bx_timer[i].remaining<min_time)) {
      found = 1;
      bx_min_timer_index = i;
      min_time = bx_timer[i].remaining;
      }
    }

  if (found == 0)
    bx_panic("new_min_timer(): no more active timers\n");
}


  void
bx_timer_handler(void)
{
  Bit32u previous_min;
  Bit32u min_timer;
  unsigned i;


  /* subtract last timer interval from all remaining times */
  previous_min = bx_timer[bx_min_timer_index].remaining;

  for (i=0; i < bx_num_timers; i++) {
    bx_timer[i].triggered = 0;
    if (bx_timer[i].active) {
      bx_timer[i].remaining -= previous_min;
      if (bx_timer[i].remaining == 0) {
        bx_timer[i].triggered = 1;
        /* reset remaining period for triggered timer */
        bx_timer[i].remaining = bx_timer[i].period;

        /* if triggered timer is one-shot, deactive */
        if (bx_timer[i].continuous==0)
          bx_timer[i].active = 0;

        }
      }
    }


  new_min_timer();
  min_timer = bx_timer[bx_min_timer_index].remaining;

  bx_num_ticks = 1 + (min_timer / BX_TICK_USECONDS);

  for (i=0; i < bx_num_timers; i++) {
    /* call requested timer function.  It may request a different
     * timer period or deactivate, all cases handled below
     */
    if (bx_timer[i].triggered && bx_timer[i].funct!=NULL)
      bx_timer[i].funct();
    }
}

#if BX_EXTERNAL_ENVIRONMENT==1
#  include "tick.h"
#endif



  /* BX_REGISTER_TIMER()
   *   funct:      a function to be called after useconds
   *   useconds:   the interval between calls to funct
   *   continuous: 0=1-shot invokation, 1=continuous calls
   *   active:     0=initially inactive, 1=intially active
   */
  int
bx_register_timer( void (*funct)(void), Bit32u useconds, Boolean continuous,
  Boolean active)
{
  if (bx_num_timers >= BX_MAX_TIMERS) {
    bx_panic("bx_register_timer: too many registered timers.");
    }

  bx_num_timers++;
  bx_timer[bx_num_timers - 1].period    = useconds;
  bx_timer[bx_num_timers - 1].remaining = useconds;
  bx_timer[bx_num_timers - 1].active    = active;
  bx_timer[bx_num_timers - 1].funct     = funct;
  bx_timer[bx_num_timers - 1].continuous = continuous;

  /* return timer id */
  return(bx_num_timers - 1);
}



  void
bx_start_timers(void)
{
  if (!bx_num_timers)
    bx_panic("start_timers(): no timers found\n");

  new_min_timer();
  bx_num_ticks = 1 + (bx_timer[bx_min_timer_index].remaining / BX_TICK_USECONDS);

bx_printf("start_timers(): timer %u initially %u ticks\n",
  (unsigned) bx_min_timer_index, (unsigned) bx_num_ticks);
}

  Bit32u
bx_get_timer(int index)
{
  Bit32u u_elapsed, u_remainder, min_u_remainder;

  if (index >= bx_num_timers)
    bx_panic("bx_get_timer(): bad timer index %d\n", index);

  /* 1st get the useconds left for the next timer to go off */
  min_u_remainder = bx_num_ticks * BX_TICK_USECONDS;
  if (min_u_remainder > bx_timer[bx_min_timer_index].remaining)
    min_u_remainder = bx_timer[bx_min_timer_index].remaining;

  /* if requested timer is next to go off */
  if (index == bx_min_timer_index) {
    return(min_u_remainder);
    }

  /* else calculate elapsed time from timer-at-bat, and subtract */
  else {
    u_elapsed = bx_timer[bx_min_timer_index].remaining - min_u_remainder;
    u_remainder = bx_timer[index].remaining - u_elapsed;
    return(u_remainder);
    }
}

  void
bx_activate_timer( int timer_index, Boolean activate, Bit32u useconds )
{
  Bit32u min_timer;
  Bit32u spent_time, current_remaining;
  unsigned i;

  if (timer_index >= bx_num_timers)
    bx_panic("activate_timer(): bad timer index given\n");

  current_remaining = bx_num_ticks * BX_TICK_USECONDS;

  if (current_remaining > bx_timer[bx_min_timer_index].remaining)
    current_remaining = bx_timer[bx_min_timer_index].remaining;

  spent_time = (bx_timer[bx_min_timer_index].remaining - current_remaining);
  for (i=0; i<bx_num_timers; i++) {
    if (bx_timer[i].active)
      bx_timer[i].remaining -= spent_time;
    }

  /* if useconds = 0, use default stored in period field
   * else set new period from useconds */
  if (useconds==0)
    useconds = bx_timer[timer_index].period;
  else
    bx_timer[timer_index].period = useconds;

  /* is timer being deactived? */
  if (activate == 0) {
    bx_timer[timer_index].active = 0;
    /* if currently scheduled timer is requested timer to deactivate */
    if (bx_min_timer_index == timer_index) {
      new_min_timer();
      }
    }
  else { /* activating timer */
    bx_timer[timer_index].active = 1;
    bx_timer[timer_index].remaining = bx_timer[timer_index].period;

    new_min_timer();
    }

  min_timer = bx_timer[bx_min_timer_index].remaining;
  bx_num_ticks = 1 + (min_timer / BX_TICK_USECONDS);
}
