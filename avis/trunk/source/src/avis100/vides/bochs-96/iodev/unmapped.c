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

static Bit16u test_port16=0;
static Bit32u test_port32=0;


static Bit8u port8e = 0x00;





  Bit32u
bx_unmapped_io_read_handler(Bit32u address, unsigned io_len)
{
/* ??? put in for external environment testing */
if (address == 0xfffc)
  return(test_port32);
if (address == 0xfffe)
  return(test_port16);

  /* This function gets called for access to any IO ports which
     are not mapped to any device handler.  Reads return 0 */

if (address >= 0x02e0 && address <= 0x02ef) return(0);
  switch (address) {
   case 0x80:
      bx_panic("unmapped: io read from port 80h\n");
      break;
    case 0x8e:
      return(port8e);
      break;
    case 0x03df:
      /* bx_printf("unsupported IO read from port %04x (CGA)\n", address);*/
      break;
    case 0x023a:
    case 0x02f8: /* UART */
    case 0x02f9: /* UART */
    case 0x02fb: /* UART */
    case 0x02fc: /* UART */
    case 0x02fd: /* UART */
    case 0x02ea:
    case 0x02eb:
    case 0x03e8:
    case 0x03e9:
    case 0x03ea:
    case 0x03eb:
    case 0x03ec:
    case 0x03ed:
    case 0x03f8: /* UART */
    case 0x03f9: /* UART */
    case 0x03fb: /* UART */
    case 0x03fc: /* UART */
    case 0x03fd: /* UART */
    case 0x17c6:
      /* bx_printf("unsupported IO read from port %04x\n", address); */
      break;
    default:
      bx_printf("unsupported IO read from port %04x\n", address);
    }
  return(0);
}

  void
bx_unmapped_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
/* ??? put in for external environment testing */
if (address == 0xfffc)
  test_port32 = value;
if (address == 0xfffe)
  test_port16 = value;


/* ??? */
if (address==0xee || address==0xed) return;

  /* This function gets called for access to any IO ports which
     are not mapped to any device handler.  Writes to an unmapped
     IO port are ignored. */

if (address >= 0x02e0 && address <= 0x02ef) return;
  switch (address) {
    case 0x80: /* diagnostic test port to display progress of POST */
      /*bx_printf("Diagnostic port 80h: write = %02xh\n", (unsigned) value);*/
      return;
      break;

    case 0x8e: /* ??? */
      port8e = value;
      return;

    case 0x2f2:
    case 0x2f3:
    case 0x2f4:
    case 0x2f5:
    case 0x2f6:
    case 0x2f7:
    case 0x3e8:
    case 0x3e9:
    case 0x3eb:
    case 0x3ec:
    case 0x3ed:
      bx_printf("unsupported IO write to port %04x of %02x\n",
        address, value);
      break;
    default:
      bx_printf("unsupported IO write to port %04x of %02x\n",
        address, value);
    }
}
