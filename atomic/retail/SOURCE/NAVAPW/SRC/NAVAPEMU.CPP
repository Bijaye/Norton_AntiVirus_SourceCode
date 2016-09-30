// Copyright 1994 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVAPW.NT/VCS/NAVAPEMU.CPv   1.0   24 Apr 1997 21:27:50   ksackin  $
//
// Description:
//      NAVAPEMU.CPP - This file contains the routines necessary to emulate
//      commucations with the VxD.  This debugging tool replaces the
//      the following functions with emulators:
//              CreateFile, CloseHandle, LoadVxD, FreeVxD,
//              DeviceIoControl
//
// Contains:
//      MyCreateFile()
//      MyCloseHandle()
//      MyLoadVxD()
//      MyFreeVxD()
//      MyDeviceIoControl()
//
//***************************************************************************
// $Log:   S:/NAVAPW.NT/VCS/NAVAPEMU.CPv  $
// 
//    Rev 1.0   24 Apr 1997 21:27:50   ksackin
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:12:24   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 12:13:34   DKEEFER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:52:38   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:14:52   BARRY
// Initial revision.
// 
//    Rev 1.8   08 Jul 1995 13:04:10   KEITH
// Pass OEM filenames to the VXD
// 
//    Rev 1.7   07 Mar 1995 20:16:42   RSTANEV
// Not using shell2.h anymore.
//
//    Rev 1.6   24 Jan 1995 01:59:58   DALLEE
// Removed VXDPUBLC.H.  Auto-protect log string ID's are back in NAVAPLOG.H
//
//    Rev 1.5   23 Jan 1995 19:30:20   MFALLEN
// Strings are being used from the NAVXUTIL project. Solution had to be found.
//
//    Rev 1.4   28 Dec 1994 09:23:32   MARKL
// Activity log fields are now WORDs instead of UINTs.  Also set the activity
// log spool file to be in the same location as the activity log.
//
//    Rev 1.3   22 Dec 1994 13:28:00   MARKL
// Fixed some string variable names.
//
//    Rev 1.2   20 Dec 1994 18:32:52   MARKL
// Now uses activity log commands to emulate communications between the
// VxD and the Agent.
//
//    Rev 1.1   14 Dec 1994 20:29:12   MARKL
// wSize is now dwSize
//
//    Rev 1.0   11 Dec 1994 15:18:28   MARKL
// Initial revision.
//***************************************************************************



//***************************************************************************
// Include Files
//***************************************************************************
#include "platform.h"
#include "windowsx.h"
#include "xapi.h"
#include "symalog.h"
#include "msgdisp.h"                    // For error message display
#include "navapw.h"                     // Auto-Protect Agent items
#include "navapcom.h"                   // Agent <-> VxD communication items
#include "navaplog.h"                   // LOG FILE IDS
#include "navapstb.h"                   // Agent string table ids.


//***************************************************************************
// External and Internal Globals
//***************************************************************************
extern          NAVAPW_CONFIG           g_Cfg;
extern          NAVAPW_COMM             g_Comm;

WORD            wVirMessageID           = IDS_VIR_MSG_IS_INFECTED;


//***************************************************************************
// MyCreateFile()
//
// Description:
//      Emulates the CreateFile() routine for testing Agent<->VxD
//      communications.
//
// Parameters:
//      lpszName
//      fdwAccess
//      fdwShareMode
//      lpsa
//      fdwCreate
//      fdwAttrsAndFlags
//      hTemplateFile
//
// Return Value:
//      HANDLE                          - Returns NULL or
//                                        INVALID_HANDLE_VALUE
//
//***************************************************************************
// 12/11/1994 MARKL Function Created.
//***************************************************************************
HANDLE PASCAL MyCreateFile (
    LPCSTR      lpszName,
    DWORD       fdwAccess,
    DWORD       fdwShareMode,
    LPSECURITY_ATTRIBUTES lpsa,
    DWORD       fdwCreate,
    DWORD       fdwAttrsAndFlags,
    HANDLE      hTemplateFile)
{
    char        szTemp[512];

    wsprintf (szTemp, "File:\t\t%s\nAccess:\t\t%ld\nShareMode:\t%ld\nfCreate:\t%ld\nAttrs:\t\t%ld\nTemplate:\t%d\n\nAllow?",
              (LPSTR)lpszName, (DWORD)fdwAccess, (DWORD)fdwShareMode,
              (DWORD)fdwCreate, (DWORD)fdwAttrsAndFlags, (WORD)hTemplateFile);

    if (MessageBox (NULL, szTemp, "Emulated CreateFile()", MB_YESNO)
        == IDYES)
        {
        return ((HANDLE)1);
        }
    else
        {
        return (INVALID_HANDLE_VALUE);
        }
} // MyCreateFile()



//***************************************************************************
// MyCloseHandle()
//
// Description:
//      Emulates the CloseHandle() routine for testing Agent<->VxD
//      communications.
//
// Parameters:
//      hObject
//
// Return Value:
//      BOOL                            - Returns TRUE or FALSE
//
//***************************************************************************
// 12/11/1994 MARKL Function Created.
//***************************************************************************
BOOL PASCAL MyCloseHandle (
    HANDLE hObject)
{
    char        szTemp[512];

    wsprintf (szTemp, "Handle:\t\t%lx\n\nAllow?", (DWORD)hObject);

    if (MessageBox (NULL, szTemp, "Emulated CloseHandle()", MB_YESNO)
        == IDYES)
        {
        return (TRUE);
        }
    else
        {
        return (FALSE);
        }
} // MyCloseHandle()



//***************************************************************************
// MyLoadVxD()
//
// Description:
//      Emulates the LoadVxD() routine for testing Agent<->VxD
//      communications.
//
// Parameters:
//      pW32IOCTLDATA
//      lpszVxDFile
//      wFlags
//
// Return Value:
//      UINT                         - Returns VXDLOAD_ERR_NONE or
//                                     VXDLOAD_ERR_NOT_FOUND
//
//***************************************************************************
// 12/11/1994 MARKL Function Created.
//***************************************************************************
UINT PASCAL MyLoadVxD (
    W32IOCTLDATA * pData,
    char const * lpszVxDFile,
    WORD wFlags )
{
    char        szTemp[512];

    wsprintf (szTemp, "File:\t\t%s\nFlags:\t\t%d\n\nAllow?",
              (LPSTR)lpszVxDFile, (WORD)wFlags);

    if (MessageBox (NULL, szTemp, "Emulated LoadVxD()", MB_YESNO)
        == IDYES)
        {
        return (VXDLOAD_ERR_NONE);
        }
    else
        {
        return (VXDLOAD_ERR_NOT_FOUND);
        }
} // MyVxDLoad()



//***************************************************************************
// MyFreeVxD()
// 
// Description:
//      Emulates the FreeVxD() routine for testing Agent<->VxD
//      communications.
// 
// Parameters:
//      pW32IOCTLDATA
//
// Return Value: 
//      UINT                         - Returns VXDLOAD_ERR_NONE or
//                                     VXDLOAD_ERR_VXDLOADER
// 
//***************************************************************************
// 12/11/1994 MARKL Function Created.                                   
//***************************************************************************
UINT PASCAL MyFreeVxD ( 
    W32IOCTLDATA * pData)
{
    if (MessageBox (NULL, "Allow?", "Emulated FreeVxD()", MB_YESNO)
        == IDYES)
        {
        return (VXDLOAD_ERR_NONE);
        }
    else
        {
        return (VXDLOAD_ERR_VXDLOADER);
        }
} // MyFreeVxD()



//***************************************************************************
// MyDeviceIoControl()
// 
// Description:
//      Emulates the DeviceIoControl() routine for testing Agent<->VxD
//      communications.
// 
// Parameters:
//      hDevice
//      dwIoControlCode
//      lpvInBuffer
//      cbInBuffer
//      lpvOutBuffer
//      cbOutBuffer
//      lpcbBytesReturned
//      lpoOverlapped
// 
// Return Value: 
//      BOOL                            - Returns TRUE or FALSE
// 
//***************************************************************************
// 12/11/1994 MARKL Function Created.                                   
//***************************************************************************
BOOL PASCAL MyDeviceIoControl (
    HANDLE      hDevice,
    DWORD       dwIoControlCode,
    LPVOID      lpvInBuffer,
    DWORD       cbInBuffer,
    LPVOID      lpvOutBuffer,
    DWORD       cbOutBuffer,
    LPDWORD     lpcbBytesReturned,
    LPOVERLAPPED lpoOverlapped)
{
    auto        char    szTemp1[512];
    auto        char    szAdditionalInfo[512];
    auto        char    szTemp[1024];

    *szAdditionalInfo = EOS;

    wsprintf (szTemp1, "Device:\t\t0x%x\nControl ID:\t%ld\n   Command:\t",
              (WORD)hDevice, (DWORD)dwIoControlCode);
    
    switch (dwIoControlCode)
        {
        case ID_COMMAND_NO_COMMAND:
            STRCAT (szTemp1, "ID_COMMAND_NO_COMMAND\n");
            break;
        case ID_COMMAND_GET_VXD_VERSION:
            STRCAT (szTemp1, "ID_COMMAND_GET_VXD_VERSION\n");
            break;
        case ID_COMMAND_GET_ENABLE_DISABLE:
            STRCAT (szTemp1, "ID_COMMAND_GET_ENABLE_DISABLE\n");
            break;
        case ID_COMMAND_GET_CMD_FROM_VXD:
            {
            LPNAVAP_LOG lpNavApLog;

            lpNavApLog = (LPNAVAP_LOG)(((LPNAVAP_AGENT_COMMAND)lpvOutBuffer)->szData);
            lpNavApLog->wSize = sizeof (NAVAP_LOG);
            lpNavApLog->byRecType = ALOG_FI_KNOWNVIRUS;
            lpNavApLog->wVirActionID = IDS_VIR_ACT_NONE;
            STRCPY (lpNavApLog->szVirus, "Mark's Test");
            STRCPY (lpNavApLog->szFile, "C:\\FUNKY\\MOE\\TOWN.EXE");
            STRCPY (lpNavApLog->szProgram, "C:\\MYSTERY\\PROGRAM\\THEATRE.EXE");
            AnsiToOem (lpNavApLog->szVirus, lpNavApLog->szVirus);
            AnsiToOem (lpNavApLog->szFile, lpNavApLog->szFile);
            AnsiToOem (lpNavApLog->szProgram, lpNavApLog->szProgram);

            lpNavApLog->wVirMessageID = wVirMessageID;

            STRCAT (szTemp1, "ID_COMMAND_GET_CMD_FROM_VXD\n");
            ((LPNAVAP_AGENT_COMMAND)lpvOutBuffer)->dwAgentCommandID = ID_AGENT_COMMAND_DO_LOG;
            ((LPNAVAP_AGENT_COMMAND)lpvOutBuffer)->dwSize = 
                sizeof (NAVAP_AGENT_COMMAND) + sizeof (NAVAP_LOG);
            wsprintf (szAdditionalInfo, "\tWill Return ID_AGENT_COMMAND_DO_LOG: %d\r\n", (WORD)wVirMessageID);

            if (wVirMessageID == IDS_VIR_MSG_LOW_FORMAT)
                {
                wVirMessageID = IDS_VIR_MSG_IS_INFECTED;
                }
            else
                {
                wVirMessageID++;
                }
            }
            break;
        case ID_COMMAND_AGENT_STARTED:
            STRCAT (szTemp1, "ID_COMMAND_AGENT_STARTED\n");
            break;
        case ID_COMMAND_AGENT_TERMINATING:
            STRCAT (szTemp1, "ID_COMMAND_AGENT_TERMINATING\n");
            break;
        case ID_COMMAND_VXD_AUTOLOADED:
            {
            char szAnsiPath[SYM_MAX_PATH] = {0};
            OemToAnsi((LPSTR)lpvInBuffer, szAnsiPath);
            STRCAT (szTemp1, "ID_COMMAND_VXD_AUTOLOADED\n");
            wsprintf (szAdditionalInfo, "\tlpvInBuffer = Path = %s\n", 
                      szAnsiPath);
            }
            break;
        case ID_COMMAND_ENABLE_VXD:
            STRCAT (szTemp1, "ID_COMMAND_ENABLE_VXD\n");
            break;
        case ID_COMMAND_DISABLE_VXD:
            STRCAT (szTemp1, "ID_COMMAND_DISABLE_VXD\n");
            break;
        case ID_COMMAND_UNLOAD_VXD:
            STRCAT (szTemp1, "ID_COMMAND_UNLOAD_VXD\n");
            break;
        case ID_COMMAND_SHUTDOWN_IN_PROGRESS:
            STRCAT (szTemp1, "ID_COMMAND_SHUTDOWN_IN_PROGRESS\n");
            break;
        default:
            STRCAT (szTemp1, "Unknown Command!!!\n");
            break;
        }

    wsprintf (szTemp, "%slpvInBuffer:\t0x%lx\ncbInBuffer:\t%ld\nlpvOutBuffer:\t0x%lx\ncbOutBuffer:\t%ld\nlpcbBytesReturned:\t0x%lx\n\n%s\n\nAllow?",
              (LPSTR)szTemp1,
              (DWORD)lpvInBuffer,
              (DWORD)cbInBuffer, (DWORD)lpvOutBuffer, (DWORD)cbOutBuffer,
              (DWORD)lpcbBytesReturned,
              (LPSTR)szAdditionalInfo);

    if (MessageBox (NULL, szTemp, "Emulated DeviceIoControl()", MB_YESNO)
        == IDYES)
        {
        return (TRUE);
        }
    else
        {
        return (FALSE);
        }
} // MyDeviceIoControl()







