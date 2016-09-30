#ifndef RCPROCESS_H
#define RCPROCESS_H

#define PROCESS_STILL_ACTIVE 259


// class definition
class RCProcess 
{
     public:
		 RCProcess() {}
		 void Init(RCCommon *Common);
        int StartProcess(CString commandline, int InitializeTime,  PROCESS_INFORMATION *pPI);
        long SyncSystem(CString commandline, long Timeout, char *Usname) ;
    private:
        RCError Error;
		RCCommon *Common;
};
#endif
