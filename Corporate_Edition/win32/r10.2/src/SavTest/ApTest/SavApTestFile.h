// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
//////////////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>
#include "SavApMetadata.h"

#define METADATA_SUFFIX "_metadata.xml";

//Actions available for files
enum FILEACTION
{
	ACTION_COPY,
	ACTION_CLOSE,
	ACTION_DELETE,
	ACTION_EXECUTE,
	ACTION_MOVE,
	ACTION_OPEN,
	ACTION_READ,
	ACTION_RENAME
};

/** 
 * Encapsulates information and actions for a file under test
 * Allowing default implementations of Assignment Operator and Copy Constructor
 */
class SavApTestFile
{
public:
	SavApTestFile( void );
	SavApTestFile( LPCTSTR name );
	SavApTestFile( LPCTSTR name, INFECTIONTYPE infection, VIRUSTYPE virus );
	virtual ~SavApTestFile( void );

	virtual LPCTSTR GetName() const { return m_name.c_str(); }
	virtual void SetName( LPCTSTR name ) { m_name = name; }
	virtual SavApMetadata& GetMetadata() { return m_metadata; }
	virtual DWORD ParseMetadata();
	virtual LPCTSTR GetExtension() const;
	virtual bool GetModified() const { return m_modified; }
	virtual void SetModified( bool modified ) { m_modified = modified; }

	virtual DWORD Copy( LPCTSTR destination, BOOL failIfExists = FALSE );
	virtual DWORD Close();
	virtual DWORD Delete();
	virtual DWORD Execute();
	virtual DWORD Move( LPCTSTR destination );
	virtual DWORD Open();
	virtual DWORD Read( unsigned int count = 10 );
	virtual DWORD Rename( LPCTSTR newName );

protected:
	FILE* m_file;
	std::string m_name;
	SavApMetadata m_metadata;
	bool m_modified;
};