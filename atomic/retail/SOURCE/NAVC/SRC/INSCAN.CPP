// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/inscan.cpv   1.0   06 Feb 1997 20:56:30   RFULLER  $
//
// NOTE: this code was pulled out of SCN_DTA.CPP in the VIRSCAN project
//       it is engine specific, therefore it belongs in CERTLIB
//
// Description:
// Create INSCAN files.
//
//
// Contains:
//      GetScanData()
//      SortMemorySigs()
//
//************************************************************************
// $Log:   S:/navc/VCS/inscan.cpv  $
// 
//    Rev 1.0   06 Feb 1997 20:56:30   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:21:12   MKEATIN
// Initial revision.
// 
//    Rev 1.0   06 Dec 1996 11:46:14   JBELDEN
// Initial revision.
// 
//    Rev 1.14   27 Jun 1996 16:53:34   DALLEE
// LuigiPlus port:
// Removed second write of the INSCANB header.
// 
//    Rev 1.13   13 Feb 1996 12:33:04   GDZIECI
// Fixed GPF in GetCRCVirscanDat(). (WIN16).
// Since we are compiling for medium memory model and DS != SS, a far ptr must
// be used when taking address of a local variable.
// 
//    Rev 1.12   02 Feb 1996 14:28:48   RSTANEV
// Added Unicode and SYM_NTK support.
//
//    Rev 1.11   11 Jan 1996 17:40:42   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.8   10 Nov 1995 16:12:40   GDZIECI
// INSCAN?.DAT file time set on NetWare (NLM platform only).
//
//    Rev 1.7   22 Jul 1995 14:30:42   DALLEE
// Zero out the inscan header before setting uNumAlgs and writing
// INSCANB.DAT.  Compatibility w/ NAV 3.x and Luigi.
// Still need to port the INTSR creation code, but I felt safer for now
// with the MEMSET one-liner.
//
//    Rev 1.6.1.1   16 Jul 1995 02:05:34   SZIADEH
// added support for INTSRx.dat.
//
//    Rev 1.6   24 May 1995 20:45:36   RSTANEV
// Now CreateInScan() will work properly with different character sets
// when SYM_VXD is defined.
//
//    Rev 1.5   17 May 1995 15:39:08   SZIADEH
// FAR * is required when building for WIN platform in the GetCRCVirscan
// func.
//
//    Rev 1.4   17 May 1995 14:44:34   SZIADEH
// verify virscan.dat before creating the inscans.
//
//    Rev 1.3   21 Mar 1995 20:18:34   DALLEE
// Free memory before calling SortMemorySigs() -- AllocBiggestChunk()
//   takes all available memory in DOS platform causing the sort to fail.
//
//
//    Rev 1.2   15 Mar 1995 03:44:32   JMILLARD
// fix problem with NetWare 3.11
// greatly improve error returns
// fix Watcom whine about unused parameter
//
//    Rev 1.1.1.0   08 Mar 1995 09:50:34   JREARDON
// Branch base for version QAK6
//
//    Rev 1.1   08 Feb 1995 15:54:24   SZIADEH
// replace VirusScanGetDataFile with STRCPY since the path\name is
// being passed in.
//
//    Rev 1.0   02 Feb 1995 16:59:08   SZIADEH
// Initial revision.
//************************************************************************
#include "platform.h"
#include <stdio.h>
#include "xapi.h"
#include "file.h"
#include "ctsn.h"
#include "virscan.h"

// Local Functions Prototypes //
BOOL SortMemorySigs (HFILE hN30M, WORD wMem, WORD  cdCount, WORD tomCount, WORD allCount );
HGLOBAL LOCAL AllocBiggestChunk (DWORD dwMemToAlloc, LPDWORD lpdwMemSize);

MODULE_NAME;

//************************************************************************
// Local data for CreateInScan function:
//************************************************************************

#define MAX_VIRSCAN_BUFFER_SIZE     64000

typedef struct
{
    WORD wVirScanIndex;
    WORD wControl;
    BYTE lpBooBuff[SIZE_BOOTSIG];
} BOOTTYPE;

typedef struct
{
    WORD wVirScanIndex;
    BYTE lpMemBuffer[SIZE_MEM];
} MEMTYPE;

//************************************************************************
WORD GetCRCVirscanDat ( VIRSCANDATHEADER FAR *lpHeader )
{
    WORD wCheckSum;
    int  i;
    LPWORD pwPtr;
    VIRSCANDATHEADER header;


    MEMCPY ( &header, lpHeader, sizeof (VIRSCANDATHEADER) );
    header.wHeaderCheckSum = 0;
    wCheckSum = 0;

    pwPtr = (LPWORD)&header;

    for (i=0; i < sizeof (VIRSCANDATHEADER)/2; i++, pwPtr++)
        wCheckSum += *pwPtr;

    return ( wCheckSum );

}
//************************************************************************
// CreateInScan(LPTSTR lpVirScanPath)
//
// This function creates the INSCANF.DAT, INSCANB.DAT and INSCANM.DAT files
// from the VIRSCAN.DAT and gives them the same date as the VIRSCAN.DAT file.
//
// Parameters:
//      LPTSTR lpVirScanPath;   Path & Filename of VIRSCAN.DAT file.  In OEM
//                              for SYM_VXD!
//
// Returns:
//      TRUE (1) if the INSCAN files were successfully created
//      FALSE (0) on error
//
//
// INSCANX file structures:
//
// INSCANF:                     INSCANB:            INSCANM:
//
//  wAlgEntries                 wBooEntries         wMemEntries
//  wBooEntries                 wEmpty              wEmpty
//  wCrcEntries                 wEmpty              wEmpty
//  wRemEntries                 wEmpty              wEmpty
//  wFixedIndex[64]             bEmpty[128]         bEmpty[128]
//  bAlgSig[17]                 wVirScanIndex1      wVirScanIndex1
//  bAlgSig[17]                 wControlFlags1      bMemSig1[15]
//  ...                         bBooSig1[14]        ...
//  bAlgSig[17]                 ...                 wVirScanIndexN
//  wCrc                        wVirScanIndexN      bMemSigN[15]
//  wCrc                        wControlFlagsN
//  ...                         bBooSigN[14]
//  wCrc
//
//
//************************************************************************
//************************************************************************
BOOL WINAPI CreateInScan(LPTSTR lpVirScanPath)
{
    HGLOBAL         hMem = NULL;
    HPSTR           hpMem;
    DWORD           dwMemActual, dwBufferSize, dwBufferEntries;
#if defined(SYM_VXD)
    char            lpN30[SYM_MAX_PATH*sizeof(WCHAR)];
    char            lpN30F[SYM_MAX_PATH*sizeof(WCHAR)];
    char            lpN30B[SYM_MAX_PATH*sizeof(WCHAR)];
    char            lpN30M[SYM_MAX_PATH*sizeof(WCHAR)];
#else
    TCHAR           lpN30[SYM_MAX_PATH+1];
    TCHAR           lpN30F[SYM_MAX_PATH+1];
    TCHAR           lpN30B[SYM_MAX_PATH+1];
    TCHAR           lpN30M[SYM_MAX_PATH+1];
#endif
    HFILE           hN30, hN30F, hN30B, hN30M;
    FILETIME        fileTime;
    DWORD           dwCount;
    LPN30           lpN30Ptr;
    WORD            wAlg, wBoo, wCrc, wMem, wRem, wTotal;
    MEMTYPE         MemBuffer;
    BOOTTYPE        BooBuffer;
    HEADER          InScanHeader = {0};
    HEADER          InTSRHeader  = {0};

    WORD            wBytesWrote, wError;
    VIRSCANDATHEADER    wVerifyHeader;

    WORD            cdCount, tomCount, allCount;
    WORD            wCRC;
    WORD            wTSRTotal, wTSRAlg, wTSRBoo, wTSRCrc;

    static char   szTestFile[10];
    BOOL       bUseWild = FALSE;

                                        // -----------------------------------
                                        // next obtain the three filenames we
                                        // will be writing to:
                                        // -----------------------------------

    // -- not needed (snz) --  VirusScanGetDatFile(lpN30, sizeof(lpN30) - 1);

#if defined(SYM_VXD)
    STRCPY ( lpN30, lpVirScanPath );
    NameStripFile ( lpN30 );
    NameAppendFile ( lpN30, "INSCANF.DAT" );
    VxDGenericStringConvert ( lpN30, CHARSET_OEM, lpN30F, CHARSET_CURRENT );

    STRCPY ( lpN30, lpVirScanPath );
    NameStripFile ( lpN30 );
    NameAppendFile ( lpN30, "INSCANB.DAT" );
    VxDGenericStringConvert ( lpN30, CHARSET_OEM, lpN30B, CHARSET_CURRENT );

    STRCPY ( lpN30, lpVirScanPath );
    NameStripFile ( lpN30 );
    NameAppendFile ( lpN30, "INSCANM.DAT" );
    VxDGenericStringConvert ( lpN30, CHARSET_OEM, lpN30M, CHARSET_CURRENT );

    VxDGenericStringConvert ( lpVirScanPath, CHARSET_OEM, lpN30, CHARSET_CURRENT );
#else
    STRCPY (lpN30, lpVirScanPath );

    NameReturnFile( lpN30, szTestFile);
    if ( 0 == stricmp("VIRWILD.DAT", szTestFile))
        bUseWild = TRUE;

    STRCPY(lpN30F, lpN30);
    NameStripFile(lpN30F);
    if ( !bUseWild )
      NameAppendFile(lpN30F, _T("INSCANF.DAT"));
    else
      NameAppendFile(lpN30F, _T("INWILDF.DAT"));

    STRCPY(lpN30B, lpN30);
    NameStripFile(lpN30B);
    if ( !bUseWild )
      NameAppendFile(lpN30B, _T("INSCANB.DAT"));
    else
      NameAppendFile(lpN30B, _T("INWILDB.DAT"));

    STRCPY(lpN30M, lpN30);
    NameStripFile(lpN30M);
    if ( !bUseWild )
      NameAppendFile(lpN30M, _T("INSCANM.DAT"));
    else
      NameAppendFile(lpN30M, _T("INWILDM.DAT"));

#endif

    // delete any old files that are hanging around - this is needed
    // only for NetWare 3.11, but shouldn't hurt anybody else

    FileDelete (lpN30F);
    FileDelete (lpN30B);
    FileDelete (lpN30M);
                                        // -----------------------------------
                                        // open each of the output files and
                                        // the input file:
                                        // -----------------------------------

    if ((hN30 = FileOpen (lpN30, READ_ONLY_FILE)) == HFILE_ERROR)
        return(FALSE);

    if (FileRead(hN30, &wVerifyHeader, STARTSIZE) != STARTSIZE ||
        wVerifyHeader.dwVersion != 3)
        {
        FileClose(hN30);

        FileDelete (lpN30F);
        FileDelete (lpN30B);
        FileDelete (lpN30M);

        return(FALSE);
        }
                                        // check for virscan.dat crc
    wCRC = GetCRCVirscanDat ( &wVerifyHeader );
    if ( wCRC !=  wVerifyHeader.wHeaderCheckSum )
        {
        FileClose(hN30);
        FileDelete (lpN30B);
        FileDelete (lpN30F);
        FileDelete (lpN30M);

        return(FALSE);
        }

    if ((hN30F = FileCreate(lpN30F, FA_NORMAL)) == HFILE_ERROR)
        {
        FileClose(hN30);
        return(FALSE);
        }

    if ((hN30B = FileCreate(lpN30B, FA_NORMAL)) == HFILE_ERROR)
        {
        FileClose(hN30);
        FileClose(hN30F);
        FileDelete (lpN30F);
        return(FALSE);
        }

    if ((hN30M = FileCreate(lpN30M, FA_NORMAL)) == HFILE_ERROR)
        {
        FileClose(hN30);
        FileClose(hN30F);
        FileClose(hN30B);
        FileDelete (lpN30F);
        FileDelete (lpN30B);
        return(FALSE);
        }


                                        // -----------------------------------
                                        // allocate as much memory as we
                                        // can/need!  If AllocBiggestChunk
                                        // returns NULL that means that we
                                        // couldn't even allocate sN30 bytes
                                        // of memory!
                                        // -----------------------------------

    hMem = AllocBiggestChunk(MAX_VIRSCAN_BUFFER_SIZE, &dwMemActual);
    if (hMem == NULL)
        {
        FileClose(hN30);
        FileClose(hN30F);
        FileClose(hN30B);
        FileClose(hN30M);

        FileDelete (lpN30F);
        FileDelete (lpN30B);
        FileDelete (lpN30M);

        return(FALSE);
        }

    hpMem = (HPSTR)MemLock(hMem);       // assume that memlock works!

                                        // -----------------------------------
                                        // now initialize our # of entries &
                                        // buffer information
                                        // -----------------------------------

    dwBufferSize = dwBufferEntries = 0;

                                        // -----------------------------------
                                        // seek past the header region in all
                                        // files.  Take care of updating
                                        // headers after all is done
                                        // -----------------------------------

    FileSeek(hN30, STARTSIZE, SEEK_SET);

    FileSeek(hN30F, sHEADER, SEEK_SET);
    FileSeek(hN30B, sHEADER, SEEK_SET);
    FileSeek(hN30M, sHEADER, SEEK_SET);

    wTotal  = wAlg = wBoo = wCrc = wMem = wRem = 0;
    cdCount = tomCount = allCount = 0;
    wTSRTotal = wTSRAlg = wTSRBoo = wTSRCrc = 0;
    wError  = FALSE;

                                        // -----------------------------------
                                        // since the file is guaranteed to be
                                        // in the A->B->C order, this works
                                        // nicely!
                                        // -----------------------------------


    do
    {
                                        // -----------------------------------
                                        // read in as many entries as we can
                                        // and then find out how many we got.
                                        // -----------------------------------

        dwBufferSize =  FileRead(hN30, hpMem, (WORD)dwMemActual);
        dwBufferEntries = (DWORD)(dwBufferSize / sN30);
        lpN30Ptr = (LPN30)hpMem;

        for (dwCount = 0;dwCount < dwBufferEntries && !wError; dwCount++, lpN30Ptr++)
            {
                                        // -----------------------------------
                                        // increment the total # of sigs thus far.
                                        // -----------------------------------
            wTotal++;

            switch (lpN30Ptr->wType)
                {
                case ALGtype:
                    wAlg ++;

                    wBytesWrote = FileWrite(hN30F, &lpN30Ptr->lpSig,SIZE_SIG);
                    if (wBytesWrote != SIZE_SIG)
                        {
                        wError = TRUE;
                        }
                    break;

                case CRCtype:
                    wCrc ++;

                    wBytesWrote = FileWrite(hN30F, &lpN30Ptr->wCrc, sizeof(WORD));
                    if (wBytesWrote != sizeof(WORD))
                        {
                        wError = TRUE;
                        }
                    break;

                case BOOtype:
                                        // -----------------------------------
                                        // only count the boot sig IF the first
                                        // byte is NON-ZERO
                                        // -----------------------------------

                    if (lpN30Ptr->lpSig[0] != 0)
                        wBoo ++;

                    BooBuffer.wVirScanIndex = wTotal;
                    BooBuffer.wControl = lpN30Ptr->wControl;
                    MEMCPY(BooBuffer.lpBooBuff, lpN30Ptr->lpSig, SIZE_BOOTSIG);
                    wBytesWrote = FileWrite(hN30B, &BooBuffer, sizeof(BooBuffer));
                    if (wBytesWrote != sizeof(BooBuffer))
                        {
                        wError = TRUE;
                        }
                    else
                        if (!(lpN30Ptr->wControl2 & NOT_IN_TSR))
                            {
                            wTSRTotal++;

                                        // only count the boot sig IF the
                                        // first byte is NON-ZERO
                            if (lpN30Ptr->lpSig[0] != 0)
                                wTSRBoo++;

                            BooBuffer.wVirScanIndex = wTSRTotal;
                                        // wControl is already set from above
                                        // lpBooBuff is filled from above
                            }
                    break;

                default:
                    wRem++;
                    break;

                }

            if (lpN30Ptr->wFixedID > 0 &&
                lpN30Ptr->wFixedID < SIZE_FIXED)
                    InScanHeader.wFixedIDs[lpN30Ptr->wFixedID] = wTotal;

            if (!(lpN30Ptr->wControl2 & NOT_IN_TSR) &&
                lpN30Ptr->wFixedID > 0 &&
                lpN30Ptr->wFixedID < SIZE_FIXED)
                    InTSRHeader.wFixedIDs[lpN30Ptr->wFixedID] = wTSRTotal;

                                        // -----------------------------------
                                        // check first word of memory sig to
                                        // see if we want to add memsig
                                        // -----------------------------------

            if (lpN30Ptr->lpMem[0] || lpN30Ptr->lpMem[1])
                {
                wMem++;
                MemBuffer.wVirScanIndex = wTotal;
                MEMCPY(MemBuffer.lpMemBuffer, lpN30Ptr->lpMem, SIZE_MEM);
                wBytesWrote = FileWrite(hN30M, &MemBuffer, sizeof(MemBuffer));

                if (wBytesWrote != sizeof(MemBuffer))
                    {
                    wError = TRUE;
                    }
                                        // count each type of memory sig
                if ( lpN30Ptr->lpMem[0] == 0 )
                    {
                    switch ( lpN30Ptr->lpMem[1] )
                      {
                      case MEMORY_413:
                           tomCount++;
                           break;
                      case MEMORY_ALL:
                           allCount++;
                           break;
                      default:           // this is for sigs that start
                           cdCount++;    // with 0xFF see def07.src 0xc48
                      }
                    }
                else
                    {
                    cdCount++;
                    }


                }
            }
    }
    while (dwBufferEntries != 0 && !wError);

    // if there was an error, I can only leave, where I will delete the
    // files I built

    if (wError)
        {
        goto AllDone;
        }
                                        // -----------------------------------
                                        // time to write headers out!
                                        // first lets do the INSCANF header:
                                        // -----------------------------------

    InScanHeader.uNumALGs = wAlg;
    InScanHeader.uNumBOOs = wBoo;
    InScanHeader.uNumCRCs = wCrc;
    InScanHeader.uNumREMs = wRem;

    FileSeek(hN30F, 0, SEEK_SET);
    if (FileWrite(hN30F, &InScanHeader, sHEADER) != sHEADER)
        {
        wError = TRUE;
        goto AllDone;
        }

                                        // Add Boos and Algs to get to
                                        // the point in VIRSCAN.DAT where the
                                        // CRC Defs Start.. So fix it here.
    InTSRHeader.uNumALGs = wTSRAlg;
    InTSRHeader.uNumBOOs = wTSRBoo;
    InTSRHeader.uNumCRCs = wTSRCrc;
    InTSRHeader.uNumREMs = wRem;



                                        // -----------------------------------
                                        // now do the BOO header
                                        // -----------------------------------

    MEMSET(&InScanHeader,0,sHEADER);
    InScanHeader.uNumALGs = wBoo;

    FileSeek(hN30B, 0, SEEK_SET);
    if (FileWrite(hN30B, &InScanHeader, sHEADER) != sHEADER)
        {
        wError = TRUE;
        goto AllDone;
        }


    MEMSET(&InTSRHeader,0,sHEADER);
    InTSRHeader.uNumALGs = wTSRBoo;



                                        // -----------------------------------
                                        // now do the INSCANM header
                                        // -----------------------------------

    MEMSET(&InScanHeader,0,sHEADER);
    InScanHeader.uNumALGs = wMem;
    InScanHeader.uNumBOOs = cdCount;    // saving the count of each memory
    InScanHeader.uNumCRCs = tomCount;   // sig type will help in speeding
    InScanHeader.uNumREMs = allCount;   // up memory scanning


    // *** WARNING!!! *** WARNING!!! *** WARNING!!! ***
    // ------------------------------------------------
    // Past this point you may no longer use hMem nor
    // the N30 buffer pointers, since we need to free up
    // memory for SortMemorySigs().

    if ( NULL != hMem )                 // Need to free memory before calling
        {                               // SortMemorySigs().  In DOS platform,
        MemRelease( hMem );             // AllocBiggestChunk() really does
        MemFree( hMem );                // take all available memory...
        hMem = NULL;
        hpMem = NULL;
        lpN30Ptr = NULL;
        }

                                        // sorts memory sigs
    if ( !SortMemorySigs ( hN30M, wMem, cdCount, tomCount, allCount ) )
        {
        wError = TRUE;
        goto AllDone;
        }

    FileSeek(hN30M, 0, SEEK_SET);
    if (FileWrite(hN30M, &InScanHeader, sHEADER) != sHEADER)
        {
        wError = TRUE;
        goto AllDone;
        }

                                        // -----------------------------------
                                        // date all the INSCANX files the same
                                        // as VIRSCAN.DAT now:
                                        // -----------------------------------

    FileGetTime(hN30 , TIME_LASTWRITE, &fileTime);

    FileSetTime(hN30F, TIME_LASTWRITE, &fileTime);
    FileSetTime(hN30B, TIME_LASTWRITE, &fileTime);
    FileSetTime(hN30M, TIME_LASTWRITE, &fileTime);

AllDone:
                                        // -----------------------------------
                                        // time to shut down!
                                        // -----------------------------------

    FileClose(hN30);
    FileClose(hN30F);
    FileClose(hN30B);
    FileClose(hN30M);

#ifdef SYM_NLM
    // On NetWare, file date & time is set using file name, after file is closed,
    if (!wError)
        {
        FileSetTimeByName(lpN30F, TIME_LASTWRITE, &fileTime);
        FileSetTimeByName(lpN30B, TIME_LASTWRITE, &fileTime);
        FileSetTimeByName(lpN30M, TIME_LASTWRITE, &fileTime);
        }
#endif

    if (NULL != hMem)
        {
        MemRelease( hMem );
        MemFree( hMem );
        }
                                        // -----------------------------------
                                        // if we had an error writing to any
                                        // of the files at any time during
                                        // this process, delete all INSCANX
                                        // files.
                                        // -----------------------------------
    if (wError)
        {
        FileDelete (lpN30F);
        FileDelete (lpN30B);
        FileDelete (lpN30M);
        }

    return (wError == 0);

} // End CreateInScan()


/////////////////////////////////////////////////////////////////
//
// SortMemorySigs
//  sorts memory signatures based on type
//
// HFILE hN30M:     file handle to "INSCANM.DAT"
// WORD  wMem:      total number of memory sigs
// WORD  cdCount:   number of CD type sigs
// WORD  tomCount:  number of SCAN_413 type sigs
// WORD  allCount:  number of SCAN_ALL type sigs
//
// return TRUE  on success
//        FALSE otherwise
//
//
/////////////////////////////////////////////////////////////////

BOOL SortMemorySigs (HFILE hN30M, WORD wMem, WORD  cdCount, WORD tomCount,
                                                            WORD allCount )
{
  auto HGLOBAL    hMem, hMem2;
  auto LPBYTE     lpBuffer, lpSort, lpMemSig;
  auto LPBYTE     lpTom, lpCd, lpAll;
  auto UINT       BufferSize, RecordSize;
  auto int        i;

  auto BOOL       bRet = TRUE;


  (void) allCount;

  RecordSize = SIZE_MEM + 2;                  // +2 for for VID
  BufferSize = RecordSize * wMem;

                                        // Primitive overflow check for 16-bit
                                        // platforms when we bust 64K...
  if (RecordSize != BufferSize / wMem)
  {
     return ( FALSE );
  }

                                        // Allocate buffer for unsorted sigs.
  hMem = MemAlloc( GHND, BufferSize );
  if ( hMem == NULL )
     return ( FALSE );
  lpBuffer = (LPBYTE) MemLock ( hMem );

                                        // Allocate buffer for sorted sigs.
  hMem2 = MemAlloc( GHND, BufferSize );
  if ( NULL == hMem2 )
  {
     MemRelease( hMem );
     MemFree( hMem );
     return ( FALSE );
  }
  lpSort   = (LPBYTE) MemLock ( hMem2 );

  ////////////////////////////////////////////////////////
  // skip header size
  // seek and read the file
  ////////////////////////////////////////////////////////
  FileSeek(hN30M, sHEADER, SEEK_SET);

  if ( FileRead ( hN30M, lpBuffer, BufferSize ) != BufferSize )
      {
      MemRelease( hMem );
      MemFree( hMem );

      MemRelease( hMem2 );
      MemFree( hMem2 );

      return ( FALSE );
      }

  /////////////////////////////////////////////////////////////
  // sort buffer
  /////////////////////////////////////////////////////////////
  lpCd     = lpSort;
  lpTom    = lpCd  + cdCount  * RecordSize;
  lpAll    = lpTom + tomCount * RecordSize;

  lpMemSig = lpBuffer;
  for ( i=0; i< wMem; i++ )
      {
      if ( *(lpMemSig+2) == 0 )            // skip 2 for VID
         {
         switch ( *(lpMemSig+3) )          // sig type
             {
             case MEMORY_413:
                  MEMCPY ( lpTom, lpMemSig, RecordSize );
                  lpTom += RecordSize;
                  break;
             case MEMORY_ALL:
                  MEMCPY ( lpAll, lpMemSig, RecordSize );
                  lpAll += RecordSize;
                  break;
             default:                      // this is for sigs that start
                  MEMCPY ( lpCd, lpMemSig, RecordSize );
                  lpCd += RecordSize;
             }

         }
      else
         {
         MEMCPY ( lpCd, lpMemSig, RecordSize );
         lpCd += RecordSize;
         }

      lpMemSig += RecordSize;               // get next record
      }

  ////////////////////////////////////////////////////////////////
  // write sorted buffer back to file
  ////////////////////////////////////////////////////////////////
  FileSeek(hN30M, sHEADER, SEEK_SET);
  if ( FileWrite ( hN30M, lpSort, BufferSize ) != BufferSize )
      {
      bRet = FALSE;
      }

  MemRelease( hMem );
  MemFree( hMem );

  MemRelease( hMem2 );
  MemFree( hMem2 );

  return ( bRet );
}

//************************************************************************
// AllocBiggestChunk(DWORD dwMemToAlloc, DWORD *dwMemSize)
//
// This function attempts to allocate the closest multiple of sN30 bytes to
// dwMemToAlloc bytes.  If it is unable to do so, it will allocate as many
// bytes as it can in multiples of sN30 bytes.  *dwMemSize will be set to
// the actual number of bytes allocated.
//
// Parameters:
//      DWORD   dwMemToAlloc;   Number of bytes to allocate in best case
//      DWORD  *dwMemSize;      Number of bytes actually allocated.
//
//
// Returns:
//      Handle to a block of memory for VIRSCAN.DAT buffering
//      A NULL is returned if error.
//
//************************************************************************
// 7/28/93 CAREY created
// 7/28/93 DALLEE, MemAlloc amount should be dwMaxMem, not dwMemToAlloc.
//************************************************************************

HGLOBAL LOCAL AllocBiggestChunk(DWORD dwMemToAlloc, LPDWORD dwMemSize)
{
    HGLOBAL         hMem;
    DWORD           dwMaxMem;

    // first find out how many chunks we'll need in order to read the whole
    // file in.

    dwMaxMem = dwMemToAlloc / sN30;
    dwMaxMem = (dwMaxMem * sN30) + sN30;
    hMem = NULL;

    do
        {
        hMem = MemAlloc(GHND | GMEM_SHARE,dwMaxMem);
        if (hMem == NULL)
            dwMaxMem -= sN30;

        }
    while (hMem == NULL && dwMaxMem != 0);

    *dwMemSize = dwMaxMem;

    return(hMem);
} // End AllocBiggestChunk()

