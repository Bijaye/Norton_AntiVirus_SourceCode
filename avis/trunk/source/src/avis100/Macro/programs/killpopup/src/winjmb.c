/*******************************************************************
*                                                                  *
* File name:     winjmb.c                                          *
*                                                                  *
* Description:   Get window title                                  *
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
* Function:      get title of the window given the handle          *
*                                                                  *
*******************************************************************/
#include <stdio.h>
#include <windows.h>
#include <winuser.h>

int GetWinTitle(HWND winhwnd,char * buffer,int buffersize)
{
int titlelength;
	
	if (IsWindow(winhwnd) ==0)
		return 1;
	else
	{
		titlelength = GetWindowTextLength(winhwnd);			
        if (buffersize < titlelength + 1)
			return 2;
		else
		{
			titlelength = GetWindowText(winhwnd, buffer,buffersize);
			buffer[titlelength] = 0;
			return 0;
		}
	}
}
