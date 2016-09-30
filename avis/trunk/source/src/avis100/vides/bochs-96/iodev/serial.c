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



#include "iodev.h"


#if BX_EXTERNAL_ENVIRONMENT==0
static void bx_bios_int14_handler(int vector);
#endif



  void
bx_init_serial_hardware(void)
{
#if 0
  bx_iodev_t  io_device;

  /* SERIAL PORT 1 */
  io_device.read_funct   = bx_serial_io_read_handler;
  io_device.write_funct  = bx_serial_io_write_handler;
  io_device.handler_name = "Serial Port 1";
  io_device.start_addr   = 0x03F8;
  io_device.end_addr     = 0x03FF;
  io_device.irq          = 4;
  bx_register_io_handler(io_device);
#endif
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_serial_bios(void)
{
  Bit8u  zero8;
  Bit16u zero16;

  bx_isr_code_iret[2] = 0x0C;
  bx_register_int_vector(0x0C, bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_serial_int_handler
#ifdef WIN32DEBUG
      ,H_SERIALINT
#endif      
      );

  bx_isr_code_iret[2] = 0x14;
  bx_register_int_vector(0x14,
      bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_bios_int14_handler
#ifdef WIN32DEBUG
      ,H_BIOSINT14
#endif      
      );

  zero16 = 0;

  /* serial I/O address, port 1 */
  bx_access_physical(0x400, 2, BX_WRITE, &zero16);

  /* serial I/O address, port 2 */
  bx_access_physical(0x402, 2, BX_WRITE, &zero16);

  /* serial I/O address, port 3 */
  bx_access_physical(0x404, 2, BX_WRITE, &zero16);

  /* serial I/O address, port 4 */
  bx_access_physical(0x406, 2, BX_WRITE, &zero16);

  /* equipment word at 40:10, bits 9..11 hold number of serial ports */
  /* leave 0 for now */

  zero8 = 0;

  /* serial 1, timeout */
  bx_access_physical(0x47C, 1, BX_WRITE, &zero8);

  /* serial 2, timeout */
  bx_access_physical(0x47D, 1, BX_WRITE, &zero8);

  /* serial 3, timeout */
  bx_access_physical(0x47E, 1, BX_WRITE, &zero8);

  /* serial 4, timeout */
  bx_access_physical(0x47F, 1, BX_WRITE, &zero8);
}
#endif


  Bit8u
bx_serial_io_read_handler(Bit32u address)
{
  /* SERIAL PORT 1 */
  return(0);
}

  void
bx_serial_io_write_handler(Bit32u address, Bit8u value)
{
  /* SERIAL PORT 1 */
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_serial_int_handler(int vector)
{
}
#endif




#if BX_EXTERNAL_ENVIRONMENT==0
  static
void bx_bios_int14_handler(int vector)
{
  /* serial communications */

#ifdef BX_DEBUG
  bx_printf("BIOS: *** INT 14h called with AH=%02x, AL=%02x, DX=%04x\n",
    (int) AH, (int) AL, (int) DX);
#endif

  switch (AH) {
    /* ??? */
    case 0x00:
      bx_printf("BIOS: INT 14h, funct 0, port %u called\n",
        (unsigned) DX);
      if (DX > 3) {
#if 0 /* bill */
        bx_panic("BIOS: INT 14h, funct 0 called with bogus port #\n");
#else
        bx_printf("BIOS: INT 14h, funct 0 called with bogus port #\n");
#endif
        }
      AH = 0x80; /* bogus values */
      AL = 0;
      break;

    /* ??? */
    case 0x01:
      bx_printf("BIOS: INT 14h, funct 1, port %u called\n",
        (unsigned) DX);
      bx_printf("      write byte %02x to port %u\n",
        (unsigned) AL, (unsigned) DX);
      AH = 0x00;
      return;
      break;

    default:
#if 0 /* bill */
      bx_panic("BIOS: *** int 14h function AH=%02x not yet supported!\n",
        (int) AH);
#else
      bx_printf("BIOS: *** int 14h function AH=%02x not yet supported!\n",
        (int) AH);
#endif
    }
}
#endif
