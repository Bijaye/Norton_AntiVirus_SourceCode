#include <stdio.h>

#include <windows.h>
#include <winbase.h>

//error.c, by JMB
//aimed at printing intelligible error messages to the screen, given a mysterious windows error number
void PrintErr(char * ContextMsg, DWORD err)
{

void *lpMsgBuf;

	FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					err,
					//MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //default language
					MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), //to avoid getting strange messages in foreign versions of windows
					(LPTSTR) &lpMsgBuf,
					0,
					NULL );

	fprintf(stderr, "%s%s", ContextMsg, lpMsgBuf);

	LocalFree(lpMsgBuf);
}
