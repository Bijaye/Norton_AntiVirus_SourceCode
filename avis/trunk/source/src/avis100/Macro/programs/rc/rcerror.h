/*******************************************************************
*                                                                  *
* File name:     rcerror.h                                         *
*                                                                  *
* Description:   RCError class definition file                     *
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
* Function:      class definition for rc errors processor          *
*                                                                  *
*******************************************************************/
#ifndef RCERROR_H
#define RCERROR_H
class RCError {

  public:

     RCError();
     void ReportError(char *fmt, ...);
     void ExitError(int exitcode, char *fmt,... );
	 void SetVerbose(int verbose_value);
  private:
    char *tmpPath;
    void szReportError(char *ErrMsg);
	BOOL verbose;

};
#endif


