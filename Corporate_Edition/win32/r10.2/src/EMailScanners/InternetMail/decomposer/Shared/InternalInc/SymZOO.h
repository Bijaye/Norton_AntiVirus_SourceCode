// SymZOO.h : Functions exported from SYMZOO.LIB
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(SYMZOO_H)
#define SYMZOO_H

void ZOO_SetMaxExtractSize(DWORD dwMaxSize);
void ZOO_AbortProcess(bool bAbort);

/////////////////////////////////////////////////////////////////////////////
// Class CZOOObject
//
// This class provides the interface into the ZOO file engine.

class CZOOObject
{
public:
	CZOOObject();
	virtual ~CZOOObject();

// Public functions...
	int		ZOOOpen(char *pszFileName);
	int		ZOOFindFirst(void);
	int		ZOOFindNext(void);
	int		ZOOExtract(const char *pszFileName);
	int		ZOOClose(void);
	int		ZOOGetName(char *pszName, unsigned int uiNameSize);
	long	ZOOGetUncompressedSize(void);
	long	ZOOGetCompressedSize(void);
	void	ZOOGetDateTime(int *piYear, int *piMonth, int *piDay, int *piHour, int *piMinute, int *piSecond);
	int		ZOODelete(void);
	int		ZOOReplace(const char *pszFileName);
	int		ZOOAdd(const char *pszFileName, const char *pszName);
	int		ZOOSetPassword(char *pszPassword);

private:
	void	*m_pZOOFile;
	long	m_lOffset;
};

#endif	// SYMZOO_H
