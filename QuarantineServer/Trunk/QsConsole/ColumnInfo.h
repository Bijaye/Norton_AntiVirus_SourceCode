/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ColumnInfo.h: interface for the CColumnInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLUMNINFO_H__21B9086E_C046_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_COLUMNINFO_H__21B9086E_C046_11D2_8F46_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CColumnInfo : public CObject  
{
public:
	CColumnInfo();
	virtual ~CColumnInfo();
	void Init(int iNoColumns);
	int GetNoColumns(void) {return m_iNoColumns;};
// 
// Overrides
// 
public:
    virtual void Serialize( CArchive & ar );

public:
    DECLARE_SERIAL( CColumnInfo );

// 
// Data members.
// 
public:
    // 
    // Column to sort by.
    // 
    DWORD       m_dwSortColumn; 

	//
	// Flag to indicate if array has been init
	//
	DWORD		m_dwfInit;

    // 
    // Array of column widths.
    //
    CArray< DWORD, DWORD > m_aColumnWidths;

	//
	// number of columns
	//
	int			m_iNoColumns;
};

#endif // !defined(AFX_COLUMNINFO_H__21B9086E_C046_11D2_8F46_3078302C2030__INCLUDED_)
