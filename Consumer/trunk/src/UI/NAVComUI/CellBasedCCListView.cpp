////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//***************************************************************************
// CellBasedCCListView.cpp: implementation of the CCellBasedCCListView class.
//***************************************************************************
#include "stdafx.h"
#include "CellBasedCCListView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//***************************************************************************
// 
//***************************************************************************
CCellBasedCCListView::CCellBasedCCListView() 
{
    const int INITIAL_SIZE = 10;
    std::vector< DWORD > vTypePadding;
    std::vector< COLORREF > vColorPadding;

    m_gridCellTypes.resize(INITIAL_SIZE, vTypePadding);
    m_gridCellTextColors.resize(INITIAL_SIZE, vColorPadding);

    for (int counter = 0; counter < INITIAL_SIZE; counter++) {
       m_gridCellTypes[counter].resize(INITIAL_SIZE, 1);
       m_gridCellTextColors[counter].resize(INITIAL_SIZE);
    }
}

//***************************************************************************
// 
//***************************************************************************
CCellBasedCCListView::~CCellBasedCCListView()
{
}

//***************************************************************************
// 
//***************************************************************************
BOOL CCellBasedCCListView::GetCellType(INT iRow, INT iCol, DWORD *pdwType)
{
    INT iRowCount = GetItemCount();

    if ( iRow >= iRowCount || iRow < 0 ) 
    {
        return FALSE;
    }
    
    INT iColumnCount = CHeaderCtrl(GetHeader()).GetItemCount();

    if (iCol > iColumnCount || iCol < 0 ) 
    {
        return FALSE;
    }
    
    if(NULL == pdwType) {
        return FALSE;
    }

    if(m_gridCellTypes.size() < (size_t) iRow) {
        m_gridCellTypes.resize(iRowCount + 10);
    }

    if(m_gridCellTypes[iRow].size() < (size_t) iCol) {
        m_gridCellTypes[iRow].resize(iColumnCount + 10);
    }

    *pdwType = m_gridCellTypes[iRow][iCol];

    return TRUE;
}

//***************************************************************************
// 
//***************************************************************************
BOOL CCellBasedCCListView::GetCellTextColor(INT iRow, INT iCol, 
                                            COLORREF *pcrColor)
{
    INT iRowCount = GetItemCount();

    if ( iRow > iRowCount || iRow < 0 ) 
    {
        return FALSE;
    }

    INT iColumnCount = CHeaderCtrl(GetHeader()).GetItemCount();

    if ( iCol > iColumnCount || iCol < 0 ) 
    {
        return FALSE;
    }

    if(NULL == pcrColor) {
        return FALSE;
    }

    if(m_gridCellTextColors.size() < (size_t) iRow) {
        m_gridCellTextColors.resize(iRowCount + 10);
    }

    if(m_gridCellTextColors[iRow].size() < (size_t) iCol) {
        m_gridCellTextColors[iRow].resize(iColumnCount + 10);
    }

    *pcrColor = m_gridCellTextColors[iRow][iCol];

    return TRUE;
}

