////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//***************************************************************************
// CellBasedCCListView.h: interface for the CCellBasedCCListView class.
//***************************************************************************
#pragma once

#include "CustomCellListView.h"

class CCellBasedCCListView : public CCustomCellListView 
{
public:
    CCellBasedCCListView();
    virtual ~CCellBasedCCListView();

    BOOL GetCellType(INT iRow, INT iCol, DWORD *pdwType);
    BOOL GetCellTextColor(INT iRow, INT iCol, COLORREF *pcrColor);

protected:
    typedef std::vector< std::vector<DWORD> > GRID_DWORD;
    typedef std::vector< std::vector<COLORREF> > GRID_COLORREF;

    GRID_DWORD m_gridCellTypes;
    GRID_COLORREF m_gridCellTextColors;
};

