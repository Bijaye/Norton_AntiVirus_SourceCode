/* extra.c -- extra Bochs services to act sort of like an interrupt but
   not really.

   Does stuff like exit bochs, switch boot drive on the fly, and pretty 
   much else whatever we damn well feel like. 
*/

#include "bochs.h"
#include "iodev/bios.h"

#include <time.h>


extern bx_options_t bx_options;
extern time_t time_ofs;
extern void (*bx_interrupt_table[])(int);
extern int switchC;

void bx_EXTRA_SERVICES(void)
{
  Bit8u service = bx_fetch_next_byte();

  switch(service)
    {
    case 0x0: /* halt */
      exit(0);
      break;
      
    case 0x1: /* switch boot drive to A */
      switchC = 0;
      strcpy(bx_options.bootdrive,"a");
      break;

    case 0x2: /* switch boot drive to B */
      switchC = 0;
      strcpy(bx_options.bootdrive,"b");
      break;

    case 0x3: /* switch boot drive to C */
      strcpy(bx_options.bootdrive,"c");
      break;

    case 0x4: /* "Cold" reboot */
      bx_coldboot();
      break;

    case 0x5: /* switch diskimage to file */
      bx_switch_disk();
      break;

    default:
      bx_panic("Extra service %d not supported!\n", service);
      return;
    }
}      

mytime(time_t *t)
{
  return (time(t)+time_ofs);
}




