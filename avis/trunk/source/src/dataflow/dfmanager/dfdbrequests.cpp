/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFDBRequests.cpp                                                */
/* FUNCTION:  To open, close and query the database                           */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY: 10 April, 1999  SNK                                               */
/*----------------------------------------------------------------------------*/
#include "afxtempl.h"
#include "afxwin.h"
#include <iostream>
#include <fstream>
#include <strstream>
#include "dfpackthread.h"
#include "dfevalstatus.h"
#include "dfevalstate.h"
#include "dfjob.h"
#include "dfsample.h"
#include "sqlext.h"
#include "dfdirinfo.h"
#include "paramvalue.h"
#include "resource.h"
#include "dfdefaults.h"
#include "dfevalstatus.h"
#include "dfevalstate.h"
#include "dfmsg.h" 
#include "dfmatrix.h"
#include "dfjob.h"
#include "dfsample.h"
#include "dferror.h"  
#include "DFManager.h"
#include "dfdbrequests.h"
#define BUF_SIZE 256

SQLHENV phenv;
SQLHDBC phdbc;
SQLHSTMT pstmt;
/*----------------------------------------------------------------------------*/
/* Procedure name:      DbOpen                                                */
/* Description:         To open the database                                  */
/*                                                                            */
/* Input:               None                                                  */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - success; -1, otherwise                            */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBRequests::DbOpen()
{
//	char		buffer[BUF_SIZE];
	SQLRETURN	retcode;
//	SQLCHAR		szConnStrIn[BUF_SIZE];
//	char		*pszConnStrIn;
//	SQLCHAR		szConnStrOut[BUF_SIZE];
//	SQLSMALLINT	cbConnStrOut;

	// Allocate the environment handle and set environment attribute
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &phenv);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
	//AfxMessageBox("Error allocating Environment Handle",
	//		MB_ICONSTOP, 0);
		return (-1);
	}
	retcode = SQLSetEnvAttr(phenv, SQL_ATTR_ODBC_VERSION,
				(SQLPOINTER)SQL_OV_ODBC3, 0);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error setting Environment Attributes",
		//	MB_ICONSTOP, 0);
		return (-1);
	}

	// Allocate the connection handle
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, phenv, &phdbc);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error allocating Connection Handle",
		//	MB_ICONSTOP, 0);
		return (-1);
	}
    //strcpy(buffer1, "AVISdata");
	retcode = SQLConnect(phdbc, 	(SQLCHAR *) "avisdata", SQL_NTS,
									(SQLCHAR *) "", SQL_NTS,
									(SQLCHAR *) "", SQL_NTS);

	if (retcode != SQL_SUCCESS) {
		//AfxMessageBox("Fatal Error Connecting to the Database. Exiting...",
		//	MB_ICONSTOP, 0);
		return (-1);
	}
/*
		// Connect to the Database by calling SQLBrowseConnect

	SQLCHAR		szConnStrIn[BUF_SIZE];
	char		*pszConnStrIn;
	SQLCHAR		szConnStrOut[BUF_SIZE];
	SQLSMALLINT	cbConnStrOut;
	pszConnStrIn = (char *)szConnStrIn;
	char		buffer[BUF_SIZE];
	
//  (void) sprintf(buffer, "DRIVER={SQL Server};");
	(void) sprintf(buffer, 
		"DSN=%s;SERVER=%s;UID=%s;PWD=%s;DB=%s;LANGUAGE=%s;",
		 "AVISDATA","DB2DASOO","kras", "moideti", "AVISDATA", "us_english");
	lstrcpy(pszConnStrIn, buffer);
	
	
	do {
		retcode = SQLBrowseConnect(phdbc, szConnStrIn, SQL_NTS,
					szConnStrOut, sizeof(szConnStrOut), &cbConnStrOut);
		//retcode = SQLBrowseConnect(phdbc, szConnStrIn, SQL_NTS,
		//			szConnStrOut, sizeof(szConnStrOut), &cbConnStrOut);

		if (retcode == SQL_NEED_DATA) {
			// Verify that Login and Password are correct
			
			if (strstr((char *)szConnStrOut, "LoginID=?") != NULL ||
				strstr((char *)szConnStrOut, "Password=?") != NULL) {
				AfxMessageBox("Incorrect Login ID and/or Password",
					MB_ICONSTOP, 0);
				return (-2);
			}

			// Compose Database List
			char DbList[1024];
			char *pDbList;
			ostrstream instore(DbList, sizeof(DbList) - 1);
			pDbList = strpbrk((char *)szConnStrOut,"*Database");
			pDbList = strchr(pDbList, '{');
			instore << ++pDbList << ends;
			pDbList = strchr(DbList, '}');
			if (pDbList == NULL) {
				AfxMessageBox("Error Connecting to the Database",
					MB_ICONSTOP, 0);
				return (-2);
			}
			memset(pDbList, '\0', 1);
			
		}
	} while (retcode == SQL_NEED_DATA);
*/
    return 0;
}


/*----------------------------------------------------------------------------*/
/* Procedure name:      DbClose                                               */
/* Description:         To close the database                                 */
/*                                                                            */
/* Input:               None                                                  */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              0 - success; -1, otherwise                            */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBRequests::DbClose()
{
	SQLRETURN	retcode;

	// Disconect database
	retcode = SQLDisconnect(phdbc);
	if (retcode != SQL_SUCCESS) {
		//AfxMessageBox("Error Disconnecting from the Database",
		//	MB_ICONSTOP, 0);
		return (-1);
	}

	// Free the connection handle
	retcode = SQLFreeHandle(SQL_HANDLE_DBC, phdbc);
	if (retcode != SQL_SUCCESS) {
		//AfxMessageBox("Error Freeing DB Connection Handler",
		//	MB_ICONSTOP, 0);
		return (-1);
	}

	// Free environment handle
	retcode = SQLFreeHandle(SQL_HANDLE_ENV, phenv);
	if (retcode != SQL_SUCCESS) {
		//AfxMessageBox("Error Freeing Environment Handler",
		//	MB_ICONSTOP, 0);
		return (-1);
	}

	return (0);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetCookiesInStateWithPriority                         */
/* Description:         To get cookies of highest priority in a particular st.*/                                                                            
/* Input:               DFArrivedList &arrivedList - list of arrived cookies  */
/*                      int neededNumber - number of cookies                  */
/*                      DFSample::Origin origin                               */                                                                                                  
/* Output:              None                                                  */
/* Return:              0 - success; -1, otherwise                            */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBRequests::GetCookiesInStateWithPriority(DFArrivedList &arrivedList, int neededNumber, DFSample::Origin origin)
{
	// Clean up the collection of arrived cookies
	DFSample* pSample;
	POSITION pos1, pos2;
	for (pos1 = arrivedList.GetHeadPosition(); (pos2 = pos1) != NULL;)
	{
		pSample = arrivedList.GetNext(pos1);
		if (pSample) {
			arrivedList.RemoveAt(pos2);
        	delete pSample;
        }
     } 
		// Allocate the statement handle
	int retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error allocating Statement Handle (1)",
		//	MB_ICONSTOP, 0);
		return(-1);
	}


    SQLCHAR Statement[1024];		 
	char *pStatement = (char *)Statement;
	char aStatement[2048];
	char msg[256];
	char state[256];
	SQLUINTEGER id;
	SQLINTEGER  idLength;
    
    // to get the priority from AnalysisStateInfo table
	std::ostrstream inStatement(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg(msg, sizeof(msg) - 1);
	CString strId;
	switch (origin)
	{
		case DFSample::IMPORTED:
		{
		strId = CString(((LPCSTR) (IDS_IMPORTED))); 
        }
		break;
    }   
    strcpy(state,(LPTSTR) (LPCTSTR) strId);
    
	// Compose the statement
	inStatement << "SELECT AnalysisStateID FROM AnalysisStateInfo WHERE Name = '";
	inStatement << state;
	inStatement << "'";
	inStatement	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
		if (retcode != SQL_SUCCESS)	{
		inMsg << "ERROR selecting Analysis State id (2)" ;
		inMsg << std::ends;
		//AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}

    SQLBindCol(pstmt, 1, SQL_C_ULONG, &id, 0,  &idLength);	

	while (TRUE) {
	    retcode = SQLFetch(pstmt);
		if (retcode == SQL_ERROR) {
			inMsg << "ERROR In Fetching the Analysis State id (3)" ;
			inMsg << std::ends;
			//AfxMessageBox(msg, MB_ICONSTOP, 0);
			SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
			return(-1);
		}
		if (retcode != SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO) {
	       break;
		}
    }
	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);

    // to get the cookies
	// Allocate the statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error allocating Statement Handle (4)",
		//	MB_ICONSTOP, 0);
		return(-1);
	}

	std::ostrstream inStatement1(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg1(msg, sizeof(msg) - 1);
    // Compose the  statement
	inStatement1 << "SELECT * FROM AnalysisRequest WHERE  ";
	inStatement1 << "AnalysisStateID =  ";
	inStatement1 << id;
	inStatement1 << " ORDER BY Priority DESC";
	inStatement1	<< std::ends;
	(void) strcpy(pStatement, aStatement);

	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
	if (retcode != SQL_SUCCESS)	{
		inMsg1 << "ERROR in executing select statement for AnalysisRequest table(5)  " ;
		inMsg1 << std::ends;
		//AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}

	#define NAME_LEN 35
	SQLUINTEGER sLocalCookie, sCheckSum;
	SQLINTEGER  cLocalCookie, cPriority, cCheckSum;
	short	sPriority;

   /* Bind columns 1, 2, and 3 */
   	SQLBindCol(pstmt, 1, SQL_C_ULONG, &sLocalCookie, 0, &cLocalCookie);
	SQLBindCol(pstmt, 2, SQL_C_CHAR, &sCheckSum, sizeof(sCheckSum), &cCheckSum);
	SQLBindCol(pstmt, 3, SQL_C_USHORT, &sPriority, 0, &cPriority);

	DFSample::Origin whereFrom;
	DFSample::SampleStatus status;

	while (TRUE) {
	    retcode = SQLFetch(pstmt);
		if (retcode == SQL_ERROR) {
			inMsg << "ERROR IN Fetching local imported cookies (6)" ;
			inMsg << std::ends;
			//AfxMessageBox(msg, MB_ICONSTOP, 0);
			SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
			return(-1);
		}
		if (retcode == SQL_SUCCESS) {
			if (arrivedList.GetCount() != neededNumber) {
				CString cookie;
				cookie.Format("%d", sLocalCookie);
				status = DFSample::WAIT;
				whereFrom = DFSample::IMPORTED;
				DFSample *pSample = new DFSample(cookie, "NEWSAMPLE", status); 
				pSample->SetOrigin(whereFrom);
				pSample->SetPriority(sPriority);
  				CTime t = CTime::GetCurrentTime();
   				pSample->SetArrivalTime(t);
				arrivedList.AddHead(pSample);
			}

		}
		else {
			if ( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		 	  break;
        }
   }

	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
	
	return (0);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      UpdateCookieRecord                                    */
/* Description:         To get cookies of highest priority in a particular st.*/                                                                            
/* Input:               int localCookie                                       */
/*                      DFManager::ReportStage stage                          */
/*                      DFSample::SampleStatus sampleStatus                   */ 
/* Output:              None                                                  */
/* Return:              0 - success; -1, otherwise                            */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBRequests::UpdateCookieRecord(int localCookie,DFManager::ReportStage stage, CString newState)
{

	// Allocate the statement handle
	int retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
//		AfxMessageBox("Error allocating Statement Handle (11)",
//			MB_ICONSTOP, 0);
		return(-1);
	}

	
    
    SQLCHAR Statement[1024];		 
	char *pStatement = (char *)Statement;
	char aStatement[2048];
	char msg[256];
	char state[256];
	SQLUINTEGER id;
	SQLINTEGER  idLength;

	std::ostrstream inStatement(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg(msg, sizeof(msg) - 1);
    strcpy(state, (LPTSTR) (LPCTSTR) newState); 
	// Compose the statement
	inStatement << "SELECT AnalysisStateID FROM AnalysisStateInfo WHERE Name = '";
	inStatement << state;
	inStatement << "'";
	inStatement	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
		if (retcode != SQL_SUCCESS)	{
		inMsg << "ERROR in executing select id statement (12) for AnalysisStateID table" ;
		inMsg << std::ends;
//		AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}

    SQLBindCol(pstmt, 1, SQL_C_ULONG, &id, 0,  &idLength);	

	while (TRUE) {
	    retcode = SQLFetch(pstmt);
		if (retcode == SQL_ERROR) {
			inMsg << "ERROR IN Fetch the id from Analysis State ID table (13)" ;
			inMsg << std::ends;
			//AfxMessageBox(msg, MB_ICONSTOP, 0);
			SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
			return(-1);
		}
		if (retcode != SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO) {
	       break;
		}
   }
	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);

	// Allocate the statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error allocating Statement Handle (14)",
		//	MB_ICONSTOP, 0);
		return(-1);
	}


	std::ostrstream inStatement1(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg1(msg, sizeof(msg) - 1);
    //strcpy(state, "imported"); 
	// Compose the statement
	inStatement1 << "UPDATE AnalysisRequest SET AnalysisStateID=";
	inStatement1 << id;
	inStatement1 << " WHERE LocalCookie = ";
	inStatement1 <<  localCookie;
	inStatement1	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
	if (retcode != SQL_SUCCESS)	{
		inMsg1 << "ERROR in executing the Update statement (15)" ;
		inMsg1 << std::ends;
		//AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}
	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
    return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      UpdateAnalysisStateInfo                               */
/* Description:         To update the analysis state info table with new sts  */                                                                       
/* Input:               CString strState                                      */
/* Output:              None                                                  */
/* Return:              0 - success; -1, otherwise                            */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBRequests::UpdateAnalysisStateInfoTable(CString strState)
{
    SQLCHAR Statement[1024];		 
	char *pStatement = (char *)Statement;
	char aStatement[2048];
	char msg[256];
	char state[256];
	SQLUINTEGER id; 
	SQLINTEGER  idLength, answer;
	std::ostrstream inStatement(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg(msg, sizeof(msg) - 1);
    strcpy(state, (LPTSTR) (LPCTSTR) strState);

	//check whether the state is in the table 
	// Allocate the statement handle
	int retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error allocating Statement Handle (19)",
		//	MB_ICONSTOP, 0);
		return(-1);
	}

	std::ostrstream inStatement1(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg1(msg, sizeof(msg) - 1);
    strcpy(state, (LPTSTR) (LPCTSTR)strState); 
	// Compose the statement
	inStatement1 << "SELECT COUNT(*) FROM AnalysisStateInfo WHERE NAME='" ;
	inStatement1 << state;
	inStatement1 << "'";
	//inStatement1 << ") SELECT 1 ELSE 0";
	inStatement1	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
	if (retcode != SQL_SUCCESS)	{
		inMsg1 << "ERROR in executing the Insert statement (20)" ;
		inMsg1 << std::ends;
		//AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}

    SQLBindCol(pstmt, 1, SQL_C_ULONG, &answer, 0,  &idLength);	

	while (TRUE) {
	    retcode = SQLFetch(pstmt);
		if (retcode == SQL_ERROR) {
			inMsg << "ERROR IN Fetch the id from Analysis State ID table (18)" ;
			inMsg << std::ends;
			//AfxMessageBox(msg, MB_ICONSTOP, 0);
			SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
			return(-1);
		}
		if (retcode != SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO) {
	       break;
		}
	}
	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);

   if (answer == 1)
	   return 0;

   //insert the state in the table
	// Allocate the statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error allocating Statement Handle (16)",
		//	MB_ICONSTOP, 0);
		return(-1);
	}
    
	// Compose the statement
	inStatement << "SELECT MAX(AnalysisStateID) FROM AnalysisStateInfo";
	inStatement	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
		if (retcode != SQL_SUCCESS)	{
		inMsg << "ERROR in executing select id statement for AnalysisStateID table (17)";
		inMsg << std::ends;
		//AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}

    SQLBindCol(pstmt, 1, SQL_C_ULONG, &id, 0,  &idLength);	

	while (TRUE) {
	    retcode = SQLFetch(pstmt);
		if (retcode == SQL_ERROR) {
			inMsg << "ERROR IN Fetch the id from Analysis State ID table (18)" ;
			inMsg << std::ends;
			//AfxMessageBox(msg, MB_ICONSTOP, 0);
			SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
			return(-1);
		}
		if (retcode != SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO) {
	       break;
		}
   }
	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
	id++;

	std::ostrstream inStatement2(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg2(msg, sizeof(msg) - 1);
    //strcpy(state, (LPTSTR) (LPCTSTR)strState); 
	// Compose the statement
	inStatement2 << "INSERT INTO AnalysisStateInfo VALUES( " ;
	inStatement2 << id;
	inStatement2 << ", '";
	inStatement2 <<  state;
	inStatement2 << "')";
	inStatement2	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
	if (retcode != SQL_SUCCESS)	{
		inMsg2 << "ERROR in executing the Insert statement (20)" ;
		inMsg2 << std::ends;
		//AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}
   return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetCookiePriority                                     */
/* Description:         To get cookie's priority.                             */                                                                            
/* Input:               int localCookie                                       */
/* Output:              None                                                  */
/* Return:              priority                                              */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBRequests::GetCookiePriority(int localCookie)
{

	// Allocate the statement handle
	int retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error allocating Statement Handle (21)",
		//	MB_ICONSTOP, 0);
		return(-1);
	}

	
    
    SQLCHAR Statement[1024];		 
	char *pStatement = (char *)Statement;
	char aStatement[2048];
	char msg[256];

	std::ostrstream inStatement(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg(msg, sizeof(msg) - 1);
	// Compose the statement
	inStatement << "SELECT Priority FROM AnalysisRequest WHERE LOCALCOOKIE = ";
	inStatement << localCookie;
	inStatement	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
		if (retcode != SQL_SUCCESS)	{
		inMsg << "ERROR in executing select id statement (22) for AnalysisStateID table" ;
		inMsg << std::ends;
		//AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}

	SQLINTEGER cPriority; 
	short	sPriority;

	SQLBindCol(pstmt, 1, SQL_C_USHORT, &sPriority, 0, &cPriority);

	while (TRUE) {
	    retcode = SQLFetch(pstmt);
		if (retcode == SQL_ERROR) {
			inMsg << "ERROR IN Fetch the id from Analysis State ID table (23)" ;
			inMsg << std::ends;
			//AfxMessageBox(msg, MB_ICONSTOP, 0);
			SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
			return(-1);
		}
		if (retcode != SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO) {
	       break;
		}
   }
	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);

    return sPriority;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetCookieAnalysisStateId                              */
/* Description:         To get analysis stateid from the database             */                                                                            
/* Input:               int localCookie                                       */
/* Output:              None                                                  */
/* Return:              priority                                              */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBRequests::GetCookieAnalysisStateId(int localCookie)
{

	// Allocate the statement handle
	int retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		//AfxMessageBox("Error allocating Statement Handle (21)",
		//	MB_ICONSTOP, 0);
		return(-1);
	}

    
    SQLCHAR Statement[1024];		 
	char *pStatement = (char *)Statement;
	char aStatement[2048];
	char msg[256];

	std::ostrstream inStatement(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg(msg, sizeof(msg) - 1);
	// Compose the statement
	inStatement << "SELECT AnalysisStateId FROM AnalysisRequest WHERE LOCALCOOKIE = ";
	inStatement << localCookie;
	inStatement	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
		if (retcode != SQL_SUCCESS)	{
		inMsg << "ERROR in executing select id statement (30) for AnalysisStateID table" ;
		inMsg << std::ends;
		//AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}

	SQLUINTEGER id;
	SQLINTEGER  idLength;

	SQLBindCol(pstmt, 1, SQL_C_ULONG, &id, 0, &idLength);

	while (TRUE) {
	    retcode = SQLFetch(pstmt);
		if (retcode == SQL_ERROR) {
			inMsg << "ERROR IN Fetch the id from Analysis State ID table (31)" ;
			inMsg << std::ends;
			//AfxMessageBox(msg, MB_ICONSTOP, 0);
			SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
			return(-1);
		}
		if (retcode != SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO) {
	       break;
		}
   }
	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);

    return id;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      GetStateId                                            */
/* Description:         To get analysis state id from the database             */                                                                            
/* Input:               int localCookie                                       */
/* Output:              None                                                  */
/* Return:              priority                                              */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int DFDBRequests::GetStateId(CString strState)
{
	int retcode = SQLAllocHandle(SQL_HANDLE_STMT, phdbc, &pstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
//		AfxMessageBox("Error allocating Statement Handle (11)",
//			MB_ICONSTOP, 0);
		return(-1);
	}

	
    
    SQLCHAR Statement[1024];		 
	char *pStatement = (char *)Statement;
	char aStatement[2048];
	char msg[256];
	char state[256];
	SQLUINTEGER id;
	SQLINTEGER  idLength;

	std::ostrstream inStatement(aStatement, sizeof(aStatement) - 1);
	std::ostrstream inMsg(msg, sizeof(msg) - 1);
    strcpy(state, (LPTSTR) (LPCTSTR) strState); 
	// Compose the statement
	inStatement << "SELECT AnalysisStateID FROM AnalysisStateInfo WHERE Name = '";
	inStatement << state;
	inStatement << "'";
	inStatement	<< std::ends;

	(void) strcpy(pStatement, aStatement);
	retcode = SQLExecDirect(pstmt, Statement, SQL_NTS);
		if (retcode != SQL_SUCCESS)	{
		inMsg << "ERROR in executing select id statement (40) for AnalysisStateID table" ;
		inMsg << std::ends;
//		AfxMessageBox(msg, MB_ICONSTOP, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
		return(-1);
	}

    SQLBindCol(pstmt, 1, SQL_C_ULONG, &id, 0,  &idLength);	

	while (TRUE) {
	    retcode = SQLFetch(pstmt);
		if (retcode == SQL_ERROR) {
			inMsg << "ERROR IN Fetch the id from Analysis State ID table (41)" ;
			inMsg << std::ends;
			//AfxMessageBox(msg, MB_ICONSTOP, 0);
			SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
			return(-1);
		}
		if (retcode != SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO) {
	       break;
		}
   }
	SQLFreeHandle(SQL_HANDLE_STMT, pstmt);
	return id;
}
