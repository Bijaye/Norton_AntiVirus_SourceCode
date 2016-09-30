/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFDBRequests.h                                                  */
/* FUNCTION:  To query the database                                           */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY:                                                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFDBREQUESTS_H
#define DFDBREQUESTS_H

class DFDBRequests
{   
public:
	DFDBRequests();
	~DFDBRequests();
	static int GetCookiesInStateWithPriority(DFArrivedList &arrivedList,int neededNumber, DFSample::Origin origin);
	static int UpdateCookieRecord(int localCookie, DFManager::ReportStage stage, CString state);
	static int DbOpen();
	static int DbClose();
    DFManager           *manager;                                     // manager 
	static int UpdateAnalysisStateInfoTable(CString strState);
	static int GetCookiePriority(int localCookie);
	static int GetCookieAnalysisStateId(int localCookie);
    static int GetStateId(CString strState);

private:
    
   DFDBRequests(const DFDBRequests &);
   DFDBRequests &operator =(const DFDBRequests &);
 

};


#endif 
