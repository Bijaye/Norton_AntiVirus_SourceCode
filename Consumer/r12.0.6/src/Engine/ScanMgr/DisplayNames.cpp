#include "Stdafx.h"
#include <mlang.h>
#include "DisplayNames.h"
#include "DecDefs.h"
#include "Resource.h"
#include "FileAPI.h"
#include "ScanManager.h"
#include "ThreatCatInfo.h"
#include "ccSymStringImpl.h"
#include "ccSymKeyValueCollectionImpl.h"

using namespace std;

CDisplayNames::CDisplayNames()
{
}

void CDisplayNames::SetScanType(bool bEmailScan, bool bDeepDelete)
{
    m_bEmailScan = bEmailScan;
    m_bDeepDelete = bDeepDelete;
}

bool CDisplayNames::SetScanInfectionRemdiationActionText(ccEraser::IRemediationAction* pScanRem)
{
    CEZRemediation EZRem(pScanRem);

    // Get the remediation action properties
    cc::IKeyValueCollectionPtr pProps = EZRem.GetProperties();
    if( pProps == NULL )
    {
        CCTRACEE(_T("CDisplayNames::SetScanInfectionRemdiationActionText() - Error getting remediation action properties."));
        return false;
    }

    // Get the user data properties
    cc::IKeyValueCollectionPtr pUserData = EZRem.GetUserData();
    if( pUserData != NULL )
    {
        // If text is already set we don't need to do it again cause it ain't gonna change
        if( pUserData->GetExists(RemediationDescription) )
        {
            CCTRACEI(_T("CDisplayNames::SetScanInfectionRemdiationActionText() - Scan infection remediation descriptive text is already set for this item."));
            return true;
        }

        // Get the scan infection item
        IScanInfectionPtr pInfection = EZRem.GetScanInfection();
        if( pInfection != NULL )
        {
            // Get the display text for this scan infection
            ATL::CAtlString strRemDesc;
            if( GetFullScanInfectionPath(pInfection, strRemDesc) && !strRemDesc.IsEmpty() )
            {
                if( pUserData )
                {
                    // Set the string in the user data as an IString
                    cc::IStringPtr pStr;
                    pStr.Attach(ccSym::CStringImpl::CreateStringImpl());

                    if( pStr )
                    {
                        pStr->SetStringA(strRemDesc);
                        if( pUserData->SetValue(RemediationDescription, pStr) )
                        {
                            CCTRACEI(_T("CDisplayNames::SetScanInfectionRemdiationActionText() - Scan infection remediation descriptive text set to %s in user data."), pStr->GetStringA());
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool CDisplayNames::SetRemediationStatusText(ccEraser::IRemediationAction* pAction, CommonUIInfectionStatus eStatus)
{
    if( pAction == NULL )
    {
        CCTRACEE(_T("CDisplayNames::SetRemediationStatusText() - Remediation action is NULL."));
        return false;
    }

    ATL::CAtlString strStatus;
    CEZRemediation ezRem(pAction);

    // Get the remediation type
    ccEraser::eObjectType type = ezRem.GetType();

    // Get the remediation state and result to build the status string
    ccEraser::IRemediationAction::Action_State state = ezRem.GetState();
    ccEraser::IRemediationAction::Operation_Type opType = ezRem.GetOperationType();
    ccEraser::eResult eRemediationResult = ccEraser::False;
    bool bHaveRemediationResult = ezRem.GetRemediationResult(eRemediationResult);

    // Is this something that requires reboot?
    if( eRemediationResult == ccEraser::RebootRequired || ezRem.ManualRebootRequired() )
    {
        strStatus.LoadString(IDS_REBOOT_REQUIRED);
        return ezRem.SetStatusText(strStatus);
    }

    // If this is an excluded or failed exclude anomaly then mark
    // all remediation actions appropriately
    if( eStatus == InfectionStatus_Exclude_Failed )
    {
        strStatus.LoadString(IDS_EXCLUDE_FAILED);
        return ezRem.SetStatusText(strStatus);
    }
    else if( eStatus == InfectionStatus_Excluded )
    {
        strStatus.LoadString(IDS_EXCLUDED);
        return ezRem.SetStatusText(strStatus);
    }

    if( type == ccEraser::InfectionRemediationActionType )
    {
        // Set status for scan infection items to whatever
        // the user data status is, if we can't get it use the anomaly status
        IScanInfectionPtr pInfection = ezRem.GetScanInfection();
        if( pInfection )
        {
            eStatus = static_cast<CommonUIInfectionStatus>(pInfection->GetUserData());
        }

        switch(eStatus)
        {
        case InfectionStatus_Repaired:
            strStatus.LoadString(IDS_REPAIRED);
            break;
        case InfectionStatus_Repair_Failed:
            strStatus.LoadString(IDS_REPAIR_FAILED);
            break;
        case InfectionStatus_Quarantine_Failed:
            strStatus.LoadString(IDS_QUARANTINE_FAILED);
            break;
        case InfectionStatus_Delete_Failed:
            strStatus.LoadString(IDS_DELETE_FAILED);
            break;
        case InfectionStatus_Quarantined:
            strStatus.LoadString(IDS_QUARANTINED);
            break;
        case InfectionStatus_Deleted:
            strStatus.LoadString(IDS_DELETED);
            break;
        case InfectionStatus_AccessDenied:
            strStatus.LoadString(IDS_ACCESS_DENIED);
            break;
        case InfectionStatus_NotDetected:
            strStatus.LoadString(IDS_NOACTION_REQUIRED);
            break;
        case InfectionStatus_RebootRequired:
            strStatus.LoadString(IDS_REBOOT_REQUIRED);
            break;
        case InfectionStatus_CantDelete:
        case InfectionStatus_Unhandled:
        default:
            strStatus.LoadString(IDS_DETECTED);
            break;
        }
    }
    else
    {
        if( !bHaveRemediationResult )
        {
            strStatus.LoadString(IDS_NOACTION_TAKEN);
        }
        else
        {
            // The remediation was performed

            // Certain results are displayed generically for all types
            if( eRemediationResult == ccEraser::AccessDenied )
            {
                strStatus.LoadString(IDS_ACCESS_DENIED);
            }
            else if( eRemediationResult == ccEraser::FileNotFound )
            {
                strStatus.LoadString(IDS_NOT_DETECTED);
            }
            else if( eRemediationResult == ccEraser::NothingToDo || 
                     eRemediationResult == ccEraser::FileStillPresent && eStatus == InfectionStatus_Repaired )
            {
                strStatus.LoadString(IDS_NOACTION_REQUIRED);
            }
            else if( eRemediationResult == ccEraser::Deleted )
            {
                strStatus.LoadString(IDS_DELETED);
            }
            else if( eRemediationResult == ccEraser::Repaired )
            {
                strStatus.LoadString(IDS_REPAIRED);
            }
            else
            {
                GetRemediationStatus(eRemediationResult, opType, strStatus);
            }
        }
    }

    return ezRem.SetStatusText(strStatus);
}

bool CDisplayNames::GetRemediationStatus(ccEraser::eResult eResult, ccEraser::IRemediationAction::Operation_Type opType, ATL::CAtlString& strStatus)
{
    if( ccEraser::Succeeded(eResult) )
    {
        switch(opType)
        {
        case ccEraser::IRemediationAction::Delete:
        case ccEraser::IRemediationAction::Remove:
            strStatus.LoadString(IDS_DELETED);
            break;
        case ccEraser::IRemediationAction::Suspend:
        case ccEraser::IRemediationAction::Stop:
        case ccEraser::IRemediationAction::Terminate:
            strStatus.LoadString(IDS_TERMINATED);
            break;
        case ccEraser::IRemediationAction::Move:
        case ccEraser::IRemediationAction::CreateEmptyFile:
        case ccEraser::IRemediationAction::DeleteLine:
        case ccEraser::IRemediationAction::Set:
        case ccEraser::IRemediationAction::SetIPAddress:
        case ccEraser::IRemediationAction::SetDomainName:
        default:
            strStatus.LoadString(IDS_REPAIRED);
            break;
        }
    }
    else // Failure
    {
        // For all other failure types just using "Failed" text
        switch(opType)
        {
        case ccEraser::IRemediationAction::Delete:
        case ccEraser::IRemediationAction::Move:
        case ccEraser::IRemediationAction::CreateEmptyFile:
        case ccEraser::IRemediationAction::DeleteLine:
        case ccEraser::IRemediationAction::Set:
        case ccEraser::IRemediationAction::Terminate:
        case ccEraser::IRemediationAction::Suspend:
        case ccEraser::IRemediationAction::Stop:
        case ccEraser::IRemediationAction::Remove:
        case ccEraser::IRemediationAction::SetIPAddress:
        case ccEraser::IRemediationAction::SetDomainName:
        default:
            strStatus.LoadString(IDS_OPERATION_FAILED);
            break;
        }
    }

    return true;
}

bool CDisplayNames::UpdateAllRemediationStatusText(CEZAnomaly* pEZAnomaly)
{
    if( pEZAnomaly == NULL )
    {
        CCTRACEE(_T("CDisplayNames::UpdateAllRemediationStatusText() - Anomaly is NULL."));
        return false;
    }

    // The generic text is simply the count of affected areas
    DWORD dwTotalCount = 0;

    // Get the user data
    cc::IKeyValueCollectionPtr pUserData = pEZAnomaly->GetUserData();
    if( !pUserData )
    {
        CCTRACEE(_T("CDisplayNames::UpdateAllRemediationStatusText() - Failed to get the anomaly user data property."));
        return false;
    }

    // Get the total remediation count
    dwTotalCount = pEZAnomaly->GetRemediationCount();

    DWORD dwTotal = dwTotalCount;
    for(DWORD nCur = 0; nCur < dwTotal; nCur++)
    {
        ccEraser::IRemediationActionPtr pAction = pEZAnomaly->GetRemediation(nCur);
        if( pAction != NULL )
        {
            // Set the status text for this remediation action
            DWORD dwStatus = InfectionStatus_Unhandled;
            pUserData->GetValue(CurrentStatus, dwStatus);
            SetRemediationStatusText(pAction, static_cast<CommonUIInfectionStatus>(dwStatus));
        }
    }

    return true;
}

bool CDisplayNames::SetGenericText(ccEraser::IAnomaly* pAnomaly)
{
    if( pAnomaly == NULL )
    {
        CCTRACEE(_T("CDisplayNames::SetGenericText() - Anomaly is NULL."));
        return false;
    }

    // The generic text is simply the count of affected areas
    DWORD dwTotalCount = 0;

    CEZAnomaly ezAnom(pAnomaly);

    // Get the user data
    cc::IKeyValueCollectionPtr pUserData = ezAnom.GetUserData();
    if( !pUserData )
    {
        CCTRACEE(_T("CDisplayNames::SetGenericText() - Failed to get the user data property."));
        return false;
    }

    // Get the total remediation count
    dwTotalCount = ezAnom.GetRemediationCount();

    DWORD dwTotal = dwTotalCount;
    for(DWORD nCur = 0; nCur < dwTotal; nCur++)
    {
        ccEraser::IRemediationActionPtr pAction = ezAnom.GetRemediation(nCur);
        if( pAction != NULL )
        {
            CEZRemediation EZRemediation(pAction);

            // Get the type
            ccEraser::eObjectType remType = EZRemediation.GetType();

            // Set the status text for this remediation action
            DWORD dwStatus = InfectionStatus_Unhandled;
            pUserData->GetValue(CurrentStatus, dwStatus);
            SetRemediationStatusText(pAction, static_cast<CommonUIInfectionStatus>(dwStatus));

            // Is this is a scan infection remediation action?
            if( remType == ccEraser::InfectionRemediationActionType )
            {
                // Save off the display text for this remediation action
                SetScanInfectionRemdiationActionText(pAction);

                // Need to find out if this is a file, boot record, mbr, or memory scan infection

                // Get the scan infection item
                IScanInfectionPtr pInfection = EZRemediation.GetScanInfection();
                if( pInfection == NULL )
                {
                    CCTRACEE(_T("CDisplayNames::SetGenericText() - Failed to get the scan infection object, will not count this item."));
                    continue;
                }

                // Get the user data
                cc::IKeyValueCollectionPtr pColUserData = EZRemediation.GetUserData();
                if( pColUserData == NULL )
                {
                    CCTRACEE(_T("CDisplayNames::SetGenericText() - Failed to get the user data, will not count this item."));
                    continue;
                }

                IScanFileInfectionQIPtr pQIFile;
                IScanCompressedFileInfectionQIPtr pQICompFile;
                IScanBootInfectionQIPtr pQIBoot;
                IScanMBRInfectionQIPtr pQIMBR;
                if( pQIFile = pInfection )
                {
                    // Set the type in the user data
                    if( pColUserData->SetValue(ScanInfectionType, static_cast<DWORD>(FileInfectionType)) )
                        CCTRACEI(_T("CDisplayNames::SetGenericText() - Scan infection remediation type set to %d in user data."), FileInfectionType);
                }
                else if( pQICompFile = pInfection )
                {
                    // Set the type in the user data
                    if( pColUserData->SetValue(ScanInfectionType, static_cast<DWORD>(CompressedInfectinType)) )
                        CCTRACEI(_T("CDisplayNames::SetGenericText() - Scan infection remediation type set to %d in user data."), CompressedInfectinType);

                }
                else if( pQIBoot = pInfection )
                {
                    // Set the type in the user data
                    if( pColUserData->SetValue(ScanInfectionType, static_cast<DWORD>(BRInfectionType)) )
                        CCTRACEI(_T("CDisplayNames::SetGenericText() - Scan infection remediation type set to %d in user data."), BRInfectionType);
                }
                else if( pQIMBR = pInfection )
                {
                    // Set the type in the user data
                    if( pColUserData->SetValue(ScanInfectionType, static_cast<DWORD>(MBRInfectionType)) )
                        CCTRACEI(_T("CDisplayNames::SetGenericText() - Scan infection remediation type set to %d in user data."), MBRInfectionType);
                }
                else
                {
                    CCTRACEE(_T("CDisplayNames::SetGenericText() - Failed to QI for any of the known scan infection types."));
                }
            }
            else if( remType == ccEraser::ProcessRemediationActionType )
            {
                // Decrement process suspend types to avoid duplicates
                if( EZRemediation.GetOperationType() == ccEraser::IRemediationAction::Suspend && dwTotalCount >= 2 )
                    dwTotalCount--;
            }
            else if( remType == ccEraser::ServiceRemediationActionType )
            {
                // Decrement service stop types to avoid duplicates
                if( EZRemediation.GetOperationType() == ccEraser::IRemediationAction::Stop && dwTotalCount >= 2 )
                    dwTotalCount--;
            }
        }
        else
        {
            CCTRACEE(_T("CDisplayNames::SetGenericText() - Failed to get remediation action %d."), nCur);
            return false;
        }
    }

    // Convert the count to a string
    TCHAR szCount[50] = {0};
    _ultot(dwTotalCount, szCount, 10);

    cc::IStringPtr psymstrText;
    psymstrText.Attach(ccSym::CStringImpl::CreateStringImpl(szCount));
    if( !pUserData->SetValue(GenericDescription, psymstrText) )
    {
        CCTRACEE(_T("CDisplayNames::SetGenericText() - Failed to set the userdata generic description value"));
        return false;
    }

    return true;
}

void CDisplayNames::ConvertFwdSlashToBckSlash(LPTSTR pszStrToConvert)
{
    ATL::CAtlString strTemp(pszStrToConvert);
    strTemp.Replace(_TCHAR('/'), _TCHAR('\\'));
    _tcscpy(pszStrToConvert, strTemp);
}

bool CDisplayNames::GetDisplayNameForFile(IScanInfection* pInfection, string& strFileName)
{
    // Is this a file infection?

    CScanPtr<IScanFileInfection2> pFileInfection;
    SYMRESULT Status = pInfection->QueryInterface(IID_ScanFileInfection2,
        (void**) &pFileInfection);

    if (SYM_SUCCEEDED(Status))
    {
        // Get the infected file name.

        strFileName = pFileInfection->GetLongFileName();

        // Convert the file name to ANSI (if necessary).

        if (pFileInfection->AreOEMFileApis())
        {
            vector<char> vBuffer;
            vBuffer.reserve(strFileName.size() + 1);
            OemToCharBuff(strFileName.c_str(), &vBuffer[0], strFileName.size());
            vBuffer[strFileName.size()] = '\0';
            strFileName = &vBuffer[0];
        }

        // Are we (possibly) dealing with a SFN? If so, try to find the LFN.

        if (_tcschr(strFileName.c_str(), '~') != NULL)
        {
            CFileAPI FileAPI;
            FileAPI.SwitchAPIToANSI();

            char szLongPath[MAX_PATH];
            if (GetLongPathName(strFileName.c_str(), szLongPath) == true)
            {
                strFileName = szLongPath;
            }
        }

        return true;
    }

    // Is this a compressed file infection?

    CScanPtr<IScanCompressedFileInfection2> pCompressedInfection;
    Status = pInfection->QueryInterface(IID_ScanCompressedFileInfection2,
        (void**) &pCompressedInfection);

    if (SYM_SUCCEEDED(Status))
    {
        // Get the infected component name.
        ATL::CAtlString strFileNameTemp;

        // Perform the appropriate conversion (if necessary).
        int nSize = strlen(pCompressedInfection->GetComponent(0)) + 1;
        ConvertToCharSet( pCompressedInfection->GetComponent(0),
            strFileNameTemp.GetBuffer(nSize),
            nSize,
            pCompressedInfection->GetDecCharSet(),
            pCompressedInfection->AreOEMFileApis());

        strFileNameTemp.ReleaseBuffer();

        // Replace any forward slashes with backslashes
        strFileNameTemp.Replace(_TCHAR('/'), _TCHAR('\\'));

        // Get the file name component.
        int nSlashPos = strFileNameTemp.ReverseFind(_TCHAR('\\'));

        if( nSlashPos != -1  && nSlashPos != strFileNameTemp.GetLength() )
        {
            strFileNameTemp = strFileNameTemp.Right(strFileNameTemp.GetLength() - (nSlashPos+1));
        }

        strFileName = strFileNameTemp.GetString();

        return true;
    }

    return false;
}

bool CDisplayNames::ISO2022JPToMBCS(const char* szSource, char* szTarget, int iTargetSize)
{
    bool bResult = true;
    IMLangConvertCharset* pConvert = NULL;

    try
    {
        // Create and initialize the MLang conversion component.

        HRESULT hResult = CoCreateInstance(CLSID_CMLangConvertCharset, NULL,
            CLSCTX_INPROC_SERVER, IID_IMLangConvertCharset, (void**) &pConvert);

        if (FAILED(hResult))
            throw runtime_error("Unable to create MLang component.");

        hResult = pConvert->Initialize(50220, 1200, MLCONVCHARF_AUTODETECT);

        if (FAILED(hResult))
            throw runtime_error("Unable to initialize the MLang component.");

        // Convert from iso-2022-jp to Unicode.

        UINT uBufferSize = _tcslen(szSource);
        wchar_t* pBuffer = new wchar_t[uBufferSize];
        if( NULL == pBuffer )
            throw runtime_error("CDisplayNames::ISO2022JPToMBCS - Error unable to allocate a buffer.");

        hResult = pConvert->DoConversionToUnicode( (char*)szSource, NULL, pBuffer,
            &uBufferSize);

        if (SUCCEEDED(hResult))
        {
            // Convert from Unicode to MBCS.

            int iBytes = WideCharToMultiByte(CP_ACP, 0, pBuffer, uBufferSize,
                szTarget, iTargetSize, NULL, NULL);

            szTarget[iBytes] = '\0';
        }

        delete [] pBuffer;

        // Make sure the conversion was successful.

        if (FAILED(hResult))
        {
            throw runtime_error("DoConversionToUnicode() failed.");
        }
    }
    catch(exception& Ex)
    {
        bResult = false;

        CCTRACEE(Ex.what());
    }
    catch(...)
    {
        bResult = false;

        CCTRACEE("Caught in CDisplayName::ISO2022JPToMBCS().");
    }

    if (pConvert != NULL)
        pConvert->Release();

    return bResult;
}

bool CDisplayNames::GetLongPathName(const char* szShortPath, char* szLongPath)
{
    // if szShortPath > MAX_PATH do not do conversion
    if(strlen(szShortPath) > MAX_PATH)
        return false;

    // Make a copy of the short path.

    char szShortCopy[MAX_PATH];
    _tcsncpy(szShortCopy, szShortPath, MAX_PATH);

    // Get the drive portion (i.e. C:).

    const char* pToken = _tcstok(szShortCopy, "\\");

    if (pToken == NULL)
        return false;

    _stprintf(szLongPath, "%s", pToken);

    // Tack on each directory/file.

    char szBuffer[MAX_PATH];
    _tcscpy(szBuffer, szLongPath);

    HANDLE hFind = NULL;
    WIN32_FIND_DATA FindData;

    while(pToken = _tcstok(NULL, "\\"))
    {
        _tcscat(szBuffer, "\\");
        _tcscat(szBuffer, pToken);

        hFind = FindFirstFile(szBuffer, &FindData);

        if (hFind == INVALID_HANDLE_VALUE)
            return false;

        FindClose(hFind);

        if (_tcslen(szLongPath) + _tcslen(FindData.cFileName) >= MAX_PATH)
            return false;

        _tcscat(szLongPath, "\\");
        _tcscat(szLongPath, FindData.cFileName);
        _tcscpy(szBuffer, szLongPath);
    }

    // Make sure we got a valid long file name.

    if (GetFileAttributes(szLongPath) == -1)
        return false;

    return true;
}

bool CDisplayNames::ConvertToCharSet(const char* szSource, 
                                     char* szTarget, 
                                     int iTargetSize, 
                                     DWORD dwCharSet,
                                     bool bOemPaths)
{
    bool bReturn = true;

    // Perform the appropriate conversion (if necessary).

    if (dwCharSet == DEC_CHARSET_CP850)
    {
        OemToCharBuff(szSource, szTarget, iTargetSize);
    }
    else if (dwCharSet == DEC_CHARSET_ISO_2022_JP)
    {
        if (ISO2022JPToMBCS(szSource, szTarget, iTargetSize) == false)
        {
            bReturn = false;
        }
    }
    else if (dwCharSet == DEC_CHARSET_CP1252)
    {
        if (bOemPaths == true)
        {
            OemToCharBuff(szSource, szTarget, iTargetSize);
        }
        else
        {
            strncpy(szTarget, szSource, iTargetSize);
        }
    }
    else if (dwCharSet == DEC_CHARSET_ISO_8859_2)
    {
        if (ISO8859_2ToMBCS(szSource, szTarget, iTargetSize) == false)
            bReturn = false;
    }
    else if (dwCharSet == DEC_CHARSET_UTF_8)
    {
        if (UTF8ToMBCS(szSource, szTarget, iTargetSize) == false)
        {
            bReturn = false;
        }
    }
    else
    {
        strncpy(szTarget, szSource, iTargetSize);
    }

    return bReturn;
}

bool CDisplayNames::ISO8859_2ToMBCS(const char* szSource, char* szTarget, int iTargetSize)
{
    bool bResult = true;
    IMLangConvertCharset* pConvert = NULL;

    try
    {
        // Create and initialize the MLang conversion component.

        HRESULT hResult = CoCreateInstance(CLSID_CMLangConvertCharset, NULL,
            CLSCTX_INPROC_SERVER, IID_IMLangConvertCharset, (void**) &pConvert);

        if (FAILED(hResult))
            throw runtime_error("Unable to create MLang component.");

        hResult = pConvert->Initialize(28592, 1200, MLCONVCHARF_AUTODETECT);

        if (FAILED(hResult))
            throw runtime_error("Unable to initialize the MLang component.");

        // Convert from iso-8859-2 to Unicode.

        UINT uBufferSize = _tcslen(szSource);
        wchar_t* pBuffer = new wchar_t[uBufferSize];
        if( NULL == pBuffer )
            throw runtime_error("CDisplayNames::ISO8859_2ToMBCS - Error unable to allocate a buffer.");

        hResult = pConvert->DoConversionToUnicode( (char*)szSource, NULL, pBuffer,
            &uBufferSize);

        if (SUCCEEDED(hResult))
        {
            // Convert from Unicode to MBCS.

            int iBytes = WideCharToMultiByte(CP_ACP, 0, pBuffer, uBufferSize,
                szTarget, iTargetSize, NULL, NULL);

            szTarget[iBytes] = '\0';
        }

        delete [] pBuffer;

        // Make sure the conversion was successful.

        if (FAILED(hResult))
        {
            throw runtime_error("DoConversionToUnicode() failed.");
        }
    }
    catch(exception& Ex)
    {
        bResult = false;

        CCTRACEE(Ex.what());
    }
    catch(...)
    {
        bResult = false;

        CCTRACEE("Caught in CDisplayName::ISO2022JPToMBCS().");
    }

    if (pConvert != NULL)
        pConvert->Release();

    return bResult;
}

bool CDisplayNames::UTF8ToMBCS(const char* szSource, char* szTarget, int iTargetSize)
{
    try
    {
        UINT uInputSize = _tcslen(szSource);
        UINT uOutputSize = _tcslen(szSource);

        // Make sure the output bufffer is big enough
        if(iTargetSize < (int)uOutputSize)
            return false;

        vector<char> Input(uInputSize);
        vector<wchar_t> Output(uOutputSize);

        CopyMemory(&Input[0], szSource, uInputSize);

        // Convert from UTF-8 to Unicode.

        if (MultiByteToWideChar(CP_UTF8, 0, &Input[0], uInputSize, &Output[0], uOutputSize) == 0)
            throw runtime_error("Conversion to Unicode failed.");

        // Convert from Unicode to MBCS.

        int iFinalSize = WideCharToMultiByte(CP_ACP, 0, &Output[0], uOutputSize,
            &Input[0], uInputSize, NULL, NULL);

        if (iFinalSize == 0)
            throw runtime_error("Conversion from Unicode to MBCS failed.");

        _tcscpy(szTarget, &Input[0]);

        return true;
    }
    catch(exception& Ex)
    {
        CCTRACEE(Ex.what());
    }
    catch(...)
    {
        CCTRACEE("Unknown exception caught in CDisplayNames::UTF8ToMBCS.");
    }

    // An error occured
    return false;
}

bool CDisplayNames::IsNonViralThreat(IScanInfection* pInfection)
{
    bool bThreat = false;
    const unsigned long* pCategories = pInfection->GetCategories();

    if( pCategories && *pCategories )
    {
        if( *pCategories > SCAN_THREAT_TYPE_HEURISTIC )
            bThreat = true;
    }

    return bThreat;
}

bool CDisplayNames::GetFullScanInfectionPath(IScanInfection* pInfection, ATL::CAtlString& strPath)
{
    // File infection?
    IScanFileInfection2QIPtr pFile2 = pInfection;
    if( pFile2.m_p != NULL )
    {
        strPath = pFile2->GetLongFileName();

        // Need to convert to ANSI?
        if( pFile2->AreOEMFileApis() )
            strPath.OemToAnsi();

        return true;
    }

    // Compressed file infection?
    IScanCompressedFileInfection2QIPtr pCompressedInfection = pInfection;
    if( pCompressedInfection.m_p != NULL )
    {
        // This will be the infected component file name
        ATL::CAtlString strFileName;

        // Perform the appropriate conversion (if necessary).
        int nSize = strlen(pCompressedInfection->GetComponent(0)) + 1;
        ConvertToCharSet( pCompressedInfection->GetComponent(0),
            strFileName.GetBuffer(nSize),
            nSize,
            pCompressedInfection->GetDecCharSet(),
            pCompressedInfection->AreOEMFileApis());

        strFileName.ReleaseBuffer();

        // Replace any forward slashes with backslashes
        strFileName.Replace(_TCHAR('/'), _TCHAR('\\'));

        // Get the file name component.
        int nSlashPos = strFileName.ReverseFind(_TCHAR('\\'));

        if( nSlashPos != -1  && nSlashPos != strFileName.GetLength())
        {
            strFileName = strFileName.Right(strFileName.GetLength() - (nSlashPos+1));
        }

        // Start building the path
        char szWithin[MAX_PATH];
        LoadString(g_hInstance, IDS_WITHIN, szWithin, MAX_PATH);

        int iCount = pCompressedInfection->GetComponentCount();

        unsigned long ulDeletableAncestor = 0;

        // For Deep Deletes, get the file we actually delete
        if(m_bDeepDelete)
        {
            ulDeletableAncestor = pCompressedInfection->GetCanDeleteAncestorDepth();

            // Fix up the Deletable Ancestor count since it counts up from the Top level down 
            // IE the top level container is level 0 and GetComponent works in the opposite direction.
            if(ulDeletableAncestor > 0)
            {
                char szDeletedName[MAX_PATH];
                ulDeletableAncestor = iCount - ulDeletableAncestor - 1;

                ConvertToCharSet(pCompressedInfection->GetComponent(ulDeletableAncestor), 
                    szDeletedName, 
                    MAX_PATH, 
                    pCompressedInfection->GetComponentCharSet(ulDeletableAncestor),
                    pCompressedInfection->AreOEMFileApis());

                ConvertFwdSlashToBckSlash(szDeletedName);

                strPath = szDeletedName;
            }
            else 
                strPath = strFileName;
        }
        else
            strPath = strFileName;

        if (m_bEmailScan == true)
        {
            iCount--;
        }

        char szContainer[SYM_MAX_LONG_PATH] = {0};

        int iIndex = 1;
        if(m_bDeepDelete)
            iIndex = ulDeletableAncestor + 1;

        // For certain languages (e.g.Japanese), we need to change the order we build
        // the display string in. To accomplish this, we check the resources for our flag
        // to reverse the order.
        //
        // English string example: "Virus.exe within container 1 within container 2."
        // Reversed string example: "Virus.exe within container 2 withing container 1."

        char szFlag[MAX_PATH];
        LoadString(g_hInstance, IDS_REVERSE_COMPRESS_STRING_FLAG, szFlag, sizeof(szFlag));

        // English Version.
        char szDecoded[SYM_MAX_LONG_PATH] = {0};

        if(0 != _tcsicmp("true", szFlag))
        {
            for (iIndex; iIndex < iCount; iIndex++)
            {
                if (iIndex < iCount)
                {
                    strPath += szWithin;
                }

                _tcsncpy(szContainer, pCompressedInfection->GetComponent(iIndex), 
                    SYM_MAX_LONG_PATH);

                ConvertFwdSlashToBckSlash(szContainer);

                // If this is the top most container only convert to ANSI if the
                // file api's are in OEM...don't do this for email scans because
                // we won't ever touch the top level component since it's in a MIME
                // container.
                if( !m_bEmailScan && iIndex+1 == iCount )
                {
                    if( pCompressedInfection->AreOEMFileApis() )
                        OemToChar(szContainer, szDecoded);
                    else
                        _tcscpy(szDecoded, szContainer);
                }
                else
                {
                    if (ConvertToCharSet(szContainer, szDecoded, sizeof(szDecoded),
                        pCompressedInfection->GetComponentCharSet(iIndex),
                        pCompressedInfection->AreOEMFileApis()) == false)
                    {
                        _tcscpy(szDecoded, szContainer);
                    }
                    else
                        ConvertFwdSlashToBckSlash(szDecoded);
                }

                // Are we (possibly) dealing with a SFN? If so, try to find the LFN.

                if (_tcschr(szDecoded, '~') != NULL)
                {
                    CFileAPI FileAPI;
                    FileAPI.SwitchAPIToANSI();
                    char szLongPath[MAX_PATH];
                    if (GetLongPathName(szDecoded, szLongPath) == true)
                    {
                        _tcscpy(szDecoded, szLongPath);
                    }
                }

                strPath += szDecoded;
            }
        }

        // Special localized version.
        else
        {
            // Clear out the string since we need to put the infection name last.
            bool bFirstPass = true;
            strPath.Empty();

            for (iCount; iIndex < iCount; iCount--)
            {
                // Only add the append string if we are in the middle of the string
                // We do not want to add it on the first iteration.

                if (iIndex < iCount && !bFirstPass)
                {
                    strPath += szWithin;
                }

                _tcsncpy(szContainer, pCompressedInfection->GetComponent(iCount-1), 
                    SYM_MAX_LONG_PATH);

                ConvertFwdSlashToBckSlash(szContainer);

                // The first pass is the outer most container, so only convert it to ANSI
                // if we are in OEM mode but not for email scans
                if( !m_bEmailScan && bFirstPass )
                {
                    if( pCompressedInfection->AreOEMFileApis() )
                        OemToChar(szContainer, szDecoded);
                    else
                        _tcscpy(szDecoded, szContainer);
                }
                else
                {
                    if (ConvertToCharSet(szContainer, szDecoded, sizeof(szDecoded),
                        pCompressedInfection->GetComponentCharSet(iCount-1),
                        pCompressedInfection->AreOEMFileApis()) == false)
                    {
                        _tcscpy(szDecoded, szContainer);
                    }
                    else
                        ConvertFwdSlashToBckSlash(szDecoded);
                }

                bFirstPass = false;

                // Are we (possibly) dealing with a SFN? If so, try to find the LFN.

                if (_tcschr(szDecoded, '~') != NULL)
                {
                    CFileAPI FileAPI;
                    FileAPI.SwitchAPIToANSI();
                    char szLongPath[MAX_PATH];
                    if (GetLongPathName(szDecoded, szLongPath) == true)
                    {
                        _tcscpy(szDecoded, szLongPath);
                    }
                }

                strPath += szDecoded;
            }

            // We don't want to add the word "within" if the infected
            // file is a direct email attachment (i.e. not inside of another container).

            if ((m_bEmailScan == false) ||
                (pCompressedInfection->GetComponentCount() > 2))
            {
                strPath += szWithin;
            }

            strPath += strFileName;
        }

        return true;
    }

    // MBR?
    IScanMBRInfectionQIPtr pMBRInfection = pInfection;
    if( pMBRInfection.m_p != NULL )
    {
        // Generate the infection name.
        strPath.Format(IDS_DISPLAY_MBR, pMBRInfection->GetDriveNumber());
        return true;
    }

    // BR?
    IScanBootInfectionQIPtr pBoot = pInfection;
    if( pBoot.m_p != NULL )
    {
        strPath.Format(IDS_DISPLAY_BOOTREC, pBoot->GetDriveLetter());
        return true;
    }

    CCTRACEE(_T("CDisplayNames::GetFullScanInfectionPath() - Failed to QI for both compressed and regular file infection objects."));
    return false;
}