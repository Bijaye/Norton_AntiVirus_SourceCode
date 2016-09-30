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

int mouse_timer_index;


static Bit8u int0Dh_code[] = {
  0x0F, 0x1F, 0x0D,         /* pseudo-int 0Dh */
  0x50,                     /* push ax */
  0xB0, 0x65,               /* mov al, #0x65 */
  0xE6, 0x20,               /* out 0x20, al    ;send EOI5 to PIC */
  0x58,                     /* pop ax */
  0xcf                      /* iret */
  };


static struct {
  Boolean enabled;
  } mouse;


  Bit8u
bx_mouse_io_read_handler(Bit32u address)
{
  switch (address) {
    case 0x23d: /* signature port */
      bx_printf("mouse_io_read: 23d: returning signature byte a5\n");
      return(0xa5);
      break;

    case 0x23e:
      return(0);
      bx_printf("mouse io read: 23e: returning 0 ??? \n");
      break;

    default:
      bx_panic("mouse io read handler unsupported (%04x)\n",
        (unsigned) address);
      return(0);
    }
}

  void
bx_mouse_io_write_handler(Bit32u address, Bit8u value)
{
  switch (address) {
    case 0x23d: /* signature port */
      if (value == 0xa5) {
        bx_printf("mouse_io_write: signature port receives signature byte 0xa5\n");
        }
      else {
        bx_panic("mouse_io_write: 23d: value = %02x\n", (unsigned) value);
        }
      break;

    case 0x23e: /* control/interrupt port */
      if (value == 0x00) { /* enable */
        mouse.enabled = 0;
        bx_printf("mouse_io_write: interrupt enabled\n");
        bx_trigger_irq(5);
        }
      else if (value == 0x10) { /* disable */
        mouse.enabled = 1;
        bx_printf("mouse_io_write: interrupt disabled\n");
        }
      else {
        bx_panic("mouse_io_write: 23e: value = %02x\n", (unsigned) value);
        }
      break;

    case 0x23f:
      if (value == 0x91) {
        bx_printf("mouse_io_write: config port receives config byte 0x91\n");
        }
      else {
        bx_panic("mouse_io_write: 23f: value = %02x\n", (unsigned) value);
        }
      break;
    default:
      bx_panic("mouse io write handler unsupported (%04x<--%02x)\n",
        (unsigned) address, (unsigned) value);
    }
}

  /* INT 74h BIOS mouse handler */
  void
bx_mouse_int_handler(int vector)
{
  bx_printf("mouse int handler unsupported\n");
}

  void
bx_init_mouse(void)
{
  bx_iodev_t  io_device;
  Bit16u equip_word;

#if 0
  bx_access_physical(0x410, 2, BX_READ, &equip_word);
  equip_word |= 0x04; /* pointing device installed */
  bx_access_physical(0x410, 2, BX_WRITE, &equip_word);
#endif

/*
  mouse_timer_index = bx_register_timer( bx_mouse_timer_handler, 750000);
*/


  /* Logitech mouse */
  io_device.read_funct   = bx_mouse_io_read_handler;
  io_device.write_funct  = bx_mouse_io_write_handler;
  io_device.handler_name = "Logitech style mouse";
  io_device.start_addr   = 0x023c;
  io_device.end_addr     = 0x023f;
  io_device.irq          = 5;
  bx_register_io_handler(io_device);


  bx_register_int_vector(0x0D, int0Dh_code, sizeof(int0Dh_code),
      bx_mouse_int_handler);

  mouse.enabled = 0;
}


  /* int 15h function C2h: mouse bios interface */

  void
bx_mouse_bios(void)
{
  bx_printf("MOUSE: *** int 15h function AX=%04x, BX=%04x not yet supported!\n",
    (unsigned) AX, (unsigned) BX);
  bx_set_CF(1);
  AH = 0x86;
}



  void
bx_mouse_timer_handler(void)
{
}
