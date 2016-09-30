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
#include "StdAfx.h"
#include ".\SavApTestFile.h"

SavApTestFile::SavApTestFile( void ):
m_file( 0 ),
m_name( 0 ),
m_metadata(),
m_modified( false )
{
}

SavApTestFile::SavApTestFile( LPCTSTR name ):
m_file( 0 ),
m_name( name ),
m_metadata(),
m_modified( false )
{
}

SavApTestFile::SavApTestFile( LPCTSTR name, INFECTIONTYPE infection, VIRUSTYPE virus ):
m_file( 0 ),
m_name( name ),
m_metadata( infection, virus ),
m_modified( false )
{
}

SavApTestFile::~SavApTestFile( void )
{
}

/**
 * Opens the file and save the pointer.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::Open()
{
	DWORD ret = ERROR_SUCCESS;

	if ( NULL == ( m_file = fopen ( m_name.c_str(), "r" ) ) )
	{
		ret = ERROR_FILE_NOT_FOUND;
		debug << "DEBUG_FAIL: SavApTestFile::Open() on " << m_name << std::endl;
	}
	else
		debug << "DEBUG_SUCCESS: SavApTestFile::Open() on " << m_name << std::endl;

	return ret;
}

/**
 * Close the file and invalidate the pointer.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::Close()
{
	DWORD ret = ERROR_SUCCESS;

	if ( m_file )
	{
		if ( EOF == fclose( m_file ) )
		{
			ret = ERROR_HANDLE_EOF;
			debug << "DEBUG_FAIL: SavApTestFile::Close() on " << m_name << std::endl;
		}
		else
		{
			ret = ERROR_INVALID_HANDLE;
			debug << "DEBUG_SUCCESS: SavApTestFile::Close() on " << m_name << std::endl;
		}

		//Whether fclose fails or not, our pointer is invalid
		m_file = NULL;
	}
	else
	{
		ret = ERROR_INVALID_HANDLE;
		debug << "DEBUG_FAIL: SavApTestFile::Close() had invalid handle for " << m_name << std::endl;
	}

	return ret;
}

/**
 * Read a few bytes from the file.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::Read( unsigned int count )
{
	DWORD ret = ERROR_SUCCESS;

	if ( m_file )
	{
		//Allocate a buffer for the read
		char* szBuf = new char[count];

		//Read
		fread( reinterpret_cast<void *>( szBuf ), sizeof( char ), count, m_file );

		//Release buffer
		delete[] szBuf;

		debug << "DEBUG_SUCCESS: SavApTestFile::Read() on " << m_name << ", count was " << count << std::endl;
	}
	else
	{
		ret = ERROR_INVALID_HANDLE;
		debug << "DEBUG_FAIL: SavApTestFile::Read() had invalid handle for " << m_name << std::endl;
	}

	return ret;
}

/**
 * Copy the file to the specified destination.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::Copy( LPCTSTR destination, BOOL failIfExists )
{
	DWORD ret = ERROR_SUCCESS;

	if ( ! CopyFile( m_name.c_str(), destination, failIfExists ) )
	{
		ret = GetLastError();
		debug << "DEBUG_FAIL: SavApTestFile::Copy() on " << m_name << "Code: " << ret << std::endl;
	}
	else
		debug << "DEBUG_SUCCESS: SavApTestFile::Copy() on " << m_name << " to " << destination << std::endl;

	return ret;
}

/**
 * Move the file to the specified destination.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::Move( LPCTSTR destination )
{
	DWORD ret = ERROR_SUCCESS;

	if ( ! MoveFile( m_name.c_str(), destination ) )
	{
		ret = GetLastError();
		debug << "DEBUG_FAIL: SavApTestFile::Move() on " << m_name << " Code: " << ret << std::endl;
	}
	else
	{
		m_name = destination; //Save the new name
		debug << "DEBUG_SUCCESS: SavApTestFile::Move() on " << m_name << " to " << destination << std::endl;
	}

	return ret;
}

/**
 * Rename the file.  This will move it (with a new name) instead if 
 * a different folder is specified in the new name.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::Rename( LPCTSTR newName )
{
	DWORD ret = ERROR_SUCCESS;

	//rename() returns 0 if successful.  GetLastError() can't be used with it.
	if ( rename( m_name.c_str(), newName ) )
	{
		ret = ERROR_ACCESS_DENIED;
		debug << "DEBUG_FAIL: SavApTestFile::Rename() on " << m_name << " to " << newName << std::endl;
	}
	else
	{
		m_name = newName; //Save the new name
		debug << "DEBUG_SUCCESS: SavApTestFile::Rename() on " << m_name << " to " << newName << std::endl;
	}

	return ret;
}

/**
 * Execute the file, as if from the Run prompt.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::Execute()
{
	DWORD ret = ERROR_SUCCESS;

	//GetLastError() can't be used with ShellExecute()
	if ( 32 >= ( int ) ShellExecute( NULL, NULL, m_name.c_str(), NULL, NULL, SW_SHOWNORMAL ) )
	{
		ret = ERROR_ACCESS_DENIED;
		debug << "DEBUG_FAIL: SavApTestFile::Execute() on " << m_name << std::endl;
	}
	else
		debug << "DEBUG_SUCCESS: SavApTestFile::Execute() on " << m_name << std::endl;

	return ret;
}

/**
 * Delete the file.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::Delete()
{
	DWORD ret = ERROR_SUCCESS;

	if ( ! DeleteFile( m_name.c_str() ) )
	{
		ret = GetLastError();
		debug << "DEBUG_FAIL: SavApTestFile::Delete() on " << m_name << " Code: " << ret << std::endl;
	}
	else
	{
		debug << "DEBUG_SUCCESS: SavApTestFile::Delete() on " << m_name << std::endl;
		m_file = NULL; //The file is deleted, our pointer is longer valid
		
	}

	return ret;
}

/**
 * Delete the file.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestFile::ParseMetadata()
{
	DWORD ret = ERROR_SUCCESS;

	//The associated metadata file has the same name with a suffix
	std::string metadataName( m_name );
	metadataName += METADATA_SUFFIX;

	ret = m_metadata.Parse( metadataName.c_str() );

	return ret;
}

LPCTSTR SavApTestFile::GetExtension() const
{
	int index = 0;
	std::string extension;

	index = m_name.find_last_of( "." );
	extension = m_name.substr( index );

	return extension.c_str();
}