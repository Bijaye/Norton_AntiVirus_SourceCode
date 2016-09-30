/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFDBSelect.cpp                                                  */
/* FUNCTION:  To interface with the Status database                           */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY: 21 SEPTEMBER, 1998  SNK                                           */
/*----------------------------------------------------------------------------*/
#include "afxwin.h"
#include "afxtempl.h"
#include <iostream>
#include <fstream>
#include <ios>
#include <strstream>
#include "dfpackthread.h"
#include "paramvalue.h"
#include "resource.h"
#include "dfdirinfo.h"

#ifndef DFEVALSTATUS_H
	#include "dfevalstatus.h"
#endif
#ifndef DFEVALSTATE_H
	#include "dfevalstate.h"
#endif
#ifndef DFJOB_H
	#include "dfjob.h"
#endif

#ifndef DFERROR_H
	#include "dferror.h"  
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
#ifndef DFMANAGER_H
	#include "DFManager.h"
#endif

#ifndef DFDBSELECT_H
	#include "dfdbselect.h"
#endif
#ifdef DATABASE_INTERFACE
	#include "AnalysisStateInfo.h"
	#include "AnalysisRequest.h"
#else
    #include "dfdbrequests.h"
#endif

/*----------------------------------------------------------------------------*/
/* Procedure name:      SelectFirst                                           */
/* Description:         To select a sample from the arrived collection        */
/*                                                                            */
/* Input:               reference  to a sample object                         */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_SUCCESS - success                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBSelect::SelectFirst(DFSample &sample)
{
  DFSample *selectSample = arrivedList.GetTail();
  CString cookie = selectSample->GetCookieId();
  CString state = selectSample->GetCurrentStateStr();
  DFSample::SampleStatus status = selectSample->GetCurrentStatus();
  DFSample::Origin origin = selectSample->GetOrigin();
  CTime t = selectSample->GetArrivalTime();
  int prior = selectSample->GetPriority();
  sample.SetCookieId(cookie);
  sample.SetOrigin(origin); 
  sample.SetCurrentStateStr(state);
  sample.SetCurrentStatus(status);
  sample.SetArrivalTime(t);
  sample.SetPriority(prior);
  arrivedList.RemoveTail();
  delete selectSample;
  return(DF_SUCCESS); 
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
void DFDBSelect::AppendPath(char *lpszFirst,char *lpszSec)
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
/* Procedure name:      IsFileFound                                           */
/* Description:         To check for file existance                           */
/*                                                                            */
/* Input:               file name                                             */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_SUCCESS - success                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
/*
int DFDBSelect::IsFileFound(char *fileName )
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
/*----------------------------------------------------------------------------*/
/* Procedure name:      ReadConfigFile                                        */
/* Description:         To check the config file                              */
/*                                                                            */
/* Input:               char *filePath - avisdf.cfg path                      */
/*                      DFDBSelect::ConfigKey key                             */                                            
/*                                                                            */
/* Output:              None                                                  */
/* Return:              DF_SUCCESS - success                                  */
/*                      DF_CONFIG_FILE_ERROR - file error                     */
/*                      DF_FILE_OPEN_ERROR   - no file                        */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBSelect::ReadConfigFile(char *value, DFDBSelect::ConfigKeys key )
{
    FILE *fp;
    BOOL flag = FALSE;
	char modulePath[1024];
	
    
    CParamValue paramValue;
    HINSTANCE hInst = AfxGetInstanceHandle();
    (void) GetModuleFileName(hInst, modulePath, 1024);
    char *p = strrchr(modulePath, '\\');
    *p = '\0';
	CString cfgFile = CString((LPCSTR) IDS_CFG_FILE); 
    AppendPath(modulePath, (LPTSTR) (LPCTSTR)cfgFile); 
	fp = fopen (modulePath, _T("r"));
    if (fp != (FILE *) NULL)
    {
        int numParam;

 		while (TRUE) {
            numParam = paramValue.ReadParamValue (fp, TRUE, TRUE);
	        if (!numParam)
		         return (DF_CONFIG_FILE_ERROR);

			CString str;
			BOOL brc;
			switch (key) {
			case UNCPATH:
			{
                brc = paramValue.GetValue (CString((LPCSTR) IDS_UNCPATH), str);
				if (brc == 0) {
					strcpy(value, (LPTSTR) (LPCTSTR)str);
					fclose(fp);
					return(DF_UNC_EMPTY);
				}

			}	
			break;
			case BuildDefFilename:
			{
                brc = paramValue.GetValue (CString((LPCSTR) IDS_BUILD_DEF_FILENAME), str);
				if (brc == 0) {
					strcpy(value, (LPTSTR) (LPCTSTR)str);
					fclose(fp);
					return(DF_BUILD_DEF_FILENAME_EMPTY);

					
				}

			}	
			break;
			case DefBaseDir:
			{
                brc = paramValue.GetValue (CString((LPCSTR) IDS_DEF_BASE_DIR), str);
				if (brc == 0) {
					strcpy(value, (LPTSTR) (LPCTSTR)str);
					fclose(fp);
 					return(DF_BASE_DEF_DIR_EMPTY);

				}

			}
			break;
			case DefImporterDir:
			{
                brc = paramValue.GetValue (CString((LPCSTR) IDS_DEF_IMPORTER_DIR), str);
				if (brc == 0) {
					strcpy(value, (LPTSTR) (LPCTSTR)str);
					fclose(fp);
 					return(DF_DEF_IMPORTER_DIR_EMPTY);

				}

			}
			break;
			case SampleSubmissionInterval:
			{
		        brc = paramValue.GetValue (CString((LPCSTR) IDS_TIMER_INTERVAL ), str);
			}
			break;
			case SampleArrivalCheckInterval:
			{
		        brc = paramValue.GetValue (CString((LPCSTR) IDS_SAMPLE_REQUEST_DELAY ), str);
			}
            break;
			case DatabaseRescanInterval:
			{
		        brc = paramValue.GetValue (CString((LPCSTR) IDS_DATABASE_RESCAN_INTERVAL ), str);
			}
            break;
			case DefImportInterval:
			{
		        brc = paramValue.GetValue (CString((LPCSTR) IDS_DEF_IMPORT_INTERVAL ), str);
			}
            break;
			case StatusUpdateInterval:
			{
		        brc = paramValue.GetValue (CString((LPCSTR) IDS_STATUS_UPDATE_INTERVAL ), str);
			}
            break;
			case UndeferrerInterval:
			{
		        brc = paramValue.GetValue (CString((LPCSTR) IDS_UNDEFERRER_INTERVAL ), str);
			}
            break;
			case AttributesInterval:
			{
		        brc = paramValue.GetValue (CString((LPCSTR) IDS_ATTRIBUTE_INTERVAL ), str);
			}
            break;
			case ConcurrentNumberOfSamplesToBeProcessed:
			{
		        brc = paramValue.GetValue (CString((LPCSTR) IDS_MAX_SAMPLES ), str);
			}
			break;
            default:
				;
			}
			if (brc) {
				strcpy(value, (LPTSTR) (LPCTSTR)str);
				fclose(fp);
			    return(DF_SUCCESS);
		    }
			else {
				fclose(fp);
				return (DF_CONFIG_FILE_ERROR);
			}	
                
		}
	}
	else {
		return (DF_FILE_OPEN_ERROR); 
	} 
     
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      DFImportSamples                                       */
/* Description:         To import new samples from the UNC directory          */
/*                                                                            */
/* Input:               needed number - number of samples                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              None                                                  */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/ 
/*
void DFDBSelect::DFImportSamples(int neededNumber)
{
    char cookiePath[MAX_PATH];
	char startFileName[MAX_PATH];
	char finishFileName[MAX_PATH];
	char statusFileName[MAX_PATH];
	DFSample::Origin whereFrom;
	DFSample::SampleStatus status;

    char temp[MAX_PATH]; // temporary for directory
    int rc; // return code from findfirst
    long hFile;
	struct _finddata_t result;   
    strcpy(temp, manager->UNCPath);
    strcat(temp,"\\*.*");

    hFile = _findfirst(temp, &result);
    rc = (hFile == -1) ? 1 : 0;
    while (rc == 0) {
		if(result.attrib & _A_SUBDIR ) {
			strcpy(cookiePath, manager->UNCPath);
			AppendPath(cookiePath, result.name);
			strcpy(startFileName, cookiePath);
			AppendPath(startFileName, "dfstart.dat"); 
			strcpy(finishFileName, cookiePath);
			AppendPath(finishFileName, "dffinish.dat"); 
			strcpy(statusFileName, cookiePath);
			AppendPath(statusFileName, "dfstatus.dat"); 
            // check the cash for the sample
			CString dirPath = CString(cookiePath);
            DFDirInfo *dirInfoObj = manager->FindObjectInDirList(dirPath);
			if (dirInfoObj == NULL) {
				int rc1 = IsFileFound(startFileName);  // is dfstart.dat there?
				int rc2 = IsFileFound(finishFileName); // is dffinish.dat there?
				int rc3 = IsFileFound(statusFileName); // is dfstatus.dat there?

				if (rc1 == 0  && rc2 != 0 && rc3 != 0) {
					status = DFSample::WAIT;
					whereFrom = DFSample::IMPORTED;
					DFSample *pSample = new DFSample(result.name, "NEWSAMPLE", status); 
					pSample->SetOrigin(whereFrom);
  					CTime t = CTime::GetCurrentTime();
   					pSample->SetArrivalTime(t);
					arrivedList.AddTail(pSample);
					if (arrivedList.GetCount() == neededNumber) 
						break;
				}
				if (rc1 == 0) {
					CTime t = CTime::GetCurrentTime();
					dirInfoObj = new DFDirInfo(dirPath, t);
					manager->dirInfoList.AddTail(dirInfoObj); 
#ifdef _DEBUG
					manager->LogMessage(0, dirPath);
#endif
                }
			}
		}
		rc = _findnext(hFile, &result);
	
    }
	_findclose( hFile );
    
	return;
}
*/
int DFDBSelect::DFImportSamplesFromDB(int neededNumber, DFSample::Origin stage) 
{
	char sampleStatus[MAX_LENGTH];
	DFSample::Origin whereFrom;
	DFSample::SampleStatus status;

	if (stage == DFSample::IMPORTED) {  
		if (neededNumber == 0 )
			  return 0;
	  	strcpy(sampleStatus, "imported");
	  	status = DFSample::WAIT;
		whereFrom = DFSample::IMPORTED;

    }
	if (stage == DFSample::RESCAN) {  
	  	strcpy(sampleStatus, "rescan");
  	  	status = DFSample::WAIT;
		whereFrom = DFSample::RESCAN;

    }
	if (stage == DFSample::DEFERRED) {  
	  	strcpy(sampleStatus, "deferred");
    }
	
    int rc;

  	std::string strAnalysisState;
	strAnalysisState = std::string(sampleStatus);
	AnalysisStateInfo analysisStateObject( strAnalysisState );

	
	AnalysisRequest analysisRequestObj;
    
    

    rc  = analysisRequestObj.HighestPriorityInState( analysisStateObject, 
         analysisRequestObj );

	if (stage == DFSample::DEFERRED)   
            return rc;

	if (rc) {
		long localCookie = analysisRequestObj.LocalCookie();
		int priority = analysisRequestObj.Priority();

		CString cookie;
		cookie.Format("%d", localCookie);
		CString state;
		if (whereFrom == DFSample::RESCAN)
			state = manager->scan;
		else
			state = manager->newSample;

		DFSample *pSample = new DFSample(cookie, state, status); 

		pSample->SetOrigin(whereFrom);
		pSample->SetPriority(priority);
		CTime t = CTime::GetCurrentTime();
		pSample->SetArrivalTime(t);
  		arrivedList.AddTail(pSample);
    }

  return rc;	
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
BOOL DFDBSelect::MarkSamplesInDB()
{
		try {
			std::string strImported, strRescan;
			strImported = std::string("imported");
			strRescan = std::string("rescan");
			AnalysisStateInfo importedObj(strImported);
			AnalysisStateInfo rescanObj(strRescan);
			AnalysisRequest analysisRequestObj;

			if( analysisRequestObj.ChangeAllInStateToAnotherState(importedObj, rescanObj))
				return TRUE;
			else 
				return FALSE;
							
		} 
		catch (AVISDBException exception) {
			char msg[1028];
			std::string	        detailedString; 

			detailedString = exception.DetailedInfo();
			std::ostrstream inMsg(msg, sizeof(msg) - 1);
   			inMsg << detailedString;
			inMsg << std::ends;
			AfxMessageBox(msg, MB_ICONSTOP, 0);
			return(IDS_DATABASE_UPDATE_RECORD_ERROR); 
		}


}
