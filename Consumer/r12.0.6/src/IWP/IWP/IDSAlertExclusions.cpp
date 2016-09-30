#include "StdAfx.h"
#include ".\idsalertexclusions.h"

const WCHAR szIWPIDSExclusionsKey [] = L"Internet Worm Protection\\IDSExclusions";

CIDSAlertExclusions::CIDSAlertExclusions(void)
{
    m_exclusions.Load ( szIWPIDSExclusionsKey );
}

CIDSAlertExclusions::~CIDSAlertExclusions(void)
{
    m_exclusions.Save ( szIWPIDSExclusionsKey );
}

bool CIDSAlertExclusions::IsExcluded (DWORD dwIDSSignature)
{
    DWORD dwReturn = 0;
    WCHAR szSignature [50] = {0};
    _ultow ( dwIDSSignature, szSignature, 10 );
    return SYM_SUCCEEDED ( m_exclusions.GetSettings()->GetDword ( szSignature, dwReturn ));
}

bool CIDSAlertExclusions::AddExclusion (DWORD dwIDSSignature)
{
    long lTrue = 1;
    WCHAR szSignature [50] = {0};
    _ultow ( dwIDSSignature, szSignature, 10 );
    return SYM_SUCCEEDED ( m_exclusions.GetSettings()->PutDword ( szSignature, lTrue));
}

bool CIDSAlertExclusions::RemoveExclusion (DWORD dwIDSSignature)
{
    WCHAR szSignature [50] = {0};
    _ultow ( dwIDSSignature, szSignature, 10 );
    return SYM_SUCCEEDED ( m_exclusions.GetSettings()->DeleteValue ( szSignature ));
}

bool CIDSAlertExclusions::RemoveAll (void)
{
    m_exclusions.Delete ( szIWPIDSExclusionsKey );
    return m_exclusions.Load ( szIWPIDSExclusionsKey );
}