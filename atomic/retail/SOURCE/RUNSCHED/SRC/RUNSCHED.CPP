#include "platform.h"
#include <malloc.h>
#include <fcntl.h>

#include "navw.h"
#include "apscomm.h"
#include "resource.h"
#include "svcctrl.h"
#include "navutil.h"

BOOL IsSchedSystemLogin(HINSTANCE hInstance)
{
    TCHAR  szServiceName[MAX_SERVICENAME_SIZE];
    BOOL   bRet = TRUE;

    LoadString (hInstance, IDS_SCHEDULESERVICE, szServiceName, sizeof (szServiceName));

    SC_HANDLE hSCMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  // we want access to everything

    if (hSCMgr)
    {
        SC_HANDLE hService = OpenService(hSCMgr, szServiceName, SERVICE_ALL_ACCESS);// requested access clearance
        if (hService)
        {
            QUERY_SERVICE_CONFIG ServiceConfig;
            DWORD                dwBytesNeeded;
            TCHAR  szLocalSystemName[MAX_SERVICENAME_SIZE];

            LoadString (hInstance, IDS_LOCALSYSTEM, szLocalSystemName, sizeof (szLocalSystemName));

            if (QueryServiceConfig(hService, &ServiceConfig, sizeof (ServiceConfig), &dwBytesNeeded))
            {
                if (strcmp(ServiceConfig.lpServiceStartName, szLocalSystemName))
                    bRet = FALSE;
            }
            else
            {
                LPQUERY_SERVICE_CONFIG lpServiceConfig;
                DWORD dwError = GetLastError() ;

                if (dwError == ERROR_INSUFFICIENT_BUFFER)
                {
                    // Don't bother to free(Should not) these allocations as this function (_alloca)creates
                    // them on the stack as opposed to malloc, so they get free'd when this
                    // function returns
                    lpServiceConfig =     (LPQUERY_SERVICE_CONFIG)_alloca(dwBytesNeeded);
                    lpServiceConfig->lpBinaryPathName   = (LPTSTR)_alloca(dwBytesNeeded);
                    lpServiceConfig->lpLoadOrderGroup   = (LPTSTR)_alloca(dwBytesNeeded);
                    lpServiceConfig->lpDependencies     = (LPTSTR)_alloca(dwBytesNeeded);
                    lpServiceConfig->lpServiceStartName = (LPTSTR)_alloca(dwBytesNeeded);
                    lpServiceConfig->lpDisplayName      = (LPTSTR)_alloca(dwBytesNeeded);

                    if (QueryServiceConfig(hService, lpServiceConfig, dwBytesNeeded, &dwBytesNeeded))
                    {
                        if (strcmp(lpServiceConfig->lpServiceStartName, szLocalSystemName))
                            bRet = FALSE;
                    }
                }
            }
        }
    }

    return (bRet);
}

int IsNAVServiceAround(LPTSTR lpCmd, DWORD dwCmd, BOOL fCheck)
{
    //connect to the service pipe
    char  szPipeName[] = "\\\\.\\pipe\\NAVAPSVC";
    DWORD dwBytesRead;
    DWORD dwReturn;
    CMDPACKET rPacket;

    rPacket.dwSize    = sizeof (CMDPACKET);
    rPacket.dwCommand =  dwCmd;
    strcpy (rPacket.rData.rExeData.szCmdLine, lpCmd);
    rPacket.rData.rExeData.nCmdShow = SW_SHOWNORMAL;

    BOOL fRet = CallNamedPipe(
                szPipeName,              // Pipe Name
                (LPVOID) &rPacket,       // Code to send
                sizeof (CMDPACKET),      // Bytes to send
                (LPVOID) &dwReturn,      // Return code
                sizeof (dwReturn),       // Bytes to receive
                &dwBytesRead,            // Returns the number read
                NMPWAIT_WAIT_FOREVER);   // Don't return until you have something

    if (fCheck)
        return (fRet);         //BOOL return if the NAV services are avilable
    else
        return ((int)dwReturn);
}

void ErrProcess(int Err, UINT& msg)
{
    switch (Err)
    {
        case ERROR_BAD_FORMAT:
            msg = IDS_ERR_INVALIDFILE;
            break;

        case ERROR_FILE_NOT_FOUND:
            msg = IDS_ERR_FILENOTFOUND;
            break;

        case ERROR_PATH_NOT_FOUND:
            msg = IDS_ERR_PATHNOTFOUND;
            break;
    }
}

DWORD CmdExecute(LPSTR lpCmdLine, int nCmdShow)
{
    //get the command line
    DWORD dwReturn = 0;
    char file[600];
    strcpy(file, lpCmdLine);
    LPTSTR lpParm = strchr (file, ' ');  //point to the param list
    LPTSTR lpFN   = strrchr(file, '"');  //point to the file name
    if (lpFN)
    {
        *lpFN = '\0';
        lpFN = file;
        lpFN++;
        strcat(lpFN, lpParm);
    }
    else
        lpFN = file;

    SECURITY_DESCRIPTOR  sdSec;
    if (InitializeSecurityDescriptor (&sdSec, SECURITY_DESCRIPTOR_REVISION))
    {
        if (SetSecurityDescriptorDacl (&sdSec, TRUE, (PACL) NULL, FALSE))
        {
            SECURITY_ATTRIBUTES  saThread, saProcess;
            PROCESS_INFORMATION  rNav;
            STARTUPINFO          si;

            // Only if we get the default desktop name do the following
            MEMSET (&si,        0, sizeof (si));
            MEMSET (&saProcess, 0, sizeof (saProcess));
            MEMSET (&saThread,  0, sizeof (saThread));

            // Set the startup Info.
            si.cb = sizeof (si);
            si.lpDesktop = "WinSta0\\Default";
            si.dwFlags = 0;
            si.wShowWindow = SW_SHOWNORMAL;

            saProcess.nLength = sizeof (saProcess);
            saProcess.lpSecurityDescriptor = &sdSec;
            saProcess.bInheritHandle = FALSE;

            saThread.nLength = sizeof (saThread);
            saThread.lpSecurityDescriptor = &sdSec;
            saThread.bInheritHandle = FALSE;

            if (!CreateProcess (NULL,
                                lpFN,
                                &saProcess,
                                &saThread,
                                FALSE,
                                HIGH_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
                                NULL,
                                NULL,
                                &si,
                                &rNav))
            {
                dwReturn = GetLastError ();
            }
        }
        else
            dwReturn = GetLastError ();
    }
    else
        dwReturn = GetLastError ();

    return (dwReturn);
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //check if the system scheduler loged in as system account
    if (STRLEN(lpCmdLine))
    {
        UINT msg = 0;
        HWND hDlg = NULL;

        if (IsSchedSystemLogin(hInstance) == TRUE)
        {
            //find if there is a running copy of navwnt.exe
            if ((hDlg = FindWindow(NAV_MAIN_WND_CLASS, NULL)))
                msg = IDS_ERR_NAVRUNNING;
            else
            {
                //ErrProcess (WinExec(lpFN, SW_SHOWNORMAL), msg);
                ErrProcess((int)CmdExecute(lpCmdLine, nCmdShow), msg);
            }
        }
        else if (IsNAVServiceAround(lpCmdLine, NAVAPSVC_GET_STATUS, TRUE))     //check if our services are available and use them
        {
            //find if there is a running copy of navwnt.exe
            if ((hDlg = FindWindow(NAV_MAIN_WND_CLASS, NULL)))
                msg = IDS_ERR_NAVRUNNING;
            else
                ErrProcess (IsNAVServiceAround(lpCmdLine, NAVAPSVC_EXEC_PROGRAM, FALSE), msg);
        }
        else
            msg = IDS_ERR_CANNOTSTART;

        if (msg)
        {
            auto NOTIFYSTRUCT Notify;

            MEMSET (&Notify, EOS, sizeof(Notify));

            if (hDlg && IsWindowVisible(hDlg) && !IsIconic(hDlg))
                Notify.hParent = hDlg;
            else
                Notify.hParent = NULL;

            Notify.uMsgID        = msg;
            Notify.uTitleID      = IDS_NAVW;
            Notify.hInstance     = hInstance;

            NotifyUser (&Notify);
        }
    }
    return (1);
}
