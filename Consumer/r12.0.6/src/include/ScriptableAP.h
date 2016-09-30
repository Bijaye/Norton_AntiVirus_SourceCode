#pragma once

#include "ccThread.h"

// Use this class if you need to call Scriptable AP from a MTA thread. 
// See defect 1-2VMSMB.
//
class CScriptableAP : public ccLib::CThread
{
public:
    enum FUNCTION
    {
	    GetState = 0,
        TurnOn,
        TurnOff
    };

    enum STATE
    {
        Error = 0,
        Enabled,
        Disabled,
        NotLoaded
    };

    CScriptableAP(FUNCTION iFunction /*in*/,
                  STATE* pResult /*out*/,
                  HRESULT* phrSucceeded /*out*/);
    ~CScriptableAP(void);

// CThread
    int Run (void);

protected:
    FUNCTION m_iFunction;
    STATE* m_pResult;
    HRESULT* m_phrSucceeded;

// not allowed    
    CScriptableAP(void){};
};
