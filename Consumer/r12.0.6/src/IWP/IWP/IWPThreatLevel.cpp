#include "StdAfx.h"
#include "IWPThreatLevel.h"
#include "ccSymStringImpl.h"
#include "resource.h"     // resource string IDs

CIWPThreatLevel::CIWPThreatLevel(void) throw()
{
}

CIWPThreatLevel::~CIWPThreatLevel(void) throw()
{
    if ( m_pTLevel )
        m_pTLevel.Release();
}


bool CIWPThreatLevel::Initialize() throw()
{
    // Load the Threat Level object
    if(SYM_FAILED(m_TLevelLoader.CreateObject(&m_pTLevel)))
    {
        return false;
    }

    // Initialize the test values
    cc::IKeyValueCollectionPtr pValues;
    if(!m_pTLevel->GetAssessorValues(&pValues) || !InitializeTestResultValues(pValues))
    {
        return false;
    }

	if( !m_pTLevel->Load(IThreatLevel::eVirusScanAssessor) || 
        !m_pTLevel->Load(IThreatLevel::eSignedCodeAssessor))
    {
        CCTRACEE(_T("CIWPThreatLevel::Initialize() : Load Assessors failed\n"));
        return false;
    }

    return true;
}

bool CIWPThreatLevel::GetThreatLevel(LPCSTR szApplicationPath, IWP_THREAT_LEVEL& eLevel ) throw()
{
    // Make sure that we have been initialized
    if(!m_pTLevel)
    {
        CCTRACEE(_T("CIWPThreatLevel::GetThreatLevel() : Not Initialized\n"));
        return false;
    }

    // Reset any previous results
    if(!m_pTLevel->Reset())
    {
        return false;
    }

	ccSym::CStringImpl strApplicationPath(szApplicationPath);

    if(!m_pTLevel->Analyze(strApplicationPath.GetStringA()))
    {
        CCTRACEE(_T("CIWPThreatLevel::GetThreatLevel() : Analyze Failed\n"));
        return false;
    }

    eLevel = ComputeThreatLevel();

    return true;
}

CIWPThreatLevel::IWP_THREAT_LEVEL CIWPThreatLevel::ComputeThreatLevel() throw()
{
    if (!m_pTLevel)
        return IWP_THREAT_HIGH;

    // Result
    // 1 = allow and autocreate     (Symantec signature)
    // 0 = prompt                   (no signature, no virus)
    // negative = block             (virus found)

    // Get the summed result value from TLevel
    long lResult = -1;
    m_pTLevel->Compute(lResult);

    if(lResult < 0)
        return IWP_THREAT_HIGH;     // virus, etc. - should recommend closing app and scanning with NAV, or offer to
    else if(lResult < 1)
        return IWP_THREAT_MEDIUM;   // unknown app, etc.
    else
        return IWP_THREAT_LOW;      // Symantec signature
}

bool CIWPThreatLevel::InitializeTestResultValues(cc::IValueCollection* pResultValues) throw()
{
    if (!pResultValues)
        return false;

    // 0 = no effect
    // 1 = trusted
    // 0 = non-viral threat
    // -1000 = viral threat

    // Set the Values for the VirusScanAssessor
    pResultValues->SetValue(IThreatLevel::eVirusTestNotPerformed, static_cast<long>(0));
    pResultValues->SetValue(IThreatLevel::eFileNotFound, static_cast<long>(0));
    pResultValues->SetValue(IThreatLevel::eVirusNotFound, static_cast<long>(0));
    pResultValues->SetValue(IThreatLevel::eNonViralThreatFound, static_cast<long>(0));
    pResultValues->SetValue(IThreatLevel::eVirusFound, static_cast<long>(-1000));

    // Set the Values for the SignedCodeAssessor
    pResultValues->SetValue(IThreatLevel::eSignatureTestNotPerformed, static_cast<long>(0));
    pResultValues->SetValue(IThreatLevel::eClass3DigitalSignature, static_cast<long>(0));
    pResultValues->SetValue(IThreatLevel::eSymantecSignature, static_cast<long>(1));
    pResultValues->SetValue(IThreatLevel::eNoDigitalSignature, static_cast<long>(0));

    return true;
}