#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <winuser.h>
#include <winbase.h>


long KillAppInstance(HWND hAppWin);
long KillAllAppInstances(LPCTSTR lpszClass);
long KillAllUserAppInstances(LPCTSTR lpszClass, LPCTSTR lpszUserName, LPCTSTR lpszPassword);
