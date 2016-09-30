// ColumnInfo.cpp: implementation of the CColumnInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ColumnInfo.h"
#include "macros.h"
#include "mmc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL( CColumnInfo, CObject, 1 );



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CColumnInfo::CColumnInfo
//
// Description   : Constructor.
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CColumnInfo::CColumnInfo()
{
    // 
    // Set some reasonable defaults.
    // 
    m_dwSortColumn = 0;
    m_aColumnWidths.SetSize( MAX_RESULT_COLUMNS );

    for( int i = 0; i < MAX_RESULT_COLUMNS; i++ )
        {
        m_aColumnWidths[i] = MMCLV_AUTO;
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CColumnInfo::~CColumnInfo
//
// Description   : Destructor
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CColumnInfo::~CColumnInfo()
{
    // 
    // Cleanup.
    // 
    m_aColumnWidths.RemoveAll();
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CColumnInfo::Serialize
//
// Description   : Override of CObject::Serialize()
//
// Return type   : void 
//
// Argument      : CArchive & ar
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CColumnInfo::Serialize( CArchive & ar )
{
    if( ar.IsStoring() )
        {
        // 
        // Save off sort column.
        // 
        ar << m_dwSortColumn;

        // 
        // Save off column widths.
        //
        m_aColumnWidths.Serialize( ar );
        }
    else
        {
        // 
        // Load sort column.
        // 
        ar >> m_dwSortColumn;

        // 
        // Load column widths.
        // 
        m_aColumnWidths.Serialize( ar );
        }
}