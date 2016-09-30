////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymInterface.h"

namespace AvProd
{

    //****************************************************************************
    //  Display an alert message
    //****************************************************************************
    namespace OnEmailThreatsDetected
    {
        enum
        {
            eFirst = 0,
            eEmailInfo,                                     // AVModule::IAVMapStrData, E-mail info
            eReturnData,                                    // AVModule::IAVMapStrData, Return data
            eLast
        };
    }
    // {521221E5-2114-4e62-A37F-EBCD4CADC8E3}
    SYM_DEFINE_GUID(CMDID_OnEmailThreatsDetected, 
                    0x521221e5, 0x2114, 0x4e62, 0xa3, 0x7f, 0xeb, 0xcd, 0x4c, 0xad, 0xc8, 0xe3);

    //****************************************************************************
    //  Display an alert message
    //****************************************************************************
    namespace OnOEHDetection
    {
        enum
        {
            eFirst = OnEmailThreatsDetected::eLast,
            eOEHInfo,                                     // AVModule::IAVMapStrData, E-mail info
            eOEHAction,                                   // DWORD, Return data
            eLast
        };
    }
    // {2C7D6F39-164C-44ef-98C3-451CB1A41659}
    SYM_DEFINE_GUID(CMDID_OnOEHDetection, 
        0x2c7d6f39, 0x164c, 0x44ef, 0x98, 0xc3, 0x45, 0x1c, 0xb1, 0xa4, 0x16, 0x59);

    //****************************************************************************
    //  Reboot required notification
    //****************************************************************************
    namespace OnRebootRequired
    {
        enum
        {
            eFirst = OnOEHDetection::eLast,
            eThreatInfo,                                     // AVModule::IAVMapDwordData, E-mail info
            eAction,                                         // DWORD, Result
            eLast
        };
    }
    // {EDA3810D-4D93-4a89-8AF0-AC8AF0BDBFF0}
    SYM_DEFINE_GUID(CMDID_OnRebootRequired, 
        0xeda3810d, 0x4d93, 0x4a89, 0x8a, 0xf0, 0xac, 0x8a, 0xf0, 0xbd, 0xbf, 0xf0);

    //****************************************************************************
    //  Process Termination required notification
    //****************************************************************************
    namespace OnProcTermRequired
    {
        enum
        {
            eFirst = OnRebootRequired::eLast,
            eThreatInfo,                                     // AVModule::IAVMapDwordData, E-mail info
            eAction,                                         // DWORD, Result
            eLast
        };
    }
    // {A4D33194-5005-4422-B9AF-CB20203C3BAA}
    SYM_DEFINE_GUID(CMDID_OnProcTermRequired, 
        0xa4d33194, 0x5005, 0x4422, 0xb9, 0xaf, 0xcb, 0x20, 0x20, 0x3c, 0x3b, 0xaa);

    //****************************************************************************
    //  Process Termination required notification
    //****************************************************************************
    namespace OnAskUserForThreatRemediation
    {
        enum
        {
            eFirst = OnProcTermRequired::eLast,
            eThreatInfo,                                     // AVModule::IAVMapDwordData, Threat Track info
            eAction,                                         // DWORD, Result
            eLast
        };

        enum
        {
            eIgnoreOnce,
            eIgnoreAlways,
            eRemediate,
            eRemindMeLater
        };
    }
    // {77C88627-CBEE-48a4-9A51-3A8F52B75C4E}
    SYM_DEFINE_GUID(CMDID_OnAskUserForThreatRemediation, 
        0x77c88627, 0xcbee, 0x48a4, 0x9a, 0x51, 0x3a, 0x8f, 0x52, 0xb7, 0x5c, 0x4e);

    //****************************************************************************
    //  Launch Manual Scanner
    //****************************************************************************
    namespace OnLaunchManualScanner
    {
        enum
        {
            eFirst = OnAskUserForThreatRemediation::eLast,
            eAdditionalParameters,                         // IString, additional cmd line parms
            eLast
        };

    }
    // {AAEFFAA9-D6B5-4d36-9B4F-A2E449475DDD}
    SYM_DEFINE_GUID(CMDID_OnLaunchManualScanner, 
        0xaaeffaa9, 0xd6b5, 0x4d36, 0x9b, 0x4f, 0xa2, 0xe4, 0x49, 0x47, 0x5d, 0xdd);

}; // AvProd
