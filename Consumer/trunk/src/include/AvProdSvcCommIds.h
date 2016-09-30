////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace AvProdSvc
{   
    //****************************************************************************
    //  General Communication Channel
    //****************************************************************************
    extern const LPWSTR CommChannelGeneral;
    
    //////////////////////////////////////////////////////////////////////////////
    //  Disable Reboot Dialog
    //    Sending this message will suppress the reboot dialog until 
    //    either a reboot occurs, or EnableRebootDialog is sent.
    //    This message is ref-counted, you must call EnableRebootDialog once
    //    your need to suppress is over.
    //    
    //////////////////////////////////////////////////////////////////////////////
    namespace DisableRebootDialog
    {
        enum
        {
            eFirst = 0xE0000000,
            eClientCookie, // IString: client cookie
            eLast
        } ;
    }
    // {70A32D03-482A-4897-9E0B-A5BCFA6984D1}
    SYM_DEFINE_GUID(CMDID_DisableRebootDialog, 
        0x70a32d03, 0x482a, 0x4897, 0x9e, 0xb, 0xa5, 0xbc, 0xfa, 0x69, 0x84, 0xd1);

    //////////////////////////////////////////////////////////////////////////////
    //  Enable Reboot Dialog
    //    Sending this message enables the reboot dialog.
    //    This message is ref-counted with DisableRebootDialog.  The last
    //    caller to send the EnableRebootDialog command is the only one 
    //    where the parameters are acted upon.
    //    
    //////////////////////////////////////////////////////////////////////////////
    namespace EnableRebootDialog
    {
        enum
        {
            eFirst = DisableRebootDialog::eLast,
            eClientCookie, // IString: client cookie
            eProcessPendingRebootRequests,  // bool: false = discard, true = process
            eForceRebootDialog, // bool: true = display reboot dialog
            eLast
        } ;
    }
    // {89A11DB6-BA12-46f2-A963-A4C5229F0748}
    SYM_DEFINE_GUID(CMDID_EnableRebootDialog, 
        0x89a11db6, 0xba12, 0x46f2, 0xa9, 0x63, 0xa4, 0xc5, 0x22, 0x9f, 0x7, 0x48);

    //////////////////////////////////////////////////////////////////////////////
    //  Log Manual Scan Results
    //    Convenient way to log manual scan results asynchronously.  This
    //    main is to enable manual scan to close immediately, and not
    //    wait for logging to complete.
    //    
    //////////////////////////////////////////////////////////////////////////////
    namespace LogManualScanResults
    {
        enum
        {
            eFirst = EnableRebootDialog::eLast,
            eManualScanInstanceId, // GUID: AvModule Scan Instance ID
            eLast
        } ;
    }
	// {60326d97-8f1b-4f50-907b-8435360ced18} 
    SYM_DEFINE_GUID(CMDID_LogManualScanResults, 
		0x60326d97, 0x8f1b, 0x4f50, 0x90, 0x7b, 0x84, 0x35, 0x36, 0x0c, 0xed, 0x18);
   
}; // AvProdService