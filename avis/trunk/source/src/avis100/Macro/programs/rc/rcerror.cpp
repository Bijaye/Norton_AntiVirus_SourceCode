/*******************************************************************
*                                                                  *
* File name:     RCError.cpp                                       *
*                                                                  *
* Description:   RC error handler                                  *
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
* Function:      Prints out RC error messages                      *
*                                                                  *
*******************************************************************/

#include "afxtempl.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "rcdefs.h"
#include "errors.h"
#include "RCError.h"

const char *LogFile = "RC.LOG";

// constructor
RCError::RCError()
{
	verbose = 0; //default
}

// write string to log file
void RCError::szReportError(char *ErrMsg)
{

   FILE *f;

    if (strcmp(ErrMsg, ""))
      if (verbose)
        MessageBox(NULL,  ErrMsg, NULL, MB_OK |MB_ICONERROR);
   
    if ((f =fopen(LogFile, "a")) !=NULL)
   {
         fprintf(  f, ErrMsg);
        fclose(f);
    }


}

// create error message from parameter list and call szReportError 
// to write it to the log file
void RCError::ReportError(char *fmt, ...)
{
   va_list ap;
   char ErrMsg[MAXMSG];
    
   va_start(ap, fmt);
   vsprintf(ErrMsg, fmt, ap);
   strcat(ErrMsg, "\n");
   szReportError(ErrMsg);
   va_end( ap);
}

// report error and exit
void RCError::ExitError(int exitcode, char *fmt,... )
{
	char ErrMsg[MAXMSG];
	va_list ap;

	va_start(ap, fmt);

	vsprintf(ErrMsg, fmt, ap);
    
    szReportError(ErrMsg);
//    PostQuitMessage(exitcode); // comment out for now, need to move to IdleAction
// for this to work
	exit(exitcode);
}

// indicate how many messages will go to log file 
// this is not used at present, but will be in future
void RCError::SetVerbose(int verbose_value)
{
	verbose = verbose_value;
}
