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

/*#define BX_EBDA (639*1024)*/
#define BX_EBDA 0x9fc00


void bx_mouse_timer_handler(void);
void bx_mouse_int_handler(int vector);

int mouse_timer_index;

#if 0
static struct {
  Bit8u   package_size;
  Bit8u   sample_rate;
  Bit8u   resolution;
  Bit8u   scaling;
  Bit16u  handler_seg;
  Bit16u  handler_offset;
  } mouse;
#endif


#if 0
  /* INT 74h BIOS mouse handler
   *   push four words of mouse information,
   *   enable interrupts
   *   make far call
   *   pop  four words
   *   interrupt return
   */
static Bit8u int74_code[] = {
  0x50,                            /* push ax */
  0xe4, 0x64,                      /* in AL, #64 */
  0xa8, 0x20,                      /* test AL, #20 */

  0x75, 0x02,                      /* jnz MOUSE_DATA */
#if 0
  0x75, 0x09,                      /* jnz MOUSE_DATA */
  0xf4,                            /* halt (for now to see if this gets here) */
  0xB0, 0x20,                      /* mov AL, #20 ; send EOI */
  0xE6, 0xa0,                      /* out #0xa0, AL ; to slave PIC */
  0xE6, 0x20,                      /* out #0x20, AL ; to master PIC */
#endif
  0x58,                            /* pop ax */
  0xcf,                            /* iret */

                                   /* MOUSE_DATA: */
  0xB0, 0xa7,                      /* mov AL, #a7  ;disable aux */
  0xE6, 0x64,                      /* out #0x64, AL */
  0xe4, 0x60,                      /* in AL, #60   ;read mouse data*/




  0x0F, 0x1F, 0x74,                /* PSEUDO-INT 74h */
#if 0
  0x3c, 0x01,                      /* cmp AL, #01 */
  0x74, 0x0c,                      /* je  FULL_PACKET */

  0xB0, 0x20,                      /* mov AL, #20 ; send EOI */
  0xE6, 0xa0,                      /* out #0xa0, AL ; to slave PIC */
  0xE6, 0x20,                      /* out #0x20, AL ; to master PIC */
  0xB0, 0xa8,                      /* mov AL, #a8   ;enable aux */
  0xE6, 0x64,                      /* out #0x64, AL */
  0x58,                            /* pop ax */
  0xcf,                            /* iret */
#endif

                                   /* FULL_PACKET: */
  0x1e,                            /* push ds */

  0xb8, 0xc0, 0x9f,                /* mov  ax, #9fc0 */
  0x8e, 0xd8,                      /* mov  ds, ax */

#if 0
  0xff, 0x36, 0x28, 0x00,          /* push word ds:0028 */
  0xff, 0x36, 0x2a, 0x00,          /* push word ds:002A */
  0xff, 0x36, 0x2c, 0x00,          /* push word ds:002C */
  0xff, 0x36, 0x2e, 0x00,          /* push word ds:002E */
#endif

#if 0
  0xb4, 0x00,                      /* mov AH, #00 */
  0x8a, 0x06, 0x28, 0x00,          /* mov AL, DS:0028 */
  0x50,                            /* push ax */
  0x8a, 0x06, 0x29, 0x00,          /* mov AL, DS:0029 */
  0x50,                            /* push ax */
  0x8a, 0x06, 0x2a, 0x00,          /* mov AL, DS:002a */
  0x50,                            /* push ax */
  0xb0, 0x00,                      /* mov AL, #00 */
  0x50,                            /* push ax */
#endif

#if 0
  0xb8, 0x00, 0x08,                /* mov AX, #0800 */
  0x50,                            /* push ax */
  0xb8, 0x01, 0x01,                /* mov AX, #0101 */
  0x50,                            /* push ax */
  0x44, /* inc sp */
  /* 3 bytes pushed */
#endif

  0xb8, 0x08, 0x00,                /* mov AX, #0008 */
  0x50,                            /* push ax */
  0xb8, 0x00, 0x00,                /* mov AX, #0000 */
  0x50,                            /* push ax */
  0xb8, 0x01, 0x00,                /* mov AX, #0001 */
  0x50,                            /* push ax */
  0xb8, 0x00, 0x00,                /* mov AX, #0000 */
  0x50,                            /* push ax */

#if 0
  0xff, 0x36, 0x28, 0x00,          /* push word ds:0028 */
  0xff, 0x36, 0x2a, 0x00,          /* push word ds:002A */
#endif

#if 0
  0xff, 0x36, 0x2a, 0x00,          /* push word ds:002A */
  0xff, 0x36, 0x28, 0x00,          /* push word ds:0028 */
#endif

#if 0
  0xfb,                            /* STI */
#endif

  0xff, 0x1e, 0x22, 0x00,          /* call_ep ds:0022   */

  0x58, 0x58, 0x58, 0x58,          /* pop 8 bytes from stack */

#if 0
  0x58, 0x58,                      /* pop 4 bytes from stack */
#endif

#if 0
  /* pop 3 bytes from stack */
  0x44, /* inc sp */
  0x44, /* inc sp */
  0x44, /* inc sp */
#endif

  0xfa,                            /* CLI */
  0xB0, 0x20,                      /* mov AL, #20 ; send EOI to slave PIC */
  0xE6, 0xa0,                      /* out #0xa0, AL ; to slave PIC */
  0xE6, 0x20,                      /* out #0x20, AL ; to master PIC */
  0xB0, 0xa8,                      /* mov AL, #a8   ;enable aux */
  0xE6, 0x64,                      /* out #0x64, AL */
  0x1f,                            /* pop ds */
  0x58,                            /* pop ax */
  0xcf                             /* iret */
  };
#endif



#if 0
static Bit8u int74_code[] = {
  0x50,                            /* push ax */
  0xe4, 0x64,                      /* in AL, #64 */
  0xa8, 0x20,                      /* test AL, #20 */

  0x75, 0x02,                      /* jnz MOUSE_DATA */
#if 0
  0x75, 0x09,                      /* jnz MOUSE_DATA */
  0xf4,                            /* halt (for now to see if this gets here) */
  0xB0, 0x20,                      /* mov AL, #20 ; send EOI */
  0xE6, 0xa0,                      /* out #0xa0, AL ; to slave PIC */
  0xE6, 0x20,                      /* out #0x20, AL ; to master PIC */
#endif
  0x58,                            /* pop ax */
  0xcf,                            /* iret */

                                   /* MOUSE_DATA: */
  0x1e,                            /* push ds */
  0xb8, 0xc0, 0x9f,                /* mov  ax, #9fc0 */
  0x8e, 0xd8,                      /* mov  ds, ax */

  0xB0, 0xad,                      /* mov AL, #ad  ;disable keyboard */
  0xE6, 0x64,                      /* out #0x64, AL */
  0xB0, 0xa8,                      /* mov AL, #ad  ;enable aux */
  0xE6, 0x64,                      /* out #0x64, AL */

  0xb4, 0x00,                      /* mov AH, #00 */
  0xe4, 0x60,                      /* in AL, #60   ;read mouse data (header)*/
  0x50,                            /* push ax */
  0xe4, 0x60,                      /* in AL, #60   ;read mouse data (dx) */
  0x50,                            /* push ax */
  0xe4, 0x60,                      /* in AL, #60   ;read mouse data (dy) */
  0x50,                            /* push ax */
  0xb0, 0x00,                      /* mov AL, #00  ;     mouse data (dz=0) */
  0x50,                            /* push ax */

  0xB0, 0xae,                      /* mov AL, #ae   ;enable keyboard */
  0xE6, 0x64,                      /* out #0x64, AL */

#if 0
  0xfb,                            /* STI */
#endif

  0xff, 0x1e, 0x22, 0x00,          /* call_ep ds:0022   */

  0x58, 0x58, 0x58, 0x58,          /* pop 8 bytes from stack */

  0xfa,                            /* CLI */
  0xB0, 0x20,                      /* mov AL, #20 ; send EOI to slave PIC */
  0xE6, 0xa0,                      /* out #0xa0, AL ; to slave PIC */
  0xE6, 0x20,                      /* out #0x20, AL ; to master PIC */
  0x1f,                            /* pop ds */
  0x58,                            /* pop ax */
  0xcf                             /* iret */
  };
#endif


  void
bx_init_mouse_hardware(void)
{
#if 0
  Bit16u equip_word;
  Bit8u mouse_data, index;

  mouse.package_size   = 8;
  mouse.sample_rate    = 100; /* reports per second (default) */
  mouse.resolution     = 100; /* dpi, 4 counts per millimeter (default) */
  mouse.scaling        = 1;   /* 1:1 (default) */
  mouse.handler_seg    = 0x0000; /* uninstalled */
  mouse.handler_offset = 0x0000;

#if 0
  mouse_timer_index = bx_register_timer( bx_mouse_timer_handler, 750000);
#endif

#endif
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_mouse_bios(void)
{
#if 0
  Bit16u equip_word;
  Bit8u mouse_data, index;

  bx_access_physical(0x410, 2, BX_READ, &equip_word);
  equip_word |= 0x04; /* pointing device installed */
  bx_access_physical(0x410, 2, BX_WRITE, &equip_word);

  /* clear data area */
  mouse_data = 0;
  for (index=0; index<8; index++)
    bx_access_physical(BX_EBDA + 0x28 + index, 1, BX_WRITE, &mouse_data);

  bx_register_int_vector(0x74, int74_code, sizeof(int74_code),
      bx_mouse_int_handler);
#endif
}
#endif


  Bit32u
bx_mouse_io_read_handler(Bit32u address, unsigned io_len)
{
  bx_panic("mouse io read handler unsupported\n");
  return(0);
}

  void
bx_mouse_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  bx_panic("mouse io write handler unsupported\n");
}


#if BX_EXTERNAL_ENVIRONMENT==0
  /* INT 74h BIOS mouse handler */
  void
bx_mouse_int_handler(int vector)
{
  Bit8u mouse_flags_1, mouse_flags_2, index, mouse_data;

  mouse_data = AL;
  bx_printf("mouse_int(): AL=%02x\n", (unsigned) mouse_data);

  bx_access_physical(BX_EBDA + 0x26, 1, BX_READ, &mouse_flags_1);
  index = mouse_flags_1 & 0x07;
  if (index==0) {
    AL = 1; /* full packet read */
bx_dbg.debugger = 1;
    bx_access_physical(BX_EBDA + 0x27, 1, BX_READ, &mouse_flags_2);
    mouse_flags_1 = (mouse_flags_2 & 0x07) - 1;
if (mouse_flags_1 != 2) bx_panic("mouse_int(): mouse_flags_1 != 2\n");
    if (mouse_flags_1 > 7) bx_panic("mouse_int_handler(): new index > 7\n");
    bx_access_physical(BX_EBDA + 0x26, 1, BX_WRITE, &mouse_flags_1);
    }
  else {
    mouse_flags_1--;
    AL = 0;
    bx_access_physical(BX_EBDA + 0x26, 1, BX_WRITE, &mouse_flags_1);
    }

#if 0
{
  /* write mouse data byte to mouse data area */
  bx_access_physical(BX_EBDA + 0x28 + (2-index)*2, 1, BX_WRITE, &mouse_data);
  return;
}
#endif

  /* write mouse data byte to mouse data area */
  bx_access_physical(BX_EBDA + 0x28 + index, 1, BX_WRITE, &mouse_data);
bx_printf("mouse_int(): index = %u\n", (unsigned) index);
}


  /* int 15h function C2h: mouse bios interface */

  void
bx_mouse_bios(void)
{
#if 0
  Bit8u mouse_flags_1, mouse_flags_2;

  bx_access_physical(BX_EBDA + 0x27, 1, BX_READ, &mouse_flags_2);

  switch (AL) {
    case 0x00:
      if (BH == 0) { /* disable mouse */
        bx_panic("mouse: disable mouse unfinished\n");
        return;
        }
      else if (BH == 1) { /* enable mouse */
        if ( (mouse_flags_2 & 0x80)==0 ) {
          bx_set_CF(1); /* fail */
          AH = 0x05; /* cannot enable mouse, no far call handler installed */
          bx_printf("mouse: enable mouse, far call not installed\n");
          return;
          }
        bx_set_CF(0); /* success */
        AH = 0x00; /* success */
        bx_set_mouse_enable(1);
        mouse_flags_1 = mouse.package_size - 1; /* clear bits 3..7 ??? */
        bx_access_physical(BX_EBDA + 0x26, 1, BX_WRITE, &mouse_flags_1);
        bx_printf("mouse: enable mouse\n");
        return;
        }

      bx_set_CF(1);
      AH = 0x01; /* invalid subfunction */
      bx_panic("mouse: invalid subfunction AL=0, BH=%02x\n",
        (unsigned) BH);
      return;
      break;

    case 0x01: /* reset mouse */
      bx_set_mouse_enable(0);
      mouse.sample_rate  = 100; /* reports per second */
      mouse.resolution   = 100; /* dpi, 4 counts per millimeter */
      mouse.scaling      = 1;   /* 1:1 */
      if ( (mouse_flags_2 & 0x80)==0 ) {
        AH = 0x05; /* no far call handler installed */
        bx_printf("mouse: reset mouse, far call not installed\n");
        }
      else
        AH = 0x00; /* success */
      bx_set_CF(0); /* success */
      BH = 0; /* device ID */
      BL = 0xaa; /* attached device is a mouse */
      bx_printf("mouse: reset mouse\n");
      return;
      break;

    case 0x02: /* set sample rate */
      switch (BH) {
        case 0:
          mouse.sample_rate = 10;
          break;
        case 1:
          mouse.sample_rate = 20;
          break;
        case 2:
          mouse.sample_rate = 40;
          break;
        case 3:
          mouse.sample_rate = 60;
          break;
        case 4:
          mouse.sample_rate = 80;
          break;
        case 5:
          mouse.sample_rate = 100;
          break;
        case 6:
          mouse.sample_rate = 200;
          break;
        default:
          bx_set_CF(1); /* fail */
          AH = 0x02; /* invalid input value */
          return;
        }
      bx_set_CF(0); /* success */
      if ( (mouse_flags_2 & 0x80)==0 ) {
        AH = 0x05; /* no far call handler installed */
        bx_printf("mouse: function 2, far call not installed\n");
        }
      else
        AH = 0x00; /* success */
      bx_printf("mouse: set sample rate to %u reports/second\n",
        (unsigned) mouse.sample_rate);
      return;
      break;

    case 0x03: /* set resolution */
      switch (BH) {
        case 0:
          mouse.resolution = 25;  /* dpi, 1 count  / millimeter */
          break;
        case 1:
          mouse.resolution = 50;  /* dpi, 2 counts / millimeter */
          break;
        case 2:
          mouse.resolution = 100; /* dpi, 4 counts / millimeter */
          break;
        case 3:
          mouse.resolution = 200; /* dpi, 8 counts / millimeter */
          break;
        default:
          bx_panic("mouse: set resolution: invalid resolution\n");
          bx_set_CF(1); /* fail */
          AH = 0x02; /* invalid input value */
          return;
        }
      bx_set_CF(0); /* success */
      if ( (mouse_flags_2 & 0x80)==0 ) {
        AH = 0x05; /* no far call handler installed */
        bx_printf("mouse: set resolution, far call not installed\n");
        }
      else
        AH = 0x00; /* success */
      bx_printf("mouse: set resolution to %u dpi\n",
        (unsigned) mouse.resolution);
      return;
      break;

    case 0x04: /* get device ID */
      bx_set_CF(0); /* success */
      if ( (mouse_flags_2 & 0x80)==0 ) {
        AH = 0x05; /* no far call handler installed */
        bx_printf("mouse: get device ID, far call not installed\n");
        }
      else
        AH = 0x00; /* success */
      BH = 0; /* device ID */
      bx_printf("mouse: get device ID (returning 0)\n");
      return;
      break;

    case 0x05: /* initialize mouse */
      if (BH<1 || BH>8) {
        bx_printf("mouse: init_mouse: invalid package size of %u\n",
          (unsigned) BH);
        bx_set_CF(1);
        AH = 0x02; /* input value out of range */
        return;
        }
      mouse.package_size = BH;
if (mouse.package_size!=3) bx_panic("mouse.package_size!=3\n");
      bx_set_mouse_enable(0); /* disabled */
      mouse.sample_rate  = 100; /* reports per second */
      mouse.resolution   = 100; /* dpi, 4 counts per millimeter */
      mouse.scaling      = 1;   /* 1:1 */
      bx_printf("mouse: init mouse: package size set to %u\n",
        (unsigned) BH);

      /* resets device driver far call flag, bit7  ??? */
      mouse_flags_2 = (mouse_flags_2 & 0x80) | mouse.package_size;
      bx_access_physical(BX_EBDA + 0x27, 1, BX_WRITE, &mouse_flags_2);

      if ( (mouse_flags_2 & 0x80)==0 ) {
        AH = 0x05; /* no far call handler installed */
        bx_printf("mouse: init mouse, far call not installed\n");
        }
      else
        AH = 0x00; /* success */
      bx_set_CF(0); /* success */
      return;
      break;

    case 0x06:
      switch (BH) {
        case 0: /* get return status */
          bx_panic("mouse: get return status unfinished\n");
          return;
          break;
        case 1: /* set scaling factor to 1:1 */
          if ( (mouse_flags_2 & 0x80)==0 ) {
            AH = 0x05; /* no far call handler installed */
            bx_printf("mouse: set scaling 1:1, far call not installed\n");
            }
          else
            AH = 0x00; /* success */
          bx_set_CF(0); /* success */
          mouse.scaling = 1;
          bx_printf("mouse: set scaling factor to 1:1\n");
          return;
          break;
        case 2: /* set scaling factor to 2:1 */
          if ( (mouse_flags_2 & 0x80)==0 ) {
            AH = 0x05; /* no far call handler installed */
            bx_printf("mouse: set scaling 2:1, far call not installed\n");
            }
          else
            AH = 0x00; /* success */
          bx_set_CF(0); /* success */
          mouse.scaling = 2;
          bx_printf("mouse: set scaling factor to 2:1\n");
          return;
          break;
        default:
          AH = 0x01; /* invalid subfunction call */
          bx_set_CF(1); /* fail */
          bx_panic("mouse: AL=6, invalid subfunc BH=%02x\n",
            (unsigned) BH);
          return;
        }
      break;

    case 0x07: /* set mouse handler address (ES:BX) */
      mouse.handler_seg = bx_cpu.es.selector.value;
      mouse.handler_offset = BX;
if (mouse.handler_seg==0 && mouse.handler_offset==0)
  bx_panic("mouse: set mouse handler address: reset to 0000:0000\n");
      /* write into extended BIOS data area: mouse device driver far call */
      bx_access_physical(BX_EBDA + 0x22, 2, BX_WRITE, &mouse.handler_offset);
      bx_access_physical(BX_EBDA + 0x24, 2, BX_WRITE, &mouse.handler_seg);

      mouse_flags_2 |= 0x80; /* device driver far call flag */
      bx_access_physical(BX_EBDA + 0x27, 1, BX_WRITE, &mouse_flags_2);

      AH = 0x00; /* success */
      bx_set_CF(0); /* success */
      bx_printf("mouse: set mouse handler address\n");
      return;
      break;

    default:
      bx_set_CF(1); /* fail */
      AH = 0x01; /* invalid subfunction call */
      bx_panic("mouse: unknown mouse function: int15 AH=C2, AL=%02x\n",
        (unsigned) AL);
      return;
    }
#endif

  bx_printf("MOUSE: *** int 15h function AX=%04x, BX=%04x not yet supported!\n",
    (unsigned) AX, (unsigned) BX);
  bx_set_CF(1);
  AH = 0x86;
}
#endif /* BX_EXTERNAL_ENVIRONMENT==0 */
