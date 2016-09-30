////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// SymRegKeys.h: interface for the CSymRegKeys class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYMREGKEYS_H__8DBC81B1_143E_4DCE_95BE_A4869C6490FC__INCLUDED_)
#define AFX_SYMREGKEYS_H__8DBC81B1_143E_4DCE_95BE_A4869C6490FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Report.h"

class CSymRegKeys : public CReport  
{
public:
	bool CreateReport ( LPCTSTR lpcszFolderPath );
    CSymRegKeys();
	virtual ~CSymRegKeys();

};

#endif // !defined(AFX_SYMREGKEYS_H__8DBC81B1_143E_4DCE_95BE_A4869C6490FC__INCLUDED_)
