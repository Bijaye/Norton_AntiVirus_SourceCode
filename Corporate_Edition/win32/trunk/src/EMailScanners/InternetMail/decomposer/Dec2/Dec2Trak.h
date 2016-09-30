// Dec2Trak.h : Decomposer 2 Result Tracker
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2TRAK_H)
#define DEC2TRAK_H

/////////////////////////////////////////////////////////////////////////////
// Class CDecResultTracker

class CDecResultTracker
{
public:
	CDecResultTracker(IDecContainerObjectEx *pObject, WORD *pwResult, char *pszNewDataFile);
	DECRESULT Apply(WORD wResult, const char *szNewDataFile);
	bool Deleted();
	~CDecResultTracker();

private:
	IDecContainerObjectEx *m_pObject;
	WORD	*m_pwResult;
	char	*m_pszNewDataFile;
	bool	m_bOrigCanModify;
	char	m_szOrigDataFile[MAX_PATH];
	bool	m_bReplacedDataFile;
};


#endif	// DEC2TRAK_H
