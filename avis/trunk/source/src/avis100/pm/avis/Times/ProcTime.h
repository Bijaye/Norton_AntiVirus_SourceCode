/*******************************************************************
*                                                                  *
* File name:     ProcTime.h                                        *
*                                                                  *
* Description:   header for ProcTime.c                             *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 2000                      *
*                                                                  *
* Author:        Jean-Michel Boulay                                *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
*******************************************************************/


#include <windows.h>
#include <winbase.h>

static double filetime_to_clock(PFILETIME ft);
int avis_win32_times(double * plutime, double * plstime, double * plcutime, double * plcstime);

