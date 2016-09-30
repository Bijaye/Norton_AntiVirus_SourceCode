/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   V:/INCLUDE/VCS/TYPES.H_V   1.31   30 Oct 1992 22:51:36   JOHN  $ *
 *                                                                      *
 * Description:                                                         *
 *   Norton Library global types, constants, and other good stuff.      *
 *                                                                      *
 ************************************************************************
 * $Log:   V:/INCLUDE/VCS/TYPES.H_V  $ *
 * 
 *    Rev 1.31   30 Oct 1992 22:51:36   JOHN
 * File cleanup
 * 
 *    Rev 1.30   30 Oct 1992 15:01:44   HENRI
 * Check file in prior to build.  No idea what changed!
 *
 *    Rev 1.29   03 Aug 1992 14:25:46   CRAIG
 * Moved typedef of Handle from VMM.H to TYPES.H.
 *
 *    Rev 1.28   19 Mar 1992 11:08:40   MARKK
 * Removed Debugging
 *
 *    Rev 1.27   13 Mar 1992 11:03:02   ED
 * Added FASTCALL
 *
 *    Rev 1.26   01 Mar 1992 18:13:48   PETERD
 * Added (Boolean) cast to FlagTest macro.
 *
 *    Rev 1.25   19 Feb 1992 09:30:04   CRAIG
 * Added SMALL_MODEL, MEDIUM_MODEL, and LARGE_MODEL.
 *
 *    Rev 1.24   07 Feb 1992 16:33:04   JOHN
 * Added GMOUSE_... constants for graphic mouse cursor types.
 *
 *    Rev 1.23   04 Feb 1992 12:21:02   ED
 * Added ShowFunc
 *
 *    Rev 1.22   23 Jan 1992 00:30:42   GREG
 *
 *    Rev 1.21   23 Jan 1992 00:26:40   GREG
 * added typedef for MSC size_t, used by offsetof
 *
 *    Rev 1.20   23 Jan 1992 00:08:56   TORSTEN
 * Added the offsetof macro.
 *
 *    Rev 1.19   21 Jan 1992 14:32:52   TORSTEN
 * Modified FP_SEG and FP_OFF to suppress Lint error message 740.
 *
 *    Rev 1.18   16 Jan 1992 15:21:08   ED
 * Added MK_FP
 *
 *    Rev 1.17   12 Dec 1991 15:57:38   BARRY
 * Added NOERR and ERR.
 *
 *    Rev 1.16   26 Nov 1991 15:39:44   MARKK
 * Added defensive VMM stuff in
 *
 *    Rev 1.15   13 Nov 1991 21:13:28   ED
 * Added FarToNear macro
 *
 *    Rev 1.14   24 Jul 1991 16:12:04   PETERD
 * Added macros to deal with bit "flags".
 *
 *    Rev 1.13   24 Jul 1991 16:07:32   MARKK
 * Added LARGE_DATA, SMALL_DATA, LARGE_CODE, and SMALL_CODE.
 *
 *    Rev 1.12   10 Jul 1991 17:00:56   MARKK
 * Undid MOUSE_EVENT change.
 *
 *    Rev 1.11   10 Jul 1991 09:11:34   MARKK
 * Added ALT_KEY_EVENT and changed MOUSE_EVENT to 0xFFFF
 *
 *    Rev 1.10   03 Jul 1991 10:31:04   MARKK
 * Converted to large model.
 *
 *    Rev 1.9   25 Jun 1991 17:08:10   BARRY
 * No change.
 *
 *    Rev 1.8   25 Jun 1991 09:36:48   MARKK
 * ifdef'd the definition of HUGE
 *
 *    Rev 1.7   25 Jun 1991 09:35:26   MARKK
 * Added HUGE
 *
 *    Rev 1.6   25 Apr 1991 11:34:54   PETERD
 * Added TASK_SWITCH to event types.
 *
 *    Rev 1.5   27 Mar 1991 13:33:32   BRAD
 * Defined VOID as void.
 *
 *    Rev 1.4   22 Mar 1991 15:40:04   JOHN
 * General cleanup.
 *
 *    Rev 1.3   09 Mar 1991 13:56:26   ED
 * Added #ifndefs to TRUE, FALSE, NULL, EOF, FP_OFF, and FP_SEG.  This
 * prevents redefinition errors....
 *
 * Added definition for FAR because I wanted to.
 *
 *    Rev 1.2   06 Mar 1991 17:45:10   BARRY
 * Added EVENTS_NOTRAP_GREYPLUS and EVENTS_NOTRAP_ALT.
 *
 *    Rev 1.1   20 Nov 1990 15:42:02   BRAD
 * Removed SWITCH_CHAR, since we made it a variable
 *
 *    Rev 1.0   29 Aug 1990 11:24:26   DAVID
 * Initial revision.
 ************************************************************************/

#ifndef INCL_TYPES
#define INCL_TYPES

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/

#ifdef	M_I86SM
#define	SMALL_DATA
#define	SMALL_CODE
#define	SMALL_MODEL
#endif

#ifdef	M_I86MM
#define	SMALL_DATA
#define	LARGE_CODE
#define	MEDIUM_MODEL
#endif

#ifdef	M_I86LM
#define	LARGE_DATA
#define	LARGE_CODE
#define	LARGE_MODEL
#endif

/*----------------------------------------------------------------------*
 * Use to avoid "segment lost in conversion" warnings for		*
 * Medium model/Large model code					*
 *----------------------------------------------------------------------*/

#ifdef	SMALL_DATA
#define	FarToNear(fp)		((void *)(unsigned)(unsigned long)(fp))
#else
#define	FarToNear(fp)		(fp)
#endif

/*----------------------------------------------------------------------*
 * NOTE: The two constants TRUE and ERROR must be different, and non-	*
 *	 zero since some returns return one of TRUE, FALSE, or ERROR,	*
 *	 and the difference between TRUE and ERROR is very important.	*
 *----------------------------------------------------------------------*/

#ifndef	TRUE
#define TRUE    	1
#endif

#ifndef	FALSE
#define FALSE   	0
#endif

#ifndef NOERR
#define NOERR 	        0              	/* No error indication          */
#endif

#ifndef ERR
#define	ERR	        (-1)		/* Generic error condition	*/
#endif

#ifndef ERROR
#define	ERROR		-1		/* Error condition		*/
#endif

#define ABORT		-2		/* Abort the operation		*/
#define	WARNING		-3		/* Not quite an error, but...	*/

#define BREAK_OFF       0
#define BREAK_ON        1

#ifndef	NULL
#define NULL            0               /* null ptr                     */
#endif

#define EOS		'\0'		/* End Of String                */

#define	STDIN	        0
#define	STDOUT	        1
#define	STDERR	        2

#define	PARAM_ARG	0		/* Parameter argument		*/
#define	SWITCH_ARG	1		/* A switch argument		*/

#define	READ		0		/* Used by OPEN_FILE, read only	*/
#define	WRITE		1		/* Write only			*/
#define	RW		2		/* Read and write		*/

#ifndef	EOF
#define	EOF		-1		/* Char returned at end of file */
#endif

#define	EOF_Z		26		/* DOS end of file marker	*/

#define	IGNORE_CASE	0		/* Ignore case in string_comp	*/
#define	CASE_SENSE	1		/* Case sensitive string_comp	*/

#define	IS_DEVICE	0x80		/* IOCTL_STATUS flag		*/

#define	PARAGRAPH_SIZE	 16

#define	BASE_10		10
#define	BASE_16		16

/*----------------------------------------------------------------------*
 * The following constants are for the mouse support.			*
 *----------------------------------------------------------------------*/

#define	NO_BUTTON		0
#define	LEFT_BUTTON		1
#define	RIGHT_BUTTON		2
#define	BOTH_BUTTONS		3

#define	UP_ARROW		1	/* Up arrow in a scroll bar	*/
#define	DOWN_ARROW		2	/* Down arrow in a scroll bar	*/

#define GMOUSE_ARROW		1	/* Arrow mouse cursor		*/
#define GMOUSE_HOURGLASS	2	/* Hour glass mouse cursor	*/
#define GMOUSE_IBEAM		3	/* I-Beam mouse cursor		*/

/*----------------------------------------------------------------------*
 * The following define the various types of events that you can post	*
 * or that EventGetNext() will return.					*
 * 									*
 * NOTE: event numbers > 32 are available for applications.		*
 *----------------------------------------------------------------------*/

#define	NULL		0
#define	KEYBOARD	1
#define	EXIT		2
#define	MOUSE_DOWN	3
#define	UPDATE		4
#define	REDRAW		5
#define	ACTIVATE	6
#define	DEACTIVATE	7
#define	EDIT_DONE	8
#define ALT_KEY_TOGGLE	9
#define TASK_SWITCH     10

/*----------------------------------------------------------------------*
 * The following define the special keys that are trapped by            *
 * EventGetNext().                                                      *
 *----------------------------------------------------------------------*/

#define EVENTS_NOTRAP_GREYPLUS  1
#define EVENTS_NOTRAP_ALT       2

#define ALT_KEY_EVENT  0xFFFE
#define MOUSE_EVENT	(-1)            /* Mouse event message	        */

#define BELL	        ( (Byte)  7 )
#define BACKSPACE       ( (Byte)  8 )
#define ENDFILE         ( (Byte) 26 )
#define ESCAPE          ( (Byte) 27 )
#define BREAK           ( (Byte)  3 )
#define SPACEBAR        ( (Byte) ' ')
#define BLANK_CHAR 	SPACEBAR

/*----------------------------------------------------------------------*
 * These constants are used by the directory search routines that work	*
 * in memory.							      	*
 *----------------------------------------------------------------------*/

#define	FULL_MATCH	 0
#define	PARTIAL_MATCH	 1

/*----------------------------------------------------------------------*
 * The following constant defines the search attributes for directory	*
 * listings.  Right now I'm allowing searching of hidden files.  I may	*
 * not want to include hidden files in the final version.		*
 *----------------------------------------------------------------------*/

#define	SEARCH_ATTR	    (SUB_DIR | HIDDEN | SYSTEM)

/*----------------------------------------------------------------------*
 * define the offsetof macro (stolen from Microsoft's STDDEF.H file)    *
 *----------------------------------------------------------------------*/

typedef unsigned int size_t;

#ifndef offsetof
#define offsetof(s,m)	(size_t)&(((s *)0)->m)
#endif

/*----------------------------------------------------------------------*
 * macros to break MS C "far" pointers into their segment and offset	*
 * components (Taken from Microsoft's DOS.H file)	                *
 *----------------------------------------------------------------------*/

#ifndef	FP_SEG
#define FP_SEG(fp)      /*lint -e740 */ \
    (*((unsigned int far *)&(fp) + 1)) \
    /*lint -restore */
#endif

#ifndef	FP_OFF
#define FP_OFF(fp)      /*lint -e740 */ \
    (*((unsigned int far *)&(fp))) \
    /*lint -restore */
#endif

#ifndef MK_FP
#define MK_FP(seg,ofs)  ((void far *) \
         (((unsigned long)(seg) << 16) | (unsigned)(ofs)))
#endif

/*----------------------------------------------------------------------*
 *			     Type Definitions				*
 *----------------------------------------------------------------------*/

typedef	unsigned char Boolean;
typedef unsigned char Byte;
typedef unsigned int  Word;
typedef unsigned long Ulong;
typedef unsigned int  FileHandle;

typedef Word Handle;

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/

#ifndef STATIC
#define STATIC	static
#endif

#ifndef NEAR
#define NEAR	near
#endif

#ifndef FAR
#define	FAR	far
#endif

#ifndef HUGE
#define HUGE	huge
#endif

#ifndef CONST
#define	CONST	const
#endif

#ifndef PASCAL
#define PASCAL	 pascal
#endif

#ifndef CDECL
#define CDECL	 cdecl
#endif

#ifndef FASTCALL
#define FASTCALL _fastcall              /* MSC 6.0 only                    */
#endif

#ifndef VOID
#define VOID	 void
#endif

#ifndef LOCAL
#define LOCAL   static near		/* Use this for local procedures   */
#endif

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/

#ifndef PATH_CHAR
#define PATH_CHAR	'\\'
#endif

#define bin(x)   ((((0##x##L/010000000L)%010L) <<7) + \
                  (((0##x##L/ 01000000L)%010L) <<6) + \
                  (((0##x##L/  0100000L)%010L) <<5) + \
                  (((0##x##L/   010000L)%010L) <<4) + \
                  (((0##x##L/    01000L)%010L) <<3) + \
                  (((0##x##L/     0100L)%010L) <<2) + \
                  (((0##x##L/      010L)%010L) <<1) + \
                   ((0##x##L           %010L) <<0)   \
                 )

/*----------------------------------------------------------------------*
 * This structure is used to hold both product and program version	*
 * strings.								*
 *									*
 * The flag string allows us to TextSearch for all name/version sets.	*
 * It should be set to PROD_VERSION_FLAG for all product names, and	*
 * PROG_VESION_FLAG for all product names.				*
 *									*
 * A VersionRec named PRODUCT_NAME is defined in version.c in the	*
 * language library.  Each program also defines a VersionRec named	*
 * PROGRAM_NAME.							*
 *									*
 * The PRODUCT_NAME info appears in the program's command-line banner	*
 * (MainPrintBanner(), called by MainCheckHelp()).			*
 *									*
 * The PROGRAM_NAME info appears in the help line (HelpLine() and	*
 * the programs display initialization code), and in the help dialog	*
 * index (passed to HelpInit())						*
 *----------------------------------------------------------------------*/

#define PROD_VERSION_FLAG	"Prod_$vflag"
#define PROG_VERSION_FLAG	"Prog_$vflag"

typedef struct
    {
    Byte *flag;			/* PROD_ or PROG_VERSION_FLAG		*/
    Byte *name;			/* e.g. "Norton Utilities", "UnErase"	*/
    Byte *version;		/* e.g. "5.0.12á", "0.32"		*/
    } VersionRec;

/*----------------------------------------------------------------------*
 *----------------------------------------------------------------------*/

extern Word near displayColumns;

#define MIN_COL		0	/* Minimum column value			*/
#define MAX_COL		(displayColumns-1)/* Maximum column value	*/

/*----------------------------------------------------------------------*
 * These macros implement some "flag" operations using bitwise logical  *
 * functions.  The flag varaible can be any integer type.               *
 *----------------------------------------------------------------------*/

#define	FlagTest(f, b)		(Boolean)(((f) & (b)) != 0)
#define	FlagSet(f, b)		((f) |= (b))
#define	FlagClear(f, b)		((f) &= ~(b))
#define	FlagToggle(f, b)	((f) ^= (b))
#define	FlagCopy(f, b, c)	((c) ? FlagSet(f, b) : FlagClear(f, b))

/*----------------------------------------------------------------------*
 * DEBUGGING STUFF							*
 *----------------------------------------------------------------------*/

#ifdef  SHOW_FUNC
#define ShowFunc(s)             DOSPrintStdaux(s"\n")
#else
#define ShowFunc(s)             { }
#endif

/*----------------------------------------------------------------------*
 * Include Windows compatibility stuff					*
 *----------------------------------------------------------------------*/

#ifndef INCL_WINGLUE
#include <winglue.h>
#endif

#endif /* INCL_TYPES */
