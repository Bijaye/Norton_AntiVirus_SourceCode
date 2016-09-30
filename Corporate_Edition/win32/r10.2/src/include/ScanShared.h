// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef SCAN_SHARED
#define SCAN_SHARED

extern "C"
{

// checks to see if Startup Scans are enabled
BOOL StartupScansEnabled();

// checks to see if Startup Scans are locked
BOOL StartupScansLocked();

// Creates and opens a ScanConfig in one call
HRESULT CreateScanConfig(const CLSID &clsidScanner, DWORD RootID, char *SubKey, IScanConfig*& pConfig);

// Clear out the windows message queue.
void Relinquish();

}

#endif // SCAN_SHARED