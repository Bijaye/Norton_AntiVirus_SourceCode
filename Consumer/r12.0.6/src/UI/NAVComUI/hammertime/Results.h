// Results.h: interface for the CResults class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTS_H__B0A86F93_0A53_42BB_9353_85DBBFEA3F81__INCLUDED_)
#define AFX_RESULTS_H__B0A86F93_0A53_42BB_9353_85DBBFEA3F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInfection;

#include "navscan.h"

class CResults : public IScanResults, public ISymBaseImpl< CSymThreadSafeRefCount >  
{
public:
	CResults( int iRepCount, int iQuarCount, int iDelCount, int iNonDelCount, int iCompRepCount, int iCompQuarCount, int iCompDelCount, int iCompNonDeleteCount, int iCompressionDepth, int iMBRRep, int iMBRUnRep, int iBRRep,  int iBRUnRep, int iThreatCount, int iNoDelThreat);
	virtual ~CResults();

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY( IID_ScanResults, IScanResults )
	SYM_INTERFACE_MAP_END()

	void Initialize();

	//
	// IScanResults methods
	//
	virtual int GetTotalFilesScanned();
	virtual int GetTotalBootRecordsScanned();
	virtual int GetTotalMBRsScanned();
	virtual int GetTotalInfectionCount();
    virtual int GetRemainingInfectionCount();
    virtual int GetRepairedInfectionCount();
    virtual int GetDeletedInfectionCount();
    virtual SCANSTATUS GetInfection( int iInfectionNum, IScanInfection** ppInfection );
    virtual int GetErrorCount();
    virtual SCANSTATUS GetError( int iError, IScanError** ppError );
    virtual SYMRESULT Merge( IScanResults* pOther );
	enum {REPAIRABLE, QUARANTINABLE, DELETEABLE, NONDELETEABLE, COMPREPAIRABLE, COMPQUARANTINABLE, COMPDELETEABLE, COMPNONDELETEABLE, MBRREPAIRABLE, MBRNONREPAIRABLE, BRREPAIRABLE, BRNONREPAIRABLE};


public:
	// Number of fake infections we have.
	int	m_iRepCount;
	int	m_iQuarCount;
	int	m_iDelCount;
	int	m_iNonDelCount;
	int	m_iCompRepCount;
	int	m_iCompQuarCount;
	int	m_iCompDeleteCount;
	int	m_iCompNonDeleteCount;
	int	m_iCompressionDepth;
	int m_iTotalInfections;
	int m_iMBRRep;
	int m_iMBRUnRep;
	int m_iBRRep;
	int m_iBRUnRep;
    int m_iThreats;
    int m_iNonDelThreats;
	// vector of fake infections.
	vector<IScanInfection*>	m_vpResults;

	//Parallel array to Results vector to store infection Type
	int* m_iResultsType;

};

#endif // !defined(AFX_RESULTS_H__B0A86F93_0A53_42BB_9353_85DBBFEA3F81__INCLUDED_)
