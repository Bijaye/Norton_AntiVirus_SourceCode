/*******************************************************************
*                                                                  *
* File name:     popup.h                                           *
*                                                                  *
* Description:   definitions used in popup.c                       *
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
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#include <windows.h>
#include <winuser.h>
#include <winbase.h>

#define DoNotMove 0
#define MoveRight 1

int PushButtons(HWND hwnd, int lateral);
int PushEsc(HWND hwnd);
int Close_Window(HWND hwnd);
