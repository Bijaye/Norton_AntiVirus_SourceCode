// Copyright 1995 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/navxutil/VCS/n30_rw.cpv   1.0   06 Feb 1997 21:05:30   RFULLER  $ 
// 
// Description: Read and Write Nav 3 compatible NAVOPTS files
// 
// Contains:                                                            
// N30_Append
// N30_FindVirus
// N30_NumRecs
// N30_Read
// N30_Open
// N30_Close
// N30_GetComment
// N30_AddComment
// N30_CreateBak
// N30_Delete
// N30_Write
// 
//***************************************************************************
// $Log:   S:/navxutil/VCS/n30_rw.cpv  $ 
// 
//    Rev 1.0   06 Feb 1997 21:05:30   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:56:44   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:48   BARRY
// Initial revision.
// 
//    Rev 1.1   08 Jul 1995 22:19:50   BARRY
// Added C header
//***************************************************************************

#include "platform.h"
#ifdef SYM_WIN
#include "navutil.h"
#include "virscan.h"
#include "file.h"

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

UINT SYM_EXPORT WINAPI N30_Append(UINT hFile, N30 FAR *lpN30)
{
   FileSeek (hFile, 0L, SEEK_END);

   return (FileWrite(hFile, lpN30, sizeof(N30)));
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

UINT SYM_EXPORT WINAPI N30_FindVirus(UINT hFile, WORD wVirID, N30 FAR *lpN30)
{
   UINT uRecNo;
   int  uInRec = -1;
   UINT uNumRecs;

   if ((uNumRecs =  N30_NumRecs(hFile)) > 0)
      {
      for (uRecNo = 0; uRecNo < uNumRecs; uRecNo++) 
         {
         if (N30_Read(hFile, uRecNo, lpN30) == sizeof(N30))
            {
            if (lpN30->wVirID == wVirID)
               {
               uInRec = uRecNo;
               break;
               }
            }
         else
            break;
         }
      }

   return (uInRec);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

UINT SYM_EXPORT WINAPI N30_NumRecs(UINT hFile)
{
    DWORD dwFileSize;
    UINT uNumRecords = 0;

    if ((dwFileSize = FileLength (hFile)) != -1)
       uNumRecords = (UINT)(dwFileSize / sizeof(N30));

    return (uNumRecords);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

UINT SYM_EXPORT WINAPI N30_Read(UINT hFile, UINT uRecNo, N30 FAR *lpN30)
{
   UINT uBytes = 0;
   DWORD dwOff;

   // ********************************************************** //
   // If the record number indicates that the next record should //
   // be read, then just doit without error checking. This is to //
   // improve on speed, but it assumes that the user knows what  //
   // he's doing.                                                //
   // ********************************************************** //

   if (uRecNo == N30_NEXT)
      {
      uBytes = FileRead (hFile, lpN30, sizeof(N30));
      }
   else if (uRecNo <= N30_NumRecs(hFile))
      {
      dwOff = (DWORD)uRecNo * (DWORD)sizeof(N30);
      FileSeek (hFile, dwOff, SEEK_SET);
      uBytes = FileRead (hFile, lpN30, sizeof(N30));
      }

   return (uBytes);
}


#if 0
// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

UINT SYM_EXPORT WINAPI N30_Open(HINSTANCE hInstance)
{
    char  lpFullPath[SYM_MAX_PATH];
    UINT  hFile;
    OFSTRUCT    of;


    N30_GetPath (hInstance, lpFullPath, SYM_MAX_PATH);

    // ***************************************************** //
    // Open file, and if successful, calculate number of N30 //
    // records it contains.                                  //
    // ***************************************************** //
    hFile = FileOpen (lpFullPath, &of, READ_WRITE_FILE);

    return (hFile);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI N30_Close(UINT hFile)
{
    FileClose(hFile);
}
#endif


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI N30_GetComment (LPSTR lpFile, UINT uVirusID, LPSTR lpAlias,
                                   LPSTR lpComment)
{
    auto    DWORD       dwOffset;
    auto    DWORD       dwNoInfo;
    auto    LPSTR       lpMem;
    auto    BOOL        bStatus = TRUE;
    auto    UINT        hFile;
    auto    UINT        uINF_Format;
    auto    UINT        uMax_support;

   // ************************************************************** //
   // Now try to open the complemetary file where we expect it to be //
   // ************************************************************** //

   if ((hFile = FileOpen(lpFile, READ_ONLY_FILE)) != HFILE_ERROR)
      {
      // ******************************************************** //
      // Allocate a chunk of memory big enough to store a comment //
      // and an alias string.                                     //
      // ******************************************************** //

      if ((lpMem = MemAllocPtr (GHND, SIZE_ALIAS + SIZE_COMMENT + 10)) != NULL)
         {

         FileSeek (hFile, SIZE_COPYRIGHT, SEEK_SET);
         FileRead (hFile, &uINF_Format, sizeof(UINT));
         if (uINF_Format != 0)
             return(FALSE);

         FileRead (hFile, &uMax_support, sizeof(UINT));
         FileRead (hFile, &dwNoInfo, sizeof(DWORD));
         if (uVirusID > uMax_support)
             uVirusID = 0;

         FileSeek (hFile, uVirusID * sizeof(WORD), SEEK_CUR);
         FileRead (hFile, lpMem, sizeof(WORD));
         dwOffset = *(UINT FAR *)lpMem;
         dwOffset = (dwOffset * 4) + dwNoInfo;

         FileSeek (hFile, dwOffset, SEEK_SET);

         FileRead (hFile, lpMem, SIZE_ALIAS + SIZE_COMMENT + 2);

         // **************************************************** //
         // First comes the alias field. If its length is longer //
         // than the maximum possible alias field then dump it.  //
         // **************************************************** //
         if (STRLEN(lpMem) <= SIZE_ALIAS)
            {
            OemToAnsi (lpMem, lpAlias);
            lpMem += (STRLEN(lpMem) + 1);

            // *********************************** //
            // The same goes for the comment field //
            // *********************************** //
            if (STRLEN(lpMem) <= SIZE_COMMENT)
                OemToAnsi (lpMem, lpComment);
            else
               {
               lpComment[0] = EOS;
               bStatus = FALSE;
               }
            }
         else
            {
            lpAlias[0] = EOS;
            lpComment[0] = EOS;
            bStatus = FALSE;
            }

         MemFreePtr (lpMem);
         }

      FileClose (hFile);
      }
   else
      bStatus = FALSE;    

   return (bStatus);
}


#if 0

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

DWORD SYM_EXPORT WINAPI N30_AddComment (HINSTANCE hInstance, LPCSTR lpAlias,
                                    LPCSTR lpComment)
{
   UINT hFile;
   DWORD dwOffset = 0;
   BYTE lpN30[SYM_MAX_PATH+1];
   OFSTRUCT     of;

    
   VirScanGetInfFile(lpN30, sizeof(lpWork) - 1);

   if ((hFile = FileOpen (lpN30, &of, READ_WRITE_FILE)) == HFILE_ERROR)
        hFile = FileCreate (lpN30, FA_NORMAL);

   if (hFile)
      {
      dwOffset = FileSeek (hFile, 0L, SEEK_END);
      // ********************************************** //
      // Note that we are writing STRLEN + 1 so the EOS //
      // will be written too.                           //
      // ********************************************** //

      FileWrite (hFile, lpAlias, STRLEN(lpAlias)+1);
      FileWrite (hFile, lpComment, STRLEN(lpComment)+1);
      FileClose (hFile);
      }

   return (dwOffset);
}
#endif

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI N30_CreateBak (HINSTANCE hInstance)
{
   BOOL bStatus = TRUE;
   HGLOBAL hMem;
   LPSTR lpMem;
   UINT uSizeMem = 1000;
   BYTE lpN30[SYM_MAX_PATH+1];

   if ((hMem = MemAlloc (GHND, uSizeMem)) != 0)
        {
        lpMem = MemLock (hMem);

        VirusScanGetDatFile(lpN30, sizeof(lpN30) - 1);

        bStatus = CreateBackup (lpN30, "BAK");

        MemUnlock (hMem, lpMem);
        MemFree (hMem);
        }
    else
        bStatus = FALSE;

   return (bStatus);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI N30_Delete (UINT hFile, UINT uVirID)
{
    HGLOBAL hN30;
    N30 FAR *lpN30;
    UINT uRecNo;
    BOOL bStatus;

    if ((hN30 = MemAlloc(GHND, sizeof(N30))) != 0)
       {
       lpN30 = MemLock (hN30);

       if ((uRecNo = N30_FindVirus(hFile, uVirID, lpN30)) != (UINT)-1)
          {
          lpN30->wType += 4;
          if (!N30_Write (hFile, uRecNo, lpN30))
             bStatus = FALSE;
          }
       else
          bStatus = FALSE;

       MemUnlock (hN30);
       MemFree(hN30);
       }
    else
       bStatus = FALSE;

    return (bStatus);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

UINT SYM_EXPORT WINAPI N30_Write (UINT hFile, UINT uRecNo, N30 FAR *lpN30)
{
   UINT uBytes = 0;
   DWORD dwOff;

   if (uRecNo == N30_NEXT)
      uBytes = FileWrite (hFile, lpN30, sizeof(N30));
   else if (uRecNo <= N30_NumRecs(hFile))
      {
      dwOff = (DWORD) uRecNo * (DWORD)sizeof(N30);
      FileSeek (hFile, dwOff, SEEK_SET);
      uBytes = FileWrite (hFile, lpN30, sizeof(N30));
      }

   return (uBytes);
}
#endif  // (#ifdef SYM_WIN)
