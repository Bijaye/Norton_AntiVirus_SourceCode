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



#include <stdio.h>
#ifndef WIN32
   #include <unistd.h>
   #include <sys/time.h>
#else
   #include <time.h>
#endif

#include "iodev.h"

/* ??? should be in bochs.h */
#define BX_NULL_TIMER_HANDLE 10000

#define double(i) ((double) (i))

#define BX_PIT_LATCH_MODE_LSB   10
#define BX_PIT_LATCH_MODE_MSB   11
#define BX_PIT_LATCH_MODE_16BIT 12

#define COUNTS_TO_USEC(counts) \
	( ((counts)==0) ? 54924.57 : 0.838082467 * (double) (counts) )


typedef struct {
  Bit8u      mode;
  Bit8u      latch_mode;
  Bit16u     input_latch_value;
  Boolean    input_latch_toggle;
  Bit16u     output_latch_value;
  Boolean    output_latch_toggle;
  Boolean    output_latch_full;
  Bit16u     counter_max;
  Boolean    bcd_mode;
  Boolean    GATE;
  int        timer_handle;
  } bx_pit_t;

static bx_pit_t timer0, timer1, timer2;

#if BX_EXTERNAL_ENVIRONMENT==0
static Bit8u int08_code[] = {
  0x0F,            /* pseudo-int 08h */
  0x1F,
  0x08,
  0xcd, 0x1c,      /* int 1c */
  0x50,            /* push ax */
  0xB0, 0x20,      /* mov al, #0x20 */
  0xE6, 0x20,      /* out 0x20, al    ;send EOI to PIC */
  0x58,            /* pop ax */
  0xCF             /* IRET */
  };
#endif

static Bit8u timer2_out = 0;
static Bit8u speaker_data_on = 0;

static void write_pit_count_reg( bx_pit_t *timerX, Bit8u value,
                                 unsigned timerid );
static Bit8u read_pit( bx_pit_t *timerX, unsigned timerid );
static void pit_latch( bx_pit_t *timerX, unsigned timerid );
static void pit_set_GATE(unsigned pit_id, unsigned value);
static void start_pit(bx_pit_t *timerX, unsigned timerid);

static Boolean refresh_clock_div2 = 0;



  void
bx_init_pit_hardware(void)
{
  bx_iodev_t  io_device;

  /* 8254 PIT (Programmable Interval Timer) */
  io_device.read_funct   = bx_pit_io_read_handler;
  io_device.write_funct  = bx_pit_io_write_handler;
  io_device.handler_name = "8254 PIT";
  io_device.start_addr   = 0x0040;
  io_device.end_addr     = 0x0043;
  io_device.irq          = 0;
  bx_register_io_handler(io_device);

  io_device.start_addr   = 0x0061;
  io_device.end_addr     = 0x0061;
  bx_register_io_handler(io_device);


  timer0.mode        = 3;  /* periodic rate generator */
  timer0.latch_mode  = BX_PIT_LATCH_MODE_16BIT;
  timer0.input_latch_value = 0;
  timer0.input_latch_toggle = 0;
  timer0.output_latch_value = 0;
  timer0.output_latch_toggle = 0;
  timer0.output_latch_full = 0;
  timer0.counter_max = 0;  /* 0xFFFF + 1 : (1193200 / 65535 = 18.2Hz) */
  timer0.bcd_mode    = 0;  /* binary counting mode */
  timer0.GATE        = 1;  /* GATE tied to + logic */
  timer0.timer_handle = bx_register_timer( bx_pit_timer_handler,
    (unsigned) COUNTS_TO_USEC(timer0.counter_max), 1, 1);


  timer1.mode        = 3;  /* periodic rate generator */
  timer1.latch_mode  = BX_PIT_LATCH_MODE_16BIT;
  timer1.input_latch_value = 0;
  timer1.input_latch_toggle = 0;
  timer1.output_latch_value = 0;
  timer1.output_latch_toggle = 0;
  timer1.output_latch_full = 0;
  timer1.counter_max = 0;  /* 0xFFFF + 1 : (1193200 / 65535 = 18.2Hz) */
  timer1.bcd_mode    = 0;  /* binary counting mode */
  timer1.GATE        = 1;  /* GATE tied to + logic */
  timer1.timer_handle = BX_NULL_TIMER_HANDLE;

  timer2.mode        = 3;  /* periodic rate generator */
  timer2.latch_mode  = BX_PIT_LATCH_MODE_16BIT;
  timer2.input_latch_value = 0;
  timer2.input_latch_toggle = 0;
  timer2.output_latch_value = 0;
  timer2.output_latch_toggle = 0;
  timer2.output_latch_full = 0;
  timer2.counter_max = 0;  /* 0xFFFF + 1 : (1193200 / 65535 = 18.2Hz) */
  timer2.bcd_mode    = 0;  /* binary counting mode */
  timer2.GATE        = 0;  /* timer2 gate controlled by port 61h bit 0 */
  timer2.timer_handle = BX_NULL_TIMER_HANDLE;
  /* Register timer2 as continuous, but NON-active.  Also, the interval,
   * for this call doesn't matter, since it'll be reprogrammed later via port IO.
   */
  timer2.timer_handle = bx_register_timer( NULL,
    (unsigned) COUNTS_TO_USEC(timer0.counter_max), 1, 0);
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_pit_bios(void)
{
  bx_register_int_vector(0x08, int08_code, sizeof(int08_code),
      bx_pit_int_handler
#ifdef WIN32DEBUG
      ,H_PITINT
#endif      
      );
}
#endif


  Bit32u
bx_pit_io_read_handler(Bit32u address, unsigned io_len)
{
  if (bx_dbg.pit)
    bx_printf("pit: io read from port %04x\n", (unsigned) address);

  switch (address) {
    case 0x40: /* timer 0 - system ticks */
      return( read_pit(&timer0, 0) );
      break;

    case 0x42: /* timer 2 read */
      return( read_pit(&timer2, 2) );
      break;

    case 0x61:
#if 0
      return( (speaker_data << 1) | gate2_timer
        | ((!bx_get_kbd_enable()) << 7)
        );
#endif
      /* AT, port 61h */
      if (timer2.GATE)
        timer2_out = !timer2_out; /* ??? just toggle on/off for now */
      refresh_clock_div2 = !refresh_clock_div2;
      return( (timer2_out<<5) |
              (refresh_clock_div2<<4) |
              (speaker_data_on<<1) |
              (timer2.GATE) );
      break;

    default:
      bx_panic("pit: unsupported io read from port %04x\n", address);
    }
  return(0); /* keep compiler happy */
}

  void
bx_pit_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  Bit8u  command, mode, bcd_mode;

  if (bx_dbg.pit)
    bx_printf("pit: write to port %04x = %02x\n",
      (unsigned) address, (unsigned) value);

  switch (address) {
    case 0x40: /* timer 0: write count register */
      write_pit_count_reg( &timer0, value, 0 );
      break;

    case 0x41: /* timer 1: write count register */
      write_pit_count_reg( &timer1, value, 1 );
      break;

    case 0x42: /* timer 2: write count register */
      write_pit_count_reg( &timer2, value, 2 );
      break;

    case 0x43: /* timer 0-2 mode control */
      /* |7 6 5 4|3 2 1|0|
       * |-------|-----|-|
       * |command|mode |bcd/binary|
       */
      command  = value >> 4;
      mode     = (value >> 1) & 0x07;
      bcd_mode = value & 0x01;
#if 0
bx_printf("timer 0-2 mode control: comm:%02x mode:%02x bcd_mode:%u\n",
  (unsigned) command, (unsigned) mode, (unsigned) bcd_mode);
#endif

      if ( (mode > 5) || (command > 0x0e) )
        bx_panic("pit: outp(43h)=%02xh out of range\n", (unsigned) value);
      if (bcd_mode)
        bx_panic("pit: outp(43h)=%02xh: bcd mode unhandled\n",
          (unsigned) bcd_mode);

      switch (command) {
        case 0x0: /* timer 0: counter latch */
          pit_latch( &timer0, 0 );
          break;

        case 0x1: /* timer 0: LSB mode */
        case 0x2: /* timer 0: MSB mode */
          bx_panic("pit: outp(43h): command %02xh unhandled\n",
            (unsigned) command);
          break;
        case 0x3: /* timer 0: 16-bit mode */
          timer0.mode = mode;
          timer0.latch_mode   = BX_PIT_LATCH_MODE_16BIT;
          timer0.input_latch_value = 0;
          timer0.input_latch_toggle = 0;
          timer0.bcd_mode    = bcd_mode;
          if ( (mode!=3 && mode!=2) || bcd_mode!=0 )
            bx_panic("pit: outp(43h): comm 3, mode %02x, bcd %02x unhandled\n",
              (unsigned) mode, bcd_mode);
          break;
        case 0x4: /* timer 1: counter latch */
        case 0x5: /* timer 1: LSB mode */
        case 0x6: /* timer 1: MSB mode */
          bx_panic("pit: outp(43h): command %02xh unhandled\n",
            (unsigned) command);
          break;
        case 0x7: /* timer 1: 16-bit mode */
          timer1.mode = mode;
          timer1.latch_mode   = BX_PIT_LATCH_MODE_16BIT;
          timer1.input_latch_value = 0;
          timer1.input_latch_toggle = 0;
          timer1.bcd_mode    = bcd_mode;
          if ( mode!=2 || bcd_mode!=0 )
            bx_panic("pit: outp(43h): comm 7, mode %02x, bcd %02x unhandled\n",
              (unsigned) mode, bcd_mode);
          break;
        case 0x8: /* timer 2: counter latch */
        case 0x9: /* timer 2: LSB mode */
        case 0xa: /* timer 2: MSB mode */
          bx_panic("pit: outp(43h): command %02xh unhandled\n",
            (unsigned) command);
          break;
        case 0xb: /* timer 2: 16-bit mode */
          timer2.mode = mode;
          timer2.latch_mode   = BX_PIT_LATCH_MODE_16BIT;
          timer2.input_latch_value = 0;
          timer2.input_latch_toggle = 0;
          timer2.bcd_mode    = bcd_mode;
          if ( mode!=3 || bcd_mode!=0 )
            bx_panic("pit: outp(43h): comm Bh, mode %02x, bcd %02x unhandled\n",
              (unsigned) mode, bcd_mode);
          break;
        case 0xd: /* general counter latch */
        case 0xe: /* latch status of timers */
          bx_panic("pit: outp(43h): command %02xh unhandled\n",
            (unsigned) command);
          break;
        default: /* 0xc & 0xf */
          bx_panic("pit: outp(43h) command %1xh unhandled\n",
            (unsigned) command);
          break;
        }
      break;

    case 0x61:
      speaker_data_on = (value >> 1) & 0x01;
/*??? only on AT+ */
      pit_set_GATE(2, value & 0x01);
#if BX_CPU < 2
      /* ??? XT: */
      bx_kbd_port61h_write(value);
#endif
      break;

    default:
      bx_panic("pit: unsupported io write to port %04x = %02x\n",
        (unsigned) address, (unsigned) value);
    }
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_pit_int_handler(int vector)
{
  Bit32u ticks;

  bx_access_physical(0x046c, 4, BX_READ, &ticks);
  ticks++;
  bx_access_physical(0x046c, 4, BX_WRITE, &ticks);
  bx_outp(0x20, 0x20, 1); /* send End Of Interrupt Command */
}
#endif


  void
bx_pit_timer_handler(void)
{
  bx_trigger_irq(0);
}


  static void
write_pit_count_reg( bx_pit_t *timerX, Bit8u value, unsigned timerid )
{
  Boolean xfer_complete;

  switch ( timerX->latch_mode ) {
    case BX_PIT_LATCH_MODE_16BIT: /* write1=LSB, write2=MSB */
      if (timerX->input_latch_toggle==0) {
        timerX->input_latch_value = value;
        timerX->input_latch_toggle = 1;
        xfer_complete = 0;
        if (bx_dbg.pit)
          bx_printf("pit: timerX write L = %02x\n", (unsigned) value);
        }
      else {
        timerX->input_latch_value |= (value << 8);
        timerX->input_latch_toggle = 0;
        xfer_complete = 1;
        if (bx_dbg.pit)
          bx_printf("pit: timerX write H = %02x\n", (unsigned) value);
        }
      break;

    case BX_PIT_LATCH_MODE_MSB: /* write1=MSB, LSB=0 */
      timerX->input_latch_value = (value << 8);
      xfer_complete = 1;
      if (bx_dbg.pit)
        bx_printf("pit: timerX write H = %02x\n", (unsigned) value);
      break;

    case BX_PIT_LATCH_MODE_LSB: /* write1=LSB, MSB=0 */
      timerX->input_latch_value = value;
      xfer_complete = 1;
      if (bx_dbg.pit)
        bx_printf("pit: timerX write L = %02x\n", (unsigned) value);
      break;

    default:
      bx_panic("write_pit_count_reg: latch_mode unknown\n");
      xfer_complete = 0;
    }

  if (xfer_complete) {
    timerX->counter_max = timerX->input_latch_value;
    if (timerX->GATE)
      start_pit(timerX, timerid);
    }
}


  static Bit8u
read_pit( bx_pit_t *timerX, unsigned timerid )
{
  Bit32u u_left, u_max, counter_max;
  Bit16u counter_value;
  Bit8u  retval;

  if (timerid !=0) {
    }

  if (timerX->output_latch_full) { /* latched read */
    counter_value = timerX->output_latch_value;
    }
  else { /* direct unlatched read */
    if (timerX->timer_handle == BX_NULL_TIMER_HANDLE)
      bx_panic("pit: read_pit(): NULL timer handle %u\n", timerid);
    u_left = bx_get_timer(timerX->timer_handle);
    u_max = (unsigned) COUNTS_TO_USEC(timerX->counter_max);
    if (u_left > u_max) {
      u_left = u_max;
      }

    if (timerX->counter_max == 0)
      counter_max = 0xffff;
    else
      counter_max = timerX->counter_max;

    counter_value = (double(u_left) / double(u_max)) * double(counter_max);
    }

  switch (timerX->latch_mode) {
    case BX_PIT_LATCH_MODE_LSB:
      retval = (Bit8u) counter_value;
      timerX->output_latch_full = 0;
      break;
    case BX_PIT_LATCH_MODE_MSB:
      retval = counter_value >> 8;
      timerX->output_latch_full = 0;
      break;
    case BX_PIT_LATCH_MODE_16BIT:
      if (timerX->output_latch_toggle==0) { /* LSB 1st */
        retval = (Bit8u) counter_value;
        }
      else { /* MSB 2nd */
        retval = counter_value >> 8;
        }
      timerX->output_latch_toggle = !timerX->output_latch_toggle;
      if (timerX->output_latch_toggle == 0)
        timerX->output_latch_full = 0;
      break;
    default:
#if 0 /* riad what the hell is this ? */
      bx_panic("pit: io read from port 40h: unknown latch mode\n");
#else
      bx_printf("pit: io read from port 40h: unknown latch mode\n");
#endif
      retval = 0; /* keep compiler happy */
    }
  return( retval );
}


  static void
pit_latch( bx_pit_t *timerX, unsigned timerid )
{
  Bit32u u_left, u_max, counter_max;

  /* subsequent counter latch commands are ignored until value read out */
  if (timerX->output_latch_full) {
    bx_panic("pit: pit(%u) latch: output latch full, ignoring\n",
              timerid);
    return;
    }

  if (timerX->timer_handle == BX_NULL_TIMER_HANDLE) {
    bx_panic("pit: pit(%u) latch: NULL timer handle\n", timerid);
    }

  u_left = bx_get_timer(timerX->timer_handle);
  u_max = (unsigned) COUNTS_TO_USEC(timerX->counter_max);
  if (u_left > u_max) {
    u_left = u_max;
    if (bx_dbg.pit)
      bx_printf("pit: counter latch timer 0: u_left > u_max\n");
    }

  if (timerX->counter_max == 0)
    counter_max = 0xffff;
  else
    counter_max = timerX->counter_max;
  if (bx_dbg.pit) {
    bx_printf("pit: u_left = %lu\n", u_left);
    bx_printf("pit: u_max = %lu\n", u_max);
    bx_printf("pit: counter_max = %lu\n", counter_max);
    }
  timerX->output_latch_value = 
    (double(u_left) / double(u_max)) * double(counter_max);
  if (bx_dbg.pit)
    bx_printf("pit: latch_value = %lu\n", timerX->output_latch_value);
  timerX->output_latch_toggle = 0;
  timerX->output_latch_full   = 1;
}

  static void
pit_set_GATE(unsigned pit_id, unsigned value)
{
  if (pit_id != 2)
    bx_panic("pit_set_GATE: pit_id != 2\n");

  value = (value > 0);

  /* if no transition of GATE input line, then nothing to do */
  if (value == timer2.GATE)
    return;

  if (value) { /* PIT2: GATE transition from 0 to 1 */
    /* pulse of GATE2 from 0 to 1 resets counter */
    timer2.GATE  = 1;
    start_pit(&timer2, 2);
    }
  else {       /* PIT2: GATE transition from 1 to 0, deactivate */
    timer2.GATE  = 0;
    bx_activate_timer(timer2.timer_handle, 0, 0);
    }
}


  static void
start_pit(bx_pit_t *timerX, unsigned timerid)
{
  double period_hz, period_usec;

  if (timerX->counter_max == 0x0000) {
    period_hz   = 1193200.0 / 65536.0; /* 18.2 Hz */
    }
  else {
    period_hz = double(1193200.0) / double(timerX->counter_max);
    }
  period_usec = COUNTS_TO_USEC(timerX->counter_max);
  bx_printf("timer%u period set to %lf hz, %lf usec\n",
            timerid, period_hz, period_usec);
  if (timerid==0 && period_hz>100.5)
    bx_panic("pit0: start_pit: period > 100Hz!");
  if (timerid==2 && period_hz>1080.0)
    bx_panic("pit2: start_pit: period > 1080Hz!");

  switch (timerX->mode) {
    case 0: /* single timeout */
      bx_panic("start_pit: mode %u unhandled\n",
               (unsigned) timerX->mode);
      break;
    case 1: /* retriggerable one-shot */
      bx_panic("start_pit: mode %u unhandled\n",
               (unsigned) timerX->mode);
      break;
    case 2: /* rate generator */
      if (timerid == 1)
        bx_printf("start_pit: mode 2: rate generator\n");
      if (timerid==0 || timerid==2) {
        /* change timer interval */
        bx_activate_timer(timerX->timer_handle, 1, (unsigned) period_usec);
        }
      break;
    case 3: /* square wave mode */
      bx_printf("start_pit: mode 3: square wave\n",
               (unsigned) timerX->mode);
      
      if (timerid==0 || timerid==2) {
        /* change timer interval */
        bx_activate_timer(timerX->timer_handle, 1, (unsigned) period_usec);
        }
      break;
    case 4: /* software triggered strobe */
      bx_panic("start_pit: mode %u unhandled\n",
               (unsigned) timerX->mode);
      break;
    case 5: /* hardware retriggerable strobe */
      bx_panic("start_pit: mode %u unhandled\n",
               (unsigned) timerX->mode);
      break;
    default:
      bx_panic("start_pit: timer%u has bad mode\n",
               (unsigned) timerX->mode);
    }
}
