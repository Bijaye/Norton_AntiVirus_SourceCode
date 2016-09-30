/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFSample.h                                                      */
/* FUNCTION:  To create a sample object                                       */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* HISTORY:  August 14, 1998                                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFSAMPLE_H
#define DFSAMPLE_H

class DFDBRequests;

class DFSample 
{
public:
	enum  SampleStatus {UNDEFINED, WAIT, SUCCESS, FAIL, INPROGRESS, FINAL, CriticalError};
	enum  SamplePrior {HIGH, LOW}; 
    enum  UserRequest {NONE, PAUSE, RESUME, STOP, DISABLE, ENABLE, DEFER};
	enum  Origin {UNKNOWN, IMPORTED, RESCAN, DEFERRED };

    // constructing a sample object
 	DFSample(CString Id);
	DFSample() { ofile = NULL;}
	DFSample(CString Id, CString state, SampleStatus status);
	~DFSample();

    //sample 
	DFSample* FindObjectWithCookieId(CString cookieId);  
	CString GetCookieId() { return cookieId;} 
	void SetCookieId(CString cookie) { cookieId = cookie;} 

    //state
//	int GetCurrentState() { return currentState;} 
	CString GetCurrentStateStr() {return strCurrentState;}
	void SetCurrentState(int state) { currentState = state;}
	void SetCurrentStateStr(CString state) { strCurrentState = state;}
//	int GetNextState() { return nextState;}
	void SetNextState(int state) { nextState = state;}
	CString GetNextStateStr() { return strNextState;}
	void SetNextStateStr(CString strState) { strNextState = strState;}
	CString GetPreviousStateStr() { return strPreviousState;}
	void SetPreviousStateStr(CString strState) { strPreviousState = strState;}
	void SetAcceptedState(CString state) { acceptedState = state;}
	CString GetAcceptedState() { return acceptedState;}

    //status 
	SampleStatus GetCurrentStatus() { return currentStatus;} 
	CString GetCurrentStatusStr(SampleStatus status); 
	void SetCurrentStatus(SampleStatus status) { currentStatus = status;} 

    // init condition
	int GetInitCond() { return initCond;}
	void SetInitCond(int cond) { initCond = cond;}
	void AddInitCond() { ++initCond;}

    // stop condition
	int GetStopCond() { return stopCond;}
	void SetStopCond(int cond) { stopCond = cond;}
    BOOL IsStopCondMet() { return stopCondFlag;}
	void SetStopCondMet (BOOL flag) { stopCondFlag = flag;}  

    //jobs 
	int GetDoneJobs() { return doneJobs;}
	void SetDoneJobs(int done) {  doneJobs = done;}
	int GetSuccessJobs() { return successJobs;}
	void AddSuccessJobs() { ++successJobs;}
	void SubtractSuccessJobs() { --successJobs;}
	void AddDoneJobs() { ++doneJobs;}
	void SetSuccessJobs(int success) { successJobs = success;}
	int GetInProgressJobs() { return inProgressJobs;}
	void AddInProgressJobs() { ++inProgressJobs;}
	void SubtractInProgressJobs() { --inProgressJobs;}
	void SetInProgressJobs(int inprogress) { inProgressJobs = inprogress;}

    // seq
	int GetSeqNumber() { return seqNumber;}
	void CorrectSeqNumber(int delta) { if (seqNumber  != 0) {seqNumber = seqNumber - delta;} }
	void SetSeqNumber() { seqNumber++;}
    DFJob *FindJobWithSeq(int seq, POSITION& pos);

    // user's request
	void SetUserRequest(UserRequest request) {userRequest = request;}
	UserRequest GetUserRequest() {return userRequest;}

    // origin
	Origin GetOrigin() { return origin;}
	void SetOrigin(Origin org) { origin = org;}
	// priority
	int GetPriority() { return priority;}
	void SetPriority(int prt) { priority = prt;}

	// scanning
	BOOL IsScanRequired() { return scanRequired;}
	void SetScanRequired(BOOL  flag ) { scanRequired = flag;}
	// set def sequence number
	BOOL SetDefSeqNumber(CString strParam);
    CString GetDefSeqNumber() { return defSeqNumber; }     
	//scanning
    BOOL GetScanRequired() { return scanRequired;} 
	BOOL SetScanResults(CString strParam);

    //time
	void SetArrivalTime(CTime time) { arrivalTime = time;} 
	void SetSubmissionTime(CTime time) { submissionTime = time;} 
	void SetCompletionTime(CTime time) { completionTime = time;} 
    CTime GetArrivalTime() { return arrivalTime;} 
    CTime GetSubmissionTime() { return submissionTime;} 
    CTime GetCompletionTime() { return completionTime;} 
    
	// delete collection
	int  DeleteAllJobs();

    // set cookie's state in the Database 
	int SetSampleState(CString state);
    

    DFJobList jobList;						// collection of jobs
	DFJobList jobNewList;					// collection of new jobs
	DFJob     *scanJob;                     // scan job 
    int stopSatisfied;						// debug flag
	std::ofstream *ofile;         
	std::ofstream *dfstatus;				// dfstatus.dat file - processing log
	std::ofstream *dffinish;				// dffinish.dat file - file to signal the end of processing
	std::ofstream *dfstart;					// dfstart.dat file -  file to signal the beginning of processing
    BOOL     statusDet;						// status has to be redetermined 
	BOOL     ignoreFlag;					// ignore flag
    BOOL     priorityFlag;
	CString  unc;
	BOOL     noBuildFlag;                   // build flag;
	BOOL     scanArchived;                    
	CString  packageName;                   // name of the built package file
	int      criticalErrorFlag;
	int      dffinishFlag;
    int		 pipelineFlag;
    int		 evalPathFlag;                 // failure and next step is taken from EVAL  
    int      serializeStateFlag; 
    char     buildDefFilename[MAX_PATH];   // build definition filename	
    char     defBaseDir[MAX_PATH];    // definition base directory	
    BOOL     packDone;
	CString  defSeqNumber;                  // def seq number 
    int      deferCriticalSample;

private:
	CString cookieId;						// cookie id
	int currentState;                       // current state 
	int nextState;                          // next state 
	CString strNextState;                   // next state 
	CString strCurrentState;                // current state
	CString strPreviousState;               // previous state
	SampleStatus  currentStatus;            // current status 
	int initCond;                           // number of init cond  
	int stopCond;                           // number of state cond
	int doneJobs;                           // number of jobs finished
	int successJobs;                        // number of successful jobs 
	int inProgressJobs;                     // number of jobs in proscessing
	int seqNumber;                          // sequence number
	BOOL stopCondFlag;                      // stop cond flag 
	UserRequest userRequest;                // user's request
	CTime arrivalTime;                      // arrival time
	CTime submissionTime;                   // submission time
	CTime completionTime;                   // completion time
    int priority;                           // priority
    Origin origin;	                        // origin of a sample (imported, defqueue, defcompiler)
	CString acceptedState;                  // accepted state
	BOOL scanRequired;                      // 'scan required' flag 
	CString  virusId;						// virus id 
 	CString  virusName;						// virus name 
    
    DFSample(const DFSample &);
    DFSample &operator =(const DFSample &);
 

};
// collection of processed samples
typedef CTypedPtrList<CPtrList, DFSample*> DFSampleList;  
// collection of arrived samples
typedef CTypedPtrList<CPtrList, DFSample*> DFArrivedList; 


#endif 
