/*******************************************************************
*                                                                  *
* File name:     keys.h                                            *
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
* Function:      define keystrokes function                        *
*                                                                  *
*******************************************************************/
#include <windows.h>

int SendKeys(char * keys,HWND winhwnd);
