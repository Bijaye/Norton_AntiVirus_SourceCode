#ifdef WIN32

#include "iodev.h"
#include "bochs_icon"

#include <time.h> /* riad */

#ifndef WIN32GUI

  void bx_W_init(int argc, char *argv[]) {}
  void bx_W_beep(void) {}
  void bx_W_outstring(Bit8u *string, Bit8u* attr, unsigned len, unsigned row, unsigned col) {}
  void bx_W_clear_screen(unsigned col1, unsigned row1, unsigned col2, unsigned row2, Bit8u blank_attr) {}
  void bx_W_scroll_screen(unsigned col1, unsigned row1, unsigned col2, unsigned row2, unsigned rows) {}
  void bx_W_draw_point(unsigned x, unsigned y, Bit8u se) {}
  void bx_W_switch_mode(unsigned mode) {}
  void bx_W_update(void) {}
  unsigned bx_handle_W_events(void) { return 0;}
  void bx_W_flush(void) {}

#else

/*-------------------------------------------------------------------*/

#include <windows.h>

#define MAX_MAPPED_STRING_LENGTH 10

static char *progname; /* name this program was invoked by */
static int rows=25, columns=80;
static unsigned int width, height, extrawidth, extraheight;  /* window size */
static unsigned font_width, font_height, font_ascent;
static Bit8u blank_line[80];

static unsigned ximage_width, ximage_height, ximage_width_bytes;
static unsigned x_tiles=18, y_tiles=10;

// Added a last element (value 0) just to make the table 128 bytes long, so that I can address it with ...&0x7F, Fred
Bit8u ascii_to_scancode[0x80] = {
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
  0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x29, 0x00  /* xyz{|}~  */
  };

// This array indicates which keys need to be shifted before the scancode is sent
Bit8u shifted_ascii[0x80] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,

  0, 1, 0, 1, 1, 1, 1, 0, /*  !"#$%&' */
  1, 1, 1, 1, 0, 0, 0, 0, /* ()*+,-./ */
  0, 0, 0, 0, 0, 0, 0, 0, /* 01234567 */
  0, 0, 1, 0, 1, 0, 1, 1, /* 89:;<=>? */

  0, 1, 1, 1, 1, 1, 1, 1, /* @ABCDEFG */
  1, 1, 1, 1, 1, 1, 1, 1, /* HIJKLMNO */
  1, 1, 1, 1, 1, 1, 1, 1, /* PQRSTUVW */
  1, 1, 1, 1, 0, 1, 0, 0, /* XYZ[\]^_ */   /* STefan: changed Shiftstate of "\" 1, 1, 1, 1, 1, 1, 0, 0, */ /* XYZ[\]^_ */

  0, 0, 0, 0, 0, 0, 0, 0, /* `abcdefg */
  0, 0, 0, 0, 0, 0, 0, 0, /* hijklmno */
  0, 0, 0, 0, 0, 0, 0, 0, /* pqrstuvw */
  0, 0, 0, 0, 0, 0, 0, 0  /* xyz{|}~  */
  };

#if 0
static int current_x = -1, current_y = -1;
#endif

extern Bit8u graphics_snapshot[32 * 1024];
extern int novideo;

#if BX_CPU < 2
const char window_name[] = "Bochs 8086 emulator, http://world.std.com/~bochs";
#elif BX_CPU == 2
const char window_name[] = "Bochs 80286 emulator, http://world.std.com/~bochs";
#elif BX_CPU == 3
const char window_name[] = "Bochs 80386 emulator, http://world.std.com/~bochs";
#else
const char window_name[] = "Bochs 80386+ emulator, http://world.std.com/~bochs";
#endif
const char icon_name[]   = "Bochs";

/* win32 variables */
const  char szAppName[]  = "Bochs 8x86";

static HINSTANCE g_hinst;/* not neccessary for it to be global */
static HWND      g_hwnd; /* not neccessary for it to be global */
static UINT      g_result;

LRESULT CALLBACK boboWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
void             setHDCTextColors(HDC hdc, int attr);

#define BLACK_COLOR RGB(0  ,0  ,0)
#define WHITE_COLOR RGB(255,255,255)

/*-------------------------------------------------------------------*/

void bx_W_init(int argc, char **argv)
{
  extern int iconic;

  unsigned i;

  WNDCLASSEX  wndclass ;

  #if 1 /* bill */
  if (novideo) return;
  #endif

  for (i=0; i<80; i++) 
  {
    blank_line[i] = ' ';
  }

  progname = argv[0];

  /* get usefull informations first */
  extrawidth = //GetSystemMetrics(SM_CXBORDER)*2 +
               GetSystemMetrics(SM_CXFRAME) *2;

  extraheight= //GetSystemMetrics(SM_CYBORDER)*2 +
               GetSystemMetrics(SM_CYFRAME) *2 +
               GetSystemMetrics(SM_CYCAPTION);

  /* Get the hinstance handle object */
  g_hinst = GetModuleHandle(NULL);

  /* let's first register the bochs class thingy */
  wndclass.cbSize        = sizeof (wndclass) ;
  /* the OWNDC style is to save all changes made to DC for later */
  /* Initial initializations could be made during WM_CREATE message */
  wndclass.style         = CS_OWNDC | CS_DBLCLKS;/* | CS_HREDRAW | CS_VREDRAW ;*/
  wndclass.lpfnWndProc   = boboWndProc; /* never put NULL here, or you are dead */
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = g_hinst;
  wndclass.hIcon         = LoadIcon (g_hinst, icon_name) ;
  wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
  wndclass.lpszMenuName  = NULL ;
  wndclass.lpszClassName = szAppName ;
  wndclass.hIconSm       = LoadIcon (g_hinst, icon_name) ;

  RegisterClassEx (&wndclass) ;

  /* a non resizable window */
  g_hwnd = CreateWindowEx(0, 
                          szAppName, window_name,
                          WS_OVERLAPPEDWINDOW,
                          //WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX | (iconic?WS_ICONIC:0),
                          0, 0,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, g_hinst, NULL) ;

  //SetClassLong(g_hwnd, GCL_HICON, LoadIcon (g_hinst, icon_name));
  //SetClassLong(g_hwnd, GCL_HICONSM, LoadIcon (g_hinst, icon_name));
  /* show window but don't display anything in it (don't generate the
   * WM_PAINT message
   */
  ShowWindow(g_hwnd, SW_SHOWNOACTIVATE | (iconic?SW_MINIMIZE:0));
  //ShowWindow(g_hwnd, SW_SHOWNORMAL);
  //UpdateWindow(g_hwnd);

  bx_handle_W_events();

  printf("WIN32INIT: done\n");

  /* sometimes, it really eats too much CPU */
//  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST); 

  return;
}

/*---------------------------------------------------------------------*/

extern int g_hurry_up;

unsigned bx_handle_W_events(void)
{
   MSG msg;
#ifdef AUDIT_ENABLED
   static clock_t firstime = -1;
#endif

   while (TRUE)
   {
      /* if you use g_hand rather than null, some messages, like
       * WM_QUIT won't come here. but may be going to the console.
       * but as the console doesn't proceed this, the window would
       * be alone to be destroyed
       */
      if(PeekMessage (&msg, NULL/*g_hwnd*/, 0, 0, PM_REMOVE))
      {
         if(msg.message == WM_QUIT)
         {
            bx_panic("WM_QUIT: received\n");
            break;
         }
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
      }
      else
         break; /* no more messages, let's ge tthe hell outta here */
   }
//#ifdef AUDIT_ENABLED
//   if(g_hurry_up)
//   {
//	   if(firstime == (clock_t)-1) firstime = clock();
//	   if((clock()-firstime)> 10000)
//	   {
//           bx_printf("* Forcing bochs to generate ESCAPE keycode\n");
//		   firstime = -1;
//		   bx_keybd_gen_scancode(0x01);
//	   }
//   }
//#endif

#if 0
   if(1)
   {
      int prev_x, prev_y;
      prev_x = current_x;
      prev_y = current_y;

      if (bx_mouse_device_enabled) 
      {
         /* see if cursor coordinates changed */
         if (prev_x!=current_x || prev_y!=current_y) 
         {
            Bit8s dx_byte, dy_byte, header_byte;
            int dx, dy;
            if (current_x!=-1 && current_y!=-1 && prev_x!=-1 && prev_y!=-1) 
            {
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
   }
#endif

   return g_result ;
}

/*---------------------------------------------------------------------*/

LRESULT CALLBACK boboWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   HDC hdc;

   PAINTSTRUCT  ps ;
   TEXTMETRIC   tm ;
   Bit8u        scancode;
   int          press_release = 0;
   ULONG        keysym;
   LPMINMAXINFO lpmmi;

   //RECT         rect;
   POINT        point;


   switch (iMsg)
   {
   case WM_CREATE :
      /* I can also get those values in the init function using
       * CreateDC("DISPLAY",...) or may be CreateIC("DISPLAY",...)
       */

      /* Set the Font Object, Set the window Sizes */
      hdc = GetDC(hwnd);

      SelectObject(hdc, GetStockObject (OEM_FIXED_FONT));
      GetTextMetrics (hdc, &tm) ;
      font_width  = tm.tmMaxCharWidth;
      font_height = tm.tmHeight ;//+ tm.tmExternalLeading;
      font_ascent = tm.tmAscent;
      width       = columns * font_width;
      height      = rows * font_height;

      bx_printf("font_width = %u\n", (unsigned) font_width);
      bx_printf("font_height = %u\n", (unsigned) font_height);

      ReleaseDC(hwnd, hdc);

      /* now set the dimensions of the zarking window */
      MoveWindow(hwnd, 
         0, 
         0, 
         width + extrawidth,
         height+ extraheight, 
         TRUE/*FALSE*/);

      /* set again the window icon */

      /* screen needs to be redrawn */
      g_result = 1;
      return 0;

   case WM_SIZE:
      /* zarkon....may be try to prevent it from being resizable */
      /* try to prevent the window from being resized or sized more */
      /* than or less than a certain value */

      //GetWindowRect(hwnd, &rect);
      //printf("Window: top=%4d left=%4d - bottom=%4d right=%4d\n",
      //   rect.top, rect.left, rect.bottom, rect.right);
      //GetClientRect(hwnd, &rect);
      //printf("Client: top=%4d left=%4d - bottom=%4d right=%4d\n",
      //   rect.top, rect.left, rect.bottom, rect.right);
      break;

   case WM_GETMINMAXINFO:
      lpmmi   = (LPMINMAXINFO) lParam;
      point.x = width + extrawidth;
      point.y = height+ extraheight;
      lpmmi->ptMaxSize = point;
      lpmmi->ptMaxTrackSize = point;
      lpmmi->ptMinTrackSize = point;
      break;

   case WM_LBUTTONDBLCLK:
      bx_printf("using buttonpress to invoke exit()\n");
      PostQuitMessage(0);
      //PostMessage(hwnd, WM_QUIT, 0, 0);
      return 0;

   case WM_DESTROY:
      bx_printf("using windows CLOSE to invoke exit()\n");
      PostQuitMessage(0);
      //PostMessage(hwnd, WM_QUIT, 0, 0);
      return 0;

   case WM_PAINT:
      /* If we need refresh, let bochs know it, and bochs would call
       * bx_W_update() to do the work of WM_PAINT
       */
      /* for multiple WM_PAINT (like when report.xexpos.count != 0)
       * windowNT or 95 handles this already
       */
      /* Usually, by calling BeginPaint, the background is erased (I think)
       * in this case, we just don't give a damn. But we need it for scrollwindow
       * for exemple, or other related problems.
       */
      //bx_W_update();
      //{
      //char zarkon[] = "This Works ???????????????";
      hdc = BeginPaint(hwnd, &ps);
      //TextOut(hdc, 0, 0, zarkon, sizeof(zarkon));
      EndPaint(hwnd, &ps);
      //}
      g_result = 1;
      return 0;

   case WM_SYSKEYUP:
   case WM_KEYUP:
      press_release = 1;
      /* then continue with the same zarking routine */
   case WM_SYSKEYDOWN:
   case WM_KEYDOWN:
      keysym = wParam;
#if 0
      if (keysym == VK_F12) 
      {
        bx_dbg.debugger = 1;
        bx_printf("F12: ++++++++++++++++++++++++++\n");
        break;
      }
      else (keysym == VK_F10) 
      {
        if (press_release==0) 
        {
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
        break;
      }
      else if (keysym == VK_F12) {
        if (press_release==0) {
          bx_mouse_enQ_packet(8, 2, 0); /* header */
          }
        break;
        }
      else if (keysym == VK_F11) {
        if (press_release==0) {
          bx_mouse_enQ_packet(8, 0, 2); /* header */
          }
        break;
        }
      else if (keysym == VK_F10) {
        if (press_release==0) {
          bx_mouse_enQ_packet(8 | 0x20, 0, 0xfe); /* header */
          }
        break;
        }
      else if (keysym == VK_F9) {
        if (press_release==0) {
          bx_mouse_enQ_packet(8 | 0x10, 0xfe, 0); /* header */
          }
        break;
        }
      else if (keysym == VK_F7) {
        if (press_release==0) {
          bx_mouse_enQ_packet(8 | 0x02, 0, 0); /* header */
          }
        else {
          bx_mouse_enQ_packet(8 | 0x00, 0, 0); /* header */
          }
        break;
        }
     else if (keysym == VK_F8) {
        if (press_release==0) {
          bx_mouse_enQ_packet(8 | 0x01, 0, 0); /* header */
          }
        else {
          bx_mouse_enQ_packet(8 | 0x00, 0, 0); /* header */
          }
        break;
        }

     if (keysym == VK_TAB) {
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
        break;
        }
#endif

      /* Let's just get the 8-Bit OEM Scan Code */
      scancode = (Bit8u)((lParam >> 16) & 0xFF);
      if(press_release) scancode |= 0x80;
      //printf("scancode=%02X (lParam=%04X)\n", scancode, lParam);
      /* some scancodes may not be handled by bochs, take care ol'buddy */
      bx_keybd_gen_scancode(scancode);
      /* break, so SYSKEYUP messages are processed */
      break;

   case WM_MOUSEMOVE:
#if 0
      current_x = LOWORD(lParam);
      current_y = HOWORD(lParam);
      fprintf(stderr, "  x,y = (%d,%d)\n", current_x, current_y);
#endif
      break;

   case WM_SETFOCUS:
      break;

   case WM_KILLFOCUS:
      break;

   default:
      break;
   }

   return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}


/*---------------------------------------------------------------------*/

void bx_W_outstring(Bit8u *string, 
                    Bit8u* attr, 
                    unsigned len,  /* for 16 bits portability, use unsigned long or short */
                    unsigned row,
                    unsigned col)
{
  /* int font_height; */
  unsigned  i, start_i;
  Bit8u     start_attr;
  HDC       hdc;

#ifdef WIN32DEBUG3
  //printf("outstring: row=%d col=%d\n", row, col);
#endif

#if 1 /* bill gogo */
  if (novideo) return;
#endif

  /*font_height = font_info->ascent + font_info->descent;*/

  /* convert NULL's to spaces */
  for (i=0; i<len; i++)
    /* this about UNICODE buddy */
    if (string[i] == '\0') string[i] = ' ' /* 32 */;

  start_i = 0;
  start_attr = attr[0];

  //InvalidateRect(g_hwnd, NULL, FALSE);
  hdc = GetDC(g_hwnd);

  for (i=1; i<len; i++) 
  {
     setHDCTextColors(hdc, start_attr & 0x07);
     if (attr[i] != start_attr) 
     {
      TextOut(
        hdc, 
        (col+start_i)*font_width,
        row * font_height, // + font_ascent,
        &string[start_i],
        i-start_i);
      start_i = i;
      start_attr = attr[i];
     }
   }

  setHDCTextColors(hdc, start_attr & 0x07);
  TextOut(
    hdc,
    (col+start_i)*font_width,
    row * font_height, //+ font_ascent,
    &string[start_i],
    i-start_i);

  ReleaseDC(g_hwnd, hdc);

#if 0
     for(i=0;i<len;i++) putchar(string[i]);
#endif
  return;
}

/*---------------------------------------------------------------------*/

void bx_W_clear_screen(unsigned c1, 
                       unsigned r1, 
                       unsigned c2, 
                       unsigned r2,
                       Bit8u blank_attr)
{
  static Bit8u blank_line_attr[80];
  unsigned row, i;

#ifdef WIN32DEBUG3
  //printf("clear_screen: c1=%d r1=%d c2==%d r2=%d\n", c1, r1, c2, r2);
#endif

#if 1 /* billos */
  if (novideo) return;
#endif

  for (i=0; i<80; i++)
    blank_line_attr[i] = blank_attr;

  for (row=r1; row<=r2; row++) 
  {
    bx_W_outstring(blank_line, blank_line_attr, 1+c2-c1, row, c1);
  }
}

/*---------------------------------------------------------------------*/

void bx_W_scroll_screen(unsigned c1, 
                        unsigned r1, 
                        unsigned c2, 
                        unsigned r2,
                        unsigned rows)
{
   RECT scrollrect;

#ifdef WIN32DEBUG3
  //printf("scroll: c1=%d r1=%d c2=%d r2=%d rows=%d\n", c1,r1,c2,r2,rows);
#endif

#if 1 /* billos */
  if (novideo) return;
#endif

  /* I can also use PatBlt, but since the purpose here is to
   * scroll, let's do it directly, faster, and zarking better
   */
  scrollrect.left       = (c1)*font_width;
  scrollrect.top        = (r1)*font_height;
  scrollrect.right      = (c2+1)*font_width;
  scrollrect.bottom     = (r2+1)*font_height;

  ScrollWindow (   /* scroll will call WM_PAINT, so let's update */
    g_hwnd, 
    0,
    -rows*(font_height),
    &scrollrect, 
    NULL);

  UpdateWindow(g_hwnd); /* very important !! */

#if 0
  XCopyArea(display, win, win, gc,
    c1*font_info->max_bounds.width, /* srcx */
    (r1+rows)*font_height, /* srcy */
    (1+c2-c1)*font_info->max_bounds.width, /* width */
    ((1+r2-r1)-rows)*font_height, /* height */
    c1*font_info->max_bounds.width, /* dstx */
    r1*font_height /* dsty */
    );
#endif
}

/*---------------------------------------------------------------------*/

void bx_W_flush(void) {}

/*---------------------------------------------------------------------*/

void bx_W_beep(void)
{
   Beep(100, 500);
}

/*---------------------------------------------------------------------*/

void bx_W_draw_point(unsigned x, unsigned y, Bit8u set)
{
  HDC hdc;

#ifdef WIN32DEBUG3
  printf("draw_point: x=%d y=%d\n", x, y);
#endif

#if 1 /* billos */
  if (novideo) return;
#endif

  hdc = GetDC(g_hwnd);
  SetPixel(hdc, x, y, set?BLACK_COLOR:BLACK_COLOR);
  ReleaseDC(g_hwnd, hdc);
  /*bx_printf("x = %u, y = %u, set = %u\n", x, y, (unsigned) set);*/
}

/*---------------------------------------------------------------------*/

void bx_W_switch_mode(unsigned mode)
{
  HDC  hdc;
  RECT rectos;

#if 1 /* billos */
  if (novideo) return;
#endif

  hdc = GetDC(g_hwnd);
  GetWindowRect(g_hwnd, &rectos);
  FillRect(hdc, &rectos, GetStockObject(BLACK_BRUSH));
  /* may be call DeleteObject() ? */
  ReleaseDC(g_hwnd, hdc);
  return; 
  
  /*ClearWindow(display, win);*/
}

/*---------------------------------------------------------------------*/

void bx_W_update(void)
{
  unsigned changed;
  unsigned row, byte, byte_start, byte_end, bit;
  unsigned row_start, row_end, tilex, tiley;
  Bit32u   offset;
  Bit8u    new_value;

  HDC          hdc;
  //PAINTSTRUCT  ps;

#ifdef WIN32DEBUG3
  printf("update:\n");
#endif

#if 1 /* billos */
  if (novideo) return;
#endif

  hdc = GetDC(g_hwnd);
  //hdc = BeginPaint (g_hwnd, &ps) ;

  offset = HGA_OFFSET;

  for (tiley=0; tiley<(y_tiles); tiley++) 
  {
    row_start = tiley * ximage_height;
    row_end   = (tiley+1) * ximage_height - 1;
    if (row_end >= 348) row_end = 347;
    for (tilex=0; tilex<(x_tiles); tilex++) 
    {
      byte_start = tilex * ximage_width_bytes;
      byte_end   = (tilex+1) * ximage_width_bytes - 1;
      if (byte_end >= 90) byte_end = 89;

      /* See if pixels in a given tile have changed. If not,
         skip repainting it. */
      changed = 0;
      for (row=row_start; row <= row_end; row++) 
      {
        if (memcmp(&bx_hga_memory[offset + 0x2000*(row%4) +
              90*(row/4) + byte_start], &graphics_snapshot[
              0x2000*(row%4) + 90*(row/4) + byte_start],
              (1 + byte_end - byte_start))) 
        {
          changed = 1;
          break;
        }
      }
      if (!changed) continue;

      for (row=row_start; row <= row_end; row++) 
      {
        for (byte=byte_start; byte<=byte_end; byte++) 
        {
          new_value = bx_hga_memory[offset + 0x2000*(row%4) +
            90*(row/4) + byte];
          graphics_snapshot[0x2000*(row%4) + 90*(row/4) + byte] = new_value;
          bit = 8;
          do 
          {
            bit -= 1;
              SetPixel(hdc, (byte-byte_start)*8 + bit,
                 (row-row_start), (new_value & 0x01)?WHITE_COLOR:BLACK_COLOR);
            new_value >>= 1;
          } while (bit>0);
        } /* for (byte... */
      } /* for (row... */

#if 0
      XPutImage(display, pm, gc_pixmap, ximage, 
        0,0,
        0,0,
        ximage_width, ximage_height);
      XCopyPlane(display, pm, win, gc,
        0,0,
        ximage_width, ximage_height,
        tilex * ximage_width, tiley * ximage_height, 1);
#endif
    } /* for (tilex... */
  } /* for (tiley... */

  ReleaseDC(g_hwnd, hdc);
  //EndPaint (g_hwnd, &ps);
}

/*---------------------------------------------------------------------*/

void setHDCTextColors(HDC hdc, int attr)
{
    if(attr) 
    { SetTextColor(hdc, WHITE_COLOR); SetBkColor(hdc, BLACK_COLOR); }
    else
    { SetTextColor(hdc, BLACK_COLOR); SetBkColor(hdc, WHITE_COLOR); }

}
/*---------------------------------------------------------------------*/

#endif /* WIN32GUI */

#endif /* WIN32 */
