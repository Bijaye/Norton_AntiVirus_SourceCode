// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// File:        QuarantineInfectedFileItem.h
//
// Description: This file contains the class CQuarantineInfectedFileItem.  It is 
//              derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              infected file objects (both legacy stand-alone, as well as
//              infections that are part of VBinSessions, or Anomalies).
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : File created.
///////////////////////////////////////////////////////////////////////////////
#ifndef QUARANTINE_INFECTED_FILE_ITEM_H
#define QUARANTINE_INFECTED_FILE_ITEM_H

#include "QuarItem.h"

class CQuarantineInfectedFileItem : public CQuarItem
{
public:
    CQuarantineInfectedFileItem( IVBin2 *pVBin2, VBININFO *pVBinInfo );
    ~CQuarantineInfectedFileItem(void);

    // Implemented virtual methods from CQuarItem base class.
    DWORD Repair();
    BOOL  Restore( BOOL bMoveToBackup );
    DWORD SaveToQuarantine( IVBinSession *pVBinSession = 0, LPCTSTR szNewLogSessionGUID = NULL );
};


#endif // #ifndef QUARANTINE_INFECTED_FILE_ITEM_H