#include "StdAfx.h"
#include ".\imscanwrapper.h"
#include "NAVTrust.h"

CIMScanWrapper::CIMScanWrapper(void) : m_pfnIsIMScannerInstalled(NULL)
{
    // Load ccIMScan.dll

    // Get the full path to the IM scanner
    TCHAR szCCImscanPath[MAX_PATH + 16];
    ::_tcscpy(szCCImscanPath, g_NAVInfo.GetNAVDir());
	::_tcscat(szCCImscanPath, _T("\\ccIMScan.dll")); // Not defined in ccModuleNames.cpp :(

    // Verify the signature
    if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szCCImscanPath) )
    {
        CCTRACEE ("CIMScanWrapper - failed trust check");
        throw false;
    }
    else
    {
        if (!m_modIMScan.Load ( szCCImscanPath, LOAD_WITH_ALTERED_SEARCH_PATH ))
        {
            CCTRACEE ("CIMScanWrapper - failed to load");
            throw false;
        }
        else
        {
            // Get the IMScanInstalled function
            m_pfnIsIMScannerInstalled = reinterpret_cast<pfnISIMCLIENTINSTALLED>(m_modIMScan.GetProc ("IsIMClientInstalled"));
            //m_pfnIsIMScannerInstalled = reinterpret_cast<pfnISIMCLIENTINSTALLED>(::GetProcAddress (m_modIMScan.GetModule(), "IsIMClientInstalled"));
            
            if ( !m_pfnIsIMScannerInstalled )
            {
                CCTRACEE ("CIMScanWrapper - failed to GetProc");
                throw false;
            }
        }
    }
}

CIMScanWrapper::~CIMScanWrapper(void)
{
}

bool CIMScanWrapper::IsIMScannerInstalled (IMTYPE imtype)
{
    if ( !m_pfnIsIMScannerInstalled )
        return false;

    return m_pfnIsIMScannerInstalled(imtype);
}

