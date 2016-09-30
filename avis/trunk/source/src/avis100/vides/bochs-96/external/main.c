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



#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <bochs.h>


static Bit8u hga_video_buffer[64 * 1024];

#define NUM_MESSAGES 7
static char *message[NUM_MESSAGES] = {
  "This message should come up right away.",
  "The next messages don't depend on TICK(), only bx_hga_set_video_memory().",
  "  ... (1) this should have printed about 1 second later.",
  "  ... (2) and another 1 second later.",
  "  ... (3) and another 1 second later.",
  "  ... (4) and another 1 second later.",
  "  ... (5) and another 1 second later."
  };

static Bit8u a20_gate = 1;

extern Bit32u bx_num_ticks;
Boolean failure = 0;

  int
main(int argc, char *argv[])
{
  Bit8u value;
  Bit32u i;
  Bit32u value32;
  Bit16u value16;
  unsigned times;

  bx_bochs_init(argc, argv);

  memset(hga_video_buffer, 0, 64*1024);

  for (i=0; i<80*25; i++) {
    hga_video_buffer[i*2] = 0x20; /* space */
    hga_video_buffer[i*2 + 1] = 0x07; /* normal text attribute */
    }

  for (i=0; i<strlen(message[0]); i++) {
    hga_video_buffer[0 + i*2] = (Bit8u) message[0][i];
    hga_video_buffer[0 + i*2 + 1] = 0x07; /* normal text attribute */
    }
  
  fprintf(stderr, "=========================================================\n");
  fprintf(stderr, "TEST1: setting HGA video memory:\n");
  fprintf(stderr, "  screen should be all blanks except for message on top\n");
  fprintf(stderr, "  it might take a few seconds for the X window to map...\n");
  bx_hga_set_video_memory(hga_video_buffer);

  fprintf(stderr, "=========================================================\n");
  fprintf(stderr, "TEST2: inp()/outp() calls and the PIC\n");
  fprintf(stderr, "\nlooking at master PIC IMR\n");
  value = bx_inp(0x0021, 1);
  fprintf(stderr, "INP(21) = %02x\n", (unsigned) value);

  fprintf(stderr, "\nallowing IMR IRQ 7\n");
  value &= 0x7f;
  bx_outp(0x0021, value, 1);

  fprintf(stderr, "\nlooking at master PIC IMR\n");
  value = bx_inp(0x0021, 1);
  fprintf(stderr, "INP(21) = %02x\n", (unsigned) value);
  if (value != 0x7f)
    failure = 1;

  /* Give enough ticks to set off the PIT timer. */
  for (i=0; i<= 60000; i++) {
    bx_TICK();
    }

  fprintf(stderr, "=========================================================\n");
  fprintf(stderr, "TEST3:\n");
  bx_outp(0x0064, 0xd0, 1); /* read output port */
  value = bx_inp(0x0060, 1);
  fprintf(stderr, "A20 GATE initially of value %u\n",
    (unsigned) (value & 0x02) > 0);
  if ( ((value & 0x02) > 0) != a20_gate ) {
    fprintf(stderr, "\nERROR in A20 logic!\n");
    exit(-1);
    } 

  fprintf(stderr, "\nusing keyboard to disable A20 GATE\n");
  bx_outp(0x0064, 0xd1, 1); /* write output port */
  bx_outp(0x0060, value & 0xfd, 1); /* mask out A20 */
  fprintf(stderr, "A20 GATE now of value %u\n", (unsigned) a20_gate);
  if (a20_gate) failure = 1;

  fprintf(stderr, "\nusing keyboard to enable A20 GATE\n");
  bx_outp(0x0064, 0xd1, 1); /* write output port */
  bx_outp(0x0060, value | 0x02, 1); /* enable A20 */
  fprintf(stderr, "A20 GATE now of value %u\n", (unsigned) a20_gate);
  if (a20_gate==0) failure = 1;
  fprintf(stderr, "=========================================================\n");
  fprintf(stderr, "TEST3:\n");

  fprintf(stderr, "\nenabling IRQ 0\n");
  value = bx_inp(0x0021, 1);
  value &= 0xfe;
  bx_outp(0x0021, value, 1);
  
  fprintf(stderr, "external_set_INTR() should have been called\n");
  fprintf(stderr, "acknowledging interrupt\n");
  value = bx_IAC();
  fprintf(stderr, "CPU would call Int%02xh\n", (unsigned) value);
  if (value != 0x08) failure = 1;

  fprintf(stderr, "=========================================================\n");
  fprintf(stderr, "TEST4:\n");
  fprintf(stderr, "\nwriting 01234567h to 32bit IO port fffch\n");
  bx_outp(0xfffc, 0x01234567, 4);
  value32 = bx_inp(0xfffc, 4);
  fprintf(stderr, "32 bit IO read of port fffch is %08x\n",
    (unsigned) value32);
  if (value32 != 0x01234567) failure = 1;

  fprintf(stderr, "\nwriting abcdh to 16bit IO port fffeh\n");
  bx_outp(0xfffe, 0xabcd, 2);
  value16 = bx_inp(0xfffe, 2);
  fprintf(stderr, "16 bit IO read of port fffeh is %04x\n",
    (unsigned) value16);
  if (value16 != 0xabcd) failure = 1;

  fprintf(stderr, "\nreading 32bit IO port fffc again; shouldn't have"
                  " changed since last 32bit write\n");
  value32 = bx_inp(0xfffc, 4);
  fprintf(stderr, "32 bit IO read of port fffch is %08x\n",
    (unsigned) value32);
  if (value32 != 0x01234567) failure = 1;

  fprintf(stderr, "=========================================================\n");
  for (times=1; times<NUM_MESSAGES; times++) {
    for (i=0; i<strlen(message[times]); i++) {
      hga_video_buffer[times*80*2 + i*2] = (Bit8u) message[times][i];
      }
    bx_hga_set_video_memory(hga_video_buffer);
    sleep(1);
    }
  fprintf(stderr, "=========================================================\n");
  fprintf(stderr, "TEST5:\n");
  fprintf(stderr, "\nsending keyboard CPU reset command\n");
  bx_outp(0x0064, 0xfe, 1);

  /* shouldn't get here, since external_reset_cpu() should bomb */
  fprintf(stderr, "ERROR, shouldn't get here!\n");
  return(0);
}

  void
external_set_enable_a20(Bit8u value)
{
  fprintf(stderr, ">> external_set_enable_a20() called\n");
  a20_gate = value;
}

  Boolean
external_get_enable_a20(void)
{
  fprintf(stderr, ">> external_get_enable_a20() called\n");
  return(a20_gate);
}

  void
external_reset_cpu(void)
{
  fprintf(stderr, ">> reset_cpu(): keyboard requested RESET!\n");
  fprintf(stderr, ">> hit [return] to end\n");
  getchar();
  if (failure==0)
    fprintf(stderr, "All tests passed!  Could you read the text in the window?\n");
  else
    fprintf(stderr, "One or more tests failed!\n");
  exit(0);
}

  void
external_set_INTR(Boolean value)
{
  fprintf(stderr, ">> external_set_INTR(%u) called\n", (unsigned) value);
}
