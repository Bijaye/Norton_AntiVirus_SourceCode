#include <windows.h>

#include <stdlib.h>
#include <stdio.h>

void main(int argc, char * argv[])
{
UINT uCmdId;
HWND hWnd;

if (argc ==1) {
	fprintf(stderr, "syntax: sendcommand classname uint\n");
    exit(1);
}

hWnd = FindWindow(argv[1],0);
uCmdId = (UINT) atoi(argv[2]);
printf("%d\n", uCmdId);

PostMessage(hWnd, WM_COMMAND, uCmdId, 0);
}
