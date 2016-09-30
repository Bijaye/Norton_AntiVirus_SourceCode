/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFManager.h                                                     */
/* FUNCTION:  To manage samples                                               */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY:  September 3, 1998  SNK                                           */
/*----------------------------------------------------------------------------*/
#ifndef DFMANAGER_H
#define DFMANGER_H

//#define SCHEDULED_TASK_TEST
class DFManager 
{
public:
	int CreateConfigFile();
	DFManager();
	~DFManager() {
#ifdef _DEBUG 
		if (testFile) {
     		  testFile->close(); delete testFile;
        }
		      
#endif
//#ifdef SCHEDULED_TASK_TEST
//		delete logFile;
//#endif
    
	}

    int Init();

	// reporting and logging
    enum ReportStage {UNDEFINED, ARRIVAL, SUBMISSION, ACCEPTANCE, COMPLETION}; 
	int ReportToLog(long report, long error, const char *string);
    int ReportToStatusDatabase(DFSample *pSample, ReportStage stage);  
	int ReportToStatusDatabase(DFSample *pSample, ReportStage stage, BOOL scanFlag = FALSE );

	int MessageLogging(CString id, int seq, CString strStatus, int returncode);

    // get and process new samples
	int GetNewSamplesForProcessing();
	int GetNewSamplesForRescan();
	int ProcessArrivedCookie(CString cookieId, CString state, DFSample::SampleStatus status, int priority, DFSample::Origin origin, CTime t);
	int ProcessArrivedForScanCookie(CString cookieId, CString state, DFSample::SampleStatus status, int priority, DFSample::Origin origin, CTime t );

    // job processing  
    int SubmitSamplesForProcessing();
	int SubmitJob(DFJob *job, DFSample *pSample);
	int IgnoreJob(DFJob *job);

    // service methods 
	int GetCurSel(void);
    DFSample *FindObjectWithIdInHPList(CString cookieId, POSITION& pos);             
	void AppendPath(char *lpszFirst,char *lpszSec);
	int IsFileFound(char *fileName );

	// Processing of Launcher's messages
	int  ProcessCompletedJob(DFJob *pJob);
	int  ProcessAcceptedJob(DFJob *pJob);
	int  ProcessPostponedJob(DFJob *pJob);
    int  ProcessTimedoutJob(DFJob *pJob);
    int  ProcessIgnoredJob(DFJob *pJob);

    // sample, job status determination 
    int DetermineSampleStatus(DFSample *pSample);
	int DetermineSampleStatusPlus();
    int LoadSampleData(DFJob *job);
	int ReadStopCondition(DFJob *jobObj);
    CString GetCurrentJobStatus(DFJob::JobStatus status);

    // launcher
	void SetLauncherHandle(HWND wnd) { DFLWnd = wnd;}
    void SetMainWindowHandle(CWnd  *gui) { mainWnd = gui;}

    // handling user's request
	void SetupDisableRequest();
	void SetupEnableRequest();
	void SetupDeferRequest();
	BOOL IsAtLeastOneOnHold();
	BOOL IsAtLeastOneNotOnHold();

    // cleaning up
	//void CleanupDir(char *dirPath);
	//void CleanupAllCookieDirs();
	//void CleanDirInfoList();

	// termination
	void TermManager();

    // sample's directories
    DFDirInfo *FindObjectInDirList(CString dirPath);
	void LogMessage(int flag, CString dirPath);
	 
	// configuration file
	int SaveConfigDefaultSettings();

    // change priority
	int ChangeCookiePriority(DFSample *pSample);
	int UpdatePriority();

	BOOL SerializeSubmit(DFJob *job, DFSample *pSample);
	BOOL PreSubmitScanJob(DFSample *pSample);
	BOOL SubmitScanJob(DFSample *pSample);
	int ReadScanResult(DFSample *pSample);
    void MarkSamples();
//    BOOL CreatePackageAndSignaturFiles(DFSample *pSample);
	BOOL AddSignature(DFSample *pSample);
	int GetLatestSeqNumber();
	int ImportNewDefinitions();
	int ReadImportResult();
	int CheckDeferredSamples();
	int LaunchStatusUpdater();
	int LaunchSampleAttributeCollector();
	int MarkOnImport();
	int StopImporter();
	int StopUndeferrer();
	int StopUpdater();
    int StopAttrCollector();
	int FinishPackaging(DFSample *pSample);
	void LogActivity(int logFlag, CString string1, CString string2);

    DFSampleList		sampleHPList;			 // sample's collection
	DFDirList           dirInfoList;			 // dir collection 
	DFJobList           schedJobList;			 // collection of scheduled jobs    
	HWND                DFLWnd;					 // launcher
    CWnd                *mainWnd;				 // main window
    char                UNCPath[MAX_PATH];		 // UNC path	
    char                logPath[MAX_PATH];		 // log path	
    char                buildDefFilename[MAX_PATH];   // build definition filename	
    char                defBaseDir[MAX_PATH];    // definition base directory	
    char                defImporterDir[MAX_PATH];// definition importer directory	
    char                modulePath[MAX_PATH];	 // module name	
    char                attributeFile[MAX_PATH]; // attribute file	
	long				sampleSubmissionInterval;
    long				sampleArrivalCheckInterval;
    long				databaseRescanInterval;
    long				defImportInterval;
    long				statusUpdateInterval;
    long				undeferrerInterval;
    long				attributeInterval;
	long                maxSamples;
	DFMatrix            Matrix;					// evaluator matrix
	CTime               checkTime;
	std::ofstream       *testFile;     
	std::ofstream       *logFile;     
    CTime               startTime;
	long                arrivedSamples;
	long                successfulSamples;
	long                rescannedSamples;
	long                deferredSamples;
	long                criticalSamples;
	long                returnedBackSamples;
	long                averageProcessingTime;
    int                 UNCExistanceFlag;  
	long                arrivalCount;
	POSITION            currentPos;
	CString             deferring;
	CString             newSample;
	CString             serializing;
	CString             import;
	CString             undeferrer;
	CString             statusupdate;
	CString             attributecollect;
	std::string	        detailedString; 
	BOOL                serialFlag;
	CString				defSeqNumber;     	// def seq number 
	CString				virusId;			// virus id 
	CString				virusName;			// virus name 
 	int                 criticalErrorFlag;   
 	int                 criticalErrorFlag1;   
 	int                 deferCriticalSample;   
    CString             scan;
    CString             criticalSample;
	DFJob				*defImportJob;			// def import job 
	DFJob				*undeferrerJob;			// undeferrer job 
	DFJob				*statusUpdaterJob;		// update status job 
	DFJob				*attributeCollectorJob; // update status job 
	char                testPath[MAX_PATH];
    CString             firstToken; 
    CString             secondToken; 
    char                computerName[MAX_COMPUTERNAME_LENGTH + 1];	// computer name	
    int                 importPending; 
	CWinThread          *pObj; 
	CDFPackThread       *packThread;
	DFSample            *packSample; 
	int                 closeWndRequest;
	int                 bgtrap;
	char                avisdflog[MAX_PATH];

private:
	std::ofstream outfile;


    DFManager(const DFManager &);
    DFManager &operator =(const DFManager &);
 

};


#endif 
