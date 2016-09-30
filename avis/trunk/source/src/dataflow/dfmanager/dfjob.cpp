*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFJob.cpp                                                    */
/* FUNCTION:  To create a sample object                                       */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY: 27 AUGUST, 1998  SNK                                              */
/*----------------------------------------------------------------------------*/
//#include "stdafx.h"
#include "afxtempl.h"
#include "dfevalstatus.h"
#include "dfevalstate.h"
#include "DFJob.h"

/*----------------------------------------------------------------------------*/
/* Procedure name:      DFJob                                              */
/* Description:         Constructor                                           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFJob::DFJob(int seq, CString Id, CString cookie, CString command)
{
		jobSeq = seq;
		jobId = Id;
		cookieId = cookie;
        commandArg = command;  		
}
