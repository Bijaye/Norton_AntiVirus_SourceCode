/**********************************************************************
** Copyright 1993 Symantec, Peter Norton Group
**
** File: l_addlog.c
**
** This file contains the AddToLog() to add a log entry to the activity
** log using logio.c functions.
**
**
** See Also:
** Future Enhancements:
***********************************************************************
** $Header:   S:/SYMALOG/VCS/symaadd.c_v   1.34   01 Jul 1998 10:42:56   TSmith  $
**
** $Log:   S:/SYMALOG/VCS/symaadd.c_v  $
// 
//    Rev 1.34   01 Jul 1998 10:42:56   TSmith
// Added code to ALogAdd and ALogAddEx to conditionally call TlaLogAdd in the
// SYMTLA (Tivoli Logfile Adapter) DLL which creates an entry in an ASCII text
// file in fixed field length length records.
// 
//    Rev 1.33   09 Aug 1996 10:20:14   PVENKAT
// Changed the Error type to Information for non-VirusFound messages.
// 
// 
//    Rev 1.32   06 Aug 1996 11:52:44   PVENKAT
// Fixed 1.  GPF on NT Event log (wrong # of strings was passed).
//       # 61914.  Message is converted to ANSI before NT Event log.
// 
// 
//    Rev 1.31   02 Aug 1996 09:04:40   PVENKAT
// Fixed #62615.  Now we get the current User name in NT Log Entries.
// 
//    Rev 1.30   01 Aug 1996 09:39:56   PVENKAT
// Fixed #62613.  Now Scan Completed, Interrupted type of messages or Warnings
//                 and only Virus found is an Error (with stop sign).
// 
// 
//    Rev 1.29   11 Jul 1996 18:01:28   MZAREMB
// Added another SYM_WIN32 #ifdef.
// 
//    Rev 1.28   11 Jul 1996 15:39:28   PVENKAT
// Added #ifdef SYM_WIN32 for NT only changes.
// 
//    Rev 1.27   13 Jun 1996 14:27:04   PVENKAT
// Added NTEventLog to report events.
// 
//    Rev 1.26   08 Nov 1995 18:08:16   GDZIECI
// Ifdef'd out ALogGetMaxSize() for NLM.
// 
//    Rev 1.25   28 Dec 1994 10:41:50   MARKL
// Added ALogAddEx() for when the date/time must be passed as parameters
// since the current date/time may not be correct for the log entry.
// 
//    Rev 1.24   20 Dec 1994 17:10:12   MARKL
// Fixed where SYMALOG was writing UINTs to the disk.  This doesn't work where
// 16 and 32 bit components have to share the same log file and be able to
// read/write each other's data.  These UINTs were changed to type WORD.
// 
//    Rev 1.23   15 Mar 1994 12:05:08   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.22   09 Mar 1994 20:55:40   BRAD
// Fixed WIN32 warnings/errors
// 
//    Rev 1.21   28 Feb 1994 18:19:14   BRAD
// Changes needed for W32.
// 
//    Rev 1.20   28 Feb 1994 16:20:12   BRAD
// Updated for WIN32.
// 
//    Rev 1.19   12 Oct 1993 16:46:32   PATRICKM
// Baseline from QAK1 branch
// 
//    Rev 1.17.1.3   06 Oct 1993 16:16:10   EHU
// #include <io.h> for SYM_NLM.
// 
//    Rev 1.17.1.2   17 Sep 1993 09:46:58   MFALLEN
// Activity log error checking.
// 
//    Rev 1.17.1.1   15 Sep 1993 20:41:56   BARRY
// Detect errors when closing files and propogate up to caller
// 
//    Rev 1.17.1.0   03 Sep 1993 21:53:50   SCOTTP
// Branch base for version QAK1
// 
//    Rev 1.17   22 Aug 1993 23:53:18   BARRY
// Added MODULE_NAME for VMM debugging
// 
//    Rev 1.16   23 Jul 1993 10:25:54   JMILLARD
// add AnsiOem covers for SYM_NLM as well
**
** 04/20/93 WRL is William R. Lee II (BLEE)
**            - Moved to Quake from NIRVANA:ACTILOG
** 05/04/93 WRL Renamed from l_addlog.c to symaadd.c
**            - Changed entry point from AddToLog() to symALogAdd()
**********************************************************************/
#ifdef SYM_NLM
#include <io.h>                         /* lseek, write              */
#endif

#include "symalogi.h"                   /* Project specific dcls     */
#include "symintl.h"                    /* NSetIntlDate()            */
                                        /* NSetIntlTime()            */
#include "xapi.h"                       /* DOSGetDate DOSGetTime     */
#include "file.h"                       /* READ_WRITE_FILE           */
#include "logio.h"                      /* "Low-level" logging       */
                                        /*    NAVLOG_ERR_MEMORY      */
#include "symarc.h"                     /* for IDS_NAV_NAME          */
#include "symtla.h"                     /* Tivoli Logfile Adapter    */

#if defined( SYM_WIN32 )
                                        /* Function pointer declared */
                                        /* and initialized in        */
                                        /* DLLINIT.C.                */
extern SYMTLA_TLALOGADD lpfnTlaLogAdd;

#endif /* SYM_WIN32 */

BOOL NTEventLog (LPALOGADD lpAddLog);


MODULE_NAME;

#if !defined(SYM_WIN) 

/**********************************************************************
** - DOS and SYM_NLM cover for AnsiToOem                                         **
**********************************************************************/
#define AnsiToOem(a,o)  { STRCPY(o,a); }
#define OemToAnsi(o,a)  { STRCPY(a,o); }

#endif

/**********************************************************************
** - Local macros and prototypes
**********************************************************************/

BOOL LOCAL IsThereDiskSpace (
    HFILE hFile,
    DWORD  dwNumBytes);

#define PrefixField(Type, DataLen, Ptr) ( /* Add field prefix        */\
    *((FIELDLEN FAR *)(Ptr))   = (DataLen)/* Prefix field            */\
                                 + sizeof(FIELDLEN)   \
                                 + sizeof(ALOGFIELD), \
    (Ptr) += sizeof(FIELDLEN),          /* Now ++ (cause WatCom says)*/\
    *((ALOGFIELD FAR *)(Ptr)) = (Type), /* Prefix field              */\
    (Ptr) += sizeof(ALOGFIELD)      )   /* Now ++ (cause WatCom says)*/

#define AddField(Type,DataLen,Data,Ptr) ( /* Add a field to buffer   */\
    PrefixField(Type, DataLen, Ptr),    /* Prefix field              */\
    MEMCPY(Ptr, Data, DataLen),         /* Field value               */\
    (Ptr) += (DataLen)              )   /* Update pointer            */

/*** ALogAdd() ********************************************************
** Add a virus scan record to the activity log.
**
** NOTE:
**      >>>>>>>>>> Look at ALogAddEx() when making any <<<<<<<<<<
**      >>>>>>>>>> modifications to this routine.      <<<<<<<<<<
**
** Return Values:
**
** Future Enhancements:
** - Dont do open and close on each write; do file sharing.
** - Check that ALOGADD size structure is correct.
** - Validate appropriate field values
***********************************************************************
** 04/20/93 WRL Moved local variables more local
**            - combined ++ of uOffset w/usage where convenient
**            - Set Status in any case
**            - Change return from BOOL to INT
** 05/04/93 WRL Changed entry point from AddToLog() to ALogAdd()
** 05/20/93 WRL Write new, binary format.
** 12/28/94 MSL Be sure to look at new ALogAddEx if the date/time
**              needs to be other than the current date/time.
**********************************************************************/
WORD SYM_EXPORT WINAPI ALogAdd(             /* Add Activity log item     */
    LPSTR    lpLogFile,                 /* Path to the log file      */
    BYTE     byRecType,                 /* one of ALOG_FI_xxx        */
    BYTE     byProgType,                /* one of ALOG_PT_xxx        */
    DWORD    FileSize,                  /* Size of file (or 0)       */
    LPALOGADD lpAddLog,                 /* Log Information           */
    DWORD    dwMaxSize)                 /* max size of log if create */
{
#  define MAX_LOGRECORD   1000          /* Max. size of a single record */

    UINT  hFile;                        /* Log file handle           */
    int   Status;                       /* Return value              */
    char  szLogFilePath[ SYM_MAX_PATH ];

                                        // ---------------------------------
                                        // Do the AnsiToOem conversion
                                        // on all the fileds that will
                                        // be written to the log file
                                        // ---------------------------------
    if (lpAddLog->lpAction)
        AnsiToOem (lpAddLog->lpAction, lpAddLog->lpAction);

    if (lpAddLog->lpMessage)
        AnsiToOem (lpAddLog->lpMessage, lpAddLog->lpMessage);

    if (lpAddLog->lpNetAddr)
        AnsiToOem (lpAddLog->lpNetAddr, lpAddLog->lpNetAddr);

    if (lpAddLog->lpPath)
        AnsiToOem (lpAddLog->lpPath, lpAddLog->lpPath);

    if (lpAddLog->lpServer)
        AnsiToOem (lpAddLog->lpServer, lpAddLog->lpServer);

    if (lpAddLog->lpUser)
        AnsiToOem (lpAddLog->lpUser, lpAddLog->lpUser);

    if (lpAddLog->lpVirus)
        AnsiToOem (lpAddLog->lpVirus, lpAddLog->lpVirus);

    if (lpAddLog->lpOS)
        AnsiToOem (lpAddLog->lpOS, lpAddLog->lpOS);


    if ((Status=LogOpen(lpLogFile, READ_WRITE_FILE, &hFile, TRUE, dwMaxSize))
                        == NOERR)
    {
        LPSTR lpMem;

        if (lpMem = MemAllocPtr(GHND, MAX_LOGRECORD)) {
            LPSTR pWalk=lpMem;          /* Ptr to walk lpMem         */
            DWORD FieldFlags=0;

            /**********************************************************
            ** Now we add the mandatory progtype
            **********************************************************/
            PrefixField(ALOG_FT_PROGTYPE, sizeof(byProgType), pWalk);
            *pWalk++ = byProgType;
            FieldFlags |= ALOG_FTB_PROGTYPE;

            /**********************************************************
            ** Now we add the mandatory date
            **********************************************************/
            {
                UINT  uYear, uMonth, uDay;
                DWORD dwEncoded;

                DOSGetDate(&uMonth, &uDay, &uYear);
                dwEncoded = ConvertDateToEncoded(uMonth, uDay, uYear);

                PrefixField(ALOG_FT_DATE, sizeof(dwEncoded), pWalk);
                *((LPDWORD)pWalk) = dwEncoded;    /* This has to be two  */
                pWalk += sizeof(dwEncoded);       /* statements cause of */
                                                  /* WATCOM's compiler.  */
                FieldFlags |= ALOG_FTB_DATE;
            }

            /**********************************************************
            ** Now we add the mandatory time
            **********************************************************/
            {
                UINT  uHour, uMinute, uSecond, uTemp;
                DWORD dwEncoded;

                DOSGetTime(&uHour, &uMinute, &uSecond, &uTemp);
                dwEncoded = ConvertTimeToEncoded(uHour, uMinute, uSecond);

                PrefixField(ALOG_FT_TIME, sizeof(dwEncoded), pWalk);
                *((LPDWORD)pWalk) = dwEncoded;    /* This has to be two  */
                pWalk += sizeof(dwEncoded);       /* statements cause of */
                                                  /* WATCOM's compiler.  */
                FieldFlags |= ALOG_FTB_TIME;
            }

            /**********************************************************
            ** Now we just concentrate on the fields provided by the user
            **********************************************************/
            if (FileSize) {
                PrefixField(ALOG_FT_FILESIZE, sizeof(FileSize), pWalk);

                *((DWORD FAR *)pWalk) = FileSize; /* This has to be two */
                pWalk += sizeof(FileSize);        /* statements cause of */
                                                  /* WATCOM's compiler. */
                FieldFlags |= ALOG_FTB_FILESIZE;
            } // if

            /**********************************************************
            ** Some kinda direction indicator, if a file was being
            ** copied to the server or from the server.
            **********************************************************/
            if (lpAddLog->wDirection) {
                PrefixField(ALOG_FT_DIRECTION, sizeof(lpAddLog->wDirection),
                            pWalk);

                *((WORD FAR *)pWalk) = lpAddLog->wDirection; /* This has to be two */
                pWalk += sizeof(lpAddLog->wDirection);       /* statements cause of */
                                                             /* WATCOM's compiler. */
                FieldFlags |= ALOG_FTB_DIRECTION;
            } // if

            /**********************************************************
            ** Action indicator. What was done on the event
            **********************************************************/
            if (lpAddLog->lpAction && *lpAddLog->lpAction) {
                AddField(ALOG_FT_ACTION, STRLEN(lpAddLog->lpAction),
                         lpAddLog->lpAction, pWalk);
                FieldFlags |= ALOG_FTB_ACTION;
            } // if

            /**********************************************************
            ** Some message that the user should see, like "Call Symantec"
            **********************************************************/
            if (lpAddLog->lpMessage && *lpAddLog->lpMessage) {
                AddField(ALOG_FT_MESSAGE, STRLEN(lpAddLog->lpMessage),
                         lpAddLog->lpMessage, pWalk);
                FieldFlags |= ALOG_FTB_MESSAGE;
            }

            /**********************************************************
            ** Network address of the user who caused this log to be written
            **********************************************************/
            if (lpAddLog->lpNetAddr && *lpAddLog->lpNetAddr) {
                AddField(ALOG_FT_NETWORK, STRLEN(lpAddLog->lpNetAddr),
                         lpAddLog->lpNetAddr, pWalk);
                FieldFlags |= ALOG_FTB_NETWORK;
            }

            /**********************************************************
            ** Name of a file or drive
            **********************************************************/
            if (lpAddLog->lpPath && *lpAddLog->lpPath) {
                AddField(ALOG_FT_PATH, STRLEN(lpAddLog->lpPath),
                         lpAddLog->lpPath, pWalk);
                FieldFlags |= ALOG_FTB_PATH;
            }

            /**********************************************************
            ** Name of a network server
            **********************************************************/
            if (lpAddLog->lpServer && *lpAddLog->lpServer) {
                AddField(ALOG_FT_SERVER, STRLEN(lpAddLog->lpServer),
                         lpAddLog->lpServer, pWalk);
                FieldFlags |= ALOG_FTB_SERVER;
            }

            /**********************************************************
            ** Name of a network user
            **********************************************************/
            if (lpAddLog->lpUser && *lpAddLog->lpUser) {
                AddField(ALOG_FT_USER, STRLEN(lpAddLog->lpUser),
                         lpAddLog->lpUser, pWalk);
                FieldFlags |= ALOG_FTB_USER;
            }

            /**********************************************************
            ** Name of virus found
            **********************************************************/
            if (lpAddLog->lpVirus && *lpAddLog->lpVirus) {
                AddField(ALOG_FT_VIRUSNAME, STRLEN(lpAddLog->lpVirus),
                         lpAddLog->lpVirus, pWalk);
                FieldFlags |= ALOG_FTB_VIRUSNAME;
            }

            /**********************************************************
            **   Operating System used
            **********************************************************/
            if (lpAddLog->lpOS && *lpAddLog->lpOS) {
                AddField(ALOG_FT_OS, STRLEN(lpAddLog->lpOS),
                         lpAddLog->lpOS, pWalk);
                FieldFlags |= ALOG_FTB_OS;
            }

            /**********************************************************
            ** Terminate record
            **********************************************************/
            *((FIELDLEN FAR *)pWalk) = 0; /* This has to be two      */
            pWalk += sizeof(FIELDLEN);    /* statements cause of     */
                                          /* WATCOM's compiler.      */
            /**********************************************************
            ** if we successfully write the record, then we indicate
            ** that the completion status is successful.
            **********************************************************/

            if (IsThereDiskSpace(GetTrueHandle (hFile), pWalk-lpMem))
                Status = LogRecWrite(hFile, byRecType, FieldFlags,
                                     (WORD)(pWalk-lpMem), lpMem);
            else
                Status = NAVLOG_ERR_NO_SPACE;

                                        /* If the function pointer   */
                                        /* for the Tivoli logfile    */
                                        /* functionality is valid,   */
                                        /* prepare the log file path */
                                        /* and call the function.    */
#if defined( SYM_WIN32 )

            if ( lpfnTlaLogAdd != NULL )
                {
                STRCPY( szLogFilePath, lpLogFile );
                *( STRRCHR( szLogFilePath, '\\' ) ) = '\0';
                lpfnTlaLogAdd( lpMem, byRecType, byProgType, FieldFlags,
                               MAX_LOGRECORD, szLogFilePath );
                }

#endif /* SYM_WIN32 */

            MemFreePtr(lpMem);
        } // if
        else
            Status = ALOGERR_MEMORY;

        // &? Both LogRecWrite and LogClose return NAVLOG_ERR_ values
        //    but ALogAdd normally returns ALOGERR_ values or NOERR

        if ( NOERR == Status )
            Status = LogClose ( hFile ) ;
        else
            LogClose ( hFile ) ;
    } // if

#ifdef   SYM_WIN32                     // only on NT
    NTEventLog (lpAddLog);
#endif    
    
    return (Status);
} // ALogAdd()


#ifndef SYM_NLM
/*** ALogAddEx() ******************************************************
** Add a virus scan record to the activity log when the date/time needs
** to be something other than the current date/time.
**
** NOTE:
**      >>>>>>>>>> Look at ALogAdd() when making any <<<<<<<<<<
**      >>>>>>>>>> modifications to this routine.    <<<<<<<<<<
**
** Return Values:
**
** Future Enhancements:
** - Dont do open and close on each write; do file sharing.
** - Check that ALOGADD size structure is correct.
** - Validate appropriate field values
***********************************************************************
** 12/28/94 MSL Function created cut/paste style from ALogAdd()
**********************************************************************/
WORD SYM_EXPORT WINAPI ALogAddEx(       /* Add Activity log item     */
    LPSTR    lpLogFile,                 /* Path to the log file      */
    BYTE     byRecType,                 /* one of ALOG_FI_xxx        */
    BYTE     byProgType,                /* one of ALOG_PT_xxx        */
    DWORD    FileSize,                  /* Size of file (or 0)       */
    LPALOGADD lpAddLog,                 /* Log Information           */
    DWORD    dwMaxSize,                 /* max size of log if create */
   DWORD    dwEncodedDate,              /* date to write to log      */
   DWORD    dwEncodedTime)              /* time to write to log      */
{
#  define MAX_LOGRECORD   1000          /* Max. size of a single record */

    UINT  hFile;                        /* Log file handle           */
    int   Status;                       /* Return value              */
    char  szLogFilePath[ SYM_MAX_PATH ];

                                        // ---------------------------------
                                        // Do the AnsiToOem conversion
                                        // on all the fileds that will
                                        // be written to the log file
                                        // ---------------------------------
    if (lpAddLog->lpAction)
        AnsiToOem (lpAddLog->lpAction, lpAddLog->lpAction);

    if (lpAddLog->lpMessage)
        AnsiToOem (lpAddLog->lpMessage, lpAddLog->lpMessage);

    if (lpAddLog->lpNetAddr)
        AnsiToOem (lpAddLog->lpNetAddr, lpAddLog->lpNetAddr);

    if (lpAddLog->lpPath)
        AnsiToOem (lpAddLog->lpPath, lpAddLog->lpPath);

    if (lpAddLog->lpServer)
        AnsiToOem (lpAddLog->lpServer, lpAddLog->lpServer);

    if (lpAddLog->lpUser)
        AnsiToOem (lpAddLog->lpUser, lpAddLog->lpUser);

    if (lpAddLog->lpVirus)
        AnsiToOem (lpAddLog->lpVirus, lpAddLog->lpVirus);

    if (lpAddLog->lpOS)
        AnsiToOem (lpAddLog->lpOS, lpAddLog->lpOS);


    if ((Status=LogOpen(lpLogFile, READ_WRITE_FILE, &hFile, TRUE, dwMaxSize))
                        == NOERR)
    {
        LPSTR lpMem;

        if (lpMem = MemAllocPtr(GHND, MAX_LOGRECORD)) {
            LPSTR pWalk=lpMem;          /* Ptr to walk lpMem         */
            DWORD FieldFlags=0;

            /**********************************************************
            ** Now we add the mandatory progtype
            **********************************************************/
            PrefixField(ALOG_FT_PROGTYPE, sizeof(byProgType), pWalk);
            *pWalk++ = byProgType;
            FieldFlags |= ALOG_FTB_PROGTYPE;

            /**********************************************************
            ** Now we add the mandatory date
            **********************************************************/
            {
                PrefixField(ALOG_FT_DATE, sizeof(dwEncodedDate), pWalk);
                *((LPDWORD)pWalk) = dwEncodedDate; /* This has to be two  */
                pWalk += sizeof(dwEncodedDate);    /* statements cause of */
                                                   /* WATCOM's compiler.  */
                FieldFlags |= ALOG_FTB_DATE;
            }

            /**********************************************************
            ** Now we add the mandatory time
            **********************************************************/
            {
                PrefixField(ALOG_FT_TIME, sizeof(dwEncodedTime), pWalk);
                *((LPDWORD)pWalk) = dwEncodedTime; /* This has to be two  */
                pWalk += sizeof(dwEncodedTime);    /* statements cause of */
                                                   /* WATCOM's compiler.  */
                FieldFlags |= ALOG_FTB_TIME;
            }

            /**********************************************************
            ** Now we just concentrate on the fields provided by the user
            **********************************************************/
            if (FileSize) {
                PrefixField(ALOG_FT_FILESIZE, sizeof(FileSize), pWalk);

                *((DWORD FAR *)pWalk) = FileSize; /* This has to be two */
                pWalk += sizeof(FileSize);        /* statements cause of */
                                                  /* WATCOM's compiler. */
                FieldFlags |= ALOG_FTB_FILESIZE;
            } // if

            /**********************************************************
            ** Some kinda direction indicator, if a file was being
            ** copied to the server or from the server.
            **********************************************************/
            if (lpAddLog->wDirection) {
                PrefixField(ALOG_FT_DIRECTION, sizeof(lpAddLog->wDirection),
                            pWalk);

                *((WORD FAR *)pWalk) = lpAddLog->wDirection; /* This has to be two */
                pWalk += sizeof(lpAddLog->wDirection);       /* statements cause of */
                                                             /* WATCOM's compiler. */
                FieldFlags |= ALOG_FTB_DIRECTION;
            } // if

            /**********************************************************
            ** Action indicator. What was done on the event
            **********************************************************/
            if (lpAddLog->lpAction && *lpAddLog->lpAction) {
                AddField(ALOG_FT_ACTION, STRLEN(lpAddLog->lpAction),
                         lpAddLog->lpAction, pWalk);
                FieldFlags |= ALOG_FTB_ACTION;
            } // if

            /**********************************************************
            ** Some message that the user should see, like "Call Symantec"
            **********************************************************/
            if (lpAddLog->lpMessage && *lpAddLog->lpMessage) {
                AddField(ALOG_FT_MESSAGE, STRLEN(lpAddLog->lpMessage),
                         lpAddLog->lpMessage, pWalk);
                FieldFlags |= ALOG_FTB_MESSAGE;
            }

            /**********************************************************
            ** Network address of the user who caused this log to be written
            **********************************************************/
            if (lpAddLog->lpNetAddr && *lpAddLog->lpNetAddr) {
                AddField(ALOG_FT_NETWORK, STRLEN(lpAddLog->lpNetAddr),
                         lpAddLog->lpNetAddr, pWalk);
                FieldFlags |= ALOG_FTB_NETWORK;
            }

            /**********************************************************
            ** Name of a file or drive
            **********************************************************/
            if (lpAddLog->lpPath && *lpAddLog->lpPath) {
                AddField(ALOG_FT_PATH, STRLEN(lpAddLog->lpPath),
                         lpAddLog->lpPath, pWalk);
                FieldFlags |= ALOG_FTB_PATH;
            }

            /**********************************************************
            ** Name of a network server
            **********************************************************/
            if (lpAddLog->lpServer && *lpAddLog->lpServer) {
                AddField(ALOG_FT_SERVER, STRLEN(lpAddLog->lpServer),
                         lpAddLog->lpServer, pWalk);
                FieldFlags |= ALOG_FTB_SERVER;
            }

            /**********************************************************
            ** Name of a network user
            **********************************************************/
            if (lpAddLog->lpUser && *lpAddLog->lpUser) {
                AddField(ALOG_FT_USER, STRLEN(lpAddLog->lpUser),
                         lpAddLog->lpUser, pWalk);
                FieldFlags |= ALOG_FTB_USER;
            }

            /**********************************************************
            ** Name of virus found
            **********************************************************/
            if (lpAddLog->lpVirus && *lpAddLog->lpVirus) {
                AddField(ALOG_FT_VIRUSNAME, STRLEN(lpAddLog->lpVirus),
                         lpAddLog->lpVirus, pWalk);
                FieldFlags |= ALOG_FTB_VIRUSNAME;
            }

            /**********************************************************
            **   Operating System used
            **********************************************************/
            if (lpAddLog->lpOS && *lpAddLog->lpOS) {
                AddField(ALOG_FT_OS, STRLEN(lpAddLog->lpOS),
                         lpAddLog->lpOS, pWalk);
                FieldFlags |= ALOG_FTB_OS;
            }

            /**********************************************************
            ** Terminate record
            **********************************************************/
            *((FIELDLEN FAR *)pWalk) = 0; /* This has to be two      */
            pWalk += sizeof(FIELDLEN);    /* statements cause of     */
                                          /* WATCOM's compiler.      */
            /**********************************************************
            ** if we successfully write the record, then we indicate
            ** that the completion status is successful.
            **********************************************************/

            if (IsThereDiskSpace(GetTrueHandle (hFile), pWalk-lpMem))
                Status = LogRecWrite(hFile, byRecType, FieldFlags,
                                     (WORD)(pWalk-lpMem), lpMem);
            else
                Status = NAVLOG_ERR_NO_SPACE;

                                        /* If the function pointer   */
                                        /* for the Tivoli logfile    */
                                        /* functionality is valid,   */
                                        /* prepare the log file path */
                                        /* and call the function.    */
#if defined( SYM_WIN32 )

            if ( lpfnTlaLogAdd != NULL )
                {
                STRCPY( szLogFilePath, lpLogFile );
                *( STRRCHR( szLogFilePath, '\\' ) ) = '\0';
                lpfnTlaLogAdd( lpMem, byRecType, byProgType, FieldFlags,
                               MAX_LOGRECORD, szLogFilePath );
                }

#endif /* SYM_WIN32 */

            MemFreePtr(lpMem);
        } // if
        else
            Status = ALOGERR_MEMORY;

        // &? Both LogRecWrite and LogClose return NAVLOG_ERR_ values
        //    but ALogAdd normally returns ALOGERR_ values or NOERR

        if ( NOERR == Status )
            Status = LogClose ( hFile ) ;
        else
            LogClose ( hFile ) ;
    } // if

    return (Status);
} // ALogAddEx()
#endif

BOOL LOCAL IsThereDiskSpace (HFILE hFile, DWORD dwNumBytes)
{
    #define TESTSTR " "
    auto    BOOL    bStatus = TRUE;
    auto    DWORD   dwSaveOffset;
    auto    DWORD   dwOffset;

    dwSaveOffset = FileSeek(hFile, 0L, SEEK_CUR);

    dwOffset = FileSeek(hFile, 0L, SEEK_END);

    FileSeek (hFile, dwOffset + dwNumBytes, SEEK_SET);

    if (FileWrite (hFile, TESTSTR, sizeof(TESTSTR)) == sizeof(TESTSTR))
        {
        FileSeek (hFile, dwOffset, SEEK_SET);
        FileWrite (hFile, "", 0);
        }
    else
        bStatus = FALSE;

    FileSeek (hFile, dwSaveOffset, SEEK_SET);

    return (bStatus);
}

#ifdef   SYM_WIN32                     // only on NT
#define     MAX_STRS             (10)
#define     ID_CATEGORY_ERROR    (0x0001)
#define     ID_CATEGORY_WARNING  (0x0002)
#define     ID_LOG_NAV_MSG1      (0x1001)
#define     ID_LOG_NAV_MSG2      (0x1002)
#define     MAX_SID_VALUE        (128)

//*************************************************************************
//*
//* NTEventLog
//*
//*   To put the events in the NT Event Log.  Scan Results & Infected files
//* 
//*   Parameter:     lpAddLog    LPALOGADD
//*
//*   Returns:       0 on success and nonzero on failure
//*
//*************************************************************************
BOOL NTEventLog (LPALOGADD lpAddLog)
{
    BOOL                        bUserOk = TRUE, bRet = FALSE;
    HANDLE                      hEventSource;
    TCHAR                       szAppName[SYM_MAX_PATH + 1];
    LPTSTR                      lpszStr[MAX_STRS];
    DWORD                       dwErr;
    HANDLE                      hThread;
    TOKEN_USER *                ptuUser = NULL;
    DWORD                       dwTokenUserSize;
    SID_IDENTIFIER_AUTHORITY    siaAuthority = SECURITY_NT_AUTHORITY;
    TCHAR                       szMessage[2 * SYM_MAX_PATH + 1];

    if (!OpenThreadToken (GetCurrentThread (), TOKEN_QUERY, FALSE, &hThread))
    {
        if (GetLastError () == ERROR_NO_TOKEN)
        {
            if (!OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &hThread))
                bUserOk = FALSE;
        }
        else
            bUserOk = FALSE;
    }
    if (bUserOk )
    {
       GetTokenInformation (hThread, 
                                    TokenUser, 
                                    NULL, 
                                    0, 
                                    &dwTokenUserSize);
         // Ignore the return value as this is to find how large the
         // buffer needs to be.
        ptuUser = MemAllocPtr (GPTR, dwTokenUserSize);
        if (ptuUser != NULL)
        {
            if (!GetTokenInformation (hThread, 
                                    TokenUser, 
                                    ptuUser, 
                                    dwTokenUserSize, 
                                    &dwTokenUserSize))
            {
                MemFreePtr (ptuUser);
                ptuUser = NULL;
            }
        }
    }

   LoadString (hDLLInst,
                  IDS_NAV_NAME,
                  szAppName,
                  sizeof (szAppName));


   // Parameters for message.
   lpszStr[0] = lpAddLog->lpMessage;   // What operation 
   lpszStr[1] = lpAddLog->lpAction;    // What action taken.

   // We get only OEM string, so change it to ANSI, NT expects only ANSI.
   STRCPY (szMessage, lpAddLog->lpMessage);
   OemToAnsi (szMessage, szMessage);
   lpszStr[0] = szMessage;


   // Log the event
   hEventSource = RegisterEventSource (NULL, szAppName);
   if (hEventSource)
   {
      WORD  wType;
      WORD  wCategory;
      
      wType = (lpszStr[1] != NULL) ? EVENTLOG_ERROR_TYPE : EVENTLOG_INFORMATION_TYPE;
      wCategory = (lpszStr[1] != NULL) ? ID_CATEGORY_ERROR : ID_CATEGORY_WARNING;
      if (!ReportEvent (hEventSource,
                        wType,
                        wCategory,
                        (lpszStr[1] != NULL) ? ID_LOG_NAV_MSG1 : ID_LOG_NAV_MSG2 ,
                        ptuUser->User.Sid,
                        (WORD )((lpszStr[1] == NULL) ? 1 : 2),
                        0,
                        (LPCTSTR *)lpszStr,
                        NULL))
        {
            bRet = TRUE;
            dwErr = GetLastError ();
        }
        DeregisterEventSource (hEventSource);
   }
   else
        bRet = TRUE;

    if (ptuUser != NULL)
        MemFree (ptuUser);
    return bRet;
}
#endif
