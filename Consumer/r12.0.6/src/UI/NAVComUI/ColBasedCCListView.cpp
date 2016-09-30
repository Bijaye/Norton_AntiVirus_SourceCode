//***************************************************************************
// ColBasedCCListView.cpp: implementation of the CColBasedCCListView class.
//***************************************************************************
#include "stdafx.h"
#include "ColBasedCCListView.h"

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
CColBasedCCListView::CColBasedCCListView() 
{
    m_lstColTypes.resize(10, 1);
    m_lstColTextColors.resize(10);
}

//***************************************************************************
// 
//***************************************************************************
CColBasedCCListView::~CColBasedCCListView()
{
}

//***************************************************************************
// 
//***************************************************************************
BOOL CColBasedCCListView::GetCellType(INT iRow, INT iCol, DWORD *pdwType)
{
    INT iRowCount = GetItemCount();

    if ( iRow >= iRowCount || iRow < 0 ) 
    {
        return FALSE;
    }
    
    return GetColumnType(iCol, pdwType);
}

//***************************************************************************
// 
//***************************************************************************
BOOL CColBasedCCListView::GetCellTextColor(INT iRow, INT iCol, 
                                           COLORREF *pcrColor)
{
    INT iRowCount = GetItemCount();

    if ( iRow > iRowCount || iRow < 0 ) 
    {
        return FALSE;
    }

    return GetColumnTextColor(iCol, pcrColor);
}

//***************************************************************************
//***************************************************************************
// 
//***************************************************************************
BOOL CColBasedCCListView::SetColumnType(INT iCol, DWORD dwType)
{
    INT iColumnCount = CHeaderCtrl(GetHeader()).GetItemCount();

    if (iCol > iColumnCount || iCol < 0) 
    {
        return FALSE;
    } 

    if (m_lstColTypes.size() < (size_t)iCol) 
    {
        m_lstColTypes.resize(iColumnCount + 10);
    }

    m_lstColTypes[iCol] = dwType;

    return TRUE;
}

//***************************************************************************
// 
//***************************************************************************
BOOL CColBasedCCListView::GetColumnType(INT iCol, DWORD *pdwType)
{
    INT iColumnCount = CHeaderCtrl(GetHeader()).GetItemCount();

    if ( iCol > iColumnCount || iCol < 0 ) 
    {
        return FALSE;
    }
    
    if(NULL == pdwType) {
        return FALSE;
    }

    if(m_lstColTypes.size() < (size_t)iCol) {
        m_lstColTypes.resize(iColumnCount + 10);
    }

    *pdwType = m_lstColTypes[iCol];

    return TRUE;
}

//***************************************************************************
// 
//***************************************************************************
BOOL CColBasedCCListView::SetColumnTextColor(INT iCol, COLORREF crColor)
{
    INT iColumnCount = CHeaderCtrl(GetHeader()).GetItemCount();

    if ( iCol > iColumnCount || iCol < 0 ) 
    {
        return FALSE;
    }

    if(m_lstColTextColors.size() < (size_t)iCol) {
        m_lstColTextColors.resize(iColumnCount + 10);
    }

    m_lstColTextColors[iCol] = crColor;

    return TRUE;
}

//***************************************************************************
// 
//***************************************************************************
BOOL CColBasedCCListView::GetColumnTextColor(INT iCol, COLORREF *pcrColor)
{
    INT iColumnCount = CHeaderCtrl(GetHeader()).GetItemCount();

    if ( iCol > iColumnCount || iCol < 0 ) 
    {
        return FALSE;
    }

    if(NULL == pcrColor) {
        return FALSE;
    }

    if(m_lstColTextColors.size() < (size_t)iCol) {
        m_lstColTextColors.resize(iColumnCount + 10);
    }

    *pcrColor = m_lstColTextColors[iCol];

    return TRUE;
}

