/* Copyright 1993 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/PRM_MACH.C_v   1.0   26 Jan 1996 20:22:04   JREARDON  $ *
 *                                                                      *
 * Description:                                                         *
 *      Determine the current machine ID and class.                     *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/PRM_MACH.C_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:22:04   JREARDON
// Initial revision.
// 
//    Rev 1.16   05 May 1994 13:03:44   BRAD
// Typo
// 
//    Rev 1.15   05 May 1994 12:35:36   BRAD
// DX stuff
// 
//    Rev 1.14   18 Apr 1994 17:00:12   SKURTZ
// Added MemoryCopyROM macro
// 
//
//    Rev 1.13   20 Mar 1994 18:26:18   BRAD
// Fixed up a WIN32 issue
//
//    Rev 1.12   15 Mar 1994 12:32:32   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.11   25 Feb 1994 15:04:22   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.9   15 Oct 1993 07:27:26   ED
// Swan Technologies
//
//    Rev 1.8   14 Oct 1993 15:52:32   ED
// More segments
//
//    Rev 1.7   06 Oct 1993 13:28:30   ED
// Added another ROM offset for detecting Northgates
//
//    Rev 1.6   04 Oct 1993 17:00:14   ED
// Changed ROMSEGMENTS to be F000OFFSET, since it really is a table of
// OFFSETs from F000.  Recast the table to reflect its new role.  Now
// we actually read the right data in protected mode!!!
//
//    Rev 1.5   28 Sep 1993 14:21:46   ED
// Minor update to names
//
//    Rev 1.4   28 Sep 1993 13:11:00   JOHN
// Fixed so it works on DOS and Windows.
//
//    Rev 1.3   15 Sep 1993 12:04:02   ED
// Added new names stolen from Hammer
//
//    Rev 1.2   15 Sep 1993 10:32:16   ED
// Added HWGetROMSamples and HWGetROMSamplesSize, which allows anyone to
// use the same ROM scan samples that HWGetMachineID does.
//
//    Rev 1.1   14 Sep 1993 15:46:22   ED
// Added CompuAdd detection
//
//    Rev 1.0   14 Sep 1993 14:28:40   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "hardware.h"

#ifndef SYM_NLM

BOOL LOCAL PASCAL GetModelID (UINT FAR *lpwModelPtr, UINT FAR *lpwSubModelPtr);
BOOL LOCAL PASCAL IsIBM3270 (void);

#define MATCH_ANY         0xFFFF

typedef struct IBMSIGNATURES
    {
    UINT        wModel;
    UINT        wSubModel;
    UINT        wClass;
    UINT        wID;
    } IBMSIGNATURES;

static IBMSIGNATURES rIBMTypes[] =
  {
    { 0xF8, 0x00,       PS2_IBM,   ID_PS2MODEL80      },
    { 0xF8, 0x01,       PS2_IBM,   ID_PS2MODEL80      },
    { 0xF8, 0x04,       PS2_IBM,   ID_PS2MODEL70_20   },
    { 0xF8, 0x09,       PS2_IBM,   ID_PS2MODEL70_16   },
    { 0xF8, 0x0B,       PS2_IBM,   ID_PS2_LAPTOP      },
    { 0xF8, 0x0C,       PS2_IBM,   ID_PS2_MODEL55SX   },
    { 0xF8, 0x0D,       PS2_IBM,   ID_PS2MODEL70_25   },
    { 0xF8, 0x14,       PS2_IBM,   ID_PS2_MODEL90     },
    { 0xF8, 0x16,       PS2_IBM,   ID_PS2_MODEL90     },
    { 0xF8, 0x1B,       PS2_IBM,   ID_PS2_MODEL70_486 },
    { 0xF8, 0x1C,       PS2_IBM,   ID_PS2_MODEL65     },
    { 0xF8, 0x50,       PS2_IBM,   ID_PS2_LAPTOP      },
    { 0xF8, 0x80,       PS2_IBM,   ID_PS2MODEL80      },
    { 0xF8, MATCH_ANY,  PS2_IBM,   ID_PS2             },
    { 0xFA, 0x00,       PS2_IBM,   ID_PS2MODEL30      },
    { 0xFA, 0x01,       PS2_IBM,   ID_PS2MODEL25      },
    { 0xFA, MATCH_ANY,  PS2_IBM,   ID_PS2MODEL25      },
    { 0xFB, MATCH_ANY,  OLDER_IBM, ID_PCXT            },
    { 0xFC, 0x00,       OLDER_IBM, ID_PCAT            },
    { 0xFC, 0x01,       OLDER_IBM, ID_PCAT            },
    { 0xFC, 0x02,       OLDER_IBM, ID_XT286           },
    { 0xFC, 0x04,       PS2_IBM,   ID_PS2MODEL50      },
    { 0xFC, 0x05,       PS2_IBM,   ID_PS2MODEL60      },
    { 0xFC, 0x09,       PS2_IBM,   ID_PS2MODEL30X     },
    { 0xFC, 0x0B,       OLDER_IBM, ID_PS1             },
    { 0xFC, MATCH_ANY,  OLDER_IBM, ID_PCAT            },
    { 0 }
  };

typedef struct ROMSIGNATURE
    {
    LPBYTE      lpName;                 // rom signature string
    UINT        wId;                    // our machine ID
    } ROMSIGNATURE;

/* Remember to put more general machine     */
/* names at the end of this list.           */

static ROMSIGNATURE rROMSignature[] =
    {
        { "COMPAQ",             ID_COMPQ },
        { "TEXAS INS",          ID_TIPRO },
        { "TANDY",              ID_TANDY },
        { "TANDON",             ID_TANDON },
        { "TTaannddoonn",       ID_TANDON },
        { "TTTTaaaannnnddddoooonnnn", ID_TANDON },
        { "CORONA",             ID_CORON },
        { "MINDSET",            ID_MINDSET },
        { "WANG",               ID_WANG },
        { "MATSUSHITA",         ID_PANSR },
        { "(C)ZDS",             ID_ZENITH },
        { "ZENITH",             ID_ZENITH },
        { "COLUMBIA",           ID_COLUMB },
        { "EPSON",              ID_EPSON },
        { "SHARP",              ID_SHARP },
        { "LEADING T",          ID_LEADINGTECH },
        { "LEADING",            ID_LEAD_EDG },
        { "T3100",              ID_TOSH3100 },
        { "T5200",              ID_TOSH5200 },
        { "TVS",                ID_TELEVIDEO },
        { "MORROW",             ID_MORROW },
        { "WYSE",               ID_WYSE },
        { "TAVA",               ID_TAVA },
        { "HEWLETT",            ID_HP },
        { "EAGLE",              ID_EAGLE },
        { "BYTEC",              ID_HYPERION },
        { "DATA GEN",           ID_DG_BOOK1 },
        { "TOMCAT",             ID_TOMCAT },
        { "MELCO",              ID_MITSUBISHI },
        { "MITSUBI",            ID_SPERRY },
        { "OLIVET",             ID_OLIVETTI },
        { "SPARK",              ID_SPARK },
        { "NORTHGATE",          ID_NORTHGATE },
        { "TOSHIBA",            ID_TOSHIBA },
        { "ACER",               ID_ACER },
        { "AST RESEA",          ID_AST_PREM },
        { "COMPUADD",           ID_COMPUADD },
        { "NEC",                ID_NEC },
        { "GATEWAY",            ID_GATEWAY },           // Gateway 2000
        { "EarthStation",       ID_EARTH },             // Earth Computers
        { "DELL",               ID_DELL },
        { "PC BRAND",           ID_PCBRAND },           // PC Brand
        { "EVEREX",             ID_EVEREX },
        { "Advanced Logic Research",            ID_ALR },
        { "ALR",                ID_ALR },
        { "ERGO",               ID_ERGO },              // Ergo Computing
        { "NCR",                ID_NCR },
        { "AUSTIN",             ID_AUSTIN },
        { "DTK",                ID_DTK },
        { "ZEOS",               ID_ZEOS },
        { "INSIGNIA",           ID_INSIGNIA },          // Insignia Solutions
        { "SWAN TECH",          ID_SWAN },
    };

#define NUM_SIGNATURES (sizeof(rROMSignature) / sizeof(ROMSIGNATURE))

typedef UINT    F000OFFSET;

static F000OFFSET wROMOffsets[] =
    {
    0x0020,             /* HP Vectra */
    0x7600,             /* Mindset */
    0x9C50,             // NEC
    0x9FC0,             // NEC
    0xA1D0,             /* Northgate */
    0xA460,             /* DG-Book1 */
    0xB000,             /* Zenith Data Systems */
    0xC020,             /* Mitsubishi */
    0xC050,             /* Olivetti */
    0xC210,             /* NCR-PC4 */
    0xE000,             /* PS/1 - "IBM" signature */
    0xE070,             // NEC, ALR
    0xE090,             /* AST Premium */
    0xE0C0,             // Swan Technologies
    0xE0D0,             /* Tandy 3000 */
    0xE150,             /* Acer */
    0xE300,             /* Northgate */
    0xE320,             /* Leading Technologies        */
    0xE400,             /* Leading Edge */
    0xF760,             /* Columbia */
    0xF810,             /* Eagle PC Plus */
    0xF9C0,             /* Panasonic Senior Partner */
    0xFC00,             /* Wang */
    0xFF00,             /* NEC APC */
    0xFF70,             /* Spark */
    0xFFA0,             /* Eagle PC */
    0xFFC0,             /* Hyperion */
    0
    };

/*----------------------------------------------------------------------*
 * here is what is copied into POOL, for scanning by this program or
 *   by others:
 *
 *  16 bytes from FFFF:0 (for IBM date and id byte)
 * 100 bytes each from eight areas, F800:0 through FF00:0
 *  50 bytes from FFFC:0  for Hyperion
 *  50 bytes from FA46:0  for Data General One (production)
 *  50 bytes from F760:0  for Mindset
 *  50 bytes from FFFC:0  for Wang
 *  50 bytes from FFF0:0  for NEC
 *  50 bytes from FF9C:0  for Panasonic Sr Partner
 *  50 bytes from FC21:0  for NCR - PC4
 *  50 bytes from FF76:0  for Columbia
 *  50 bytes from FE0D:0  for Tandy 3000, Morrow Pivot and Zenith 171
 *  50 bytes from FE40:0  for Leading Edge 2
 *
 *  Note: IBM id byte will be at POOL[14]; IBM date at POOL[5]
 *
 *  IBM id bytes:
 *  - 3270 PC uses 'strange' test for video interrupt 10h returning
 *        ptrs in CX:DX.
 *
 *       PC-1                  FF
 *       PC-2                  FE
 *       PCjr                  FD
 *       PC-AT                 FC
 *        PC-Convertible  F9
 *        PS/2 Model 30   FA
 *
 *  Compaq    = scan for COMPAQ
 *  TI Pro    = scan for TEXAS
 *  Tandy2000 = scan for TANDY
 *  Corona    = scan for CORONA
 *  DG Book1  = scan for DATA GEN
 *  Mindset   = scan for MINDSET
 *  Wang      = scan for WANG
 *  Columbia  = scan for COLUMBIA
 *  Epson     = scan for EPSON
 *  Tandy3000 = scan for TANDY
 *  Sharp     = scan for SHARP
 *  Toshiba   = scan for TOSHIBA
 *  Lead Edge = scan for LEADING
 */


#define CHUNK_LG        100             // 100-byte chunk
#define CHUNK_SM        50              // 50-byte chunk
#define ROM_END         16              // 16-byte chunk

#define TABLE_SIZE      (sizeof (wROMOffsets) / sizeof (F000OFFSET))

#define SEGMENT_CHUNKS  8
                                        // size of the pool buffer
#define POOL_SIZE       ((TABLE_SIZE * CHUNK_SM) + (SEGMENT_CHUNKS * CHUNK_LG) + ROM_END)

#define MemoryCopyROM(d,s,l)    MemCopyPhysical(d,s,l,0)

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the amount of memory needed to hold the data to be      *
 *      returned in the HWGetROMSamples call.                           *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/15/1993 ED Function Created.                                      *
 ************************************************************************/

UINT SYM_EXPORT WINAPI HWGetROMSamplesSize (void)
{
    return (POOL_SIZE);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Fills the specified buffer with chunks of ROM data, which can   *
 *      then be scanned for identifiers for whatever reason.            *
 *                                                                      *
 *      NOTE: Unless you enjoy living dangerously (and even if you      *
 *      do), call HWGetROMSamplesSize to determine the size of the      *
 *      buffer you will need.                                           *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/15/1993 ED Function Created.                                      *
 ************************************************************************/


UINT SYM_EXPORT WINAPI HWGetROMSamples (LPVOID lpPoolBuffer)
{
    LPBYTE      lpWorkPtr;
    UINT        wOffset;
    WORD        wSrcOffset;
    UINT        i;

    lpWorkPtr = (LPBYTE) lpPoolBuffer;          // cast to a useful value

    MEMSET (lpWorkPtr, 0, POOL_SIZE);

        // -----------------------------------------------------------
        // read the last 16 bytes of the ROM to get the machine ID
        // and ROM date */
        // -----------------------------------------------------------
    MemCopyPhysical(lpWorkPtr, MAKELP(0xF000, 0xFFF0), ROM_END, 0);

    wOffset = ROM_END;

        // -----------------------------------------------------------
        // Read 100-byte chunks of memory at F800, F900, ..., FF00
        // -----------------------------------------------------------

    for (i = 0; i < SEGMENT_CHUNKS; i++)
         {
         wSrcOffset = 0x8000 + (i * 0x1000);

         MemCopyPhysical(lpWorkPtr + wOffset, MAKELP(0xF000, wSrcOffset), CHUNK_LG, 0);

         wOffset += CHUNK_LG;
         }

        // -----------------------------------------------------------
        // Read in 50-byte chunks of memory at locations known to be
        // used by certain machines.
        // -----------------------------------------------------------

    for (i = 0; wROMOffsets [i] != 0; i++)
        {
        wSrcOffset = wROMOffsets [i];

        MemCopyPhysical(lpWorkPtr + wOffset, MAKELP(0xF000, wSrcOffset), CHUNK_SM, 0);

        wOffset += CHUNK_SM;
        }

#ifdef  SYM_WIN
    AnsiUpperBuff (lpPoolBuffer, POOL_SIZE);
#else
    BufferToUpper(FP_SEG(lpPoolBuffer), FP_OFF(lpPoolBuffer), POOL_SIZE);
#endif

    return (NOERR);
}


UINT SYM_EXPORT WINAPI HWGetMachineID (UINT FAR *lpwClass)
{
    LPBYTE      lpPoolBuffer;
    UINT        wModel, wSubModel;
    UINT        wMachineID;
    UINT        wIndex;
    UINT        wSize;
    UINT        i;

    *lpwClass = NON_IBM;                /* Assume a NON - IBM machine        */

    wSize = HWGetROMSamplesSize ();     // how big a buffer do we need?

                                        // get the buffer
    lpPoolBuffer = (LPBYTE) MemAllocPtr (GMEM_MOVEABLE, (DWORD) wSize);

    if (lpPoolBuffer == NULL)
        return (ID_UNKN);

    HWGetROMSamples (lpPoolBuffer);     // get the ROM samples

    wMachineID = ID_UNKN;               // default to an unknown machine

    for (i = 0; i < NUM_SIGNATURES; i++)
        {
        if (MemorySearch (lpPoolBuffer, wSize, rROMSignature [i].lpName,
                                   STRLEN (rROMSignature [i].lpName)))
            {
            wMachineID = rROMSignature [i].wId;
            break;
            }
        }

    if (wMachineID == ID_UNKN)
        {
        if (MemorySearch (lpPoolBuffer, wSize, "IBM", 3))
            {
            // if this is an IBM machine, look at the model ID byte

            if (GetModelID (&wModel, &wSubModel))
                {
                wIndex = 0;

                while (rIBMTypes[wIndex].wModel)
                    {
                    if (wModel == rIBMTypes [wIndex].wModel)
                        {
                        if (wSubModel == rIBMTypes [wIndex].wSubModel ||
                                        rIBMTypes [wIndex].wSubModel == MATCH_ANY)
                            {
                            *lpwClass = rIBMTypes [wIndex].wClass;
                            wMachineID = rIBMTypes [wIndex].wID;
                            return (wMachineID);
                            }
                        }

                    wIndex++;
                    }
                }
            else
                *lpwClass = OLDER_IBM;

            wModel = lpPoolBuffer [14];         /* Get model number         */

            if (wModel == (BYTE) 0xFD)
                wMachineID = ID_PCJR;

            else if (IsIBM3270())               /* test for 3270 case   */
                wMachineID = ID_3270PC;

            else if (wModel == (BYTE) 0xFC)
                wMachineID = ID_PCAT;

            else if (wModel == (BYTE) 0xFF)
                wMachineID = ID_PC1;

            else if (wModel == (BYTE) 0xFE)
                wMachineID = ID_PC2;

            else if (wModel == (BYTE) 0xF9)
                wMachineID = ID_PCCONV;         /* IBM/PC Convertible */
            }
        }

    MemFreePtr (lpPoolBuffer);

    return (wMachineID);
}


BOOL LOCAL PASCAL GetModelID (UINT FAR *lpwModelPtr, UINT FAR *lpwSubModelPtr)
{
   BOOL bInfoAvail;

    _asm
        {
        mov     ah, 0C0h                // Get MACHINE CONFIGURATION
        int     15h

        jc      notInstalled            // No machine info available

        or      ah, ah                  // Double check the availability of
        jnz     notInstalled            //        machine info.

        push    ds
        lds     si, lpwModelPtr         // get the machine type
        mov     al, es:[bx+2]
        mov     byte ptr [si], al

        lds     si, lpwSubModelPtr      // get submodel type
        mov     al, es:[bx+3]
        mov     byte ptr [si], al
        pop     ds

        mov     ax, 1                   // Info available
        jmp     short retMachine

    notInstalled:
        xor     ax, ax                  // No info available

    retMachine:
        mov     bInfoAvail, ax  ;This is redundant ... but is done to get rid
                                ; warning.
        }

   return(bInfoAvail);
}

BOOL LOCAL PASCAL IsIBM3270 (void)
{
   BOOL bIs3270;

    _asm
        {
        mov     ax,3000h                ; service call to get ptr. to MONITOR ID byte.
        xor     cx,cx                   ; clear for testing
        xor     dx,dx                   ; clear for testing
        int     10h                     ; BIOS video services

        or      cx,cx                   ; see if ptr. returned
        jne     IsID                    ; Yes. See if 3270

        or      dx,dx                   ; again see if ptr. returned
        jne     IsID                    ; Yes. See if 3270

        jmp     short IsPC              ; This is an IBM PC

             ; now get MONITOR ID and see if it is 3270 id

    IsID:
        push    ds                      ; save data ptr.
        mov     ds,cx                   ; segment ptr to MONITOR ID
        mov     bx,dx                   ; wOffset ptr to MONITOR ID
        mov     al,byte ptr [bx+2]      ; get the MONITOR ID
        pop     ds                      ; restore data ptr.

        cmp     al,0FFh                 ; is this a PC?
        je      IsPC                    ; Yes

        mov     ax,1                    ; return a value of 1 because it is 3270
        jmp     short GoodID

    IsPC:
        xor     ax,ax                   ; return 0 because it is not 3270

    GoodID:
        mov     bIs3270, ax    ; update return value ... do this to get rid of
                                ; warning.
   }

   return(bIs3270);
}

#endif  // SYM_NLM 
