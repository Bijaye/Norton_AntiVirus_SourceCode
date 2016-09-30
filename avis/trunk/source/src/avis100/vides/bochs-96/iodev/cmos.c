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



#include <time.h>
#include "iodev.h"

#if 1 /* bill */
#define time(a) mytime(a)
#endif

static Bit8u  cmos_mem_address = 0;


#if BX_EXTERNAL_ENVIRONMENT==0
static void bx_bios_int1a_handler(int vector);
static void set_ticks(void);
#endif

static Bit8u cmos_seconds_alarm = 0xff;
static Bit8u cmos_minutes_alarm = 0xff;
static Bit8u cmos_hours_alarm = 0xff;

static Bit8u cmos_shutdown_status = 0x00; /* proceed with normal POST */
static Bit8u diagnostic_status = 0x00;


  /* only 10h..3Fh used */
Bit8u bx_cmos_reg[BX_NUM_CMOS_REGS];




  void
bx_init_cmos_hardware(void)
{
  bx_iodev_t  io_device;
  unsigned i;

  /* CMOS RAM & RTC */
  io_device.read_funct   = bx_cmos_ram_io_read_handler;
  io_device.write_funct   = bx_cmos_ram_io_write_handler;
  io_device.handler_name = "CMOS RAM";
  io_device.start_addr   = 0x0070;
  io_device.end_addr     = 0x0071;
  io_device.irq          = BX_NO_IRQ;
  bx_register_io_handler(io_device);

  for (i=0; i<BX_NUM_CMOS_REGS; i++) {
    bx_cmos_reg[i] = 0;
    }
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_cmos_bios(void)
{
  time_t time_seconds;
  struct tm *time_calendar;
  Bit32u days_since_1980;

  bx_isr_code_cf[2] = 0x1a;
  bx_register_int_vector(0x1a, bx_isr_code_cf, sizeof(bx_isr_code_cf),
      bx_bios_int1a_handler
#ifdef WIN32DEBUG
      ,H_BIOSINT1A
#endif
      );

  bx_isr_code_iret[2] = 0x70;
  bx_register_int_vector(0x70, bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_cmos_rtc_int_handler
#ifdef WIN32DEBUG
      ,H_CMOSRTC
#endif
      );

  time_seconds = time(NULL);
  time_calendar = localtime(&time_seconds);
  days_since_1980 = (time_calendar->tm_year - 80) * 365 +
    time_calendar->tm_yday;
  bx_access_physical(0x4ce, 4, BX_WRITE, &days_since_1980);

  set_ticks();
}
#endif



  Bit32u
bx_cmos_ram_io_read_handler(Bit32u address, unsigned io_len)
{
  time_t timep;
  struct tm *time_calendar;
  Bit8u val_bcd;

  if (bx_dbg.cmos)
    bx_printf("CMOS read of CMOS register 0x%x\n",
      (unsigned) cmos_mem_address);


  /* CMOS RAM */
  switch (address) {
    case 0x0071:
      switch (cmos_mem_address) {
        case 0x01: return(cmos_seconds_alarm);
        case 0x03: return(cmos_minutes_alarm);

        case 0x00: /* seconds in BCD format */
          timep = time(NULL);
          time_calendar = localtime(&timep);
          val_bcd = 
            ((time_calendar->tm_sec  / 10) << 4) |
            (time_calendar->tm_sec % 10);
          return(val_bcd);
          break;
        case 0x02: /* minutes in BCD format */
          timep = time(NULL);
          time_calendar = localtime(&timep);
          val_bcd = 
            ((time_calendar->tm_min  / 10) << 4) |
            (time_calendar->tm_min % 10);
          return(val_bcd);
          break;
        case 0x04: /* hours in BCD */
          timep = time(NULL);
          time_calendar = localtime(&timep);
          val_bcd = 
            ((time_calendar->tm_hour  / 10) << 4) |
            (time_calendar->tm_hour % 10);
          return(val_bcd);
          break;

        case 0x05: return(cmos_hours_alarm);
        case 0x06: /* day of the week ???*/
          return(5);
        case 0x07: /* day of the month in BCD ???*/
#if 0 /* bill */
          return(0x25);
#else
          timep = time(NULL);
          time_calendar = localtime(&timep);
          val_bcd =
            ((time_calendar->tm_mday  / 10) << 4) |
            (time_calendar->tm_mday % 10);
          return(val_bcd);
          break;
#endif
        case 0x08: /* month in BCD ???*/
#if 0 /* bill */
          return(0x04);
#else
          timep = time(NULL);
          time_calendar = localtime(&timep);
          val_bcd =
            ((time_calendar->tm_mon  / 10) << 4) |
            (time_calendar->tm_mon % 10);
          return(val_bcd);
          break;     
#endif
        case 0x09: /* year in BCD ???*/
#if 0 /* bill */
          return(0x96);
#else
          timep = time(NULL);
          time_calendar = localtime(&timep);
          val_bcd =
            ((time_calendar->tm_year  / 10) << 4) |
            (time_calendar->tm_year % 10);
          return(val_bcd);
          break; 
#endif
        case 0x0a: return(0x26);
        case 0x0b: return(0x02);
        case 0x0d: /* bit7: RTC power stable since last read */
          return(0x80);
        case 0x0e: /* diagnostic status, during POST */
          return(diagnostic_status); /* no errors */
        case 0x0f: return(cmos_shutdown_status);

        default:
          if (bx_dbg.cmos) {
            bx_printf("cmos: read cmos[%02xh] = %02x\n",
                      (unsigned) cmos_mem_address,
                      (unsigned) bx_cmos_reg[cmos_mem_address]);
            }
          if (cmos_mem_address>=0x10 && cmos_mem_address<BX_NUM_CMOS_REGS) {
            return(bx_cmos_reg[cmos_mem_address]);
            }
          bx_panic("unsupported cmos io read, register(%xh)!\n",
            (unsigned) cmos_mem_address);
        }
      break;
    default:
      bx_panic("unsupported cmos read, address=%0x%x!\n",
        (unsigned) address);
      break;
    }
  return(0);
}

  void
bx_cmos_ram_io_write_handler(Bit32u address, Bit32u value, unsigned len)
{
  if (bx_dbg.cmos)
    bx_printf("CMOS write to address: 0x%x = 0x%x\n",
      (unsigned) address, (unsigned) value);


  /* CMOS RAM */
  switch (address) {
    case 0x0070:
      cmos_mem_address = value & 0x3F;
      break;
    case 0x0071:
      switch (cmos_mem_address) {
#if 1 /* bill */
        case 0x00:
        case 0x02:
        case 0x04:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
          break;
#endif
        case 0x01: /* seconds alarm */
          cmos_seconds_alarm = value;
bx_printf("setting cmos seconds alarm to %x\n", (unsigned) value);
          break;
        case 0x03: /* minutes alarm */
          cmos_minutes_alarm = value;
bx_printf("setting cmos minutes alarm to %x\n", (unsigned) value);
          break;
        case 0x05: /* hours alarm */
          cmos_hours_alarm = value;
bx_printf("setting cmos hours alarm to %x\n", (unsigned) value);
          break;
        case 0x0a: /* status register A */
          if (value != 0x26)
            bx_panic("unsupported cmos io write, case 0x0a!\n");
          break;

        case 0x0b: /* status register B */
          if (value != 0x02)
            bx_panic("unsupported cmos io write, case 0x0b!\n");
          break;

        case 0x0e: /* diagnostic status */
          diagnostic_status = value;
          bx_printf("CMOS: write register 0Eh: %02x\n", (unsigned) value);
          break;

	case 0x0f: /* shutdown status */
          switch (value) {
            case 0: /* proceed with normal POST (soft reset) */
              if (bx_dbg.reset)
                bx_printf("CMOS: Reg 0F set to 0: shutdown action = normal POST\n");
              cmos_shutdown_status = value;
              break;
            case 0x02: /* shutdown after memory test */
              if (bx_dbg.reset)
                bx_printf("CMOS: Reg 0Fh: request to change shutdown action"
                             " to shutdown after memory test\n");
              cmos_shutdown_status = value;
              break;
            case 4: /* jump to disk bootstrap routine */
              bx_printf("CMOS: Reg 0Fh: request to change shutdown action "
                             "to jump to disk bootstrap routine.\n");
              cmos_shutdown_status = value;
              break;
            case 9: /* return to BIOS extended memory block move
                       (interrupt 15h, func 87h was in progress) */
              if (bx_dbg.reset)
                bx_printf("CMOS: Reg 0Fh: request to change shutdown action "
                             "to return to BIOS extended memory block move.\n");
              cmos_shutdown_status = value;
              break;
            case 0x0a: /* jump to DWORD pointer at 40:67 */
              if (bx_dbg.reset)
                bx_printf("CMOS: Reg 0Fh: request to change shutdown action"
                             " to jump to DWORD at 40:67\n");
              cmos_shutdown_status = value;
              break;
            default:
              bx_panic("unsupported cmos io write to reg F, case %x!\n",
                (unsigned) value);
              break;
            }
          break;

        default:
          if (cmos_mem_address>=0x10 && cmos_mem_address<BX_NUM_CMOS_REGS) {
            bx_printf("CMOS: write reg %02xh: value = %02xh\n",
              (unsigned) cmos_mem_address, (unsigned) value);
            bx_cmos_reg[cmos_mem_address] = value;
            break;
            }
          bx_printf("unsupported cmos io write, register(%xh)=%02x!\n",
            (unsigned) cmos_mem_address, (unsigned) value);
        }
      break;
    }
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_cmos_rtc_int_handler(int vector)
{
  bx_panic("cmos_rtc_int_handler(): not implemented yet\n");
}
#endif


#if BX_EXTERNAL_ENVIRONMENT==0
  static void
bx_bios_int1a_handler(int vector)
{
  time_t time_seconds;
  struct tm *time_calendar;
  unsigned century, year, month, day;
  Bit16u dx, cx;
  Bit8u  al;

  if (bx_dbg.cmos) {
    bx_printf("CMOS: *** INT 1ah called with AH=%02x\n", (int) AH);
    }

  /* time of day functions */


  switch (AH) {
    case 0x00: /* get current clock count */
      /* set_ticks(); */
      bx_access_physical(0x046c, 2, BX_READ, &dx);
      bx_access_physical(0x046e, 2, BX_READ, &cx);
      bx_access_physical(0x0470, 1, BX_READ, &al);
      CX = cx;
      DX = dx;
      AL = al;
      bx_set_CF(0); /* ??? */
#if 0
bx_printf("clock tick: DX=%04x CX=%04x AL=%02x\n",
  (unsigned) dx, (unsigned) cx, (unsigned) al);
#endif
      break;

    case 0x01: /* set current clock count */
#if 0 /* bill */
      bx_panic("unsupported int1ah function (01h)\n");
#else
      cx = CX;
      dx = DX;
      bx_access_physical(0x046c, 2, BX_WRITE, &dx);
      bx_access_physical(0x046e, 2, BX_WRITE, &cx);
#endif
      break;

    case 0x02: /* read CMOS time */
      time_seconds = time(NULL);
      time_calendar = localtime(&time_seconds);

      bx_set_CF(0);
      CH = ((time_calendar->tm_hour / 10) << 4)
          | (time_calendar->tm_hour % 10);
      CL = ((time_calendar->tm_min  / 10) << 4)
          | (time_calendar->tm_min % 10);
      DH = ((time_calendar->tm_sec  / 10) << 4)
          | (time_calendar->tm_sec % 10);
      DL = (time_calendar->tm_isdst) > 0;
      break;

    case 0x03: /* set CMOS time */
      bx_set_CF(0);
      break;

    case 0x04: /* read CMOS date */
      time_seconds = time(NULL);
      time_calendar = localtime(&time_seconds);

      century = 19 + (time_calendar->tm_year / 100);
      year    = time_calendar->tm_year % 100;
      month   = time_calendar->tm_mon + 1;
      day     = time_calendar->tm_mday;

      bx_set_CF(0);
      CH = ((century / 10) << 4) | (century % 10);
      CL = ((year  / 10) << 4) | (year % 10);
      DH = ((month / 10) << 4) | (month % 10);
      DL = ((day   / 10) << 4) | (day % 10);
      break;

    case 0x05: /* set CMOS date */
      bx_set_CF(0);
      break;

    default:
      bx_set_CF(1);
      bx_printf("*** int 1A function %d not yet supported!\n",
        (int) AH);
    }
}

  static void
set_ticks(void)
{
  time_t time_seconds;
  struct tm *time_calendar;
  Bit32u ticks;
  Bit8u  rollover;

  /* return the number of ticks since midnight */

  time_seconds = time(NULL);
  time_calendar = localtime(&time_seconds);

  ticks = (time_calendar->tm_hour * 3600 + time_calendar->tm_min * 60 +
    time_calendar->tm_sec) * 18.2;
  rollover = 0;
  bx_access_physical(0x046c, 4, BX_WRITE, &ticks);
  bx_access_physical(0x0470, 1, BX_WRITE, &rollover);
}
#endif /* BX_EXTERNAL_ENVIRONMENT==0 */

  void
bx_checksum_cmos(void)
{
  unsigned i;
  Bit16u sum;

  sum = 0;
  for (i=0x10; i<=0x2d; i++) {
    sum += bx_cmos_reg[i];
    }
  bx_cmos_reg[0x2e] = (sum >> 8) & 0xff; /* checksum high */
  bx_cmos_reg[0x2f] = (sum & 0xff);      /* checksum low */

bx_printf("cmos: cmos[0x14] = %02x\n", (unsigned) bx_cmos_reg[0x14]);
}
