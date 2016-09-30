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

#include "SAVAVDetection.h"
#include "SAVSubmitterInterface.h"
#include "EraserInterfaceLoader.h"
#include "ComponentNames.h"
#include "IQuaran.h"
#include "Utils.h"
#include "EraserUtils.h"
#include "resource.h"
#include "VPExceptionHandling.h"

#include "ccKeyValueCollectionInterface.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccTrace.h"
#include "ccLib.h"

using namespace clfs;

namespace SAVSubmission {

CSAVAVDetection::CSAVAVDetection() :
			CSubmissionBase(NULL),
#ifdef PERSIST_AVPING_ANOMALY
			m_ptrAnomaly(NULL),
#endif // PERSIST_AVPING_ANOMALY
			m_pEraserLoader(NULL)
{
	CCTRACEI(CCTRCTX);
}

CSAVAVDetection::CSAVAVDetection(ccScanw::IScannerw* pScanner) :
			CSubmissionBase(pScanner),
#ifdef PERSIST_AVPING_ANOMALY
			m_ptrAnomaly(NULL),
#endif // PERSIST_AVPING_ANOMALY
			m_pEraserLoader(NULL)
{
	CCTRACEI(CCTRCTX);
}

CSAVAVDetection::~CSAVAVDetection(void)
{
	CCTRACEI(CCTRCTX);
	if (m_pEraserLoader != NULL)
	{
		m_pEraserLoader->Unload();
		delete m_pEraserLoader;
		m_pEraserLoader = NULL;
	}
}

HRESULT CSAVAVDetection::GetTypeId(GUID& typeId) const throw()
{
	typeId = TYPEID_AV_DETECTION;
	return S_OK;
}

bool CSAVAVDetection::GetObjectId(SYMOBJECT_ID& oid) const throw()
{
	oid = CLSID_SAVAVDetection;
	return true;
}

// IAVSubmission
HRESULT CSAVAVDetection::GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw()
{
#ifdef PERSIST_AVPING_ANOMALY
	utils::FreeObject(pAnomaly);
	HRESULT hr = E_POINTER;
	pAnomaly = m_ptrAnomaly;
	if(pAnomaly != NULL)
	{
		pAnomaly->AddRef();
		hr = S_OK;
	}
	return hr;
#else
	return E_NOTIMPL;
#endif // PERSIST_AVPING_ANOMALY
}

HRESULT CSAVAVDetection::GetRemediationCount(size_t& nSize) const throw()
{
	nSize = 0;

#ifdef PERSIST_AVPING_ANOMALY
	if ( !m_ptrAnomaly )
	{
		CCTRCTXE0(_T("Null anomaly"));
		return E_POINTER;
	}

	ccEraser::eResult eRemediationResult;
	ccEraser::IRemediationActionListPtr pRemediationActionList;
	eRemediationResult = m_ptrAnomaly->GetRemediationActions(pRemediationActionList.m_p);
	if( !Succeeded(eRemediationResult) || !pRemediationActionList )
	{
		CCTRCTXE1(_T("Failed to get remediation actions: %d"), eRemediationResult);
		return E_FAIL;
	}
	eRemediationResult = pRemediationActionList->GetCount(nSize);
	if( !Succeeded(eRemediationResult) )
	{
		CCTRCTXE1(_T("Failed to get remediation count: %d"), eRemediationResult);
		return E_FAIL;
	}
	CCTRCTXI1(_T("Remediation count = %Iu"), nSize);
#endif // PERSIST_AVPING_ANOMALY
	return S_OK;
}

HRESULT CSAVAVDetection::GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRem, cc::IStream*& pStream) const throw()
{
	utils::FreeObject(pRem);
	utils::FreeObject(pStream);

#ifdef PERSIST_AVPING_ANOMALY
	STAHLSOFT_HRX_TRY(hr)
	{
		CCTRCTXI1(_T("Load Remediation @ %d"), nIndex);

		if ( !m_ptrAnomaly )
		{
			CCTRCTXE0(_T("Null anomaly"));
			hrx << E_POINTER;
		}

		ccEraser::eResult eRemediationResult;
		ccEraser::IRemediationActionListPtr pRemediationActionList;
		eRemediationResult = m_ptrAnomaly->GetRemediationActions(pRemediationActionList.m_p);
		if( !Succeeded(eRemediationResult) || !pRemediationActionList )
		{
			CCTRCTXE1(_T("Failed to get remediation actions: %d"), eRemediationResult);
			hrx << E_FAIL;
		}

		size_t nRemediationActionCount;
		eRemediationResult = pRemediationActionList->GetCount(nRemediationActionCount);
		if( !Succeeded(eRemediationResult) )
		{
			CCTRCTXE1(_T("Failed to get remediation count: %d"), eRemediationResult);
			hrx << E_FAIL;
		}
		if (nIndex >= nRemediationActionCount)
		{
			CCTRCTXE2(_T("Asking for remediation %d but only have %d remediations"), nIndex, nRemediationActionCount);
			hrx << HRESULT_FROM_WIN32(ERROR_INVALID_INDEX);
		}
		eRemediationResult = pRemediationActionList->GetItem(nIndex, pRem);
		if( !Succeeded(eRemediationResult) )
		{
			CCTRCTXE2(_T("Failed to get remediation %d: %d"), nIndex, eRemediationResult);
			return E_FAIL;
		}

		CCTRCTXI2(_T("r: %p s: %p"), pRem, pStream);
		DumpRemediation(nIndex, pRem);
	}
	CATCH_RETURN(hr);
#else
	return E_INVALIDARG;
#endif // PERSIST_AVPING_ANOMALY
}

HRESULT CSAVAVDetection::GetEraserInstance(ccEraser::IEraser*& pEraser)
{
	if (pEraser != NULL)
	{
		pEraser->Release();
		pEraser = NULL;
	}

	if (m_pEraserLoader == NULL)
	{
		try
		{
			m_pEraserLoader = new CEraserInterfaceLoader();
		}
		VP_CATCH_MEMORYEXCEPTIONS(;);
		if (m_pEraserLoader == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}

	const TCHAR* szDefsDir = _T("C:\\Program Files\\Common Files\\Symantec Shared\\VirusDefs\\20060914.009");	//TODO:JJM: need to fix this

    // Format the name of the eraser DLL in the path provided.
    TCHAR szEraserDLL[MAX_PATH];
    if (ssStrnCpy (szEraserDLL, szDefsDir, sizeof (szEraserDLL)) >= sizeof (szEraserDLL))
	{
		CCTRCTXE1(_T("Path too long: %s"), szDefsDir);
        return E_INVALIDARG;
	}
    if (ssStrnAppendFile (szEraserDLL, cc::sz_ccEraser_dll, sizeof (szEraserDLL)) >= sizeof (szEraserDLL))
	{
		CCTRCTXE2(_T("Path too long: %s + %s"), szDefsDir, szEraserDLL);
        return E_INVALIDARG;
	}

    // Use the specified SymInterfaceLoader object to load an instance of this Eraser DLL.
    SYMRESULT hSymResult = m_pEraserLoader->Initialize (szEraserDLL);
    if (SYM_FAILED(hSymResult))
    {
		CCTRCTXE2(_T("Loader failed to initialize for %s: 0x08%x"), szEraserDLL, hSymResult);
        return E_NOINTERFACE;
    }
	ISymBase* pBase;
    hSymResult = m_pEraserLoader->CreateObject( ccEraser::IID_Eraser, ccEraser::IID_Eraser, (void**)&pBase );
    if (SYM_FAILED(hSymResult))
    {
		CCTRCTXE1(_T("Loader failed to create object: 0x08%x"), hSymResult);
        return E_NOINTERFACE;
    }
	ccEraser::IEraserQIPtr pEraserPtr = ccEraser::IEraserQIPtr(pBase);
	pBase->Release();
	if (pEraserPtr != NULL)
	{
		ccEraser::eResult eEraserResult = pEraserPtr->Initialize(NULL, NULL, NULL, ccEraser::CurrentVersion);

		if (!ccEraser::Succeeded(eEraserResult))
		{
			CCTRCTXE1(_T("Loader initialize Eraser: 0x08%x"), eEraserResult);
			return E_NOINTERFACE;
		}
	}
	else
	{
		CCTRCTXE0(_T("Failed to find interface"));
		return E_NOINTERFACE;
	}

	pEraser = pEraserPtr.Detach();
	return S_OK;
}

#ifdef PERSIST_AVPING_ANOMALY
ccEraser::IAnomaly* CSAVAVDetection::LoadAnomalyFromStream(cc::IStream* pStream)
{
	ccEraser::IAnomaly* pResult = NULL;
	ISymBasePtr pBase;
	ccEraser::eObjectType type;
	ccEraser::IEraserPtr ptrEraser;
	
	HRESULT hResult = GetEraserInstance(ptrEraser.m_p);

	if (SUCCEEDED(hResult) && ptrEraser != NULL)
	{
		ccEraser::eResult eResult = ptrEraser->CreateObjectFromStream(pStream, &type, pBase);

		if (ccEraser::Succeeded(eResult) && pBase != NULL)
		{
			SAVASSERT(ccEraser::AnomalyType == type);
			ccEraser::IAnomalyQIPtr ptrAnomaly(pBase);
			pBase->Release();
			pResult = ptrAnomaly.Detach();
		}
		else
		{
			CCTRCTXE1(_T("Failed to read anomaly from stream: 0x08%x"), eResult);
		}
	}
	return pResult;
}
#endif // PERSIST_AVPING_ANOMALY

bool CSAVAVDetection::Load(ccLib::CArchive& Archive) throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	bool bRes = false;
	HRESULT hr = S_OK;
	try
	{
		if (CSubmissionBase::Load(Archive))
		{
#ifdef PERSIST_AVPING_ANOMALY
			DWORD dwVer = 0;

			Archive >> dwVer;
			if (dwVer == kdwVersion)
			{
				ccSym::CMemoryStreamImplPtr ptrStream;
				
				ptrStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
				if (ptrStream != NULL)
				{
					Archive >> ptrStream->GetMemory();
					size_t nBytes = ptrStream->GetMemory().GetSize();
					CCTRCTXI1(_T("Read %d bytes"), nBytes);
					
					// Load the anomaly from the stream.
					m_ptrAnomaly = LoadAnomalyFromStream(ptrStream);
					cc::ISerializeQIPtr ptrSerialize = m_ptrAnomaly;
					if (ptrSerialize != NULL && ptrSerialize->Load(ptrStream))
					{
						bRes = true;
					}
					else
					{
						CCTRCTXE0(_T("Couldn't load anomaly"));
					}
				}
				else
				{
					CCTRCTXE0(_T("Couldn't create memory stream while loading anomaly"));
				}
/*
				if (ccSym::CPersist::Read(m_ptrAnomaly, NULL, NULL, &Archive, NULL, NULL))
				{
					CCTRCTXI0(_T("Dumping restored anomaly"));
					DumpAnomaly(m_ptrAnomaly);
					bRes = true;
				}
				else
				{
					CCTRCTXE0(_T("Failed to read anomaly"));
				}
*/
			}
			else
			{
				CCTRCTXE1(_T("Cannot read version: 0x%08X"), dwVer);
			}
#else
			bRes = true;
#endif //PERSIST_AVPING_ANOMALY
		}
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught CArchiveException"));
	}
	CATCH_NO_RETURN(hr);

	return bRes;
}

bool CSAVAVDetection::Save(ccLib::CArchive& Archive) const throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	bool bRes = false;
	HRESULT hr = S_OK;
	try
	{
		if (CSubmissionBase::Save(Archive))
		{
#ifdef PERSIST_AVPING_ANOMALY
			Archive << kdwVersion;

			ccSym::CMemoryStreamImplPtr ptrStream;
			ptrStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
			if (ptrStream != NULL)
			{
				// Save this object to the stream.
				cc::ISerializeQIPtr ptrSerialize = m_ptrAnomaly;
				if (ptrSerialize != NULL && ptrSerialize->Save(ptrStream))
				{
					Archive << ptrStream->GetMemory();
					bRes = true;
					size_t nBytes = ptrStream->GetMemory().GetSize();
					CCTRCTXI1(_T("Anomaly took %d bytes"), nBytes);
				}
				else
				{
					CCTRCTXE0(_T("Couldn't save anomaly"));
				}
			}
			else
			{
				CCTRCTXE0(_T("Couldn't create memory stream while saving anomaly"));
			}
/*
			if (ccSym::CPersist::Write(m_ptrAnomaly, NULL, NULL, &Archive, NULL, NULL))
			{
				bRes = true;
			}
*/
#else
			bRes = true;
#endif // PERSIST_AVPING_ANOMALY
		}
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught CArchiveException"));
	}
	CATCH_NO_RETURN(hr);

	return bRes;
}

HRESULT CSAVAVDetection::Initialize(const ccEraser::IAnomaly* pAnomaly)
{
	if (pAnomaly == NULL)
		return E_INVALIDARG;

#ifdef PERSIST_AVPING_ANOMALY
	m_ptrAnomaly = pAnomaly;
#endif // PERSIST_AVPING_ANOMALY

	STAHLSOFT_HRX_TRY(hr)
	{
		m_pAttributes.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		if (m_pAttributes != NULL)
			hrx << E_OUTOFMEMORY;

		DWORD dwVid;
		ccLib::CStringW strTemp;
		cc::IStringPtr pProduct;
		cc::IStringPtr pVersion;
		cc::IStringPtr pActor;

		hrx << GetPVA(pProduct, pVersion, pActor);
		if (pProduct != NULL)
			m_pAttributes->SetValue(clfs::ISubmission::eProductName, pProduct);
		if (pVersion != NULL)
			m_pAttributes->SetValue(clfs::ISubmission::eProductVersion, pVersion);
		if (pActor != NULL)
			m_pAttributes->SetValue(clfs::ISubmission::eActor, pActor);

		hrx << SetDescription(m_pAttributes, IDS_AV_DETECTION);
		hrx << SetDefDateAndSeqAttributes();

		m_pAttributes->SetValue(eScanResult, DWORD(eHueristic));
		m_pAttributes->SetValue(IAVSubmission::eManual, false);

		DWORD dwTimeNow = DWORD(time(NULL));
		m_pAttributes->SetValue(IAVSubmission::eDateDetected, dwTimeNow);

		cc::IKeyValueCollectionConstPtr ptrProps;
		hrx << pAnomaly->GetProperties(ptrProps.m_p);	//TODO:JJM: returns eResult not HRESULT so fix this
		hrx << ptrProps->GetValue(ccEraser::IAnomaly::VID, dwVid);

		SAVASSERT(dwVid != 0);

//		hrx << CheckBloodhound(dwVid);	// AV Pings should not filter out non-Bloodhound detections
		m_pAttributes->SetValue(IAVSubmission::eVID, dwVid);

		hrx << SetVirusNameAttribute(dwVid);

		hrx << GetAVVer(strTemp);
		hrx << SetComponent(SubSDK::Components::g_szAV, strTemp, IAVSubmission::eCategoryAntiVirus, NULL);
		hrx << SetDetails();

		HRESULT hr2 = SetThreatCatProperties(pAnomaly);
		if (FAILED(hr2))
		{
			CCTRCTXE1(_T("SetThreatCatProperties == 0x%08X"), hr2);
		}
	}
	CATCH_RETURN(hr);
}

} //namespace SAVSubmission
