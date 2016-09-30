/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFManager.cpp                                                   */
/* FUNCTION:  To manage samples                                               */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY:  SEPTEMBER, 1998  SNK                                             */
/*----------------------------------------------------------------------------*/
#include "afxtempl.h"
#include "afxwin.h"
#include <iostream>
#include <fstream>
#include <ios>
#include <strstream>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "direct.h"
#include "math.h"
#include "dfdirinfo.h"
#include "paramvalue.h"
#include "resource.h"
#include "dfdefaults.h"
#include "computeMD5CheckSum.h"
#include "avissendmail.h"
#include "dfpackthread.h"

#ifndef DFEVALSTATUS_H
	#include "dfevalstatus.h"
#endif
#ifndef DFEVALSTATE_H
	#include "dfevalstate.h"
#endif
#ifndef DFMSG_H
	#include "dfmsg.h" 
#endif
#ifndef DFMATRIX_H
	#include "dfmatrix.h"
#endif
#ifndef DFJOB_H
	#include "dfjob.h"
#endif
#ifndef DFSAMPLE_H
	#include "dfsample.h"
#endif
#ifndef DFERROR_H
	#include "dferror.h"  
#endif
#ifndef DFMANAGER_H
	#include "DFManager.h"
#endif
#ifndef DFDBSELECT_H
	#include "dfdbselect.h"
#endif

#ifdef DATABASE_INTERFACE
	#include "AnalysisStateInfo.h"
	#include "AnalysisRequest.h"
	#include "avisdbexception.h"
    #include "signature.h" 
	#include "sigsToBeExported.h" 
#else
	#include "dfdbrequests.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define USE_AVISDF_CONFIG_FILE

/*----------------------------------------------------------------------------*/
/* Procedure name:      DFManager                                             */
/* Description:         Costructor                                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              pointer to DFSample object                            */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFManager::DFManager():
    testFile(0),
    logFile(0),
	arrivedSamples(0),
	averageProcessingTime(0),
	arrivalCount(1),
	serialFlag(FALSE),
	successfulSamples(0),
	rescannedSamples(0),
	deferredSamples(0),
	returnedBackSamples(0),
	criticalSamples(0),
    criticalErrorFlag(0),
    criticalErrorFlag1(0),
	deferCriticalSample(0),
	defSeqNumber(CString()),
	criticalSample(), 
	defImportJob(NULL),
	statusUpdaterJob(NULL),
	undeferrerJob(NULL),
	attributeCollectorJob(NULL),
	importPending(0),
	packThread(NULL),
	packSample(NULL),
	closeWndRequest(0),
	bgtrap(0)
{
	strcpy(UNCPath, "");
	strcpy(buildDefFilename, "");
	strcpy(defBaseDir, "");
	strcpy(defImporterDir, "");

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      Init                                                  */
/* Description:         To initialize the manager: read the configuration,    */
/*                      initialize the evaluator matrix, clenaup the sample's */
/*                      directories                                           */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - Success; DF_EVALUATOR_FAILED - evaluator failed;  */
/*                      DF_CONFIG_FILE_ERROR - configuration file error.      */ 
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::Init()
{
	char configPath[1024];
	int cfgFlag = 0;
#ifdef USE_AVISDF_CONFIG_FILE    // Jalan
	int rc, rc1, rc2, rc3, rc4; 
    char chTimerInterval[MAX_LENGTH];
	char chSampleRequestDelay[MAX_LENGTH];
	char chDatabaseRescanInterval[MAX_LENGTH];
	char chDefImportInterval[MAX_LENGTH];
	char chStatusUpdateInterval[MAX_LENGTH];
	char chUndeferrerInterval[MAX_LENGTH];
	char chAttributeInterval[MAX_LENGTH];
	char chSampleMax[MAX_LENGTH];
#endif

 	// computer name 
	DWORD bufSize =  MAX_COMPUTERNAME_LENGTH;
    if (!GetComputerName((LPTSTR)computerName, &bufSize))
		strcpy(computerName, "Unknown");
     
    startTime = CTime::GetCurrentTime();
	 HINSTANCE hInst = AfxGetInstanceHandle();
    (void) GetModuleFileName(hInst, configPath, 1024);
    char *p = strrchr(configPath, '\\');
    *p = '\0';
	strcpy(modulePath, configPath);
	CString cfgFile = CString((LPCSTR) IDS_CFG_FILE); 
    AppendPath(configPath, "avisdf.cfg"); 
	
	if ( Matrix.Init() != DF_SUCCESS)
		return(DF_EVALUATOR_FAILED);

#ifdef USE_AVISDF_CONFIG_FILE    // Jalan
    // config file is not found 
	if (IsFileFound(configPath)) {
		// config file is not found; I am creating the file with 
		// default values!
     int rc = SaveConfigDefaultSettings();
	 if (rc ==  DF_FILE_OPEN_ERROR)
        return (DF_CONFIG_FILE_CREATE_ERROR);
	 cfgFlag = 1;
	}	
#endif

	DFDBSelect selectSamples(this);
		
#ifdef USE_AVISDF_CONFIG_FILE    // Jalan
    // submission interval
	rc = selectSamples.ReadConfigFile(chTimerInterval,DFDBSelect::SampleSubmissionInterval);
	if (rc != DF_SUCCESS)
		return(DF_CONFIG_FILE_ERROR);
	sampleSubmissionInterval = atol(chTimerInterval); 
    if (sampleSubmissionInterval < DEFAULT_SAMPLE_SUBMISSION_INTERVAL || sampleSubmissionInterval > DEFAULT_SAMPLE_SUBMISSION_INTERVAL_MAX )
		sampleSubmissionInterval = (DEFAULT_SAMPLE_SUBMISSION_INTERVAL);
	// arrival check interval 
	rc = selectSamples.ReadConfigFile(chSampleRequestDelay,DFDBSelect::SampleArrivalCheckInterval);
	if (rc != DF_SUCCESS)
		return(DF_CONFIG_FILE_ERROR); 
	sampleArrivalCheckInterval = atol(chSampleRequestDelay); 
    if ( sampleArrivalCheckInterval <= 0 || sampleArrivalCheckInterval > DEFAULT_SAMPLE_ARRIVAL_INTERVAL_MAX )
        sampleArrivalCheckInterval = (DEFAULT_SAMPLE_ARRIVAL_INTERVAL) ;  
	// database rescan interval 
	rc = selectSamples.ReadConfigFile(chDatabaseRescanInterval,DFDBSelect::DatabaseRescanInterval);
	if (rc != DF_SUCCESS)
		return(DF_CONFIG_FILE_ERROR); 
	databaseRescanInterval = atol(chDatabaseRescanInterval); 
    if ( databaseRescanInterval <= 0 || databaseRescanInterval > DEFAULT_DATABASE_RESCAN_INTERVAL_MAX )
        databaseRescanInterval = (DEFAULT_DATABASE_RESCAN_INTERVAL) ;  

	// defImport interval 
	rc = selectSamples.ReadConfigFile(chDefImportInterval,DFDBSelect::DefImportInterval);
	if (rc != DF_SUCCESS)
		return(DF_CONFIG_FILE_ERROR); 
	defImportInterval = atol(chDefImportInterval); 
    if ( defImportInterval <= 0 || defImportInterval > DEFAULT_DEF_IMPORT_INTERVAL_MAX )
        defImportInterval = (DEFAULT_DEF_IMPORT_INTERVAL) ;  
	// statusUpdate interval 
	rc = selectSamples.ReadConfigFile(chStatusUpdateInterval,DFDBSelect::StatusUpdateInterval);
	if (rc != DF_SUCCESS)
		return(DF_CONFIG_FILE_ERROR); 
	statusUpdateInterval = atol(chStatusUpdateInterval); 
    if ( statusUpdateInterval <= 0 || statusUpdateInterval > DEFAULT_STATUS_UPDATE_INTERVAL_MAX )
        statusUpdateInterval = (DEFAULT_STATUS_UPDATE_INTERVAL) ;  
	// undeferrer interval 
	rc = selectSamples.ReadConfigFile(chUndeferrerInterval,DFDBSelect::UndeferrerInterval);
	if (rc != DF_SUCCESS)
		return(DF_CONFIG_FILE_ERROR); 
	undeferrerInterval = atol(chUndeferrerInterval); 
    if ( undeferrerInterval <= 0 || undeferrerInterval > DEFAULT_UNDEFERRER_INTERVAL_MAX )
        undeferrerInterval = (DEFAULT_UNDEFERRER_INTERVAL) ;  
	// attribute interval 
	rc = selectSamples.ReadConfigFile(chAttributeInterval,DFDBSelect::AttributesInterval);
	if (rc != DF_SUCCESS)
		return(DF_CONFIG_FILE_ERROR); 
	attributeInterval = atol(chAttributeInterval); 
    if ( attributeInterval <= 0 || attributeInterval > DEFAULT_ATTRIBUTE_INTERVAL_MAX )
        attributeInterval = (DEFAULT_ATTRIBUTE_INTERVAL) ;  



    //samples -- max
	rc = selectSamples.ReadConfigFile(chSampleMax,DFDBSelect::ConcurrentNumberOfSamplesToBeProcessed);
	if (rc != DF_SUCCESS)
		return(DF_CONFIG_FILE_ERROR); 
	maxSamples = atol(chSampleMax); 
    if ( maxSamples > DEFAULT_SAMPLES_MAX )
        maxSamples= DEFAULT_SAMPLES;  
    // buildDefFilename
	rc1 = selectSamples.ReadConfigFile(buildDefFilename,DFDBSelect::BuildDefFilename);
    // defBaseDir
	rc2 = selectSamples.ReadConfigFile(defBaseDir,DFDBSelect::DefBaseDir);
    // UNC path
	rc3 = selectSamples.ReadConfigFile(UNCPath,DFDBSelect::UNCPATH);
    // def importer dir
	rc4 = selectSamples.ReadConfigFile(defImporterDir,DFDBSelect::DefImporterDir);

	if (rc != DF_SUCCESS)	
		return(DF_CONFIG_FILE_ERROR); 
#else
        dirCheckInterval = DEFAULT_DIR_CHECK_INTERVAL ;
		cacheCheckInterval = DEFAULT_CACHE_CHECK_INTERVAL;
		sampleSubmissionInterval = (DEFAULT_SAMPLE_SUBMISSION_INTERVAL);
		databaseRescanInterval = (DEFAULT_DATABASE_RESCAN_INTERVAL);
        sampleArrivalCheckInterval = ( DEFAULT_SAMPLE_ARRIVAL_INTERVAL );  
        defImportInterval = ( DEFAULT_DEF_IMPORT_INTERVAL );  
        statusUpdateInterval = ( DEFAULT_STATUS_UPDATE_INTERVAL );  
        undeferrerInterval = ( DEFAULT_UNDEFERRER_INTERVAL );  
        attributeInterval = ( DEFAULT_ATTRIBUTE_INTERVAL );  

        maxSamples= DEFAULT_SAMPLES;  
        strcpy (UNCPath , "\\\\rushmore\\srvroot\\data");
#endif

#ifdef _DEBUG
      	strcpy(logPath, UNCPath);
		if (strcmp(UNCPath, "") != 0) {
		  AppendPath(logPath, "avisdf.log");
		  testFile = new std::ofstream;
		  testFile->open(logPath, std::ios::out | std::ios::app);
 		  *(testFile) << "Sample Activity Testing" << "\n";
		  testFile->flush();
          testFile->close(); 
        }


#endif
			
#ifndef DATABASE_INTERFACE

	int retcode = DFDBRequests::DbOpen();
    if (retcode == -1)
		return (DF_DABASE_OPEN_ERROR) ;
	int numberOfStates = Matrix.GetNumberOfStates();
	CString stateName;
    for (int i = 1; i <= numberOfStates; i++) {
		DFEvalState::StateType type = Matrix.GetStateType(i);
		if ( type == DFEvalState::PROCESSING) {
			stateName = Matrix.GetStateName(i);
			CString strStarted = Matrix.GetProcessingStatus((LPTSTR) (LPCTSTR)stateName);
			if (strStarted.CompareNoCase(CString("NULL")) != 0) {
	     		rc = DFDBRequests::UpdateAnalysisStateInfoTable(strStarted);
				if (rc != 0)
					return(DF_DABASE_ADD_NEW_STATE_ERROR); 
			} 
			CString strFinished = Matrix.GetEndStatus((LPTSTR) (LPCTSTR)stateName);
			if (strFinished.CompareNoCase(CString("NULL")) != 0) {
				rc = DFDBRequests::UpdateAnalysisStateInfoTable(strFinished);
				if (rc != 0)
				   return(DF_DABASE_ADD_NEW_STATE_ERROR); 
            }
        }
         
    }
#else
	int numberOfStates = Matrix.GetNumberOfStates();
	CString stateName;
    for (int i = 1; i <= numberOfStates; i++) {
		DFEvalState::StateType type = Matrix.GetStateType(i);
		if ( type == DFEvalState::PROCESSING ){
			stateName = Matrix.GetStateName(i);
  
			CString strStarted = Matrix.GetProcessingStatus((LPTSTR) (LPCTSTR)stateName);
			CString strFinished = Matrix.GetEndStatus((LPTSTR) (LPCTSTR)stateName);
			try
			{
				if (strStarted.CompareNoCase(CString("NULL"))) {
					std::string	startedName((LPTSTR) (LPCTSTR) strStarted);
					AnalysisStateInfo::AddNew(startedName);
				}
				if (strFinished.CompareNoCase(CString("NULL"))) {
					std::string finishedName((LPTSTR) (LPCTSTR) strFinished);
				    AnalysisStateInfo::AddNew(finishedName);    
                } 

			}
			catch (AVISDBException exception)
			{
					char msg[1028];
                 	detailedString = exception.DetailedInfo();
		            std::ostrstream inMsg(msg, sizeof(msg) - 1);
        			inMsg << detailedString;
					inMsg << std::ends;
					AfxMessageBox(msg, MB_ICONSTOP, 0);
					return(DF_DABASE_ADD_NEW_STATE_ERROR); 
        
			}

        }
    }
    int dsn =  GetLatestSeqNumber();
	if (dsn == 0)
		return (DF_LATEST_SIGNATURE_ERROR);

    CString strDsn;
	strDsn.Format("%d", dsn);
	defSeqNumber = strDsn;

#endif
	deferring = Matrix.FindFirstStateInGroup(DFEvalState::DEFER);  
	if (deferring == CString())
		return(DF_EVALUATOR_FAILED);
	newSample = Matrix.FindFirstStateInGroup(DFEvalState::ANALYSIS);  
	if (newSample == CString())
		return(DF_EVALUATOR_FAILED);
	scan = Matrix.FindFirstStateInGroup(DFEvalState::SCAN);  
	if (scan == CString())
		return(DF_EVALUATOR_FAILED);
	serializing = Matrix.FindFirstStateInGroup(DFEvalState::BUILD);  
	if (serializing == CString())
		return(DF_EVALUATOR_FAILED);
	import = Matrix.FindFirstStateInGroup(DFEvalState::IMPORT);  
	if (import == CString())
		return(DF_EVALUATOR_FAILED);

	undeferrer = Matrix.FindFirstStateInGroup(DFEvalState::UNDEFER);
	if (undeferrer  == CString())
		return(DF_EVALUATOR_FAILED);

	statusupdate = Matrix.FindFirstStateInGroup(DFEvalState::UPDATE);;
	if (statusupdate == CString())
		return(DF_EVALUATOR_FAILED);

	attributecollect = Matrix.FindFirstStateInGroup(DFEvalState::COLLECT);
	if (attributecollect == CString())
		return(DF_EVALUATOR_FAILED);
    // attributeFile
   	strcpy(attributeFile, UNCPath);
	if (strcmp(attributeFile, "") != 0) {
		AppendPath(attributeFile, "SampleAttributes\attribute.txt");
	}

    if (cfgFlag)
       return (DF_CONFIG_FILE_HAS_BEEN_CREATED); 
		
	if (rc1 == DF_UNC_EMPTY || rc2 == DF_BUILD_DEF_FILENAME_EMPTY || rc3 == DF_BASE_DEF_DIR_EMPTY  ||
		rc4 == DF_DEF_IMPORTER_DIR_EMPTY) { 
		return(rc1); 
    }
     

	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindObjectWithIdInHPList                              */
/* Description:         To find a sample object in the sample collection      */
/*                                                                            */
/* Input:               CString cookieId  - sample's id                       */
/*                      POSITION  &pos - position in the collection           */
/* Output:              None                                                  */
/* Return:              pointer to DFSample object. If NULL - object in not in*/
/*                      the collection                                        */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFSample *DFManager::FindObjectWithIdInHPList(CString cookieId, POSITION& pos)
{
     
		pos = sampleHPList.GetHeadPosition();
	    int i = 0;
		DFSample* sampleObject;
		while (pos != NULL)
		{
			sampleObject = sampleHPList.GetNext(pos);
			if (sampleObject->GetCookieId() == cookieId)
               return sampleObject;
		
		}

		return NULL;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetNewSamplesForProcessing                            */
/* Description:         To get new samples with "imported" status             */
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              0 - no new samples; DF_SAMPLE_ARRIVED - new samples   */
/*                      arrived                                               */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::GetNewSamplesForProcessing()
{
		int HasSampleArrived = 0;

        if (maxSamples == 0)
			return (HasSampleArrived);
		int rc;
		DFDBSelect selectSamples(this);
		DFSample cookie;

		int neededNumber = maxSamples  - sampleHPList.GetCount() - 1;
        if (neededNumber >= 0) {
			
			rc = selectSamples.DFImportSamplesFromDB(1, DFSample::IMPORTED);
			if (rc == -1 )
				return (DF_GET_SAMPLE_ERROR);


			
			while (sampleHPList.GetCount() < maxSamples  && selectSamples.arrivedList.GetCount() !=0 ) {

				int rc =  selectSamples.SelectFirst(cookie);
				if (rc != DF_SUCCESS) 
					continue;

				CString cookieId = cookie.GetCookieId();
				CString state = cookie.GetCurrentStateStr();
				DFSample::SampleStatus status = cookie.GetCurrentStatus();
				DFSample::Origin origin = cookie.GetOrigin();
				int priority = cookie.GetPriority();
				CTime t = cookie.GetArrivalTime();   
				rc = ProcessArrivedCookie(cookieId, state, status, priority, origin, t);   
				if (rc != 0) 
					 HasSampleArrived = DF_SAMPLE_ARRIVED;
								
			}
        }
		return(HasSampleArrived);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetNewSamplesForRescan                                */
/* Description:         To get new samples with "rescan" status               */
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              0 - no new samples; DF_SAMPLE_ARRIVED - new samples   */
/*                      arrived                                               */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::GetNewSamplesForRescan()
{
		int HasSampleArrived = 0;

        if (maxSamples == 0)
			return (HasSampleArrived);

		int rc;
		DFDBSelect selectSamples(this);
		DFSample cookie;

		if (sampleHPList.GetCount()	== maxSamples + 1) 
	      return ( HasSampleArrived );

		rc = selectSamples.DFImportSamplesFromDB(1, DFSample::RESCAN);
		if (rc == -1 )
			return (DF_GET_SAMPLE_ERROR);

		while (selectSamples.arrivedList.GetCount() !=0 ) {

            int rc =  selectSamples.SelectFirst(cookie);
			if (rc != DF_SUCCESS) 
				continue;

			CString cookieId = cookie.GetCookieId();
			CString state = cookie.GetCurrentStateStr();
			DFSample::SampleStatus status = cookie.GetCurrentStatus();
			DFSample::Origin origin = cookie.GetOrigin();
            int priority = cookie.GetPriority();
            CTime t = cookie.GetArrivalTime();   
            rc = ProcessArrivedCookie(cookieId, state, status, priority, origin, t);   
	        if (rc != 0) 
				 HasSampleArrived = DF_SAMPLE_ARRIVED;
							
        }




        return ( HasSampleArrived );
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      RecordErrorToLog                                      */
/* Description:         Record Error to the eval.log                          */
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
int DFManager::ReportToLog(long report, long error, const char *string)
{
   char logPath[MAX_PATH]; 

   HINSTANCE hInst = AfxGetInstanceHandle();
   (void) GetModuleFileName(hInst, logPath, 1024);
   char *p = strrchr(logPath, '\\');
   *p = '\0';
   AppendPath(logPath, "dfgui.cfg"); 

   AppendPath(logPath, LOGFILE);
   std::ofstream ofile( logPath, std::ios::app);
   if (!ofile) {
       return(DF_FILE_OPEN_ERROR);
   } 
	   //file is open 
    CString msg;  
    msg.LoadString(report);
	if (string == NULL)
		ofile << msg <<  "\n";
    else
    	ofile << msg << " " << string <<  "\n";
	   ofile.close();
    return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      AppendPath                                            */
/* Description:         To append one path to another one                     */
/*                                                                            */
/* Input:               lpszFirst: Specifies the first path to which second   */ 
/*                                   path is to be appended.                  */
/*                        lpszSecond: Specifies the path to be appenced to    */ 
/*						            lpszFirst                                 */ 
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              pszFirst: Containst he resultant path on return.      */
/*                                                                            */
/* Global References:   None                                                  */
/* Notes: This function combines two paths and takes care of trailing         */
/*          slashes in both of them. It ensures that when the two             */ 
/*          paths are appended, there is a single slash separating            */
/*          them. For ex: if first path has a trailing slash and              */
/*          second path has a starting slash, it ensures to remove            */
/*          one of the slashes. Similarly, if first path does not             */
/*          have a trailing slash and second path does not have               */
/*          a starrting slash, a single slash is appended to the first        */
/*          path before appending the second path.                            */ 
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFManager::AppendPath(char *lpszFirst,char *lpszSec)
{
        if(lpszFirst == NULL || lpszSec == NULL) return;

        int nLen1 = strlen(lpszFirst);

        // If last char in First is not a slash and first char
        // in Sec is not a slash, append slash.
                                        
        if(lpszFirst[nLen1-1] != '\\' && lpszSec[0] != '\\')                                                                                           
               strcat(lpszFirst,"\\");
        else if(lpszFirst[nLen1-1] == '\\' && lpszSec[0] == '\\')                                                   
                lpszFirst[nLen1-1] = 0;

        strcat(lpszFirst,lpszSec);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      ProcessArrivedCookie                                  */
/* Description:         To add a new cookie to the collection                 */
/*                                                                            */
/* Input:               CString cookieId - sample id                          */
/*						CString state  - sample's state                       */
/*						DFSample::SampleStatus status - sample's status       */
/*						DFSample::SamplePrior prior - collection priority,    */
/*						DFSample::Origin origin  - where the sample came from */
/*                      CTime t  - arrival time                               */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - no samples added; 1 -  sample has been added.     */
/* Global References:   None                                                  */
/* Notes:  collection priority is what has been left from the idea to have two*/
/* separate collection: high and low priority samples (I'm keeping it for the */
/* future)                                                                    */
/* Origin: samples can enter the processing loop as new ones (imported), or   */  
/* coming after def queue, or coming after def compiler. As for now all the   */  
/* samples are "imported". I'm keeping the paramenters.              .        */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::ProcessArrivedCookie(CString cookieId, CString state, DFSample::SampleStatus status, int priority, DFSample::Origin origin, CTime t )
{
    int isAdded = 0; 
	char logPath[MAX_LENGTH];
	char cookieLog[MAX_LENGTH];
    char cookiePath[MAX_LENGTH];
    char cookieUNC[MAX_LENGTH];
	POSITION pos;
	DFSample *sampleObject;
    pos = sampleHPList.GetHeadPosition();
	currentPos = pos;
      //List is empty
	if (pos == NULL) {
		strcpy(logPath, UNCPath);
		CString modId(cookieId); 
        while (modId.GetLength() < 8)
           modId.Insert(0, '0');
		AppendPath(logPath, (LPTSTR) (LPCTSTR) modId);
		strcpy(cookiePath, logPath);
		strcpy(cookieUNC, cookiePath);
		strcpy(cookieLog, cookieId);
		strcat(cookieLog, ".log"); 
		//path for the log file
		AppendPath(logPath, cookieLog);
		// path for the status file
//		AppendPath(cookiePath, "dfstatus.dat");

		sampleObject = new DFSample(cookieId, state , status);
		sampleObject->unc = CString(cookieUNC);
		sampleObject->dfstatus = new std::ofstream;
		//sampleObject->dfstatus->open(cookiePath, std::ios::out | std::ios::app);


#ifdef _DEBUG
		sampleObject->ofile = new std::ofstream;
	    sampleObject->ofile->open(logPath, std::ios::out | std::ios::app);
		*(sampleObject->ofile) << "MessageTesting; TestCodes: 222-submit, 111-ignore, 777-complete  " << "\n";
 		sampleObject->ofile->flush();
#endif
        
		sampleHPList.AddTail(sampleObject);
        currentPos = sampleHPList.GetHeadPosition();          
		if (origin == DFSample::IMPORTED) {
			sampleObject->SetCurrentStateStr(state);
			int intState = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)newSample);
			sampleObject->SetCurrentState(intState);
			CString strState = sampleObject->GetCurrentStateStr();
			CString nextState = Matrix.GetNextStateStr(strState,"WAIT");
			sampleObject->SetNextStateStr(nextState);		  
			sampleObject->SetPriority(priority);		  
			sampleObject->SetScanRequired(TRUE);

		
		}
		if (origin == DFSample::RESCAN) {
			sampleObject->SetCurrentStateStr(state);
			int intState = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)scan);
			sampleObject->SetCurrentState(intState);
			sampleObject->SetNextStateStr("RESCAN");		  
			sampleObject->SetPriority(priority);		  
			sampleObject->SetScanRequired(TRUE);

		
		}

	    sampleObject->SetOrigin(origin);      
	    sampleObject->SetArrivalTime(t);		  
		// there is no report to the database 
	    (void)ReportToStatusDatabase(sampleObject, ARRIVAL, FALSE);
	 
	    isAdded = 1; 
           
      }
	  else {

	    int i = 0;
	    DFSample* sampleObject;
		int found = 0;
	    while (pos != NULL)
		{   
			POSITION prevPos = pos;
			sampleObject = sampleHPList.GetNext(pos);
		    CString Id = sampleObject->GetCookieId();
			if (priority <= sampleObject->GetPriority()) {
				 currentPos = prevPos;
            } 
			if (Id == cookieId) 
				found = 1;
		
		}
		if (found == 0 ) {
			strcpy(logPath, UNCPath);
			CString modId(cookieId); 
			while (modId.GetLength() < 8)
			   modId.Insert(0, '0');
			AppendPath(logPath, (LPTSTR) (LPCTSTR) modId);
			strcpy(cookiePath, logPath);
			strcpy(cookieUNC, cookiePath);
			strcpy(cookieLog, cookieId);
			strcat(cookieLog, ".log"); 
			//path for the log file
			AppendPath(logPath, cookieLog);
			// path for the status file
//			AppendPath(cookiePath, "dfstatus.dat");

			sampleObject = new DFSample(cookieId, state , status);
			sampleObject->unc = CString(cookieUNC);

			sampleObject->dfstatus = new std::ofstream;
//			sampleObject->dfstatus->open(cookiePath, std::ios::out | std::ios::app);
    		
#ifdef _DEBUG
			sampleObject->ofile = new std::ofstream;
			sampleObject->ofile->open(logPath, std::ios::out | std::ios::app);
	 		 *(sampleObject->ofile) << "MessageTesting; TestCodes: 222-submit, 111-ignore, 777-complete  " << "\n";
			 sampleObject->ofile->flush();
#endif
		
           //sampleHPList.AddTail(sampleObject);
		   sampleHPList.InsertAfter(currentPos,sampleObject);
		   if (origin == DFSample::IMPORTED) {
			    sampleObject->SetCurrentStateStr(state);
				int intState = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)newSample);
				sampleObject->SetCurrentState(intState);
				CString strState = sampleObject->GetCurrentStateStr();
				CString nextState = Matrix.GetNextStateStr(strState,"WAIT");
				sampleObject->SetNextStateStr(nextState);		  
				sampleObject->SetPriority(priority);		  
				sampleObject->SetScanRequired(TRUE);
		    }
		   if (origin == DFSample::RESCAN) {
				sampleObject->SetCurrentStateStr(state);
				int intState = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)newSample);
				sampleObject->SetCurrentState(intState);
				sampleObject->SetNextStateStr("RESCAN");		  
				sampleObject->SetPriority(priority);		  
				sampleObject->SetScanRequired(TRUE);

		    }

			sampleObject->SetOrigin(origin);      
			sampleObject->SetArrivalTime(t);		  
	 		(void) ReportToStatusDatabase(sampleObject, ARRIVAL, FALSE);
	 
		     isAdded = 1;
          
		 } 
		 
	 }
     return (isAdded);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SubmitSamplesForProcessing                            */
/* Description:         To submit jobs                                        */ 
/*                                                                            */
/* Input:                                                                     */ 
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - status of jobs statyes the same. 1 - there are    */
/*                      changes                                               */ 
/* Global References:   None                                                  */
/* Notes: a job submitted to the locator has the unc path in the following    */
/* format: AAAAAAAA. If number of chars less then eight, the '0' are added.   */  
/* The cookie id is the tracking number id.                                   */ 
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::SubmitSamplesForProcessing()
{ 
    char filePath[MAX_LENGTH];
    char unc[MAX_LENGTH];
	char cookieLog[MAX_LENGTH];
	CString reportStatus;
    int nCount;
    int submitFlag = 0; 
	int HasStatusChanged = 0;
	bgtrap = 0;     		
	POSITION pos1, pos2;
	DFSample* pSample;
	//if (prior == DFSample::HIGH)
	nCount = (WORD)sampleHPList.GetCount();
    //	else
    //  nCount = (WORD)sampleList.GetCount();
     //to decrease the trafic let's keep track of submisstion to a particular
     //state only once during the loop iteration 	 
    Matrix.FreeStates();

	CTimeSpan resubInterval = CTimeSpan(0, 0,  1, 0); 
    CTimeSpan complTimeOut  = CTimeSpan(0, 0, 10, 0);  
	for (pos1 = sampleHPList.GetHeadPosition(); (pos2 = pos1) != NULL;)
	{
#ifdef _DEBUG
#endif
		submitFlag = 0;
		pSample = sampleHPList.GetNext(pos1);
       
		CString cookieId = pSample->GetCookieId();
		//strcpy(unc, UNCPath);
		//AppendPath(unc, (LPTSTR)(LPCTSTR)cookieId);
		//CString uncString(unc);

		DFSample::SampleStatus currentStatus = pSample->GetCurrentStatus();
		CString strState = pSample->GetCurrentStateStr();
		//int state = pSample->GetCurrentState();
	
		if (currentStatus == DFSample::WAIT) {
			if (pSample->GetUserRequest() == DFSample::PAUSE || pSample->GetUserRequest() == DFSample::STOP) 
                continue;
			if (pSample->GetScanRequired()) {
					(void) SubmitScanJob(pSample);
						HasStatusChanged = 1;
						continue;
                
            }
			// A SAMPLE brought from the db for scanning was unsuccessfully scanned.
			// I am changing the state of the sample BACK to 'IMPORTED' and deleting 
			// the sample from the collection.
   			CString nextState = pSample->GetNextStateStr();
            if (nextState.CompareNoCase(CString("RESCAN")) == 0) {
			    pSample->SetSampleState(CString("imported"));
			    if (pSample->dfstatus)
					delete pSample->dfstatus;
				DFSample *pa;
				pa = sampleHPList.GetAt(pos2); 	
				sampleHPList.RemoveAt(pos2);
			    delete pa;
				++returnedBackSamples; 
			   	HasStatusChanged = 2;
				continue;

            } 

			if (pSample->GetUserRequest() == DFSample::DEFER) {
				if (pSample->jobList.GetCount() != 0)  {
						POSITION position = pSample->jobList.GetHeadPosition();
						POSITION pos1, pos2;
						DFJob* pJob;
						for (pos1 = pSample->jobList.GetHeadPosition(); (pos2 = pos1) != NULL;)
						{
							pJob = pSample->jobList.GetNext(pos1);
							pSample->jobList.RemoveAt(pos2);
							delete pJob;
						}
						pSample->SetNextStateStr(deferring);
						pSample->SetDoneJobs(0);
  						pSample->SetSuccessJobs(0);
						pSample->SetInitCond(0);
						pSample->SetInProgressJobs(0);
                }
            } 
			   	
			if (pSample->jobList.GetCount() == 0)  {
				CString nextState = pSample->GetNextStateStr();
				if (Matrix.IsStateBusy(nextState))
                    continue;
				else {
				   Matrix.SetState(nextState, TRUE);
				}

				int iNextState = Matrix.FindMappingNumber((LPTSTR) (LPCTSTR)nextState);
   				pSample->SetSeqNumber();
 				int seqNumber = pSample->GetSeqNumber();
   				CString command = CString(cookieId);
				DFJob *pJob = new DFJob(seqNumber, nextState, cookieId, command, pSample->unc);
				pSample->jobList.AddTail(pJob); 
			    int initCond = pSample->jobList.GetCount();                       
			    pSample->SetInitCond(initCond);           
			   // Launcher
				if (SerializeSubmit(pJob, pSample)) {
					pJob->SetStatus(DFJob::SUBMITTED);
					CTime t = CTime::GetCurrentTime();
   					pJob->SetSubmitTime(t);
					DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), command, pSample->unc);    
					int rc =  SubmitJob(transitJob, pSample);
					 // there is no report to the database 
					(void) ReportToStatusDatabase(pSample, SUBMISSION, FALSE);
                } 
		 	   //status of a sample has not been changed. It will be changed when at least
			   //one job is accepted.
				HasStatusChanged = 1;
			} 
			else {
            	// for samples with postponed jobs or left after issuing immediate hold
				POSITION pos = pSample->jobList.GetHeadPosition();
         		int i = 0;
				DFJob *pJob;
				while (pos != NULL) {
					pJob = pSample->jobList.GetNext(pos);
					CString id = pJob->GetJobId();
	    		
					if ( pJob->GetStatus() == DFJob::POSTPONED) {
							CTime ct = CTime::GetCurrentTime();
							CTime pt = pJob->GetPostponedTime();
							CTimeSpan diff = ct - pt;
							long sec = diff.GetTotalSeconds();
							if (sec > JOB_RESUBMIT_INTERVAL ) { 
								if (Matrix.IsStateBusy(id))
									break;
								else {
									Matrix.SetState(id, TRUE);
								}
							 //  Launcher
								if (SerializeSubmit(pJob, pSample)) {
									pJob->SetStatus(DFJob::SUBMITTED);
                            		DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), pJob->GetCommandArg(), pSample->unc);    
									int rc =  SubmitJob(transitJob, pSample);
                                } 
      							HasStatusChanged = 1;
							}
					}
					else {
						//jobs after im.hold should be in "undefined" status
						if (SerializeSubmit(pJob, pSample)) {
							pJob->SetStatus(DFJob::SUBMITTED);
							DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), pJob->GetCommandArg(), pSample->unc);    
							int rc =  SubmitJob(transitJob, pSample);
                        }
						HasStatusChanged = 1;
					}			

                }
			}       

        }
		if ((currentStatus == DFSample::CriticalError && deferCriticalSample) || (currentStatus == DFSample::CriticalError && pSample->deferCriticalSample)) {
			if (pSample->GetUserRequest() == DFSample::PAUSE || pSample->GetUserRequest() == DFSample::STOP) 
                continue;

			//deferCriticalSample = 0;
			if (pSample->jobList.GetCount() != 0)  {
					POSITION position = pSample->jobList.GetHeadPosition();
					POSITION pos1, pos2;
					DFJob* pJob;
					for (pos1 = pSample->jobList.GetHeadPosition(); (pos2 = pos1) != NULL;)
					{
						pJob = pSample->jobList.GetNext(pos1);
						pSample->jobList.RemoveAt(pos2);
						delete pJob;
					}
					pSample->SetDoneJobs(0);
  					pSample->SetSuccessJobs(0);
					pSample->SetInitCond(0);
					pSample->SetInProgressJobs(0);
            } 
			if (pSample->serializeStateFlag) {
				pSample->SetNextStateStr(serializing);
				pSample->SetCurrentStatus(DFSample::SUCCESS);
	    	    pSample->serializeStateFlag = 0;
            }
	        else 
			    pSample->SetNextStateStr(deferring);
			   	
			if (pSample->jobList.GetCount() == 0)  {
				CString nextState = pSample->GetNextStateStr();
				if (Matrix.IsStateBusy(nextState))
                    continue;
				else {
				   Matrix.SetState(nextState, TRUE);
				}

				int iNextState = Matrix.FindMappingNumber((LPTSTR) (LPCTSTR)nextState);
   				pSample->SetSeqNumber();
 				int seqNumber = pSample->GetSeqNumber();
   				CString command = CString(cookieId);
				DFJob *pJob = new DFJob(seqNumber, nextState, cookieId, command, pSample->unc);
				pSample->jobList.AddTail(pJob); 
			    int initCond = pSample->jobList.GetCount();                       
			    pSample->SetInitCond(initCond);           
			   // Launcher
				if (SerializeSubmit(pJob, pSample)) {
					pJob->SetStatus(DFJob::SUBMITTED);
					CTime t = CTime::GetCurrentTime();
   					pJob->SetSubmitTime(t);
					DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), command, pSample->unc);    
					int rc =  SubmitJob(transitJob, pSample);
					 // there is no report to the database 
					//(void) ReportToStatusDatabase(pSample, SUBMISSION, FALSE);
                } 
		 	   //status of a sample has not been changed. It will be changed when at least
			   //one job is accepted.
				HasStatusChanged = 1;
			} 
			else {
            	// for samples with postponed jobs or left after issuing immediate hold
				POSITION pos = pSample->jobList.GetHeadPosition();
         		int i = 0;
				DFJob *pJob;
				while (pos != NULL) {
					pJob = pSample->jobList.GetNext(pos);
					CString id = pJob->GetJobId();
	    		
					if ( pJob->GetStatus() == DFJob::POSTPONED) {
							CTime ct = CTime::GetCurrentTime();
							CTime pt = pJob->GetPostponedTime();
							CTimeSpan diff = ct - pt;
							long sec = diff.GetTotalSeconds();
							if (sec > JOB_RESUBMIT_INTERVAL ) { 
								if (Matrix.IsStateBusy(id))
									break;
								else {
									Matrix.SetState(id, TRUE);
								}
							 //  Launcher
								if (SerializeSubmit(pJob, pSample)) {
									pJob->SetStatus(DFJob::SUBMITTED);
                            		DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), pJob->GetCommandArg(), pSample->unc);    
									int rc =  SubmitJob(transitJob, pSample);
                                } 
      							HasStatusChanged = 1;
							}
					}
					else {
						//jobs after im.hold should be in "undefined" status
						if (SerializeSubmit(pJob, pSample)) {
							pJob->SetStatus(DFJob::SUBMITTED);
							DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), pJob->GetCommandArg(), pSample->unc);    
							int rc =  SubmitJob(transitJob, pSample);
                        }
						HasStatusChanged = 1;
					}			

                }
			}       

        }

		if (currentStatus == DFSample::FINAL)  {
	   				if (pSample->GetUserRequest() == DFSample::PAUSE || pSample->GetUserRequest() == DFSample::STOP) 
		               continue;

			//		DFEvalState::StateGroup group = Matrix.GetStateGroup(pSample->GetPreviousStateStr());
			//		group = Matrix.GetStateGroup(pSample->GetNextStateStr());
			//		if ( group == DFEvalState::STOP ) {
			//			if (serialFlag && (!pSample->noBuildFlag)) 
			//			    serialFlag = FALSE;
            //            pSample->SetCurrentStatus(DFSample::CriticalError); 
            //       	    continue;
            //        }
					// critical error
					//DFEvalState::StateGroup group = Matrix.GetStateGroup(pSample->GetNextStateStr());
					//if ( group == DFEvalState::STOP ) {
						//pSample->SetCurrentStatus(DFSample::CriticalError);
						//pSample->criticalErrorFlag = 1;
						//mainWnd->PostMessage(DFMANAGER_BLOCK_PROCESSING , (WPARAM)0, (LPARAM)0); 
						//mainWnd->SendMessage(DFMANAGER_SEND_MAIL , 0, 0); 
					//	continue;
                    //}
					
					DFSample *pa;
					pa = sampleHPList.GetAt(pos2);
					strcpy(filePath, UNCPath);
					strcpy(cookieLog, (LPTSTR)(LPCTSTR)pa->GetCookieId());
					strcat(cookieLog, ".log"); 
    
                	AppendPath(filePath, cookieLog);
					 	
					if (serialFlag && (!pSample->noBuildFlag)) 
						    serialFlag = FALSE;
				
				    if (pSample->dfstatus)
						delete pSample->dfstatus;

                    sampleHPList.RemoveAt(pos2);
				    delete pa;
				   	HasStatusChanged = 2;
				
        }
     	if (currentStatus == DFSample::FAIL)  {
					if (pSample->GetUserRequest() == DFSample::PAUSE || pSample->GetUserRequest() == DFSample::STOP ) 
		               continue;
					// critical error
					//DFEvalState::StateGroup group = Matrix.GetStateGroup(pSample->GetNextStateStr());
					//if ( group == DFEvalState::STOP ) {
					//	mainWnd->SendMessage(DFMANAGER_BLOCK_PROCESSING , 0, 0); 
                     //   continue;
                    //}
			        DFSample *pa;
					pa = sampleHPList.GetAt(pos2);	
					strcpy(filePath, UNCPath);
					strcpy(cookieLog, (LPTSTR)(LPCTSTR)pa->GetCookieId());
					strcat(cookieLog, ".log"); 
    
                	AppendPath(filePath, cookieLog);
					//DFEvalState::StateGroup group = Matrix.GetStateGroup(pSample->GetCurrentStateStr());
			//		if (serialFlag && (!pSample->noBuildFlag)) 
			//				serialFlag = FALSE;
				


#ifdef _DEBUG
					pSample->ofile->close();
#endif
					if (pSample->dfstatus)
						delete pSample->dfstatus;

					sampleHPList.RemoveAt(pos2);
                    delete pa;
					
		     	    HasStatusChanged = 2;
		}
     	
		if (currentStatus == DFSample::SUCCESS)  {
			if (pSample->GetUserRequest() == DFSample::PAUSE || pSample->GetUserRequest() == DFSample::STOP) 
               continue;
#ifdef SCHEDULED_TASK_TEST
//test log
		CTime t = CTime::GetCurrentTime();
		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << " Next state is: " << *pSample->GetNextStateStr() <<"Pending flag =" << importPending << "Serial Flag =" << serialFlag <<"\n";
		logFile->flush();
		logFile->close();

#endif
       		if (pSample->GetNextStateStr() == serializing && !pSample->noBuildFlag && !packThread && serialFlag ) {
				if(closeWndRequest) { 
				   bgtrap = 1;
			       return 0;
            
                } 
			} 
	  
			if (pSample->GetNextStateStr() == serializing && importPending == 1 && serialFlag == FALSE) {
				(void) ImportNewDefinitions();
			     continue;
            } 
			if (pSample->GetScanRequired()) { 
				// a sample is before entering one of the Analysis 
				// of Build state
					(void) SubmitScanJob(pSample);
					HasStatusChanged = 1;
					continue;
            }

			if ( pSample->GetUserRequest() == DFSample::DEFER && pSample->GetNextStateStr() != deferring){
				if (pSample->jobNewList.GetCount() != 0)  {
						POSITION position = pSample->jobNewList.GetHeadPosition();
						POSITION pos1, pos2;
						DFJob* pJob;
						for (pos1 = pSample->jobNewList.GetHeadPosition(); (pos2 = pos1) != NULL;)
						{
							pJob = pSample->jobNewList.GetNext(pos1);
							pSample->jobNewList.RemoveAt(pos2);
							delete pJob;
						}
						pSample->SetNextStateStr(deferring);
				}
				if (pSample->jobList.GetCount() != 0)  {
  						POSITION position = pSample->jobList.GetHeadPosition();
						POSITION pos1, pos2;
						DFJob* pJob;
						for (pos1 = pSample->jobList.GetHeadPosition(); (pos2 = pos1) != NULL;)
						{
							pJob = pSample->jobList.GetNext(pos1);
							pSample->jobList.RemoveAt(pos2);
							delete pJob;
						}
						pSample->SetNextStateStr(deferring);
				}
				pSample->SetDoneJobs(0);
				pSample->SetSuccessJobs(0);
				pSample->SetInitCond(0);
				pSample->SetInProgressJobs(0);
            }
			
	      			//the jobs will be launched if the new List is not empty! 
			    DFJob *pNewJob;
				if (pSample->jobNewList.GetCount() != 0 ) {
					pNewJob = pSample->jobNewList.GetTail();
					CString state = pNewJob->GetJobId();
					if(SerializeSubmit(pNewJob, pSample)) {
						while (pSample->jobNewList.GetCount() != 0) {

			    			pNewJob = pSample->jobNewList.GetTail();
							CString state = pNewJob->GetJobId();
							CString command = pNewJob->GetCommandArg();
							pSample->SetSeqNumber();
							int seqNumber = pSample->GetSeqNumber();
							DFJob *pJob = new DFJob(seqNumber, state, cookieId, command, pSample->unc); 
							CTime t = CTime::GetCurrentTime();
 							pJob->SetSubmitTime(t);
							pJob->SetStatus(DFJob::SUBMITTED);

							strcpy(unc, UNCPath);
							AppendPath(unc, (LPTSTR)(LPCTSTR)cookieId);
							pJob->SetUNCPath(unc);
	        				pSample->jobList.AddTail(pJob); 
							// Launcher
#ifdef _DEBUG

							*(pSample->ofile) <<"1.NewList=" << pSample->jobNewList.GetCount() <<"\n";
							*(pSample->ofile) <<"1.List=" << pSample->jobList.GetCount() <<"\n";
							pSample->ofile->flush();
#endif
							DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), command, pSample->unc);
							int rc =  SubmitJob(transitJob, pSample);
							HasStatusChanged = 1;
							pSample->jobNewList.RemoveTail();
 							delete pNewJob;
						}
                    }
                }
				else {
			          // for samples coming after DefCompiler and DefQueue and after the hold 
					  // imm. request 
                      // New design 05/25/99:  
					  // At this point I don't have samples coming after
					  // Defcompiler and DefQueue, but I have samples for which the 
					  // Defer job has to be resubmitted or some other states, but always the  
					  // a state was determined with Evaluator. The type of the states is always
                      // 'Processing'. 
          			  // 
                      //  create a new job
                        if (pSample->jobList.GetCount() == 0 ) { 
                 			CString strState = pSample->GetNextStateStr();
							int state = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)strState);
							if (Matrix.IsStateBusy(strState))
							     continue;
							else {
								Matrix.SetState(strState, TRUE);
							}

							pSample->SetSeqNumber();
							int seqNumber = pSample->GetSeqNumber();
							CString command = CString();  
							DFJob *pJob = new DFJob(seqNumber, strState, cookieId, command, pSample->unc); 
							strcpy(unc, UNCPath);
				            AppendPath(unc, (LPTSTR)(LPCTSTR)cookieId);
				            pJob->SetUNCPath(unc);
							pSample->jobList.AddTail(pJob); 
							
                           // Launcher 
                           // command argument is a cookie id 
#ifdef _DEBUG
							*(pSample->ofile) <<"2.NewList=" << pSample->jobNewList.GetCount() <<"\n";
							*(pSample->ofile) <<"2.List=" << pSample->jobList.GetCount() <<"\n";
							pSample->ofile->flush();
#endif				
							if (SerializeSubmit(pJob, pSample)) {
								pJob->SetStatus(DFJob::SUBMITTED);
								CTime t = CTime::GetCurrentTime();
   								pJob->SetSubmitTime(t);        
 								DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), command, pSample->unc);    
								int rc =  SubmitJob(transitJob, pSample);
                            }           
		 				   //status of a sample has not been changed. It will be changed when at least
						   //one job is accepted.
 							HasStatusChanged = 1;

						}
						else {
						// for samples with postponed jobs and after the request to resume 
                        // hold imm jobs
							POSITION pos = pSample->jobList.GetHeadPosition();
         					int i = 0;
							DFJob *pJob;
							while (pos != NULL) {
								pJob = pSample->jobList.GetNext(pos);
								if ( pJob->GetStatus() == DFJob::POSTPONED) {
										
									CTime ct = CTime::GetCurrentTime();
									CTime pt = pJob->GetPostponedTime();
									CTimeSpan diff = ct - pt;
									long sec = diff.GetTotalSeconds();
									if (sec > JOB_RESUBMIT_INTERVAL ) { 
										if (Matrix.IsStateBusy(pJob->GetJobId()))
											 break;
										else {
											Matrix.SetState(pJob->GetJobId(), TRUE);
										}

										//Launcher
#ifdef _DEBUG
										*(pSample->ofile) <<"3.NewList=" << pSample->jobNewList.GetCount() <<"\n";
										*(pSample->ofile) <<"3.List=" << pSample->jobList.GetCount() <<"\n";
										pSample->ofile->flush();
#endif
										if (SerializeSubmit(pJob, pSample)) {
											pJob->SetStatus(DFJob::SUBMITTED);
											DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), pJob->GetCommandArg(), pSample->unc);    
											int rc =  SubmitJob(transitJob, pSample);
										}       
	        							HasStatusChanged = 1;
									}
		
								}
								else { 
									//jobs after im.hold have to be in "undefined" status
#ifdef _DEBUG
									*(pSample->ofile) <<"4.NewList=" << pSample->jobNewList.GetCount() <<"\n";
									*(pSample->ofile) <<"4.List=" << pSample->jobList.GetCount() <<"\n";
									pSample->ofile->flush();
#endif	
									if (SerializeSubmit(pJob, pSample)) {

										pJob->SetStatus(DFJob::SUBMITTED);
										DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), pJob->GetCommandArg(), pSample->unc);    
										int rc =  SubmitJob(transitJob, pSample);                                       
                                    }
	        						HasStatusChanged = 1;
                                }

							}
						}                              
                }	 
				int initCond = pSample->jobList.GetCount();                       
			    pSample->SetInitCond(initCond);           
		}
		if (currentStatus == DFSample::INPROGRESS)  {
              
			//	If I submitted jobs and the status of a sample has not chaged yet
			//  some of jobs might be postponed. Let's check them.		
			 
           	POSITION pos = pSample->jobList.GetHeadPosition();
         	int i = 0;
		    DFJob *pJob;
			int ignoreFlag = 0;
		    while (pos != NULL) {
			    pJob = pSample->jobList.GetNext(pos);
				// if there are jobs which the user wants to ignore putting a sample "ON HOLD with IGNORE"
                if ( pSample->GetUserRequest() == DFSample::STOP  && pSample->ignoreFlag == FALSE ) {
               	   if (pJob->GetStatus() == DFJob::ACCEPTED || pJob->GetStatus() == DFJob::SUBMITTED) {
#ifdef _DEBUG
	(void) MessageLogging(pJob->GetCookieId(), pJob->GetJobSeq(), "FromSubmit:InP/A/S", 999);
#endif
		                  //Launcher 
					     int rc; 
						 DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), CString(), pSample->unc);    
						 rc =  IgnoreJob(transitJob);
						 ignoreFlag = 1;
				      // we'll wait for ignore response to handle the number of "processed" and 
						 // "ignored" jobs
				   }
				   
                   if (pJob->GetStatus() == DFJob::POSTPONED )  {
#ifdef _DEBUG
	(void) MessageLogging(pJob->GetCookieId(), pJob->GetJobSeq(), "FromSubmit:InProg:Po", 999);
#endif

						 pJob->SetStatus(DFJob::FAIL);
						 // don't loose a job...
						 pSample->AddDoneJobs();
						 pSample->statusDet = TRUE;
                		 //pSample->SubtractInProgressJobs();
						 HasStatusChanged = 1;
                   }


                }  
                else {
					if ( pJob->GetStatus() == DFJob::POSTPONED) {
						CTime ct = CTime::GetCurrentTime();
						CTime pt = pJob->GetPostponedTime();
						CTimeSpan diff = ct - pt;
						long sec = diff.GetTotalSeconds();
						if (sec > JOB_RESUBMIT_INTERVAL ) { 
							if (Matrix.IsStateBusy(pJob->GetJobId()))
								 break;
							else {
							   Matrix.SetState(pJob->GetJobId(), TRUE);
							}
							if (SerializeSubmit(pJob, pSample)) {

								pJob->SetStatus(DFJob::SUBMITTED);
								//  Launcher 
 								DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), pJob->GetCommandArg(), pSample->unc);    
								int rc =  SubmitJob(transitJob, pSample);
                            }
		        			HasStatusChanged = 1;
						}
					}
				}

			}
			if (ignoreFlag == 1)
				pSample->ignoreFlag = TRUE;
		}
     }

	 deferCriticalSample = 0;
	

	 int rc = DetermineSampleStatusPlus();
	 if (rc == -1)
		 return rc;
	 return(HasStatusChanged);
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      SubmitJob                                             */
/* Description:         To post a submit message to the Job Locator           */
/*                                                                            */
/* Input:               DFJob *job - job object                               */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - success                                           */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::SubmitJob(DFJob *job, DFSample *pSample)
{
	char cookiePath[MAX_LENGTH];

	CString id = CString("");
	int seq = 0;
	CString jobId = CString("");
	if (job != NULL) {
		 id = job->GetCookieId();
		 seq = job->GetJobSeq();
		 jobId = job->GetJobId();
	}
	// correct the paramenter list 
	DFEvalState::StateGroup group = Matrix.GetStateGroup(job->GetJobId());
	if ( group == DFEvalState::FULLBUILD || group == DFEvalState::BUILD ) {
		CString command;
        CString tmpCommand;
        tmpCommand = job->GetCommandArg() + CString(" ") + CString(buildDefFilename);
		if (pSample->evalPathFlag) {
			command =  deferring + CString(" ") + tmpCommand; 
        } 
		else { 
			command =  tmpCommand; 
        }
		job->SetCommandArg(command);

	} 	
	//create dffinish.dat file at this stage for the Defer and Archive states
	if ( group == DFEvalState::DEFER  || group == DFEvalState::ARCHIVE ) {
		if (pSample->dffinishFlag  == 0 ) {
			strcpy(cookiePath, pSample->unc);
			AppendPath(cookiePath, "dffinish.dat");
			pSample->dffinish = new std::ofstream;
			pSample->dffinish->open(cookiePath, std::ios::out | std::ios::app);
			pSample->dffinish->close();
			delete pSample->dffinish;
			pSample->dffinishFlag = 1;
        } 
        
   }

#ifdef _DEBUG 
    (void) MessageLogging(id, seq, jobId, 222);
#endif
	CWnd *pDFLWnd = CWnd::FromHandle(DFLWnd);
	pDFLWnd->PostMessage(ALM_REQ_START_JOB, 0, (LPARAM) job);

    return 0;

}

/*----------------------------------------------------------------------------*/
/* Procedure name:      IgnoreJob                                             */
/* Description:         To post an ignore message to the job Locator          */
/*                                                                            */
/* Input:               DFJob *job - job object                               */
/*                                                                            */
/* Output:              0 - success                                           */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::IgnoreJob(DFJob *job)
{

#ifdef _DEBUG
	CString id = CString("");
	int seq = 0;
	CString jobId = CString("");
	if (job != NULL) {
	   id = job->GetCookieId();
       seq = job->GetJobSeq();
	   jobId = job->GetJobId();
    }
    (void) MessageLogging(id, seq, jobId, 111);
#endif

	CWnd *pDFLWnd = CWnd::FromHandle(DFLWnd);
	pDFLWnd->PostMessage(ALM_REQ_IGNORE_JOB, 0, (LPARAM) job);
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      ReportToStatusDatabase                                */
/* Description:         To report to the Status Database                      */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - success                                           */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/* Well, the status database is file "dfstatus.dat" ...as for today           */
/* I create dffinish.dat file even if the sample fails as a flag not to       */
/* process the sample again.                                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::ReportToStatusDatabase(DFSample *pSample, ReportStage stage, BOOL scanFlag /* =FALSE*/ )
{
	char cookiePath[MAX_LENGTH];
	char statusPath[MAX_LENGTH];
	CString cookie = pSample->GetCookieId();
	int localCookie = atoi((LPTSTR) (LPCTSTR) cookie);
	strcpy(statusPath, pSample->unc);
	AppendPath(statusPath, "dfstatus.dat");

	if (stage == ARRIVAL) {
		CTime t = pSample->GetArrivalTime(); 
	    CString arrivalTime = t.Format( "%d %B %Y,  %H:%M:%S" );
		pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
		*(pSample->dfstatus) << " Issue tracking number: " << (LPTSTR) (LPCTSTR) pSample->GetCookieId() <<   "\n";
		if ( pSample->GetOrigin() == DFSample::IMPORTED )
			*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) arrivalTime << " Sample " << (LPTSTR) (LPCTSTR) pSample->GetCookieId() <<  " was brought from DB as IMPORTED " << "\n";
		else
			*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) arrivalTime << " Sample " << (LPTSTR) (LPCTSTR) pSample->GetCookieId() << " was brought from DB as RESCAN " <<  "\n";

		pSample->dfstatus->flush();
		pSample->dfstatus->close();
		arrivedSamples++;
    } 

	if (stage == SUBMISSION) {
		CTime t = CTime::GetCurrentTime();
		pSample->SetSubmissionTime(t);
	    CString submissionTime = t.Format( "%d %B %Y,  %H:%M:%S" );
		pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
		*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) submissionTime << " Sample submitted " << "\n";
 		pSample->dfstatus->flush();
		pSample->dfstatus->close();
		strcpy(cookiePath, pSample->unc);
		AppendPath(cookiePath, "dfstart.dat");
		pSample->dfstart = new std::ofstream;
		pSample->dfstart->open(cookiePath, std::ios::out);
		pSample->dfstart->close();
		delete pSample->dfstart;
		pSample->pipelineFlag = 1; 


    } 
	if (stage == ACCEPTANCE) {
		CTime t = CTime::GetCurrentTime();
		//pSample->SetSubmissionTime(t);
	    CString acceptanceTime = t.Format( "%d %B %Y,  %H:%M:%S" );
		pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
	    CString currentState = pSample->GetCurrentStateStr();
		DFEvalState::StateGroup group = Matrix.GetStateGroup(currentState);
		if (group == DFEvalState::ARCHIVE && pSample->GetOrigin() == DFSample::IMPORTED) {
// 				*(pSample->dfstatus) <<  " The new built definition sequence number is: " 
//				<< (LPTSTR) (LPCTSTR) defSeqNumber << "\n";
        }
		if ( scanFlag ) {
			if (pSample->GetNextStateStr().CompareNoCase(scan) == 0) {
				*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) acceptanceTime << " DBRescan:Sample was accepted by the scanner" <<"\n";
            }
			else {
				if (pSample->pipelineFlag)
					*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) acceptanceTime << " Rescan:Sample was accepted by the scanner" <<"\n";
				else
					*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) acceptanceTime << " INRescan:Sample was accepted by the scanner" <<"\n";

            }
			*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) acceptanceTime << " The sample will be scanned with the DefSeqNumber = " << (LPTSTR) (LPCTSTR) defSeqNumber << "\n";
			pSample->dfstatus->flush();
			pSample->dfstatus->close();
			return 0;
        }
        else 
			*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) acceptanceTime << " Sample was accepted by " << 
				(LPTSTR) (LPCTSTR) pSample->GetAcceptedState() <<"\n";
		pSample->dfstatus->flush();
		pSample->dfstatus->close();
		CString strState =  pSample->GetAcceptedState();
		CString acceptedState;
		acceptedState = Matrix.GetProcessingStatus(strState);
		if(acceptedState.CompareNoCase(CString("NULL")) == 0) {
			return 0;
        }
		char cAcceptedState[256];
		strcpy(cAcceptedState, (LPTSTR) (LPCTSTR)acceptedState);

		try {
			AnalysisRequest request(localCookie);
			std::string strAnalysisState;
			strAnalysisState = std::string(cAcceptedState);
			AnalysisStateInfo stateObject( strAnalysisState );
			request.State(stateObject);
		}
		catch (AVISDBException exception) {
			char msg[1028];
            detailedString = exception.DetailedInfo();
	        std::ostrstream inMsg(msg, sizeof(msg) - 1);
       		inMsg << detailedString;
			inMsg << std::ends;
			AfxMessageBox(msg, MB_ICONSTOP, 0);
			return(IDS_DATABASE_UPDATE_RECORD_ERROR); 
        
		}

		//create dffinish.dat file at this stage for the Defer and Archive states
	    //currentState = pSample->GetCurrentStateStr();
		//group = Matrix.GetStateGroup(currentState);
		//if ( group == DFEvalState::DEFER  || group == DFEvalState::ARCHIVE ) {
	    //    strcpy(cookiePath, pSample->unc);
		//	AppendPath(cookiePath, "dffinish.dat");
		//	pSample->dffinish = new std::ofstream;
		//	pSample->dffinish->open(cookiePath, std::ios::out | std::ios::app);
		//	pSample->dffinish->close();
		//	delete pSample->dffinish;
        //
        //}
    } 
	if (stage == COMPLETION) {
		CTime t = CTime::GetCurrentTime();
		pSample->SetCompletionTime(t);
	    CString completionTime = t.Format( "%d %B %Y,  %H:%M:%S" );
		CString finishedState;
		finishedState = Matrix.GetEndStatus(pSample->GetCurrentStateStr());
		char cFinishedState[256];
		strcpy(cFinishedState, (LPTSTR) (LPCTSTR)finishedState);

		if (pSample->GetCurrentStatus() == DFSample::SUCCESS ||
			pSample->GetCurrentStatus() == DFSample::WAIT ) 
		{
//			if ( scanFlag ) {
//				pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
//				if (pSample->GetCurrentStateStr() == scan)
//					*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) completionTime << " DBRescan:Scanning completed" <<"\n";
//				else
//					*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) completionTime << " Rescan:Scanning completed" <<"\n";
//				pSample->dfstatus->flush();
//				pSample->dfstatus->close();
//				return 0;
//            }
       	    CString currentState = pSample->GetCurrentStateStr();
			if (finishedState.CompareNoCase(CString("NULL")) == 0) {
				return 0;
            }
			DFEvalState::StateGroup group = Matrix.GetStateGroup(currentState);

			try {
				std::string strAnalysisState;
				strAnalysisState = std::string(cFinishedState);
				AnalysisRequest request(localCookie);
				// The cookie that is at defer state gains the SUCCESS status,
				// even there is no result file for the job. The Deferrer 
				// program is supposed to update the database. It the database 
				// was not updated, a defer job has to be submitted again
				if ( group == DFEvalState::DEFER ) {
					AnalysisStateInfo cookieState = request.State();
				    AnalysisStateInfo deferState( strAnalysisState);
					if (cookieState != deferState) {
						pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
						*(pSample->dfstatus) << (LPTSTR) (LPCTSTR)completionTime  << " " <<  (LPTSTR) (LPCTSTR)pSample->GetCurrentStateStr() 
						<< " completed. The database has not been updated by the Deferrer program." << "\n";
						*(pSample->dfstatus) << "                            " << "Next state -- " << (LPTSTR) (LPCTSTR)pSample->GetNextStateStr() << "\n";  
						pSample->dfstatus->flush();
						pSample->dfstatus->close();
					
						return 0;
					}
					else  {
						++deferredSamples;
			    		pSample->SetCurrentStatus(DFSample::FINAL);
						return 0;
                    }
                
				
				}
				else {
					AnalysisStateInfo stateObject( strAnalysisState );
					request.State(stateObject);
                }
			}
			catch (AVISDBException exception) {
				char msg[1028];
				detailedString = exception.DetailedInfo();
				std::ostrstream inMsg(msg, sizeof(msg) - 1);
       			inMsg << detailedString;
				inMsg << std::ends;
				AfxMessageBox(msg, MB_ICONSTOP, 0);
				pSample->dfstatus->close();
				return(IDS_DATABASE_UPDATE_RECORD_ERROR); 
			}
			pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
			*(pSample->dfstatus) << (LPTSTR) (LPCTSTR)completionTime  << " " <<  (LPTSTR) (LPCTSTR)pSample->GetCurrentStateStr() 
			<< " completed with status " << (LPTSTR) (LPCTSTR) pSample->GetCurrentStatusStr(pSample->GetCurrentStatus()) << "\n";
			*(pSample->dfstatus) << "                            " << "Next state -- " << (LPTSTR) (LPCTSTR)pSample->GetNextStateStr() << "\n";  
				
			pSample->dfstatus->flush();
			pSample->dfstatus->close();

		}
		if (pSample->GetCurrentStatus() == DFSample::FAIL) {
		    CString currentState = pSample->GetCurrentStateStr();
			DFEvalState::StateGroup group = Matrix.GetStateGroup(currentState);

			if ( group != DFEvalState::ARCHIVE) {
				pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
			    *(pSample->dfstatus) <<  (LPTSTR) (LPCTSTR)completionTime << " " << (LPTSTR) (LPCTSTR) pSample->GetCurrentStateStr()  
				<< " completed with status FAIL" << "\n";
				*(pSample->dfstatus) <<"                            " << "(Next state -- " << (LPTSTR) (LPCTSTR) pSample->GetNextStateStr() << ")" << "\n";
				pSample->dfstatus->flush();
				pSample->dfstatus->close();

            }
			else {
				// the status of a sample is 'FAIL', because there is no 'result'
				// file for the 'archive job

				CString seqNumber = pSample->GetDefSeqNumber();
				CTimeSpan diff = pSample->GetCompletionTime() - pSample->GetSubmissionTime();
				long procTime = diff.GetTotalSeconds();
                double total;
                double preciseTime;
				if (seqNumber != CString()) {
                    total = (double) (successfulSamples + rescannedSamples);
                    ++successfulSamples; 
					preciseTime = ( (total * (double) averageProcessingTime) + (double) procTime ) / (total + 1);
					averageProcessingTime = (long) floor(preciseTime + 0.5);

					if ( defSeqNumber.CompareNoCase(seqNumber) ){
						// create a package, copy files, check the directory 
					    packSample = new DFSample(pSample->GetCookieId(),pSample->GetCurrentStateStr(),
						pSample->GetCurrentStatus()); 

                        strcpy(packSample->buildDefFilename, buildDefFilename); 
                        strcpy(packSample->defBaseDir, defBaseDir); 
						packSample->defSeqNumber = pSample->GetDefSeqNumber(); 

						//AfxBeginThread(CreatePackageAndSignaturFiles, pSample);
#ifdef _DEBUG
		(void) LogActivity(1, "Archiving started. CookieId=", pSample->GetCookieId());
#endif

						packThread = new CDFPackThread(this, packSample); 	
						if (packThread)
							packThread->Start();
						//packSample = pSample;
                        return 0;
                     	/*  thred changes
	                    if ( CreatePackageAndSignaturFiles(pSample))
							return(DF_PACKAGE_ERROR) ;
						 //add a new signature to the Signature table; add it to all 
						 //active servers 
						if (AddSignature(pSample))
	           				return(IDS_DATABASE_UPDATE_RECORD_ERROR); 
                        */   
                    }

				}
				else {
                    total = (double) successfulSamples + rescannedSamples;
                    ++rescannedSamples; 
					preciseTime = (( total * (double) averageProcessingTime) + (double) procTime ) / (total + 1);
					averageProcessingTime = (long) floor(preciseTime + 0.5);
                }
				/* thread changes  
				if ( seqNumber != CString() && defSeqNumber.CompareNoCase(seqNumber) ){
						defSeqNumber = seqNumber;
                       // samples are marked when a new defseq number is created 
						(void) MarkSamples();
						BOOL rc = DFDBSelect::MarkSamplesInDB();

				}

                */
				try {
						std::string strAnalysisState;
						strAnalysisState = std::string("infected");
						AnalysisStateInfo stateObject(strAnalysisState);
						AnalysisRequest request(localCookie);
						// FinalState function has to update the defseq number 
						int intDsn = atoi((LPTSTR) (LPCTSTR) defSeqNumber);
						request.FinalState(stateObject, intDsn);
							
				} 
				catch (AVISDBException exception) {
						char msg[1028];
						detailedString = exception.DetailedInfo();
						std::ostrstream inMsg(msg, sizeof(msg) - 1);
   						inMsg << detailedString;
						inMsg << std::ends;
						AfxMessageBox(msg, MB_ICONSTOP, 0);
						return(IDS_DATABASE_UPDATE_RECORD_ERROR); 
				}
				
            }
 	
		}

    }
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      LoadSampleData                                        */
/*                                                                            */
/* Description:         Read the "Result" file                                */
/*                                                                            */
/* Input:               DFJob *job - job object                               */
/*                                                                            */
/* Output:              DF_SUCCESS - success                                  */
/*                      DF_OBJECT_ERROR  - object is not in the collection    */
/*                      DF_JOB_STATUS_ERROR - status is incorrect (to read    */
/*                      the result of a job, status of the job has to be      */
/*                      ACCEPTED.                                             */   
/*                      DF_FILE_OPEN_ERROR  - file open error                 */
/*                      DF_RESULT_FILE_ERROR   - file content error           */
/* Return:                                                                    */
/*                                                                            */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::LoadSampleData(DFJob *jobObj)
{
	CString cookieId = jobObj->GetCookieId();
	POSITION pos;
    DFSample *pSample = FindObjectWithIdInHPList(cookieId, pos);
    if (pSample == NULL) { 
		return(DF_OBJECT_ERROR);
    }
    int seq = jobObj->GetJobSeq();
    DFJob *pJob = pSample->FindJobWithSeq(seq, pos);         
	if ( pJob == NULL ) {
	     return (DF_OBJECT_ERROR);
    }
    if (pJob->GetStatus() != DFJob::ACCEPTED)
		return (DF_JOB_STATUS_ERROR);


	char filePath[MAX_LENGTH];
	char dfstatusPath[MAX_LENGTH];

	char inBuff[256];
	char buffer[256];
	char seqString[100];
	char seqFile[100];
	char rest[256];
   

	strcpy(filePath, UNCPath);
	strcpy(dfstatusPath, UNCPath);
	char cookie[100];
	strcpy(cookie, (LPTSTR) (LPCTSTR) cookieId);
	CString modId(cookieId); 
	while (modId.GetLength() < 8)
	   modId.Insert(0, '0');
	AppendPath(filePath, (LPTSTR) (LPCTSTR) modId);
	AppendPath(dfstatusPath, (LPTSTR) (LPCTSTR) modId);
	AppendPath(filePath, "dfresults");
	AppendPath(dfstatusPath, "dfstatus.dat");
	sprintf(seqString, "%d", seq);
	strcpy(seqFile, "result");
	strcat(seqFile, seqString);
	strcat(seqFile, ".dat");
	AppendPath(filePath, seqFile);
	std::ifstream ifile(filePath, std::ios::in);
	int noRead = 0;

   	CString firstState = Matrix.FindFirstStateInGroup(DFEvalState::ARCHIVE);
    if  ( firstState.CompareNoCase(pJob->GetJobId()) == 0) 
         noRead = 1;
    // no result file or archive job 
	if (ifile.fail() || noRead ) {
		CString stateFromEval = Matrix.GetNextStateStr(pSample->GetCurrentStateStr(), CString("FAIL"));
		int intStateFromEval = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)stateFromEval);
		DFEvalState::StateType type = Matrix.GetStateType(intStateFromEval);
		if (type == DFEvalState::FINAL || type == DFEvalState::UNDEFINED ) {  
			pJob->SetStatus(DFJob::FAIL);			     
        }
		else {
	         pJob->SetStatus(DFJob::SUCCESS);			     
			 pSample->AddSuccessJobs();
        }
		pSample->SetNextStateStr(stateFromEval);
		pSample->SetNextState(intStateFromEval);
		pSample->evalPathFlag = 1;

		return(DF_RESULT_FILE_ERROR); 
	}

	CString status;
	CString jobStatus;
	CString strTime;
	CTime   time;
	char seps[]   = " ";
	char cookieKey[MAX_LENGTH];
	char ccommand[MAX_LENGTH];
	char cnextStep[MAX_LENGTH];
    int cFlag = 0, pFlag = 0, nFlag = 0;   
	char key[256];
	char rem[256];
	char *token;    
	strcpy(inBuff, "");
	strcpy(cnextStep, "");  
	strcpy(ccommand, "");  
	strcpy(key, "");  
	strcpy(cookieKey, "");  
	strcpy(rem, "");  
	strcpy(rest, "");  

	int i = 1;
	int newJobSeq = 0;
	int processed = 0;
	//int evalPathFlag = 0;
    pSample->evalPathFlag = 0;
	DFJob::JobStatus  oldStatus = pJob->GetStatus(); 
    int oldSuccessJobs = pSample->GetSuccessJobs();
	while (ifile.getline(inBuff, 256))
	{ 
        memset(buffer, '\0', 256);
		strcpy(buffer, inBuff);
		int length = strlen(buffer);
		buffer[length] = '\0';
	  	
		if (strcmp(inBuff, "") == 0)
		  continue;
		token = strtok( inBuff, seps);
		strupr(token); 
	 	  
		if (strcmp(token, "COOKIEID:") == 0) {
		  sscanf(buffer, "%s %s ", key, cookieKey);                   
		  i = 1;
		  cFlag  = 1;
		     //check if the cookie in the file is the right one 
			if (strcmp(cookieKey, "") != 0) {
				CString cookie = CString(cookieKey);
			    if ( cookie != cookieId )
				   return (DF_RESULT_FILE_ERROR);
            }
            else 
				return (DF_RESULT_FILE_ERROR);
            
		} 

		if (strcmp(token, "PARAMETERS:") == 0) {
 		  char *result;
          result = strpbrk( buffer, ":");	 
		  strcpy(ccommand, ++result); 
		  i++;
		  pFlag = 1;

	 }	
   	 if (strcmp(token, "NEXTSERVICE:") == 0) {
		  sscanf(buffer, "%s %s %s", key, cnextStep, rem);                   
		  i++;
		  nFlag = 1;
     }
	 if (strcmp(token, "1122334455") == 0) {
			sscanf(buffer, "%s %s ", key, rest);                   
            i=0;		  
     }

     if (i == 3 ) {          
		 processed = 1; 	 
		 CString state = CString(cnextStep);
		 state.TrimRight();
		 state.TrimLeft();
		 //test code
	 	 //pSample->dfstatus->open(dfstatusPath, std::ios::out | std::ios::app);
   		 //*(pSample->dfstatus) << " Next step is " <<  (LPTSTR) (LPCTSTR)state << "\n";
		 //pSample->dfstatus->flush();
		 //pSample->dfstatus->close();
		 //test code

		 CString command = CString(ccommand); 
		 //check the state
		 int checkState = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)state);
     //
	 // Here, at this point I am checking the "next state". If the state is not 
	 // recognized by the evaluator, the job is considered a failure. In order to
	 // direct a sample coming from the MacroReplicationEngine 
	 //	to the MacroReplicationController (in case of failure!), I am requesting
	 //	the Evaluator to give me the "next state" in accordance to its Matrix.   
     //
		CString stateFromEval;
		int intStateFromEval; 
		stateFromEval = Matrix.GetNextStateStr(pSample->GetCurrentStateStr(), CString("FAIL"));
		intStateFromEval = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)stateFromEval);
		DFEvalState::StateType type = Matrix.GetStateType(intStateFromEval);
	 	CString currentState = pSample->GetCurrentStateStr();
		DFEvalState::StateGroup group = Matrix.GetStateGroup(currentState);
		int fbFlag = 0;
		if (group == DFEvalState::FULLBUILD) {
			if (!pSample->SetDefSeqNumber(command)) {
					pJob->SetStatus(DFJob::FAIL);			     
					fbFlag = 1;
        	}
			else {
			     // log new seq number
				pSample->dfstatus->open(dfstatusPath, std::ios::out | std::ios::app);
 				*(pSample->dfstatus) <<  " The new built definition sequence number is: " 
				<< (LPTSTR) (LPCTSTR) pSample->GetDefSeqNumber() << "\n";
				pSample->dfstatus->flush();
				pSample->dfstatus->close();
			}

        } 
		if ( checkState == 0 || fbFlag == 1) {
			 // the state is not in the evaluator and the next job type is 'fail'
			 // job failed
			if (type == DFEvalState::FINAL ) {  
					 pJob->SetStatus(DFJob::FAIL);			     
			}
			else {
	           // The sample will go to the end if the current state is not after Replication Engine 			 
			     pJob->SetStatus(DFJob::SUCCESS);			     
				 pSample->AddSuccessJobs();
			}
			pSample->SetNextStateStr(stateFromEval);
			pSample->SetNextState(intStateFromEval);
			pSample->evalPathFlag = 1;

         
        }
		else {
			    pSample->SetNextStateStr(state);
				pSample->SetNextState(checkState);
	  		    DFEvalState::StateType type = Matrix.GetStateType(checkState);
				if (type == DFEvalState::FINAL ) { // "End" is here!! 
					pJob->SetNextStateType(type);
				    pJob->SetStatus(DFJob::SUCCESS);
					pSample->AddSuccessJobs();
                } 
                else {  
			        if(pSample->jobList.GetCount() > 1) {

				//after replication engene
						if(pSample->jobNewList.GetCount() == 0) {
				//State here is the next state for the sample. 
				//I am eliminating the control of the mandatory
				//next state when replication job is ended with a hope 
				//that result files will be correct...
							char unc[MAX_LENGTH];
							strcpy(unc, UNCPath);
							AppendPath(unc, (LPTSTR)(LPCTSTR)cookieId);
							CString uncString(unc);
							state = pSample->GetNextStateStr();
	      					DFJob *newJob = new DFJob(newJobSeq, state, cookieId,  command, pSample->unc); 
							pSample->jobNewList.AddTail(newJob);
							newJobSeq++;
#ifdef _DEBUG

						*(pSample->ofile) <<"10.List=" << pSample->jobList.GetCount() <<"\n";
						*(pSample->ofile) <<"1.NewList=" << pSample->jobNewList.GetCount() <<"\n";
						pSample->ofile->flush();
#endif
				
						}
						pJob->SetStatus(DFJob::SUCCESS);
						pSample->AddSuccessJobs();


					}
					else {
						char unc[MAX_LENGTH];
						strcpy(unc, UNCPath);
						AppendPath(unc, (LPTSTR)(LPCTSTR)cookieId);
						CString uncString(unc);
	      				DFJob *newJob = new DFJob(newJobSeq, state, cookieId,  command, pSample->unc); 
						pSample->jobNewList.AddTail(newJob);
						newJobSeq++;
						if (pSample->jobNewList.GetCount() == 1) {
				  			pJob->SetStatus(DFJob::SUCCESS);
							pSample->AddSuccessJobs();

						}
					}
             		pJob->SetStatus(DFJob::SUCCESS);
					
				}	
       	}
	  }
	}
   if (cFlag == 0 || pFlag == 0 || nFlag == 0) {
		CString stateFromEval = Matrix.GetNextStateStr(pSample->GetCurrentStateStr(), CString("FAIL"));
		int intStateFromEval = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)stateFromEval);
		DFEvalState::StateType type = Matrix.GetStateType(intStateFromEval);
		if (type == DFEvalState::FINAL || type == DFEvalState::UNDEFINED ) {  
			pJob->SetStatus(DFJob::FAIL);			     
        }
		else {
	         pJob->SetStatus(DFJob::SUCCESS);			     
			 pSample->AddSuccessJobs();
        }
		pSample->SetNextStateStr(stateFromEval);
		pSample->SetNextState(intStateFromEval);
		pSample->evalPathFlag = 1;

		return(DF_RESULT_FILE_ERROR); 
	}

   //check if the signature is the last line in the "result" file
   if (pSample->evalPathFlag == 0) {
	   if (strcmp(key, "1122334455") != 0) {
		    pSample->SetSuccessJobs(oldSuccessJobs);
			newJobSeq = 0;
			DFJob *pTmpJob;
			while (pSample->jobNewList.GetCount() != 0) {
				pTmpJob = pSample->jobNewList.GetTail();
				pSample->jobNewList.RemoveTail();
				delete pTmpJob;
			}
 	
	   		CString stateFromEval = Matrix.GetNextStateStr(pSample->GetCurrentStateStr(), CString("FAIL"));
			int intStateFromEval = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)stateFromEval);
			DFEvalState::StateType type = Matrix.GetStateType(intStateFromEval);
			if (type == DFEvalState::FINAL || type == DFEvalState::UNDEFINED ) {  
				pJob->SetStatus(DFJob::FAIL);			     
			}
			else {
				 pJob->SetStatus(DFJob::SUCCESS);			     
				 pSample->AddSuccessJobs();
			}
			pSample->SetNextStateStr(stateFromEval);
			pSample->SetNextState(intStateFromEval);
			ifile.close();
			return(DF_RESULT_FILE_ERROR); 
       }

   }   
    ifile.close();
    return DF_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      ReadStopCondition                                     */
/* Description:         To read sample's stop condition file                  */
/*                                                                            */
/* Input:               DFJob *job                                            */
/*                      DF_SUCCESS - success                                  */
/*                      DF_OBJECT_ERROR  - object is not in the collection    */
/*                      DF_FILE_OPEN_ERROR  - file open error                 */
/*                      DF_STOP_FILE_ERROR   - file content error             */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::ReadStopCondition(DFJob *jobObj)
{
  	CString cookieId = jobObj->GetCookieId();
	POSITION pos;
    DFSample *pSample = FindObjectWithIdInHPList(cookieId, pos);
    if (pSample == NULL) { 
		return(DF_OBJECT_ERROR);
    }
    int seq = jobObj->GetJobSeq();
    DFJob *pJob = pSample->FindJobWithSeq(seq, pos);         
	if ( pJob == NULL ) {
	     return (DF_OBJECT_ERROR);
    }

    char filePath[MAX_LENGTH];
    char inBuff[MAX_LENGTH];
    char buffer[MAX_LENGTH];
    char seqString[MAX_LENGTH];
    char seqFile[MAX_LENGTH];
   

    strcpy(filePath, UNCPath);
    char cookie[MAX_LENGTH];
    strcpy(cookie, (LPTSTR) (LPCTSTR) cookieId);

	CString modId(cookieId); 
	while (modId.GetLength() < 8)
	   modId.Insert(0, '0');
	AppendPath(filePath, (LPTSTR) (LPCTSTR) modId);
	AppendPath(filePath, "dfresults");
	sprintf(seqString, "%d", seq);
    strcpy(seqFile, "stop");
    strcat(seqFile, seqString);
    strcat(seqFile, ".dat");
    AppendPath(filePath, seqFile);
    std::ifstream ifile(filePath, std::ios::in);
   
    if (ifile.fail()) {
      return(DF_FILE_OPEN_ERROR); 
    }
   CString status;
   CString jobStatus;
   char seps[]   = " ";
   char cookieKey[256];
   char cstopcond[256];
   char rest[256];
   char key[256];
   char *token;    
   strcpy(inBuff, ""); 
   strcpy(key, ""); 
   strcpy(cookieKey, ""); 
   strcpy(cstopcond, ""); 
   strcpy(rest, ""); 
   int i = 0;
   int newJobSeq = 0;
   while (ifile.getline(inBuff, 256))
   { 
     memset(buffer, '\0', 256);
	 strcpy(buffer, inBuff);
	 int length = strlen(buffer);
	 buffer[length] = '\0';
	  	
	 if (strcmp(inBuff, "") == 0)
		  continue;
	 token = strtok( inBuff, seps);
	 strupr(token); 
	 if (strcmp(token, "COOKIEID:") == 0) {
		  sscanf(buffer, "%s %s ", key, cookieKey);                   
		  i++;
	 }	
   	 if (strcmp(token, "STOPCONDITION:") == 0) {
		  sscanf(buffer, "%s %s ", key, cstopcond);                   
		  i++;
     }
   	 if (strcmp(token, "1122334455") == 0) {
		  sscanf(buffer, "%s %s ", key, rest);                   

	 }   
              
   }
   if ( i != 2)
	   return (DF_STOP_FILE_ERROR);

   //check if the signature is the last line in the "stop" file
   if (strcmp(key, "1122334455") != 0)
	   return (DF_STOP_FILE_ERROR);

   //check if the cookie in the file is the right one 
   if (strcmp(cookieKey, "") != 0) {
      CString cookie = CString(cookieKey);
	  if ( cookie != cookieId )
		  return (DF_STOP_FILE_ERROR);
   }
   else 
	   return (DF_STOP_FILE_ERROR);

   // check if stopping condition was present in the file 
   if (strcmp(cstopcond, "") == 0) 
	   return (DF_STOP_FILE_ERROR);

   // OK 
   int stopCond;
   stopCond = atoi(cstopcond);
   
   
   pSample->SetStopCond(stopCond);	
   ifile.close();

  return DF_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/* Procedure name:      DetermineSampleStatus                                 */
/* Description:         To determine status of a sample                       */
/*                                                                            */
/* Input:               DFSample pSample - sample object                      */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              1 - status has changed; 0 - otherwise                 */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::DetermineSampleStatus(DFSample *pSample)
{
         
		POSITION pos = pSample->jobList.GetHeadPosition();
	    int HasStatusChanged = 0;
		int i = 0;
		DFJob* pJob;
		int deleteJobs = 0;
		int ignoreJobs = 0;
		pSample->stopSatisfied = 0;
		//criticalErrorFlag  = 0;
		while (pos != NULL)
		{
			pJob = pSample->jobList.GetNext(pos);
		
			DFJob::JobStatus status =  pJob->GetStatus();
            switch(status) {
			   case DFJob::SUCCESS:
				   {
					if (pSample->jobList.GetCount() == 1) {
						pSample->SetCurrentStatus(DFSample::SUCCESS);
					    if (pJob->GetNextStateType() == DFEvalState::FINAL) {
							//critical error checking
							DFEvalState::StateGroup group = Matrix.GetStateGroup(pSample->GetNextStateStr());
							if ( group == DFEvalState::STOP ) {
								pSample->criticalErrorFlag = 1;
								if  ( serializing.CompareNoCase(pJob->GetJobId()) == 0) 
								    pSample->serializeStateFlag = 1;
								criticalErrorFlag = 1;
								criticalSample = pSample->GetCookieId();
								pSample->SetCurrentStatus(DFSample::CriticalError);
								if (serialFlag && (!pSample->noBuildFlag)) {
									serialFlag = FALSE;
						    		pSample->noBuildFlag = TRUE;
                                } 


							}
							else {
								int rc  = ReportToStatusDatabase(pSample, COMPLETION, FALSE);
								if (rc != 0)
									return rc;

							    pSample->SetCurrentStatus(DFSample::FINAL);
                            }
							pSample->stopSatisfied = 1; 
                            

		  					  
                       }  
					   deleteJobs = 1;
					}
				   }
				   break;
			   case DFJob::FAIL:
				   {
					   if (pSample->jobList.GetCount() == 1) {
						   if (pSample->GetUserRequest() != DFSample::STOP) {
	   							pSample->SetCurrentStatus(DFSample::FAIL);
							   //critical error checking
								CString stateFromEval = Matrix.GetNextStateStr(pJob->GetJobId(), CString("FAIL"));
								DFEvalState::StateGroup group = Matrix.GetStateGroup(stateFromEval);
								if ( group == DFEvalState::STOP ) {
									pSample->criticalErrorFlag = 1;
									if  ( serializing.CompareNoCase(pJob->GetJobId()) == 0) 
									    pSample->serializeStateFlag = 1;
									criticalErrorFlag =1;
									criticalSample = pSample->GetCookieId();
									//pSample->SetCurrentStatus(DFSample::FINAL);
									pSample->SetCurrentStatus(DFSample::CriticalError);
									if (serialFlag && (!pSample->noBuildFlag)) {
										serialFlag = FALSE;
							    		pSample->noBuildFlag = TRUE;
                                    }


								}

								deleteJobs = 1;
                           } 
						   else {
#ifdef _DEBUG
	(void) MessageLogging(pJob->GetCookieId(), pJob->GetJobSeq(), "FromDetermine:Fail", 999);
#endif

								pSample->SetNextStateStr(pSample->GetCurrentStateStr());
								pSample->SetCurrentStateStr(pSample->GetPreviousStateStr());
								if (pSample->GetCurrentStateStr() != newSample)
								    pSample->SetCurrentStatus(DFSample::SUCCESS);
								else 
									pSample->SetCurrentStatus(DFSample::WAIT);
								deleteJobs = 0;
								pSample->SetDoneJobs(0);
  								pSample->SetSuccessJobs(0);
								pSample->ignoreFlag = FALSE;
								pSample->SetInProgressJobs(0);
								pSample->SetAcceptedState(pSample->GetPreviousStateStr());
								pJob->SetStatus(DFJob::UNDEFINED);		  

								HasStatusChanged = 1;
                          }

                       }
				   }
				   break;
			   case DFJob::POSTPONED:
				   {
					// The resources are not available. We'll resubmit the job
					// I don't change sample's status at this point 
					// It can be in WAIT, SUCCESS, and INPROGRESS
					CTime t = CTime::GetCurrentTime();
					pJob->SetPostponedTime(t);
					DFSample::SampleStatus currentStatus = pSample->GetCurrentStatus();
					if (currentStatus == DFSample::CriticalError)
                       deferCriticalSample = 1;
					   pSample->deferCriticalSample = 1;
  				   }
                   break;
               case DFJob::TIMEDOUT:
				   {
					   if (pSample->jobList.GetCount() == 1) {
							pSample->SetCurrentStatus(DFSample::FAIL);
							deleteJobs = 1;
                       } 

				   }
				   break;
               case DFJob::IGNORED:
				   {
					   if (pSample->jobList.GetCount() == 1) {
							pSample->SetCurrentStatus(DFSample::FAIL);
							deleteJobs = 1;
					   }

				   }
				   break;
				   
			   case DFJob::ACCEPTED:
				   {
					// I am changing sample's status at this point, because at least 
					// one job is in progress					   
                    // current and next statuses will coinside until the result of one job is received
                     
						pSample->SetCurrentStatus(DFSample::INPROGRESS);
						CString job = pJob->GetJobId();
						//job.MakeUpper();
						if (pSample->GetAcceptedState() !=  job) {
							pSample->SetPreviousStateStr(pSample->GetCurrentStateStr());
							//pSample->SetCurrentState(pSample->GetNextState());
							pSample->SetCurrentStateStr(pSample->GetNextStateStr()); 
				
							pSample->SetAcceptedState(job);
							HasStatusChanged = 1;
							// the repeated deferred job will not be reported,
							// because its status stays the same.
				    		int rc = ReportToStatusDatabase(pSample, ACCEPTANCE, FALSE);
                            if (rc  != 0)
								return rc;
						} 
				   }
				   break; 
			}
		
        }
		// Send ignore jobs on Defer for replicatons -- if a user defers a sample, the replications
		// are stopped. The "ignore" msg is "returning" back as a completed message.     
		if  (pSample->jobList.GetCount() != 1 && pSample->GetUserRequest() == DFSample::DEFER ) {
				DFJob *pJob;
				POSITION pos = pSample->jobList.GetHeadPosition();
				while (pos != NULL) {
				   pJob = pSample->jobList.GetNext(pos);
					   if (pJob->GetStatus() == DFJob::ACCEPTED || pJob->GetStatus() == DFJob::SUBMITTED) {
//#ifdef _DEBUG
//	(void) MessageLogging(pJob->GetCookieId(), pJob->GetJobSeq(), "FromDetermine:A", 999);
//#endif
							  //Launcher 
							 int rc; 
							 DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), CString(), pSample->unc);    
							 rc =  IgnoreJob(transitJob);
					   
					   } 
					   if (pJob->GetStatus() == DFJob::POSTPONED )  {
//#ifdef _DEBUG
//	(void) MessageLogging(pJob->GetCookieId(), pJob->GetJobSeq(), "FromDetermine:B", 999);
//#endif

							 pJob->SetStatus(DFJob::FAIL);
							 // don't loose a job...
							 pSample->AddDoneJobs();
							 //pSample->SubtractInProgressJobs(); 
							 HasStatusChanged = 1;
					   }
				}

        }
		if  (pSample->jobList.GetCount() != 1 && (!pSample->IsStopCondMet()) ) {
			// to do it only once!  It is only to send ignore, but I don't send ignore for 
			// im.hold, because I am sending them during submission 
			if ( pSample->GetSuccessJobs() >= pSample->GetStopCond() ) {
				DFJob *pJob;
				POSITION pos = pSample->jobList.GetHeadPosition();
				while (pos != NULL) {
				   pJob = pSample->jobList.GetNext(pos);
				   if (pSample->GetUserRequest() != DFSample::STOP) {
					   if (pJob->GetStatus() == DFJob::ACCEPTED || pJob->GetStatus() == DFJob::SUBMITTED) {
#ifdef _DEBUG
	(void) MessageLogging(pJob->GetCookieId(), pJob->GetJobSeq(), "FromDetermine:A", 999);
#endif
							  //Launcher 
							 int rc; 
							 DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), CString(), pSample->unc);    
							 rc =  IgnoreJob(transitJob);
					   
					   } 
					   if (pJob->GetStatus() == DFJob::POSTPONED )  {
#ifdef _DEBUG
	(void) MessageLogging(pJob->GetCookieId(), pJob->GetJobSeq(), "FromDetermine:B", 999);
#endif

							 pJob->SetStatus(DFJob::FAIL);
							 // don't loose a job...
							 pSample->AddDoneJobs();
							 pSample->SubtractInProgressJobs(); // ???
							 HasStatusChanged = 1;
					   }
				   }

				}
	            pSample->SetStopCondMet(TRUE);


			}

		}
        if (pSample->jobList.GetCount() != 1 ) {
				if ( pSample->GetDoneJobs() == pSample->GetInitCond() ) {
						if (pSample->GetUserRequest() != DFSample::STOP) {
			//  I am setting the sample's status after replication engine to SUCCESS
			//  without checking the number of successful jobs. The Controller will
			// 'decide' on SUCCESS or on FAIL. But let's check that we have at least one  
			//  successful job to be sure that next state (Controller) was set. 
							if ( pSample->GetSuccessJobs() == 0) {
									CString stateFromEval = Matrix.GetNextStateStr(pSample->GetCurrentStateStr(), CString("FAIL"));
									int intStateFromEval = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)stateFromEval);
									pSample->SetNextStateStr(stateFromEval);
				        			pSample->SetNextState(intStateFromEval);
                            } 
					    	pSample->SetCurrentStatus(DFSample::SUCCESS);
		                    deleteJobs = 1; 		
	
						}
						else {
						//sample was put on imm hold!
#ifdef _DEBUG
	(void) MessageLogging(pJob->GetCookieId(), pJob->GetJobSeq(), "FromDetermine:C", 999);
#endif

								pSample->SetNextStateStr(pSample->GetCurrentStateStr());
								pSample->SetCurrentStateStr(pSample->GetPreviousStateStr());
								if (pSample->GetCurrentStateStr() != newSample)
								    pSample->SetCurrentStatus(DFSample::SUCCESS);
								else 
									pSample->SetCurrentStatus(DFSample::WAIT);
								deleteJobs = 0;
								pSample->SetDoneJobs(0);
  								pSample->SetSuccessJobs(0);
								pSample->ignoreFlag = FALSE;
								pSample->SetInProgressJobs(0);
								pSample->SetAcceptedState(pSample->GetPreviousStateStr());
								POSITION pos = pSample->jobList.GetHeadPosition();
         						int i = 0;
								DFJob *pJob;
								while (pos != NULL) {
									pJob = pSample->jobList.GetNext(pos);
									CString id = pJob->GetJobId();
	    							pJob->SetStatus(DFJob::UNDEFINED);
							     }
								// in case a Replication Driver job has been created already 
								DFJob *pNewJob;
								if (pSample->jobNewList.GetCount() != 0 ) {
									while (pSample->jobNewList.GetCount() != 0) {
										pNewJob = pSample->jobNewList.GetTail();
										pSample->jobNewList.RemoveTail();
 										delete pNewJob;
									}
								}

								 HasStatusChanged = 1; 
					      
                        }
						pSample->stopSatisfied = 1; //testing path
				}
        }
        if (deleteJobs) {
				DFJob *pJob;
				while (pSample->jobList.GetCount() != 0) {
					pJob = pSample->jobList.GetTail();
					pSample->jobList.RemoveTail();
					delete pJob;
				}
				deleteJobs = 0;
				pSample->SetDoneJobs(0);
  				pSample->SetSuccessJobs(0);
				pSample->SetInitCond(0);
				pSample->SetInProgressJobs(0);
                HasStatusChanged = 1;
				
		}



						
		if (HasStatusChanged) {
    		int rc = ReportToStatusDatabase(pSample, COMPLETION, FALSE);
		    if (rc != 0)
				return rc;
		}	 
        
            
	return (HasStatusChanged);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      ProcessCompletedJob                                   */
/* Description:         Process a compeleted job                              */
/*                                                                            */
/* Input:               DFJob *pJob - job object                              */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_OBJECT_ERROR  - job was not found                  */                                                     
/*                      0/1 - status of a job has changed/stayed the same     */                                     
/*                      DF_RESULT_FILE_ERROR - result file error              */
/*						DF_FILE_OPEN_ERROR  - file open error                 */
/*						DF_JOB_STATUS_ERROR - job status error                */ 
/*DF_STOP_FILE_ERROR  - stop file error				                          */	
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::ProcessCompletedJob(DFJob *pJob)
{
	int seq = pJob->GetJobSeq();

    if (seq == DEFIMPSEQ) {
		if (defImportJob == NULL)
			return(DF_OBJECT_ERROR);
		int rc = ReadImportResult(); 
		if (rc != DF_SUCCESS) {
			switch (rc) {
				case DF_OBJECT_ERROR:
				case DF_JOB_STATUS_ERROR:
				case DF_RESULT_FILE_ERROR:
				case DF_FILE_OPEN_ERROR:
					{
					delete defImportJob;
					defImportJob = NULL;
#ifdef SCHEDULED_TASK_TEST
		logFile->open(testPath, std::ios::out | std::ios::app);
		CTime t = CTime::GetCurrentTime();
		CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
		*(logFile) << (LPTSTR) (LPCTSTR) tt << " After reading the result: Seq number was not imported" << "\n";
		logFile->flush();
		logFile->close();

#endif
					return (rc);
					} 
				case DF_CRITICAL_ERROR:
					{
					int seq = atoi((LPTSTR) (LPCTSTR) firstToken); 
                    if (seq != 0)
                        (void) MarkOnImport();                     
					delete defImportJob;
					defImportJob = NULL;
#ifdef SCHEDULED_TASK_TEST
		logFile->open(testPath, std::ios::out | std::ios::app);
		CTime t = CTime::GetCurrentTime();
		CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
		*(logFile) << (LPTSTR) (LPCTSTR) tt << " After reading the result: Seq number was not imported" << "\n";
		logFile->flush();
		logFile->close();

#endif

					return (rc);
					} 
				default:
					{
					return (rc);
					}
			}
        }
		(void) MarkOnImport();
		delete defImportJob;
		defImportJob = NULL;
		return rc;
			
	}
    if (seq == UNDEFSEQ) {
		if (undeferrerJob == NULL)
			return(DF_OBJECT_ERROR);
		delete undeferrerJob;
		undeferrerJob = NULL;
		return 0;
			
	}
    if (seq == STUPDATESEQ) {
		if (statusUpdaterJob == NULL)
			return(DF_OBJECT_ERROR);
		delete statusUpdaterJob;
		statusUpdaterJob = NULL;
		return 0;
			
	}
    if (seq == ATTRIBUTECOLLECTSEQ) {
		if (attributeCollectorJob == NULL)
			return(DF_OBJECT_ERROR);
		delete attributeCollectorJob;
		attributeCollectorJob = NULL;
#ifdef SCHEDULED_TASK_TEST
		logFile->open(testPath, std::ios::out | std::ios::app);
		CTime t = CTime::GetCurrentTime();
		CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
		*(logFile) << (LPTSTR) (LPCTSTR) tt << "Attribute job completed" << "\n";
		logFile->flush();
		logFile->close();

#endif

		return 0;
			
	}

	POSITION pos;
	CString cookieId = pJob->GetCookieId();
    DFSample *pSample = FindObjectWithIdInHPList(cookieId, pos);
    if (pSample == NULL) { 
		return(DF_OBJECT_ERROR);
    }

	char statusPath[MAX_LENGTH];
	strcpy(statusPath, pSample->unc);
	AppendPath(statusPath, "dfstatus.dat");
    if (seq == SCANSEQ ) {
		if (pSample->scanJob == NULL)
			return(DF_OBJECT_ERROR);
#ifdef _DEBUG
		(void) LogActivity(1, "ReadScanResult started. CookieId=", pSample->GetCookieId());
#endif
		int rc = ReadScanResult(pSample); 
#ifdef _DEBUG
		(void) LogActivity(1, "ReadScanResult ended. CookieId=", pSample->GetCookieId());
#endif
		if (rc != DF_SUCCESS) {
			switch (rc) {
				case DF_OBJECT_ERROR:
				case DF_JOB_STATUS_ERROR:
				case DF_RESULT_FILE_ERROR:
				case DF_FILE_OPEN_ERROR:
					{
					delete pSample->scanJob;
					pSample->SetScanRequired(FALSE);
					pSample->scanJob = NULL;
					pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
					CTime t = CTime::GetCurrentTime();
					pSample->SetCompletionTime(t);
					CString completionTime = t.Format( "%d %B %Y,  %H:%M:%S" );
					if (pSample->GetCurrentStateStr() == scan)
							*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) completionTime << " DBRescan:Scanning completed with failure" <<"\n";
					else
							*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) completionTime << " Rescan:Scanning completed with failure" <<"\n";
					pSample->dfstatus->flush();
					pSample->dfstatus->close();

					//(void)ReportToStatusDatabase(pSample, COMPLETION, TRUE);
					return rc;		 

					}
				default:
					{
					return (rc);
					}
			}
        }
		delete pSample->scanJob;
		pSample->scanJob = NULL;
		pSample->SetScanRequired(FALSE);
		pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
		CTime t = CTime::GetCurrentTime();
		pSample->SetCompletionTime(t);
		CString completionTime = t.Format( "%d %B %Y,  %H:%M:%S" );
		if (pSample->GetCurrentStateStr() == scan)
				*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) completionTime << " DBRescan:Scanning completed with success" <<"\n";
		else
				*(pSample->dfstatus) << (LPTSTR) (LPCTSTR) completionTime << " Rescan:Scanning completed with success" <<"\n";
		pSample->dfstatus->flush();
		pSample->dfstatus->close();

		//(void)ReportToStatusDatabase(pSample, COMPLETION, TRUE);

		return rc;
			
	}

    DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL ) {
	     return (DF_OBJECT_ERROR);
    }

	if (jobObj->GetStatus() != DFJob::ACCEPTED) {
	    return (DF_JOB_STATUS_ERROR); 
    }

	CString nextStep; 
	
	int seqId = pJob->GetJobSeq(); 


	cookieId = pJob->GetCookieId();
#ifdef _DEBUG
		(void) LogActivity(1, "LoadSampleData started. CookieId=", pSample->GetCookieId());
#endif

	int rc = LoadSampleData(pJob); 
#ifdef _DEBUG
		(void) LogActivity(1, "LoadSampleData ended. CookieId=", pSample->GetCookieId());
#endif

	if (rc != DF_SUCCESS) {
		switch (rc) {
			case DF_OBJECT_ERROR:
			case DF_JOB_STATUS_ERROR:
				{  
				return rc;		 
				}
			case DF_RESULT_FILE_ERROR:
			case DF_FILE_OPEN_ERROR:
				 {
				pSample->AddDoneJobs();
				pSample->SubtractInProgressJobs();
  				int retcode =  DetermineSampleStatus(pSample);
				if (retcode != 0 && retcode != 1)
					return (retcode);
				return (rc);
				} 
			default:
				{
				return (rc);
				}
		}
    }
	// Job's status was setup in LoadSampleData depending on the result 	pSample->AddDoneJobs();
    // Read stoping condition after replication driver completed its work  
    // Let's check if Replication driver completed its work. 
	// If it is, I am reading  the stopXX.dat file
	if (pSample->jobNewList.GetCount() > 1) {

	        int rc = ReadStopCondition(pJob);	
			if (rc != DF_SUCCESS) {
				switch (rc) {
					case DF_OBJECT_ERROR:
					case DF_JOB_STATUS_ERROR:
						{  
						return rc;		 
						}
					case PATH_ERROR:
					case DF_STOP_FILE_ERROR:
					case DF_FILE_OPEN_ERROR:
						{
						jobObj->SetStatus(DFJob::FAIL);
						pSample->AddDoneJobs();
						pSample->SubtractInProgressJobs();
  						int retcode = DetermineSampleStatus(pSample);
						if (rc != 0 && rc != 1)
						   return (retcode);
						}
						return rc;
					default:
						{
						return (rc);
						}
				}
			}
	}
	pSample->AddDoneJobs();
	pSample->SubtractInProgressJobs();
	rc = DetermineSampleStatus(pSample);
		
    return rc;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      ProcessAcceptedJob                                    */
/* Description:         Process an accepted job                               */
/*                                                                            */
/* Input:               DFJob *pJob - job object                              */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_OBJECT_ERROR  - job was not found                  */                                                     
/*                      0/1 status of a job has chnaged/stayed the same       */                                     
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::ProcessAcceptedJob(DFJob *pJob)
{
    int seq = pJob->GetJobSeq();
	// importing new definition job
    if (seq == DEFIMPSEQ) {
		if (defImportJob->GetStatus() != DFJob::SUBMITTED) 
		    return (DF_JOB_STATUS_ERROR); 
        defImportJob->SetStatus(DFJob::ACCEPTED);
		return (DF_SUCCESS);

	}
	// check deferred samples
    if (seq == UNDEFSEQ) {
		if (undeferrerJob->GetStatus() != DFJob::SUBMITTED) 
		    return (DF_JOB_STATUS_ERROR); 
        undeferrerJob->SetStatus(DFJob::ACCEPTED);
		return (DF_SUCCESS);

	}		
	// status update job
    if (seq == STUPDATESEQ) {
		if (statusUpdaterJob->GetStatus() != DFJob::SUBMITTED) 
		    return (DF_JOB_STATUS_ERROR); 
        statusUpdaterJob->SetStatus(DFJob::ACCEPTED);
		return (DF_SUCCESS);

	}		
	// status update job
    if (seq == ATTRIBUTECOLLECTSEQ) {
		if (attributeCollectorJob->GetStatus() != DFJob::SUBMITTED) 
		    return (DF_JOB_STATUS_ERROR); 
        attributeCollectorJob->SetStatus(DFJob::ACCEPTED);
		return (DF_SUCCESS);

	}		
	
	
	CString cookieId = pJob->GetCookieId();
	POSITION pos;
    DFSample *pSample = FindObjectWithIdInHPList(cookieId, pos);
    if (pSample == NULL) {
		return(DF_OBJECT_ERROR);
    }
    if (seq == SCANSEQ) {
		if (pSample->scanJob == NULL)
			return(DF_OBJECT_ERROR);
		if (pSample->scanJob->GetStatus() != DFJob::SUBMITTED) 
		    return (DF_JOB_STATUS_ERROR); 
        pSample->scanJob->SetStatus(DFJob::ACCEPTED);
		(void)ReportToStatusDatabase(pSample, ACCEPTANCE, TRUE);

		return (DF_SUCCESS);
    }
	DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL) {
		return(DF_OBJECT_ERROR);
	}
	if (jobObj->GetStatus() != DFJob::SUBMITTED) {
		return (DF_JOB_STATUS_ERROR); 
	}
	jobObj->SetStatus(DFJob::ACCEPTED);
	pSample->AddInProgressJobs();
	int rc =  DetermineSampleStatus(pSample);

    return rc;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      ProcessPostponedJob                                   */
/* Description:         Process a postponed job                               */
/*                                                                            */
/* Input:               DFJob *pJob - job object                              */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_OBJECT_ERROR  - job was not found                  */                                                     
/*                      0/1 - status of a job has changed/stayed the same     */                                     
/*						DF_JOB_STATUS_ERROR - job status error                */ 
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::ProcessPostponedJob(DFJob *pJob)
{

    int seq = pJob->GetJobSeq();
    if (seq == DEFIMPSEQ) {
		if (defImportJob == NULL)
			return(DF_OBJECT_ERROR);
//		if (defImportJob->GetStatus() != DFJob::SUBMITTED && defImportJob->GetStatus() != DFJob::ACCEPTED) {
//		    return (DF_JOB_STATUS_ERROR); 
//        }
//        defImportJob->SetStatus(DFJob::POSTPONED);
//		CTime t = CTime::GetCurrentTime();
//		defImportJob->SetPostponedTime(t);
//		return (DF_SUCCESS);
		delete defImportJob;
		defImportJob = NULL;


    }
    if (seq == UNDEFSEQ) {
		if (undeferrerJob == NULL)
			return(DF_OBJECT_ERROR);
//		if (undeferrerJob->GetStatus() != DFJob::SUBMITTED && undeferrerJob->GetStatus() != DFJob::ACCEPTED) {
//		    return (DF_JOB_STATUS_ERROR); 
//        }
//        undeferrerJob->SetStatus(DFJob::POSTPONED);
//		CTime t = CTime::GetCurrentTime();
//		undeferrerJob->SetPostponedTime(t);
//		return (DF_SUCCESS);
		delete undeferrerJob;
		undeferrerJob = NULL;


	}		

    if (seq == STUPDATESEQ) {
//		if (statusUpdaterJob == NULL)
//			return(DF_OBJECT_ERROR);
//		if (statusUpdaterJob->GetStatus() != DFJob::SUBMITTED && statusUpdaterJob->GetStatus() != DFJob::ACCEPTED) {
//		    return (DF_JOB_STATUS_ERROR); 
//        }
//        statusUpdaterJob->SetStatus(DFJob::POSTPONED);
//		CTime t = CTime::GetCurrentTime();
//		statusUpdaterJob->SetPostponedTime(t);
//		return (DF_SUCCESS);
//
		delete statusUpdaterJob;
		statusUpdaterJob = NULL;

	}		

    if (seq == ATTRIBUTECOLLECTSEQ) {
		delete attributeCollectorJob;
		attributeCollectorJob = NULL;

	}		

	CString cookieId = pJob->GetCookieId();
	POSITION pos;
    DFSample *pSample = FindObjectWithIdInHPList(cookieId, pos);
    if (pSample == NULL) {
		return(DF_OBJECT_ERROR);
    }
	// Scan jobs
    if (seq == SCANSEQ) {
		if (pSample->scanJob == NULL)
			return(DF_OBJECT_ERROR);
		if (pSample->scanJob->GetStatus() != DFJob::SUBMITTED && pSample->scanJob->GetStatus() != DFJob::ACCEPTED) {
		    return (DF_JOB_STATUS_ERROR); 
        }
        pSample->scanJob->SetStatus(DFJob::POSTPONED);
		CTime t = CTime::GetCurrentTime();
		pSample->scanJob->SetPostponedTime(t);
		return (DF_SUCCESS);

    }

    DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL) {
		return(DF_OBJECT_ERROR);
    }
	if (jobObj->GetStatus() != DFJob::SUBMITTED && jobObj->GetStatus() != DFJob::ACCEPTED) {
	    return (DF_JOB_STATUS_ERROR); 
    }
    if (jobObj->GetStatus() == DFJob::ACCEPTED) 
		   pSample->SubtractInProgressJobs();
    jobObj->SetStatus(DFJob::POSTPONED);
	int rc = DetermineSampleStatus(pSample);


    return rc;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      ProcessTimeoutJob                                     */
/* Description:         Process a postponed job                               */
/*                                                                            */
/* Input:               DFJob *pJob - job object                              */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_OBJECT_ERROR  - job was not found                  */                                                     
/*                      0/1 - status of a job has changed/stayed the same     */                                     
/*						DF_JOB_STATUS_ERROR - job status error                */ 
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::ProcessTimedoutJob(DFJob *pJob)
{
	POSITION pos;
    int seq = pJob->GetJobSeq();
    if (seq == DEFIMPSEQ) {
		if (defImportJob == NULL)
			return(DF_OBJECT_ERROR);
		if (defImportJob->GetStatus() != DFJob::ACCEPTED) 
		    return (DF_JOB_STATUS_ERROR); 
        defImportJob->SetStatus(DFJob::TIMEDOUT);
		return (DF_SUCCESS);
    }
    if (seq == UNDEFSEQ) {
		if (undeferrerJob == NULL)
			return(DF_OBJECT_ERROR);
		if (undeferrerJob->GetStatus() != DFJob::ACCEPTED) 
		    return (DF_JOB_STATUS_ERROR); 
        undeferrerJob->SetStatus(DFJob::TIMEDOUT);
		return (DF_SUCCESS);
    }
    if (seq == STUPDATESEQ) {
		if (statusUpdaterJob == NULL)
			return(DF_OBJECT_ERROR);
		if (statusUpdaterJob->GetStatus() != DFJob::ACCEPTED) 
		    return (DF_JOB_STATUS_ERROR); 
        statusUpdaterJob->SetStatus(DFJob::TIMEDOUT);
		return (DF_SUCCESS);
    }
    if (seq == ATTRIBUTECOLLECTSEQ) {
		if (attributeCollectorJob == NULL)
			return(DF_OBJECT_ERROR);
		if (attributeCollectorJob->GetStatus() != DFJob::ACCEPTED) 
		    return (DF_JOB_STATUS_ERROR); 
        attributeCollectorJob->SetStatus(DFJob::TIMEDOUT);
		return (DF_SUCCESS);
    }
	
	CString cookieId = pJob->GetCookieId();
    DFSample *pSample = FindObjectWithIdInHPList(cookieId, pos);
    if (pSample == NULL) { 
		return(DF_OBJECT_ERROR);
    }
    if (seq == SCANSEQ ) {
		if (pSample->scanJob == NULL)
			return(DF_OBJECT_ERROR);
		if (pSample->scanJob->GetStatus() != DFJob::ACCEPTED) 
		    return (DF_JOB_STATUS_ERROR); 
        pSample->scanJob->SetStatus(DFJob::TIMEDOUT);
		return (DF_SUCCESS);

    }




    DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL ) {
		return(DF_OBJECT_ERROR);
    }

	if (jobObj->GetStatus() != DFJob::ACCEPTED) {
	    return (DF_JOB_STATUS_ERROR); 
    }
    jobObj->SetStatus(DFJob::TIMEDOUT);
	//I don't read the result; so I add a "done" job here. .   
	pSample->AddDoneJobs();
	pSample->SubtractInProgressJobs();
	int rc =  DetermineSampleStatus(pSample);
		
    return rc;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      ProcessIgnoredJob                                     */
/* Description:         Process an ignored job                                */
/*                                                                            */
/* Input:               DFJob *pJob - job object                              */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_OBJECT_ERROR  - job was not found                  */                                                     
/*                      0/1 - status of a job has changed/stayed the same     */                                     
/*						DF_JOB_STATUS_ERROR - job status error                */ 
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  DFManager::ProcessIgnoredJob(DFJob *pJob)
{
	POSITION pos;
    int seq = pJob->GetJobSeq();

	// importing new definition job
    if (seq == DEFIMPSEQ) {
		if (defImportJob == NULL)
			return(DF_OBJECT_ERROR);
        delete defImportJob;
		defImportJob = NULL;
		return (DF_SUCCESS);

	}
	// check deferred samples
    if (seq == UNDEFSEQ) {
		if (undeferrerJob == NULL)
			return(DF_OBJECT_ERROR);
		delete undeferrerJob;
		undeferrerJob = NULL;
		return (DF_SUCCESS);
	}		
	// status update job
    if (seq == STUPDATESEQ) {
		if (statusUpdaterJob == NULL)
			return(DF_OBJECT_ERROR);
		delete statusUpdaterJob;
		statusUpdaterJob = NULL;
		return (DF_SUCCESS);

	}		
	// attribute collector job
    if (seq == ATTRIBUTECOLLECTSEQ) {
		if (attributeCollectorJob == NULL)
			return(DF_OBJECT_ERROR);
		delete attributeCollectorJob;
		attributeCollectorJob = NULL;
		return (DF_SUCCESS);

	}		
	
	
	CString cookieId = pJob->GetCookieId();
    DFSample *pSample = FindObjectWithIdInHPList(cookieId, pos);
    if (pSample == NULL) { 
		return(DF_OBJECT_ERROR);
    }
    DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL ) {
		 return(DF_OBJECT_ERROR);

    }

    // change the status of a job
    if (jobObj->GetStatus() == DFJob::SUCCESS || jobObj->GetStatus() == DFJob::FAIL  ) 
	 {
	    return (DF_JOB_STATUS_ERROR); 
	 }
    //jobObj->SetStatus(DFJob::IGNORED);
	//I don't read the result; so I'm  adding a "done" job here. .   
	pSample->AddDoneJobs();
	pSample->SubtractInProgressJobs();
    jobObj->SetStatus(DFJob::FAIL);

	int rc = DetermineSampleStatus(pSample);
		
    return rc;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      MessageLogging                                        */
/* Description:         To log information about a message                    */
/*                                                                            */
/* Input:               CString id - message id                               */
/*                      int seq   - sequence number                           */ 
/*                      CString status  - status                              */
/*                      int returncode  - return code                         */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_OBJECT_ERROR  - job was not found                  */                                                     
/*                      0 - success                                           */                                     
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::MessageLogging(CString id, int seq, CString status, int returncode)
{
   char logPath[MAX_LENGTH];
   char cookieLog[MAX_LENGTH];
   
   HINSTANCE hInst = AfxGetInstanceHandle();
   (void) GetModuleFileName(hInst, logPath, 1024);
   char *p = strrchr(logPath, '\\');
   *p = '\0';

   strcpy(cookieLog, (LPTSTR)(LPCTSTR)id);
   strcat(cookieLog, ".log"); 
    
   AppendPath(logPath, cookieLog);

	POSITION pos;
	
   DFSample *pSample = FindObjectWithIdInHPList(id, pos);
	if (pSample == NULL)
		return(DF_OBJECT_ERROR);
     
    CTime t = CTime::GetCurrentTime(); 
	CString cookieTime = t.Format( "%d %B %Y,  %H:%M:%S" );
#ifdef _DEBUG
    *(pSample->ofile) << (LPTSTR) (LPCTSTR)cookieTime << "  COOKIEID:  " <<  (LPTSTR) (LPCTSTR)id <<  "\n";
	*(pSample->ofile) << "   <<Job Seq>>" <<  seq << "<< Job Status/JobId>>" <<  (LPTSTR) (LPCTSTR)status  <<"<< RC/Msg>>:" << returncode <<  "\n";
	pSample->ofile->flush();
#endif
    return 0;

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetCurrentJobStatus                                   */
/* Description:         To get current job status as a string                 */
/*                                                                            */
/* Input:               DFJob::JobStatus status                               */
/* Output:              None                                                  */
/* Return:              CString string - status                               */                                                     
/*                      0 - success                                           */                                     
/* Global References:   None                                                  */
/* Notes: I have to move the string to the resource file                      */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
CString DFManager::GetCurrentJobStatus(DFJob::JobStatus status)
{

	    CString strStatus;
        switch (status)
		{	
		case DFJob::ACCEPTED:
			strStatus = CString("ACCEPTED");
			break;
		case DFJob::SUBMITTED:
			strStatus = CString("SUBMITTED");
			break;
		case DFJob::POSTPONED:
			strStatus = CString("POSTPONED");
			break;
		case DFJob::COMPLETED:
			strStatus = CString("COMPLETED");
			break;
		case DFJob::SUCCESS:
			strStatus = CString("SUCCESS");
			break;
		case DFJob::FAIL:
			strStatus = CString("FAIL");
			break;
		case DFJob::IGNORED:
			strStatus = CString("IGNORED");
			break;
        default:
			strStatus = CString();


        }
        return strStatus;

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      TermManager                                           */
/* Description:         To terminate the manager                              */
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              None                                                  */                                                     
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFManager::TermManager()
{

   	if (sampleHPList.GetCount() > 0) {
		POSITION position = sampleHPList.GetHeadPosition();
		POSITION pos1, pos2;
		DFSample *pSample;
		for (pos1 = sampleHPList.GetHeadPosition(); (pos2 = pos1) != NULL;)
		{
			pSample = sampleHPList.GetNext(pos1);
			
			if (pSample) {

#ifdef _DEBUG
			  if (pSample->ofile) {
				if (pSample->ofile->is_open()) {
					pSample->ofile->close();
				}
			  }
#endif
			  
			  if (pSample->jobList.GetCount() > 0) 
       				pSample->jobList.RemoveAll();
			  if (pSample->jobNewList.GetCount() > 0) 
       				pSample->jobNewList.RemoveAll();
				
			 
			  sampleHPList.RemoveAt(pos2);
			  delete pSample;
		    }
		}
    }
	if (packThread)
	   packThread->Kill();
	return;   
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      DetermineSampleStatusPlus                             */
/* Description:         To redetermine status of a sample when it is needed   */
/*                      nated.                                                */ 
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              None                                                  */                                                     
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::DetermineSampleStatusPlus()
{
	POSITION pos = sampleHPList.GetTailPosition();
	CString newStr, oldStr;
	while (pos != NULL)
	{
	    
		DFSample *pSample = sampleHPList.GetPrev(pos);
		if (pSample->statusDet) { 
           int rc = DetermineSampleStatus(pSample);
		   if (rc == -1)
		       return rc; 
		    pSample->statusDet = FALSE; 
        }

    }
	return(0);   
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SetupDisableRequest                                   */
/* Description:         To mark the samples with 'disable' request            */
/*                      nated.                                                */ 
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              None                                                  */                                                     
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFManager::SetupDisableRequest()
{
	POSITION pos = sampleHPList.GetTailPosition();
	while (pos != NULL)
	{
	    
		DFSample *pSample = sampleHPList.GetPrev(pos);
		if (pSample->GetUserRequest() != DFSample::PAUSE && pSample->GetUserRequest() != DFSample::STOP) { 
			pSample->SetUserRequest(DFSample::PAUSE);  
        }
    }
	return;   
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SetupEnableRequest                                    */
/* Description:         To mark the samples with 'enable' request             */
/*                      nated.                                                */ 
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              None                                                  */                                                     
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFManager::SetupEnableRequest()
{
	POSITION pos = sampleHPList.GetTailPosition();
	while (pos != NULL)
	{
	    
		DFSample *pSample = sampleHPList.GetPrev(pos);
		if (pSample->GetUserRequest() == DFSample::STOP || pSample->GetUserRequest() == DFSample::PAUSE ) { 
			pSample->SetUserRequest(DFSample::RESUME);  
        }

    }
	return;   
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      IsAtLeastOneOnHold                                    */
/* Description:         To determine whether at least one sample was put on   */
/*                      on hold                                               */ 
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              0 - no; 1 - otherwise                                 */                                                     
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFManager::IsAtLeastOneOnHold()
{
	POSITION pos = sampleHPList.GetTailPosition();
	CString newStr, oldStr;
    BOOL flag = FALSE;
	while (pos != NULL)
	{
	    
		DFSample *pSample = sampleHPList.GetPrev(pos);
		if (pSample->GetUserRequest() == DFSample::PAUSE || pSample->GetUserRequest() == DFSample::STOP) { 
			flag = TRUE;  
			break;
        }

    }
	return (flag);         
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      IsAtLeastOneNotOnHold                                 */
/* Description:         To determine whether at least one sample was not put  */
/*                      on hold                                               */ 
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              0 - no; 1 - otherwise                                 */                                                     
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFManager::IsAtLeastOneNotOnHold()
{
	POSITION pos = sampleHPList.GetTailPosition();
	CString newStr, oldStr;
    BOOL flag = FALSE; // all on Hold or PAUSE
	while (pos != NULL)
	{
	    
		DFSample *pSample = sampleHPList.GetPrev(pos);
		if (pSample->GetUserRequest() != DFSample::PAUSE && pSample->GetUserRequest() != DFSample::STOP) { 
			flag = TRUE;  
			break;
        }

    }
	return (flag);         
}
     
/*----------------------------------------------------------------------------*/
/* Procedure name:      CleanupAllCookiDirs                                   */
/* Description:         To cleanup cooki directories when it is needed        */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes: if dfffinish.dat is there, I am not cleaning the dir                */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
/*void DFManager::CleanupAllCookieDirs()
{
    char cookiePath[MAX_PATH];
	char startFileName[MAX_PATH];
	char finishFileName[MAX_PATH];
 
    char temp[MAX_PATH]; // temporary for directory
    int rc;              // return code from findfirst
    long hFile;
#ifdef _DEBUG
	_unlink("cookietest.log");
#endif 
	struct _finddata_t result;   
    strcpy(temp, UNCPath);
    strcat(temp,"\\*.*");

    hFile = _findfirst(temp, &result);
    rc = (hFile == -1) ? 1 : 0;

    while (rc == 0) {
		if(result.attrib & _A_SUBDIR ) {
			strcpy(cookiePath, UNCPath);
			AppendPath(cookiePath, result.name);
			strcpy(startFileName, cookiePath);
			AppendPath(startFileName, "dfstart.dat"); 
			strcpy(finishFileName, cookiePath);
			AppendPath(finishFileName, "dffinish.dat"); 

			int rc1 = IsFileFound(startFileName);
			int rc2 = IsFileFound(finishFileName);
			if (rc1 == 0  && rc2 != 0) 
				CleanupDir(cookiePath);
		}
		rc = _findnext(hFile, &result);

    }
	_findclose( hFile );
 
	return;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      CleanupDir                                            */
/* Description:         To cleanup a directory, but dfstart.dat               */
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
/*
void DFManager::CleanupDir(char *dirPath)
{
	char fileName[MAX_PATH];
	char temp[MAX_PATH]; // temporary for directory
    int rc;              // return code from findfirst
    long hFile;
	struct _finddata_t result;   
    strcpy(temp, dirPath);
    strcat(temp,"\\*.*");

    hFile = _findfirst(temp, &result);
    rc = (hFile == -1) ? 1 : 0;

    while (rc == 0) {
		if (stricmp(result.name, "dfstart.dat")) {
			strcpy(fileName, dirPath);
			AppendPath(fileName, result.name);
            _unlink(fileName);		
		}
		rc = _findnext(hFile, &result);
	}
	_findclose( hFile );
 
    strcpy(temp, dirPath);
	AppendPath(temp, "dfresults");
    strcat(temp,"\\*.*");

    hFile = _findfirst(temp, &result);
    rc = (hFile == -1) ? 1 : 0;

    while (rc == 0) {
		strcpy(fileName, dirPath);
		AppendPath(fileName, "dfresults");
        AppendPath(fileName, result.name);
        _unlink(fileName);		
		rc = _findnext(hFile, &result);
	}
	_findclose( hFile );
 
	return;
}*/
/*----------------------------------------------------------------------------*/
/* Procedure name:      IsFileFound                                           */
/* Description:         To determine whether file in in dir                   */
/*                                                                            */
/* Input:               file name                                             */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - file was found; 1- otherwise                      */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
/*
int DFManager::IsFileFound(char *fileName )
{

    char temp[256]; // temporary for directory
    int rc;         // return code from findfirst
    long hFile;
	struct _finddata_t result;   
    strcpy(temp, fileName);
    
    hFile = _findfirst(temp, &result);
    rc = (hFile == -1) ? 1 : 0;
    _findclose( hFile );
          
	return rc;



} 
*/
int DFManager::IsFileFound(char *fileName )
{

    LPCTSTR temp = (LPCTSTR) fileName; // temporary for directory
    int rc;         // return code from findfirst
    HANDLE hFindFile;
	WIN32_FIND_DATA result;   
    
    hFindFile = FindFirstFile(temp, &result);
	if (hFindFile == INVALID_HANDLE_VALUE) 
		rc = 1;
	else
		rc = 0;

    FindClose( hFindFile );
          
	return rc;



} 
   
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindObjectInDirList                                   */
/* Description:         To find a dir object in the dir collection            */
/*                                                                            */
/* Input:               CString path  - path                                  */
/* Output:              None                                                  */
/* Return:              pointer to DFDirInfo object. If NULL - object in not  */
/*                      in the collection                                     */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
DFDirInfo *DFManager::FindObjectInDirList(CString dirPath)
{
     
		POSITION pos = dirInfoList.GetHeadPosition();
	    int i = 0;
		DFDirInfo* pDirInfo;
		while (pos != NULL)
		{
			pDirInfo = dirInfoList.GetNext(pos);
			if (pDirInfo) {
				if (pDirInfo->dirPath == dirPath)
					return pDirInfo;
			} 
		}

		return NULL;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      CleanDirInfoList                                      */
/* Description:         Clean up the directories collection                   */
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
/*
void DFManager::CleanDirInfoList()
{
     
	DFDirInfo* pDirInfo;
	POSITION pos1, pos2;
	for (pos1 = dirInfoList.GetHeadPosition(); (pos2 = pos1) != NULL;)
	{
		pDirInfo = dirInfoList.GetNext(pos1);
		if (pDirInfo) {
			CTime time = pDirInfo->dirTime; 
			CTime ct = CTime::GetCurrentTime();
			CTimeSpan diff = ct - time;
			long sec = diff.GetTotalSeconds();
//			if (sec > CHECK_DIR_INTERVAL) {
			if (sec > dirCheckInterval) {

				dirInfoList.RemoveAt(pos2);
#ifdef _DEBUG
				LogMessage(1, pDirInfo->dirPath );
#endif
        		delete pDirInfo;
           }
        } 
	}

	return;
}
*/
/*----------------------------------------------------------------------------*/
/* Procedure name:      SaveConfigDefaultSettings                             */
/* Description:                                                               */
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::SaveConfigDefaultSettings()
{
    FILE * fp;
	char cfgPath[1024];
    strcpy(cfgPath, modulePath);
    AppendPath(cfgPath,"avisdf.cfg");


    fp = fopen (cfgPath, _T("w"));

    if (fp)
    {

		CParamValue paramValue;


		CString checkCashIntervalTag((LPCTSTR)IDS_CHECK_CASH_INT );
		CString checkDirIntervalTag((LPCTSTR) IDS_CHECK_DIR_INT);
		CString submissionIntervalTag((LPCTSTR) IDS_TIMER_INTERVAL);
		CString arrivalIntervalTag((LPCTSTR) IDS_SAMPLE_REQUEST_DELAY);
		CString maxSamplesTag((LPCTSTR) IDS_MAX_SAMPLES);
		CString buildDefFilenameTag((LPCTSTR) IDS_BUILD_DEF_FILENAME); 
		CString defBaseDirTag((LPCTSTR) IDS_DEF_BASE_DIR); 
		CString pathTag((LPCTSTR) IDS_UNCPATH); 
		CString defImporterDirTag((LPCTSTR) IDS_DEF_IMPORTER_DIR); 
		CString databaseRescanIntervalTag((LPCTSTR) IDS_DATABASE_RESCAN_INTERVAL);
		CString defImportIntervalTag((LPCTSTR) IDS_DEF_IMPORT_INTERVAL);
		CString statusUpdateIntervalTag((LPCTSTR) IDS_STATUS_UPDATE_INTERVAL);
		CString undeferrerIntervalTag((LPCTSTR) IDS_UNDEFERRER_INTERVAL);
		CString attributeIntervalTag((LPCTSTR) IDS_ATTRIBUTE_INTERVAL);

		CString strSubmissionInterval;
		strSubmissionInterval.Format("%d", DEFAULT_SAMPLE_SUBMISSION_INTERVAL);
		CString strArrivalInterval;
		strArrivalInterval.Format("%d", DEFAULT_SAMPLE_ARRIVAL_INTERVAL);
		CString strDatabaseRescanInterval;
		strDatabaseRescanInterval.Format("%d", DEFAULT_DATABASE_RESCAN_INTERVAL);
		CString strDefImportInterval;
		strDefImportInterval.Format("%d", DEFAULT_DEF_IMPORT_INTERVAL);
		CString strStatusUpdateInterval;
		strStatusUpdateInterval.Format("%d", DEFAULT_STATUS_UPDATE_INTERVAL);
		CString strUndeferrerInterval;
		strUndeferrerInterval.Format("%d", DEFAULT_UNDEFERRER_INTERVAL);
		CString strAttributeInterval;
		strAttributeInterval.Format("%d", DEFAULT_ATTRIBUTE_INTERVAL);


		CString strMaxSamples;
		strMaxSamples.Format("%d", DEFAULT_SAMPLES );
        CString  path(""); 

		CString cfgProlog((LPCTSTR) IDS_CFG_PROLOG);
		fwrite((LPCTSTR)cfgProlog, cfgProlog.GetLength(), 1, fp);
//		paramValue.AddParamValue (checkCashIntervalTag, strCashInterval );
//		paramValue.AddParamValue (checkDirIntervalTag, strCheckDirInterval );
		paramValue.AddParamValue (submissionIntervalTag,  strSubmissionInterval );
		paramValue.AddParamValue (arrivalIntervalTag, strArrivalInterval);
		paramValue.AddParamValue (maxSamplesTag, strMaxSamples );
		paramValue.AddParamValue (buildDefFilenameTag, path);
		paramValue.AddParamValue (defBaseDirTag, path);
		paramValue.AddParamValue (pathTag, path);
		paramValue.AddParamValue (defImporterDirTag, path);
		paramValue.AddParamValue (databaseRescanIntervalTag, strDatabaseRescanInterval);
		paramValue.AddParamValue (defImportIntervalTag, strDefImportInterval);
		paramValue.AddParamValue (statusUpdateIntervalTag, strStatusUpdateInterval);
		paramValue.AddParamValue (undeferrerIntervalTag, strUndeferrerInterval);
		paramValue.AddParamValue (attributeIntervalTag, strAttributeInterval);
    

        paramValue.WriteParamValue (fp);
        fclose (fp);
		//  pass the values to the Manager  
//		cacheCheckInterval =  DEFAULT_CACHE_CHECK_INTERVAL;
//		dirCheckInterval  = DEFAULT_DIR_CHECK_INTERVAL;
		sampleSubmissionInterval = (DEFAULT_SAMPLE_SUBMISSION_INTERVAL);
		sampleArrivalCheckInterval = (DEFAULT_SAMPLE_ARRIVAL_INTERVAL); 
		databaseRescanInterval = (DEFAULT_DATABASE_RESCAN_INTERVAL); 
		defImportInterval = (DEFAULT_DEF_IMPORT_INTERVAL); 
		statusUpdateInterval = (DEFAULT_STATUS_UPDATE_INTERVAL); 
		undeferrerInterval = (DEFAULT_UNDEFERRER_INTERVAL); 
		attributeInterval = (DEFAULT_ATTRIBUTE_INTERVAL); 

		maxSamples = DEFAULT_SAMPLES;
        strcpy(UNCPath, ""); 
        strcpy(buildDefFilename, ""); 
        strcpy(defBaseDir, ""); 
        strcpy(defImporterDir, ""); 
			
		return DF_SUCCESS;
	}
	else 
       return(DF_FILE_OPEN_ERROR);

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      ChangeCookiePriority                                  */
/* Description:         To change a priority of a cookie if it has been       */
/*                      changed in the database                               */
/* Input:               DFSample *dfSample  - sample's object                 */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::ChangeCookiePriority(DFSample *dfSample)
{
	int newPriority;
	CString trackingNumber = dfSample->GetCookieId();
	int localCookie = atoi((LPTSTR) (LPCTSTR) trackingNumber);
    int priority = dfSample->GetPriority();

	try {

		AnalysisRequest request(localCookie);
		newPriority = request.Priority();
	}
	catch (AVISDBException exception) {
		char msg[1028];
       	detailedString = exception.DetailedInfo();
	    std::ostrstream inMsg(msg, sizeof(msg) - 1);
    	inMsg << detailedString;
		inMsg << std::ends;
		AfxMessageBox(msg, MB_ICONSTOP, 0);
//					delete exception;
		return(IDS_DATABASE_GET_RECORD_ERROR);
        
	}
	// update priority in the collection
	if (priority == newPriority )
             return 0;
	POSITION pos1, pos2;
	DFSample *pSample, *sampleObj;
	BOOL deleteFlag = 0;
	for (pos1 = sampleHPList.GetHeadPosition(); (pos2 = pos1) != NULL;)
	{
		pSample = sampleHPList.GetNext(pos1);
		if (pSample) {
			CString cookieId = pSample->GetCookieId(); 
			if (cookieId == trackingNumber) { 
				sampleHPList.RemoveAt(pos2);
				deleteFlag = 1;
				break; 
			}
		} 
	}
   // sample object is alive, I'll insert it now again in the list.    
	if (deleteFlag == 0 )
		return 0;
    
	int i = 0;
	POSITION insertPos;
	pSample->SetPriority(newPriority);
	pos1 = sampleHPList.GetHeadPosition();
	insertPos = pos1;
	while (pos1 != NULL) {   
		POSITION pos2 = pos1;
		sampleObj = sampleHPList.GetNext(pos1);
		CString Id = sampleObj->GetCookieId();
		if (newPriority <= sampleObj->GetPriority()) {
				 insertPos = pos2;
          			
		}
	}	
	sampleHPList.InsertAfter(insertPos, pSample);
	return 1;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      UpdatePriority                                        */
/* Description:         Udate priorities in the collection of cookies         */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::UpdatePriority()
{
	POSITION pos = sampleHPList.GetHeadPosition();
	DFSample* sampleObject;
	while (pos != NULL)
	{
	   sampleObject = sampleHPList.GetNext(pos);
	   sampleObject->priorityFlag = FALSE;
         
	}

	while (true) {
		pos = sampleHPList.GetHeadPosition();
	    int i = 0;
		DFSample* sampleObject;
		int changeFlag = 0;
		while (pos != NULL) {
			sampleObject = sampleHPList.GetNext(pos);
			if (sampleObject->priorityFlag == 0) {
				int rc = ChangeCookiePriority(sampleObject);
				if (rc == -1)
					return (IDS_DATABASE_GET_RECORD_ERROR);   
				if ( rc == 1) {
					sampleObject->priorityFlag = TRUE;
                    changeFlag = 1; 
                    break;
				 }
           }
		}
		if (changeFlag == 0)
			break;

	}
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SerializeSubmit                                       */
/* Description:         To check whether the job has to be submitted          */
/* Input:               DFJob *jobId                                          */ 
/*                      DFSample *pSample                                     */
/* Output:              None                                                  */
/* Return:              TRUE - to submit, FALSE - otherwise                   */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFManager::SerializeSubmit(DFJob *jobId, DFSample *pSample)
{

	//synchronization
	// The DFM allows only one sample to be processed
	// in the Build+FullDefBuild group at a time.
	// State/SerilaFlag submit job table
	//                    true   | false
	//--------------------------------------
    //Analysis         | +       |  +
    //Build/FullBuild  | - (true)|  + (false)
    //Defer            | +       |  + 
    //Archive          | +       |  + 
	//End              | +       |  +
	//Stop               +       |  +
    BOOL submitFlag = FALSE; 
	DFEvalState::StateGroup group = Matrix.GetStateGroup(jobId->GetJobId());
	if (serialFlag ){
		if (pSample->noBuildFlag) {
			if (group != DFEvalState::BUILD && group != DFEvalState::FULLBUILD) {
				submitFlag = TRUE;
            }
			
        } 
		else {	
				//CWnd *pDFLWnd = CWnd::FromHandle(DFLWnd);
				//pDFLWnd->PostMessage(ALM_REQ_START_JOB, 0, (LPARAM) job);
			// I am "excluding "Archive" from post-Build group, because I want a sample waiting to
			// enter the Build group to be scanned.
				submitFlag = TRUE;
				if ( group == DFEvalState::STOP || group == DFEvalState::END 
		        //   || group == DFEvalState::DEFER || group == DFEvalState::ARCHIVE ) {
				   || group == DFEvalState::DEFER ) {
					serialFlag = FALSE;
					pSample->noBuildFlag = TRUE;

                } 
		}
    }
	else {
		if(pSample->noBuildFlag) {    
			if ( group == DFEvalState::BUILD || group == DFEvalState::FULLBUILD) {
                if (defImportJob == 0) {
					if (pSample->GetScanRequired()) { 
					// a sample is about to enter the Build state 
						if (pSample->scanJob == NULL) {

							CString seq1;
							//CString seq2;
							// comment out these two lines later
							seq1 = defSeqNumber;

							//seq1.Format("%d", seqNumber);
							pSample->scanJob = new DFJob(SCANSEQ, scan, pSample->GetCookieId(), seq1, pSample->unc);
							pSample->scanJob->SetStatus(DFJob::SUBMITTED);
							CTime t = CTime::GetCurrentTime();
							pSample->scanJob->SetSubmitTime(t);
#ifdef _DEBUG
               			CString ttt = t.Format( "%d %B %Y,  %H:%M:%S" );

						*(pSample->ofile) << (LPTSTR) (LPCTSTR)ttt << "  COOKIEID:  " <<  (LPTSTR) (LPCTSTR)pSample->GetCookieId() <<  "\n";
						*(pSample->ofile) << "   <<Job Seq>>" <<  pSample->GetSeqNumber() << "<< Job Status/JobId>>" <<  (LPTSTR) (LPCTSTR)pSample->GetCurrentStateStr() <<"<< RC/Msg>>:" << "scan job submitted" <<"\n";
						pSample->ofile->flush();
#endif

							DFJob *transitJob = new DFJob(pSample->scanJob->GetJobSeq(), pSample->scanJob->GetJobId(), pSample->GetCookieId(), seq1, pSample->unc);    
							(void) SubmitJob(transitJob, pSample);
						} 

					}
					else {
						pSample->noBuildFlag = FALSE;
						serialFlag = TRUE;
						submitFlag = TRUE;
					}
				}
			}
			else 
				submitFlag = TRUE;


		}
		else
		   submitFlag = TRUE;
		

	
    }
    return (submitFlag);
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      Mark samples                                          */
/* Description:         To mark the samples for scanning                      */
/* Input:               None                                                  */ 
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFManager::MarkSamples()
{
	POSITION pos = sampleHPList.GetHeadPosition();
	DFSample* pSample;
	while (pos != NULL)
	{
		pSample = sampleHPList.GetNext(pos);
		DFEvalState::StateGroup group = Matrix.GetStateGroup(pSample->GetCurrentStateStr());
        if (group == DFEvalState::ANALYSIS )  {
			if (pSample->jobList.GetCount() != 0)  {
				POSITION pos1 = pSample->jobList.GetHeadPosition();
				if  (pos1 != NULL) {
					DFJob *pJob = pSample->jobList.GetNext(pos1);
			   		CString firstState = Matrix.FindFirstStateInGroup(DFEvalState::ARCHIVE);
					if  ( firstState.CompareNoCase(pJob->GetJobId()) != 0) 
	    						pSample->SetScanRequired(TRUE);
                }
			}
			else
			    pSample->SetScanRequired(TRUE);         
         
        }
        else {
			// before entering the build group
			if (serialFlag  && group == DFEvalState::FULLBUILD && pSample->noBuildFlag)
				pSample->SetScanRequired(TRUE);
        }         
	}

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      SubmitScanJob                                         */
/* Description:         To do the checking for a scan job to be submitted     */
/* Input:               None                                                  */ 
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFManager::SubmitScanJob(DFSample *pSample)
{
	//int	blessedSeqNumber;
	CString seq1;
	CString seq2;
#ifdef _DEBUG
	*(pSample->ofile) <<"SubmitScanJob entered"<<"\n";
	pSample->ofile->flush();
#endif
	if (pSample->scanJob != NULL) {
#ifdef _DEBUG
		*(pSample->ofile) <<"Scan job exists"<<"\n";
		pSample->ofile->flush();
#endif
		if ( pSample->scanJob->GetStatus() == DFJob::POSTPONED) {
			CTime ct = CTime::GetCurrentTime();
			CTime pt = pSample->scanJob->GetPostponedTime();
			CTimeSpan diff = ct - pt;
			long sec = diff.GetTotalSeconds();
#ifdef _DEBUG
			*(pSample->ofile) <<"When job is postponed ...sec"<< sec <<"\n";
			pSample->ofile->flush();
#endif
			if (sec > JOB_RESUBMIT_INTERVAL ) { 
//				if (Matrix.IsStateBusy(scan)) {
//					return FALSE;
//                }
//				else {
//					Matrix.SetState(scan, TRUE);
					pSample->scanJob->SetStatus(DFJob::SUBMITTED);
					CTime t = CTime::GetCurrentTime();
   					pSample->scanJob->SetSubmitTime(t);
					seq1 = defSeqNumber;
					DFJob *transitJob = new DFJob(pSample->scanJob->GetJobSeq(), pSample->scanJob->GetJobId(), pSample->GetCookieId(), seq1, pSample->unc);    
					(void) SubmitJob(transitJob, pSample);
					return TRUE;
//				}

			}

        }
	}	
	else {
#ifdef _DEBUG
			*(pSample->ofile) <<"ScanJob=NULL. New one has been created"<<"\n";
			pSample->ofile->flush();
#endif
//			int seqNumber;
			//int	blessedSeqNumber;
			CString seq1;
			//CString seq2;
#ifdef DATABASE_INTERFACE
			//Signature  latest;
			//Signature  blessed;

			//Signature::Latest(latest);
			//seqNumber = latest.SequenceNumber();
	        //Signature::LatestBlessed(blessed);
	        //blessedSeqNumber = blessed.SequenceNumber();
#else
			//seqNumber = defSeqNumber;
	        //blessedSeqNumber = defSeqNumber;
#endif
			// comment out these two lines later
			seq1 = defSeqNumber;
	        //seq2 = defSeqNumber;

			//seq1.Format("%d", seqNumber);
			//seq2.Format("%d", blessedSeqNumber);
			//CString seqString = seq1 + CString(" ") + seq2;
			pSample->scanJob = new DFJob(SCANSEQ, scan, pSample->GetCookieId(), seq1, pSample->unc);
			pSample->scanJob->SetStatus(DFJob::SUBMITTED);
			CTime t = CTime::GetCurrentTime();
   			pSample->scanJob->SetSubmitTime(t);
			DFJob *transitJob = new DFJob(pSample->scanJob->GetJobSeq(), pSample->scanJob->GetJobId(), pSample->GetCookieId(), seq1, pSample->unc);    
			(void) SubmitJob(transitJob, pSample);
			return TRUE;
			
	}
#ifdef _DEBUG
			*(pSample->ofile) <<"Scan State is free -- Exit when time is not expired yet!"<<"\n";
			pSample->ofile->flush();
#endif

    return TRUE;
}    
/*----------------------------------------------------------------------------*/
/* Procedure name:      ReadScanResult                                        */
/* Description:         Read the "Result" file                                */
/*                                                                            */
/* Input:               DFJob *job - job object                               */
/*                                                                            */
/* Output:              DF_SUCCESS - success                                  */
/*                      DF_OBJECT_ERROR  - object is not in the collection    */
/*                      DF_JOB_STATUS_ERROR - status is incorrect (to read    */
/*                      the result of a job, status of the job has to be      */
/*                      ACCEPTED.                                             */   
/*                      DF_FILE_OPEN_ERROR  - file open error                 */
/*                      DF_RESULT_FILE_ERROR   - file content error           */    
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::ReadScanResult(DFSample *pSample)
{
	if ( pSample->scanJob == NULL ) {
	     return (DF_OBJECT_ERROR);
    }

	char filePath[MAX_LENGTH];
	char inBuff[256];
	char buffer[256];
	char seqString[100];
	char seqFile[100];
	char rest[256];
	int checkState;
	CString command;

	CString cookieId = pSample->GetCookieId();
	strcpy(filePath, UNCPath);
	char cookie[100];
	strcpy(cookie, (LPTSTR) (LPCTSTR) cookieId );
	CString modId(cookieId); 
	while (modId.GetLength() < 8)
	   modId.Insert(0, '0');
	AppendPath(filePath, (LPTSTR) (LPCTSTR) modId);
	AppendPath(filePath, "dfresults");
	sprintf(seqString, "%d", SCANSEQ);
	strcpy(seqFile, "result");
	strcat(seqFile, seqString);
	strcat(seqFile, ".dat");
	AppendPath(filePath, seqFile);
	std::ifstream ifile(filePath, std::ios::in);
   
	if (ifile.fail()) {
		return(DF_RESULT_FILE_ERROR); 
	}

//test code
	char statusPath[MAX_LENGTH];
	strcpy(statusPath, pSample->unc);
	AppendPath(statusPath, "dfstatus.dat");
//test code



	CString status;
	CString jobStatus;
	CString strTime;
	CTime   time;
	char seps[]   = " ";
	char cookieKey[MAX_LENGTH];
	char ccommand[MAX_LENGTH];
	char cnextStep[MAX_LENGTH];
	char key[256];

	strcpy(cnextStep, "");  
	strcpy(ccommand, "");  
	strcpy(key, "");  
	strcpy(cookieKey, "");  

	char *token;    
	strcpy(inBuff, ""); 
	int i = 1;
	int newJobSeq = 0;
	int processed = 0;
	int evalPathFlag = 0;
	CString stateString;
	CString state;

	while (ifile.getline(inBuff, 256))
	{ 
        memset(buffer, '\0', 256);
		strcpy(buffer, inBuff);
		int length = strlen(buffer);
		buffer[length] = '\0';
	  	
		if (strcmp(inBuff, "") == 0)
		  continue;
		token = strtok( inBuff, seps);
		strupr(token); 
	 	  
		if (strcmp(token, "COOKIEID:") == 0) {
		   sscanf(buffer, "%s %s ", key, cookieKey);                   
		   i = 1;
		     //check if the cookie in the file is the right one 
			if (strcmp(cookieKey, "") != 0) {
				CString cookie = CString(cookieKey);
			    if ( cookie != cookieId )
				   return (DF_RESULT_FILE_ERROR);
            }
            else 
				return (DF_RESULT_FILE_ERROR);
            
		} 

		if (strcmp(token, "PARAMETERS:") == 0) {
 		  char *result;
          result = strpbrk( buffer, ":");	 
		  strcpy(ccommand, ++result); 
		  i++;

		}	
   		if (strcmp(token, "NEXTSERVICE:") == 0) {
		  sscanf(buffer, "%s %s ", key, cnextStep);                   
		  i++;
		}
		if (strcmp(token, "1122334455") == 0) {
			sscanf(buffer, "%s %s ", key, rest);                   
            i=0;		  
		}

		if (i == 3 ) {          
			processed = 1; 
			command = CString(ccommand); 
			if (command == CString()) {
				ifile.close();
 				return(DF_RESULT_FILE_ERROR); 
			}
			state = CString(cnextStep);
			//test code
			//pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
			//*(pSample->dfstatus) << " Next step is " <<  (LPTSTR) (LPCTSTR)state  <<"\n";
			//pSample->dfstatus->flush();
			//pSample->dfstatus->close();
			//test code
	
			if (state == CString()) {
				ifile.close();
				//test code
				//pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
				//*(pSample->dfstatus) << " Next step is not identified." <<   "\n";
				//pSample->dfstatus->flush();
				//pSample->dfstatus->close();
				//test code
				return(DF_RESULT_FILE_ERROR); 
			}
			state.TrimRight();
			state.TrimLeft();
			checkState = Matrix.FindMappingNumber( (LPTSTR) (LPCTSTR)state);
			if ( checkState == 0 ) {
				ifile.close();
 				return(DF_RESULT_FILE_ERROR); 
			}
			CString firstState = Matrix.FindFirstStateInGroup(DFEvalState::ARCHIVE);
				//stateString = Matrix.GetStateName(checkState);
			if (firstState.CompareNoCase(state) ) {
				ifile.close();
 				return(DF_RESULT_FILE_ERROR);
            }
		
		}

	}
	//test code
	//pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
	//*(pSample->dfstatus) << " Before checking the marker. i MUST be 3. ; i = " << i  << "\n";
	//*(pSample->dfstatus) << " Let's check the state again" << (LPTSTR) (LPCTSTR)state << "\n";
	//pSample->dfstatus->flush();
	//pSample->dfstatus->close();
	//test code


	if (strcmp(key, "1122334455") != 0) {
		ifile.close();
		return(DF_RESULT_FILE_ERROR); 
	}
    //delete the existing jobs and create a new one - ARCHIVE
	int seqDelta = 0;
	if (pSample->jobNewList.GetCount() != 0)  {
			POSITION position = pSample->jobNewList.GetHeadPosition();
			POSITION pos1, pos2;
			DFJob* pJob;
			for (pos1 = pSample->jobNewList.GetHeadPosition(); (pos2 = pos1) != NULL;)
			{
				pJob = pSample->jobNewList.GetNext(pos1);
				pSample->jobNewList.RemoveAt(pos2);
				delete pJob;
				seqDelta++;
			}
	}
	// I am cleaning up here, because some jobs could be postponed, before they were 
	// accepted and the status of the sample is STILL 'SUCCESS'( I submit a scan job having 
	// a number of postponed jobs in the job collection, even if the status of the sample 
	// 'SUCCESS'. 07/09/99
	if (pSample->jobList.GetCount() != 0)  {
			POSITION position = pSample->jobList.GetHeadPosition();
			POSITION pos1, pos2;
			DFJob* pJob;
			for (pos1 = pSample->jobList.GetHeadPosition(); (pos2 = pos1) != NULL;)
			{
				pJob = pSample->jobList.GetNext(pos1);
				pSample->jobList.RemoveAt(pos2);
				delete pJob;
				seqDelta++;
			}
	} 
	pSample->SetDoneJobs(0);
	pSample->SetSuccessJobs(0);
	pSample->SetInitCond(0);
	pSample->SetInProgressJobs(0);
	
	char unc[MAX_LENGTH];
	strcpy(unc, UNCPath);
	AppendPath(unc, (LPTSTR)(LPCTSTR)cookieId);
	CString uncString(unc);
//test code
	pSample->dfstatus->open(statusPath, std::ios::out | std::ios::app);
	*(pSample->dfstatus) << " Archive  job is about to be created; job id is    " << (LPTSTR) (LPCTSTR)state << "\n";
	pSample->dfstatus->flush();
	pSample->dfstatus->close();
//test code

	DFJob *newJob = new DFJob(newJobSeq, state, cookieId,  command, pSample->unc); 
	pSample->jobNewList.AddTail(newJob);
	pSample->SetNextStateStr(state);
	pSample->SetNextState(checkState);

	ifile.close();
    return DF_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      AddSignature                                          */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
BOOL DFManager::AddSignature(DFSample *pSample)
{
#ifdef _DEBUG
		(void) LogActivity(1, "AddSignature started(1). CookieId=", pSample->GetCookieId());
#endif

	BOOL returnCode = FALSE;
	Signature  latest;
	int latestSeqNumber;
	DateTime	nullTime;
	nullTime.Null(true);
	CTime t; 
    std::string fileName;
    std::string checkSum;
	t = CTime::GetCurrentTime();
	DateTime currentTime(t.GetYear(), t.GetMonth(),
	t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());

	CString sequenceNumber = pSample->GetDefSeqNumber();
	uint intDsn = atoi((LPTSTR) (LPCTSTR) pSample->GetDefSeqNumber());
#ifdef _DEBUG
		(void) LogActivity(1, " In AddSignature (2). CookieId=", pSample->GetCookieId());
#endif

	try {    
#ifdef _DEBUG
		(void) LogActivity(1, " In AddSignature (3). CookieId=", pSample->GetCookieId());
#endif

		Signature::Latest(latest);
		latestSeqNumber = latest.SequenceNumber();
		// calculate checksum
        
#ifdef _DEBUG
		(void) LogActivity(1, " In AddSignature (4). CookieId=", pSample->GetCookieId());
#endif

        fileName = pSample->packageName.GetBuffer(2048);
        checkSum = computeMD5CheckSum (fileName);
#ifdef _DEBUG
		char buffer1[100];
		CString dsnString;
		sprintf(buffer1, "%d", intDsn); 
        dsnString = CString(buffer1);  

		char buffer2[100];
		CString latestString;
		sprintf(buffer2, "%d", latestSeqNumber); 
        latestString = CString(buffer2);  
		
		(void) LogActivity(1, " In AddSignature (5). CookieId=", pSample->GetCookieId());
		 testFile->open(logPath, std::ios::out | std::ios::app);
 		 *(testFile) << "Dsn=" << (LPTSTR) (LPCTSTR)dsnString << "  LatestSeqNumber= " << (LPTSTR) (LPCTSTR)latestString << "\n";
		 testFile->flush();
         testFile->close(); 

#endif
    } 
	catch (AVISDBException exception) {
		char msg[1028];
    	detailedString = exception.DetailedInfo();
		std::ostrstream inMsg(msg, sizeof(msg) - 1);
		inMsg << detailedString;
		inMsg << std::ends;
		AfxMessageBox(msg, MB_ICONSTOP, 0);
		return(TRUE); 
	}
		if (intDsn > latestSeqNumber) {
#ifdef _DEBUG
			(void) LogActivity(1, " In AddSignature before adding the sig(6). CookieId=", pSample->GetCookieId());
#endif

			try {    
    			returnCode = Signature::AddNew(intDsn, nullTime, currentTime,
				currentTime, checkSum);
			}
			catch (AVISDBException& exception) {
				char msg[1028];
    			detailedString = exception.DetailedInfo();
				std::ostrstream inMsg(msg, sizeof(msg) - 1);
				inMsg << detailedString;
				inMsg << std::ends;
				AfxMessageBox(msg, MB_ICONSTOP, 0);
				return(TRUE); 
			}

#ifdef _DEBUG

			(void) LogActivity(1, " In AddSignature (7).after adding the sig CookieId=", pSample->GetCookieId());
#endif
			
			Signature::Latest(latest);
#ifdef _DEBUG
			(void) LogActivity(1, " In AddSignature (8). CookieId=", pSample->GetCookieId());
#endif

        }
		else {
#ifdef _DEBUG
			(void) LogActivity(1, " In AddSignature (9). CookieId=", pSample->GetCookieId());
			return FALSE;
#endif
		}

		
	try {
#ifdef _DEBUG
		(void) LogActivity(1, " In AddSignature (10). CookieId=", pSample->GetCookieId());
#endif
		
	     if (returnCode) {
			    returnCode =  SigsToBeExported::AddForAllActiveServers(latest);
#ifdef _DEBUG
				(void) LogActivity(1, " In AddSignature (11). CookieId=", pSample->GetCookieId());
#endif

		}
	}
	catch (AVISDBException exception) {
		char msg[1028];
    	detailedString = exception.DetailedInfo();
		std::ostrstream inMsg(msg, sizeof(msg) - 1);
		inMsg << detailedString;
		inMsg << std::ends;
		AfxMessageBox(msg, MB_ICONSTOP, 0);
		return(TRUE); 
	}
#ifdef _DEBUG
		(void) LogActivity(1, "AddSignature ended(12). CookieId=", pSample->GetCookieId());
#endif

	return FALSE;

}
/*----------------------------------------------------------------------------*/
/* Procedure name:   GetLatestSeqNumber                                       */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::GetLatestSeqNumber()
{

	Signature  latest;
	int latestSeqNumber;
	try {    
		Signature::Latest(latest);
		latestSeqNumber = latest.SequenceNumber();
		
    }
	catch (AVISDBException exception) {
		char msg[1028];
    	detailedString = exception.DetailedInfo();
		std::ostrstream inMsg(msg, sizeof(msg) - 1);
		inMsg << detailedString;
		inMsg << std::ends;
		AfxMessageBox(msg, MB_ICONSTOP, 0);
		return 0; 
	}
    return (latestSeqNumber);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:   ImportNewDefinitions                                     */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::ImportNewDefinitions()
{
#ifdef SCHEDULED_TASK_TEST
//test log
		CTime t = CTime::GetCurrentTime();
		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << " ImportNewDefinitions entered. " << "\n";
		logFile->flush();
		logFile->close();

#endif


	if (defImportJob == NULL) {
#ifdef SCHEDULED_TASK_TEST
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << " Importing definition job submitted." << "\n";
		logFile->flush();
		logFile->close();

#endif
		importPending = 0;
		CString command = CString(defBaseDir) + CString(" ") + CString(buildDefFilename);
		defImportJob = new DFJob(DEFIMPSEQ, import, CString("00000000"), command, CString(defImporterDir));
		defImportJob->SetStatus(DFJob::SUBMITTED);
		CTime t = CTime::GetCurrentTime();
		defImportJob->SetSubmitTime(t);
		secondToken = CString();
		firstToken = CString();
		DFJob *transitJob = new DFJob(DEFIMPSEQ, import, CString("00000000"), command, CString(defImporterDir));    
		(void) SubmitJob(transitJob, NULL);	
		return 0;

	}
    return 1; 
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      ReadImportResult                                      */
/* Description:         Read the "Result" file from an import definition job  */
/*                                                                            */
/* Input:               DFJob *job - job object                               */
/*                                                                            */
/* Output:              DF_SUCCESS - success                                  */
/*                      DF_OBJECT_ERROR  - object is not in the collection    */
/*                      DF_JOB_STATUS_ERROR - status is incorrect (to read    */
/*                      the result of a job, status of the job has to be      */
/*                      ACCEPTED.                                             */   
/*                      DF_FILE_OPEN_ERROR  - file open error                 */
/*                      DF_RESULT_FILE_ERROR   - file content error           */    
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::ReadImportResult()
{
#ifdef SCHEDULED_TASK_TEST
		CTime t = CTime::GetCurrentTime();
		CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) tt << " ReadImportResult entered." << "\n";
		logFile->flush();
		logFile->close();

#endif

	char filePath[MAX_LENGTH];
	char inBuff[256];
	char buffer[256];
	char seqString[100];
	char seqFile[100];
	char rest[256];
	CString command;

	strcpy(filePath, defImporterDir);
	AppendPath(filePath, "dfresults");
	sprintf(seqString, "%d", DEFIMPSEQ);
	strcpy(seqFile, "result");
	strcat(seqFile, seqString);
	strcat(seqFile, ".dat");
	AppendPath(filePath, seqFile);
	std::ifstream ifile(filePath, std::ios::in);
   
	if (ifile.fail()) {
#ifdef SCHEDULED_TASK_TEST
		CTime t = CTime::GetCurrentTime();
		CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) tt << " No result file." << "\n";
		logFile->flush();
		logFile->close();

#endif

		return(DF_RESULT_FILE_ERROR); 
	}

	CString status;
	CString jobStatus;
	CString strTime;
	CTime   time;
	char seps[]   = " ";
	char cookieKey[MAX_LENGTH];
	char ccommand[MAX_LENGTH];
	char cnextStep[MAX_LENGTH];
   
	char key[256];
	char *token;    
	strcpy(inBuff, ""); 
	strcpy(cookieKey, ""); 
	strcpy(key, ""); 
	strcpy(ccommand, ""); 
	strcpy(cnextStep, ""); 
	int i = 1;
	int newJobSeq = 0;
	int processed = 0;
	int evalPathFlag = 0;
	CString stateString;
	CString state;
	while (ifile.getline(inBuff, 256))
	{ 
        memset(buffer, '\0', 256);
		strcpy(buffer, inBuff);
		int length = strlen(buffer);
		buffer[length] = '\0';
	  	
		if (strcmp(inBuff, "") == 0)
		  continue;
		token = strtok( inBuff, seps);
		strupr(token); 
	 	  
		if (strcmp(token, "COOKIEID:") == 0) {
		  sscanf(buffer, "%s %s ", key, cookieKey);                   
		  i = 1;
		     
		} 

		if (strcmp(token, "PARAMETERS:") == 0) {
 		  char *result;
          result = strpbrk( buffer, ":");	 
		  strcpy(ccommand, ++result); 
		  i++;

		}	
   		if (strcmp(token, "NEXTSERVICE:") == 0) {
		  sscanf(buffer, "%s %s ", key, cnextStep);                   
		  i++;
		}
		if (strcmp(token, "1122334455") == 0) {
			sscanf(buffer, "%s %s ", key, rest);                   
            i=0;		  
		}

		if (i == 3 ) {          
			CString state = CString(cnextStep);
			state.TrimRight();
			state.TrimLeft();
			CString command = CString(ccommand); 
			command.TrimRight();
			command.TrimLeft();
			int n = command.Find(' ');
			if (n== -1) {
#ifdef SCHEDULED_TASK_TEST
		CTime t = CTime::GetCurrentTime();
		CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) tt << " Error in the result file." << "\n";
		logFile->flush();
		logFile->close();

#endif

		return(DF_RESULT_FILE_ERROR); 
		}

			firstToken = command.Left(n);
			int seq = atoi((LPTSTR) (LPCTSTR) firstToken); 
			secondToken = command.Mid(n + 1);
			 //check the group
			DFEvalState::StateGroup group = Matrix.GetStateGroup(state);
            if (group  == DFEvalState::STOP) {
				 criticalErrorFlag1 = 1;
	 			 ifile.close();
#ifdef SCHEDULED_TASK_TEST
//test log
			CTime t = CTime::GetCurrentTime();
			CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
			logFile->open(testPath, std::ios::out | std::ios::app);
			*(logFile) << (LPTSTR) (LPCTSTR) tt << "Critical error!" <<  "\n";
			logFile->flush();
			logFile->close();

#endif


 				 return(DF_CRITICAL_ERROR); 
			}  
#ifdef SCHEDULED_TASK_TEST 
//test log
			CTime t = CTime::GetCurrentTime();
			CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
			logFile->open(testPath, std::ios::out | std::ios::app);
			*(logFile) << (LPTSTR) (LPCTSTR) tt << "   The result file's seq definition. " << (LPTSTR) (LPCTSTR)command << "\n";
			logFile->flush();
			logFile->close();

#endif

			 if (seq == 0 ){
				ifile.close();
#ifdef SCHEDULED_TASK_TEST
//test log
			CTime t = CTime::GetCurrentTime();
			CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
			logFile->open(testPath, std::ios::out | std::ios::app);
			*(logFile) << (LPTSTR) (LPCTSTR) tt << "   The first token is 00000000: " << (LPTSTR) (LPCTSTR)command << "\n";
			logFile->flush();
			logFile->close();

#endif
 				return(DF_RESULT_FILE_ERROR); 
			 }
		}

	}
	if (strcmp(key, "1122334455") != 0) {
		ifile.close();
#ifdef SCHEDULED_TASK_TEST
//test log
			CTime t = CTime::GetCurrentTime();
			CString tt = t.Format( "%d %B %Y,  %H:%M:%S" );
			logFile->open(testPath, std::ios::out | std::ios::app);
			*(logFile) << (LPTSTR) (LPCTSTR) tt << "   Check the last line in the result file! " <<  "\n";
			logFile->flush();
			logFile->close();

#endif

		return(DF_RESULT_FILE_ERROR); 
	}


	ifile.close();

    return DF_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:   CheckDeferredSamples                                     */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::CheckDeferredSamples()
{
#ifdef SCHEDULED_TASK_TEST
//test log
		CTime t = CTime::GetCurrentTime();
		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << "   CheckDeferredSamples entered. " << "\n";
		logFile->flush();
		logFile->close();
#endif

	if (undeferrerJob == NULL) {
#ifdef SCHEDULED_TASK_TEST
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << " Undeferred job submitted." << "\n";
		logFile->flush();
		logFile->close();

#endif

		DFDBSelect selectSamples(this);
		int rc = selectSamples.DFImportSamplesFromDB(1, DFSample::DEFERRED);
		if (rc != 0) {	
			undeferrerJob = new DFJob(UNDEFSEQ, undeferrer, CString("00000000"), CString(), UNCPath);
			undeferrerJob->SetStatus(DFJob::SUBMITTED);
			CTime t = CTime::GetCurrentTime();
   			undeferrerJob->SetSubmitTime(t);
			DFJob *transitJob = new DFJob(undeferrerJob->GetJobSeq(), undeferrerJob->GetJobId(), CString("00000000"), CString(), UNCPath);    
			(void) SubmitJob(transitJob, NULL);	
        } 
	}

  return 0;

}
/*----------------------------------------------------------------------------*/
/* Procedure name:   CheckDeferredSamples                                     */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::LaunchStatusUpdater()
{
#ifdef SCHEDULED_TASK_TEST
//test log
		CTime t = CTime::GetCurrentTime();
		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << "   LaunchStatusUpdater entered. " << "\n";
		logFile->flush();
		logFile->close();

#endif

	if (statusUpdaterJob == NULL) {
#ifdef SCHEDULED_TASK_TEST
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << " StatusUpdater job submitted." << "\n";
		logFile->flush();
		logFile->close();

#endif

		statusUpdaterJob = new DFJob(STUPDATESEQ, statusupdate, CString("00000000"), CString(), UNCPath);
		statusUpdaterJob->SetStatus(DFJob::SUBMITTED);
		CTime t = CTime::GetCurrentTime();
		statusUpdaterJob->SetSubmitTime(t);
		DFJob *transitJob = new DFJob(STUPDATESEQ, statusupdate, CString("00000000"), CString(), UNCPath);    
		(void) SubmitJob(transitJob, NULL);	
	}
    return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:   MarkOnImport                                             */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::MarkOnImport()
{

		int dsn =  GetLatestSeqNumber();
		if (dsn == 0) {
#ifdef SCHEDULED_TASK_TEST
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << " LatestSignatureError" << "\n";
		logFile->flush();
		logFile->close();

#endif
		return (DF_LATEST_SIGNATURE_ERROR);
		}
		CString strDsn;
		strDsn.Format("%d", dsn);
#ifdef SCHEDULED_TASK_TEST
//test log
		CTime t = CTime::GetCurrentTime();
		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << " The new signature from the DB is :" << (LPTSTR) (LPCTSTR) strDsn << "\n";
		*(logFile) << (LPTSTR) (LPCTSTR) time << " Samples are marked" <<  "\n";
		logFile->flush();
		logFile->close();

#endif


		if (defSeqNumber.CompareNoCase(strDsn) ){

			defSeqNumber = strDsn;
           // samples are marked when a new defseq number is imported
			(void) MarkSamples();
			BOOL rc = DFDBSelect::MarkSamplesInDB();

		}
		return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:   StopImporter                                             */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::StopImporter()
{
	if (defImportJob != NULL) {
       if (defImportJob->GetStatus() == DFJob::ACCEPTED || defImportJob->GetStatus() == DFJob::SUBMITTED) {
		    	DFJob *transitJob = new DFJob(DEFIMPSEQ, import, CString("00000000"), CString(), CString());    
				(void) IgnoreJob(transitJob);	
       }
	}

	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:   StopUndeferrer                                           */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::StopUndeferrer()
{
	if (undeferrerJob != NULL) {
       if (undeferrerJob->GetStatus() == DFJob::ACCEPTED || undeferrerJob->GetStatus() == DFJob::SUBMITTED) {
		    	DFJob *transitJob = new DFJob(UNDEFSEQ, undeferrer, CString("00000000"), CString(), CString());    
				(void) IgnoreJob(transitJob);	
	   }
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:   StopUpdater                                              */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::StopUpdater()
{
	if (statusUpdaterJob != NULL) {
       if (statusUpdaterJob->GetStatus() == DFJob::ACCEPTED || statusUpdaterJob->GetStatus() == DFJob::SUBMITTED) {
		    	DFJob *transitJob = new DFJob(STUPDATESEQ, statusupdate, CString("00000000"), CString(), CString());    
				(void) IgnoreJob(transitJob);	
       }
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:   StopUpdater                                              */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::StopAttrCollector()
{
	if (attributeCollectorJob != NULL) {
       if (attributeCollectorJob->GetStatus() == DFJob::ACCEPTED || attributeCollectorJob->GetStatus() == DFJob::SUBMITTED) {
		    	DFJob *transitJob = new DFJob(ATTRIBUTECOLLECTSEQ, attributecollect, CString("00000000"), CString(), CString());    
				(void) IgnoreJob(transitJob);	
       }
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:   LaunchSampleAttributeCollector                           */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::LaunchSampleAttributeCollector()
{
#ifdef SCHEDULED_TASK_TEST
//test log
		CTime t = CTime::GetCurrentTime();
		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << "   LaunchSampleAttributeCollector entered" << "\n";
		logFile->flush();
		logFile->close();

#endif

	if (attributeCollectorJob == NULL) {
#ifdef SCHEDULED_TASK_TEST
		logFile->open(testPath, std::ios::out | std::ios::app);
		*(logFile) << (LPTSTR) (LPCTSTR) time << " AttributeCollector job submitted." << "\n";
		logFile->flush();
		logFile->close();

#endif

		attributeCollectorJob = new DFJob(ATTRIBUTECOLLECTSEQ, attributecollect, CString("00000000"), CString(), UNCPath);
		attributeCollectorJob->SetStatus(DFJob::SUBMITTED);
		CTime t = CTime::GetCurrentTime();
		attributeCollectorJob->SetSubmitTime(t);
		DFJob *transitJob = new DFJob(ATTRIBUTECOLLECTSEQ, attributecollect, CString("00000000"), CString(), UNCPath);    
		(void) SubmitJob(transitJob, NULL);	
	}
    return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FinishPackaging                                       */
/*                                                                            */
/* Description:         Update database with new signature a                  */
/*                                                                            */
/* Input:               DFSample *sample                                      */
/*                                                                            */
/* Return:                                                                    */
/*                                                                            */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFManager::FinishPackaging(DFSample *pSample)
{
#ifdef _DEBUG
		(void) LogActivity(1, "FinishPackaging started. CookieId=", pSample->GetCookieId());
#endif

		int localCookie = atoi((LPTSTR) (LPCTSTR) pSample->GetCookieId());

		if (AddSignature(packSample)) {
#ifdef _DEBUG
		(void) LogActivity(1, "FinishPackaging ended(1). CookieId=", pSample->GetCookieId());
#endif
       		serialFlag = FALSE;
	       	return(IDS_DATABASE_UPDATE_RECORD_ERROR); 
        } 
 		
		if ( pSample->GetDefSeqNumber() != CString() && defSeqNumber.CompareNoCase(pSample->GetDefSeqNumber()) ){
				defSeqNumber = pSample->GetDefSeqNumber();
                // samples are marked when a new defseq number is created 
#ifdef _DEBUG
		(void) LogActivity(1, "FinishPackaging - Before marking samples. CookieId=", pSample->GetCookieId());
#endif

				(void) MarkSamples();
#ifdef _DEBUG
		(void) LogActivity(1, "FinishPackaging - After marking samples. CookieId=", pSample->GetCookieId());
#endif
#ifdef _DEBUG
		(void) LogActivity(1, "FinishPackaging - Before marking samples in the database. CookieId=", pSample->GetCookieId());
#endif


				BOOL rc = DFDBSelect::MarkSamplesInDB();
#ifdef _DEBUG
		(void) LogActivity(1, "FinishPackaging - After marking samples in the database. CookieId=", pSample->GetCookieId());
#endif

		}

		try {
			std::string strAnalysisState;
			strAnalysisState = std::string("infected");
			AnalysisStateInfo stateObject(strAnalysisState);
			AnalysisRequest request(localCookie);
			// FinalState function has to update the defseq number 
			int intDsn = atoi((LPTSTR) (LPCTSTR) defSeqNumber);
			request.FinalState(stateObject, intDsn);
			
		} 
		catch (AVISDBException exception) {
				char msg[1028];
				detailedString = exception.DetailedInfo();
				std::ostrstream inMsg(msg, sizeof(msg) - 1);
				inMsg << detailedString;
				inMsg << std::ends;
				AfxMessageBox(msg, MB_ICONSTOP, 0);
				return(IDS_DATABASE_UPDATE_RECORD_ERROR); 
		}
		serialFlag = FALSE;
#ifdef _DEBUG
		(void) LogActivity(1, "FinishPackaging ended(2). CookieId=", pSample->GetCookieId());
#endif

        return 0;             
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      LogMessage                                            */
/* Description:         To log messages                                       */
/*                                                                            */
/* Input:               None                                                  */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void DFManager::LogActivity(int logFlag, CString string1, CString string2)
{
 

	 testFile->open(logPath, std::ios::out | std::ios::app);

	 if (logFlag  == 0) {

		CTime t = CTime::GetCurrentTime();
		CString logTime = t.Format( "%d %B %Y,  %H:%M:%S" );
	    *(testFile) <<  (LPTSTR) (LPCTSTR)logTime << (LPTSTR) (LPCTSTR)string1 << "\n";
		testFile->flush();
	    testFile->close();
		return;

    }
	else {
		CTime t = CTime::GetCurrentTime();
		CString logTime = t.Format( "%d %B %Y,  %H:%M:%S" );
	    *(testFile) <<  (LPTSTR) (LPCTSTR)logTime << (LPTSTR) (LPCTSTR)string1 << (LPTSTR) (LPCTSTR)string2 << "\n";
		testFile->flush();
	    testFile->close();
		return;


    }
	return;
}
