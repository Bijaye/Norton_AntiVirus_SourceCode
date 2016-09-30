#include <stdio.h>

#include <windows.h>
#include <winbase.h>

//Prints intelligible error messages to the standard error device given the windows error code
//Context: description string printed before the message
//err: error code provided by the Win32 function GetLastError(). 
void PrintErr(char* Context, DWORD err);
