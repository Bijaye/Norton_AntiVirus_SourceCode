/*******************************************************************
*                                                                  *
* File name:     appli.h                                           *
*                                                                  *
* Description:   some definitions for appli.c functions            *
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
* Function:      Contains function definitions for termination     *
*                and initialization of the application             *
*                                                                  *
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#include <windows.h>
#include <winuser.h>
#include <winbase.h>

int kill_app_instance(HWND apphwnd);
int kill_all_app_instances(LPCTSTR AppClassName);
HANDLE start_app_instance (char * Path);
