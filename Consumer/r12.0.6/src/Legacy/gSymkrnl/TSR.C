/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/TSR.C_v   1.0   26 Jan 1996 20:22:08   JREARDON  $
 *                                                                      *
 * Description:                                                         *
 *      Routines for manipulating TSRs or Device Drivers from Symantec. *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/TSR.C_v  $
// 
//    Rev 1.0   26 Jan 1996 20:22:08   JREARDON
// Initial revision.
// 
//    Rev 1.4   26 Aug 1994 15:27:24   JRAMLOC
// Change function header EXPORT to SYM_EXPORT on TSRCOMMANDEX.
// 
//    Rev 1.3   26 Aug 1994 11:53:54   JRAMLOC
// Moved Anvil code to Quake 6
// 
//    Rev 1.2   15 Mar 1994 12:32:14   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.1   25 Feb 1994 12:23:12   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
// 
//    Rev 1.0   30 Jul 1993 13:20:34   KEITH
// Initial revision.
// 
//    Rev 1.0   31 May 1993 20:30:26   BRAD
// Initial revision.
 ************************************************************************/

#include "platform.h"
#include "tsr.h"


#pragma optimize("leg", off)    /* Can't global optimize inline assembly */

extern  Do_INT2F_Call();

                                        // Define make long if not already
                                        // defined...
#ifndef MAKELONG
#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#endif


/*----------------------------------------------------------------------*/
/* TSRGetPSP
//	Returns PSP segment if TSR is loaded or 0000H if not		;
/*----------------------------------------------------------------------*/

WORD SYM_EXPORT WINAPI TSRGetPSP(UINT uID)
{
    auto        WORD    wRetValue;

    _asm
        {
        push    si
        push    di

	mov     si, uID
	mov	al, TSR_STATUS
	call	Do_INT2F_Call
        jc      locRet1
	mov	ax,cx			;Return the PSP segment
locRet1:
        mov     wRetValue, ax

        pop     di
        pop     si
        }

    return(wRetValue);
}


/*----------------------------------------------------------------------*/
/* TSRIsEnabled                                                         */
//	Returns TRUE if TSR is enabled, FALSE if it is disabled		;
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI TSRIsEnabled(UINT uID)
{
    auto        BOOL            bRetValue;

    _asm {
        push    si
        push    di

        mov     si, uID
        mov     al, TSR_STATUS
	call	Do_INT2F_Call
	jc	locRet2
	mov	ax,si
	mov	al,ah
	xor	ah,ah
locRet2:
        mov     bRetValue, ax

        pop     di
        pop     si
        }

    return(bRetValue);
}

/*----------------------------------------------------------------------*/
// TSRUpdateConfig
//       Passes a Pointer to a implementation dependent Data Area to     ;
//       the TSR.  The TSR uses this data to (re)configure itself.       ;
//
//      Returns TRUE if enabled, else FALSE.
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI TSRUpdateConfig(UINT uID, LPBYTE lpData)
{
    auto        BOOL            bRetValue;


    _asm {
        push    es
        push    si
        push    di

        mov     si, uID
        mov     al, TSR_STATUS
        les     bx, lpData
        call    Do_INT2F_Call
        jc      locRet3

        mov     ax,si
        mov     al,ah       ;Return "enabled" state
        xor     ah,ah
locRet3:
        mov     bRetValue, ax

        pop     di
        pop     si
        pop     es
        }

    return (bRetValue);
}


/*----------------------------------------------------------------------*/
// TSRGetDataPtr
//       Returns Pointer to TSR's Data Area if TSR is loaded or          ;
//               0000H if not ;                                          ;
/*----------------------------------------------------------------------*/

LPBYTE SYM_EXPORT WINAPI TSRGetDataPtr(UINT uID)
{
    auto        LPBYTE          lpDataArea;

    _asm {
        push    es
        push    si
        push    di

        mov     si, uID
        mov     al, TSR_STATUS
        call    Do_INT2F_Call
        jc      locRet4         ;Error: <dx> = <ax> = 0

        mov     ax,bx           ;Return the pointer in <dx:ax>
        mov     dx,es
locRet4:
        mov     WORD PTR [lpDataArea+2], dx
        mov     WORD PTR [lpDataArea], ax

        pop     di
        pop     si
        pop     es
        }

    return(lpDataArea);
}

/*----------------------------------------------------------------------*/
// TSRGetCtrlPtr
//      Returns Pointer to TSR's Control Area if TSR is loaded or       ;
//              0000H if not ;                                          ;
/*----------------------------------------------------------------------*/

LPBYTE SYM_EXPORT WINAPI TSRGetCtrlPtr(UINT uID)
{
    auto        LPBYTE          lpDataArea;

    _asm {
        push    es
        push    si
        push    di

        mov     si, uID
        mov     al, TSR_STATUS
        call    Do_INT2F_Call
        jc      locRet5         ;Error: <dx> = <ax> = 0

        mov     ax,dx           ;Return the pointer in <dx:ax>
        mov     dx,es
locRet5:
        mov     WORD PTR [lpDataArea+2], dx
        mov     WORD PTR [lpDataArea], ax

        pop     di
        pop     si
        pop     es
        }

    return(lpDataArea);
}

/*----------------------------------------------------------------------*/
// TSRCommand   
//      Called to invoke TSR command return data...                     ;
//              0000H if not ;                                          ;
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI TSRCommand(UINT uID, UINT uCommand)
{
    auto        BOOL            bRetValue;

    _asm {
        push    si
        push    di

        mov     si, uID
        mov     al, BYTE PTR uCommand
	call	Do_INT2F_Call
        mov     bRetValue, ax

        pop     di
        pop     si
        }

    return(bRetValue);
}

/*----------------------------------------------------------------------*/
// TSRCommandEx   
//      Called to invoke TSR command return data...                     ;
//              0000H if not ;                                          ;
/*----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI TSRCommandEx(UINT uID, UINT uCommand, DWORD dwExtra)
{
    auto        WORD    wRetLo = 0;
    auto        WORD    wRetHi = 0;

    // The dwExtra parameter isn't used currently, this is for future
    // use... the TSRCommandEx function was implemented primarily to 
    // change the return value to DWORD from WORD.  

    _asm {
        push    si
        push    di

        mov     si, uID
        mov     al, BYTE PTR uCommand
        call	Do_INT2F_Call
        jnc     cmd_ex_ok
                                        ; An error occured, clear results
        xor     dx, dx
        xor     ax, ax
        mov     si, 0000h

cmd_ex_ok:
                                        ; Return results, retrieve ax from si
        mov     ax,si
        mov     wRetLo, ax
        mov     wRetHi, dx
        
        pop     di
        pop     si
        }

                                        // Form result and return...
    return (MAKELONG(wRetLo, wRetHi));
}


