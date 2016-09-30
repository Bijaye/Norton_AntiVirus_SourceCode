/*******************************************************************
*                                                                  *
* File name:     popup.c                                           *
*                                                                  *
* Description:   keystroke processor definitions                   *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      push buttons to close some app dialogs            *
*                                                                  *
*******************************************************************/
#include "popup.h"
#include "winjmb.h"
#include "keys.h"

#define MoveRight 1
#define DoNotMove 0

#define WinTitleLength 100

int PushButtons(HWND hwnd, int lateral)
{
int i;
	
	SendMessage (hwnd,WM_CANCELMODE,0,0);

	if (lateral > 0)
	{
		for (i = 1; i <= lateral; i++)
		{
			SendMessage (hwnd, WM_KEYDOWN, 0x27, 0x14D000);
			SendMessage (hwnd, WM_KEYUP, 0x27, 0xC14D001);
		}
	}
	
	/* needs to be adapted
	if (lateral < 0)
	{
		for (i = -1; i >= lateral; i--)
		{
			SendMessage (hwnd, WM_KEYDOWN, &H27, &H14D000);
			SendMessage (hwnd, WM_KEYUP, &H27, &HC14D000);
		}
	}
	*/

	//SendMessage (hwnd, WM_KEYDOWN, 0xD, 0x1C0001);
	//SendMessage (hwnd, WM_CHAR, 0xD, 0x1C0001);
	//i = SendMessage (hwnd, WM_KEYUP, 0xD, 0xC01C0001);

    //SendMessage (hwnd, WM_KEYDOWN, 0x1B, 0x010001);
	//SendMessage (hwnd, WM_CHAR, 0xD, 0x1C0001);
	//i = SendMessage (hwnd, WM_KEYUP, 0x1B, 0xC0010001);

	return i;
}

int PushEsc(HWND hwnd)
{
int i;
	
	SendMessage (hwnd,WM_CANCELMODE,0,0);

	
	SendMessage (hwnd, WM_KEYDOWN, 0x1B, 0x10001);
	SendMessage (hwnd, WM_CHAR, 0x1B, 0x10001);
	i = SendMessage (hwnd, WM_KEYUP, 0x1B, 0xC0010001);

    //SendMessage (hwnd, WM_KEYDOWN, 0x1B, 0x010001);
	//SendMessage (hwnd, WM_CHAR, 0xD, 0x1C0001);
	//i = SendMessage (hwnd, WM_KEYUP, 0x1B, 0xC0010001);

	return i;
}



int Close_Window(HWND hwnd)
{
	char windowtitle[WinTitleLength];
	SendMessage (hwnd,WM_CANCELMODE,0,0);
    return SendMessage (hwnd,WM_CLOSE,0,0);
}

