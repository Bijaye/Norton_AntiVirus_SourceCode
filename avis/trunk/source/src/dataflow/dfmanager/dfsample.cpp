/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFSample.cpp                                                    */
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
#include "afxtempl.h"
#include "afxwin.h"

#include <iostream>
#include <fstream>
#include <iostream>
#include <ios>
#include <strstream>

#include "AnalysisStateInfo.h"
#include "AnalysisRequest.h"
#include "avisdbexception.h"

#ifndef DFEVALSTATUS_H
	#include "dfevalstatus.h"
#endif
#ifndef DFEVALSTATE_H
	#include "dfevalstate.h"
#endif
#ifndef DFJOB_H
	#include "dfjob.h"
#endif
#ifndef DFSAMPLE_H
	#include "dfsample.h"
#endif
#include "resource.h"
#include "dferror.h"

/*----------------------------------------------------------------------------*/
/* Procedure name:      DFSample                                              */
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
DFSample::DFSample(CString Id, CString state, SampleStatus status):
    cookieId(Id),
	strCurrentState(state),
	strNextState(""),
	strPreviousState(CString("")),	
	currentStatus(status),
	statusDet(FALSE),
	ignoreFlag(FALSE),
	ofile(NULL),
    initCond(0),
    doneJobs(0),
	successJobs(0),
	inProgressJobs(0),
	seqNumber(0),
	stopSatisfied(0),
	stopCondFlag(FALSE),
	userRequest(NONE),
	stopCond(0),
	acceptedState(CString("BEGIN")),
	noBuildFlag(TRUE),
	defSeqNumber(""),
	scanRequired(FALSE),
	scanJob(NULL),
	criticalErrorFlag(0),
	dffinishFlag(0),
	pipelineFlag(0), 
	evalPathFlag(0),
	serializeStateFlag(0),
	packDone(FALSE),
	deferCriticalSample(0)
{	
}    
/*----------------------------------------------------------------------------*/
/* Procedure name:      DFSample                                              */
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
DFSample::DFSample(CString Id):
    cookieId(Id),
//	strCurrentState(state),
	strNextState(""),
	strPreviousState(CString("")),	
//	currentStatus(status),
	statusDet(FALSE),
	ignoreFlag(FALSE),
	ofile(NULL),
    initCond(0),
    doneJobs(0),
	successJobs(0),
	inProgressJobs(0),
	seqNumber(0),
	stopSatisfied(0),
	stopCondFlag(FALSE),
	userRequest(NONE),
	stopCond(0),
	acceptedState(CString("BEGIN")),
	noBuildFlag(TRUE),
	defSeqNumber(""),
	scanRequired(FALSE),
	scanJob(NULL)
{
}  
/*----------------------------------------------------------------------------*/
/* Procedure name:      DFSample                                              */
/* Description:         Destructor                                            */
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
DFSample::~DFSample()
{

	if (jobNewList.GetCount() != 0)  {
			POSITION position = jobNewList.GetHeadPosition();
			POSITION pos1, pos2;
			DFJob* pJob;
			for (pos1 = jobNewList.GetHeadPosition(); (pos2 = pos1) != NULL;)
			{
				pJob = jobNewList.GetNext(pos1);
				jobNewList.RemoveAt(pos2);
				delete pJob;
			}
	}
	if (jobList.GetCount() != 0)  {
			POSITION position = jobList.GetHeadPosition();
			POSITION pos1, pos2;
			DFJob* pJob;
			for (pos1 = jobList.GetHeadPosition(); (pos2 = pos1) != NULL;)
			{
				pJob = jobList.GetNext(pos1);
				jobList.RemoveAt(pos2);
				delete pJob;
			}
	} 



//	if (jobList.GetCount() > 0) 
//       	jobList.RemoveAll();
//	if (jobNewList.GetCount() > 0) 
//       	jobNewList.RemoveAll();
#ifdef _DEBUG
#ifndef LAUNCHER_VIEW
	if (ofile) {
		delete ofile;
    }
#endif
#endif
}  
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindObjectWithSeqInSample                             */
/* Description:         To find a job with seq # in a sample's job collection */
/* Input:               int seq   -sequence                                   */                      
/*                      POSITION& pos - position                              */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              pointer to DFJob object                               */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFJob *DFSample::FindJobWithSeq(int seq, POSITION& pos)
{
     
		pos = jobList.GetHeadPosition();
		int i = 0;
		DFJob* pJob;
		while (pos != NULL)
		{
			pJob = jobList.GetNext(pos);
			if (pJob->GetJobSeq() == seq)
			return pJob;
		
		}
		return NULL;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      DeleteAllJobs                                         */
/* Description:         To delete all jobs in the job collection              */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - success                                           */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFSample::DeleteAllJobs()
{
		POSITION position = jobList.GetHeadPosition();
		POSITION pos1, pos2;
		DFJob* pJob;
		for (pos1 = jobList.GetHeadPosition(); (pos2 = pos1) != NULL;)
		{
			pJob = jobList.GetNext(pos1);
			jobList.RemoveAt(pos2);
			delete pJob;
		}
		return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetCurrentStatus                                      */
/* Description:         To get current status as a string                     */
/*                                                                            */
/* Input:               SampleStatus status - status of a sample              */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              CString  status                                       */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
CString DFSample::GetCurrentStatusStr(SampleStatus status)
{
   	    CString strStatus;
        switch (status)
		{	
		case WAIT:
			strStatus = CString((LPCSTR) IDS_WAIT);
			break;
		case SUCCESS:
			strStatus = CString((LPCSTR) IDS_SUCCESS);
			break;
		case INPROGRESS:
			strStatus = CString((LPCSTR) IDS_INPROGRESS);
			break;
		case FAIL:
			strStatus = CString((LPCSTR) IDS_FAIL);
			break;
		case FINAL:
			strStatus = CString((LPCSTR) IDS_FINAL);
			break;
		case CriticalError:
			strStatus = CString((LPCSTR) IDS_CRITICAL_ERROR);
			break;

        default:
			strStatus = CString();

        }
        return strStatus;

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SetDefSeqNumbers                                      */
/* Description:         To ser new def seq number                             */
/*                                                                            */
/* Input:               DFSample *pSample                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              CString  status                                       */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFSample::SetDefSeqNumber(CString strParam)
{	
	if (strParam == CString())
		return FALSE;
	strParam.TrimLeft();
	strParam.TrimRight();
	int n = strParam.Find(' ');
	if (n== -1)
		return FALSE;
	CString firstParam = strParam.Left(n);
	CString secondParam = strParam.Mid(n + 1);
	if (secondParam != CString()) {
		n = secondParam.Find(' ');
		if (n == -1)
			return FALSE;
		CString secondParam1 = secondParam.Left(n);
		if (firstParam.CompareNoCase("FULL") == 0) {
			 defSeqNumber = secondParam1;
		     return TRUE;
        }  
		else 
			return FALSE;
    }
	else return FALSE;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SetScanResults                                        */
/* Description:         To set new def seq number, virus ID, and Virus name   */
/*                                                                            */
/* Input:               DFSample *pSample                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              CString  status                                       */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFSample::SetScanResults(CString strParam)
{	
    
	if (strParam == CString())
		return FALSE;
	CString firstParam, secondParam, thirdParam;
	strParam.TrimLeft();
	strParam.TrimRight();
	int n = strParam.Find(' ');
	if ( n == -1 )
		return FALSE;
	firstParam = strParam.Left(n);
	/*
	CString restParam = strParam.Mid(n + 1);
	if (restParam != CString()) {
		n = restParam.Find(' ');
		if ( n == 0 )
			return FALSE;
		secondParam = restParam.Left(n);
        thirdParam =restParam.Mid(n + 1); 
		if (thirdParam == CString())
			return FALSE;
    } 
	defSeqNumber = firstParam;
	virusId = secondParam;
	virusName = thirdParam; 
	*/
	defSeqNumber = firstParam;
    return TRUE;

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SetSampleState                                        */
/* Description:         To set state in the DB                                */
/*                                                                            */
/* Input:               DFSample *pSample                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              CString  status                                       */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFSample::SetSampleState(CString state)
{	
   	int localCookie = atoi((LPTSTR) (LPCTSTR) GetCookieId());
	char chState[256];
	strcpy(chState, (LPTSTR) (LPCTSTR)state);

	try {
		AnalysisRequest request(localCookie);
		std::string strAnalysisState;
		strAnalysisState = std::string(chState);
		AnalysisStateInfo stateObject( strAnalysisState );
		request.State(stateObject);
	}
	catch (AVISDBException exception) {
			char msg[1028];
			std::string	  detailedString; 
            detailedString = exception.DetailedInfo();
	        std::ostrstream inMsg(msg, sizeof(msg) - 1);
       		inMsg << detailedString;
			inMsg << std::ends;
			AfxMessageBox(msg, MB_ICONSTOP, 0);
			return(IDS_DATABASE_UPDATE_RECORD_ERROR); 
        
	}
    return DF_SUCCESS;
}
