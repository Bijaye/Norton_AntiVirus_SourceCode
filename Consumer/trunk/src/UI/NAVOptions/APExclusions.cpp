////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAV Options AP exclusions integration for AP10

#include "stdafx.h"
#include "NAVOptionsObj.h"
#include "..\navoptionsres\resource.h"
#include <algorithm>

HRESULT CNAVOptions::ClearAPExclusions()
{
	PAPEXCLUSIONITEM temp;
	APExclusionList::iterator it;
	for(it = m_APExclusions.begin(); it != m_APExclusions.end(); it++)
	{
		temp = *it;
		delete[] temp->szExclusion;
		delete temp;
	}
	m_APExclusions.clear();
	return S_OK;
}

// Load exclusions and extension exclusions. Initialize both lists.
HRESULT CNAVOptions::LoadAPExclusions(bool bReload /* = false */)
{
	HRESULT hr;

	if(!m_spAPExclusions)
	{
		SYMRESULT sr;
		if(SYM_FAILED(sr = m_APExclusionLoader.CreateObject(&m_spAPExclusions)))
		{
			CCTRCTXE1(_T("Error loading AP Exclusions interface. SR = 0x%08x"), sr);
			return E_FAIL;
		}

		if(FAILED(hr = m_spAPExclusions->Load()))
		{
			CCTRCTXE1(_T("AP Exclusions failed to load. HR = 0x%08x"), hr);
			return hr;
		}

		m_bAPExclusionsLoaded = false;
	}

	if(!m_bAPExclusionsLoaded || bReload)
	{
		IEnumSrtExclusionsPtr spExcl;
		PAPEXCLUSIONITEM temp;
		DWORD dwCount, dwLength;

		if(FAILED(hr = m_spAPExclusions->GetMaxExclusionLengthWchars(m_dwAPExclusionMaxLength)))
		{
			CCTRCTXE1(_T("Failed to get AP exclusion max length. HR = 0x%08x"), hr);
			return hr;
		}

		if(FAILED(hr = m_spAPExclusions->EnumExclusions(&spExcl)))
		{
			CCTRCTXE1(_T("Failed to enumerate AP exclusions. HR = 0x%08x"), hr);
			return hr;
		}

		if(FAILED(hr = spExcl->GetCount(dwCount)))
		{
			CCTRCTXE1(_T("Failed to get count of AP exclusions. HR = 0x%08x"), hr);
			return hr;
		}

		this->ClearAPExclusions();

		for(DWORD dwi = 0; dwi < dwCount; dwi++)
		{
			temp = new(std::nothrow)APEXCLUSIONITEM;
			if(!temp)
			{
				ClearAPExclusions();
				CCTRCTXE0(_T("Failed to allocate new AP Exclusion Item."));
				return E_OUTOFMEMORY;
			}
			
			dwLength = 0;
			hr = spExcl->GetExclusion(dwi, NULL, dwLength, temp->dwFlags);
			if(FAILED(hr) && hr != E_SRT_INSUFFICIENTBUFFER)
			{
				delete temp;
				ClearAPExclusions();
				CCTRCTXE1(_T("Error getting length of AP exclusion. HR = 0x%08x"), hr);
				return hr;
			}

			temp->szExclusion = new(std::nothrow) WCHAR[dwLength];
			if(!temp->szExclusion)
			{
				delete temp;
				ClearAPExclusions();
				CCTRCTXE0(_T("Could not allocate buffer for AP Exclusion."));
				return E_OUTOFMEMORY;
			}

			if(FAILED(hr = spExcl->GetExclusion(dwi, temp->szExclusion, dwLength, temp->dwFlags)))
			{
				delete temp->szExclusion;
				delete temp;
				ClearAPExclusions();
				CCTRCTXE1(_T("Error while loading AP exclusion. HR = 0x0%08x"), hr);
				return hr;
			}

			m_APExclusions.push_back(temp);
		}

		if((FAILED(hr = LoadExtensions())))
		{
			CCTRCTXE0(_T("Error loading extension exclusions."));
			return hr;
		}

		m_bAPExclusionsLoaded = true;
	}

	return S_OK;
}

HRESULT CNAVOptions::UnloadAPExclusions()
{
	HRESULT hr;

	if(FAILED(hr = this->ClearAPExclusions()))
	{
		CCTRCTXE1(_T("Error clearing AP exclusion vector. HR = 0x%08x"), hr);
		return hr;
	}

	if(m_spAPExclusions)
	{
		m_spAPExclusions.Release();
	}
	else
		CCTRCTXW0(_T("No exclusions interface to unload!"));

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_APExclusionCount(unsigned long* pVal)
{
	HRESULT hr;
	if(!pVal)
	{
		return E_INVALIDARG;
	}

	if(!m_spAPExclusions || !m_bAPExclusionsLoaded)
	{
		if(FAILED(hr = LoadAPExclusions(false)))
		{
			CCTRCTXE1(_T("Failed to load AP Exclusions. HR = 0x%08x"), hr);
			return hr;
		}
	}

	*pVal = m_APExclusions.size() + m_APExtensions.GetSize();
	return S_OK;
}

STDMETHODIMP CNAVOptions::get_APExclusionPath(long index, BSTR* pVal)
{
	long lExtIndex;
	HRESULT hr;

	if(index < 0 || !pVal)
		return E_INVALIDARG;

	if(!m_spAPExclusions || !m_bAPExclusionsLoaded)
	{
		if(FAILED(hr = LoadAPExclusions(false)))
		{
			CCTRCTXE1(_T("Failed to load AP Exclusions. HR = 0x%08x"), hr);
			return hr;
		}
	}

	if((size_t)index > (m_APExclusions.size() + m_APExtensions.GetSize() - 1))
	{
		CCTRCTXE0(_T("Index out of range."));
		return E_INVALIDARG;
	}

	this->GetExtensionIndex(index, lExtIndex);

	if(lExtIndex >= 0)
	{	// Extension exclusion - Prepend the *. for display purposes
		// ~~~TBD~~~ What's the purpose of this next line???
		this->GetExtensionIndex(index, lExtIndex);
		CStringW cswResult(_T("*."));
		cswResult.Append(m_APExtensions[lExtIndex]);
		*pVal = cswResult.AllocSysString();
	}
	else
	{
		*pVal = ::SysAllocString(m_APExclusions[index]->szExclusion);
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_APExclusionSubFolder(long index, BOOL* pVal)
{
	HRESULT hr;

	if(index < 0 || !pVal)
		return E_INVALIDARG;

	if(!m_spAPExclusions || !m_bAPExclusionsLoaded)
	{
		if(FAILED(hr = LoadAPExclusions(false)))
		{
			CCTRCTXE1(_T("Failed to load AP Exclusions. HR = 0x%08x"), hr);
			return hr;
		}
	}

	if((size_t)index >= m_APExclusions.size())
	{	// Out of range or extension exclusion. Just return false.
		*pVal = VARIANT_FALSE;
	}
	else
	{
		*pVal = ((m_APExclusions[index]->dwFlags & SRT_EXCL_SUBFOLDERS) 
					== SRT_EXCL_SUBFOLDERS) ? VARIANT_TRUE:VARIANT_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::AddAPExclusion(BSTR bstrPath, BOOL bSubFolders, EExclusionAddResult* ulSuccess)
{
	HRESULT hr;
	PAPEXCLUSIONITEM temp;
	bool bExtension;

	if(!bstrPath || !ulSuccess)
		return E_INVALIDARG;

	// ~~~TBD~~~ What's the purpose of this right here?  If it is required regardless
	// of the result, then it should be above everything, otherwise we are returning
	// E_INVALIDARG incorrectly.  If it is required only for result of S_OK, then it
	// is useless, since it is overwritten with a different value later.
	*ulSuccess = ExclusionAddResult_Failed;

	if(!m_spAPExclusions || !m_bAPExclusionsLoaded)
	{
		if(FAILED(hr = LoadAPExclusions(false)))
		{
			CCTRCTXE1(_T("Failed to load AP Exclusions. HR = 0x%08x"), hr);
			return hr;
		}
	}

	if(FAILED(hr = IsExtensionExclusion(bstrPath, bExtension)))
	{
		CCTRCTXE1(_T("Error calling IsExtensionExclusion. HR = 0x%08x"), hr);
		return hr;
	}

	if(bExtension)
	{
		CStringW cswPath(bstrPath);
        if(wcsnicmp(cswPath.GetString(), L"*.", 2) == 0)
        {
            cswPath = cswPath.Right(cswPath.GetLength() - 2);
        }
		hr = m_APExtensions.Add(cswPath);

		if(FAILED(hr))
			CCTRCTXE1(_T("Error adding AP extension exclusion. HR = 0x%08x"), hr);
		else if(hr == S_FALSE)
			*ulSuccess = ExclusionAddResult_Duplicate;
		else
			*ulSuccess = ExclusionAddResult_New;

		return hr;
	}

	// Is it too long?
	if(wcslen(bstrPath) > m_dwAPExclusionMaxLength)
	{
		CCTRCTXE0(_T("AP Exclusion too long!"));
		return E_FAIL;
	}

	APExclusionList::iterator it = m_APExclusions.begin();
	while(it != m_APExclusions.end())
	{
		// Dupe!
		if(wcsicmp(bstrPath, (*it)->szExclusion) == 0)
		{
			*ulSuccess = ExclusionAddResult_Duplicate;
			return S_OK;
		}
		it++;
	}

	// Not a dupe. Add it. May need to add section to construct flags later
	// if we ever have more flags than subfolders.
	hr = m_spAPExclusions->AddExclusion(bstrPath, wcslen(bstrPath), (bSubFolders == TRUE) ? SRT_EXCL_SUBFOLDERS:0);
	if(FAILED(hr))
	{
		switch(hr)
		{
		case E_SRT_MAXEXCEEDED:
			*ulSuccess = ExclusionAddResult_TooMany;
			// ~~~TBD~~~ This should have a break;... right? But then it wouldn't have proper handling in the JS, so it doesn't really matter...
		case E_SRT_INVALIDEXCLUSION:
			*ulSuccess = ExclusionAddResult_Failed;
			break;
		default:
			return hr;
		}
		return S_OK;
	}

	// Success! Add it.
	temp = new(std::nothrow)APEXCLUSIONITEM;
	if(!temp)
	{
		CCTRCTXE0(_T("Memory allocation error when creating new exclusion item."));
		return E_OUTOFMEMORY;
	}

	temp->szExclusion = new(std::nothrow) WCHAR[wcslen(bstrPath)+1];
	if(!temp->szExclusion)
	{
		delete temp;
		CCTRCTXE0(_T("Memory allocation error when trying to save exclusion path."));
		return E_OUTOFMEMORY;
	}

	wcsncpy(temp->szExclusion, bstrPath,wcslen(bstrPath)+1);
	temp->dwFlags = (bSubFolders == TRUE) ? SRT_EXCL_SUBFOLDERS:0;
	m_APExclusions.push_back(temp);
	*ulSuccess = ExclusionAddResult_New;
	return S_OK;
}

// EditAPExclusion() note: the following values are returned at plResult upon
// S_OK: <0 for an error, where -1 is reserved for reporting dupes; -2 is
// reserved for soft errors to mimic ExclusionAddResult_Failed on the JS side;
// (other codes may be added as long as someone cares); >=0 for success, where
// the value is the index of the successfully edited exclusion, which may NOT
// be any longer the same as the original index supplied at lIndex. If the
// index has changed, then all items between lIndex and *plResult have been
// shifted appropriately.  Shifting can happen both forward and backward.
// ... this should have been a simple thing ... yet it is so complex
STDMETHODIMP CNAVOptions::EditAPExclusion(long lIndex, BSTR bstrPath, BOOL bSubFolders, long* plResult)
{
    HRESULT hr;

    // ~~~TBD~~~ What's the usefulness of this index validation if LoadAPExclusions()
    // may end up getting called below?  Either this is done improperly, or
    // LoadAPExclusions() never kicks in, or we rely on LoadAPExclusions() to kick in
    // with a no-op effect.  Either way, something is fishy, but I can't wrap my mind
    // around the whole thing for an immediate fix, and most importantly, there is no
    // defect reported against it.
    if(lIndex < 0 || (size_t)lIndex > (m_APExclusions.size() + m_APExtensions.GetSize() - 1))
    {
        CCTRCTXE1(_T("Index [%d] out of bounds. "), lIndex);
        return E_INVALIDARG;
    }

    if(!bstrPath || !plResult)
        return E_INVALIDARG;

    // ~~~TBD~~~ What's the purpose of this right here?  If it is required regardless
    // of the result, then it should be above everything, otherwise we are returning
    // E_INVALIDARG incorrectly.  If it is required only for result of S_OK, then it
    // is useless, since it is overwritten with a different value later.
    *plResult = -2;

    // ~~~TBD~~~ Why is this here and not above the index validation?  See comments
    // for index validation for explanation of the logic.
    if(!m_spAPExclusions || !m_bAPExclusionsLoaded)
    {
        if(FAILED(hr = LoadAPExclusions(false)))
        {
            CCTRCTXE1(_T("Failed to load AP Exclusions. HR = 0x%08x"), hr);
            return hr;
        }
    }

    // Determine whether the original exclusion is an extension or a path, and get
    // its extension index.
    long lIndexExt;
    if(FAILED(hr = this->GetExtensionIndex(lIndex, lIndexExt)))
        return hr;

    // Determine whether the edited exclusion is an extension or a path.
    bool bExtension;
    if(FAILED(hr = IsExtensionExclusion(bstrPath, bExtension)))
        return hr;

    // Take two very different routes depending on whether the edited exclusion is
    // an extension or a path.
    long lIndexDupe;
    if(!bExtension)
    {
        // Process path exclusion.

        // If path exclusion is too long, then bail out.
        if(wcslen(bstrPath) > m_dwAPExclusionMaxLength)
            return E_FAIL;

        // Check for a dupe, retrieving dupe's index.
        APExclusionList::iterator it;
        for(it = m_APExclusions.begin(), lIndexDupe = 0;
            (it != m_APExclusions.end()) || !(lIndexDupe = -1);
            it++, lIndexDupe += 1)
            if(!wcsicmp(bstrPath, (*it)->szExclusion))
                break;

        // Split processing even further depending on whether we have edited an extension
        // exclusion into a path exclusion or we have stayed within path exclusions only.
        PAPEXCLUSIONITEM temp;
        if(lIndexExt >= 0)
        {
            // Process editing of an extension exclusion into a path exclusion.

            if(lIndexDupe >= 0)
            {
                // We have a dupe, so the verdict is clear.
                *plResult = -1;
            }
            else
            {
                // We don't have a dupe, so shuffle things around.

                // Allocate a new path exclusion entry (APEXCLUSIONITEM) and insert it in our
                // array of exclusion entries.  This is done first because it is easier to undo
                // should it become necessary.
                // ~~~TBD~~~ Isn't wcsncpy() an overkill down there?
                // ~~~TBD~~~ Couldn't push_back() throw? Why isn't it being handled anywhere?
                if((temp = new(std::nothrow)APEXCLUSIONITEM) != NULL)
                    temp->szExclusion = new(std::nothrow)WCHAR[wcslen(bstrPath)+1];
                if(!temp || !temp->szExclusion)
                {
                    delete temp;
                    return E_OUTOFMEMORY;
                }
                wcsncpy(temp->szExclusion, bstrPath, wcslen(bstrPath)+1);
                temp->dwFlags = (bSubFolders == TRUE) ? SRT_EXCL_SUBFOLDERS:0;
                m_APExclusions.push_back(temp);

                // Now add it to the AP path exclusions.
                if(FAILED(hr = m_spAPExclusions->AddExclusion(bstrPath, wcslen(bstrPath), (bSubFolders == TRUE) ? SRT_EXCL_SUBFOLDERS:0)))
                {
                    // Clean up.
                    m_APExclusions.pop_back();
                    delete[] temp->szExclusion;
                    delete temp;

                    // Copy AddAPExclusion()'s error reporting... including its bugs.
                    switch(hr)
                    {
                    case E_SRT_MAXEXCEEDED:
                    case E_SRT_INVALIDEXCLUSION:
                        *plResult = -2;
                        break;
                    default:
                        return hr;
                    }
                }
                else
                {
                    // Delete the old exclusion. This should succeed. If it does not, we're in big
                    // trouble.
                    if(FAILED(hr = DeleteAPExclusion(lIndex+1)))
                        CCTRCTXE1(_T("Error deleting AP extension exclusion during edit. HR = 0x%08x"), hr);

                    // The resulting index is the last path item, as per the way the "combined"
                    // exclusions array is represented in the UI.
                    *plResult = m_APExclusions.size() - 1;
                }
            }
        }
        else
        {
            // Process editing of a path exclusion into a path exclusion.

            if(lIndexDupe >= 0 && lIndexDupe != lIndex)
            {
                // We have a dupe, which is not the original item.  This is a perfectly valid
                // reason to complain.
                *plResult = -1;
            }
            else
            {
                // We have a dupe, which is our original item, or we have a completely new path
                // exclusion to replace the original item.  Either way, because the AP
                // exclusions do not allow editing of an existing item, we have to first remove
                // the original exclusion, then add a new one.  This will cause major problems
                // in case adding fails.  Our best option for gracefully handling such failures
                // is by having at least our UI own arrays updated to match what the user has
                // requested, and leaving the AP exclusions out of sync for that particular
                // item.  It is unfortunate that the user will not understand the exclusion is
                // really missing until a threat is intercepted where the user expected to not
                // be intercepted.  This is still better than our previous behavior, where the
                // UI showed the exclusion there, but crashing once a user attempted to add/
                // edit anything after it.

                // Allocate memory for replacing the exclusion path of the already existing
                // APEXCLUSIONITEM entry.  If this fails, we can still gracefully fail the
                // edit.
                WCHAR* psztemp;
                if((psztemp = new(std::nothrow)WCHAR[wcslen(bstrPath)+1]) != NULL)
                {
                    // Prepare the new path.
                    // ~~~TBD~~~ Isn't wcsncpy() an overkill?
                    wcsncpy(psztemp, bstrPath, wcslen(bstrPath)+1);

                    // Delete the original AP path exclusion.  We will handle errors silently, just
                    // like the rest of this code.
                    temp = m_APExclusions[lIndex];
                    if(FAILED(hr = m_spAPExclusions->DeleteExclusion(temp->szExclusion, wcslen(temp->szExclusion))))
                        CCTRCTXE1(_T("Error deleting AP path exclusion during edit. HR = 0x%08x"), hr);

                    // Add the edited AP path exclusion.  If this fails, we're entering dangerous
                    // waters.
                    if(FAILED(hr = m_spAPExclusions->AddExclusion(psztemp, wcslen(psztemp), (bSubFolders == TRUE) ? SRT_EXCL_SUBFOLDERS:0)))
                    {
                        // Clean up.
                        delete[] psztemp;

                        // Recover the original AP path exclusion.  Failure here is our only weak link.
                        // At least our UI will be updated and it will not wreck havoc.
                        HRESULT hrr;
                        if(FAILED(hrr = m_spAPExclusions->AddExclusion(temp->szExclusion, wcslen(temp->szExclusion), temp->dwFlags)))
                        {
                            CCTRCTXE1(_T("Error recovering AP path exclusion during edit. HR = 0x%08x"), hrr);

                            // hr or hrr?  Doesn't really matter.
                            return hrr;
                        }
                        else
                        {
                            // Copy AddAPExclusion()'s error reporting... including its bugs.
                            switch(hr)
                            {
                            case E_SRT_MAXEXCEEDED:
                            case E_SRT_INVALIDEXCLUSION:
                                *plResult = -2;
                                break;
                            default:
                                return hr;
                            }
                        }
                    }
                    else
                    {
                        // Update the existing APEXCLUSIONITEM entry.
                        delete[] temp->szExclusion;
                        temp->szExclusion = psztemp;
                        temp->dwFlags = (bSubFolders == TRUE) ? SRT_EXCL_SUBFOLDERS:0;

                        // We have edited just the one item, thus there's no change in index.
                        *plResult = lIndex;
                    }
                }
                else
                    return E_OUTOFMEMORY;;
            }
        }
    }
    else
    {
        // Process extension exclusion.

        // Prep the edited extension exclusion.
        CStringW cswPath(bstrPath);
        // Lop off *. if necessary
        if(wcsnicmp(cswPath.GetString(), L"*.", 2) == 0)
        {
            cswPath = cswPath.Right(cswPath.GetLength() - 2);
        }

        // Check for a dupe of the edited extension exclusion and retrieve its index.
        // ~~~TBD~~~ This should really be done by CAPExtWrapper.  Unfortunately it
        // requires an interface change, which I can't make at this point.  The check
        // below will work just fine... for now.  It will become problematic if
        // CAPExtWrapper becomes case insensitive (as it should be), because here we
        // must match its case sensitivity.
        for(lIndexDupe = m_APExtensions.GetSize(); lIndexDupe-- > 0;)
            if(m_APExtensions[lIndexDupe] && !wcscmp(m_APExtensions[lIndexDupe], cswPath))
                break;

        if(lIndexDupe >= 0 && lIndexDupe != lIndexExt)
        {
            // We have a dupe, which is not the original extension exclusion.  This is a
            // perfectly good reason to complain.
            *plResult = -1;
        }
        else if(lIndexDupe >= 0)
        {
            // We have a dupe with our original extension exclusion.  Report success without
            // making any changes.  Note, that if CAPExtWrapper becomes case insensitive,
            // then upon a dupe here we may still need to update CAPExtWrapper's contents.
            // Good luck.
            *plResult = lIndex;
        }
        else
        {
            // Add the new extension exclusion.  Note, that the rest of the code assumes
            // CAPExtWrapper adds extension exclusions to the end of its array.  If this
            // functionality changes, then we will have yet another problem.
            hr = m_APExtensions.Add(cswPath);
            if(FAILED(hr))
                return hr;
            else if(hr == S_FALSE)
                *plResult = -1;
            else
            {
                // Now delete the original exclusion error-free.  At this point we should have
                // guaranteed its index is less than the index of the newly added exclusion,
                // and the deletion will succeed.  If this fails, we're in deep trouble.
                if(FAILED(hr = DeleteAPExclusion(lIndex)))
                    CCTRCTXE1(_T("Error deleting AP exclusion during edit. HR = 0x%08x"), hr);

                // The resulting index is the last item, as per CAPExtWrapper and the way the
                // "combined" exclusions array is represented in the UI.
                *plResult = m_APExclusions.size() + m_APExtensions.GetSize() - 1;
            }
        }
    }

    return S_OK;
}

STDMETHODIMP CNAVOptions::DeleteAPExclusion(long index)
{
	long lExtensionIndex;
	PAPEXCLUSIONITEM temp;
	HRESULT hr;

	// ~~~TBD~~~ What's the usefulness of this index validation if LoadAPExclusions()
	// may end up getting called below?  Either this is done improperly, or
	// LoadAPExclusions() never kicks in, or we rely on LoadAPExclusions() to kick in
	// with a no-op effect.  Either way, something is fishy, but I can't wrap my mind
	// around the whole thing for an immediate fix, and most importantly, there is no
	// defect reported against it.
	if(index < 0 || (size_t)index > (m_APExclusions.size() + m_APExtensions.GetSize() - 1))
	{
		CCTRCTXE1(_T("Index [%d] out of bounds. "), index);
		return E_INVALIDARG;
	}

	// ~~~TBD~~~ Why is this here and not above the index validation?  See comments
	// for index validation for explanation of the logic.
	if(!m_spAPExclusions || !m_bAPExclusionsLoaded)
	{
		if(FAILED(hr = LoadAPExclusions(false)))
		{
			CCTRCTXE1(_T("Failed to load AP Exclusions. HR = 0x%08x"), hr);
			return hr;
		}
	}

	if(FAILED(this->GetExtensionIndex(index, lExtensionIndex)) || lExtensionIndex < 0)
	{
		// AP Exclusion - Remove from vector
		APExclusionList::iterator it = m_APExclusions.begin();
		while(index > 0 && it != m_APExclusions.end())
		{
			it++;
			index--;
		}

		if(*it)
		{
			temp = *it;

			if(FAILED(hr = m_spAPExclusions->DeleteExclusion(temp->szExclusion, wcslen(temp->szExclusion))))
			{
				CCTRCTXE1(_T("Error deleting AP exclusion from AP list. HR = 0x%08x"), hr);
			}

			m_APExclusions.erase(it);
			delete[] temp->szExclusion;
			delete temp;
			return hr;
		}
		else
		{
			m_APExclusions.erase(it);
		}
	}
	else
	{
		hr = m_APExtensions.Remove(m_APExtensions[lExtensionIndex]);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Error while removing AP Extension Exclusion [%d]"), lExtensionIndex);
			return hr;
		}
	}

	return S_OK;
}

HRESULT CNAVOptions::SetAPExclusionDefaults()
{
	HRESULT hr;
	CStringW cswDefaults, cswToken;
	DWORD dwLength = 256;
	int iToken;

	if(!m_spAPExclusions || !m_bAPExclusionsLoaded)
	{
		if(FAILED(hr = LoadAPExclusions(false)))
		{
			CCTRCTXE1(_T("Failed to load AP Exclusions. HR = 0x%08x"), hr);
			return hr;
		}
	}

	// Initialize the defaults if they have not been initialized yet
	if (!m_bNavDefsInitialized)
	{
		// Use the default options file name in order to initialize
		// the default ccSettings NAV hive
		if( !m_pNavDefs->Init(_T("NAVOPTS.DEF"), FALSE) )
		{
			CCTRCTXE0(_T("Failed to initialize the navopts.def ccSetting hive."));
			return E_FAIL;
		}
		else
			m_bNavDefsInitialized = true;
	}

	if(FAILED(hr = m_spAPExclusions->DeleteAllExclusions()))
	{
		CCTRCTXE1(_T("Failed to delete AP exclusions. HR = 0x%08x"), hr);
		return hr;
	}
	
	this->ClearAPExclusions();
	// TODO: Default extension exclusions properly!
	if(FAILED(hr = m_spAPExclusions->SetExtensions(L"\0\0", 2)))
	{
		CCTRCTXE1(_T("Failed to clear extension string. HR = 0x%08x"), hr);
		return hr;
	}
	if(FAILED(hr = m_APExtensions.Reset()))
	{
		CCTRCTXW1(_T("Error calling APExtensions::Reset. HR = 0x%08x"), hr);
	}

	// Load defaults from ccSettings
	if(FAILED(hr = m_pNavDefs->GetValue(_T("AUTOPROTECT:Exclusions"), cswDefaults.GetBuffer(dwLength), dwLength, _T(""))))
	{
		CCTRCTXE1(_T("Failed to get AP Exclusion defaults. Hr = 0x%08x"), hr);
		cswDefaults.ReleaseBuffer();
		return S_FALSE;
	}
	cswDefaults.ReleaseBuffer();

	EExclusionAddResult addRes;
	iToken = 0;
	cswToken = cswDefaults.Tokenize(_T("|"), iToken);
	while(iToken > 0)
	{
		dwLength = cswToken.GetLength();
		hr = this->AddAPExclusion(CComBSTR(cswToken.GetString()), TRUE, &addRes);
		if(FAILED(hr) || addRes == ExclusionAddResult_Failed)
		{
			CCTRCTXE3(_T("Error adding default exclusion [%s], hr = 0x%08x, addRes = %d"), cswToken, hr, addRes);
		}
//		cswToken.ReleaseBuffer();
		cswToken = cswDefaults.Tokenize(_T("|"),iToken);
	}

	return S_OK;
}

// Extension Stuff

// Validates whether or not a given item is an extension exclusion
HRESULT CNAVOptions::IsExtensionExclusion(PCWSTR pszExclusion, bool& bExtension)
{
	if(!pszExclusion || wcslen(pszExclusion) <= 0)
		return E_INVALIDARG;

	bExtension = false;

	// Check if it's an extension exclusion (i.e: *.xxx).
	WCHAR* pszWildCard = wcschr(pszExclusion, WCHAR('*'));
	if( (NULL != pszWildCard && pszWildCard == pszExclusion && (*CharNextW(pszWildCard)) == WCHAR('.')) )
	{
		// Now we have an extension exclusion so let's make sure it's a valid extension
		WCHAR* pszAfterDot = CharNextW(CharNextW(pszWildCard)); // Move past *.
		if( !pszAfterDot || WCHAR('\0') == *pszAfterDot )
		{
			return S_FALSE;
		}

		// Now make sure the characters are not \/:"<>| which are invalid extension characters
		// Also exclude spaces since our script processing does not support it.
		int nFound = wcscspn(pszAfterDot, L"\\/:\"<>| .");

		if( nFound != wcslen(pszAfterDot) )
		{
			return S_FALSE;
		}

		// This is a valid extension exclusion
		bExtension = true;
	}

	return S_OK;
}

// Load the extension string from AP, then tokenize it and parse into individual entries
HRESULT CNAVOptions::LoadExtensions()
{
	CStringW cswExtensions, cswExt;
	DWORD dwMaxSize;
	HRESULT hr;
	int iStart = 0;

	if(!m_spAPExclusions)
		return S_FALSE;

	// Clear out existing junk
	if(FAILED(hr = m_APExtensions.Reset()))
	{
		CCTRCTXE1(_T("Error calling AP Extensions reset. HR = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = m_spAPExclusions->GetMaxExtensionDataWchars(dwMaxSize)))
	{
		CCTRCTXE1(_T("Error calling GetMaxExtensionDataWchars, hr = 0x%08x"),hr);
		return hr;
	}

	if(FAILED(hr = m_spAPExclusions->GetExtensions(cswExtensions.GetBuffer(dwMaxSize), dwMaxSize)))
	{
		CCTRCTXE1(_T("Error calling GetExtensions, hr = 0x%08x"),hr);
		cswExtensions.ReleaseBuffer();
		return hr;
	}

	if(FAILED(hr = m_APExtensions.Initialize(cswExtensions)))
		CCTRCTXE1(_T("Error initializing AP Extensions wrapper. HR = 0x%08x"), hr);

	return hr;
}

// Accumulates the '*.ext' entries from the vector into a string buffer,
// then passes them to AP for saving.
HRESULT CNAVOptions::SaveExtensions()
{
	HRESULT hr;
	CStringW cswBuffer = "";
	size_t bufSize = 0;

	if(!m_spAPExclusions || !m_bAPExclusionsLoaded || m_APExtensions.IsDirty() == FALSE)
		return S_FALSE;

	bufSize = m_APExtensions.GetRenderLength();
	if(bufSize <= 1)
	{
		// 0 or 1 null - just default to erase.
		return m_spAPExclusions->SetExtensions(_T("\0\0"), 2);
	}
	else
	{
		if(FAILED(hr = m_APExtensions.Render(cswBuffer.GetBuffer(bufSize), bufSize)))
		{
			CCTRCTXE1(_T("Failure during AP extension rendering! HR = 0x%08x"), hr);
		}
		else
		{
			hr = m_spAPExclusions->SetExtensions(cswBuffer, bufSize);
			if(FAILED(hr))
				CCTRCTXE1(_T("Error while setting AP extension exclusions!"), hr);
		}

		cswBuffer.ReleaseBuffer();
		return hr;
	}
}

HRESULT CNAVOptions::GetExtensionIndex(long lIndex, long& lExtensionIndex)
{
	// Negative or over total number of exclusions.
	if(lIndex < 0 || (size_t)lIndex > (m_APExclusions.size() + m_APExtensions.GetSize() - 1))
	{
		CCTRCTXE1(_T("Invalid index! (%d)"), lIndex);
		return E_INVALIDARG;
	}

	lExtensionIndex = lIndex - m_APExclusions.size();

	if(lExtensionIndex < 0)		
		lExtensionIndex = -1; // Invalid result! Means there were only AP exclusions.

	return S_OK;
}
