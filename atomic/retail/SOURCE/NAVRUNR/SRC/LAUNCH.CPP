#include <windows.h>
#include <winbase.h>
#include <string.h>
#include <tchar.h>

#if defined (WIN32)
    #define IS_WIN32 TRUE
#else
    #define IS_WIN32 FALSE
#endif
#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)

void GetEnv(TCHAR* szTempW, int size)
{
    HKEY hKey = 0;
    //MEMSET (szTempW, 0, size);
    memset (szTempW, 0, size);

    LPTSTR b;

    if (IS_NT)
        b = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVWNT.EXE");
    else
        b = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVW32.EXE");

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, b, 0, KEY_READ, &hKey))
    {
        b = _T("Path");
        strcpy(szTempW, _T("Path="));
        if (ERROR_SUCCESS != RegQueryValueEx(hKey, b, 0, 0, (LPBYTE)szTempW+strlen(szTempW), (LPDWORD)&size))
        {
            RegCloseKey(hKey);

            GetEnv(szTempW, size); //call again if the function fails the first time
            return;
        }
        RegCloseKey(hKey);
    }
}

int Execute(LPSTR lpCmd)
{
    DWORD dwExitCode = 0;
    SECURITY_ATTRIBUTES  saThread, saProcess;
    PROCESS_INFORMATION  ProcessInformation;
    STARTUPINFO          si;

    // Only if we get the default desktop name do the following
    memset (&si,        0, sizeof (si));
    memset (&saProcess, 0, sizeof (saProcess));
    memset (&saThread,  0, sizeof (saThread));

    if (IS_NT)
    {
        SECURITY_DESCRIPTOR  sdSec;

        InitializeSecurityDescriptor (&sdSec, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl (&sdSec, TRUE, (PACL) NULL, FALSE);

        si.lpDesktop = TEXT("WinSta0\\Default");
        saThread.lpSecurityDescriptor  = &sdSec;
        saProcess.lpSecurityDescriptor = &sdSec;
    }
    else
    {
        si.lpDesktop = NULL;
        saThread.lpSecurityDescriptor  = NULL;
        saProcess.lpSecurityDescriptor = NULL;
    }

    // Set the startup Info.
    si.cb = sizeof (si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;

    saProcess.nLength = sizeof (saProcess);
    saProcess.bInheritHandle = TRUE;

    saThread.nLength = sizeof (saThread);
    saThread.bInheritHandle = TRUE;

    //load the environment
    TCHAR env[1024] = {0};
    GetEnv(env, sizeof(env) / sizeof(env));

    dwExitCode = CreateProcess (
                        NULL,
                        lpCmd,
                        NULL,
                        NULL,
                        FALSE,
                        HIGH_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP | CREATE_SEPARATE_WOW_VDM,
                        env,
                        NULL,
                        &si,
                        &ProcessInformation);
    if (!dwExitCode)
    {
        LPVOID lpMessageBuffer;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      GetLastError(),
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
                      (LPTSTR) &lpMessageBuffer,
                      0,
                      NULL);

        //... now display this string
        MessageBox(NULL, (LPCTSTR)lpMessageBuffer, _T("Norton AntiVirus"), MB_ICONINFORMATION | MB_TASKMODAL | MB_OK);

        // Free the buffer allocated by the system
        LocalFree( lpMessageBuffer );
    }
    else
    {
        WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
    }
    return (dwExitCode);
}

void Run(LPSTR lpCmdLine)
{
    LPSTR lpBegin = strchr(lpCmdLine, '/');
    LPSTR lpEnd;

    while(lpBegin != NULL)
    {
        lpBegin++;
        lpEnd = strchr(lpBegin, '/');
        if (lpEnd)
        {
            char temp = *lpEnd;
            *lpEnd = '\0';

            //execute
            Execute(lpBegin);

            *lpEnd = temp;
            lpBegin = lpEnd;
        }
        else
        {
            //execute
            Execute(lpBegin);
            break;
        }
    }
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (strlen(lpCmdLine))
    {
        Run(lpCmdLine);
    }
    return (1);
}
