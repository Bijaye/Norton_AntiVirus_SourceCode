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


#define IO_HGA_C 1

#ifdef WIN32
   #include <time.h>
#else
   #include <unistd.h>
   #include <sys/time.h>
#endif
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>



#include "iodev.h"


#define HGA_TEXT_ADDR(row, column) \
  (((hga.start_addr + (row)*80 + (column))*2) % 4096)

#define HGA_CURSOR_X() \
  ((hga.cursor_addr - hga.start_addr) % 80)

#define HGA_CURSOR_Y() \
  ((hga.cursor_addr - hga.start_addr) / 80)

void hga_update(void);

#if BX_EXTERNAL_ENVIRONMENT==0
static void scroll_screen_up(Bit8u rows, Bit8u blank_attr, Bit8u top_row,
  Bit8u left_col, Bit8u bottom_row, Bit8u right_col);
static void scroll_screen_down(Bit8u rows, Bit8u blank_attr, Bit8u top_row,
  Bit8u left_col, Bit8u bottom_row, Bit8u right_col);
static void hga_outstring(Bit8u *string, Bit8u* attr, unsigned len,
  unsigned row, unsigned column);
static void hga_clear_screen(void);
static void set_bios_cursor_pos(Bit8u column, Bit8u row, Bit8u page);
#endif

extern bx_options_t bx_options;

static void hga_refresh_screen(void);
static void take_hga_text_snapshot(Bit8u *ptr);
static void cursor_on(void);
static void cursor_off(void);

static Bit8u text_snapshot[2000 * 2];
Bit8u graphics_snapshot[32 * 1024];


#if (BX_EXTERNAL_ENVIRONMENT==1) || (BX_EMULATE_HGA_DUMPS>0)
Bit8u bx_hga_memory[64 * 1024];
#else
#ifndef USEWIN32VIRTUALMEM
Bit8u *bx_hga_memory = &bx_phy_memory[0xb0000];
#else
Bit8u *bx_hga_memory;
#endif
#endif


Bit8u bx_hga_graphics_mode = 0;
Bit8u bx_hga_video_enabled = 0;
Bit8u bx_hga_page_displayed = 0;

Bit8u data_register = 0,
             allow_graphics = 1,
             enable_upper32k = 1;



static struct {
  Bit32u  text_base_addr;  /* text video memory base address */
  Bit16u  start_addr;      /* offset from base address (in words) */
  Bit16u  cursor_addr;     /* cursor offset relative to start_addr (in words) */
  Boolean initialized;     /* HGA code init'd? */
  } hga = { 0xB0000, 0x0000, 0x0000, 0 };


struct {
  void (*init)(int argc, char *argv[]);
  void (*beep)();
  void (*outstring)(Bit8u *string, Bit8u* attr, unsigned len, unsigned row,
           unsigned col);
  void (*clear_screen)(unsigned col1, unsigned row1,
                       unsigned col2, unsigned row2,
                       Bit8u blank_attr);
  void (*scroll_screen)(unsigned col1, unsigned row1,
                       unsigned col2, unsigned row2,
                       unsigned rows);
  void (*draw_point)(unsigned x, unsigned y, Bit8u set);
  void (*switch_mode)(unsigned mode);
  void (*update_screen)(void);
  void (*flush)(void);
  } bx_video_config = 
#ifdef WIN32
    { bx_W_init, bx_W_beep, bx_W_outstring, bx_W_clear_screen,
      bx_W_scroll_screen, bx_W_draw_point, bx_W_switch_mode,
      bx_W_update, bx_W_flush };
#else
    { bx_X_init, bx_X_beep, bx_X_outstring, bx_X_clear_screen,
      bx_X_scroll_screen, bx_X_draw_point, bx_X_switch_mode,
      bx_X_update, bx_X_flush };
#endif


/* ??? examine active page # in video bios calls */
/* 0x450 & 0xB0000 */


  void
bx_init_hga_hardware(void)
{
  bx_iodev_t  io_device;
  unsigned i;

  io_device.read_funct   = bx_hga_io_read_handler;
  io_device.write_funct  = bx_hga_io_write_handler;
  io_device.handler_name = "hga video";
  io_device.start_addr   = 0x03B4;
  io_device.end_addr     = 0x03B5;
  io_device.irq          = BX_NO_IRQ;
  bx_register_io_handler(io_device);

  io_device.start_addr   = 0x03B8;
  io_device.end_addr     = 0x03BA;
  bx_register_io_handler(io_device);

  io_device.start_addr   = 0x03BF;
  io_device.end_addr     = 0x03BF;
  bx_register_io_handler(io_device);

  hga.initialized = 1;

  
  /* Write blank with normal attribute to all possible video
   * memory locations.  A page is only 2000 words, but the
   * offset can be changed to pan the page around in memory.
   * I believe only 4K is used, and text wraps back around
   * to the beginning of the text video memory.
   */
  for (i=0; i<2048; i++) {
    bx_hga_memory[i*2 + 0] = 0x20;
    bx_hga_memory[i*2 + 1] = 0x07;
    }

  /* get snapshot of current screen contents */
  take_hga_text_snapshot(text_snapshot);

  {
  /* ??? should redo this to pass X args */
  char *argv[1] = { "bochs" };
  bx_video_config.init(1, &argv[0]);
  }

  bx_printf("hga: interval=%u\n", bx_options.hga_update_interval);
  bx_register_timer(bx_hga_timer_handler, bx_options.hga_update_interval, 1, 1);

  bx_cmos_reg[0x14] |= 0x30; /* 80 columns x 25 row, monochrome */
}


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_hga_bios(void)
{
  Bit8u  zero8, video_mode, internal_mode_reg, cga_color_reg;
  Bit16u zero16, equip_word, video_bytes_per_page;
  Bit16u video_columns, video_cursor_shape, video_port_no;
  Bit16u char_attr;
  Bit32u i;
  Bit8u bogus8;


  bx_isr_code_iret[2] = 0x10;
  bx_register_int_vector(0x10, bx_isr_code_iret, sizeof(bx_isr_code_iret),
      bx_hga_int10h_handler
#ifdef WIN32DEBUG
      ,H_HGAINT10
#endif       
      );

  zero8 = 0;
  zero16 = 0;

  /* video mode */
  video_mode = 7; /* 80x25 mono hga mode */
  bx_access_physical(0x449, 1, BX_WRITE, &video_mode);

  /* video number of columns */
  video_columns = 80;
  bx_access_physical(0x44A, 2, BX_WRITE, &video_columns);

  /* video total number of bytes per page */
  video_bytes_per_page = 4000; /* 2 * 80 * 25 */
  bx_access_physical(0x44C, 2, BX_WRITE, &video_bytes_per_page);

  /* video current page offset */
  bx_access_physical(0x44E, 2, BX_WRITE, &zero16);

  /* video cursor position pages 0 to 7 */
  for (i=0; i<8; i++)
    bx_access_physical(0x450 + i*2, 2, BX_WRITE, &zero16);

  /* video cursor shape */
  video_cursor_shape = 0x0607; /* single underline */
  bx_access_physical(0x460, 2, BX_WRITE, &video_cursor_shape);

  /* video active display page */
  bx_access_physical(0x462, 1, BX_WRITE, &zero8);

  /* video I/O port number base */
  video_port_no = 0x3B4;
  bx_access_physical(0x463, 2, BX_WRITE, &video_port_no);

  /* video internal mode register */
  internal_mode_reg = 0x09;
  bx_access_physical(0x465, 1, BX_WRITE, &internal_mode_reg);

  cga_color_reg = 0x00;
  /* video color palatte */
  bx_access_physical(0x466, 1, BX_WRITE, &cga_color_reg);

  /* now, set the initial video mode type field in the equipment word */
  bx_access_physical(0x410, 2, BX_READ, &equip_word);
  equip_word |= (3 << 4); /* mono 80cols x 25rows */
  bx_access_physical(0x410, 2, BX_WRITE, &equip_word);

  /* video initialization data pointer (uses INT 1Dh location).  it
     should point to the 16-byte video initialization array to be
     sent to ports 3b4/3b5 ?*/
  /* actually, I think this is wrong.  There are several arrays
     and variables in this Video Initialization Parameter Table.  The
     one use depends upon the current mode.  Oh well fix this later. */
  bx_set_interrupt_vector(0x1d, BX_VIDEO_INIT_ARRAY);
  bogus8 = 0x07;
  for (i=0; i<16; i++) {
    bx_access_physical(BX_VIDEO_INIT_ARRAY+i, 1, BX_WRITE, &bogus8);
    }

  char_attr = 0x0720;
  for (i=0; i<2048; i++) {
    bx_access_physical(0xb0000 + i*2, 2, BX_WRITE, &char_attr);
    }
}
#endif



  Bit32u
bx_hga_io_read_handler(Bit32u address, unsigned io_len)
{
  Bit8u status;
  static unsigned video_signal=0,
                  horizontal_count=0;
  static unsigned horizontal_sync, vertical_sync;

  if (bx_dbg.video)
    bx_printf("hga_io_read(%04x)!\n", (unsigned) address);

  switch (address) {
    case 0x03ba:
      video_signal = !video_signal;
      horizontal_sync = 0;
      vertical_sync   = 0;

      if (video_signal) {
        horizontal_count += 5;
        horizontal_sync = 1;
        if (horizontal_count >= 720) {
          horizontal_count = 0;
          vertical_sync = 1;
          }
        }
      status = (vertical_sync<<7) | (video_signal<<3) | (horizontal_sync);
#if 0
#if BX_DEBUG
      bx_printf("*** io read from hga port %x, ret=%02x\n",
        (int) address, (unsigned) status);
#endif
#endif

      return(status);
    break;

  default:
    bx_panic("*** io read from hga port %x\n",
      (int) address);
    return(0); /* keep compiler happy */
  }

}

  void
bx_hga_io_write_handler(Bit32u address, Bit32u value, unsigned io_len)
{
  Bit8u
#if 0
        prev_page_displayed = bx_hga_page_displayed,
        prev_video_enabled  = bx_hga_video_enabled,
#endif
        prev_graphics_mode  = bx_hga_graphics_mode;
  Bit8u old_allow_graphics;


  if (bx_dbg.video)
    bx_printf("hga_io_write(%04x)=%02x!\n", (unsigned) address,
      (unsigned) value);

  switch (address) {
    case 0x3b8:
      bx_hga_graphics_mode = (value >> 1) & 0x01;
      bx_hga_video_enabled = (value >> 3) & 0x01;
      bx_hga_page_displayed = (value >> 7) & 0x01;
      if ( bx_hga_graphics_mode && !prev_graphics_mode ) {
        if (allow_graphics) {
          bx_video_config.switch_mode(1);
hga_update();
          }
        }
      else if ( !bx_hga_graphics_mode && prev_graphics_mode ) {
        take_hga_text_snapshot(text_snapshot);
        bx_video_config.switch_mode(0);
hga_update();
        }
      break;

    case 0x03b9: /* set lightpen flip flop (ingored) ? */
      break;

    case 0x03b4: /* data register number select */
      if (value < 0x12)
        data_register = value;
      break;

    case 0x03b5:
      if (data_register <= 0x0b); /* write only registers */
      else if (data_register == 0x0c) { /* start address high */
        /* ??? upper 2 bits not used */
        hga.start_addr = (hga.start_addr & 0x00ff) | (value << 8);
        }
      else if (data_register == 0x0d) { /* start address low */
        cursor_off();
        hga.start_addr = (hga.start_addr & 0xff00) | value;
        if ( (hga.start_addr*2 + 2000*2 - 1) > 0xffff ) {
          bx_panic("hga: hga.start_addr out of HGA memory bounds\n");
          }
        /* ??? optimize this using scroll when possible */
        hga_update();
        }
      else if (data_register == 0x0e) { /* cursor address high */
        /* ??? upper 2 bits not used */
        cursor_off();
        hga.cursor_addr = (hga.cursor_addr & 0x00ff) | (value << 8);
        }
      else if (data_register == 0x0f) { /* cursor address low */
        cursor_off();
        hga.cursor_addr = (hga.cursor_addr & 0xff00) | value;
        cursor_on();
        }
      else
        bx_panic("HGA: setting data register %02x to %02x unsupported\n",
          (unsigned) data_register, (unsigned) value);
      break;

    case 0x03bf:
      old_allow_graphics = allow_graphics;
      allow_graphics = value & 0x01;
      enable_upper32k = (value >> 1) & 0x01;
bx_printf("allow_graphics=%u, enable_upper32k=%u\n",
  (unsigned) allow_graphics, (unsigned) enable_upper32k);
      if (bx_hga_graphics_mode && allow_graphics && !old_allow_graphics) {
        bx_video_config.switch_mode(1);
hga_update();
        }
      break;

    default:
      bx_panic("unsupported io write to port %x, val=%02x\n",
        (unsigned) address, (unsigned) value);
    }
}

  void
bx_hga_timer_handler(void)
{
  hga_update();
#ifdef WIN32
  if (bx_handle_W_events())
#else
  if (bx_handle_x_events())
#endif
    hga_refresh_screen();
  bx_video_config.flush();
}


#if (BX_EXTERNAL_ENVIRONMENT==1) || (BX_EMULATE_HGA_DUMPS>0)
  void
bx_hga_set_video_memory(Bit8u *ptr)
{
bx_printf("HGA dump...\n");
  /* Update HGA memory buffer with memory dump from external environment */
  memcpy(bx_hga_memory, ptr, 64*1024);

  /* So that changes may be seen right away, also call update handler
   * which usually is called on an interval basis
   */
  bx_hga_timer_handler();
}
#endif



  void
bx_video_message(char *message)
{
  if (!hga.initialized) return;
  /* ??? should code here */
  return;
}



#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_hga_int10h_handler(int vector)
{
  unsigned offset;
  Bit16u attr_char;
  Bit8u row, column, page;
  Bit8u character, attr, mode;
  Bit16u cursor_type, repeat_count;
  Bit8u active_page, video_mode;
  Bit16u num_columns;


  if (!hga.initialized) return;
  if (bx_hga_graphics_mode && allow_graphics) {
    bx_printf("int10h: called in graphics mode, ignoring!\n");
    bx_printf("        AH=%02x, AL=%02x\n", (unsigned) AH, (unsigned) AL);
    /* ??? */
    /* return; */
    }

  if (bx_dbg.video)
    bx_printf("int10h: AH=%02x, AL=%02x, BH=%02x, CX=%04x\n",
      (int) AH, (int) AL, (unsigned) BH, (unsigned) CX);
/*
bx_hga_timer_handler();
usleep(1000);
*/


  switch (AH) {
    case 0x00:  /* set video mode, clear screen */
bx_printf("int10h: f00h, set video mode called\n");
      mode = AL & 0x7f;
      if (mode != 0x07) {
        bx_printf("int10h: attempt to set video mode to %xh\n", (unsigned) mode);
        break;
        }
      bx_access_physical(0x449, 1, BX_WRITE, &mode);
      hga_clear_screen();
      cursor_on();
      break;

    case 0x01:  /* set cursor type */
#if BX_DEBUG
bx_printf("f01h\n");
#endif
    cursor_type = CX;
    bx_access_physical(0x460, 2, BX_WRITE, &cursor_type);
    break;

    case 0x02:  /* set cursor position */
#if BX_DEBUG
bx_printf("f02h\n");
#endif
    row = DH;
    column = DL;
    page = BH;
    if (page>0 || row>24 || column>79) {
      cursor_off();
      break;
      }
    set_bios_cursor_pos(column, row, page);
    break;

    case 0x03: /* read current cursor position */
#if BX_DEBUG
bx_printf("f03h\n");
#endif
    /* BH is video page, ignoring for now */
    bx_access_physical(0x450, 1, BX_READ, &column);
    bx_access_physical(0x451, 1, BX_READ, &row);
    bx_access_physical(0x460, 2, BX_READ, &cursor_type);
    DH = row;
    DL = column;
    CX = cursor_type;
    break;

    case 0x05: /* select active display page */
    page = AL;
    if (page > 0) {
      bx_printf("attempt to select non-zero page no %xh\n", (unsigned) page);
      break;
      }
    bx_access_physical(0x462, 1, BX_WRITE, &page);
    break;

    case 0x06: /* scroll current page up */
#if BX_DEBUG
bx_printf("f06h\n");
#endif
    cursor_off();
    scroll_screen_up(AL, BH, CH, CL, DH, DL);
    cursor_on();
    break;

    case 0x07: /* scroll current page down */
#if BX_DEBUG
bx_printf("f07h\n");
#endif
    cursor_off();
    scroll_screen_down(AL, BH, CH, CL, DH, DL);
    cursor_on();
    break;

    case 0x08: /* read character/attribute from screen */
#if BX_DEBUG
bx_printf("f08h\n");
#endif
    bx_access_physical(0x450, 1, BX_READ, &column);
    bx_access_physical(0x451, 1, BX_READ, &row);
    /* BH = page number, ignoring for now */
    character = bx_hga_memory[HGA_TEXT_ADDR(row, column)];
    attr      = bx_hga_memory[1+HGA_TEXT_ADDR(row, column)];

    AL = character;
    AH = attr;
    break;

    case 0x09: /* write character/attribute to screen */
#if BX_DEBUG
bx_printf("f09h\n");
#endif
    bx_access_physical(0x450, 1, BX_READ, &column);
    bx_access_physical(0x451, 1, BX_READ, &row);
    repeat_count = CX;
#if BX_DEBUG
bx_printf("repeat count = %d\n", (int) repeat_count);
#endif
    /* sanity check so we don't go outside video memory */
    if ( row>=25 || column>=80 )
      bx_panic("int10h: row,column out of bounds\n");
    if ( (row*80+column+repeat_count-1) >= (80*25) )
      repeat_count = (80*25) - (row*80+column);
    /* BH = page no */
    attr = BL;
    character = AL;
    if (character == 0) character = 0x20; /* convert null to space */
    attr_char = (attr << 8) + character;
    cursor_off();
    for (; repeat_count; repeat_count--) {
      bx_hga_memory[HGA_TEXT_ADDR(row,column)]   = character;
      bx_hga_memory[1+HGA_TEXT_ADDR(row,column)] = attr;
      hga_outstring(&character, &attr, 1, row, column);
      column++;
      if (column >= 80) {
        column = 0;
        row++;
        }
      }
#if 0
    if (CX>1) { /* only update cursor for repeat counts */
      /*bx_access_physical(0x450, 1, BX_WRITE, &column);*/
      /*bx_access_physical(0x451, 1, BX_WRITE, &row);*/
      set_bios_cursor_pos(column, row, page); /* ??? examine active page */
      }
#endif
    cursor_on();
    break;


    case 0x0e: /* teletype output */
    cursor_off();
    /* ??? should examine active page */
    bx_access_physical(0x450, 1, BX_READ, &column);
    bx_access_physical(0x451, 1, BX_READ, &row);
    
    switch (AL) {
      case 0x07: /* bell */
        break;
      case 0x08: /* \b = backspace */
        if (column > 0) column--;
        break;
      case 0x0a: /* \n = linefeed */
        row++;
        if (row >= 25) {
          scroll_screen_up(1, 0x07, 0, 0, 24, 79);
          row = 24;
          }
        break;
      case 0x0d: /* \r = return */
        column = 0;
        break;
      case 0x09: /* \t = tab */
bx_panic("TAB encountered\n");
        break;
      default:
        offset = (row*80 + column)*2;
        character = AL;
        bx_hga_memory[HGA_TEXT_ADDR(row, column)] = character;
        attr = bx_hga_memory[1 + HGA_TEXT_ADDR(row, column)];

        /* ??? do I need outstring here */
        hga_outstring(&character, &attr, 1, row, column);
        column++;
        if (column >= 80) {
          column = 0;
          row++;
          if (row >= 25) {
            scroll_screen_up(1, 0x07, 0, 0, 24, 79);
            row = 24;
            }
          }
        break;
      }
#if 0
    bx_access_physical(0x450, 1, BX_WRITE, &column);
    bx_access_physical(0x451, 1, BX_WRITE, &row);
#endif
    set_bios_cursor_pos(column, row, 0); /*??? should use active page */
    cursor_on();
    break;



    case 0x0f: /* return video status */
#if BX_DEBUG
bx_printf("f0fh\n");
#endif
    bx_printf("*** hga_int10h handler called, partially supported function, AH=%xh\n",
      (int) AH);
    bx_access_physical(0x462, 1, BX_READ, &active_page);
    bx_access_physical(0x449, 1, BX_READ, &video_mode);
    bx_access_physical(0x44A, 2, BX_READ, &num_columns);
    AH = num_columns & 0xff;
    AL = video_mode;
    BH = active_page;
    break;

    case 0x0b: /* set color palette */
#if BX_DEBUG
bx_printf("f0bh\n");
#endif
    bx_printf("*** hga_int10h handler called, unsupported function, AH=%xh\n",
      (int) AH);
    break;

    case 0x11: /* load character generator */
#if BX_DEBUG
bx_printf("f11h\n");
#endif
    bx_printf("*** hga_int10h unsupported function AH=%xh, subfunct AL=%xh\n",
      (int) AH, (int) AL);
    bx_printf("                                    BH=%xh, BL=%xh\n",
      (int) BH, (int) BL);
    break;

    case 0x12: /* alternate select */
#if BX_DEBUG
bx_printf("f12h\n");
#endif
    bx_printf("*** hga_int10h unsupported function, AH=%xh, subfunct AL=%xh\n",
      (int) AH, (int) AL);
    bx_printf("                                    BH=%xh, BL=%xh\n",
      (int) BH, (int) BL);
    break;

    case 0xfe: /* get relocated screen address */
    bx_printf("*** hga_int10h handler called, unsupported function, AH=%xh\n",
      (int) AH);
    bx_printf("  ES:DI = %04x:%04x\n",
      (unsigned) bx_cpu.es.selector.value, (unsigned) DI);
    break;

    default: /* any other int10h service */
    bx_printf("*** hga_int10h handler called, unsupported function, AH=%xh\n",
      (int) AH);
    break;
    }
}


  static void
scroll_screen_up(Bit8u rows, Bit8u blank_attr, Bit8u top_row, Bit8u left_col,
  Bit8u bottom_row, Bit8u right_col)
{
  unsigned reps, row, column;
  /* hack that access memory directly.  should use a call to
     bx_memory.c */

  if (rows == 0  &&  right_col>79) right_col = 79;
  if (rows == 0  &&  bottom_row>24) bottom_row = 24;

  if (left_col > right_col) {
    bx_printf("scroll_screen_up: left_col > right_col limit check invoked\n");
    return;
    }
  if (top_row > bottom_row) {
    bx_printf("scroll_screen_up: top_row >= bottom_row limit check invoked\n");
#if BX_DEBUG
bx_printf("bottom_row = %d\n", (int) bottom_row);
bx_printf("right_col  = %d\n", (int) right_col);
bx_printf("top_row    = %d\n", (int) top_row);
bx_printf("left_col   = %d\n", (int) left_col);
bx_printf("rows       = %d\n", (int) rows);
#endif
    return;
    }
  if (bottom_row >= 25  ||  right_col >=80) {
    bx_printf("scroll_screen_up: bottom>=25 || right>=80 limit check invoked\n");
#if BX_DEBUG
bx_printf("bottom_row = %d\n", (int) bottom_row);
bx_printf("right_col  = %d\n", (int) right_col);
bx_printf("top_row    = %d\n", (int) top_row);
bx_printf("left_col   = %d\n", (int) left_col);
bx_printf("rows       = %d\n", (int) rows);
#endif
    return;
    }

  if (!rows || (rows >= (1 + bottom_row - top_row)) ) {
    /* blank out entire area */
    for (row = top_row; row <= bottom_row; row++) {
      for (column = left_col; column <= right_col; column++) {
        bx_hga_memory[HGA_TEXT_ADDR(row, column)] = 0x20;
        text_snapshot[(row*80 + column)*2] = 0x20;
        bx_hga_memory[1 + HGA_TEXT_ADDR(row, column)] = blank_attr;
        text_snapshot[1 + (row*80 + column)*2] = blank_attr;
        }
      }
    bx_video_config.clear_screen(left_col, top_row, right_col, bottom_row,
      blank_attr);
    return;
    }

  /* 1st move block by 'rows' lines */
  reps = (1 + bottom_row - top_row) - rows;
  for (row = top_row; row < top_row+reps; row++) {
    for (column = left_col; column <= right_col; column++) {
      /* 1st the character */
      bx_hga_memory[HGA_TEXT_ADDR(row, column)] =
        bx_hga_memory[HGA_TEXT_ADDR(row+rows, column)];
      text_snapshot[row*80*2 + column*2] =
        text_snapshot[(row+rows)*80*2 + column*2];

      /* then the attribute */
      bx_hga_memory[1 + HGA_TEXT_ADDR(row, column)] =
        bx_hga_memory[1 + HGA_TEXT_ADDR(row+rows, column)];
      text_snapshot[1 + row*80*2 + column*2] =
        text_snapshot[1 + (row+rows)*80*2 + column*2];
      }
    }
  bx_video_config.scroll_screen(left_col, top_row,
                                right_col, bottom_row, rows);

  /* now blank out bottom lines */
  for (row = bottom_row - (rows-1); row <= bottom_row; row++) {
    for (column = left_col; column <= right_col; column++) {
      bx_hga_memory[HGA_TEXT_ADDR(row, column)] = 0x20;
      text_snapshot[row*80*2 + column*2] = 0x20;
      bx_hga_memory[1 + HGA_TEXT_ADDR(row, column)] = blank_attr;
      text_snapshot[1 + row*80*2 + column*2] = blank_attr;
      }
    }
  bx_video_config.clear_screen(left_col, bottom_row - (rows-1),
                               right_col, bottom_row, blank_attr);
}


  static void
scroll_screen_down(Bit8u rows, Bit8u blank_attr, Bit8u top_row,
  Bit8u left_col, Bit8u bottom_row, Bit8u right_col)
{
  unsigned row, column;
  Bit8u character, attr;

  /* rows: 0=blank area by scrolling all rows down
   *       n=scroll down n lines
   */
  if (right_col>79 || left_col>right_col)
    bx_panic("hga: scroll_screen_down: left/right columns out of bounds\n");
  if (bottom_row>24 || top_row>bottom_row)
    bx_panic("hga: scroll_screen_down: top/bottom rows out of bounds\n");
  if ( rows==0 || rows>=(1 + bottom_row - top_row) ) {
    /* blank out entire area */
    for (row = top_row; row <= bottom_row; row++) {
      for (column = left_col; column <= right_col; column++) {
        bx_hga_memory[HGA_TEXT_ADDR(row, column)] = 0x20;
        bx_hga_memory[1+HGA_TEXT_ADDR(row, column)] = blank_attr;
        }
      }
    return;
    }

  /* 1st move block down by 'rows' lines */
  for (row = bottom_row; row >= (top_row+rows); row--) {
    for (column = left_col; column <= right_col; column++) {
      character = bx_hga_memory[HGA_TEXT_ADDR(row-rows, column)];
      attr      = bx_hga_memory[1+HGA_TEXT_ADDR(row-rows, column)];
      bx_hga_memory[HGA_TEXT_ADDR(row, column)] = character;
      bx_hga_memory[1+HGA_TEXT_ADDR(row, column)] = attr;
      }
    }

  /* now blank out top lines */
  for (row = top_row; row < (top_row+rows); row++) {
    for (column = left_col; column <= right_col; column++) {
      bx_hga_memory[HGA_TEXT_ADDR(row, column)] = 0x20;
      bx_hga_memory[1+HGA_TEXT_ADDR(row, column)] = blank_attr;
      }
    }
}

  static void
hga_outstring(Bit8u *string, Bit8u* attr, unsigned len,
  unsigned row, unsigned col)
{
  unsigned i;

  /* update current snapshot of what's on actual video screen */
  for (i=0; i<len; i++) {
    text_snapshot[(row*80 + col + i)*2] = string[i];
    text_snapshot[(row*80 + col + i)*2 + 1] = attr[i];
    }
  bx_video_config.outstring(string, attr, len, row, col);
}
#endif /* BX_EXTERNAL_ENVIRONMENT==0 */

  void
hga_update(void)
{
  static Bit8u string[80];
  static Bit8u attrib[80];
  unsigned changed=0;
  unsigned row, col;
  static Bit8u current_text_snapshot[4000];


  if (allow_graphics && bx_hga_graphics_mode) {
    bx_video_config.update_screen();
    return;
    }

  take_hga_text_snapshot(current_text_snapshot);

  /* text mode */
  for (row=0; row<25; row++) {
    if ( memcmp(&current_text_snapshot[row*2*80], &text_snapshot[row*2*80],
         2*80) ) {
      changed = 1;
      memcpy(&text_snapshot[row*2*80], &current_text_snapshot[row*2*80], 2*80);
      for (col=0; col<80; col++) {
        string[col] = text_snapshot[(row*80+col)*2];
        attrib[col] = text_snapshot[1 + (row*80+col)*2];
        }
      bx_video_config.outstring(string, attrib, 80, row, 0);
      }
    }
  if (changed) {
    cursor_on();
    }

#if BX_DEBUG
  if (changed)
    bx_printf("hga_update: snapshot needs updating.\n");
#endif
}

#if BX_EXTERNAL_ENVIRONMENT==0
  static void
hga_clear_screen(void)
{
  unsigned row, col;

  if (bx_hga_graphics_mode) {
    memset(&bx_hga_memory[HGA_OFFSET], 0, 0x8000);
    return;
    }


  /* update actual video memory & snapshot memory */
  for (row=0; row<25; row++) {
    for (col=0; col<80; col++) {
      bx_hga_memory[HGA_TEXT_ADDR(row,col)] =
        text_snapshot[(row*80+col)*2] = 0x20; /* space */
      bx_hga_memory[1 + HGA_TEXT_ADDR(row,col)] = 
        text_snapshot[1 + (row*80+col)*2] = 0x07; /* normal attribs */
      }
    }

  bx_video_config.clear_screen(0,0, 79,24, 0x07);
  cursor_on();
}
#endif


  static void
hga_refresh_screen(void)
{
  static Bit8u string[80];
  static Bit8u attrib[80];
  unsigned row, col;


  if (allow_graphics && bx_hga_graphics_mode) {
    if (bx_dbg.video)
      bx_printf("refresh_screen()\n");

    /* clear screen, make snapshot cache reflect clear screen, then
     * call update.  I'm going to cheat & not call clear_screen(),
     * but for non X-windows, probably should uncomment this.
     */
    /*bx_video_config.clear_screen(0,0, 79,24, 0x07);*/
    memset(graphics_snapshot, 0, 31320);
                             /* 31320 = 348lines * 90bytes/line */
    bx_video_config.update_screen();
    return;
    }

  cursor_off();
  for (row=0; row<25; row++) {
    for (col=0; col<80; col++) {
      string[col] = bx_hga_memory[HGA_TEXT_ADDR(row,col)];
      attrib[col] = bx_hga_memory[1 + HGA_TEXT_ADDR(row,col)];
      }
    bx_video_config.outstring(string, attrib, 80, row, 0);
    }
  cursor_on();
}


#if BX_EXTERNAL_ENVIRONMENT==0
  /* set row & column of given page in BIOS area */
  static void
set_bios_cursor_pos(Bit8u column, Bit8u row, Bit8u page)
{
  if (page > 0)
    bx_panic("set_bios_cursor_pos(): page > 0\n");
  if (column>79 || row>24) {
    bx_panic("set_bios_cursor_pos(): column>79 || row>24\n");
    return;
    }


  /* set current column & row for given page */
  bx_access_physical(0x450 + page*2, 1, BX_WRITE, &column);
  bx_access_physical(0x451 + page*2, 1, BX_WRITE, &row);


  /* ??? should change cursor only if 'page' is current page */
  cursor_off();
  hga.cursor_addr = hga.start_addr + (row * 80) + column;
  cursor_on();
}
#endif


  /* turn text cursor off */
  static void
cursor_off(void)
{
  Bit8u attr, character;

  if (allow_graphics && bx_hga_graphics_mode) {
    return;
    }

  character = bx_hga_memory[HGA_TEXT_ADDR(HGA_CURSOR_Y(), HGA_CURSOR_X())];
  attr      = bx_hga_memory[1+HGA_TEXT_ADDR(HGA_CURSOR_Y(), HGA_CURSOR_X())];

  /* redraw it to erase cursor */
  bx_video_config.outstring(&character, &attr, 1,
    HGA_CURSOR_Y(), HGA_CURSOR_X());
}

  /* turn text cursor on */
  static void
cursor_on(void)
{
  Bit8u character, attr;

  if (allow_graphics && bx_hga_graphics_mode) {
    return;
    }

  character = bx_hga_memory[HGA_TEXT_ADDR(HGA_CURSOR_Y(), HGA_CURSOR_X())];
  attr      = bx_hga_memory[1+HGA_TEXT_ADDR(HGA_CURSOR_Y(), HGA_CURSOR_X())];

  /* to show cursor, just display reverse character as is at cursor location */
  if (attr & 0x07) attr = 0;
  else attr = 0x07;
  bx_video_config.outstring(&character, &attr, 1, HGA_CURSOR_Y(), HGA_CURSOR_X());
}


  static void
take_hga_text_snapshot(Bit8u *ptr)
{
  Bit32u first_addr;
  Bit32u first_chunk_size;


  first_addr = ((hga.start_addr*2) % 4096);
  first_chunk_size = 4096 - ((hga.start_addr*2) % 4096);

  if ( first_chunk_size >= 4000 ) {
    /* viewable page doesn't wrap in video memory */
    memcpy(ptr, &bx_hga_memory[first_addr], 4000);
    }
  else {
    /* viewable page wrap's in video memory, copy to snaphot in 2 chunks */
    memcpy(&ptr[0], &bx_hga_memory[first_addr], first_chunk_size);
    memcpy(&ptr[first_chunk_size],
           &bx_hga_memory[0], 4000 - first_chunk_size);
    }
}
