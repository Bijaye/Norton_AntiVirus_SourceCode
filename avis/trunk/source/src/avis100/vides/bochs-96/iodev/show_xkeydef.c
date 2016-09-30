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

#define MAX_MAPPED_STRING_LENGTH 10

/* These are used as arguments to nearly every Xlib routine, so it saves 
 * routine arguments to declare them global.  If there were 
 * additional source files, they would be declared extern there. */
static Display *display;
static int screen_num;

static char *progname; /* name this program was invoked by */



static Window win;
static GC gc;

static unsigned int width, height;  /* window size */







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
  0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x07, 0x0c, /* XYZ[/]^_ */
  0x29, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, /* `abcdefg */
  0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /* hijklmno */
  0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, /* pqrstuvw */
  0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x29        /* xyz{|}~  */
  };

extern Bit8u graphics_snapshot[32 * 1024];


void xkeypress(KeySym keysym, int press_release);
void refresh_screen(void);

#define ROUNDUP(nbytes, pad) ((((nbytes) + ((pad)-1)) / (pad)) * ((pad)>>3))


void main(int argc, char **argv)
{
  int x, y;   /* window position */
  unsigned int border_width = 4;  /* four pixels */



  XSizeHints size_hints;
  char *display_name = NULL;
  /* create GC for text and drawing */
  unsigned long valuemask = 0; /* ignore XGCvalues and use defaults */
  XGCValues values;



  progname = argv[0];

  /* connect to X server */
  if ( (display=XOpenDisplay(display_name)) == NULL )
  {
    fprintf(stderr, "%s: cannot connect to X server %s\n", 
        progname, XDisplayName(display_name));
  }

  /* get screen size from display structure macro */
  screen_num = DefaultScreen(display);

  /* Note that in a real application, x and y would default to 0
   * but would be settable from the command line or resource database.  
   */
  x = y = 0;


  
  width = 100;
  height = 100;

  /* create opaque window */
  win = XCreateSimpleWindow(display, RootWindow(display,screen_num),
    x, y,
    width,
    height,
    border_width,
    BlackPixel(display, screen_num),
    BlackPixel(display, screen_num));

  size_hints.flags = PPosition | PSize | PMinSize | PMaxSize;
  size_hints.max_width = size_hints.min_width = width;
  size_hints.max_height = size_hints.min_height = height;

  {
  XWMHints wm_hints;
  XClassHint class_hints;


  wm_hints.initial_state = NormalState;
  wm_hints.input = True;
  wm_hints.flags = StateHint | IconPixmapHint | InputHint;

  class_hints.res_name = progname;
  class_hints.res_class = "Bochs";

  XSetWMProperties(display, win, NULL, NULL, 
      argv, argc, &size_hints, &wm_hints, 
      &class_hints);
  }

  /* Select event types wanted */
  XSelectInput(display, win, ExposureMask | KeyPressMask | KeyReleaseMask |
      ButtonPressMask | StructureNotifyMask);


  /* Create default Graphics Context */
  gc = XCreateGC(display, win, valuemask, &values);


  /* Display window */
  XMapWindow(display, win);
  XFlush(display);

while (1) {
  bx_handle_x_events();
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


  /* get events, use first to display text and graphics */
  while (XPending(display) > 0)  {
    XNextEvent(display, &report);
    switch  (report.type) {

    case Expose:
      /* unless this is the last contiguous expose,
       * don't draw the window */
      if (report.xexpose.count != 0)
        break;

      retval = 1; /* screen needs refresh */
      break;

    case ConfigureNotify:
      break;

    case ButtonPress:
      XFreeGC(display, gc);
      XCloseDisplay(display);
      fprintf(stderr, "using buttonpress to invoke exit()\n");
      break;

    case KeyPress:
      key_event = (XKeyEvent *) &report;
      charcount = XLookupString(key_event, buffer, bufsize, &keysym, &compose);
fprintf(stderr, ">>> %s %04x\n", XKeysymToString(keysym), (unsigned) keysym);
      /*xkeypress(keysym, 0);*/
      break;

    case KeyRelease:
      key_event = (XKeyEvent *) &report;
      charcount = XLookupString(key_event, buffer, bufsize, &keysym, &compose);
fprintf(stderr, "<<< %s\n", XKeysymToString(keysym));
      /*xkeypress(keysym, 1);*/
      break;

    default:
      /* all events selected by StructureNotifyMask
       * except ConfigureNotify are thrown away here,
       * since nothing is done with them */
      break;
    } /* end switch */
  } /* end while */


  /* no sense XFlush'ing if screen needs refresh */
  if (!retval)
    XFlush(display);
  return(retval);
}
