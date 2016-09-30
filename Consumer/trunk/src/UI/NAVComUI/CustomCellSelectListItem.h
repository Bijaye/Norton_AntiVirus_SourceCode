////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//****************************************************************************
// CustomCellSelectListItem.h: Interface for CCustomCellSelectListItem class.
//****************************************************************************
#pragma once

class CCustomCellSelectListItem
{
private:
    CString  string;
    UINT     data;

public:
    CCustomCellSelectListItem( const CString & initString, 
                               const UINT initData );
    CCustomCellSelectListItem( const CCustomCellSelectListItem & sourceItem);

    CCustomCellSelectListItem &
    operator=(const CCustomCellSelectListItem & sourceItem);

    const CString &  GetString() const;
    const UINT       GetData() const;
};
