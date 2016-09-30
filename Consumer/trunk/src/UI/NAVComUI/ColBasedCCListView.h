////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//***************************************************************************
// ColBasedCCListView.h: interface for the CColBasedCCListView class.
//***************************************************************************
#pragma once

#include "CustomCellListView.h"

class CColBasedCCListView : public CCustomCellListView 
{
public:
    CColBasedCCListView();
    virtual ~CColBasedCCListView();

    BOOL SetColumnType(INT iCol, DWORD dwType);
    BOOL GetColumnType(INT iCol, DWORD *pdwType);

    BOOL SetColumnTextColor(INT iCol, COLORREF crColor);
    BOOL GetColumnTextColor(INT iCol, COLORREF *pcrColor);

    BOOL GetCellType(INT iRow, INT iCol, DWORD *pdwType);
    BOOL GetCellTextColor(INT iRow, INT iCol, COLORREF *pcrColor);

protected:
    typedef std::vector<DWORD> LST_DWORD;
    typedef std::vector<COLORREF> LST_COLORREF;

    LST_DWORD m_lstColTypes;
    LST_COLORREF m_lstColTextColors;
};

