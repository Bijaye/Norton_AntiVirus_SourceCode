// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// MSIWrapper.h : header file
//

#pragma once

#include <windows.h>
#include "msi.h"

/////////////////////////////////////////////////////////////////////////////
// CMSIWrapper
// See MSIWrapper.cpp for the implementation of this class
//

class CMSIWrapper
{
public:
	CMSIWrapper();
	CMSIWrapper( MSIHANDLE );
	CMSIWrapper( CString );
	virtual ~CMSIWrapper();

	UINT openDatabase();
	UINT closeDatabase();

	inline MSIHANDLE getDatabaseHandle() { return m_hDatabase; };
	inline CString getFilename() { return m_strFilename; };
	void setFilename( CString& );
	
	virtual UINT openTable( CString strQuery = "" )=0;
	virtual UINT getNext()=0;
	virtual UINT closeTable()=0;
	virtual int count()=0;

protected:
	inline void setDatabaseHandle( MSIHANDLE hDatabase ) { m_hDatabase = hDatabase; };
	UINT msiOpenTable( MSIHANDLE *, CString& );
	UINT msiCloseTable( MSIHANDLE );
	int countRecords( MSIHANDLE );

private:
	CString m_strFilename;
	PMSIHANDLE m_hDatabase;
};
