////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// GetFileName.h : header file
//

#pragma once

#include <isSymTheme.h>

class CGetFileName :
    public OPENFILENAME,
    public ccLib::CThread
{
public:
    CGetFileName(void)
    {
        // Setup the proxy thread options
        m_Options.m_bNoCRTThread = FALSE;
        m_Options.m_bPumpMessages = TRUE;
        m_Options.m_eCOMModel = ccLib::CCoInitialize::eSTAModel;
        m_Options.m_bWaitForInitInstance = TRUE;

        // Clear our OPENFILENAME data
        OPENFILENAME* pOFN = (OPENFILENAME*)this;
        ZeroMemory(pOFN, sizeof(OPENFILENAME));

        // Set our struct size
        lStructSize = sizeof(OPENFILENAME);
    }

    virtual ~CGetFileName(void)
    {
        CCTRCTXI0(L"dtor");
    }

    BOOL Open(bool bThemeNeeded)
    {
        m_bRet = FALSE;

        // Create the GetOpenFileName() proxy thread
        m_bSave = false;
        m_dwLastErr = 0;
        m_bThemeNeeded = bThemeNeeded;
        if(!Create(NULL, 0, 0))
            return false;

        // Wait for the thread
        WaitForExit(INFINITE);

        return m_bRet;
    }

    BOOL Save(bool bThemeNeeded)
    {
        m_bRet = FALSE;

        // Create the GetSaveFileName() proxy thread
        m_bSave = true;
        m_dwLastErr = 0;
        m_bThemeNeeded = bThemeNeeded;
        if(!Create(NULL, 0, 0))
            return false;

        // Wait for the thread
        WaitForExit(INFINITE);

        return m_bRet;
    }

protected:
    int Run()
    {
        CCTRCTXI0(L"--Enter");

        // Apply skin
	    HRESULT hr1 = S_OK;
	    CISSymTheme isSymTheme;
        if(m_bThemeNeeded)
        {
	        hr1 = isSymTheme.Initialize(GetModuleHandle(NULL));
	        if(FAILED(hr1))
	        {
		        CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr1);
	        }
        }

        // Call save or open
        if(m_bSave)
            m_bRet = GetSaveFileName(this);
        else
            m_bRet = GetOpenFileName(this);

        // Grab the last error
        m_dwLastErr = GetLastError();

        CCTRCTXI0(L"--Exit");
        return 0;
    }

protected:
    BOOL m_bRet;
    DWORD m_dwLastErr;
    bool m_bSave;
    bool m_bThemeNeeded;
};
