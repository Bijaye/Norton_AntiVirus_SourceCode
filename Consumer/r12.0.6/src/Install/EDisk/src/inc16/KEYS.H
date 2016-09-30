/* $Header:   S:/INCLUDE/VCS/KEYS.H_V   1.18   24 Apr 1992 17:57:22   PETERD  $ */

/*
 * $Log:   S:/INCLUDE/VCS/KEYS.H_V  $
 * 
 *    Rev 1.18   24 Apr 1992 17:57:22   PETERD
 * Added CTRL_ALT_F9 and _F10 keys.
 * 
 *    Rev 1.17   22 Apr 1992 15:54:14   PETERD
 * Added CTRL-ALT-F1 and CTRL-ALT-F2 keys
 * 
 *    Rev 1.16   21 Mar 1992 20:53:04   PETERD
 * Added KB_TYPEMATIC_OFF and KB_DETECT_SPECIAL flags.
 * 
 *    Rev 1.15   10 Mar 1992 08:54:08   MARKK
 * Added crtl-grey plus/minus
 * 
 *    Rev 1.14   03 Mar 1992 15:33:36   MARKK
 * Nuked ALT_TAB
 * 
 *    Rev 1.13   14 Feb 1992 16:47:28   DAVIDH
 * Added constants for the remaining shifted numeric keypad keys, 
 * to prevent having only half of them generate numbers.
 * 
 *    Rev 1.12   13 Feb 1992 04:40:16   MARKK
 * Added Ctrl-slash
 * 
 *    Rev 1.11   05 Feb 1992 13:10:06   MARKK
 * Added new keys
 * 
 *    Rev 1.10   13 Jan 1992 13:28:16   HENRI
 * Added definitions for F11 and F12.
 * 
 *    Rev 1.9   15 Nov 1991 17:24:42   KEVIN
 * removed them, should have known better
 * 
 *    Rev 1.8   15 Nov 1991 16:46:38   KEVIN
 * added some ctrl defines
 * 
 *    Rev 1.7   16 Oct 1991 13:00:06   MARKK
 * Added key defs
 * 
 *    Rev 1.6   03 Oct 1991 09:27:24   MARKK
 * Undid previous changes
 * 
 *    Rev 1.4   19 Aug 1991 08:16:02   MARKK
 * Added ALT_SPACE and ALT_MINUS.
 * 
 *    Rev 1.3   19 Jul 1991 17:45:04   HENRI
 * Added some more extended key definitions.
 * 
 *    Rev 1.2   19 Jul 1991 17:38:56   BRAD
 * Added ALT_TAB, ALT_ESC, CTRL_UPARROW,
 * and CTRL_DOWNARROW
 * 
 * 
 *    Rev 1.1   01 Apr 1991 11:11:56   ED
 * Added definitions for GREY_SLASH and GREY_STAR
 * 
 *    Rev 1.0   29 Aug 1990 11:24:46   DAVID
 * Initial revision.
 */


/*		Copyright (C) 1989 by Peter Norton Computing		*/
#ifndef INCL_KEYS
#define INCL_KEYS

/*----------------------------------------------------------------------*
 * These flags are used in the external Word wKeyboardFlags to control
 * keyboard response, and return some status.
 *----------------------------------------------------------------------*/
#define	KB_TYPEMATIC_OFF	0x0001	// Turn off typematic keys
#define	KB_DETECT_SPECIAL	0x0002	// Detect the special keyboard code

#define	KB_SPECIAL_DETECTED	0x0100	// Special combo detected.

/*----------------------------------------------------------------------*
 * These definitions are for the shift key states returned by the	*
 * Shift_flags() call.							*
 *----------------------------------------------------------------------*/
#define		INS_STATE	0x80	/* Insert state is turned on	*/
#define		CAPS_STATE	0x40	/* Caps lock key is on		*/
#define		NUM_STATE	0x20	/* Num lock is turned on	*/
#define		SCROLL_STATE	0x10	/* Scroll lock is on		*/
#define		ALT_SHIFT	0x08	/* The alt key is down		*/
#define		CTL_SHIFT	0x04	/* The control key is down	*/
#define		LEFT_SHIFT	0x02	/* The left shift key is down	*/
#define		RIGHT_SHIFT	0x01	/* The right shift key is down	*/
#define 	SHIFT_FLAGS	(LEFT_SHIFT | RIGHT_SHIFT)


/*----------------------------------------------------------------------*
 * The following constants define the character codes that		*
 * read_extended returns.  These codes make no distinction between	*
 * characters such as ^H and the grey backspace key, even though the	*
 * ROM BIOS read routine does make a distinction.			*
 *----------------------------------------------------------------------*/
#define		CTRL_A		0x001
#define		CTRL_B		0x002
#define		CTRL_C		0x003
#define		CTRL_D		0x004
#define		CTRL_E		0x005
#define		CTRL_F		0x006
#define		CTRL_G		0x007
#define		CTRL_K		0x00B
#define		CTRL_L		0x00C
#define		CTRL_N		0x00E
#define		CTRL_O		0x00F
#define		CTRL_P		0x010
#define		CTRL_Q		0x011
#define		CTRL_R		0x012
#define		CTRL_S		0x013
#define		CTRL_T		0x014
#define		CTRL_U		0x015
#define		CTRL_V		0x016
#define		CTRL_W		0x017
#define		CTRL_X		0x018
#define		CTRL_Y		0x019
#define		CTRL_Z		0x01A

#define 	ALT_A	(30 + 0x100)	
#define 	ALT_B	(48 + 0x100)
#define 	ALT_C	(46 + 0x100)
#define 	ALT_D	(32 + 0x100)
#define 	ALT_E	(18 + 0x100)
#define 	ALT_F	(33 + 0x100)
#define 	ALT_G	(34 + 0x100)
#define 	ALT_H	(35 + 0x100)
#define 	ALT_I	(23 + 0x100)
#define 	ALT_J	(36 + 0x100)
#define 	ALT_K	(37 + 0x100)
#define 	ALT_L	(38 + 0x100)
#define 	ALT_M	(50 + 0x100)
#define 	ALT_N	(49 + 0x100)
#define 	ALT_O	(24 + 0x100)
#define 	ALT_P	(25 + 0x100)
#define 	ALT_Q	(16 + 0x100)
#define 	ALT_R	(19 + 0x100)
#define 	ALT_S	(31 + 0x100)
#define 	ALT_T	(20 + 0x100)
#define 	ALT_U	(22 + 0x100)
#define 	ALT_V	(47 + 0x100)
#define 	ALT_W	(17 + 0x100)
#define 	ALT_X 	(45 + 0x100)
#define 	ALT_Y   (21 + 0x100)
#define 	ALT_Z	(44 + 0x100)

#define		BS		0x008
#define		TAB		0x009
#define		LF		0x00A
#define		CTRL_ENTER	0x00A
#define		FORM_FEED	0x00C
#define		CR		0x00D
#define		ESC		0x01B
#define		CTRL_BACKSLASH	0x01C
#define		CTRL_BS		0x07F
#define		CTRL_BREAK	0x100
#define		SHIFT_TAB	0x10F
#define		GREY_STAR	0x137
#define		GREY_SLASH	0x136		
#define		F1		0x13B
#define		F2		0x13C
#define		F3		0x13D
#define		F4		0x13E
#define		F5		0x13F
#define		F6		0x140
#define		F7		0x141
#define		F8		0x142
#define		F9		0x143
#define		F10		0x144
#define		HOME		0x147
#define		CURSOR_UP	0x148
#define		PG_UP		0x149
#define		GREY_MINUS	0x14A
#define		CURSOR_LEFT	0x14B
#define		CURSOR_RIGHT	0x14D
#define		GREY_PLUS	0x14E
#define		END		0x14F
#define		CURSOR_DOWN	0x150
#define		PG_DN		0x151
#define		INS		0x152
#define		DEL		0x153

#define		SHIFT_F1	0x154
#define		SHIFT_F2	0x155
#define		SHIFT_F3	0x156
#define		SHIFT_F4	0x157
#define		SHIFT_F5	0x158
#define		SHIFT_F6	0x159
#define		SHIFT_F7	0x15A
#define		SHIFT_F8	0x15B
#define		SHIFT_F9	0x15C
#define		SHIFT_F10	0x15D


#define		CTRL_F1		0x15E
#define		CTRL_F2		0x15F
#define		CTRL_F3		0x160
#define		CTRL_F4		0x161
#define		CTRL_F5		0x162
#define		CTRL_F6		0x163
#define		CTRL_F7		0x164
#define		CTRL_F8		0x165
#define		CTRL_F9		0x166
#define		CTRL_F10	0x167

#define		ALT_F1		0x168
#define		ALT_F2		0x169
#define		ALT_F3		0x16A
#define		ALT_F4		0x16B
#define		ALT_F5		0x16C
#define		ALT_F6		0x16D
#define		ALT_F7		0x16E
#define		ALT_F8		0x16F
#define		ALT_F9		0x170
#define		ALT_F10		0x171

#define		F11		0x185
#define		F12		0x186
#define		SHIFT_F11 	0x187
#define		SHIFT_F12	0x188
#define		CTRL_F11 	0x189
#define		CTRL_F12	0x18A
#define		ALT_F11 	0x18B
#define		ALT_F12		0x18C

#define		CTRL_CURSOR_UP		0x18D
#define		CTRL_CURSOR_DOWN	0x191
#define		CTRL_CURSOR_LEFT	0x173
#define		CTRL_CURSOR_RIGHT	0x174
#define		CTRL_END		0x175
#define		CTRL_PG_DN		0x176
#define		CTRL_HOME		0x177
#define		CTRL_PG_UP		0x184
#define		CTRL_TAB		0x194
#define		CTRL_INS		0x1A7

#define		ALT_TOGGLE		0x1FF	/* A psuedo-key returned by */
						/* DialogGetEvent()	    */
#define		ALT_PSUEDO_SCANCODE	0xFF00  // Low level value	

#define		ALT_ESC			0x101
#define		ALT_CURSOR_DOWN		0x1A0
#define		ALT_CURSOR_UP		0x198
#define		ALT_SPACE		0x139
#define		ALT_MINUS		0x10C
#define		ALT_BS			0x1A6

#define		SHIFT_INS		0x1A8
#define		SHIFT_BS		0x1A9
#define		SHIFT_DEL		0x1AA
#define		SHIFT_UP		0x1AB
#define		SHIFT_DOWN		0x1AC
#define		SHIFT_RIGHT		0x1AD
#define		SHIFT_LEFT		0x1AE

#define		CTRL_ALT_F10		0x1B0
#define		CTRL_ALT_F9		0x1AF

#define		CTRL_SLASH		0x1B1

#define		SHIFT_HOME      	0x1B2
#define		SHIFT_END             	0x1B3
#define		SHIFT_PG_UP            	0x1B4
#define		SHIFT_PG_DN            	0x1B5
#define		SHIFT_CENTER           	0x1B6

#define		CTRL_GREY_MINUS		0x1B7
#define		CTRL_GREY_PLUS		0x1B8
                
#define         CURSOR_CENTER           0x0F0


#endif /* End of file */


