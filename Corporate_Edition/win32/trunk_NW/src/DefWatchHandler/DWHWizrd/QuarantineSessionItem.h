// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// File:        QuarantineSessionItem.h
//
// Description: This file contains the class definition for CQuarantineSessionItem.  
//              It is  derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              VBin session objects.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : File created.
///////////////////////////////////////////////////////////////////////////////
#ifndef QUARANTINE_SESSION_ITEM_H
#define QUARANTINE_SESSION_ITEM_H

#include "QuarItem.h"

class CQuarantineSessionItem : public CQuarItem
{
public:
    CQuarantineSessionItem( IVBin2 *pVBin2, VBININFO *pVBinInfo );
    ~CQuarantineSessionItem(void);

    // Virtual methods from CQuarItem for repairing and
    // restoring this session item.
    DWORD   Repair( void );
    BOOL    Restore( BOOL bMoveToBackup );
    DWORD   SaveToQuarantine( IVBinSession *pVBinSession = 0, LPCTSTR szNewLogSessionGUID = NULL );


protected:

    DWORD   SaveQuarItemList( IVBinSession *pVBinSession, LPCTSTR szNewLogSessionGUID, CPtrArray *lparQuarItems );

    DWORD   RepairAllInfectedFiles( void );
    BOOL    RestoreAllInfectedFiles( void );
    BOOL    RestoreAllRemediations( void );

	void    CleanupInfectedItemArray();
	void    CleanupRemediationItemArray();

    BOOL    CreateLogGUIDString( LPTSTR szNewLogGUIDString );
private:
    CPtrArray       m_arInfectedQuarItems;
    CPtrArray       m_arRemediationQuarItems;
};


#endif // #ifndef QUARANTINE_SESSION_ITEM_H
