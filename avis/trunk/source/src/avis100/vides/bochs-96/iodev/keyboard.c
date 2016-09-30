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


#include <sys/stat.h>
#ifndef WIN32
   #include <unistd.h>
#endif
#include <stdlib.h>

#include "iodev.h"


void enqueue_key(Bit8u scan_code, Bit8u ascii_code);
int  dequeue_key(Bit8u *scan_code, Bit8u *ascii_code, Boolean incr);
void bx_set_kbd_clock_enable(Bit8u value);
void bx_set_aux_clock_enable(Bit8u value);
void bx_set_mouse_enable(Bit8u value);
static void kbd_ctrl_to_kbd(Bit8u value);
static void keyboard_timer_handler(void);
static void kbd_enQ(Bit8u scancode);
static void ctrl_activate_timer(void);
static void controller_enQ(Bit8u data);


//#define BX_KBD_ELEMENTS 16 // Changed that to 256, needed bigger buffer to send strings from asax easily, Fred
#define BX_KBD_ELEMENTS 256
#define BX_MOUSE_BUFF_SIZE 50

static struct {
  int   num_elements;
  Bit8u buffer[BX_KBD_ELEMENTS];
  int   head;
  void  (* enQ)(Bit8u scancode);
  } kbd_internal_buffer;

static struct {
  int   num_elements;
  Bit8u buffer[BX_MOUSE_BUFF_SIZE];
  int   head;
  } mouse_internal_buffer;

Boolean bx_mouse_device_enabled = 0;


struct {
  /* status bits */
  Boolean pare;
  Boolean tim;
  Boolean auxb;
  Boolean keyl;
  Boolean c_d; /* 1=command to port 64h, 0=data to port 60h */
  Boolean sysf;
  Boolean inpb;
  Boolean outb;

  Boolean scan_convert;
  Boolean kbd_clock_enabled;
  Boolean aux_clock_enabled;
  Boolean allow_irq1;
  Boolean allow_irq12;
  Bit8u   output_buffer;
  Bit8u   last_comm;
  Bit8u   expecting_port60h;
  void  (* enQ)(Bit8u data);
  void  (* activate_timer)(void);
  Boolean  timer_pending;
  } kbd_controller;

unsigned keyboard_timer_handle = BX_NULL_TIMER_HANDLE;


#if BX_CPU < 2
  /*
   * XT keyboard
   */
static Bit8u int09h_code[] = {
  0x50,              /* push ax */
  0xE4, 0x60,        /* in   al, #60h   ;read key from keyboard controller */
#if 0
  0xB4, 0x4F,        /* mov  ah, #0x4f  ;prepare for int 15 func 4f	*/
  0xf9,              /* stc */
  0xcd, 0x15,        /* int 0x15        ;call int 15 funct 4f */
#endif
#if 0
  0x73, 0x03,        /* jnc     done: */
#endif
  0x0F, 0x1F, 0x09,  /* pseudo-int 09h */

  /*              done: */
  0xB0, 0x20,        /* mov al, #0x20 */
  0xE6, 0x20,        /* out 0x20, al    ;send EOI to PIC */
  0x58,              /* pop ax */
  0xCF,              /* iret */
  };
#endif /* BX_CPU == 0 */


#if BX_EXTERNAL_ENVIRONMENT==0
#if BX_CPU >= 2
  /*
   * AT keyboard
   */
static Bit8u int09h_code[] = {
  0xfa,              /* CLI */
  0x50,              /* push ax */
  0xE4, 0x64,        /* in AL, #64h  ; controller status */
  0xa8, 0x21,        /* test AL, #21 (bits 5,0) */
  0x75, 0x02,        /* jnz  OUTB_FULL */
  0x58,              /* pop ax */
  0xCF,              /* iret */

                     /* OUTB_FULL: */
  0xa8, 0x20,        /* test AL, #20 (bit 5) */
  0x74, 0x05,        /* jz   KEY_DATA */
                     /* MOUSE_DATA: */
  0x0F, 0x1F, 0x09,  /* pseudo-int 09h */
  0x58,              /* pop ax */
  0xCF,              /* iret */

                     /* KEY_DATA: */
  0xB0, 0xAD,        /* mov AL, #AD     ;disable keyboard */
  0xE6, 0x64,        /* out #0x64, AL */

  0xE4, 0x60,        /* in   al, #60h   ;read key from keyboard controller */

#ifdef BX_CALL_INT15_4F
  0xa8, 0x80,        /* test al, #80    ;look for key release */
  0x75, 0x07,        /* jnz PROCESS_KEY */

                     /*   ;prepare for int 15 AH=4f, AL=key */
  0xB4, 0x4F,        /* mov  ah, #0x4f */
  0xf9,              /* stc */
  0xcd, 0x15,        /* int 0x15 */
                     /*   ;if carry clear, ignore key & done */
  0x73, 0x03,        /* jnc     DONE */
#endif

                     /* PROCESS_KEY: */
  0x0F, 0x1F, 0x09,  /* pseudo-int 09h */

                     /* DONE: */
  0xB0, 0xAE,        /* mov AL, #AE     ;enable keyboard */
  0xE6, 0x64,        /* out #0x64, AL */
  0xB0, 0x20,        /* mov al, #0x20   ;send EOI to PIC */
  0xE6, 0x20,        /* out 0x20, al */
  0x58,              /* pop ax */
  0xCF,              /* iret */
  };
#endif /* BX_CPU >= 2 */
#endif


#if 0
static Bit8u int16h_code[] = {
  0x1e,                    /* push ds */
  0x53,                    /* push bx */

  0x80, 0xfc, 0x00,        /* cmp AH, #0 */
  0x74, 0x09,              /* je  do_f0 */
  0x0F, 0x1F, 0x16,        /* pseudo-int 16h */
  0x5b,                    /* pop  bx */
  0x1f,                    /* pop  ds */
  0xfb,                    /* sti */
  0xca, 0x02, 0x00,        /* retf #2 */

                           /* do_f0:  ; INT 16h funct 0 */
  0xbb, 0x40, 0x00,        /* mov  bx, #40h */
  0x8e, 0xdb,              /* mov  ds, bx */
                           /* wait_for_key: */
  0xfa,                    /* cli */
  0x8b, 0x1e, 0x1a, 0x00,  /* mov  bx, [0x1a]  ; head */
  0x3b, 0x1e, 0x1c, 0x00,  /* cmp  bx, [0x1c]  ; tail */
  0xfb,                    /* sti */
  0x90,                    /* nop */
  0x74, 0xf3,              /* je   wait_for_key */

  0x0F, 0x1F, 0x16,        /* pseudo-int 16h */
  0x5b,                    /* pop  bx */
  0x1f,                    /* pop  ds */
  0xcf,                    /* iret */
  };
#endif



#if BX_EXTERNAL_ENVIRONMENT==0
static Bit8u int16h_code[] = {
  0x51,                    /* push cx */
  0xb9, 0x00, 0x00,        /* mov  cx, #0000 */
  0x8a, 0xcc,              /* mov  cl, ah  ; function number */
  0xe3, 0x18,              /* jcxz DO_F0  ; int16h, function 0 */
  0x59,                    /* pop  cx */
  0x0F, 0x1F, 0x16,        /* pseudo-int 16h */
  0x74, 0x09,              /* jz   ZERO_SET       */
                           /* ZERO CLEAR:         */
  0x55,                    /* push bp              */
  0x8b, 0xec,              /* mov BP, SP           */
  0x80, 0x66, 0x06, 0xbf,  /* and SS:[BP][06], #bf */
  0x5d,                    /* pop BP               */
  0xcf,                    /* iret                 */

                           /* ZERO_SET:           */
  0x55,                    /* push bp              */
  0x8b, 0xec,              /* mov BP, SP           */
  0x80, 0x4e, 0x06, 0x40,  /* or SS:[BP][06], #40  */
  0x5d,                    /* pop BP               */
  0xcf,                    /* iret                 */

                           /* DO_F0:  ; INT 16h funct 0 */
  0x59,                    /* pop  cx ; CX not needed anymore */
  0x1e,                    /* push ds */
  0x53,                    /* push bx */
  0xbb, 0x40, 0x00,        /* mov  bx, #40h */
  0x8e, 0xdb,              /* mov  ds, bx */

                           /* WAIT_FOR_KEY: */
  0xfa,                    /* cli */
  0x8b, 0x1e, 0x1a, 0x00,  /* mov  bx, [0x1a]  ; head */
  0x3b, 0x1e, 0x1c, 0x00,  /* cmp  bx, [0x1c]  ; tail */
#if 0
  0x75, 0x0b,              /* jne  KEY_FOUND */
#endif
  0x75, 0x04,              /* jne  KEY_FOUND */
  0xfb,                    /* sti */
  0x90, /* nop */

#if 0
                           /* no key yet, call int 15h, function AX=9002 */
  0x50,                    /* push AX */
  0xb8, 0x02, 0x90,        /* mov AX, #9002 */
  0xcd, 0x15,              /* int 15h */
  0x58,                    /* pop  AX */
  0xeb, 0xea,              /* jmp   WAIT_FOR_KEY */
#endif
  0xeb, 0xf1,              /* jmp   WAIT_FOR_KEY */

                           /* KEY_FOUND: */
  0x5b,                    /* pop  bx */
  0x1f,                    /* pop  ds */
  0x0F, 0x1F, 0x16,        /* pseudo-int 16h */
                           /* notify int16 complete w/ int 15h, function AX=9102 */
#if 0
  0x50,                    /* push AX */
  0xb8, 0x02, 0x91,        /* mov AX, #9102 */
  0xcd, 0x15,              /* int 15h */
  0x58,                    /* pop  AX */
#endif
  0xcf,                    /* iret */
  };
#endif


#define none 0
#define MAX_SCAN_CODE 0x53

struct {
  Bit16u normal;
  Bit16u shift;
  Bit16u control;
  Bit16u alt;
  } scan_to_scanascii[MAX_SCAN_CODE + 1] = {
      {   none,   none,   none,   none },
      { 0x011b, 0x011b, 0x011b, 0x0100 }, /* escape */
      { 0x0231, 0x0221,   none, 0x7800 }, /* 1! */
      { 0x0332, 0x0340, 0x0300, 0x7900 }, /* 2@ */
      { 0x0433, 0x0423,   none, 0x7a00 }, /* 3# */
      { 0x0534, 0x0524,   none, 0x7b00 }, /* 4$ */
      { 0x0635, 0x0625,   none, 0x7c00 }, /* 5% */
      { 0x0736, 0x075e, 0x071e, 0x7d00 }, /* 6^ */
      { 0x0837, 0x0826,   none, 0x7e00 }, /* 7& */
      { 0x0938, 0x092a,   none, 0x7f00 }, /* 8* */
      { 0x0a39, 0x0a28,   none, 0x8000 }, /* 9( */
      { 0x0b30, 0x0b29,   none, 0x8100 }, /* 0) */
      { 0x0c2d, 0x0c5f, 0x0c1f, 0x8200 }, /* -_ */
      { 0x0d3d, 0x0d2b,   none, 0x8300 }, /* =+ */
      { 0x0e08, 0x0e08, 0x0e7f,   none }, /* backspace */
      { 0x0f09, 0x0f00,   none,   none }, /* tab */
      { 0x1071, 0x1051, 0x1011, 0x1000 }, /* Q */
      { 0x1177, 0x1157, 0x1117, 0x1100 }, /* W */
      { 0x1265, 0x1245, 0x1205, 0x1200 }, /* E */
      { 0x1372, 0x1352, 0x1312, 0x1300 }, /* R */
      { 0x1474, 0x1454, 0x1414, 0x1400 }, /* T */
      { 0x1579, 0x1559, 0x1519, 0x1500 }, /* Y */
      { 0x1675, 0x1655, 0x1615, 0x1600 }, /* U */
      { 0x1769, 0x1749, 0x1709, 0x1700 }, /* I */
      { 0x186f, 0x184f, 0x180f, 0x1800 }, /* O */
      { 0x1970, 0x1950, 0x1910, 0x1900 }, /* P */
      { 0x1a5b, 0x1a7b, 0x1a1b,   none }, /* [{ */
      { 0x1b5d, 0x1b7d, 0x1b1d,   none }, /* ]} */
      { 0x1c0d, 0x1c0d, 0x1c0a,   none }, /* Enter */
      {   none,   none,   none,   none }, /* L Ctrl */
      { 0x1e61, 0x1e41, 0x1e01, 0x1e00 }, /* A */
      { 0x1f73, 0x1f53, 0x1f13, 0x1f00 }, /* S */
      { 0x2064, 0x2044, 0x2004, 0x2000 }, /* D */
      { 0x2166, 0x2146, 0x2106, 0x2100 }, /* F */
      { 0x2267, 0x2247, 0x2207, 0x2200 }, /* G */
      { 0x2368, 0x2348, 0x2308, 0x2300 }, /* H */
      { 0x246a, 0x244a, 0x240a, 0x2400 }, /* J */
      { 0x256b, 0x254b, 0x250b, 0x2500 }, /* K */
      { 0x266c, 0x264c, 0x260c, 0x2600 }, /* L */
      { 0x273b, 0x273a,   none,   none }, /* ;: */
      { 0x2827, 0x2822,   none,   none }, /* '" */
      { 0x2960, 0x297e,   none,   none }, /* `~ */
      {   none,   none,   none,   none }, /* L shift */
      { 0x2b5c, 0x2b7c, 0x2b1c,   none }, /* |\ */
      { 0x2c7a, 0x2c5a, 0x2c1a, 0x2c00 }, /* Z */
      { 0x2d78, 0x2d58, 0x2d18, 0x2d00 }, /* X */
      { 0x2e63, 0x2e43, 0x2e03, 0x2e00 }, /* C */
      { 0x2f76, 0x2f56, 0x2f16, 0x2f00 }, /* V */
      { 0x3062, 0x3042, 0x3002, 0x3000 }, /* B */
      { 0x316e, 0x314e, 0x310e, 0x3100 }, /* N */
      { 0x326d, 0x324d, 0x320d, 0x3200 }, /* M */
      { 0x332c, 0x333c,   none,   none }, /* ,< */
      { 0x342e, 0x343e,   none,   none }, /* .> */
      { 0x352f, 0x353f,   none,   none }, /* /? */
      {   none,   none,   none,   none }, /* R Shift */
      { 0x372a, 0x372a,   none,   none }, /* * */
      {   none,   none,   none,   none }, /* L Alt */
      { 0x3920, 0x3920, 0x3920, 0x3920 }, /* space */
      {   none,   none,   none,   none }, /* caps lock */
      { 0x3b00, 0x5400, 0x5e00, 0x6800 }, /* F1 */
      { 0x3c00, 0x5500, 0x5f00, 0x6900 }, /* F2 */
      { 0x3d00, 0x5600, 0x6000, 0x6a00 }, /* F3 */
      { 0x3e00, 0x5700, 0x6100, 0x6b00 }, /* F4 */
      { 0x3f00, 0x5800, 0x6200, 0x6c00 }, /* F5 */
      { 0x4000, 0x5900, 0x6300, 0x6d00 }, /* F6 */
      { 0x4100, 0x5a00, 0x6400, 0x6e00 }, /* F7 */
      { 0x4200, 0x5b00, 0x6500, 0x6f00 }, /* F8 */
      { 0x4300, 0x5c00, 0x6600, 0x7000 }, /* F9 */
      { 0x4400, 0x5d00, 0x6700, 0x7100 }, /* F10 */
      {   none,   none,   none,   none }, /* Num Lock */
      {   none,   none,   none,   none }, /* Scroll Lock */
      { 0x4700, 0x4737, 0x7700,   none }, /* 7 Home */
      { 0x4800, 0x4838,   none,   none }, /* 8 UP */
      { 0x4900, 0x4939, 0x8400,   none }, /* 9 PgUp */
      { 0x4a2d, 0x4a2d,   none,   none }, /* - */
      { 0x4b00, 0x4b34, 0x7300,   none }, /* 4 Left */
      { 0x4c00, 0x4c35,   none,   none }, /* 5 */
      { 0x4d00, 0x4d36, 0x7400,   none }, /* 6 Right */
      { 0x4e2b, 0x4e2b,   none,   none }, /* + */
      { 0x4f00, 0x4f31, 0x7500,   none }, /* 1 End */
      { 0x5000, 0x5032,   none,   none }, /* 2 Down */
      { 0x5100, 0x5133, 0x7600,   none }, /* 3 PgDn */
      { 0x5200, 0x5230,   none,   none }, /* 0 Ins */
      { 0x5300, 0x532e,   none,   none }  /* Del */
      };




  void
bx_init_keyboard_hardware(void)
{
  Bit32u i;
  bx_iodev_t  io_device;

  /* 8042 keyboard controller */
  io_device.read_funct   = bx_keyboard_io_read_handler;
  io_device.write_funct  = bx_keyboard_io_write_handler;
  io_device.handler_name = "8042 Keyboard controller";
  io_device.start_addr   = 0x0060;
  io_device.end_addr     = 0x0060;
  io_device.irq          = 1;
  bx_register_io_handler(io_device);

  io_device.start_addr   = 0x0064;
  io_device.end_addr     = 0x0064;
  bx_register_io_handler(io_device);


  kbd_internal_buffer.num_elements = 0;
  for (i=0; i<BX_KBD_ELEMENTS; i++)
    kbd_internal_buffer.buffer[i] = 0;
  kbd_internal_buffer.head = 0;
  kbd_internal_buffer.enQ = kbd_enQ;

  mouse_internal_buffer.num_elements = 0;
  for (i=0; i<BX_MOUSE_BUFF_SIZE; i++)
    mouse_internal_buffer.buffer[i] = 0;
  mouse_internal_buffer.head = 0;

  kbd_controller.pare = 0;
  kbd_controller.tim  = 0;
  kbd_controller.auxb = 0;
  kbd_controller.keyl = 1;
  kbd_controller.c_d  = 1;
  kbd_controller.sysf = 0;
  kbd_controller.inpb = 0;
  kbd_controller.outb = 0;

  kbd_controller.scan_convert = 1;
  kbd_controller.kbd_clock_enabled = 1;
  kbd_controller.aux_clock_enabled = 0;
  kbd_controller.allow_irq1 = 1;
  kbd_controller.allow_irq12 = 1;
  kbd_controller.output_buffer = 0;
  kbd_controller.last_comm = 0;
  kbd_controller.expecting_port60h = 0;
  kbd_controller.enQ = controller_enQ;
  kbd_controller.activate_timer = ctrl_activate_timer;

  keyboard_timer_handle = bx_register_timer( keyboard_timer_handler,
    20000, /* one shot */ 0, /* inactive */ 0);
  kbd_controller.timer_pending = 0;
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_keyboard_bios(void)
{
  Bit8u  zero8;
  Bit16u zero16, head_offset, tail_offset;
  Bit16u start_of_buffer, end_of_buffer;
  Bit32u i;

  bx_register_int_vector(0x09, int09h_code, sizeof(int09h_code),
      bx_int09h_handler
#ifdef WIN32DEBUG
      ,H_INT09
#endif
      );

  bx_register_int_vector(0x16, int16h_code, sizeof(int16h_code),
      bx_int16h_handler
#ifdef WIN32DEBUG
      ,H_INT16
#endif
      );

  zero8  = 0;
  zero16 = 0;

  /* keyboard shift flags, set 1 */
  bx_access_physical(0x417, 1, BX_WRITE, &zero8);

  /* keyboard shift flags, set 2 */
  bx_access_physical(0x418, 1, BX_WRITE, &zero8);

  /* keyboard alt-numpad work area */
  bx_access_physical(0x419, 1, BX_WRITE, &zero8);

  /* keyboard head of buffer pointer */
  head_offset = 0x001E;
  bx_access_physical(0x41A, 2, BX_WRITE, &head_offset);

  /* keyboard end of buffer pointer */
  tail_offset = head_offset;
  bx_access_physical(0x41C, 2, BX_WRITE, &tail_offset);

  /* keyboard buffer */
  for (i=0; i<16; i++)
    bx_access_physical(0x41E + i*2, 2, BX_WRITE, &zero16);

  /* keyboard ctrl-break flag */
  bx_access_physical(0x471, 1, BX_WRITE, &zero8);

  /* keyboard pointer to start of buffer */
  start_of_buffer = 0x001E;
  bx_access_physical(0x480, 2, BX_WRITE, &start_of_buffer);

  /* keyboard pointer to end of buffer */
  end_of_buffer = 0x003E;
  bx_access_physical(0x482, 2, BX_WRITE, &end_of_buffer);

  /* keyboard status flags 3 */
  bx_access_physical(0x496, 1, BX_WRITE, &zero8);

  /* keyboard status flags 4 */
  bx_access_physical(0x497, 1, BX_WRITE, &zero8);
}
#endif




  Bit32u
bx_keyboard_io_read_handler(Bit32u address, unsigned io_len)
{
  if (bx_dbg.keyboard)
    bx_printf("KBD: read from port 0x%04x\n", (unsigned) address);


  if (address == 0x60) { /* output buffer */
    Bit8u val;
    if (kbd_controller.outb) { /* byte available */
      val = kbd_controller.output_buffer;
      kbd_controller.output_buffer = 0;
      kbd_controller.outb = 0;
      kbd_controller.auxb = 0;

      kbd_controller.activate_timer();
      if (bx_dbg.keyboard)
        bx_printf("KBD: READ(%02x) = %02x\n", (unsigned) address,
          (unsigned) val);
      return(val);
      }
    else {
      if (bx_dbg.keyboard) {
        bx_printf("KBD: num_elements = %d\n", kbd_internal_buffer.num_elements);
        bx_printf("KBD: read from port 60h with outb empty\n");
        }
      val = 0;
      return(val);
      }
    }

#if BX_CPU >= 2
  else if (address == 0x64) { /* status register */
    Bit8u val;

    val = (kbd_controller.pare << 7)  |
          (kbd_controller.tim  << 6)  |
          (kbd_controller.auxb << 5)  |
          (kbd_controller.keyl << 4)  |
          (kbd_controller.c_d  << 3)  |
          (kbd_controller.sysf << 2)  |
          (kbd_controller.inpb << 1)  |
          kbd_controller.outb;
    return(val);
    }

#else /* BX_CPU > 0 */
  /* XT MODE, System 8255 Mode Register */
  else if (address == 0x64) { /* status register */
    if (bx_dbg.keyboard)
      bx_printf("KBD: IO read from port 64h, system 8255 mode register\n");
    return(kbd_controller.outb);
    }
#endif /* BX_CPU > 0 */

  else {
    bx_panic("KBD: unknown address in io read to keyboard port %x\n",
      (unsigned) address);
    return(0); /* keep compiler happy */
    }
}

  void
bx_keyboard_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  Bit8u command_byte;

  if (bx_dbg.keyboard)
    bx_printf("KBD: WRITE(%02x) = %02x\n", (unsigned) address,
      (unsigned) value);

  switch (address) {
    case 0x60: /* input buffer */
      if (kbd_controller.expecting_port60h) { /* command last sent to port 64h */
        kbd_controller.expecting_port60h = 0;
        kbd_controller.c_d = 0; /* data byte written last to 0x60 */
        if (kbd_controller.inpb) {
          bx_panic("KBD: write to port 60h, not ready for write\n");
          }
        switch (kbd_controller.last_comm) {
          case 0x60: /* write command byte */
            {
            Boolean scan_convert, ignore_parity, disable_keyboard,
                    override_inhibit, successful_test, outb_causes_irq1;

            scan_convert = (value >> 6) & 0x01;
            ignore_parity = (value >> 5) & 0x01;
            disable_keyboard = (value >> 4) & 0x01;
            override_inhibit = (value >> 3) & 0x01;
            successful_test = (value >> 2) & 0x01;
            outb_causes_irq1 = (value >> 0) & 0x01;
            if (value != 0x45)
              bx_panic("keyboard: write command byte: value!=45h\n");
            }
            break;
          case 0xd1: /* write output port */
            if (bx_dbg.keyboard)
              bx_printf("KBD: write output port with value %02xh\n",
                (unsigned) value);
            bx_set_enable_a20( (value & 0x02) != 0 );
            if (!(value & 0x01)) bx_panic("KBD: IO write: processor reset requested!\n");
            break;

          default:
            bx_panic("KBD: === unsupported write to port 60h(lastcomm=%02x): %02x\n",
              (unsigned) kbd_controller.last_comm, (unsigned) value);
          }
        }
      else {
        kbd_controller.c_d = 0; /* data byte written last to 0x60 */
        kbd_controller.expecting_port60h = 0;
        /* pass byte to keyboard */
        /* ??? should conditionally pass to mouse device here ??? */
        if (kbd_controller.kbd_clock_enabled==0) {
          bx_panic("KBD: keyboard disabled & send of byte %02x to kbd\n",
            (unsigned) value);
          }
        kbd_ctrl_to_kbd(value);
        }
      break;

    case 0x64: /* control register */
      kbd_controller.c_d = 1; /* command byte written last to 0x64 */
      kbd_controller.last_comm = value;
      switch (value) {
        case 0x20: /* get keyboard command byte */
          if (bx_dbg.keyboard)
            bx_printf("KBD: get keyboard command byte\n");
          command_byte =
            (kbd_controller.scan_convert << 6) |
            ((!kbd_controller.aux_clock_enabled) << 5) |
            ((!kbd_controller.kbd_clock_enabled) << 4) |
            (0 << 3) |
            (kbd_controller.sysf << 2) |
            (kbd_controller.allow_irq12 << 1) |
            (kbd_controller.allow_irq1  << 0);
          kbd_controller.enQ(command_byte);
          break;
        case 0x60: /* write command byte */
          if (bx_dbg.keyboard)
            bx_printf("KBD: write command byte\n");
          /* following byte written to port 60h is command byte */
          kbd_controller.expecting_port60h = 1;
          break;
        case 0xa7: /* disable the aux device */
          bx_set_aux_clock_enable(0);
          if (bx_dbg.keyboard) bx_printf("KBD: aux device disabled\n");
          break;
        case 0xa8: /* enable the aux device */
          bx_set_aux_clock_enable(1);
          if (bx_dbg.keyboard) bx_printf("KBD: aux device enabled\n");
          break;
        case 0xaa: /* motherboard controller self test */
          if (bx_dbg.keyboard) bx_printf("KBD: Self Test\n");
          kbd_controller.sysf = 1; /* self test complete */
          kbd_controller.enQ(0x55); /* controller OK */
          break;
        case 0xad: /* disable keyboard */
          bx_set_kbd_clock_enable(0);
          if (bx_dbg.keyboard) bx_printf("KBD: keyboard disabled\n");
          break;
        case 0xae: /* enable keyboard */
          bx_set_kbd_clock_enable(1);
          if (bx_dbg.keyboard) bx_printf("KBD: keyboard enabled\n");
          break;
        case 0xc0: /* read input port */
          kbd_controller.enQ(0x60); /* monochrome/normal/512k */
          break;
        case 0xd0: /* read output port: next byte read from port 60h */
          if (bx_dbg.keyboard)
            bx_printf("KBD: io write to port 64h, command d0h (partial)\n");
          kbd_controller.enQ(
              (kbd_controller.auxb << 5) |
              (kbd_controller.outb << 4) |
              (bx_get_enable_a20()<<1) |
              0x01); /* ??? should be 0 or 1 ??? */
          break;

        case 0xd1: /* write output port: next byte written to port 60h */
          if (bx_dbg.keyboard)
            bx_printf("KBD: write output port\n");
          /* following byte to port 60h written to output port */
          kbd_controller.expecting_port60h = 1;
          break;

        case 0xfe: /* System Reset, transition to real mode */
          if (bx_dbg.keyboard)
            bx_printf("KBD: system reset\n");
          bx_printf("system reset via KBD ctrl command FEh\n");
          bx_reset_cpu();
          break;
        case 0xa9:
        case 0xab:
        case 0xc1:
        case 0xc2:
        case 0xd2:
        case 0xd3:
        case 0xd4:
        case 0xe0:
        default:
          if (value==0xff || (value>=0xf0 && value<=0xfd)) {
            /* useless pulse output bit commands ??? */
            if (bx_dbg.keyboard)
              bx_printf("KBD: io write to port 64h, useless command %02x\n",
                (unsigned) value);
            return;
	    }
          bx_panic("KBD: unsupported io write to keyboard port %x, value = %x\n",
            (unsigned) address, (unsigned) value);
          break;
        }
      break;

    default: bx_panic("KBD: unknown address in keyboard_io_write_handler()\n");
    }
}



#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_int09h_handler(int vector)
{
  Bit8u scancode, asciicode, shift_flags;


  if (kbd_controller.auxb) {
    bx_panic("int09: auxb set\n");
    }

  if (AL == 0) {
    bx_printf("KBD: int09 handler: AL=0\n");
    return;
    }

  if (bx_dbg.keyboard)
    bx_printf("KBD: int09h handler called, AL=%02x.\n", (unsigned) AL);

#if 0
  bx_outp(0x20, 0x20, 1);  /* EOI to master PIC */

  status = bx_inp(0x64, 1);
  if ( !(status & 0x01) )
    /* no bytes available in motherboard controller's output buffer */
    return;
  scancode = bx_inp(0x60, 1);
#endif

  scancode = AL;

  bx_access_physical(0x417, 1, BX_READ, &shift_flags);

  switch (scancode) {
    case 0x3a: /* Caps Lock press */
      shift_flags |= 0x40;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;
    case 0xba: /* Caps Lock release */
      shift_flags &= ~0x40;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;

    case 0x2a: /* L Shift press */
      shift_flags |= 0x02;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;
    case 0xaa: /* L Shift release */
      shift_flags &= ~0x02;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;

    case 0x36: /* R Shift press */
      shift_flags |= 0x01;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;
    case 0xb6: /* R Shift release */
      shift_flags &= ~0x01;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;

    case 0x1d: /* L Cttrl press */
      shift_flags |= 0x04;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;
    case 0x9d: /* L Cttrl release */
      shift_flags &= ~0x04;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;

    case 0x38: /* L Alt press */
      shift_flags |= 0x08;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;
    case 0xb8: /* L Alt release */
      shift_flags &= ~0x08;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;

    case 0x45: /* Num Lock press */
      shift_flags |= 0x20;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;
    case 0xc5: /* Num Lock release */
      shift_flags &= ~0x20;
      bx_access_physical(0x417, 1, BX_WRITE, &shift_flags);
      break;

    default:
      if (scancode & 0x80) return; /* toss key releases ... */
      if (scancode > MAX_SCAN_CODE) {
        bx_panic("KBD: bx_int09h_handler(): unknown scancode read!\n");
        return;
        }
      if (shift_flags & 0x08) { /* ALT */
        asciicode = scan_to_scanascii[scancode].alt;
        scancode = scan_to_scanascii[scancode].alt >> 8;
        }
      else if (shift_flags & 0x04) { /* CONTROL */
        asciicode = scan_to_scanascii[scancode].control;
        scancode = scan_to_scanascii[scancode].control >> 8;
        }
      else if (shift_flags & 0x43) { /* CAPSLOCK + LSHIFT + RSHIFT */
        /* check if both CAPSLOCK and a SHIFT key are pressed */
        if ((shift_flags & 0x03) && (shift_flags & 0x40)) {
          asciicode = scan_to_scanascii[scancode].normal;
          scancode = scan_to_scanascii[scancode].normal >> 8;
          }
        else {
          asciicode = scan_to_scanascii[scancode].shift;
          scancode = scan_to_scanascii[scancode].shift >> 8;
          }
        }
      else {
        asciicode = scan_to_scanascii[scancode].normal;
        scancode = scan_to_scanascii[scancode].normal >> 8;
        }
      if (scancode==0 && asciicode==0)
        bx_printf("KBD: bx_int09h_handler(): scancode & asciicode are zero?\n"); /* WCA: used to be a bx_panic */
      else
        enqueue_key(scancode, asciicode);
      break;
    }
}

  void
bx_int16h_handler(int vector)
{
  if (bx_dbg.keyboard && AH!=1)
    bx_printf("KBD: int16h handler called.\n");


  switch (AH) {
    case 0x00: /* read keyboard input */

      if ( !dequeue_key(&AH, &AL, 1) ) {
        bx_panic("KBD: int16h: out of keyboard input\n");
        }
      if (bx_dbg.keyboard)
        bx_printf("KBD: f0 returning AH:%02x AL:%02x\n",
          (unsigned) AH, (unsigned) AL);
      break;

    case 0x01: /* check keyboard status */
      if ( !dequeue_key(&AH, &AL, 0) ) {
#if 0
        if (bx_dbg.keyboard)
          bx_printf("KBD: out of input (check)\n");
#endif
        bx_set_ZF(1);
        return;
        }
      bx_set_ZF(0);
      if (bx_dbg.keyboard)
        bx_printf("KBD: f1 returning AH:%02x AL:%02x\n",
          (unsigned) AH, (unsigned) AL);
      break;

    case 0x02: /* get shift flag status */
      /*AL = 0;*/
      bx_access_physical(0x417, 1, BX_READ, &AL);
      if (bx_dbg.keyboard)
        bx_printf("KBD: partially supported int 16h function 02h\n");
      break;

    default:
      /*bx_set_ZF(1);*/
      /* ??? */
      bx_printf("KBD: unsupported int 16h function %xh\n", (int) AH);
    }
}

  void
enqueue_key(Bit8u scan_code, Bit8u ascii_code)
{
  Bit16u buffer_start, buffer_end, buffer_head, buffer_tail, temp_tail;

#ifdef BX_DEBUG
  if (bx_dbg.keyboard)
    bx_printf("KBD:   enqueue_key() called scan:%02x, ascii:%02x\n",
      scan_code, ascii_code);
#endif

#if BX_CPU == 0
  buffer_start = 0x001E;
  buffer_end   = 0x003E;
#else
  bx_access_physical(0x480, 2, BX_READ, &buffer_start);
  bx_access_physical(0x482, 2, BX_READ, &buffer_end);
#endif

  bx_access_physical(0x41A, 2, BX_READ, &buffer_head);
  bx_access_physical(0x41C, 2, BX_READ, &buffer_tail);

  temp_tail = buffer_tail;
  buffer_tail += 2;
  if (buffer_tail >= buffer_end)
    buffer_tail = buffer_start;

  if (buffer_tail == buffer_head) {
    bx_panic("KBD: dropped key scan=%02x, ascii=%02x\n",
      (int) scan_code, (int) ascii_code);
    return;
    }

  bx_access_physical(0x400 + temp_tail,   1, BX_WRITE, &ascii_code);
  bx_access_physical(0x400 + temp_tail+1, 1, BX_WRITE, &scan_code);
  bx_access_physical(0x41C, 2, BX_WRITE, &buffer_tail);
}


  int
dequeue_key(Bit8u *scan_code, Bit8u *ascii_code, Boolean incr)
{
  Bit16u buffer_start, buffer_end, buffer_head, buffer_tail;

#if 0
#ifdef BX_DEBUG
  if (bx_dbg.keyboard)
    bx_printf("KBD:   dequeue_key() called.\n");
#endif
#endif

#if BX_CPU == 0
  buffer_start = 0x001E;
  buffer_end   = 0x003E;
#else
  bx_access_physical(0x480, 2, BX_READ, &buffer_start);
  bx_access_physical(0x482, 2, BX_READ, &buffer_end);
#endif

  bx_access_physical(0x41A, 2, BX_READ, &buffer_head);
  bx_access_physical(0x41C, 2, BX_READ, &buffer_tail);

  if (buffer_head != buffer_tail) {
    bx_access_physical(0x400 + buffer_head,   1, BX_READ, ascii_code);
    bx_access_physical(0x400 + buffer_head+1, 1, BX_READ, scan_code);
    if (incr) {
      buffer_head += 2;
      if (buffer_head >= buffer_end)
        buffer_head = buffer_start;
      bx_access_physical(0x41A, 2, BX_WRITE, &buffer_head);
      }
    return(1);
    }
  else {
    return(0);
    }
}
#endif

  void
bx_keybd_gen_scancode(Bit8u scancode)
{

  if (bx_dbg.keyboard)
    bx_printf("KBD: keybd_gen_scancode(): scancode: %02x\n", (unsigned) scancode);

  /* should deal with conversions from KSCAN to system scan codes here */

  kbd_internal_buffer.enQ(scancode);
}



  void
bx_set_kbd_clock_enable(Bit8u value)
{
  Boolean prev_kbd_clock_enabled;

  if (value==0) {
    kbd_controller.kbd_clock_enabled = 0;
    }
  else {
    /* is another byte waiting to be sent from the keyboard ? */
    prev_kbd_clock_enabled = kbd_controller.kbd_clock_enabled;
    kbd_controller.kbd_clock_enabled = 1;

    if (prev_kbd_clock_enabled==0 && kbd_controller.outb==0) {
      kbd_controller.activate_timer();
      }
    }
}



  void
bx_set_aux_clock_enable(Bit8u value)
{
  Boolean prev_aux_clock_enabled;

bx_printf("set_aux_clock_enable(%u)\n", (unsigned) value);
  if (value==0) {
    kbd_controller.aux_clock_enabled = 0;
    }
  else {
    /* is another byte waiting to be sent from the keyboard ? */
    prev_aux_clock_enabled = kbd_controller.aux_clock_enabled;
    kbd_controller.aux_clock_enabled = 1;
    if (prev_aux_clock_enabled==0 && kbd_controller.outb==0)
      kbd_controller.activate_timer();
    }
}

  /* enable/disable the mouse device as if following commands sent
   * port 64h <-- D4
   * port 60h <-- F4 (enable) or F5 (disable)
   */
  void
bx_set_mouse_enable(Bit8u value)
{

  /* enable/disable aux clock line & clear internal buffer */
  mouse_internal_buffer.head = 0;
  mouse_internal_buffer.num_elements = 0;
  bx_mouse_device_enabled = value;

  bx_set_aux_clock_enable(value);
}


  Bit8u
bx_get_kbd_enable(void)
{
#ifdef BX_DEBUG
  if (bx_dbg.keyboard)
    bx_printf("KBD: get_kbd_enable(): getting kbd_clock_enabled of: %02x\n",
      (unsigned) kbd_controller.kbd_clock_enabled);
#endif

  return(kbd_controller.kbd_clock_enabled);
}

  static void
controller_enQ(Bit8u data)
{
#if 0
  if (bx_dbg.keyboard)
#endif
    bx_printf("controller_enQ(%02x)\n", (unsigned) data);

  if (kbd_controller.outb)
    bx_panic("KBD: controller_enQ(): OUTB set\n");

  kbd_controller.output_buffer = data;
  kbd_controller.outb = 1;
  kbd_controller.auxb = 0;
  kbd_controller.inpb = 0;
  if (kbd_controller.allow_irq1)
    bx_trigger_irq(1);
}

  static void
kbd_enQ(Bit8u scancode)
{
  int tail;

  if (bx_dbg.keyboard)
    bx_printf("kbd_enQ(%02x)\n", (unsigned) scancode);

  if (kbd_internal_buffer.num_elements >= BX_KBD_ELEMENTS) {
    bx_printf("KBD: internal keyboard buffer full, ignoring scancode.(%02x)\n",
      (unsigned) scancode);
    return;
    }

  /* enqueue scancode in multibyte internal keyboard buffer */
  if (bx_dbg.keyboard)
    bx_printf("KBD: kbd_enQ: putting scancode %02x in internal buffer\n",
      (unsigned) scancode);
  tail = (kbd_internal_buffer.head + kbd_internal_buffer.num_elements) %
   BX_KBD_ELEMENTS;
  kbd_internal_buffer.buffer[tail] = scancode;
  kbd_internal_buffer.num_elements++;

  if (!kbd_controller.outb && kbd_controller.kbd_clock_enabled) {
    kbd_controller.activate_timer();
    return;
    }
}

  Boolean
bx_mouse_enQ_packet(Bit8u b1, Bit8u b2, Bit8u b3)
{
  if ((mouse_internal_buffer.num_elements + 3) >= BX_MOUSE_BUFF_SIZE)
    return(0); /* buffer doesn't have the space */
  bx_mouse_enQ(b1);
  bx_mouse_enQ(b2);
  bx_mouse_enQ(b3);
  return(1);
}

  void
bx_mouse_enQ(Bit8u mouse_data)
{
  int tail;

bx_panic("mouse_enQ(): need to recode as per kbd_enQ()\n");
  if (bx_dbg.keyboard)
    bx_printf("kbd_enQ(%02x)\n", (unsigned) mouse_data);

  if (mouse_internal_buffer.num_elements >= BX_MOUSE_BUFF_SIZE) {
    bx_printf("mouse: internal mouse buffer full, ignoring mouse data.(%02x)\n",
      (unsigned) mouse_data);
#if 0
    /*if (kbd_controller.allow_irq12)*/
      bx_trigger_irq(12);
#endif
    return;
    }

  if (!kbd_controller.outb && kbd_controller.aux_clock_enabled) {
    if (mouse_internal_buffer.num_elements)
      bx_panic("error in kbd_enQ()\n");
    if (bx_dbg.keyboard)
      bx_printf("mouse: mouse_enQ: writing mouse data %02x directly to output_buffer\n",
        (unsigned) mouse_data);
    kbd_controller.output_buffer = mouse_data;
    kbd_controller.outb = 1;
    kbd_controller.auxb = 1;
    if (kbd_controller.allow_irq12)
      bx_trigger_irq(12);
    return;
    }

  /* enqueue mouse data in multibyte internal mouse buffer */
  if (bx_dbg.keyboard)
    bx_printf("mouse: mouse_enQ: putting mouse data %02x in internal buffer\n",
      (unsigned) mouse_data);
  tail = (mouse_internal_buffer.head + mouse_internal_buffer.num_elements) %
   BX_MOUSE_BUFF_SIZE;
  mouse_internal_buffer.buffer[tail] = mouse_data;
  mouse_internal_buffer.num_elements++;
}



  static void
kbd_ctrl_to_kbd(Bit8u value)
{
  static Boolean expecting_typematic = 0, expecting_led_write = 0;
  static Bit8u delay = 1; /* 500 mS */
  static Bit8u repeat_rate = 0x0b; /* 10.9 chars/second */
  static led_status = 0;

  bx_printf("KBD: controller passed byte %02xh to keyboard\n");

  if (expecting_typematic) {
    expecting_typematic = 0;
    delay = (value >> 5) & 0x03;
    switch (delay) {
      case 0: bx_printf("KBD: setting delay to 250 mS\n"); break;
      case 1: bx_printf("KBD: setting delay to 250 mS\n"); break;
      case 2: bx_printf("KBD: setting delay to 250 mS\n"); break;
      case 3: bx_printf("KBD: setting delay to 250 mS\n"); break;
      }
    repeat_rate = value & 0x1f;
    bx_printf("KBD: setting repeat rate to %u\n", (unsigned) value);
    kbd_internal_buffer.enQ(0xFA); /* send ACK ??? */
    return;
    }

  if (expecting_led_write) {
    expecting_led_write = 0;
    led_status = value;
    bx_printf("KBD: LED status set to %02x\n", (unsigned) led_status);
#if 0
    kbd_internal_buffer.enQ(0xFA); /* send ACK */
/* ??? */
#endif
    return;
    }

  switch (value) {
    case 0x05:
      kbd_internal_buffer.enQ(0xFA); /* send ACK ??? */
      return;
      break;

    case 0xed:
      expecting_led_write = 1;
#if 0
      kbd_internal_buffer.enQ(0xFA); /* send ACK */
/* ??? */
#endif
      return;
      break;

    case 0xf2:  /* identify keyboard */
      bx_printf("KBD: indentify keyboard command received\n");
      kbd_internal_buffer.enQ(0xFA); /* send ACK ??? */
      return;
      break;

    case 0xf3:  /* typematic info */
      expecting_typematic = 1;
      bx_printf("KBD: setting typematic info\n");
      kbd_internal_buffer.enQ(0xFA); /* send ACK ??? */
      return;
      break;

    default:
      bx_panic("KBD: kbd_ctrl_to_kbd(): got value of %02x\n",
        (unsigned) value);
      kbd_internal_buffer.enQ(0xFA); /* send ACK ??? */
      return;
      break;
    }
}


  static void
keyboard_timer_handler(void)
{
  kbd_controller.timer_pending = 0;

  if (kbd_controller.outb) {
    return;
    }

  /* nothing in outb, look for possible data xfer from keyboard or mouse */
  if (kbd_controller.kbd_clock_enabled && kbd_internal_buffer.num_elements) {
    if (bx_dbg.keyboard)
      bx_printf("KBD: service_keyboard: key in internal buffer waiting\n");
    kbd_controller.output_buffer =
      kbd_internal_buffer.buffer[kbd_internal_buffer.head];
    kbd_controller.outb = 1;
    kbd_controller.auxb = 0;
    kbd_internal_buffer.head = (kbd_internal_buffer.head + 1) %
      BX_KBD_ELEMENTS;
    kbd_internal_buffer.num_elements--;
    if (kbd_controller.allow_irq1)
      bx_trigger_irq(1);
    }
  else if (kbd_controller.aux_clock_enabled && mouse_internal_buffer.num_elements) {
    if (bx_dbg.keyboard)
      bx_printf("KBD: service_keyboard: key in internal buffer waiting\n");
    kbd_controller.output_buffer =
      mouse_internal_buffer.buffer[mouse_internal_buffer.head];

    kbd_controller.outb = 1;
    kbd_controller.auxb = 1;
    mouse_internal_buffer.head = (mouse_internal_buffer.head + 1) %
      BX_MOUSE_BUFF_SIZE;
    mouse_internal_buffer.num_elements--;
    if (kbd_controller.allow_irq12)
      bx_trigger_irq(12);
    }
  else {
    if (bx_dbg.keyboard) {
      bx_printf("KBD: service_keyboard(): no keys waiting\n");
      }
    }
}


#if BX_CPU < 2
  /* AT behaviour: AT keyboard not affected by port 61h write */
  void
bx_kbd_port61h_write(Bit8u value)
{
  /* XT behaviour ???
   *   bit 7: w=0: disable system switches to port 60h,
   *               enable keyboard data, allow keyboard IRQ
   *          w=1: enable system switches to port 60h,
   *               disable keyboard data, clear keyboard IRQ
   */
  if (value >> 7) {
    /* enable system switches to port 60h, disable keyboard data,
     * clear keyboard IRQ */
    kbd_controller.allow_irq1 = 0;
    bx_set_kbd_clock_enable(0);
    }
  else {
    /* disable system switches to port 60h, enable keyboard data,
     * allow keyboard IRQ */
    kbd_controller.allow_irq1 = 1;
    bx_set_kbd_clock_enable(1);
    }
  return;
}
#endif


  static void
ctrl_activate_timer(void)
{
  if (kbd_controller.timer_pending == 0) {
    kbd_controller.timer_pending = 1;
    bx_activate_timer(keyboard_timer_handle,
                      /* activate */ 1, /* use previous given */ 0);
    }
}
