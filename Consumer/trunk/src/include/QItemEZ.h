////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccEraserInterface.h"
#include "IQuaran.h"
#include "qsfields.h"
#include "ThreatCatInfo.h"

class QItemEZ
{
public:
    // Function to get the Original ANSI file name taking into account that it
	// might be an SLFN
    static BOOL GetFullANSIFilePath(IQuarantineItem* pQItem, CString& cszOutPath, DWORD dwRemediationIndex = 0);
    static BOOL GetANSIFileNameOnly(IQuarantineItem* pQItem, CString& cszOutPath, DWORD dwRemediationIndex = 0);
    static BOOL GetANSIFilePathOnly(IQuarantineItem* pQItem, CString& cszOutPath, DWORD dwRemediationIndex = 0);

    static BOOL GetRemediationDescription(IQuarantineItem* pQItem, CString& cszRemediationDescription, DWORD dwRemediationIndex);
    static BOOL GetRemediationType(IQuarantineItem* pQItem, DWORD &dwRemediationType, DWORD dwRemediationIndex);
    static BOOL GetRemediationOperation(IQuarantineItem* pQItem, DWORD &dwRemediationOperation, DWORD dwRemediationIndex);

    static BOOL GetThreatName(IQuarantineItem* pQItem, CString& cszThreatName);
	static BOOL GetVirusName(IQuarantineItem* pQItem, CString& cszVirusName);
	static BOOL GetAnomalyName(IQuarantineItem* pQItem, CString& cszAnomalyName);

	static BOOL GetAnomalyID(IQuarantineItem* pQItem, CString& cszAnomalyID);

	static BOOL GetThreatCategoryList(IQuarantineItem* pQItem, CString& cszThreatCategoryList, BOOL bConvertToText = TRUE);
	static BOOL GetScanInfectionCategories(IQuarantineItem* pQItem, CString& cszScanInfectionCategories);
	static BOOL GetAnomalyCategories(IQuarantineItem* pQItem, CString& cszScanInfectionCategories);

    static BOOL GetOriginalMachineName(IQuarantineItem* pQItem, CString &cszOriginalMachineName);

	static BOOL InitialDummyFile(IQuarantineItem* pQItem);
	static BOOL ShouldShowRemediation(IQuarantineItem* pQItem, DWORD dwRemediationIndex);
    
    static DWORD IsViral(IQuarantineItem* pQItem);

    static INT  GetSystemIconIndex(IQuarantineItem* pQItem, INT iIconSize, DWORD dwIndex = 0);
    static INT  GetSystemIconIndex(LPCTSTR szFileName, INT iIconSize);

    static HRESULT GetString(IQuarantineItem *pItem, LPCSTR szProperty, CString &cszString);

    enum{ THREAT_NON_VIRAL = 0, THREAT_VIRAL = 1, THREAT_UNKNOWN = 2 };

};

inline BOOL QItemEZ::GetFullANSIFilePath(IQuarantineItem* pQItem, CString& sOutPath, DWORD dwRemediationIndex)
{
    sOutPath.Empty();
    HRESULT hr = S_OK;
    CString cszProperty;

    // index of 0 denotes "Main File", 1-n denotes Remediation
    if(0 == dwRemediationIndex)
        cszProperty = QSERVER_ITEM_INFO_FILENAME_ANSI;
    else
        cszProperty.Format(QSERVER_ITEM_REMEDIATION_ORIGINAL_FILENAME, dwRemediationIndex);

    hr = QItemEZ::GetString( pQItem, cszProperty, sOutPath);
    if( FAILED(hr) )
        return FALSE;

    return TRUE;
}

inline BOOL QItemEZ::GetANSIFileNameOnly(IQuarantineItem* pQItem, CString& sOutPath, DWORD dwRemediationIndex)
{
    BOOL bRet = FALSE;
    // First get the full path
    if( QItemEZ::GetFullANSIFilePath(pQItem, sOutPath, dwRemediationIndex) )
    {
        // Now get just the file name portion
        if( !sOutPath.IsEmpty() )
        {
            int nLastSlash = sOutPath.ReverseFind(_T('\\'));
            if( nLastSlash != -1 && sOutPath[nLastSlash] == _T('\\') )
            {
                sOutPath = sOutPath.Mid(nLastSlash+1);
            }
            bRet = TRUE;
        }
    }

    return bRet;
}

inline BOOL QItemEZ::GetANSIFilePathOnly(IQuarantineItem* pQItem, CString& sOutPath, DWORD dwRemediationIndex)
{
    BOOL bRet = FALSE;
    // First get the full path
    if( QItemEZ::GetFullANSIFilePath(pQItem, sOutPath, dwRemediationIndex) )
    {
        // Now remove the file name portion
        if( !sOutPath.IsEmpty() )
        {
            int nLastSlash = sOutPath.ReverseFind(_T('\\'));
            if( nLastSlash != -1 && sOutPath[nLastSlash] == _T('\\') )
            {
                sOutPath = sOutPath.Left(nLastSlash);
            }
            else
            {
                sOutPath.Empty();
            }
            bRet = TRUE;
        }
    }

    return bRet;
}

inline BOOL QItemEZ::GetRemediationDescription(IQuarantineItem* pQItem, CString& cszRemediationDescription, DWORD dwRemediationIndex)
{
    cszRemediationDescription.Empty();
    HRESULT hr = S_OK;
    CString cszProperty;

    cszProperty.Format(QSERVER_ITEM_REMEDIATION_DESCRIPTION, dwRemediationIndex);

    hr = QItemEZ::GetString( pQItem, cszProperty, cszRemediationDescription);
    if( FAILED(hr) )
        return FALSE;

    return TRUE;
}

inline BOOL QItemEZ::GetRemediationOperation(IQuarantineItem* pQItem, DWORD& dwRemediationOperation, DWORD dwRemediationIndex)
{
    //
    // Pull remediation operation from Quarantine item
    CString cszQueryString;
    cszQueryString.Format(QSERVER_ITEM_REMEDIATION_OPERATION_TYPE, dwRemediationIndex);
    HRESULT hr = pQItem->GetDWORD((LPSTR)(LPCSTR)cszQueryString, &dwRemediationOperation);
    if( FAILED(hr) )
        return FALSE;

    return TRUE;
}

inline BOOL QItemEZ::GetRemediationType(IQuarantineItem* pQItem, DWORD& dwRemediationType, DWORD dwRemediationIndex)
{
    //
    // Pull remediation Type from Quarantine item
    CString cszQueryString;
    cszQueryString.Format(QSERVER_ITEM_REMEDIATION_ACTION_TYPE, dwRemediationIndex);
    HRESULT hr = pQItem->GetDWORD((LPSTR)(LPCSTR)cszQueryString, &dwRemediationType);
    if( FAILED(hr) )
        return FALSE;

	switch(dwRemediationType)
	{
	case ccEraser::RegistryRemediationActionType:
	case ccEraser::FileRemediationActionType:
	case ccEraser::ProcessRemediationActionType:
	case ccEraser::BatchRemediationActionType:
	case ccEraser::INIRemediationActionType:
	case ccEraser::ServiceRemediationActionType:
	case ccEraser::InfectionRemediationActionType:
	case ccEraser::COMRemediationActionType:
	case ccEraser::HostsRemediationActionType:
	case ccEraser::DirectoryRemediationActionType:
	case ccEraser::LSPRemediationActionType:
		break;
	default:
		dwRemediationType = (DWORD)(ccEraser::LastRemediationType) + 1;
		break;
	}

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// CString wrapper for IQuarantineItem::GetString
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
inline HRESULT QItemEZ::GetString(IQuarantineItem *pItem, LPCSTR szProperty, CString &cszString)
{
    cszString.Empty();

    DWORD dwSize = 0;

    HRESULT hr = pItem->GetString((LPSTR)szProperty, NULL, &dwSize);
    if(0 == dwSize || hr != E_BUFFER_TOO_SMALL)
        return hr;

    hr = pItem->GetString((LPSTR)szProperty, cszString.GetBuffer(dwSize), &dwSize);
    cszString.ReleaseBuffer();

    return hr;
}

////////////////////////////////////////////////////////////////////////////
// CString wrapper that gets IQuarantineItem::GetVirusName
//	if available, or falls back to IQuarantineItem::GetAnomalyName
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
inline BOOL QItemEZ::GetThreatName(IQuarantineItem* pQItem, CString& cszThreatName)
{
    cszThreatName.Empty();

	if(QItemEZ::GetVirusName(pQItem, cszThreatName))
		return TRUE;

	if(QItemEZ::GetAnomalyName(pQItem, cszThreatName))
		return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// CString wrapper for IQuarantineItem::GetVirusName
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
inline BOOL QItemEZ::GetVirusName(IQuarantineItem* pQItem, CString& cszThreatName)
{
    cszThreatName.Empty();

    DWORD dwSize = 0;
    HRESULT hr = pQItem->GetVirusName(NULL, &dwSize);
    if(0 == dwSize || hr != E_BUFFER_TOO_SMALL)
        return FALSE;

    hr = pQItem->GetVirusName(cszThreatName.GetBuffer(dwSize + 1), &dwSize);
    cszThreatName.ReleaseBuffer();
    if(FAILED(hr))
        return FALSE;

    return TRUE;
}

inline BOOL QItemEZ::GetAnomalyName(IQuarantineItem* pQItem, CString& cszThreatName)
{
    cszThreatName.Empty();

    DWORD dwSize = 0;
    HRESULT hr = pQItem->GetAnomalyName(NULL, &dwSize);
    if(0 == dwSize || hr != E_BUFFER_TOO_SMALL)
        return FALSE;

    hr = pQItem->GetAnomalyName(cszThreatName.GetBuffer(dwSize + 1), &dwSize);
    cszThreatName.ReleaseBuffer();
    if(FAILED(hr))
        return FALSE;

    return TRUE;
}

inline BOOL QItemEZ::GetAnomalyID(IQuarantineItem* pQItem, CString& cszAnomalyID)
{
	cszAnomalyID.Empty();

    DWORD dwSize = 0;
    HRESULT hr = pQItem->GetAnomalyID(NULL, &dwSize);
    if(0 == dwSize || hr != E_BUFFER_TOO_SMALL)
        return FALSE;

    hr = pQItem->GetAnomalyID(cszAnomalyID.GetBuffer(dwSize + 1), &dwSize);
    cszAnomalyID.ReleaseBuffer();
    if(FAILED(hr))
        return FALSE;

    return TRUE;
}


inline BOOL QItemEZ::GetThreatCategoryList(IQuarantineItem* pQItem, CString& cszThreatCategoryList, BOOL bConvertToText)
{
	cszThreatCategoryList.Empty();

	// Try to get ScanInfection categories first
	CString cszThreatCategories;
	CThreatCatInfo threatInfo;
	if(!QItemEZ::GetScanInfectionCategories(pQItem, cszThreatCategories))
	{
		// Try Anomaly categories
		if(!QItemEZ::GetAnomalyCategories(pQItem, cszThreatCategories))
			return FALSE;
	}

	if(!bConvertToText)
	{
		cszThreatCategoryList = cszThreatCategories;
		if(cszThreatCategoryList.GetLength() > MAX_PATH)
			cszThreatCategoryList.Truncate(MAX_PATH - 1);

		return TRUE;
	}

	if(threatInfo.GetCategoryText(cszThreatCategories, cszThreatCategoryList.GetBuffer(MAX_PATH), MAX_PATH))
	{
		cszThreatCategoryList.ReleaseBuffer();
		if(cszThreatCategoryList.GetLength() > MAX_PATH)
			cszThreatCategoryList.Truncate(MAX_PATH - 1);
		return TRUE;
	}

	return FALSE;
}

inline BOOL QItemEZ::GetScanInfectionCategories(IQuarantineItem* pQItem, CString& sScanInfectionCategories)
{
	sScanInfectionCategories.Empty();

	HRESULT hr = QItemEZ::GetString(pQItem, QSERVER_ITEM_INFO_CATEGORIES, sScanInfectionCategories);
    if(FAILED(hr))
        return FALSE;

	return TRUE;
}

inline BOOL QItemEZ::GetAnomalyCategories(IQuarantineItem* pQItem, CString& sAnomalyCategories)
{
	sAnomalyCategories.Empty();

	DWORD dwSize = 0;
    HRESULT hr = pQItem->GetAnomalyCategories(NULL, &dwSize);
    if(0 == dwSize || hr != E_BUFFER_TOO_SMALL)
        return FALSE;

    hr = pQItem->GetAnomalyCategories(sAnomalyCategories.GetBuffer(dwSize + 1), &dwSize);
    sAnomalyCategories.ReleaseBuffer();
    if(FAILED(hr))
        return FALSE;

    return TRUE;
}

inline INT QItemEZ::GetSystemIconIndex(IQuarantineItem* pQItem, INT iIconSize, DWORD dwIndex)
{
    CString cszFileName;
    DWORD dwType = NULL;

    HRESULT hr = QItemEZ::GetRemediationType(pQItem, dwType, dwIndex);
    if(FAILED(hr))
        return hr;

    switch(dwType)
    {
    case ccEraser::COMRemediationActionType:    
    case ccEraser::RegistryRemediationActionType: 
        cszFileName = _T("qcontest.reg");
        break;

    case ccEraser::ServiceRemediationActionType:  
    case ccEraser::ProcessRemediationActionType:  
    case ccEraser::InfectionRemediationActionType:
    case ccEraser::FileRemediationActionType:     
        hr = QItemEZ::GetFullANSIFilePath(pQItem, cszFileName, dwIndex);
        if(FAILED(hr))
            return hr;
        break;

    case ccEraser::BatchRemediationActionType:    
        cszFileName = _T("qcontest.bat");
        break;

    case ccEraser::INIRemediationActionType:      
        cszFileName = _T("qcontest.ini");
        break;

    default:
        break;
    }

    return QItemEZ::GetSystemIconIndex(cszFileName, iIconSize);
}

inline INT QItemEZ::GetSystemIconIndex(LPCTSTR szFileName, INT iIconSize)
{
    //
    // Request icon from system for dummy file
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    SHGetFileInfo( szFileName,  
        FILE_ATTRIBUTE_NORMAL, 
        &sfi, sizeof(SHFILEINFO), 
        SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | iIconSize );

    return sfi.iIcon;
}

////////////////////////////////////////////////////////////////////////////
// CString wrapper for IQuarantineItem::GetOriginalMachineName
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
inline BOOL QItemEZ::GetOriginalMachineName(IQuarantineItem *pItem, CString &cszOriginalMachineName)
{
    cszOriginalMachineName.Empty();

    DWORD dwSize = 0;
    
	HRESULT hr = pItem->GetOriginalMachineName(NULL, &dwSize);
    if(0 == dwSize || hr != E_BUFFER_TOO_SMALL)
        return FALSE;

    hr = pItem->GetOriginalMachineName(cszOriginalMachineName.GetBuffer(dwSize + 1), &dwSize);
    cszOriginalMachineName.ReleaseBuffer();

    if( FAILED(hr) )
        return FALSE;

    return TRUE;
}

inline BOOL QItemEZ::InitialDummyFile(IQuarantineItem* pQItem)
{
	bool bDummy = false;
    if(FAILED(pQItem->InitialDummyFile(&bDummy)))
		return FALSE;

	return (BOOL)bDummy;
}

inline BOOL QItemEZ::ShouldShowRemediation(IQuarantineItem* pQItem, DWORD dwRemediationIndex)
{
	// If the remediation was unsuccessful or does not have undo information, shouldn't show it,
	// it is just for submission to SND
	bool bRemediationSucceeded = false;
	if(FAILED(pQItem->GetRemediationActionSucceeded(dwRemediationIndex, &bRemediationSucceeded)))
		return FALSE;

	bool bUndoAvailable = false;
	if(FAILED(pQItem->GetRemediationUndoAvailable(dwRemediationIndex, &bUndoAvailable)))
		return FALSE;

	return (bRemediationSucceeded && bUndoAvailable);
}

////////////////////////////////////////////////////////////////////////////
// Parses the Categories and looks for a match to a viral/malicious type
// 1/26/2006 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
inline DWORD QItemEZ::IsViral(IQuarantineItem* pQItem)
{
    // Based on IsViral(IScanInfection* pInfection) method in CScanManaager
    //
    //    if( pInfection->IsCategoryType(SCAN_THREAT_TYPE_VIRUS) ||
    //        pInfection->IsCategoryType(SCAN_THREAT_TYPE_MALICIOUS) ||
    //        pInfection->IsCategoryType(SCAN_THREAT_TYPE_RESERVED_MALICIOUS) ||
    //        pInfection->IsCategoryType(SCAN_THREAT_TYPE_HEURISTIC) )
    //      return true;

    DWORD dwCategory = SCAN_THREAT_TYPE_VIRUS;
    CString cszAnomalyCategories;
    QItemEZ::GetScanInfectionCategories(pQItem, cszAnomalyCategories);

    CString cszTokenized;
    TCHAR* szTokens = _T(" ,");
    int iCurPos= 0;

    cszAnomalyCategories.Trim();
    if(cszAnomalyCategories.IsEmpty())
        return THREAT_UNKNOWN;

    cszTokenized = cszAnomalyCategories.Tokenize(szTokens, iCurPos);
    while(cszTokenized != "")
    {
        DWORD dwCategory = atol(cszTokenized);
        if( SCAN_THREAT_TYPE_VIRUS == dwCategory ||
            SCAN_THREAT_TYPE_MALICIOUS == dwCategory ||
            SCAN_THREAT_TYPE_RESERVED_MALICIOUS == dwCategory ||
            SCAN_THREAT_TYPE_HEURISTIC == dwCategory )
        {
            return THREAT_VIRAL;
        }

        cszTokenized = cszAnomalyCategories.Tokenize(szTokens, iCurPos);
    }

    return THREAT_NON_VIRAL;
}
