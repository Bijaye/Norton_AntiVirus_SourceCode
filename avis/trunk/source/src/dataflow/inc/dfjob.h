/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFJob.h                                                         */
/* FUNCTION:  To create a job object                                          */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY:  September 8, 1998 - SNK                                          */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFJOB_H
#define DFJOB_H


#define STOP_CONDITION 1
#define JOB_RESUBMIT_INTERVAL  0   // sec


class DFJob 
{
public:
     enum JobStatus { UNDEFINED, ACCEPTED, SUBMITTED, POSTPONED,  SUCCESS, FAIL, TIMEDOUT, IGNORED, COMPLETED };
     DFJob(int seq, CString Id, CString cookie, CString command) {
             jobSeq = seq;
             jobId = Id;
             status = UNDEFINED; 
             cookieId = cookie;
             commandArg = command;
             nextType = DFEvalState::UNDEFINED;
             UNCPath.Empty();
     }; 
     DFJob(int seq, CString Id, CString cookie, CString command, CString unc) {
             jobSeq = seq;
             jobId = Id;
             status = UNDEFINED; 
             cookieId = cookie;
             commandArg = command;
             nextType = DFEvalState::UNDEFINED;
             UNCPath = unc;
     }; 

     ~DFJob(){;}

     CString GetCookieId() { return cookieId;} 
     CString GetJobId() { return jobId;} 
     CString GetCommandArg() { return commandArg;} 

	 int GetJobSeq() {return jobSeq;}
//     CTimeSpan GetTimeOut() {return timeOut;}
	 CTime GetSubmitTime () { return submitTime;}
	 CTime GetPostponedTime() { return postponedTime;} 
     JobStatus GetStatus() { return status;}  
     CString GetUNCPath() { return UNCPath; }
	 void SetCookieId(CString cookie){ cookieId = cookie;}
 	 void SetJobId(CString job) { jobId = job;}
     void SetCommandArg(CString arg ) { commandArg = arg; } 
	 void SetJobSeq(int seq) { jobSeq = seq;}
//	 void SetTimeOut(CTimeSpan time) { timeOut = time;}
	 void SetSubmitTime(CTime time) { submitTime = time;} 
	 void SetPostponedTime(CTime time) { postponedTime = time;} 
     void SetStatus (JobStatus jobStatus) {status = jobStatus;}
     void SetNextStateType(DFEvalState::StateType type) {nextType = type;}
	 DFEvalState::StateType  GetNextStateType() { return  nextType;} 
     void SetUNCPath(CString Path) { UNCPath = Path; }
      
   private:

     int jobSeq;
     CString jobId;
     CString cookieId;
     CString commandArg;
     JobStatus status;
     CTime submitTime;
     CTime postponedTime;
     DFEvalState::StateType nextType;
     CString UNCPath;


     DFJob(const DFJob &);
     DFJob &operator =(const DFJob &);
};

typedef CTypedPtrList<CPtrList, DFJob*> DFJobList;


#endif 
