// Results.cpp: implementation of the CResults class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "navscan.h"
#include "ccgseinterface.h"
#include "hammertime.h"
#include "commonuiinterface.h"
#include "Results.h"
#include "infection.h"
#include "CompressedInfection.h"
#include "MBRInfection.h"
#include "BootInfection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResults::CResults( int iRepCount, int iQuarCount, int iDelCount, int iNonDelCount, 
				   int iCompRepCount, int iCompQuarCount, int iCompDelCount, int iCompNonDeleteCount, 
				   int iCompressionDepth, 	int iMBRRep, int iMBRUnRep, int iBRRep,  int iBRUnRep,
                   int iThreats, int iNoDelThreats)	
{
	m_iRepCount= iRepCount;
	m_iQuarCount = iQuarCount;
	m_iDelCount = iDelCount;
	m_iNonDelCount = iNonDelCount;
	m_iCompRepCount = iCompRepCount;
	m_iCompQuarCount = iCompQuarCount;
	m_iCompDeleteCount = iCompDelCount;
	m_iCompNonDeleteCount = iCompNonDeleteCount;
	m_iCompressionDepth = iCompressionDepth;
	m_iMBRRep = iMBRRep;
	m_iMBRUnRep = iMBRUnRep;
	m_iBRRep = iBRRep;
	m_iBRUnRep = iBRUnRep;
    m_iThreats = iThreats;
    m_iNonDelThreats = iNoDelThreats;


	m_iTotalInfections = m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep + m_iMBRUnRep + m_iBRRep + m_iBRUnRep + m_iThreats + m_iNonDelThreats;
	m_iResultsType = new int[m_iTotalInfections];
	if(m_iResultsType == NULL)
		throw runtime_error("MemoryAllocation Failure");

}

CResults::~CResults()
{
	// Clean up allocation.
	for(int i = 0; i < m_iTotalInfections; i++ )
	{
		m_vpResults[i]->Release();		
	}

	delete[] m_iResultsType;
}

void CResults::Initialize()
{
	// Allocate all of the items.
	m_vpResults.reserve( m_iTotalInfections );

	for(int i = 0; i < m_iTotalInfections; i++ )
	{
		// Non-Compressed infections
		if(i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount)
		{
			m_vpResults[i] = new CInfection(i, NULL, 1, true, false);
			m_vpResults[i]->AddRef();
		}

		// Compressed infections
		else if(i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount)
		{
			m_vpResults[i] = new CCompressedInfection(i, m_iCompressionDepth);
			m_vpResults[i]->AddRef();
		}

		// MBR infections
		else if(i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep + m_iMBRUnRep)
		{
			m_vpResults[i] = new CMBRInfection('1', 2);
			m_vpResults[i]->AddRef();
		}

		// BR infections
		else if( i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep + m_iMBRUnRep + m_iBRRep + m_iBRUnRep )
		{
			m_vpResults[i] = new CBootInfection('C');
			m_vpResults[i]->AddRef();
		}
        
        // Non-Viral Threats - deleteable
        else if( i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep + m_iMBRUnRep + m_iBRRep + m_iBRUnRep + m_iThreats)
        {
            unsigned long lCats[1] = {SCAN_THREAT_TYPE_SPYWARE};
            m_vpResults[i] = new CInfection(i, lCats, 1, true);
			m_vpResults[i]->AddRef();
        }

        // Non-Viral Threats - not deleteable
        else
        {
            unsigned long lCats[1] = {SCAN_THREAT_TYPE_SPYWARE};
            m_vpResults[i] = new CInfection(i, lCats, 1, false);
			m_vpResults[i]->AddRef();
        }

		// Logic to determine what the infection type is
		if(i < m_iRepCount)
			m_iResultsType[i] = REPAIRABLE;
		else if(i < (m_iRepCount + m_iQuarCount))
			m_iResultsType[i] = QUARANTINABLE;
		else if(i < (m_iRepCount + m_iQuarCount + m_iDelCount))
			m_iResultsType[i] = DELETEABLE;
		else if(i < (m_iRepCount + m_iQuarCount + m_iDelCount+ m_iNonDelCount))
			m_iResultsType[i] = NONDELETEABLE;
		else if(i < (m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount+ m_iCompRepCount))
			m_iResultsType[i] = COMPREPAIRABLE;
		else if(i < (m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount+ m_iCompQuarCount))
			m_iResultsType[i] = COMPQUARANTINABLE;
		else if(i < (m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount+ m_iCompDeleteCount))
			m_iResultsType[i] = COMPDELETEABLE;
		else if(i < (m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount+ m_iCompDeleteCount + m_iCompNonDeleteCount))
			m_iResultsType[i] = COMPNONDELETEABLE;
		else if(i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep)
			m_iResultsType[i] = MBRREPAIRABLE;
		else if(i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep + m_iMBRUnRep)
			m_iResultsType[i] = MBRNONREPAIRABLE;
		else if(i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep + m_iMBRUnRep + m_iBRRep)
			m_iResultsType[i] = BRREPAIRABLE;
		else if(i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep + m_iMBRUnRep + m_iBRRep + m_iBRUnRep)
			m_iResultsType[i] = BRNONREPAIRABLE;
        else if(i < m_iRepCount + m_iQuarCount + m_iDelCount + m_iNonDelCount + m_iCompRepCount + m_iCompQuarCount + m_iCompDeleteCount + m_iCompNonDeleteCount + m_iMBRRep + m_iMBRUnRep + m_iBRRep + m_iBRUnRep + m_iThreats)
            m_iResultsType[i] = DELETEABLE;
        else
            m_iResultsType[i] = NONDELETEABLE;
	}
}

int CResults::GetTotalFilesScanned()
{
	// Return total numer of infections
	return m_iTotalInfections;
}

int CResults::GetTotalBootRecordsScanned()                                           
{
	return 0;
}

int CResults::GetTotalMBRsScanned()                                                  
{
	return 0;
}

int CResults::GetTotalInfectionCount()                                               
{
	return m_iTotalInfections;
}

int CResults::GetRemainingInfectionCount()                                           
{
	int iInfectionCount =0 ;
	// Walk the vector and determine how many infections are left
	for(int i =0; i <m_iTotalInfections; i++)
	{
		CScanPtr<IScanFileInfection2> pFileInfection;
		CScanPtr<IScanCompressedFileInfection> pCompressedFileInfection;
		CScanPtr<IScanMBRInfection> pMBRInfection;
		CScanPtr<IScanBootInfection> pBootInfection;
		
		// NonCompressed Infection
		if (SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanFileInfection2,(void**) &pFileInfection)))
		{
			if (((CInfection*)m_vpResults[i])->m_iStatus == IScanInfection::INFECTED)
				iInfectionCount ++;
		}

		// Compressed Infection
		else if(SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanCompressedFileInfection,(void**) &pCompressedFileInfection)))
		{
			if (((CCompressedInfection*)m_vpResults[i])->m_iStatus == IScanInfection::INFECTED)
				iInfectionCount ++;
		}

		// MBR Infection
		else if(SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanMBRInfection,(void**) &pMBRInfection)))
		{
			if (((CMBRInfection*)m_vpResults[i])->m_iStatus == IScanInfection::INFECTED)
				iInfectionCount ++;
		}

		// BR Infection
		else if(SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanBootInfection,(void**) &pBootInfection)))
		{
			if (((CBootInfection*)m_vpResults[i])->m_iStatus == IScanInfection::INFECTED)
				iInfectionCount ++;
		}

	}
	return iInfectionCount;
}

int CResults::GetRepairedInfectionCount()                                            
{
	int iRepCount =0 ;
	// Walk the vector and determine how many infections are repaired
	for(int i =0; i <m_iTotalInfections; i++)
	{
		CScanPtr<IScanFileInfection2> pFileInfection;
		CScanPtr<IScanCompressedFileInfection> pCompressedFileInfection;
		CScanPtr<IScanMBRInfection> pMBRInfection;
		CScanPtr<IScanBootInfection> pBootInfection;
		
		// NonCompressed Infection
		if ( SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanFileInfection2,(void**) &pFileInfection)))
		{
			if (((CInfection*)m_vpResults[i])->m_iStatus == IScanInfection::REPAIRED)
				iRepCount ++;
		}

		// Compressed Infection
		else if(SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanCompressedFileInfection,(void**) &pCompressedFileInfection)))
        {
			if (((CCompressedInfection*)m_vpResults[i])->m_iStatus == IScanInfection::REPAIRED)
				iRepCount ++;
		}

		// MBR Infection
		else if(SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanMBRInfection,(void**) &pMBRInfection)))
		{
			if (((CMBRInfection*)m_vpResults[i])->m_iStatus == IScanInfection::REPAIRED)
				iRepCount ++;
		}

		// BR Infection
		else if(SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanBootInfection,(void**) &pBootInfection)))
		{
			if (((CBootInfection*)m_vpResults[i])->m_iStatus == IScanInfection::REPAIRED)
				iRepCount ++;
		}
	}
	return iRepCount;
}

int CResults::GetDeletedInfectionCount()                                             
{
	int iDelCount =0 ;
	// Walk the vector and determine how many infections are deleted
	for(int i =0; i <m_iTotalInfections; i++)
	{
		// NonCompressed Infection
		CScanPtr<IScanFileInfection2> pFileInfection;
		CScanPtr<IScanCompressedFileInfection> pCompressedFileInfection;
		if (SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanFileInfection2,(void**) &pFileInfection)))
		{
			if (((CInfection*)m_vpResults[i])->m_iStatus == IScanInfection::DELETED)
				iDelCount ++;
		}

		// Compressed Infection
		else if(SYM_SUCCEEDED(m_vpResults[i]->QueryInterface(IID_ScanCompressedFileInfection,(void**) &pCompressedFileInfection)))
		{
			if (((CCompressedInfection*)m_vpResults[i])->m_iStatus == IScanInfection::DELETED)
				iDelCount ++;
		}
	}
	return iDelCount;
}

int CResults::GetErrorCount()                                                        
{
	return 0;
}

SCANSTATUS CResults::GetInfection( int iInfectionNum, IScanInfection** ppInfection )
{
	*ppInfection = m_vpResults[iInfectionNum];
	(*ppInfection)->AddRef();
	return SCAN_OK;
}

SCANSTATUS CResults::GetError( int iError, IScanError** ppError )                   
{
	return SCAN_OK;
}

SYMRESULT CResults::Merge( IScanResults* pOther )
{
    return SYM_OK;
}