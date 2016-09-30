// Copyright 1992-1996 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/navc/VCS/breaker.c_v   1.0   06 Feb 1997 20:56:26   RFULLER  $
//
//***********************************************************************
// $Log:   S:/navc/VCS/breaker.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:26   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:18   MKEATIN
// Initial revision.
// 
//    Rev 1.1   10 Dec 1996 18:41:16   JBELDEN
// implemented code for ctrl-break.
//
//    Rev 1.0   10 Dec 1996 16:48:16   JBELDEN
// Initial revision.
//***********************************************************************

/***************************************************************************
 *
 *      CTRLCB.C - Control-C and Control-Break Handling
 *
 */

#include <dos.h>

#include "platform.h"
#include <dos16.h>

#include "xapi.h"
#include "stddos.h"
#include "nonav.h"
#include "navdprot.h"

void ctrlc_install(), break_install();
extern VOID PASCAL ShutDown(void);
extern  BOOL        gbContinueScan;


struct SREGS sr = {0};                    /* necessary */
union  REGS  r  = {0};                    /* necessary */


void ctrlc_install(void);
void break_install(void);

void interrupt far control_break(ISRREGS ir)
	{
   ErrorLevelSet ( ERRORLEVEL_INTERRUPT );
   gbContinueScan = FALSE;
	}

void interrupt far ctrlC (ISRREGS ir)
	{
   ErrorLevelSet ( ERRORLEVEL_INTERRUPT );
   gbContinueScan = FALSE;
	}

void ctrlc_install()
	{
	char far *q, far *p;

	p = makeptr(mem0_sel, 0x23*4);

	/*  Install ctrlC as passup protected-mode interrupt;
	**  The interrupt is in the auto-passup range, so to make it passup,
	**  all we have to do is use DOS function 25 to install it.
	**  (This part does the real work)
	*/
	q = (void far *) ctrlC;
	r.x.ax = 0x2523;
	r.x.dx = FP_OFF(q);
	sr.ds = FP_SEG(q);
	intdosx(&r, &r, &sr);

	}

void break_install()
	{
	char far *p;
	ISRPTR v, s;
	int ok;

	/* Use d16SetPMVector to install; then use d16SetPassup to mark as passup;
	** Mostly Example test code; comments mark necessary code
	*/

	p = makeptr(mem0_sel, 0x1B*4);
	                        /* pointer to interrupt handler
									** function; necessary
									*/
	FP_SEG(v) = 0;
	/* d16SetPMVector call is necessary */
	d16SetPMVector(0x1B, control_break, (ISRPTR FarPtr) &v);
	ok = d16GetPMVector(0x1B, (ISRPTR FarPtr) &s);
	d16SetPassup(0x1B);  /* d16SetPassup(0x1B) necessary;
							  ** marks interrupt as passup
							  */
	}
