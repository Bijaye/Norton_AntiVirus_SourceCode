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


  void
bx_init_parallel_hardware(void)
{
#if 0
  bx_iodev_t  io_device;
  /* PARALLEL PORT 1 */
  io_device.read_funct   = bx_parallel_io_read_handler;
  io_device.write_funct  = bx_parallel_io_write_handler;
  io_device.handler_name = "Parallel Port 1";
  io_device.start_addr   = 0x03BC;
  io_device.end_addr     = 0x03BE;
  io_device.irq          = 7;
  bx_register_io_handler(io_device);
#endif
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_parallel_bios(void)
{
  Bit16u zero16;
  Bit8u  zero8;


  bx_isr_code_iret[2] = 0x0F;
  bx_register_int_vector(0x0F, bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_parallel_int_handler
#ifdef WIN32DEBUG
      ,H_PARALLELINT
#endif      
      );

  zero16 = 0;

  /* Parallel I/O address, port 1 */
  bx_access_physical(0x408, 2, BX_WRITE, &zero16);

  /* Parallel I/O address, port 2 */
  bx_access_physical(0x40A, 2, BX_WRITE, &zero16);

  /* Parallel I/O address, port 3 */
  bx_access_physical(0x40C, 2, BX_WRITE, &zero16);

  zero8 = 0;

  /* parallel printer 1, timeout */
  bx_access_physical(0x478, 1, BX_WRITE, &zero8);

  /* parallel printer 2, timeout */
  bx_access_physical(0x479, 1, BX_WRITE, &zero8);

  /* parallel printer 3, timeout */
  bx_access_physical(0x47A, 1, BX_WRITE, &zero8);

  /* parallel printer 4, timeout */
  bx_access_physical(0x47B, 1, BX_WRITE, &zero8);
}
#endif


  Bit32u
bx_parallel_io_read_handler(Bit32u address, unsigned io_len)
{
  /* PARALLEL PORT 1 */
  return(0);
}

  void
bx_parallel_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  /* PARALLEL PORT 1 */
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_parallel_int_handler(int vector)
{
}
#endif
