#include "stdafx.h"
#include ".\scriptableap.h"
#include "NAVAPSCR_i.c"
#include "NAVAPSCR.h"
#include <atlcom.h>         // For ScriptSafe
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security

CScriptableAP::CScriptableAP(FUNCTION iFunction, STATE* pResult, HRESULT* phrSucceeded)
{
    m_iFunction = iFunction;
    m_phrSucceeded = phrSucceeded;
    m_pResult = pResult;

    // CThread options
    //
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eSTAModel;
    m_Options.m_bPumpMessages = TRUE;
}

CScriptableAP::~CScriptableAP(void)
{
}

int CScriptableAP::Run(void)
{
    CCTRACEI ("CScriptableAP::Run - starting %d", m_iFunction);

    try
    {
        // Check parameters
        if ( !m_pResult || !m_phrSucceeded )
        {
            CCTRACEE ("CScriptableAP::Run - bad parameter!");
            return -1;
        }
        *m_pResult = Error;
        *m_phrSucceeded = E_FAIL;

        CComPtr<IScriptableAutoProtect> spAP;
        CCTRACEI ("CScriptableAP::Run - calling CoCreate");
	    *m_phrSucceeded = spAP.CoCreateInstance(CLSID_ScriptableAutoProtect, NULL, CLSCTX_ALL);              
	    if ( FAILED (*m_phrSucceeded))
        {
            CCTRACEE ("CScriptableAP::Run - failed CoCreate APSCR 0x%x", *m_phrSucceeded);
            return -1;
        }
        CCTRACEI ("CScriptableAP::Run - CoCreate success");

        // Security
		CComPtr<ISymScriptSafe> spSymScriptSafe;
		*m_phrSucceeded = spAP->QueryInterface(&spSymScriptSafe);
	    if ( FAILED (*m_phrSucceeded))
        {
            CCTRACEE ("CScriptableAP::Run - failed QI ScriptSafe 0x%x", *m_phrSucceeded);
            return -1;
        }

		*m_phrSucceeded = spSymScriptSafe->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
	    if ( FAILED (*m_phrSucceeded))
        {
            CCTRACEE ("CScriptableAP::Run - failed SetAccess 0x%x", *m_phrSucceeded);
            return -1;
        }
        CCTRACEI ("CScriptableAP::Run - security OK");

        // Call the function
        //
        switch(m_iFunction)
        {
            case GetState:
                {
                    BOOL bEnabled = FALSE;
                    *m_phrSucceeded = spAP->get_Enabled (&bEnabled);
	                if ( FAILED (*m_phrSucceeded))
                    {
                        CCTRACEE ("CScriptableAP::Run - failed Enabled 0x%x", *m_phrSucceeded);
                        return -1;
                    }
                    if ( S_FALSE == *m_phrSucceeded)
                        *m_pResult = NotLoaded;
                    else
                    {
                        *m_pResult = bEnabled ? Enabled : Disabled;
                    }
                    return 0;
                }
            break;
            case TurnOn:
            case TurnOff:
                {
                    BOOL bEnable = (TurnOn == m_iFunction) ? TRUE : FALSE;
	                *m_phrSucceeded = spAP->Configure(bEnable);
	                if ( FAILED (*m_phrSucceeded))
                    {
                        CCTRACEE ("CScriptableAP::Run - failed Configure 0x%x", *m_phrSucceeded);
                        return -1;
                    }
                    CCTRACEI ("CScriptableAP::Run - configure OK");
                    return 0;
                }
            break;
            default:
                CCTRACEE ("CScriptableAP::Run - unknown command %d", m_iFunction);
                return -1;
                break;
        }
    }
    catch(_com_error err)
    {
        CCTRACEE ("CScriptableAP::Run - exception thrown 0x%x", err.Error);
        return -1;
    }
}
