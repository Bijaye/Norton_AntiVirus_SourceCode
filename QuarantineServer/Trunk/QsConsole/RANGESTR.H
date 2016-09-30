/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/RANGESTR.H_v   1.0   06 Mar 1998 11:16:26   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// RnageStr.h
// Definitions for CRangedString - Snaggled from the SpeedDisk NT code
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/RANGESTR.H_v  $
// 
//    Rev 1.0   06 Mar 1998 11:16:26   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef _RANGESTR_H_
#define _RANGESTR_H_

class CRangedString : public CString
{
public:
	CRangedString() {};
    CRangedString( DWORD dwSizeInBytes ) { SetRangedString( dwSizeInBytes ); };
    ~CRangedString() {};

    void SetRangedString( DWORD xwSizeInBytes );
};

#endif   // _RANGESTR_H_
