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
00000 .. 9FFFF      (1st 640K)
==============
  000 ..   3FF      interrupt vector table
  400 ..   500      BIOS data area
  500 ..   8ff      DOS ???
 7C00 ..            1st 512 bytes of bootstrap program loaded here
9FC00 ..            Extended BIOS data area

B0000 .. BFFFF      HGA video buffer
E0000 .. FFFFF      ROM BIOS



*/

#define BX_DISKETTE_PARAM_TBL  0xF0000  /* 11-byte disk parameter table location */

  /* location of default interrupt handler.  Initially, all int vectors
     are loaded with this value.  Then supported ones are defined */
#define BX_DEFAULT_INT_HANDLER 0xF0010 

  /* 16 byte video initialization data array.  To be sent to ports
     3b4/3b5 */
#define BX_VIDEO_INIT_ARRAY    0xF0020

  /* 10 byte bios configuration table */
#define BX_BIOS_CONFIG_TABLE   0xF0030

  /* 16 byte hard disk prameter table */
#define BX_DISK_PARAM_TBL      0xF0040

  /* POST code */
#define BX_POST_CODE           0xF0060

  /* BIOS Copyright message */
#define BX_COPYRIGHT_LOC       0xFFF00

/* FFFF0..FFFF4 far jump to POST */
/* FFFF5 BIOS date */
/* FFFFE model id  */
