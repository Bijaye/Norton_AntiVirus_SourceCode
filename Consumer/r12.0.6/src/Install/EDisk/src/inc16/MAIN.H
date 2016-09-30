/* $Header:   S:/INCLUDE/VCS/MAIN.H_V   1.1   27 Jan 1992 22:37:02   TORSTEN  $ */

/*
 * $Log:   S:/INCLUDE/VCS/MAIN.H_V  $
 * 
 *    Rev 1.1   27 Jan 1992 22:37:02   TORSTEN
 * Changed the #include statements to use angle brackets.
 * 
 *    Rev 1.0   29 Aug 1990 11:23:54   DAVID
 * Initial revision.
 */


#ifndef INCL_TYPES
#include <types.h>
#endif


#ifndef INCL_MAIN
#define	INCL_MAIN

#define	SWITCH_PLUS	101		/* Signal for '+' on switch	*/
#define	SWITCH_MINUS	102		/* Signal for '-' on switch	*/

#define	EXACT		0		/* Exact match mode		*/
#define AT_LEAST	1		/* At least match mode		*/


/*----------------------------------------------------------------------*/
/* SwitchRec								*/
/*	Every switch uses this record.  Associated with a switch is a	*/
/*	mode byte.  This byte can either be:				*/
/*		EXACT - Match the switch exactly.  Nothing extra.	*/
/*		AT_LEAST - Match the switch. Can also have extra bytes	*/
/*			on the end.					*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte *string;
    Byte matchMode;
    } SwitchRec;

#endif

