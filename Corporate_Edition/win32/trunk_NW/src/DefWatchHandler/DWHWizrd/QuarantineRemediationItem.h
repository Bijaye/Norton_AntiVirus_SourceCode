// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// File:        QuarantineRemediationItem.h
//
// Description: This file contains the class CQuarantineRemediationItem.  It is 
//              derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              ccEraser Remediation objects.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : File created.
///////////////////////////////////////////////////////////////////////////////
#ifndef QUARANTINE_REMEDIATION_ITEM_H
#define QUARANTINE_REMEDIATION_ITEM_H

#include "QuarItem.h"

class CQuarantineRemediationItem : public CQuarItem
{
public:
    CQuarantineRemediationItem( IVBin2 *pVBin2, VBININFO *pVBinInfo );
    ~CQuarantineRemediationItem(void);

    // Virtual methods from CQuarItem for repairing and
    // restoring this remediation item.
    DWORD   Repair( void );
    BOOL    Restore( BOOL bMoveToBackup );
    DWORD   SaveToQuarantine( IVBinSession *pVBinSession = 0, LPCTSTR szNewLogSessionGUID = NULL );

protected:
    DWORD ExtractToTempFile();

};


#endif // #ifndef QUARANTINE_REMEDIATION_ITEM_H
