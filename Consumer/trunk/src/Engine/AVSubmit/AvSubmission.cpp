// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

#include "StdAfx.h"
#include "AvSubmission.h"
#include "Utils.h"
#include <hrx.h>
#include "ccInstanceFactory.h"
#include "ccSymStringImpl.h"
#include "ccSymFileStreamImpl.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "AVSubmitRes.h"
#include "Utils.h"

namespace dis = clfs::DIS;

namespace AVSubmit {

CAvSubmission::CAvSubmission(void)
{
}

CAvSubmission::~CAvSubmission(void)
{
	//clean up temp files
	CCTRCTXI1(_T("Cleaning %d temp files"), m_tempFiles.size());
	for(FileSet::const_iterator i = m_tempFiles.begin(); i!=m_tempFiles.end(); ++i)
	{
		HRESULT hr = ClearReadOnly(i->GetString());  //if this fails we've got trouble.  try anyways...
		if(FAILED(hr))
			CCTRCTXE2(_T("Failed to clear readonly flag for %s: 0x%08X"), i->GetString(), hr);

		if(!ccLib::CFile::Delete(*i, false))
		{
			CCTRCTXI2(_T("Failed to delete %s: %d"), *i, GetLastError());
		}
		else 
		{
			CCTRCTXI1(_T("Deleted %s"), i->GetString());
		}
	}
	CCTRCTXE0(_T("Done"));
}

HRESULT CAvSubmission::Initialize(AVSubmit::IAVSubmission* pSub) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
		m_pAVSubmission = pSub;
		m_pSubmission = pSub;
		if(m_pAVSubmission == NULL || m_pSubmission == NULL)
		{
			CCTRCTXE0(_T("m_pAVSubmission == NULL || m_pSubmission == NULL"));
			hrx << E_NOINTERFACE;
		}
		cc::IKeyValueCollectionPtr pAttrib;
		hrx << m_pSubmission->GetAttributes(pAttrib.m_p);
		cc::ICloneQIPtr pClone = pAttrib;
		if(pClone == NULL)
		{
			CCTRCTXE0(_T("Failed to QI for clone"));
			hrx << E_NOINTERFACE;
		}
		ISymBasePtr pBase;
		if(!pClone->Clone(pBase.m_p))
		{
			CCTRCTXE0(_T("Failed to clone attrib"));
			hrx << E_FAIL;
		}
		cc::IKeyValueCollectionQIPtr pQI = pBase;
		if(pQI == NULL)
		{
			CCTRCTXE0(_T("Failed to qi for KVC"));
			hrx << E_NOINTERFACE;
		}
		m_pAttrib = pQI;
		m_pAttrib->SetCloneInterface(true);
	}
	CATCH_RETURN(hr);
}

HRESULT CAvSubmission::AddDataItem(size_t nIndex, ccEraser::eObjectType eType) throw()
{
	HRESULT hr = S_OK;
	try
	{
		ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
		m_list.push_back(Data(static_cast<DWORD>(nIndex), static_cast<DWORD>(eType)));
		CCTRCTXI2(_T("Added index %d type %d"), nIndex, eType);
	}
	catch(std::bad_alloc& )
	{
		hr = E_OUTOFMEMORY;
	}
	return hr;
}

HRESULT CAvSubmission::LoadAttributes() throw()
{
	utils::BoolX brx(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (m_pSubmission == NULL || m_pAVSubmission == NULL ? E_POINTER : S_OK);
		cc::IStringPtr pString;
		cc::IStringPtr pOutString;

		ccLib::CString sTemp;
		cc::IKeyValueCollectionPtr pAttrib = m_pAttrib;
		DWORD dw = 0;
		SYSTEMTIME st = {0};

		hrx << (pAttrib == NULL ? E_POINTER : S_OK);
		if(!pAttrib->GetCloneInterface())
		{
			CCTRCTXW0(_T("Setting pAttrib->CloneInterface(true)"));
			brx << pAttrib->SetCloneInterface(true);
		}

		CCTRCTXI1(_T("Got attrib, size = %d"), pAttrib->GetCount());

		pString.Attach(ccSym::CStringImpl::CreateStringImpl());
		CCTHROW_BAD_ALLOC(pString);

		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eDateDetected, dw);
		ATL::CTime detectTime = (time_t(dw));
		detectTime.GetAsSystemTime(st);
		CCTRCTXI1(_T("Detected = %s"), detectTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		brx << pAttrib->SetValue(dis::eSampleDateDetected, st);
		
		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eDefsDate, st);
		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eDefsRev, dw);
		sTemp.Format(_T("%04d%02d%02d.%03d"), st.wYear, st.wMonth, st.wDay, dw);
		pString->SetString(sTemp);
		CCTRCTXI1(_T("Defs = %s"), sTemp);
		brx << pAttrib->SetValue(dis::eSignaturesVersion, pString);

		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eDefsSeq, dw);
		CCTRCTXI1(_T("Seq = %d"), dw);
		brx << pAttrib->SetValue(dis::eSignaturesSequence, dw);

		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eScanResult, dw);
		CCTRCTXI1(_T("ScanRes = %d"), dw);
		switch(dw)
		{
		case AVSubmit::IAVSubmission::eCompleted:
			pString->SetStringW(L"completed");
			break;
		case AVSubmit::IAVSubmission::eHueristic:
			pString->SetStringW(L"heuristic");
			break;
		case AVSubmit::IAVSubmission::eUnknown:
		default:
			pString->SetStringW(L"unknown");
			break;
		}
		brx << pAttrib->SetValue(dis::eScanResult, pString);

		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eVID, dw);
		CCTRCTXI1(_T("VID = %d"), dw);
		brx << pAttrib->SetValue(dis::eScanVID, dw);

		brx << utils::GetString(AVSubmit::IAVSubmission::eVirusName, pAttrib, pOutString.m_p);
		CCTRCTXI1(_T("NAME = %ls"), pOutString->GetStringW());
		brx << pAttrib->SetValue(dis::eScanVirusName, pOutString); 

		if(pAttrib->GetValue(AVSubmit::IAVSubmission::eSampleCategory, dw))
		{
			switch(dw)
			{
			case AVSubmit::IAVSubmission::eCategoryAntiVirus:
				brx << pString->SetStringW(L"AntiVirus");
				break;
			case AVSubmit::IAVSubmission::eCategoryHueristic:
				brx << pString->SetStringW(L"heuristic");
				break;
			default:
				brx << pString->SetStringW(L"unknown");
				break;
			}
			brx << pAttrib->SetValue(dis::eSampleCategory, pString);
		}

		//this is non-fatal, so just ignore result
		(void) LoadDescription(); 
		(void) GetThreatCats();
		if(utils::GetString(AVSubmit::IAVSubmission::eSubEngine, pAttrib, pOutString.m_p))
		{
			CCTRCTXI1(_T("SubEngine = %ls"), pOutString->GetStringW());
			brx << pAttrib->SetValue(dis::eClientSubEngine, pOutString);
			brx << pAttrib->SetValue(dis::eSubEngine, pOutString);
		}
		CCTRCTXI0(_T("Done"));

	}
	CATCH_RETURN(hr);
}

//ISubmission
HRESULT CAvSubmission::GetAttributes(cc::IKeyValueCollection*& pAttrib) const throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	utils::FreeObject(pAttrib);
	pAttrib = m_pAttrib;
	if(pAttrib != NULL)
		pAttrib->AddRef();
	return pAttrib == NULL ? E_POINTER : S_OK;
}

HRESULT CAvSubmission::GetSubmissionData(const cc::IKeyValueCollection* /* pCriteria */, cc::IStream*& pData) throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	utils::BoolX brx(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));
	STAHLSOFT_HRX_TRY(hr)
	{
		ccEraser::IRemediationActionPtr pRem;
		cc::IStreamPtr pStream;
		cc::IKeyValueCollectionPtr pProps;

		hrx << (m_pAVSubmission == NULL ? E_POINTER : S_OK);
		hrx << GetFileData(pRem.m_p, pStream.m_p);
		hrx << ((pRem == NULL && pStream == NULL) ? E_POINTER : S_OK);

		if(pRem == NULL && pStream != NULL)
		{
			CCTRCTXI0(_T("Data is raw stream"));
			pData = pStream.Detach();
			return S_OK;
		}

		bool bExists = false;
		ccEraser::eResult eRes = pRem->GetProperties(pProps.m_p);
		if(ccEraser::Failed(eRes) || pProps == NULL)
		{
			CCTRCTXE1(_T("Failed to get props: %d"), eRes);
			hrx << E_FAIL;
		}
		
		eRes = pRem->IsPresent(bExists);
		ccLib::CString sPath;
		if(pStream == NULL && ccEraser::Succeeded(eRes) && bExists)
		{
			CCTRCTXI0(_T("File still present"));
			cc::IStringPtr pPath;
			brx << utils::GetString(ccEraser::IRemediationAction::Path, pProps, pPath.m_p);
#ifdef _UNICODE
			sPath = pPath->GetStringW();
#else
			sPath = pPath->GetStringA();
#endif
		}
		else
		{
			CCTRCTXI0(_T("Backup Stream Available or File not present"));
			if(pStream == NULL)
			{
				CCTRCTXE0(_T("File doesnt exist, undo info is null."));
				brx << false;
			}

			ccLib::CString sFile, sTempDir;
			brx << utils::GetTempFile(sFile, sTempDir, sPath);

			CCTRCTXI1(_T("Saving undo file to %s"), sPath);

			cc::IStringPtr pTempDir, pTempFile;
			pTempDir.Attach(ccSym::CStringImpl::CreateStringImpl(sTempDir));
			pTempFile.Attach(ccSym::CStringImpl::CreateStringImpl(sFile));
			CCTHROW_BAD_ALLOC(pTempDir);
			CCTHROW_BAD_ALLOC(pTempFile);

			eRes = pRem->SaveUndoInformation(pTempDir, pTempFile, pStream);
			if(ccEraser::Failed(eRes))
			{
				CCTRCTXE2(_T("Save Undo fails: %d (%s)"), eRes, sPath);
				brx << false;
			}

			ClearReadOnly(sPath);

			m_tempFiles.push_back(sPath);

			CCTRCTXI1(_T("Undo saved to %s"), sPath);
		}

		AVSubmit::IContentFilterQIPtr pFilter = m_pAVSubmission;
		if(pFilter != NULL)
		{
			ISymBasePtr pBase;
			cc::IStringQIPtr pOutPath;
			cc::IStringPtr pInPath;
			pInPath.Attach(ccSym::CStringImpl::CreateStringImpl(sPath));
			CCTHROW_BAD_ALLOC(pInPath);
			CCTRCTXI1(_T("Filtering %s"), sPath);

			HRESULT hr2 = pFilter->FilterContent(pInPath, pBase.m_p);
			pOutPath = pBase;
			if(SUCCEEDED(hr2) && pOutPath != NULL)
			{
				sPath = ATL::CW2T(pOutPath->GetStringW());
				CCTRCTXI1(_T("New temp file: %s"), sPath);
				m_tempFiles.push_back(sPath);
			}
			else
			{
				CCTRCTXE1(_T("pOutPath == NULL: 0x%08X"), hr2);
			}
		}
		else
		{
			CCTRCTXE0(_T("pFilter == NULL"));
		}

		ccSym::CFileStreamImplPtr pFileStream;
		pFileStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
		CCTHROW_BAD_ALLOC(pFileStream);

		if(!pFileStream->GetFile().Open(sPath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL))
		{
			DWORD dwError = pFileStream->GetFile().GetError();
			if(dwError == ERROR_SUCCESS) 
				dwError = ERROR_FILE_NOT_FOUND;
			CCTRCTXE2(_T("Failed to open file: %d (%s)"), dwError, sPath);
			hrx << HRESULT_FROM_WIN32(dwError);
		}

		//do one final pass on the file itself.
		hrx << IsSubmittable(pFileStream);

		utils::FreeObject(pData);
		pData = pFileStream.Detach();
		CCTRCTXI1(_T("Loaded %s into stream"), sPath);
	}
	CATCH_RETURN(hr);
}

HRESULT CAvSubmission::ProcessResponse(const cc::IKeyValueCollection* pResponse) throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	if(m_pSubmission == NULL)
	{
		CCTRCTXE0(_T("m_pSubmission == NULL"));
		return E_POINTER;
	}
	return m_pSubmission->ProcessResponse(pResponse);
}

bool CAvSubmission::Save(ccLib::CArchive& Archive) const throw()
{
	//should already be locked by parent class...
	bool bRes = false;
	STAHLSOFT_HRX_TRY(hr)
	{
		GUID guid = GUID_NULL;
		cc::ISerializeQIPtr pSerialize = m_pSubmission;
		DWORD dwSize = static_cast<DWORD>(m_list.size());
		if(pSerialize == NULL)
		{
			CCTRCTXE0(_T("Failed to QI for serialize from submission"));
			hrx << E_NOINTERFACE;
		}
		if(!pSerialize->GetObjectId(*((SYMGUID*)&guid)) || guid == GUID_NULL)
		{
			CCTRCTXE0(_T("!pSerialize->GetObjectId(*((SYMGUID*)&guid)) || guid == GUID_NULL"));
			hrx << E_FAIL;
		}
		Archive << DWORD(eVersion);
		Archive << guid;
		Archive << dwSize;
		for(DataList::const_iterator i = m_list.begin(), e = m_list.end(); i!=e; ++i)
		{
			Archive << i->m_dwIndex;
			Archive << i->m_dwType;
		}
		if(!ccSym::CPersist::Write(m_pSubmission, NULL, NULL, &Archive, NULL, NULL))
		{
			CCTRCTXE0(_T("Faield to serialize submission to archive"));
			hrx << E_FAIL;
		}
		if(!ccSym::CPersist::Write(m_pAttrib, NULL, NULL, &Archive, NULL, NULL))
		{
			CCTRCTXE0(_T("Failed to serialize attrib to archive"));
			hrx << E_FAIL;
		}
		bRes = true;
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught CArchiveException"));
	}
	CATCH_NO_RETURN(hr);

	return bRes;
}

bool CAvSubmission::Load(ccLib::CArchive& Archive) throw()
{
	//should already be locked by parent class...
	bool bRes = false;
	STAHLSOFT_HRX_TRY(hr)
	{
		DWORD dwVer = 0;
		GUID guid = GUID_NULL;
		DWORD dwSize = 0;
		m_pSubmission.Release();
		m_pAVSubmission.Release();

		m_pAttrib.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(m_pAttrib);

		cc::IInstanceFactoryPtr pFactory = ccLib::CInstanceFactory::GetInstanceFactory();
		if(pFactory == NULL)
			hrx << E_OUTOFMEMORY;

		Archive >> dwVer;
		Archive >> guid;
		Archive >> dwSize;

		//this object will already be registered - must be for subeng to function or we would not have gotten this far
		SYMRESULT sr = pFactory->CreateInstance(*((SYMGUID*)&guid), AVSubmit::IID_AVSubmission, (void**)&m_pAVSubmission.m_p);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to create avsubmit instance: 0x%08X"), sr);
			hrx << REGDB_E_CLASSNOTREG;
		}

		for(DWORD i = 0; i<dwSize; ++i)
		{
			Data d;
			Archive >> d.m_dwIndex;
			Archive >> d.m_dwType;
			m_list.push_back(d);
		}

		m_pSubmission = m_pAVSubmission;
		if(!ccSym::CPersist::Read(m_pSubmission, NULL, NULL, &Archive, NULL, NULL))
			hrx << E_FAIL;
		if(!ccSym::CPersist::Read(m_pAttrib, NULL, NULL, &Archive, NULL, NULL))
			hrx << E_FAIL;

		bRes = true;
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught CArchiveException"));
	}
	CATCH_NO_RETURN(hr);

	return bRes;
}

//////////////////////////////////////////////////////
// Internal Functions

HRESULT CAvSubmission::GetFileData(ccEraser::IRemediationAction*& pRem, cc::IStream*& pOutStream)
{
	ccEraser::IRemediationActionPtr pFile;
	cc::IStreamPtr pStream;
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

	utils::FreeObject(pRem);
	utils::FreeObject(pOutStream);
	
	for(DataList::iterator i = m_list.begin(); i!=m_list.end(); ++i)
	{
		bool bFound = true;
		switch(i->m_dwType)
		{
		case ccEraser::FileRemediationActionType:
		case ccEraser::InfectionRemediationActionType:
			hr = m_pAVSubmission->GetRemediation(i->m_dwIndex, pFile.m_p, pStream.m_p);
			break;
		default:
			bFound = false;
			CCTRCTXI1(_T("skipping erasre type %d"), i->m_dwType);
			break;
		}
		if(bFound)
			break;
	}

	pRem = pFile.Detach();
	pOutStream = pStream.Detach();

	return hr;
}

HRESULT CAvSubmission::LoadDescription() throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		//Filter out details to just the objects we are submitting.
		ccLib::CString sFormat, sDescr;
		sFormat.LoadString(IDS_DETAILS_SEPERATOR);

		for(DataList::iterator i = m_list.begin(), e =  m_list.end(); i!=e; ++i)
		{
			ccEraser::IRemediationActionPtr pRem;
			cc::IStreamPtr pStream;
			cc::IStringPtr pString;

			HRESULT hr2 = m_pAVSubmission->GetRemediation(i->m_dwIndex, pRem.m_p, pStream.m_p);
			if(FAILED(hr2))
			{
				CCTRCTXE2(_T("Failed to get remediation @ %d: 0x%08X"), i, hr2);
				continue;
			}

			cc::IKeyValueCollectionPtr pProps;
			ccEraser::eResult eRes = pRem->GetProperties(pProps.m_p);
			if(ccEraser::Failed(eRes) || pProps == NULL)
			{
				CCTRCTXE1(_T("Failed to get props: %d"), eRes);
				continue;
			}
			ccEraser::eObjectType eType;
			pRem->GetType(eType);
			size_t nKey = ccEraser::IRemediationAction::Path;
			if(eType == ccEraser::RegistryRemediationActionType)
				nKey = ccEraser::IRemediationAction::KeyName;

			if(!utils::GetString(nKey, pProps, pString.m_p))
			{
				CCTRCTXW1(_T("Failed to get prop value %d"), nKey);
				continue;
			}
			if(pString->GetLength() > 1)
			{
				sDescr += pString->GetStringW();
				if(i != (e - 1))
					sDescr += sFormat;
			}
		}
		if(sDescr.GetLength() > 1)
		{
			cc::IStringPtr pString;
			pString.Attach(ccSym::CStringImpl::CreateStringImpl(sDescr));
			CCTHROW_BAD_ALLOC(pString);
			if(!m_pAttrib->SetValue(eDetails, pString))
				hrx << E_FAIL;
		}
	}
	CATCH_RETURN(hr);

}

HRESULT CAvSubmission::IsSubmittable(cc::IStream* pStream) const throw()
{
	return S_OK;
}

HRESULT CAvSubmission::GetThreatCats() throw()
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		ISymBasePtr pBase;
		cc::IStringPtr pString;
		cc::IIndexValueCollectionQIPtr pCats;
		ccLib::CStringW sCats, sTemp;

		brx << m_pAttrib->GetValue(AVSubmit::IAVSubmission::eThreatCats, pBase.m_p);
		brx << (pBase != NULL);
		pCats = pBase;
		brx << (pCats != NULL);

		size_t nCount = pCats->GetCount();

		CCTRCTXI1(_T("%Iu threat cats"), nCount);
		if(nCount == 0)
		{
			CCTRCTXW0(_T("No threat categories"));
			return S_OK;
		}

		for(size_t i = 0; i< nCount; ++i)
		{
			DWORD dwCat = 0;
			if(pCats->GetValue(i, dwCat) == false)
			{
				CCTRCTXI1(_T("Failed to get Cat @ %Iu"), i);
				continue;
			}

			CCTRCTXI1(_T("Adding category %d"), dwCat);
			
			sTemp.Format(L"%d", dwCat);
			sCats += sTemp;
			if(i != nCount - 1)
				sCats += L", ";
		}

		if(sCats.GetLength() < 1)
		{
			CCTRCTXI0(_T("category string empty"));
			return S_OK;
		}

		CCTRCTXI1(_T("Cats = %ls"), sCats);

		pString.Attach(ccSym::CStringImpl::CreateStringImpl(sCats));
		CCTHROW_BAD_ALLOC(pString);

		brx << m_pAttrib->SetValue(dis::eThreatCats, pString);
		brx << m_pAttrib->SetValue(dis::eClientThreatCats, pString);
	}
	CATCH_RETURN(hr);
}

HRESULT CAvSubmission::ClearReadOnly(LPCTSTR pszFile) throw()
{
	HRESULT hr = S_OK;
	if(!pszFile || !*pszFile)
	{
		CCTRCTXE0(_T("pszFile == NULL"));
		return E_INVALIDARG;
	}
	DWORD dwAttrib = ::GetFileAttributes(pszFile);
	if(dwAttrib == INVALID_FILE_ATTRIBUTES)
	{
		CCTRCTXE1(_T("%s not found"), pszFile);
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}
	CCTRCTXI2(_T("Attrib 0x%08X for %s"), dwAttrib, pszFile);

	if((dwAttrib & FILE_ATTRIBUTE_READONLY) != 0)
	{
		dwAttrib &= ~FILE_ATTRIBUTE_READONLY;
		CCTRCTXI1(_T("Clearing READONLY attribute: 0x%08X"), dwAttrib);
		if(::SetFileAttributes(pszFile, dwAttrib) == FALSE)
		{
			hr = utils::LastError();
			CCTRCTXE2(_T("Failed to set file attributes for %s: 0x%08X"), pszFile, hr);
		}
	}
	return hr;
}

}
