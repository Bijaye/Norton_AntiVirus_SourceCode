#ifndef		__PERFCNTR_H_
#define		__PERFCNTR_H_

class CPerfCounter
{
public:
	CPerfCounter (PTSTR const pszName, DWORD type, PBYTE const pData, DWORD cbData);
	~CPerfCounter (void);
	
	PTSTR GetName (void) { return m_pszName; }
	DWORD GetType (void) { return m_type; }
	BOOL GetData (PBYTE pBuffer, DWORD cBuffer, DWORD *pType);
	BOOL  Format (PTSTR pszBuffer, DWORD nSize, BOOL fHex = FALSE);

protected:
	PTSTR	m_pszName;
	DWORD	m_type;
	PBYTE	m_pData;
	DWORD	m_cbData;
};

typedef CPerfCounter * PCPerfCounter;

#endif