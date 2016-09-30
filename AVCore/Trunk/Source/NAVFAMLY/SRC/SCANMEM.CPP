////////////////////////////////////////////////////////////
/*@API:*************************************************************************
// @Declaration: GetMemoryAndScanIt ( LPMEMORY_SIG_TYPES lpMemorySigTypes )
//
// @Description:
//   obtains memory buffer, determines it type, and calls
//   appropriate function to scan it for viruses.
//
// @Notes:
//   The functions in this file are NOT alive in the VxD platform.
//
// @Parameters:
//    lpMemorySigTypes: a structure containing virus signatures sorted by type
//         it also includes count of each type.
//
// @Returns:
//    No virus in memory: wVirusId = 0
//    otherwise           wVirusId = the virus id
//
// @Include: avapi.h
//-----------------------------------------------------------------------------
// $Header:   S:/NAVFAMLY/VCS/scanmem.cpv   1.12   08 Feb 1996 18:04:44   RSTANEV  $
//-----------------------------------------------------------------------------
// $Log:   S:/NAVFAMLY/VCS/scanmem.cpv  $
// 
//    Rev 1.12   08 Feb 1996 18:04:44   RSTANEV
// All functions here return 0 when called under SYM_NTK platform.
//
//    Rev 1.11   25 May 1995 15:16:04   SZIADEH
// skip the region A000 to AFFF (VGA Graphics)
//
//    Rev 1.10   30 Mar 1995 17:53:08   DALLEE
// DOS:  No longer uses MemCopyPhysical() into allocated 34k buffer.
//   Passes pointer to real memory to ScanMemoryBuffer() and ScanMemoryCD().
// NLM:  Removed some C++ style code from ScanTopOfMemoryViruses() since
//   NLM platform secretly compiles .cpp's as straight C files.
//
//    Rev 1.9   25 Jan 1995 17:17:12   BRAD
// Added 32-bit VxD and Win32 linear support
//
//    Rev 1.8   19 Jan 1995 17:46:16   MARKL
// Now builds for VxD platform.
//
//    Rev 1.7   28 Dec 1994 17:30:38   SZIADEH
// no code changes.
//-----------------------------------------------------------------------------
*******************************************************************************/

#include "platform.h"
#include "xapi.h"
#include "avapi.h"
#include "symserve.h"           // RelinquishControl

#ifndef SYM_NLM
#define RelinquishControl( )            // empty for non-NLM platforms
#endif

#define  sBufferSize 0xA000   // 34 K of memory Not to exceed
                              // ( 64K - SIZE_MEM )

/*******************************************************************************/
// local prototypes

WORD LOCAL PASCAL ScanTopOfMemoryViruses(
     LPBYTE lpWorkBuffer,
     LPBYTE lpSignatures,
     WORD   wSigsCount );

WORD LOCAL PASCAL ScanRestOfMemory (
           LPBYTE             lpBuffer,
           LPMEMORY_SIG_TYPES lpMemorySigTypes,
           WORD               wSegToStopOn );

/*******************************************************************************/

WORD SYM_EXPORT WINAPI GetMemoryAndScanIt (
     HVCONTEXT hContext,
     LPMEMORY_SIG_TYPES lpMemorySigTypes,
     WORD  wFlags)
{
#if defined(SYM_NTK)
    SYM_ASSERT ( FALSE );
    return ( 0 );
#else
    WORD    wVirusId;
    LPBYTE  lpBuffer;


    (void) hContext;
    (void) wFlags;


#if defined(SYM_WIN32) || defined(SYM_VXD)
                                        // Point to real mode address 0:0
    lpBuffer = (LPBYTE) GetProtModePtr(MAKELP(0, 0));

#elif defined(SYM_DOS) && !defined(SYM_DOSX)
    // Do nothing, lpBuffer value is not used in ScanTopOfMemoryViruses()
    // and ScanRestOfMemory().
#else
    //////////////////////////////////////////////////////////////////
    // allocate memory for the buffer
    //////////////////////////////////////////////////////////////////
    lpBuffer = ( LPBYTE ) VLPermMemoryAlloc(sBufferSize);
    if ( lpBuffer == NULL )
       return ( FALSE );                //&? Should really communicate back
                                        // VS_MEMALLOC error.

    //////////////////////////////////////////////////////////////////
    // take care of the 0:413 stuff
    //////////////////////////////////////////////////////////////////
    MEMSET ( lpBuffer, 0, sBufferSize );  // Zero out the buffer
#endif

    wVirusId = ScanTopOfMemoryViruses( lpBuffer,
                              lpMemorySigTypes->lpTOM,
                              lpMemorySigTypes->TOMCount
                            );

    //////////////////////////////////////////////////////////////////
    // do the rest
    //////////////////////////////////////////////////////////////////
#if defined(SYM_WIN32) || defined(SYM_VXD)
#elif defined(SYM_DOS) && !defined(SYM_DOSX)
#else
    MEMSET ( lpBuffer, 0, sBufferSize );  // Zero out the buffer
#endif
    if (!wVirusId )
        wVirusId = ScanRestOfMemory (
                              lpBuffer,
                              lpMemorySigTypes,
                              wFlags
                            );


#if defined(SYM_WIN32) || defined(SYM_VXD)
    // No cleanup required since SelectorFree() is a NOP for SYM_W32 and VXD,
    // although it might be nice since it is required when GetProtModePtr()
    // is used in SYM_PROTMODE.
#elif defined(SYM_DOS) && !defined(SYM_DOSX)
    // No cleanup.
#else
    VLPermMemoryFree( lpBuffer );
#endif

    return ( wVirusId );
#endif
}

//////////////////////////////////////////////////////////////////
// ScanTopOfMemoryViruses
//////////////////////////////////////////////////////////////////
WORD LOCAL PASCAL ScanTopOfMemoryViruses(
     LPBYTE lpWorkBuffer,
     LPBYTE lpSignatures,
     WORD   wSigsCount )
{
#if defined(SYM_NTK)
    SYM_ASSERT ( FALSE );
    return ( 0 );
#else
    WORD    wVirusId;
    WORD    wTom = 0;
    WORD    wSeg;

                                        // Get the TOM
    MemCopyPhysical (&wTom, MAKELP(0, 0x413), sizeof(WORD), 0);
    wTom *= 64;                         // Convert from K to segment value

    if ( (0 == wTom) || (0xA000 < wTom) )
        {                               // this means that location 0x413 isn't
        wTom = 0x8000;                  // accessible, or is just plain wrong.
        }

    wSeg = wTom;

#if defined(SYM_WIN32) || defined(SYM_VXD)

    UINT uSizeOfBuffer = (0xA000 - wSeg) * 16L;

    lpWorkBuffer += (wSeg * 16L);
    if ( wVirusId = ScanMemoryBuffer (lpWorkBuffer,
                                      uSizeOfBuffer,
                                      lpSignatures,
                                      wSigsCount,
                                      SIZE_MEM - 2,
                                      sizeof(WORD) * 2
                                     ) )
        {
        return ( wVirusId );
        }

#else
                                        // Process in 32K increments
    for ( ; wSeg < 0xA000; wSeg += 0x800)
        {
        WORD    wCopyLen;

        wCopyLen = ((wSeg + 0x800) > 0xA000) ? (0xA000 - wSeg) * 16 : 0x8000;
                                        // Copy over 32K or remaining amount
        RelinquishControl( );

 #if defined(SYM_DOS) && !defined(SYM_DOSX)
                                        // Set up real mode DOS pointer.
                                        // Back track SIZE_MEM_OLAP to cover
                                        // overlap buffer.
                                        // Yes, this does mean we scan
                                        // SIZE_MEM_OLAP bytes before TOM.
        if ( wSeg * 16 >= SIZE_MEM_OLAP )
            {
            lpWorkBuffer = (LPBYTE) MAKELP(wSeg - (SIZE_MEM_OLAP + 15) / 16,
                                    SIZE_MEM_OLAP % 16);
            }
        else
            {
            lpWorkBuffer = (LPBYTE) MAKELP(wSeg, 0);
            }
 #else
        MemCopyPhysical (lpWorkBuffer+SIZE_MEM_OLAP, MAKELP(wSeg, 0), wCopyLen, 0);
 #endif

        RelinquishControl( );

        if ( wVirusId = ScanMemoryBuffer (lpWorkBuffer,
                                          wCopyLen + SIZE_MEM_OLAP,
                                          lpSignatures,
                                          wSigsCount,
                                          SIZE_MEM - 2,
                                          sizeof(WORD) * 2
                                         ) )
            {
            RelinquishControl( );

            return ( wVirusId );
            }

        RelinquishControl( );

 #if defined(SYM_DOS) && !defined(SYM_DOSX)
 #else
        MEMCPY (lpWorkBuffer, lpWorkBuffer + wCopyLen, SIZE_MEM_OLAP);
 #endif
        }
#endif

    return ( 0 );
#endif
}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
WORD LOCAL PASCAL ScanRestOfMemory (
           LPBYTE             lpWorkBuffer,
           LPMEMORY_SIG_TYPES lpMemorySigTypes,
           WORD               wFlags)
{
#if defined(SYM_NTK)
    SYM_ASSERT ( FALSE );
    return ( 0 );
#else
    WORD        wVirusId;

#if defined(SYM_WIN32) || defined(SYM_VXD)

#define FIRST_MEG               0x100000          // top of first megabyte
#define SIZE_BUFFER             0xA0000           // how much to scan on first
                                                  // pass if scanhi is chkd
#define START_SEG_SECOND_BUFFER 0xB000            // where to start second
                                                  // pass if scanhi is chkd
#define SIZE_REGION_SKIPPED     (START_SEG_SECOND_BUFFER * 0x10 - SIZE_BUFFER)
#define SIZE_SECOND_BUFFER      (FIRST_MEG - SIZE_BUFFER - SIZE_REGION_SKIPPED)

    LPBYTE      lpSecondBuffer;
    UINT        uSizeBuffer;


    uSizeBuffer = (wFlags) ? SIZE_BUFFER : 0xA0000;
                                           // scan first region
    if ( wVirusId = ScanMemoryCD (lpWorkBuffer,
                                  uSizeBuffer,
                                  lpMemorySigTypes->lpCD,
                                  lpMemorySigTypes->CDCount,
                                  SIZE_MEM,
                                  sizeof(WORD)
                                 ) )
        {
        return ( wVirusId );
        }

    if ( wVirusId = ScanMemoryBuffer (lpWorkBuffer,
                                      uSizeBuffer,
                                      lpMemorySigTypes->lpALL,
                                      lpMemorySigTypes->ALLCount,
                                      SIZE_MEM - 2,
                                      sizeof(WORD) * 2
                                     ))
        {
        return ( wVirusId );
        }


    if (wFlags)                           // should we scan hi memory?
        {
        lpSecondBuffer = (LPBYTE) GetProtModePtr(MAKELP(START_SEG_SECOND_BUFFER,0));

        if ( wVirusId = ScanMemoryCD (lpSecondBuffer,
                                      SIZE_SECOND_BUFFER,
                                      lpMemorySigTypes->lpCD,
                                      lpMemorySigTypes->CDCount,
                                      SIZE_MEM,
                                      sizeof(WORD)
                                     ) )
            {
            return ( wVirusId );
            }

        if ( wVirusId = ScanMemoryBuffer (lpSecondBuffer,
                                          SIZE_SECOND_BUFFER,
                                          lpMemorySigTypes->lpALL,
                                          lpMemorySigTypes->ALLCount,
                                          SIZE_MEM - 2,
                                          sizeof(WORD) * 2
                                         ))
            {
            return ( wVirusId );
            }
        }

#else

    WORD wSegToStopOn = ( wFlags ? 0 : 0xA000 );   // it wraps around 640K
    WORD wSeg     = 0;
    WORD wCopyLen = 0x8000;             // 32K increments

    do                                  // the reason for a do-loop is that
        {                               // we always want to scan starting
                                        // at segment 0 and stop when wSeg
                                        // is back to 0.
 #if defined(SYM_DOS) && !defined(SYM_DOSX)
                                        // Set up real mode DOS pointer.
                                        // Back track SIZE_MEM_OLAP to cover
                                        // overlap buffer.
        if ( wSeg * 16 >= SIZE_MEM_OLAP )
            {
            lpWorkBuffer = (LPBYTE) MAKELP(wSeg - (SIZE_MEM_OLAP + 15) / 16,
                                           SIZE_MEM_OLAP % 16);
            }
        else
            {
            lpWorkBuffer = (LPBYTE) MAKELP(wSeg, 0);
            }
 #else
        MemCopyPhysical (
            lpWorkBuffer+SIZE_MEM_OLAP, MAKELP(wSeg, 0), wCopyLen, 0);
 #endif

        RelinquishControl( );

        if ( wVirusId = ScanMemoryCD (lpWorkBuffer,
                                      wCopyLen + SIZE_MEM_OLAP,
                                      lpMemorySigTypes->lpCD,
                                      lpMemorySigTypes->CDCount,
                                      SIZE_MEM,
                                      sizeof(WORD)
                                     ) )
            {
            RelinquishControl( );

            return ( wVirusId );
            }

        RelinquishControl( );

        if ( wVirusId = ScanMemoryBuffer (lpWorkBuffer,
                                          wCopyLen + SIZE_MEM_OLAP,
                                          lpMemorySigTypes->lpALL,
                                          lpMemorySigTypes->ALLCount,
                                          SIZE_MEM - 2,
                                          sizeof(WORD) * 2
                                         ))
            {
            RelinquishControl( );

            return ( wVirusId );
            }

        RelinquishControl( );

 #if defined(SYM_DOS) && !defined(SYM_DOSX)
 #else
        MEMCPY ( lpWorkBuffer, lpWorkBuffer + wCopyLen, SIZE_MEM_OLAP );
 #endif

        wSeg += 0x800;                   // add 32
        }
    while ( wSeg != wSegToStopOn );

#endif

    return ( 0 );
#endif
}

// end of file
