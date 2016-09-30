////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Builtins.h : header file
//
#pragma once

HRESULT Computer_Load(avScanTask::IScanTask* pThis);
HRESULT HardDisks_Load(avScanTask::IScanTask* pThis);
HRESULT RemovableDrives_Load(avScanTask::IScanTask* pThis);
HRESULT Floppy_Load(avScanTask::IScanTask* pThis);
HRESULT Folders_Load(avScanTask::IScanTask* pScanTask);
HRESULT Files_Load(avScanTask::IScanTask* pScanTask);
HRESULT Drives_Load(avScanTask::IScanTask* pScanTask);
