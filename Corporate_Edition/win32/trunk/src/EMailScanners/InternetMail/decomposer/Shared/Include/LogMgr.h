// LogMgr.h : Event logging facility
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(LOGMGR_H)
#define LOGMGR_H

#if defined (SYM_NLM)
#include <nwsemaph.h>
#include <nwthread.h>
#endif

#define MAX_ITEM_TYPES	100
#define FIELD_LENGTH	64


#if defined(_WINDOWS)
#pragma pack(1)
#elif defined(OS400)
#pragma pack(1)
#elif defined(IBMCXX)
#pragma option align=packed
#endif

// This is the on-disk structure of the log file.
typedef struct tagLogFile
{
	unsigned char	TotalItems[FIELD_LENGTH];
	unsigned char	TotalItemSize[FIELD_LENGTH];
	unsigned char	TotalTime[FIELD_LENGTH];
	unsigned char	TotalExtractErrors[FIELD_LENGTH];
	unsigned char	TotalEngineErrors[FIELD_LENGTH];
	unsigned char	Items[MAX_ITEM_TYPES * FIELD_LENGTH];
	unsigned char	ItemSize[MAX_ITEM_TYPES * FIELD_LENGTH];
	unsigned char	ItemTime[MAX_ITEM_TYPES * FIELD_LENGTH];
	unsigned char	ItemExtractError[MAX_ITEM_TYPES * FIELD_LENGTH];
	unsigned char	ItemEngineError[MAX_ITEM_TYPES * FIELD_LENGTH];
	long			Count;
#if defined(__GNUC__)
} __attribute__ ((packed)) LOGFILE;
#else
} LOGFILE;
#endif

#if defined(_WINDOWS)
#pragma pack()
#elif defined(OS400)
#pragma pack()
#elif defined(IBMCXX)
#pragma option align=reset
#endif


/////////////////////////////////////////////////////////////////////////////
// ITEM_TYPE_xxx Codes
// NOTE: These definitions MUST match the DEC_TYPE_xxx definitions
//       in decdefs.h.

enum ItemTypes
{
	ITEM_TYPE_UNKNOWN = 0,		// 0
	ITEM_TYPE_AMG,				// 1
	ITEM_TYPE_ARC,				// 2
	ITEM_TYPE_ARJ,				// 3
	ITEM_TYPE_CAB,				// 4
	ITEM_TYPE_EXE,				// 5
	ITEM_TYPE_GHO,				// 6
	ITEM_TYPE_GZIP,				// 7
	ITEM_TYPE_HQX,				// 8
	ITEM_TYPE_HTTP,				// 9
	ITEM_TYPE_ID,				// 10
	ITEM_TYPE_JAR,				// 11
	ITEM_TYPE_LHA,				// 12
	ITEM_TYPE_LN,				// 13
	ITEM_TYPE_LZ,				// 14
	ITEM_TYPE_MIME,				// 15
	ITEM_TYPE_MSG,				// 16
	ITEM_TYPE_OLE1,				// 17
	ITEM_TYPE_OLESS,			// 18
	ITEM_TYPE_RAR,				// 19
	ITEM_TYPE_RTF,				// 20
	ITEM_TYPE_TAR,				// 21
	ITEM_TYPE_TNEF,				// 22
	ITEM_TYPE_UUE,				// 23
	ITEM_TYPE_ZIP,				// 24
	ITEM_TYPE_ZOO,				// 25
	ITEM_TYPE_BAG,				// 26
	ITEM_TYPE_HA,				// 27
	ITEM_TYPE_HYP,				// 28
	ITEM_TYPE_SQZ,				// 29
	ITEM_TYPE_ACE,				// 30
	ITEM_TYPE_PDF,				// 31
	ITEM_TYPE_DAS,				// 32
	ITEM_TYPE_TEXT,				 // 33
	ITEM_TYPE_ISO9660,			// 34
	ITEM_TYPE_MB3,				// 35
	ITEM_TYPE_AS,				// 36
	ITEM_TYPE_AD,				// 37
	ITEM_TYPE_SIT,				// 38
	ITEM_TYPE_DBX,				// 39
	ITEM_TYPE_MBOX				// 40
};

#define LAST_ITEM_TYPE ITEM_TYPE_OLE1


/////////////////////////////////////////////////////////////////////////////
//
//  Platform independent critical section class
//
/////////////////////////////////////////////////////////////////////////////

class CCriticalSect
{
public:
	CCriticalSect();
	~CCriticalSect();
	void In();
	void Out();

private:
#if defined(_WINDOWS)
	CRITICAL_SECTION m_LogMgrCriticalSection;
#elif defined(SYM_NLM)
	long m_LogMgrCriticalSection;
#elif defined(UNIX)
	pthread_mutex_t m_LogMgrCriticalSection;
#endif
};


class CLogMgr
{
public:
	CLogMgr();
	~CLogMgr();

	int		SetLogFile(char *pszLogFile);
	int		SetFlushFile(char *pszFlushFile, long lFlushInterval);
	int		LogFlush(void);
	int		LogEvent(int iNew, int iType, long lSize, long lTime, int iExtractError, int iEngineError);

private:
	int		SetField(unsigned char *pField, long lValue);
	int		AddToField(unsigned char *pField1, unsigned char *pField2);
	void	GetFieldString(unsigned char *pField, char *pszField);
	void	WriteLogData(FILE *fp);

	FILE			*m_fp;
	LOGFILE			*m_plog;
	char			m_szFlushFile[MAX_PATH];
	long			m_lFlushInterval;
	CCriticalSect	m_critical;
};

#endif // LOGMGR_H

