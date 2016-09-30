// Copyright 1995 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:\symalog\vcs\dumplog.c_v   1.5   12 Mar 1995 22:02:36   JMILLARD  $ 
// 
// Description: utility program to dump an activity log                                                         
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
//
// $Log:   S:\symalog\vcs\dumplog.c_v  $ 
// 
//    Rev 1.5   12 Mar 1995 22:02:36   JMILLARD
// 
// add standard file header, update copyright notice
// update to new logio api
// 
//***************************************************************************

//*********************************************************************
// Dump the contents of an Activity Log file
//*********************************************************************

#include <stdio.h>
#include <process.h>
#include "platform.h"
#include "symalogi.h"
#include "logio.h"
#include "stddos.h"
#include "file.h"                       // File_read

static int DisplayFields(
   LPBYTE Data);                        /* Data                      */
static void Errout(char *funcname, int rc);

//************************************************************************
// DEFINES
//************************************************************************
#define  VMM_HANDLES     100

//************************************************************************
// GLOBAL VARIABLES THE NORTON LIB WANTS
//************************************************************************
VersionRec PROGRAM_NAME =
{
    PROG_VERSION_FLAG,
    "NAVDEF.EXE",
    "1.0a"
};

char PRODUCT_COPYRIGHT[] = "Copyright 1993-1995 by Symantec Corporation";

VersionRec PRODUCT_NAME =
{
    PROD_VERSION_FLAG,
    "Sammi's whatever virus finder."
    "1.0a"
};

// BOOL gbUseInt16 = FALSE;
BOOL bCallSuspendInternal = FALSE;

/**********************************************************************
**
**********************************************************************/
int cdecl main(int argc, char *argv[])
{
    int rc;
    WORD hLog;

    {
        DWORD dwConvMem = ALL_MEMORY;
        DWORD dwEmsMem  = ALL_MEMORY;
        DWORD dwXmsMem  = ALL_MEMORY;
        DWORD dwDiskMem = ALL_MEMORY;

        if (!(rc=VMMInit(VMM_HANDLES,
                         &dwConvMem, &dwEmsMem, &dwXmsMem, &dwDiskMem)))
        {
            printf("Error(%d) in InitMemory()\n",rc);

            return rc;
        }
    }

    ALogioInit();

    rc = ALogOpen(argv[1], READ_ONLY_FILE, &hLog, FALSE, 0);
    Errout("ALogOpen",rc);

    {
        LOGHEADER Hdr;
        rc = ALogHeader(hLog, FALSE, &Hdr);

        printf("Version:         %08lX\n", Hdr.Version);

        printf("First Entry:     %lu (\'%08lX\'x)\n",
               Hdr.dwFirstEntry, Hdr.dwFirstEntry);
        printf("Last Entry:      %lu (\'%08lX\'x)\n",
               Hdr.dwLastEntry, Hdr.dwLastEntry);

        printf("Max size:        %lu (\'%08lX\'x)\n",
               Hdr.dwMaxSize, Hdr.dwMaxSize);
        printf("Max entries:     %u (\'%04X\'x)\n",
               Hdr.wMaxEntries, Hdr.wMaxEntries);
        printf("Current entries: %u (\'%04X\'x)\n",
               Hdr.wCurrentEntries, Hdr.wCurrentEntries);
        printf("Max days:        %u (\'%04X\'x)\n",
               Hdr.wMaxDays, Hdr.wMaxDays);

        Errout("ALogHeader",rc);
    }
    {
//      LOGRECORD Record;
        WORD  Type;
        DWORD Key;
        WORD  Size;
        WORD  CurSize=256;
        LPBYTE lpData;
        int  count=0;

        lpData = MemAllocPtr(GHND, CurSize);
        if (!lpData)
            Errout("MemAllocPtr",NAVLOG_ERR_MEMORY);

        while ( !ALogRecSeek( hLog, count ))
        {
            DWORD offset;
            offset = ALogTell(hLog);

            rc = ALogRecReadHdr(hLog, &Type, &Key, &Size, 0);
            Errout("ALogRecReadHdr",rc);
            if (Size > CurSize) {
                MemFreePtr(lpData);
                if (!(lpData = MemAllocPtr(GHND, CurSize=Size)))
                    Errout("MemAllocPtr", NAVLOG_ERR_MEMORY);
            } /* then */

            printf("%4d: offset %lu (\'%lX\'x)\t", count++, offset, offset);

            printf("Type (%d):",Type);
            switch (Type)
            {
            case ALOG_FI_VIRLISTCHANGE:
                fputs("VIRLISTCHANGE", stdout);
            break;

            case ALOG_FI_ERROR:
                fputs("ERROR", stdout);
            break;

            case ALOG_FI_INOCHANGE:
                fputs("INOCHANGE", stdout);
            break;

            case ALOG_FI_KNOWNVIRUS:
                fputs("KNOWNVIRUS", stdout);
            break;

            case ALOG_FI_UNKNOWNVIRUS:
                fputs("UNKNOWNVIRUS", stdout);
            break;

            case ALOG_FI_LOADUNLOAD:
                fputs("LOADUNLOAD", stdout);
            break;

            case ALOG_FI_STARTEND:
                fputs("STARTEND", stdout);
            break;

            case ALOG_FI_STATUS:
                fputs("STATUS", stdout);
            break;

            case ALOG_FI_VIRUSLIKE:
                fputs("VIRUSLIKE", stdout);
            break;

            default:
                fputs("[unknown]", stdout);
            } /* switch */

            printf("\tKey=%08lX Len=%u\n", Key, Size);

            rc = ALogRecReadData(hLog, Size, lpData, 0);
            Errout("ALogRecReadData",rc);

            rc = DisplayFields(lpData);

        }
        Errout("ALogRecSeek/ALogRecRead", rc);

        MemFreePtr(lpData);

    }


    VMMTerminate();
    return rc;
} /* main */

/**********************************************************************
**
**********************************************************************/
static int DisplayFields(
   LPBYTE Data)                         /* Data                      */
{

    while (*(FIELDLEN FAR *)Data) {     /* While there are fields left */
        FIELDLEN  Len;
        ALOGFIELD Type;
        LPBYTE    pData;

                                        /* If field ID matches requsted */
        Len  = *(FIELDLEN FAR *)Data;
        Type = *(((ALOGFIELD FAR *)Data)+1);
        pData= Data+sizeof(FIELDLEN)+sizeof(ALOGFIELD);
        Data += Len;

        printf("      Len=%-3u Type \'%04X\'x(%c)=", Len, Type, Type);

        Len -= sizeof(FIELDLEN)+sizeof(ALOGFIELD);
        switch ((char)Type) {
        case (char)ALOG_FT_ACTION:
            puts("ACTION");
            printf("         \"%.*Fs\"\n", Len, pData);
        break;

        case (char)ALOG_FT_DATE:
            puts("DATE");
        break;

        case (char)ALOG_FT_DIRECTION:
            puts("DIRECTION");
            printf("         %d\n", *(LPWORD)pData);
        break;

        case (char)ALOG_FT_FILESIZE:
            puts("FILESIZE");
            printf("         %ld (\'%lX\'x) \n",
                   *(LPDWORD)pData, *(LPDWORD)pData);
        break;

        case (char)ALOG_FT_MESSAGE:
            puts("MESSAGE");
            printf("         \"%.*Fs\"\n", Len, pData);
        break;

        case (char)ALOG_FT_NETWORK:
            puts("NETWORK");
            printf("         \"%.*Fs\"\n", Len, pData);
        break;

        case (char)ALOG_FT_OS:
            puts("OS");
            printf("         \"%.*Fs\"\n", Len, pData);
        break;

        case (char)ALOG_FT_PATH:
            puts("PATH");
            printf("         \"%.*Fs\"\n", Len, pData);
        break;

        case (char)ALOG_FT_PROGTYPE:
            puts("PROGTYPE");
            printf("         \'%c\'\n", *pData);
        break;

        case (char)ALOG_FT_SERVER:
            puts("SERVER");
            printf("         \"%.*Fs\"\n", Len, pData);
        break;

        case (char)ALOG_FT_TIME:
            puts("TIME");
        break;

        case (char)ALOG_FT_USER:
            puts("USER");
            printf("         \"%.*Fs\"\n", Len, pData);
        break;

        case (char)ALOG_FT_VIRUSNAME:
            puts("VIRUSNAME");
            printf("         \"%.*Fs\"\n", Len, pData);
        break;

        case (char)ALOG_FT_RECTYPE:
            puts("RECTYPE");
            printf("         \'%c\'\n", *pData);
        break;

        default:
            puts("[unknown]");
        } /* switch */

    } /* while */

    return NOERR;
}



/**********************************************************************
**
**********************************************************************/
static void Errout(char *funcname, int rc)
{
    if (!rc)
        return;

    printf("%s error(%d):  ", funcname, rc);

    switch (rc) {
    case NAVLOG_ERR_FILE:
        puts("open/create error.");
    break;

    case NAVLOG_ERR_HANDLE:
        puts("bad handle ");
    break;

    case NAVLOG_ERR_SEEK:
        puts("seek error ");
    break;

    case NAVLOG_ERR_READ:
        puts("read error ");
    break;

    case NAVLOG_ERR_WRITE:
        puts("write error ");
    break;

    case NAVLOG_ERR_MEMORY:
        puts("memory error ");
    break;

    case NAVLOG_ERR_VERSION:
        puts("version mismatch ");
    break;

    case NAVLOG_ERR_BAD_ARG:
        puts("bad argument ");
    break;

    case NAVLOG_ERR_NO_SPACE:
        puts("no disk space ");
    break;

    default:
        puts("unexpeced error number.");
    } /* switch */

    exit(rc);
}

