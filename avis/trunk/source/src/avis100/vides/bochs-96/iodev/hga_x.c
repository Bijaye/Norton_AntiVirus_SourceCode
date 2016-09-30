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


/*
  This file was based on the "basicwin" program written by
  O'Reilly and Associates, Inc.
 */



#ifndef WIN32

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "iodev.h"
#include "bochs_icon"

#define MAX_MAPPED_STRING_LENGTH 10

/* These are used as arguments to nearly every Xlib routine, so it saves 
 * routine arguments to declare them global.  If there were 
 * additional source files, they would be declared extern there. */
static Display *display;
static int screen_num;

static char *progname; /* name this program was invoked by */

static int rows=25, columns=80;

static Window win;
static GC gc, gc_reverse, gc_pixmap;
static XFontStruct *font_info;
static unsigned int width, height;  /* window size */
static unsigned font_width, font_height;
static Bit8u blank_line[80];

static XImage *ximage = NULL;
static Pixmap pm = 0;
static unsigned ximage_width, ximage_height, ximage_width_bytes;
static unsigned x_tiles=18, y_tiles=10;


Bit8u ascii_to_scancode[0x7f] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0x39, 0x02, 0x28, 0x04, 0x05, 0x06, 0x08, 0x28, /*  !"#$%&' */
  0x0a, 0x0b, 0x09, 0x0d, 0x33, 0x0c, 0x34, 0x35, /* ()*+,-./ */
  0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, /* 01234567 */
  0x09, 0x0a, 0x27, 0x27, 0x33, 0x0d, 0x34, 0x35, /* 89:;<=>? */
  0x03, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, /* @ABCDEFG */
  0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /* HIJKLMNO */
  0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, /* PQRSTUVW */
  0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x07, 0x0c, /* XYZ[\]^_ */
  0x29, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, /* `abcdefg */
  0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /* hijklmno */
  0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, /* pqrstuvw */
  0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x29        /* xyz{|}~  */
  };

extern Bit8u graphics_snapshot[32 * 1024];

#if 1 /* bill */
extern int novideo;
#endif

static void load_font(void);
static void xkeypress(KeySym keysym, int press_release);

#define ROUNDUP(nbytes, pad) ((((nbytes) + ((pad)-1)) / (pad)) * ((pad)>>3))

#if 0
/* current cursor coordinates */
static int current_x=-1, current_y=-1;
#endif





#if 0
void main(int argc, char **argv)
#else
void bx_X_init(int argc, char **argv)
#endif
{
#if 1 /* bill bobo */
  extern int iconic;
#endif

  unsigned i;
  int x, y;   /* window position */
  unsigned int border_width = 4;  /* four pixels */
#if BX_CPU < 2
  char *window_name = "Bochs 8086 emulator, http://world.std.com/~bochs";
#elif BX_CPU == 2
  char *window_name = "Bochs 80286 emulator, http://world.std.com/~bochs";
#elif BX_CPU == 3
  char *window_name = "Bochs 80386 emulator, http://world.std.com/~bochs";
#else
  char *window_name = "Bochs 80386+ emulator, http://world.std.com/~bochs";
#endif
  char *icon_name = "Bochs";
  Pixmap icon_pixmap;
  XSizeHints size_hints;
  char *display_name = NULL;
  /* create GC for text and drawing */
  unsigned long valuemask = 0; /* ignore XGCvalues and use defaults */
  XGCValues values;

#if 1 /* bill */
  if (novideo) return;
#endif

  for (i=0; i<80; i++) {
    blank_line[i] = ' ';
    }

  progname = argv[0];

  /* connect to X server */
  if ( (display=XOpenDisplay(display_name)) == NULL )
  {
    bx_panic("%s: cannot connect to X server %s\n", 
        progname, XDisplayName(display_name));
  }

  /* get screen size from display structure macro */
  screen_num = DefaultScreen(display);

  /* Note that in a real application, x and y would default to 0
   * but would be settable from the command line or resource database.  
   */
  x = y = 0;


  load_font();
  
  font_width = font_info->max_bounds.width;
  font_height = (font_info->max_bounds.ascent +
    font_info->max_bounds.descent);
  width = columns * font_width;
  height = rows * font_height;

bx_printf("font_width = %u\n", (unsigned) font_width);
bx_printf("font_height = %u\n", (unsigned) font_height);
#if 0
width = columns * 9;
height = rows * 14;
#endif

  /* create opaque window */
  win = XCreateSimpleWindow(display, RootWindow(display,screen_num),
    x, y,
    width,
    height,
    border_width,
    BlackPixel(display, screen_num),
    BlackPixel(display, screen_num));

  /* Get available icon sizes from Window manager */

  /* Create pixmap of depth 1 (bitmap) for icon */
  icon_pixmap = XCreateBitmapFromData(display, win,
    bochs_icon_bits, bochs_icon_width, bochs_icon_height);

  /* Set size hints for window manager.  The window manager may
   * override these settings.  Note that in a real
   * application if size or position were set by the user
   * the flags would be UPosition and USize, and these would
   * override the window manager's preferences for this window. */
  /* x, y, width, and height hints are now taken from
   * the actual settings of the window when mapped. Note
   * that PPosition and PSize must be specified anyway. */

  size_hints.flags = PPosition | PSize | PMinSize | PMaxSize;
  size_hints.max_width = size_hints.min_width = width;
  size_hints.max_height = size_hints.min_height = height;

  {
  XWMHints wm_hints;
  XClassHint class_hints;

  /* format of the window name and icon name 
   * arguments has changed in R4 */
  XTextProperty windowName, iconName;

  /* These calls store window_name and icon_name into
   * XTextProperty structures and set their other 
   * fields properly. */
  if (XStringListToTextProperty(&window_name, 1, &windowName) == 0) {
    bx_panic("%s: structure allocation for windowName failed.\n", 
        progname);
  }
    
  if (XStringListToTextProperty(&icon_name, 1, &iconName) == 0) {
    bx_panic("%s: structure allocation for iconName failed.\n", 
        progname);
  }
#if 1 /* bill dodo */
  if (!iconic)
    wm_hints.initial_state = NormalState;
  else
    wm_hints.initial_state = IconicState;
#else
  wm_hints.initial_state = NormalState;
#endif
  wm_hints.input = True;
  wm_hints.icon_pixmap = icon_pixmap;
  wm_hints.flags = StateHint | IconPixmapHint | InputHint;

  class_hints.res_name = progname;
  class_hints.res_class = "Bochs";

  XSetWMProperties(display, win, &windowName, &iconName, 
      argv, argc, &size_hints, &wm_hints, 
      &class_hints);
  }

  /* Select event types wanted */
  XSelectInput(display, win, ExposureMask | KeyPressMask | KeyReleaseMask |
      ButtonPressMask | StructureNotifyMask | PointerMotionMask |
      EnterWindowMask | LeaveWindowMask);


  /* Create default Graphics Context */
  gc = XCreateGC(display, win, valuemask, &values);
  gc_reverse = XCreateGC(display, win, valuemask, &values);

  /* specify font */
  XSetFont(display, gc, font_info->fid);
  XSetFont(display, gc_reverse, font_info->fid);


  ximage_width_bytes = ((720/8)+(x_tiles-1)) / x_tiles;
  ximage_width = ximage_width_bytes * 8;
  ximage_height = (348 + (y_tiles-1)) / y_tiles;

  ximage = XCreateImage(display,NULL,1,ZPixmap,0,(char *)0,
    ximage_width, ximage_height, BitmapPad(display), 0);
  ximage->bits_per_pixel = 1;
  ximage->bytes_per_line = ROUNDUP((1 * ximage->width), ximage->bitmap_pad);
  ximage->data = (char *) malloc(ximage->bytes_per_line * ximage->height);

#if 0
bx_printf("BitmapPad() = %d\n", BitmapPad(display));
bx_printf("width = %d\n", ximage_width);
bx_printf("width_bytes = %d\n", ximage_width_bytes);
bx_printf("height = %d\n", ximage_height);
bx_printf("ximage->data malloc size = %d\n",
  ximage->bytes_per_line * ximage->height);
bx_printf("ximage.width = %d\n", ximage->width);
bx_printf("ximage.height = %d\n", ximage->height);
bx_printf("ximage.xoffset = %d\n", ximage->xoffset);
bx_printf("ximage.bitmap_unit = %d\n", ximage->bitmap_unit);
bx_printf("ximage.bitmap_pad = %d\n", ximage->bitmap_pad);
bx_printf("ximage.depth = %d\n", ximage->depth);
bx_printf("ximage.bytes_per_line = %d\n", ximage->bytes_per_line);
bx_printf("ximage.bits_per_pixel = %d\n", ximage->bits_per_pixel);
#endif

  memset(ximage->data, 0x00, ximage->bytes_per_line * ximage->height);

/*  pm = XCreatePixmap(display, RootWindow(display,screen_num),
    ximage_width, ximage_height, 1); */
  pm = XCreatePixmap(display, win,
    ximage_width, ximage_height, 1);
  gc_pixmap = XCreateGC(display, pm,0,(XGCValues*)0);


  XSetState(display, gc_pixmap,
    WhitePixel(display,screen_num), BlackPixel(display,screen_num),
    GXcopy,AllPlanes);
  XSetState(display, gc,
    WhitePixel(display,screen_num), BlackPixel(display,screen_num),
    GXcopy,AllPlanes);
  XSetState(display, gc_reverse,
    BlackPixel(display,screen_num), WhitePixel(display,screen_num),
    GXcopy,AllPlanes);



  /* Display window */
  XMapWindow(display, win);
  XSync(display, /* no discard */ 0);

{
  XEvent report;

bx_printf("waiting for MapNotify\n");
  while (1) {
    XNextEvent(display, &report);
    if (report.type == MapNotify) break;
    }
bx_printf("MapNotify found.\n");
}

#if 0
while (1) {
  bx_handle_x_events();
  }
#endif
}


  static void
load_font(void)
{

  /* Load font and get font information structure. */
  if ((font_info = XLoadQueryFont(display,"hercules")) == NULL) {
      (void) fprintf( stderr, "%s: Cannot open hercules font\n",
        progname);
    bx_panic("Could not open hercules font\n");
    }
}


  unsigned
bx_handle_x_events(void)
{
  XEvent report;
  XKeyEvent *key_event;
  KeySym keysym;
  XComposeStatus compose;
  char buffer[MAX_MAPPED_STRING_LENGTH];
  int bufsize = MAX_MAPPED_STRING_LENGTH;
  int charcount;
  unsigned retval = 0; /* normal return from function */

#if 0
  XPointerMovedEvent *pointer_event;
  XEnterWindowEvent *enter_event;
  XLeaveWindowEvent *leave_event;
  int prev_x, prev_y;


  prev_x = current_x;
  prev_y = current_y;
#endif

  /* get events, use first to display text and graphics */
  while (XPending(display) > 0)  {
    XNextEvent(display, &report);
    switch  (report.type) {

    case Expose:
      /* unless this is the last contiguous expose,
       * don't draw the window */
      if (report.xexpose.count != 0) {
        break;
        }

      retval = 1; /* screen needs refresh */
      break;

    case ConfigureNotify:
      break;

    case ButtonPress:
      XUnloadFont(display, font_info->fid);
      XFreeGC(display, gc);
      XFreeGC(display, gc_reverse);
      XCloseDisplay(display);
      bx_panic("using buttonpress to invoke exit()\n");
      break;

    case KeyPress:
      key_event = (XKeyEvent *) &report;
      charcount = XLookupString(key_event, buffer, bufsize, &keysym, &compose);
/*fprintf(stderr, ">>> %s\n", XKeysymToString(keysym));*/
      xkeypress(keysym, 0);
      break;

    case KeyRelease:
      key_event = (XKeyEvent *) &report;
      charcount = XLookupString(key_event, buffer, bufsize, &keysym, &compose);
/*fprintf(stderr, "<<< %s\n", XKeysymToString(keysym));*/
      xkeypress(keysym, 1);
      break;

    case MotionNotify:
#if 0
      pointer_event = (XPointerMovedEvent *) &report;
fprintf(stderr, "  x,y = (%d,%d)\n", pointer_event->x, pointer_event->y);
      current_x = pointer_event->x;
      current_y = pointer_event->y;
#endif
      break;

    case EnterNotify:
#if 0
      enter_event = (XEnterWindowEvent *) &report;
fprintf(stderr, "ENTER\n");
#endif
      break;

    case LeaveNotify:
#if 0
      leave_event = (XLeaveWindowEvent *) &report;
      current_x = -1;
      current_y = -1;
fprintf(stderr, "LEAVE\n");
#endif
      break;

    case MapNotify:
      /* screen needs redraw, since X would have tossed previous
       * requests before window mapped
       */
bx_printf("mapnotify: found\n");
      retval = 1;
      break;

    default:
      /* all events selected by StructureNotifyMask
       * except ConfigureNotify are thrown away here,
       * since nothing is done with them */
      break;
    } /* end switch */
  } /* end while */

#if 0
  if (bx_mouse_device_enabled) {
    /* see if cursor coordinates changed */
    if (prev_x!=current_x || prev_y!=current_y) {
      Bit8s dx_byte, dy_byte, header_byte;
      int dx, dy;
      if (current_x!=-1 && current_y!=-1 && prev_x!=-1 && prev_y!=-1) {
        fprintf(stderr, "dy=%d dx=%d\n", current_y - prev_y,
          current_x - prev_x);
        dx = (current_x - prev_x);
        dy = -1 * (current_y - prev_y);
        if (dx<-100 || dx>100) dx = (dx<0)? -100 : 100;
        if (dy<-100 || dy>100) dy = (dy<0)? -100 : 100;
        dx_byte = (Bit8s) dx;
        dy_byte = (Bit8s) dy;
        header_byte = 0x08;
        if (dx<0) header_byte |= 0x10;
        if (dy<0) header_byte |= 0x20;
        bx_mouse_enQ_packet(header_byte, dx_byte, dy_byte);
        }
      }
    }
#endif


  return(retval);
}

  void
bx_X_flush(void)
{
  XFlush(display);
}


  static void
xkeypress(KeySym keysym, int press_release)
{
  Bit8u scancode;

#if 0
  if (keysym == XK_F12) {
    bx_dbg.debugger = 1;
    bx_printf("F12: ++++++++++++++++++++++++++\n");
    return;
    }

  if (keysym == XK_F10) {
    if (press_release==0) {
      bx_dbg.floppy = 1;
      bx_dbg.keyboard = 1;
      bx_dbg.video = 1;
      bx_dbg.disk = 1;
      bx_dbg.pit = 1;
      bx_dbg.pic = 1;
      bx_dbg.bios = 1;
      bx_dbg.cmos = 1;
      bx_dbg.protected = 0;
      bx_dbg.a20 = 1;
      bx_dbg.interrupts = 0;
      bx_dbg.exceptions = 0;
      bx_dbg.unsupported = 1;
      bx_dbg.temp = 1;
      bx_dbg.reset = 1;
      bx_dbg.mouse = 1;
      bx_dbg.io = 1;
      /*bx_dbg.debugger = 0;*/
      }
    return;
    }
#endif


#if 0
  if (keysym == XK_F12) {
    if (press_release==0) {
      bx_mouse_enQ_packet(8, 2, 0); /* header */
      }
    return;
    }
  if (keysym == XK_F11) {
    if (press_release==0) {
      bx_mouse_enQ_packet(8, 0, 2); /* header */
      }
    return;
    }
  if (keysym == XK_F10) {
    if (press_release==0) {
      bx_mouse_enQ_packet(8 | 0x20, 0, 0xfe); /* header */
      }
    return;
    }
  if (keysym == XK_F9) {
    if (press_release==0) {
      bx_mouse_enQ_packet(8 | 0x10, 0xfe, 0); /* header */
      }
    return;
    }
  if (keysym == XK_F7) {
    if (press_release==0) {
      bx_mouse_enQ_packet(8 | 0x02, 0, 0); /* header */
      }
    else {
      bx_mouse_enQ_packet(8 | 0x00, 0, 0); /* header */
      }
    return;
    }
  if (keysym == XK_F8) {
    if (press_release==0) {
      bx_mouse_enQ_packet(8 | 0x01, 0, 0); /* header */
      }
    else {
      bx_mouse_enQ_packet(8 | 0x00, 0, 0); /* header */
      }
    return;
    }
#endif



#if 0
  if (keysym == XK_Tab) {
    bx_dbg.floppy = 1;
    bx_dbg.keyboard = 1;
    bx_dbg.video = 1;
    bx_dbg.disk = 1;
    bx_dbg.pit = 1;
    bx_dbg.pic = 1;
    bx_dbg.bios = 1;
    bx_dbg.cmos = 1;
    /*bx_dbg.protected = 1;*/
    bx_dbg.a20 = 1;
    /*bx_dbg.interrupts = 1;*/
    bx_dbg.exceptions = 1;
    bx_dbg.unsupported = 1;
    /*bx_dbg.reset = 1;*/
    bx_printf("============================\n");
    return;
    }
#endif

  if ((keysym >= XK_space) && (keysym <= XK_asciitilde)) {
    scancode = ascii_to_scancode[keysym];
    }
  else switch (keysym) {
    case XK_KP_1:
#ifdef XK_KP_End
    case XK_KP_End:
#endif
      scancode = 0x4f; break;
    case XK_Down:
    case XK_KP_2:
#ifdef XK_KP_Down
    case XK_KP_Down:
#endif
      scancode = 0x50; break;
    case XK_KP_3:
#ifdef XK_KP_Page_Down
    case XK_KP_Page_Down:
#endif
      scancode = 0x51; break;
    case XK_Left:
    case XK_KP_4:
#ifdef XK_KP_Left
    case XK_KP_Left:
#endif
      scancode = 0x4b; break;
    case XK_KP_5:                       
      scancode = 0x4c; break;
#ifdef XK_KP_Right
    case XK_Right:
#endif
    case XK_KP_6:
#ifdef XK_KP_Right
    case XK_KP_Right:
#endif
      scancode = 0x4d; break;
    case XK_KP_7:
#ifdef XK_KP_Home
    case XK_KP_Home:
#endif
      scancode = 0x47; break;
    case XK_Up:
    case XK_KP_8:
#ifdef XK_KP_Up
    case XK_KP_Up:
#endif
      scancode = 0x48; break;
    case XK_KP_9:
#ifdef XK_KP_Page_Up
    case XK_KP_Page_Up:
#endif
      scancode = 0x49; break;
    case XK_KP_0:
#ifdef XK_KP_Insert
    case XK_KP_Insert:
#endif
      scancode = 0x52; break;

    case XK_Delete:      scancode = 0x53; break;
    case XK_BackSpace:   scancode = 0x0e; break;
    case XK_Tab:         scancode = 0x0f; break;
    case XK_Return:      scancode = 0x1c; break;
    case XK_KP_Subtract: scancode = 0x4a; break;
    case XK_KP_Add:      scancode = 0x4e; break;
    case XK_Escape:      scancode = 0x01; break;
    case XK_F1:          scancode = 0x3b; break;
    case XK_F2:          scancode = 0x3c; break;
    case XK_F3:          scancode = 0x3d; break;
    case XK_F4:          scancode = 0x3e; break;
    case XK_F5:          scancode = 0x3f; break;
    case XK_F6:          scancode = 0x40; break;
    case XK_F7:          scancode = 0x41; break;
    case XK_F8:          scancode = 0x42; break;
    case XK_F9:          scancode = 0x43; break;
    case XK_F10:         scancode = 0x44; break;
    case XK_F11:         scancode = 0x57; break;
    case XK_F12:         scancode = 0x58; break;
    case XK_Control_L:   scancode = 0x1d; break;
    case XK_Shift_L:     scancode = 0x2a; break;
    case XK_Shift_R:     scancode = 0x36; break;
    case XK_Caps_Lock:   scancode = 0x3a; break;
    case XK_Num_Lock:    scancode = 0x45; break;
    case XK_Alt_L:       scancode = 0x38; break;
    
    default:
      fprintf(stderr, "xkeypress(): keysym %x unhandled!\n", (unsigned) keysym);
      return;
      break;
    }

  if (press_release)
    scancode |= 0x80;

  bx_keybd_gen_scancode(scancode);
}

  void
bx_X_beep(void)
{
}



  void
bx_X_outstring(Bit8u *string, Bit8u* attr, unsigned len, unsigned row,
           unsigned col)
{
  int font_height;
  unsigned i, start_i;
  Bit8u start_attr;

#if 1 /* bill gogo */
  if (novideo) return;
#endif

  font_height = font_info->ascent + font_info->descent;

  /* convert NULL's to spaces */
  for (i=0; i<len; i++)
    if (string[i] == 0) string[i] = 32;

  start_i = 0;
  start_attr = attr[0];

  for (i=1; i<len; i++) {
    if (attr[i] != start_attr) {
      XDrawImageString(
        display, 
        win,
        (start_attr & 0x07) ? gc : gc_reverse,

        (col+start_i)*font_info->max_bounds.width,
        row * font_height + font_info->max_bounds.ascent,

        &string[start_i],
        i-start_i);
      start_i = i;
      start_attr = attr[i];
      }
    }

  XDrawImageString(
    display, 
    win,
    (start_attr & 0x07) ? gc : gc_reverse,

    (col+start_i)*font_info->max_bounds.width,
    row * font_height + font_info->max_bounds.ascent,

    &string[start_i],
    i-start_i);
}

  void
bx_X_clear_screen(unsigned c1, unsigned r1, unsigned c2, unsigned r2,
    Bit8u blank_attr)
{
  static Bit8u blank_line_attr[80];
  unsigned row, i;

#if 1 /* billos */
  if (novideo) return;
#endif

  for (i=0; i<80; i++)
    blank_line_attr[i] = blank_attr;

  for (row=r1; row<=r2; row++) {
    bx_X_outstring(blank_line, blank_line_attr, 1+c2-c1, row, c1);
    }
}

  void
bx_X_scroll_screen(unsigned c1, unsigned r1, unsigned c2, unsigned r2,
    unsigned rows)
{
#if 1 /* billos */
  if (novideo) return;
#endif

  XCopyArea(display, win, win, gc,
    c1*font_info->max_bounds.width, /* srcx */
    (r1+rows)*font_height, /* srcy */
    (1+c2-c1)*font_info->max_bounds.width, /* width */
    ((1+r2-r1)-rows)*font_height, /* height */
    c1*font_info->max_bounds.width, /* dstx */
    r1*font_height /* dsty */
    );

#if 0
    c1*font_info->max_bounds.width, /* srcx */
    (r1+rows)*font_height + font_info->max_bounds.ascent, /* srcy */
    (1+c2-c1)*font_info->max_bounds.width, /* width */
    ((1+r2-r1)-rows)*font_height, /* height */
    c1*font_info->max_bounds.width, /* dstx */
    (r1-1)*font_height + font_info->max_bounds.ascent /* dsty */
#endif
}

  void
bx_X_draw_point(unsigned x, unsigned y, Bit8u set)
{

#if 1 /* billos */
  if (novideo) return;
#endif

  XDrawPoint(display, win, set? gc : gc_reverse, x, y);
  /*bx_printf("x = %u, y = %u, set = %u\n", x, y, (unsigned) set);*/
}

  void
bx_X_switch_mode(unsigned mode)
{
#if 1 /* billos */
  if (novideo) return;
#endif

  XClearWindow(display, win);
}

  void
bx_X_update(void)
{
  unsigned changed;
  unsigned row, byte, byte_start, byte_end, bit;
  unsigned row_start, row_end, tilex, tiley;
  Bit32u offset;
  Bit8u new_value;

#if 1 /* billos */
  if (novideo) return;
#endif

  offset = HGA_OFFSET;

  for (tiley=0; tiley<(y_tiles); tiley++) {
    row_start = tiley * ximage_height;
    row_end   = (tiley+1) * ximage_height - 1;
    if (row_end >= 348) row_end = 347;
    for (tilex=0; tilex<(x_tiles); tilex++) {
      byte_start = tilex * ximage_width_bytes;
      byte_end   = (tilex+1) * ximage_width_bytes - 1;
      if (byte_end >= 90) byte_end = 89;

      /* See if pixels in a given tile have changed. If not,
         skip repainting it. */
      changed = 0;
      for (row=row_start; row <= row_end; row++) {
        if (memcmp(&bx_hga_memory[offset + 0x2000*(row%4) +
              90*(row/4) + byte_start], &graphics_snapshot[
              0x2000*(row%4) + 90*(row/4) + byte_start],
              (1 + byte_end - byte_start))) {
          changed = 1;
          break;
          }
        }
      if (!changed) continue;

      for (row=row_start; row <= row_end; row++) {
        for (byte=byte_start; byte<=byte_end; byte++) {
          new_value = bx_hga_memory[offset + 0x2000*(row%4) +
            90*(row/4) + byte];
          graphics_snapshot[0x2000*(row%4) + 90*(row/4) + byte] = new_value;
          bit = 8;
          do {
            bit -= 1;
              XPutPixel(ximage, (byte-byte_start)*8 + bit,
                (row-row_start), new_value & 0x01);
            new_value >>= 1;
            } while (bit>0);
          } /* for (byte... */
        } /* for (row... */


      XPutImage(display, pm, gc_pixmap, ximage, 
        0,0,
        0,0,
        ximage_width, ximage_height);
      XCopyPlane(display, pm, win, gc,
        0,0,
        ximage_width, ximage_height,
        tilex * ximage_width, tiley * ximage_height, 1);
      } /* for (tilex... */
    } /* for (tiley... */
}

#endif /* WIN32 */
