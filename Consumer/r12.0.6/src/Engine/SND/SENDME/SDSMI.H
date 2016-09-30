// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SENDME/VCS/sdsmi.h_v   1.0   18 Mar 1998 12:32:34   CNACHEN  $
//
// Description:
//  Header file for Scan and Deliver Send Me class declarations
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/SENDME/VCS/sdsmi.h_v  $
// 
//    Rev 1.0   18 Mar 1998 12:32:34   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _SDSMI_H_

#define _SDSMI_H_

#ifdef _M_ALPHA
#define RESOURCE_DLL    "SDSND32A.LOC"
#else
#define RESOURCE_DLL    "SDSND32I.LOC"
#endif

#include <atlstr.h>

class ViralSubmissionFilterImp: public ViralSubmissionFilter
{
public:

	int StartUp(LPCTSTR lpszDataFileDirectory,
				LPCTSTR	lpszAVDataFileDirectory, //optional (=NULL)
				int nFilterType);

	int AllowSubmission(LPCTSTR lpszFileName,
						LPN30 lpstN30,
						BOOL bRepairable,
						LPTSTR lpszReason);

	void ShutDown(void);

	void Release(void);

	int AllowNonRepairable(LPN30 lpstN30,
							BOOL bRepairable,
							LPTSTR lpszReason);

	int AllowBloodhoundAndNonRepairable(LPN30 lpstN30,
										BOOL bRepairable,
										LPTSTR lpszReason);

	BOOL LoadString(int nStringNum,
					LPTSTR lpszString);

private:

	int m_nFilterType;

	CString m_csProductName;
};

#endif // #ifndef _SDSMI_H_
