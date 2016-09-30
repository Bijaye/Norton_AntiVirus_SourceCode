/*******************************************************************
*                                                                  *
* File name:     rcdefs.h                                          *
*                                                                  *
* Description:   Common definitions file                           *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
* Author:        Alla Segal                                        *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      define common constants                           *
*                                                                  *
*******************************************************************/
#ifndef RCDEFS_H
#define RCDEFS_H
#define NOT_FOUND -1
#define MAXPATH 256
#define MAXARGS 30
#define MAXLINE 1024
#define MAXMSG  1024
#define BUFF_SIZE 16384


#define  NewDlgClassName          "NewDlgClass"


const int  IPC_KS = 100;
const int  IPC_WM = 200;
const int  WaitTime = 500;
const int  MaxSleepTime = 30;

const long xlExcel5 = 39;   // Microsoft xl constant for saving an excel 97 file
                     // as an excel 95 file
const long xlWorkbookNormal = -4143;
// Microsoft Power Point constants 
const long ppLayoutBlank = 12;    // type of the slide layout 
const long ppSaveAsPresentation = 1; // save format
// Microsoft Access constants
const long acSaveYes = 1;

#endif
