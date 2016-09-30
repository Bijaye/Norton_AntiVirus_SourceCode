// NAVOptionsAPExclusions.cpp

#include "stdafx.h"
#include "NAVOptionsObj.h"
#include "..\navoptionsres\resource.h"
#include "NAVErrorResource.h"

//
// Auto-Protect Exclusion functions
//

HRESULT CNAVOptions::initializeAPExclusions(void)
{
    // Open a handle to the AP Exclusions list and populate
    // our internal vector with the exclusion items. Only need
    // to do this if we do not already have an open handle to
    // the AP Exclusions
    if( !m_hAPExclusions )
    {
        // Make sure our internal list is empty
        m_vAPExclusions.clear();

        // First load up all of the excluded extensions from the savrt.dat file
        const TCHAR* pcszCurOptName = AP_ExclusionExt;
        APEXCLUSIONITEM ExcluItem;
        TCHAR szList[SAVRT_OPTS_MAX_BINARY_LENGTH] = {0};

        
        if( forceError(IDS_Err_Get_APExclusionExt) )
        {
            MakeError(IDS_Err_Get_APExclusionExt, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }

        // Load the normal extensions then the compressed extensions
        for( int i=0; i<2; i++ )
        {
            // Get the list
            if( FAILED(GetExtList(pcszCurOptName,szList)) )
            {
                return E_FAIL;
            }

            // Get the regular extension exclusions
            TCHAR *nextExt = NULL;

            // Get the first extension
            nextExt = _tcstok(szList,_T(" "));
            while( NULL != nextExt )
            {
                // Save this item
                _tcscpy(ExcluItem.szExclusion, _T("*."));
                _tcscat(ExcluItem.szExclusion, nextExt);
                ExcluItem.dwFlags = 0;

                // Save the flags
                if( i == 0 )
                    ExcluItem.dwFlags |= AP_EXC_VIRUS_FOUND;
                else
                    ExcluItem.dwFlags |= AP_EXC_VIRUS_FOUND_ARCHIVE;

                m_vAPExclusions.push_back(ExcluItem);

                // Get the next extension
                nextExt = _tcstok(NULL, _T(" "));
            }

            // Now load up the compressed extensions
            pcszCurOptName = AP_ArchiveExclusionExt;
        }

        if( APExclusionOpen && APExclusionGetNext)
        {
            if( forceError (IDS_Err_AP_Exclusions) ||
                SAVRT_OPTS_OK != APExclusionOpen(SAVRT32_VERSION, &m_hAPExclusions) )
            {
                MakeError(IDS_Err_AP_Exclusions,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
                return E_FAIL;
            }

            SAVRT_OPTS_STATUS status = SAVRT_OPTS_OK;
            TCHAR* pszExludePath = NULL;
            ExcluItem.dwFlags = 0;

            // Populate our vector with the exclusion items
            while( SAVRT_OPTS_OK == (status = APExclusionGetNext(m_hAPExclusions, &pszExludePath, &ExcluItem.dwFlags)) )
            {
                // Save this item
				if( pszExludePath )
				{
					_tcscpy(ExcluItem.szExclusion, pszExludePath);
					m_vAPExclusions.push_back(ExcluItem);
				}
                // reset the flags
                ExcluItem.dwFlags = 0;
            }

            // Find out if there was an error getting the exclusions
            if ( forceError ( IDS_Err_AP_Exclusions ))
            {
                MakeError(IDS_Err_AP_Exclusions,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
                return E_FAIL;
            }
            
            switch( status )
            {
            case SAVRT_OPTS_NO_MORE_ENTRIES: // This is what is expected
                break;
            default:
                // Error occured while getting the exclusion list
                MakeError(IDS_Err_AP_Exclusions,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
                return E_FAIL;
                break;
            }
        }
    }

    return S_OK;
}

HRESULT CNAVOptions::reloadAPExclusions(void)
{
    HRESULT hr;
    if( FAILED( hr = initializeAPExclusions() ) )
        return hr;

    // Load the values from AP
    if( APExclusionGoToListHead && APExclusionGetNext )
    {
        // Reset the list head
        APExclusionGoToListHead(m_hAPExclusions);

        // Clear our internal list
        m_vAPExclusions.clear();

        // First load up all of the excluded extensions from the savrt.dat file
        const TCHAR* pcszCurOptName = AP_ExclusionExt;
        APEXCLUSIONITEM ExcluItem;
        BYTE bufExtList[SAVRT_OPTS_MAX_BINARY_LENGTH];
        TCHAR szList[SAVRT_OPTS_MAX_BINARY_LENGTH] = {0};

        // Load the normal extensions then the compressed extensions
        for( int i=0; i<2; i++ )
        {
            // Initialize data
            ZeroMemory(bufExtList, SAVRT_OPTS_MAX_BINARY_LENGTH);
            ZeroMemory(szList, sizeof(szList));

            // Get the list from SavRT.dll
	        if( forceError (IDS_Err_Get_APExtensions) ||
                SAVRT_OPTS_OK != m_SavrtOptions.GetBinaryValue(pcszCurOptName, bufExtList, SAVRT_OPTS_MAX_BINARY_LENGTH) ) 
            {
                MakeError(IDS_Err_Get_APExtensions, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            if( FAILED(hr = BinaryToString(bufExtList, szList, SAVRT_OPTS_MAX_BINARY_LENGTH)) )
                // The BinaryToString function fills in the error information for us if it fails
                return hr;

            // Get the regular extension exclusions
            TCHAR *nextExt = NULL;

            // Get the first extension
            nextExt = _tcstok(szList,_T(" "));
            while( NULL != nextExt )
            {
                // Save this item
                _tcscpy(ExcluItem.szExclusion, _T("*."));
                _tcscat(ExcluItem.szExclusion, nextExt);
                ExcluItem.dwFlags = 0;

                // Save the flags
                if( i == 0 )
                    ExcluItem.dwFlags |= AP_EXC_VIRUS_FOUND;
                else
                    ExcluItem.dwFlags |= AP_EXC_VIRUS_FOUND_ARCHIVE;

                m_vAPExclusions.push_back(ExcluItem);

                // Get the next extension
                nextExt = _tcstok(NULL, _T(" "));
            }

            // Now load up the compressed extensions
            pcszCurOptName = AP_ArchiveExclusionExt;
        }

        // Get the paths
        SAVRT_OPTS_STATUS status = SAVRT_OPTS_OK;
        TCHAR* pszExludePath = NULL;

        // Populate our vector with the exclusion items
        while( SAVRT_OPTS_OK == (status = APExclusionGetNext(m_hAPExclusions, &pszExludePath, &ExcluItem.dwFlags)) )
        {
            // Save this item
            _tcscpy(ExcluItem.szExclusion, pszExludePath);
            m_vAPExclusions.push_back(ExcluItem);
        }

        // Find out if there was an error getting the exclusions
        if (forceError (IDS_Err_Get_APExtensions))
        {
            MakeError(IDS_Err_AP_Exclusions,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }
        
        switch( status )
        {
        case SAVRT_OPTS_NO_MORE_ENTRIES: // This is what is expected
            break;
        default:
            // Error occured while getting the exclusion list
            MakeError(IDS_Err_AP_Exclusions,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
            break;
        }

    }

    return S_OK;
}

// Takes the binaryData input and replaces all NULL values with spaces then
// stores the resulting string in pszStringData.
// binaryData must be double NULL terminated.
HRESULT CNAVOptions::BinaryToString(LPCBYTE binaryData, LPTSTR pszStringData, DWORD dwStringBufLen)
{
    if( forceError (ERR_INVALID_POINTER) ||
        !binaryData || !pszStringData )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }
    //
    // Change the NULL values to spaces
    //

	LPCTSTR pszCurrent = reinterpret_cast<LPCTSTR>(binaryData);

    // Initialize the string buffer to an empty string
    _tcscpy(pszStringData, _T(""));

    // Loop through all of the extension and store them in szList
    while(*pszCurrent)
    {
        // First make sure the buffer we've been given is big enough
        // to hold the next extension
        if( forceError (IDS_ERR_CONVERT_BIN_EXTLIST) ||
            (_tcslen(pszStringData)*sizeof(TCHAR))+(_tcslen(pszCurrent)*sizeof(TCHAR)) > dwStringBufLen )
        {
            MakeError(IDS_ERR_CONVERT_BIN_EXTLIST, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }

        // Put a copy of the current extension in the list
        _tcsncat(pszStringData, pszCurrent, MAX_PATH-1);

        // Add a space after it
        _tcscat(pszStringData, _T(" "));

        // Find the NULL delimeter. Walking a byte at a time since a NULL character is distinct
        // even on DBCS platforms
        while( *pszCurrent )
            pszCurrent++;

        // Walk past the null character...I've been assured that this will always
        // be a single character regardless of any DBCS nonesense.
        pszCurrent++;
    }

    // Get rid of the last space added to the list
    int nLastChar = _tcslen(pszStringData)-1;
    if( pszStringData[nLastChar] == _TCHAR(' ') )
        pszStringData[nLastChar] = _TCHAR('\0');

    return S_OK;
}

// Takes the pcszStringData input and replaces all spaces with NULL values
// then stores the output in binaryData with a double NULL terminator.
HRESULT CNAVOptions::StringToBinary(LPCTSTR pcszStringData, LPBYTE binaryData, DWORD &dwBinaryBufLen)
{
    // Get the buffer length needed for the binaryData, need two more for the 2 terminating NULL
    // values
    DWORD bufLen = (_tcslen(pcszStringData)*sizeof(TCHAR)) + 2;

    if( forceError (IDS_ERR_CONVERT_STR_EXTLIST) ||
        bufLen > dwBinaryBufLen )
    {
        MakeError(IDS_ERR_CONVERT_STR_EXTLIST, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // Clear the buffer
    ZeroMemory(binaryData, dwBinaryBufLen);

    // Assign the list to the buffer...don't forget the terminating NULL
    CopyMemory(binaryData, pcszStringData, bufLen);

    // The dwBinaryBufLen will now take on the actual buffer length that will be returned
    // to the caller
    dwBinaryBufLen = bufLen;

    // Walk the buffer replacing all spaces with NULLs and replacing the terminating NULL with a double
    // terminating NULL
    TCHAR* pbszCur = reinterpret_cast<TCHAR*>(binaryData);
    while( *pbszCur )
    {
        if( *pbszCur == _TCHAR(' ') )
        {
            *pbszCur = _TCHAR('\0');
            pbszCur++;
            continue;
        }
        pbszCur = CharNext(pbszCur);
    }

    // Now we're at the terminating NULL so add one more to it
    // although it should already be NULL due to the ZeroMemory
    *pbszCur = _TCHAR('\0');
    pbszCur++;
    *pbszCur = _TCHAR('\0');

    return S_OK;
}

STDMETHODIMP CNAVOptions::RemoveAPExclusionItem(long index)
{
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    return APExcl_RemoveItem(index, true);
}

HRESULT CNAVOptions::APExcl_RemoveItem(int nIndex, bool bRemoveInternal)
{
    HRESULT hr;
    APEXCLUSIONVECT::iterator it = NULL;

    // First ensure the list has been read in
    if( FAILED( hr = initializeAPExclusions() ) )
        return hr;

    // Make sure an Exclusion item exists at the index requested to commit
    if( forceError (ERR_INVALID_ARG) ||
        nIndex < 0 || nIndex > m_vAPExclusions.size() )
    {
        MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // If it's an extension exclusion remove it from the AP:ExclusionExt option
    if( IsExtensionExclusion( (m_vAPExclusions.at(nIndex)).szExclusion ) )
    {
        const TCHAR* pcszOptionName = NULL;
        TCHAR szList[SAVRT_OPTS_MAX_BINARY_LENGTH] = {0};

        if( (m_vAPExclusions.at(nIndex)).dwFlags & AP_EXC_VIRUS_FOUND_ARCHIVE )
            pcszOptionName = AP_ArchiveExclusionExt;
        else
            pcszOptionName = AP_ExclusionExt;

        if( FAILED(GetExtList(pcszOptionName,szList)) )
        {
            return E_FAIL;
        }

        // Remove the *. or . from the extension
        TCHAR *pszAfterDot = _tcschr( (m_vAPExclusions.at(nIndex)).szExclusion, _TCHAR('.'));
        pszAfterDot = CharNext(pszAfterDot);

        // Find the extension in the list
        TCHAR* pszExtToRemove = _tcsstr(szList, pszAfterDot);
        if( pszExtToRemove )
        {
            // Now get a pointer to the next extension in the list
            TCHAR* pszNextExt = pszExtToRemove;
            while( NULL != *(pszNextExt = CharNext(pszNextExt)) )
            {
                // Continue until a space is found
                if( *pszNextExt == _TCHAR(' ') )
                {
                    // Walk past the spcace to the next extension
                    pszNextExt = CharNext(pszNextExt);
                    break;
                }
            }
            // Remove the item from the list
            if( !*pszNextExt ) // In case it was the last item in the list
            {
                // If it's also the first item in the list don't go before the
                // start of the buffer!!!
                if( pszExtToRemove == szList )
                    *pszExtToRemove = _TCHAR('\0');
                else
                    *(pszExtToRemove-1) = _TCHAR('\0');
            }
            else
            {
                _tcscpy(pszExtToRemove, pszNextExt);
            }

            if( FAILED(SetExtList(pcszOptionName,szList)) )
            {
                return E_FAIL;
            }
        }
    }
    else
    {
        if( APExclusionModifyRemove )
        {
            if( forceError(IDS_ERR_DELETE_AP_EXCLUSIONITEM)
                || SAVRT_OPTS_OK != APExclusionModifyRemove(m_hAPExclusions, (m_vAPExclusions.at(nIndex)).szExclusion, (m_vAPExclusions.at(nIndex)).dwFlags) )
            {
                MakeError(IDS_ERR_DELETE_AP_EXCLUSIONITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }
        }
    }

    // Remove the internal one if asked to
    if( bRemoveInternal )
    {
        it = (m_vAPExclusions.begin() + nIndex);
        it = m_vAPExclusions.erase(it);
    }
    
	return S_OK;
}

STDMETHODIMP CNAVOptions::get_APExclusionItemPath(long index, BSTR *pVal)
{
    HRESULT hr;

    // Validate parameters
    //
    if( forceError (ERR_INVALID_POINTER) ||
        NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // First ensure the list has been read in
    if( FAILED( hr = initializeAPExclusions() ) )
        return hr;

    // Make sure an Exclusion item exists at the index requested
    if( forceError (ERR_INVALID_ARG) || index < 0 || index > m_vAPExclusions.size() )
    {
		MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Get Path
    CComBSTR bstrPath((m_vAPExclusions.at(index)).szExclusion);
    *pVal = bstrPath.Copy();

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_APExclusionItemSubFolders(long index, BOOL *pVal)
{
	HRESULT hr;

    // Validate parameters
    //
    if( forceError ( ERR_INVALID_POINTER ) ||
        NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // First ensure the list has been read in
    if( FAILED( hr = initializeAPExclusions() ) )
        return hr;

    // Make sure an Exclusion item exists at the index requested
    if( forceError ( ERR_INVALID_ARG ) || index < 0 || index > m_vAPExclusions.size() )
    {
		MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Get SubFolders flag
    *pVal = SAVRT_EXCL_SUBFOLDERS == ((m_vAPExclusions.at(index)).dwFlags & SAVRT_EXCL_SUBFOLDERS) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_APExclusionItemFlags(long index, long *pVal)
{
	HRESULT hr;

    // Validate parameters
    //
    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // First ensure the list has been read in
    if( FAILED( hr = initializeAPExclusions() ) )
        return hr;

    // Make sure an Exclusion item exists at the index requested
    if( forceError ( ERR_INVALID_ARG ) || index < 0 || index > m_vAPExclusions.size() )
    {
		MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Get loword flags
    *pVal = LOWORD( (m_vAPExclusions.at(index)).dwFlags );

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_IsExtensionExclusion(BSTR bstrPath, BOOL *pVal)
{
    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    if ( ::SysStringLen (bstrPath) > MAX_PATH )
        return E_POINTER;

    USES_CONVERSION;

	bool bIsExt = IsExtensionExclusion(OLE2T(bstrPath));

    if( bIsExt )
        *pVal = VARIANT_TRUE;
    else
        *pVal = VARIANT_FALSE;

	return S_OK;
}

// Retrieves AP:Ext binary value from the savrt.dat file and replace the NULL
// values with spaces then return the string
STDMETHODIMP CNAVOptions::get_APExtensionList(BSTR *pbstrList)
{
    HRESULT hr = S_OK;

    if( forceError (ERR_INVALID_POINTER) ||
        !pbstrList )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Initialize the out bstr
    *pbstrList = L"";

    // BUFFER used to get the list
    BYTE bufExtList[SAVRT_OPTS_MAX_BINARY_LENGTH];

    // Initialize the buffer
    ZeroMemory(bufExtList, SAVRT_OPTS_MAX_BINARY_LENGTH);

    // Get the list from SavRT.dll
	if( forceError (IDS_Err_Get_APExtensions) ||
        SAVRT_OPTS_OK != m_SavrtOptions.GetBinaryValue(AP_Ext, bufExtList, SAVRT_OPTS_MAX_BINARY_LENGTH) ) 
    {
        MakeError(IDS_Err_Get_APExtensions, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    TCHAR szList[SAVRT_OPTS_MAX_BINARY_LENGTH] = {0};
    if( FAILED( hr = BinaryToString(bufExtList, szList, SAVRT_OPTS_MAX_BINARY_LENGTH) ) )
        // The BinaryToString function fills in the error information for us if it fails
        return hr;

    // Assign the list back to the out variable
    _bstr_t tbstrList(szList);
    *pbstrList = tbstrList.copy();

    return S_OK;
}

// Replaces the spaces with NULL values in bstrList and store bstrList
// in the AP:Ext binary option in SAVRT.DAT
STDMETHODIMP CNAVOptions::put_APExtensionList(BSTR bstrList)
{
    HRESULT hr = S_OK;

    USES_CONVERSION;

    // Let's use TCHARs
    TCHAR szList[SAVRT_OPTS_MAX_BINARY_LENGTH] = {0};
    _tcsncpy(szList, OLE2T(bstrList), SAVRT_OPTS_MAX_BINARY_LENGTH -1); // Save room for the null terminator

    // The script checks the length of the list before allwoing the put
    // to occur but just to be extra carefully we'll do it here as well
    // Adding two to the buffer length to account for the 2 terminating NULLs
    DWORD bufLen = (_tcslen(szList)*sizeof(TCHAR)) + 2;
    if( forceError (IDS_Err_Put_APExtension) ||
        SAVRT_OPTS_MAX_BINARY_LENGTH < bufLen )
    {
        MakeError(IDS_Err_Put_APExtension, E_FAIL, IDS_NAVERROR_INTERNAL );
        return E_FAIL;
    }

    if( FAILED(SetExtList(AP_Ext, szList)) )
    {
		return E_FAIL;
    }

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_APExclusionCount(long *pNumExclusions)
{
    // Check for valid out parameter
    if( forceError (ERR_INVALID_POINTER) ||
        NULL == pNumExclusions )
    {    
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
		return E_POINTER;
    }

	// Return the number of exclusions currently in our list
    HRESULT hr;

    // First ensure the list has been read in
    if( FAILED( hr = initializeAPExclusions() ) )
        return hr;

    *pNumExclusions = m_vAPExclusions.size();

	return S_OK;
}

STDMETHODIMP CNAVOptions::DefaultAPExclusions()
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    HRESULT hr;

    // First ensure the list has been read in
    if( FAILED( hr = initializeAPExclusions() ) )
        return hr;

    if( APExclusionSetDefault )
    {
        if( forceError (IDS_Err_Set_AP_Default_Exclusions) ||
            SAVRT_OPTS_OK != APExclusionSetDefault(m_hAPExclusions) )
        {
            MakeError (IDS_Err_Set_AP_Default_Exclusions, E_FAIL, IDS_NAVERROR_INTERNAL);
		    return E_FAIL;
        }
    }

    // Now reload the list
    if( FAILED( hr = reloadAPExclusions() ) )
        return hr;

	return S_OK;
}

STDMETHODIMP CNAVOptions::SetAPExclusionItem(long index, BSTR bstrPath, BOOL bSubFolders, long Flags, EExclusionAddResult *peResult)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if ( forceError ( ERR_ADD_EXCLUSION ))
    {
		MakeError (ERR_ADD_EXCLUSION, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

	HRESULT hr = S_OK;
    bool bRet = false;

	if (forceError (ERR_INVALID_POINTER) ||
        !peResult)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
		return E_POINTER;
    }

    // First ensure the list has been read in
    if( FAILED( hr = initializeAPExclusions() ) )
        return hr;

    // Make sure an Exclusion item exists at the index requested to commit
    if( forceError (ERR_INVALID_ARG) ||
        index < 0 || index > m_vAPExclusions.size()+1 )
    {
        MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Initialize return value
    *peResult = ExclusionAddResult_Failed;

    // Create a new exclusion item to be added to the list
    USES_CONVERSION;
    APEXCLUSIONITEM NewExclusion;
    NewExclusion.dwFlags = 0;

    // Set the exclusion path and flags
    _tcscpy(NewExclusion.szExclusion, OLE2T(bstrPath));
    NewExclusion.dwFlags = Flags;

    // Get the type of exclusion it is
    bool bIsExtExclusion = IsExtensionExclusion(NewExclusion.szExclusion);

    // For path exclusions set the subfolder flag
    if( !bIsExtExclusion && bSubFolders )
        NewExclusion.dwFlags |= SAVRT_EXCL_SUBFOLDERS;

    // If it's a new exclusion just add it
    if( m_vAPExclusions.size()+1 == index )
    {
        if( bIsExtExclusion )
            return APExcl_AddNewExt(NewExclusion, peResult, true);
        else
            return APExcl_AddNewPath(NewExclusion, peResult, true);
    }
    else // A replacement exclusion, fun stuff
    {
        // Get the type of exclusion being replaced
        bool bOldIsExt = IsExtensionExclusion((m_vAPExclusions.at(index)).szExclusion);

        if( bIsExtExclusion ) // The new item is an extension exclusion
        {
            if( bOldIsExt ) // Extension replacing an extension
                return APExcl_ReplaceExtWithExt(index, NewExclusion, peResult);
            else // Extension replacing a path
                return APExcl_ReplacePathWithExt(index, NewExclusion, peResult);
        }
        else // The new item is a path exclusion
        {
            if( bOldIsExt ) // Path replacing an extension
                return APExcl_ReplaceExtWithPath(index, NewExclusion, peResult);
            else // Path replacing a path
                return APExcl_ReplacePathWithPath(index, NewExclusion, peResult);
        }
    }
}

HRESULT CNAVOptions::APExcl_ReplacePathWithPath(int nIndexToReplace, APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult)
{
    *peResult = ExclusionAddResult_Failed;
    APEXCLUSIONVECT::iterator it = NULL;

    if( APExclusionModifyEdit )
    {
        // Replacing an existing exclusion
        switch( APExclusionModifyEdit( m_hAPExclusions // Handle to the Exclusions
                                                ,(m_vAPExclusions.at(nIndexToReplace)).szExclusion // Old path
                                                ,(m_vAPExclusions.at(nIndexToReplace)).dwFlags // Old flags
                                                ,NewExclusion.szExclusion // New path
                                                ,NewExclusion.dwFlags ) ) // New flags
        {
        case SAVRT_OPTS_ALREADY_EXISTS:
            *peResult = ExclusionAddResult_Duplicate;
		    return S_FALSE;

        case SAVRT_OPTS_OK:
            *peResult = ExclusionAddResult_New;
            // Remove the old item from our internal vector
            it = (m_vAPExclusions.begin() + nIndexToReplace);
            it = m_vAPExclusions.erase(it);
            // Insert the new one where the old one 
		    m_vAPExclusions.insert(it, NewExclusion);
            return S_OK;
        
        default:
            // Error Adding exclusion
			MakeError (ERR_ADD_EXCLUSION, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }
    else
        return E_FAIL;

    *peResult = ExclusionAddResult_New;
    return S_OK;
}

HRESULT CNAVOptions::APExcl_ReplacePathWithExt(int nIndexToReplace, APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult)
{
    *peResult = ExclusionAddResult_Failed;

    if( APExclusionModifyRemove )
    {
        // First attempt to add the extension in case it is a duplicate,
        // don't add it to the internal list since it is a replacement item
        if( FAILED(APExcl_AddNewExt(NewExclusion,peResult,false)) )
        {
            MakeError (ERR_ADD_EXCLUSION, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
        else if( *peResult == ExclusionAddResult_Duplicate )
        {
            // Return duplicate exclusion
            return S_FALSE;
        }

        // Now remove the path
        if( SAVRT_OPTS_OK != APExclusionModifyRemove(m_hAPExclusions, (m_vAPExclusions.at(nIndexToReplace)).szExclusion, (m_vAPExclusions.at(nIndexToReplace)).dwFlags) )
        {
            MakeError(IDS_ERR_DELETE_AP_EXCLUSIONITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }

        // Update the correct internal storage item
        APEXCLUSIONVECT::iterator it = NULL;
        it = (m_vAPExclusions.begin() + nIndexToReplace);
        it = m_vAPExclusions.erase(it);
		m_vAPExclusions.insert(it, NewExclusion);
    }
    else
        return E_FAIL;

    *peResult = ExclusionAddResult_New;
    return S_OK;
}

HRESULT CNAVOptions::APExcl_ReplaceExtWithExt(int nIndexToReplace, APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult)
{
    *peResult = ExclusionAddResult_Failed;
    HRESULT hr = S_OK;

    // Save the original extension in case it needs to be restored
    APEXCLUSIONITEM OrigExclusion;
    _tcscpy(OrigExclusion.szExclusion, (m_vAPExclusions.at(nIndexToReplace)).szExclusion);
    OrigExclusion.dwFlags = (m_vAPExclusions.at(nIndexToReplace)).dwFlags;

    // Remove the original extension from the real list, leave the internal
    // list intact since it is a replacement
    if( FAILED( hr = APExcl_RemoveItem(nIndexToReplace,false) ) )
        return hr;

    // Add the new one, but not to the internal list
    if( FAILED( hr = APExcl_AddNewExt(NewExclusion,peResult,false) ) )
    {
        // Attempt to restore the original
        APExcl_AddNewExt(OrigExclusion,peResult,false);
        *peResult = ExclusionAddResult_Failed;
        return hr;
    }
    else if( *peResult == ExclusionAddResult_Duplicate )
    {
        // Attempt to restore the original
        APExcl_AddNewExt(OrigExclusion,peResult,false);
        *peResult = ExclusionAddResult_Duplicate;
        return S_FALSE;
    }

    // Replace the internal item
    APEXCLUSIONVECT::iterator it = NULL;
    it = (m_vAPExclusions.begin() + nIndexToReplace);
    it = m_vAPExclusions.erase(it);
	m_vAPExclusions.insert(it, NewExclusion);

    *peResult = ExclusionAddResult_New;
    return S_OK;
}

HRESULT CNAVOptions::APExcl_ReplaceExtWithPath(int nIndexToReplace, APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult)
{
    HRESULT hr = S_OK;
    *peResult = ExclusionAddResult_Failed;

    // First attempt to add the path, but not to the internal list since it is
    // a replacement item
    if( FAILED( hr = APExcl_AddNewPath(NewExclusion, peResult, false) ) )
    {
        return hr;
    }
    else if( *peResult == ExclusionAddResult_Duplicate )
    {
        // Duplicate item, don't allow replacement
        return S_FALSE;
    }

    // Now remove the extension
    if( FAILED(hr = APExcl_RemoveItem(nIndexToReplace,false)) )
    {
        *peResult = ExclusionAddResult_Failed;
        return hr;
    }

    // Update the internal list
    APEXCLUSIONVECT::iterator it = NULL;
    it = (m_vAPExclusions.begin() + nIndexToReplace);
    it = m_vAPExclusions.erase(it);
	m_vAPExclusions.insert(it, NewExclusion);

    *peResult = ExclusionAddResult_New;
    return S_OK;
}

HRESULT CNAVOptions::APExcl_AddNewExt(APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult, bool bAddToInternalList)
{
    if( forceError(IDS_Err_Set_APExclusionExt) )
    {
        MakeError(IDS_Err_Set_APExclusionExt, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    *peResult = ExclusionAddResult_Failed;

    HRESULT hr = S_OK;
    TCHAR szList[SAVRT_OPTS_MAX_BINARY_LENGTH] = {0};
    const TCHAR* pcszOptName = NULL;

    // Remove the *. or . from the extension
    TCHAR *pszAfterDot = _tcschr(NewExclusion.szExclusion, _TCHAR('.'));
    pszAfterDot = CharNext(pszAfterDot);

    // Make sure it doesn't already exist in either list (compressed or normal)
    pcszOptName = AP_ExclusionExt;

    // Load the normal extensions then the compressed extensions
    for( int i=0; i<2; i++ )
    {
        if( FAILED(GetExtList(pcszOptName, szList)) )
        {
            return E_FAIL;
        }

        // Look for this item
        TCHAR *nextExt = _tcstok(szList,_T(" "));
        while( NULL != nextExt )
        {
            if( 0 == _tcscmp(nextExt, pszAfterDot) )
            {
                // Already have this extension excluded
				*peResult = ExclusionAddResult_Duplicate;
				return S_FALSE;
            }
            nextExt = _tcstok(NULL,_T(" "));
        }

        // Now load up the compressed extensions
        pcszOptName = AP_ArchiveExclusionExt;
    }

    //
    // Add the extension to the correct list
    //
    // Get the correct option name for the list to add the extension to
    if( NewExclusion.dwFlags & AP_EXC_VIRUS_FOUND_ARCHIVE )
        pcszOptName = AP_ArchiveExclusionExt;
    else
        pcszOptName = AP_ExclusionExt;

    if( FAILED(GetExtList(pcszOptName, szList)) )
    {
        return E_FAIL;
    }

    // Add the extension to the list
    if( 0 == _tcslen(szList) )
        _tcscat(szList, pszAfterDot);
    else
        _stprintf(szList,_T("%s %s"), szList, pszAfterDot);

    if( FAILED(SetExtList(pcszOptName, szList)) )
    {
        return E_FAIL;
    }

    // Put the new exclusion in our internal vector
    if( bAddToInternalList )
    {
        m_vAPExclusions.push_back(NewExclusion);
    }
    *peResult = ExclusionAddResult_New;

    
    return S_OK;
}

HRESULT CNAVOptions::APExcl_AddNewPath(APEXCLUSIONITEM NewExclusion, EExclusionAddResult *peResult, bool bAddToInternalList)
{
    *peResult = ExclusionAddResult_Failed;

    if( APExclusionModifyAdd )
    {
        // Attempt to add the new exclusion to the savrt exclusions
        switch( APExclusionModifyAdd( m_hAPExclusions, NewExclusion.szExclusion, NewExclusion.dwFlags ) )
        {
        case SAVRT_OPTS_ALREADY_EXISTS:
            *peResult = ExclusionAddResult_Duplicate;
		    return S_FALSE;
        case SAVRT_OPTS_OK:
            *peResult = ExclusionAddResult_New;
            if( bAddToInternalList )
		        m_vAPExclusions.push_back(NewExclusion);
            return S_OK;
        case SAVRT_OPTS_MAX_EXCLUSIONS_EXCEEDED:
            MakeOptionsError(IDS_ERR_AP_EXCLUSION_MAX, E_FAIL);
            return E_FAIL;
        default:
            // Error Adding exclusion
			MakeError (ERR_ADD_EXCLUSION, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }
    else
        return E_FAIL;
}

HRESULT CNAVOptions::GetExtList(LPCTSTR pcszOptName, LPTSTR pszList)
{
    HRESULT hr = S_OK;

    if( !pcszOptName || !pszList )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
		return E_POINTER;
    }

    BYTE bufExtList[SAVRT_OPTS_MAX_BINARY_LENGTH];
    ZeroMemory(bufExtList, SAVRT_OPTS_MAX_BINARY_LENGTH);
    ZeroMemory(pszList, sizeof(pszList));

	if( SAVRT_OPTS_OK != m_SavrtOptions.GetBinaryValue(pcszOptName, bufExtList, SAVRT_OPTS_MAX_BINARY_LENGTH) ) 
    {
        MakeError(IDS_Err_Get_APExclusionExt, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // Convert the list to string data
    if( FAILED(hr = BinaryToString(bufExtList, pszList, SAVRT_OPTS_MAX_BINARY_LENGTH)) )
        // The BinaryToString function fills in the error information for us if it fails
        return hr;

    return S_OK;
}

HRESULT CNAVOptions::SetExtList(LPCTSTR pcszOptName, LPTSTR pszList)
{
    HRESULT hr = S_OK;

    if( !pcszOptName || !pszList )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
		return E_POINTER;
    }

    BYTE bufExtList[SAVRT_OPTS_MAX_BINARY_LENGTH];
    ZeroMemory(bufExtList, SAVRT_OPTS_MAX_BINARY_LENGTH);

    // Make sure the list isn't too big
    if( forceError ( IDS_ERR_AP_EXCLUSIONEXT_OVERFLOW )  ||
        _tcslen(pszList)*sizeof(TCHAR) > SAVRT_OPTS_MAX_BINARY_LENGTH )
    {
        MakeOptionsError(IDS_ERR_AP_EXCLUSION_MAX, E_FAIL);
        return E_FAIL;
    }

    DWORD dwBufSize = SAVRT_OPTS_MAX_BINARY_LENGTH;
    
    // Convert the new list to binary format
    if( FAILED( hr = StringToBinary(pszList, bufExtList, dwBufSize) ) )
        // StringToBinary fills out the NAVError object when it fails
        return hr;

    // Write the new binary formatted list out
    if( SAVRT_OPTS_OK != m_SavrtOptions.SetBinaryValue(pcszOptName, bufExtList, dwBufSize) )
    {
        if( 0 == _tcscmp(AP_ArchiveExclusionExt, pcszOptName) || 0 == _tcscmp(AP_ExclusionExt, pcszOptName) )
            MakeError( IDS_Err_Set_APExclusionExt, E_FAIL, IDS_NAVERROR_INTERNAL );
        else
            MakeError( IDS_Err_Put_APExtension, E_FAIL, IDS_NAVERROR_INTERNAL );
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP CNAVOptions::IsExclusionsDifferent(/*[in]*/ BSTR bstrExclusion1, /*[in]*/ BSTR bstrExclusion2, /*[out, retval]*/ BOOL *pbIsExclusionsDifferent)
{
    USES_CONVERSION;
    TCHAR szSFN1[MAX_PATH]
    , szSFN2[MAX_PATH] = {0};

    if (forceError (ERR_INVALID_POINTER) ||
        !pbIsExclusionsDifferent)
    {
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Default to the exclusions being different
    *pbIsExclusionsDifferent = VARIANT_TRUE;

    if ( ::SysStringLen ( bstrExclusion1) > MAX_PATH ||
         ::SysStringLen ( bstrExclusion2) > MAX_PATH )
        return S_FALSE;

    // Q: Is this the same BSTR?
    if (bstrExclusion1 == bstrExclusion2)
        return S_OK;

    // Q: Is one exclusion NULL and the other is not NULL?
    if (bstrExclusion1 && !bstrExclusion2 || bstrExclusion2 && !bstrExclusion1)
        return S_OK;

    // Do simple comparison first
    _ASSERT(bstrExclusion1 && bstrExclusion2);
    if (::wcsicmp(bstrExclusion1, bstrExclusion2) == 0)
    {
        *pbIsExclusionsDifferent = VARIANT_FALSE;
        return S_OK;
    }

    // Convert both exclusions to short file names
    DWORD dwRet = GetShortPathName(W2T(bstrExclusion1), szSFN1, MAX_PATH-1);
    if ( dwRet == 0 || dwRet >= MAX_PATH )
        ::_tcsncpy(szSFN1, W2T(bstrExclusion1), MAX_PATH-1 );   // Must leave room for the null terminator

    dwRet = GetShortPathName(W2T(bstrExclusion2), szSFN2, MAX_PATH-1);
    if ( dwRet == 0 || dwRet >= MAX_PATH )
        ::_tcsncpy(szSFN2, W2T(bstrExclusion2), MAX_PATH-1 );   // Must leave room for the null terminator

    // Compare the short file names
    if (::_tcsicmp(szSFN1, szSFN2) == 0)
        *pbIsExclusionsDifferent = VARIANT_FALSE;

    return S_OK;
}
