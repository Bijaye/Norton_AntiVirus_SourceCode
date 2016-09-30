// UISink.cpp: implementation of the CUISink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "navscan.h"
#include "ccgseinterface.h"
#include "hammertime.h"
#include "UISink.h"
#include "results.h"
#include "infection.h"
#include "CompressedInfection.h"
#include "BootInfection.h"
#include "MBRInfection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUISink::CUISink( CResults& results, int iHandleTime ) :
	m_Results( results )  
{
	m_iHandleTime = iHandleTime;
	m_bAbort = false;
    m_lScanCount = 0;
}

CUISink::~CUISink()
{

}

HRESULT CUISink::OnCUIPause()
{
	return S_OK;
}

HRESULT CUISink::OnCUIResume()
{
	return S_OK;	
}

HRESULT CUISink::OnCUIReady()
{
	return S_OK;	
}

HRESULT CUISink::OnCUIAbort()
{
	m_bAbort = true;
	return S_OK;	
}

HRESULT CUISink::OnCUIRepairInfection(/* [in] */ IScanInfection* pInfection)
{
	// To figure out if the infection is repairable or not, we will have to walk
	// the Results vector and see what pointer this one is equal to to get the
	// ResultsType and see if it is Repairable
	int i = 0;
	while((m_Results.m_vpResults[i] != pInfection) && (i < m_Results.m_iTotalInfections))
	{
		i++;
	}

	// We walked the entire vector and did not find the infection
	if(i== m_Results.m_iTotalInfections)
	{
		AfxMessageBox("Infection not found");
		return S_OK;
	}

	// Repairable, so Repair
	else if(m_Results.m_iResultsType[i] == CResults::REPAIRABLE || m_Results.m_iResultsType[i] == CResults::COMPREPAIRABLE || m_Results.m_iResultsType[i] == CResults::MBRREPAIRABLE || m_Results.m_iResultsType[i] == CResults::BRREPAIRABLE)
	{
		// First Sleep the amount of time to take for each action
		Sleep(1000*m_iHandleTime);
		CScanPtr<IScanFileInfection2> pFileInfection;
		CScanPtr<IScanCompressedFileInfection> pCompressedFileInfection;
		CScanPtr<IScanMBRInfection> pMBRInfection;
		CScanPtr<IScanBootInfection> pBootInfection;
	
		// NonCompressed Infection
		if (SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanFileInfection2,(void**) &pFileInfection)))
		{
			dynamic_cast< CInfection*>(pInfection)->m_iStatus = IScanInfection::REPAIRED;
			return S_OK;
		}

		// Compressed Infection
		else if(SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanCompressedFileInfection,(void**) &pCompressedFileInfection)))
		{
			dynamic_cast< CCompressedInfection*>(pInfection)->m_iStatus = IScanInfection::REPAIRED;
			return S_OK;
		}

		// MBR Infection
		else if(SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanMBRInfection,(void**) &pMBRInfection)))
		{
			dynamic_cast< CMBRInfection*>(pInfection)->m_iStatus = IScanInfection::REPAIRED;
			return S_OK;
		}

		// Boot Infection
		else if(SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanBootInfection,(void**) &pBootInfection)))
		{
			dynamic_cast< CBootInfection*>(pInfection)->m_iStatus = IScanInfection::REPAIRED;
			return S_OK;
		}

		return S_OK;
	}

	// Not repairable
	else
	{
		// First Sleep the amount of time to take for each action
		Sleep(1000*m_iHandleTime);

        // Don't set non-viral threats to repair failed
		pInfection->SetUserData(InfectionStatus_Repair_Failed);
		return S_OK;
	}
}

HRESULT CUISink::OnCUIQuarantineInfection(/* [in] */ IScanInfection* pInfection)
{

	// To figure out if the infection is quarantinable or not, we will have to walk
	// the Results vector and see what pointer this one if equal to to get the
	// ResultsType and see if it is quarantinable
	int i = 0;
	while((m_Results.m_vpResults[i] != pInfection) && (i < m_Results.m_iTotalInfections))
	{
		i++;
	}

	// We walked the entire vector and did not find the infection
	if(i== m_Results.m_iTotalInfections)
	{
		AfxMessageBox("Infection not found");
		return S_OK;
	}

	// The infection can be Quarantined, so Quarantine it
	else if(m_Results.m_iResultsType[i] == CResults::REPAIRABLE || m_Results.m_iResultsType[i] == CResults::QUARANTINABLE ||
			m_Results.m_iResultsType[i] == CResults::COMPREPAIRABLE || m_Results.m_iResultsType[i] == CResults::COMPQUARANTINABLE)
	{
		// First Sleep the amount of time to take for each action
		Sleep(1000*m_iHandleTime);
		CScanPtr<IScanFileInfection2> pFileInfection;
		CScanPtr<IScanCompressedFileInfection> pCompressedFileInfection;
	
		// NonCompressed Infection
		if (SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanFileInfection2,(void**) &pFileInfection)))
		{
			dynamic_cast< CInfection*>(pInfection)->m_iStatus = IScanInfection::DELETED;
			pInfection->SetUserData(InfectionStatus_Quarantined);
			return S_OK;
		}

		// Compressed Infection
		else if(SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanCompressedFileInfection,(void**) &pCompressedFileInfection)))
		{
			dynamic_cast< CCompressedInfection*>(pInfection)->m_iStatus = IScanInfection::DELETED;
			pInfection->SetUserData(InfectionStatus_Quarantined);
			return S_OK;
		}

		return S_OK;
	}

	// Can Not Quarantine
	else
	{
		// First Sleep the amount of time to take for each action
		Sleep(1000*m_iHandleTime);
		pInfection->SetUserData(InfectionStatus_Quarantine_Failed);
		return S_OK;
	}
}

HRESULT CUISink::OnCUIDeleteInfection(/* [in] */ IScanInfection* pInfection)
{
	// To figure out if the infection is deletable or not, we will have to walk
	// the Results vector and see what pointer this one if equal to to get the
	// ResultsType and see if it is deletable
	int i = 0;
	while((m_Results.m_vpResults[i] != pInfection) && (i < m_Results.m_iTotalInfections))
	{
		i++;
	}

	// We walked the entire vector and did not find the infection
	if(i== m_Results.m_iTotalInfections)
	{
		AfxMessageBox("Infection not found");
		return S_OK;
	}

	// The infection can be Deleted, so Delete it
	else if(m_Results.m_iResultsType[i] == CResults::REPAIRABLE || m_Results.m_iResultsType[i] == CResults::QUARANTINABLE || 
		m_Results.m_iResultsType[i] == CResults::DELETEABLE || m_Results.m_iResultsType[i] == CResults::COMPREPAIRABLE || 
		m_Results.m_iResultsType[i] == CResults::COMPQUARANTINABLE || m_Results.m_iResultsType[i] == CResults::COMPDELETEABLE)
	{

		// First Sleep the amount of time to take for each action
		Sleep(1000*m_iHandleTime);
		CScanPtr<IScanFileInfection2> pFileInfection;
		CScanPtr<IScanCompressedFileInfection> pCompressedFileInfection;
	
		// NonCompressed Infection
		if (SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanFileInfection2,(void**) &pFileInfection)))
		{
			dynamic_cast< CInfection*>(pInfection)->m_iStatus = IScanInfection::DELETED;
			pInfection->SetUserData(0);
			return S_OK;
		}

		// Compressed Infection
		else if(SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanCompressedFileInfection,(void**) &pCompressedFileInfection)))
		{
			dynamic_cast< CCompressedInfection*>(pInfection)->m_iStatus = IScanInfection::DELETED;
			pInfection->SetUserData(0);
			return S_OK;
		}
		return S_OK;
	}

	// Can Not Delete
	else
	{
		// First Sleep the amount of time to take for each action
		Sleep(1000*m_iHandleTime);
		pInfection->SetUserData(InfectionStatus_Delete_Failed);
		return S_OK;
	}
}

HRESULT CUISink::OnCUIExcludeThreat(/* [in] */ IScanInfection* pInfection)
{
    // Make sure the infection exists
	int i = 0;
	while((m_Results.m_vpResults[i] != pInfection) && (i < m_Results.m_iTotalInfections))
	{
		i++;
	}

	// We walked the entire vector and did not find the infection
	if(i== m_Results.m_iTotalInfections)
	{
		AfxMessageBox("Infection not found");
		return S_OK;
	}

	// Exclude the infection 

	// First Sleep the amount of time to take for each action
	Sleep(1000*m_iHandleTime);

    pInfection->SetUserData(InfectionStatus_Excluded);
	return S_OK;
}

HRESULT CUISink::OnCUIGetInfectionName(/* [in] */ IScanInfection* pInfection,/* [out] */ string& strName)
{
	strName = pInfection->GetVirusName();
	return S_OK;
}



HRESULT CUISink::OnCUIGetInfectionDescription(/* [in] */ IScanInfection* pInfection,/* [out] */ string& strDescription)
{

	strDescription = "Bogus Description";
	return S_OK;
}

HRESULT CUISink::OnCUIGetThreatCategoryEnabled(/* [in] */ long lCategory,/* [out] */ BOOL* pbEnabled)
{
    *pbEnabled = TRUE;
    return S_OK;
}

HRESULT CUISink::OnCUIGetCurrentFolder(LPSTR szFolder, long nLength)
{
    strncpy(szFolder, "C:\\MR.T\\Rocks", nLength-1);
    return S_OK;
}

HRESULT CUISink::OnCUIDeletedFileCount(long& nDeletedFileCount)
{
    nDeletedFileCount = 0;
    return S_OK;
}

HRESULT CUISink::OnCUIRepairedFileCount(long& nRepairedFileCount)
{
    return S_OK;
}

HRESULT CUISink::OnCUIInfectedFileCount(long& nInfectedFileCount)
{
    nInfectedFileCount = 0;
    return S_OK;
}

HRESULT CUISink::OnCUIScanFileCount(long& nScanFileCount)
{
    nScanFileCount = ++m_lScanCount;
    return S_OK;
}

HRESULT CUISink::OnCUIReScanManualItems()
{
    return S_OK;
}
