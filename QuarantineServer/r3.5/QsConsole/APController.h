// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// CAPController:
//
// Utility object to enable/disable AP for this process.
// Helper for global DisableAP()/EnableAP() functions.
//

#ifndef __APCONTROLLER_H
#define __APCONTROLLER_H

typedef BOOL (WINAPI *NAVAPFunctionPointer)();

class CAPController
{
public:
    CAPController(void);
    virtual ~CAPController(void);

    DWORD   UnprotectProcess();         // Adds the current process to AP's process exclusion list.
    DWORD   ProtectProcess();           // Removes the current process from AP's process exclusion list.

    DWORD   Initialize();               // Not required, but can pre-load AP functions by calling this manually.
                                        // Will be called by UnprotectProcess(),ProtectProcess().

private:
    CAPController( const CAPController & );                 // Not implemented
    CAPController & operator=( const CAPController & );     // Not implemented

    DWORD   Deinitialize();             // Unload AP functions. Called from destructor.

private:
    CRITICAL_SECTION        m_csInitialization; // Guards initialization/deinitialization.
    bool                    m_bInitialized;

    HMODULE                 m_hNavapLib;
    NAVAPFunctionPointer    m_pfnUnprotectProcess;
    NAVAPFunctionPointer    m_pfnProtectProcess;
};

#endif // __APCONTROLLER_H