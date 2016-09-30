// SymARC.h : Functions exported from SYMARC.LIB
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(SYMARC_H)
#define SYMARC_H


void ARC_SetMaxExtractSize(long lMaxSize);
void ARC_AbortProcess(int bAbort);
void ARC_SetKeyFile(char *szKeyFile);

#define ARC_OK					0
#define ARC_ERROR			  -1
#define ARC_BAD_CRC				1
#define ARC_MAX_EXTRACT_SIZE	2
#define ARC_USER_CANCELED		3

/////////////////////////////////////////////////////////////////////////////
// Class CARCObject
//
// This class provides the interface into the ARC file engine.

class CARCObject
{
public:
	CARCObject();
	virtual ~CARCObject();

// Public functions...
	int		ARCOpen(char *pszFileName);
	int		ARCFindFirst(void);
	int		ARCFindNext(void);
	int		ARCExtract(const char *pszFileName);
	int		ARCClose(void);
	int		ARCGetName(char *pszName, unsigned int uiNameSize);
	long	ARCGetUncompressedSize(void);
	long	ARCGetCompressedSize(void);
	void	ARCGetDateTime(int *piYear, int *piMonth, int *piDay, int *piHour, int *piMinute, int *piSecond);
	int		ARCDelete(void);
	int		ARCReplace(const char *pszFileName);
	int		ARCAdd(const char *pszFileName, const char *pszName);
	int		ARCSetPassword(char *pszPassword);

private:
	void	*m_pARCFile;
	long	m_lOffset;
};

#endif	// SYMARC_H

