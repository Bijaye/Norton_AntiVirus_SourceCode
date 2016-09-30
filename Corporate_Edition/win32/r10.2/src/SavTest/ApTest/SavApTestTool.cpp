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
#include "savaptesttool.h"

SavApTestTool::SavApTestTool(void)
{
}

SavApTestTool::~SavApTestTool(void)
{
}

SYMTESTRESULT SavApTestTool::Configure()
{
	SYMTESTRESULT ret = ERROR_SUCCESS;

	//TODO: A Reference to a real TestData instance for walking files and reading in metadata
	m_testData = "C:\\Documents and Settings\\patrick_coyne\\Desktop\\New Folder\\";
	return ret;
}

//Called by TestCase::Execute()
//TODO: A Reference to a real TestData instance for walking files and reading in metadata
SYMTESTRESULT SavApTestTool::Configure( LPCSTR testData, SavApSettingsAdapter* settings )
{
	SYMTESTRESULT ret = ERROR_SUCCESS;

	return ret;
}

//Called by TestCase::Execute()
SYMTESTRESULT SavApTestTool::Execute()
{
	SYMTESTRESULT ret = ERROR_SUCCESS;
	
	EnumerateFiles( m_testData );

	SavApFileWalker walker;
	//TODO: For each FileAction in a list, add file actions to the walker
	walker.AddFileAction( ACTION_OPEN );
	walker.AddFileAction( ACTION_READ );
	walker.AddFileAction( ACTION_CLOSE );
	walker.AddTestFiles( &m_testFiles );
	
	walker.Execute();

	return ret;
}

//Called by TestCase::Execute()
SYMTESTRESULT SavApTestTool::VerifyResults()
{
	SYMTESTRESULT ret = SYMTEST_GENERIC_ERROR;

	for ( TestFileIter iter = m_testFiles.begin(); iter != m_testFiles.end(); ++iter )
	{
		if ( FileIsClean( *iter )								|| //File clean?
		 ( ! ApIsEnabled() )									|| //AP not enabled?
		 ( ! FileIsIncluded( *iter ) )							|| //File generally included?
		 FileIsExcluded( *iter )								|| //File specifically excluded?
		 ( ! NetworkScanIsEnabled() && FileIsNetwork( *iter ) ) || //Network scan disabled and this is a network file?
		 ( ! ApTriggerOnRead() && ! AttemptedToModifyFile( *iter ) ) ) //AP triggers on modify only and file wasn't modified?
		{
			//In all the above situations, no scan occurred.
			//If the file is unchanged, we passed.
			if ( FileIsUnchanged( *iter ) )
				ret = SYMTEST_SUCCESS;
		}
		else
		{
			LPCTSTR firstAction = 0;
			LPCTSTR secondAction = 0;

			if ( FileIsMacro( *iter ) )
			{
				m_productSettings->GetString( "MacroFirstAction", firstAction );
				m_productSettings->GetString( "MacroSecondAction", secondAction );
			}
			else
			{
				m_productSettings->GetString( "NonMacroFirstAction", firstAction );
				m_productSettings->GetString( "NonMacroSecondAction", secondAction );
			}

			if ( firstAction )
				ret = VerifyAction( firstAction );

			if ( SYMTEST_SUCCESS != ret && secondAction )
				VerifyAction( secondAction );
		}

	}
	return ret;
}

/*
SYMTESTRESULT SavAPTestTool::VerifyAction( FILEACTION action )
{
	SYMTESTRESULT ret = SYMTEST_GENERIC_ERROR;

	switch ( action )
	{
		case ACTION_LOGONLY:
			if ( FileUnchanged() && ActionLogged( action ) )
				ret = SYMTEST_SUCCESS;
			break;
		case ACTION_CLEAN:
			if ( FileRepairable() && FileRepaired() && ActionLogged( action ) )
				ret = SYMTEST_SUCCESS;
			break;
		case ACTION_QUARANTINE:
			if ( FileQuarantinable() && FileQuarantined() && ActionLogged( action ) )
				ret = SYMTEST_SUCCESS;
			break;
		case ACTION_DELETE:
			if ( FileDeletable() && FileDeleted() && ActionLogged( action ) )
				ret = SYMTEST_SUCCESS;
			break;
		case default:
	}

	return ret;
}
*/

/**
 * Instantiate a TestFile object for each file in a directory.  For now, ignore subdirectories.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApTestTool::EnumerateFiles( LPCTSTR directory )
{
	DWORD ret = ERROR_SUCCESS;
	WIN32_FIND_DATA fileFindData;
	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	std::string directoryContents( directory );
	
	//The wildcard is required for FindFirstFile
	directoryContents += "*";

	debug << "DEBUG: SavApFileWalker::EnumerateFiles() about to find first file in  " <<  directoryContents.c_str() << std::endl;

	//Get a handle to the first file or subdirectory in our directory
	fileHandle = FindFirstFile( directoryContents.c_str(), &fileFindData );
	
	//Continue running actions as long as there are more files in the directory
	do
	{
		//If we've got an invalid file handle, its unrecoverable.  Set the
		//error code and return
		if ( INVALID_HANDLE_VALUE == fileHandle )
		{
			std::cout << "Error: failed to locate file in " << directory << "." << std::endl;
			ret = GetLastError();
			break;
		}

		//Ignore subdirectories for now
		if ( fileFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			continue;

		//Put together a string that consists of the file's name and its full path
		//Use it to construct a SavApTestFile
		std::string fullyQualifiedFileName( directory );
		fullyQualifiedFileName += fileFindData.cFileName;

		debug << "DEBUG: SavApFileWalker::EnumerateFiles() about to construct SavApTestFile for  " <<  fullyQualifiedFileName.c_str() << std::endl;

		SavApTestFile file( fullyQualifiedFileName.c_str() );
		ret = file.ParseMetadata();

		//If we fail to parse the associated metadata file, no verification can be done.
		//Log it and try the next one.
		if ( ERROR_SUCCESS == ret )
		{
			try
			{
				m_testFiles.push_back( file );
				debug << "DEBUG_SUCCESS: SavApFileWalker::EnumerateFiles() added test file " << directory << std::endl;
			}
			catch( std::bad_alloc& )
			{
				ret = ERROR_NOT_ENOUGH_MEMORY;
				std::cout << "Error: failed to add test file to vector: " << directory << std::endl;
			}
		}
		else
		{
			std::cout << "Error: failed to parse metadata for file: " << fullyQualifiedFileName << std::endl;
			continue;
		}

	} while ( 0 != FindNextFile( fileHandle, &fileFindData ) );

	return ret;
}

bool SavApTestTool::FileIsClean( SavApTestFile& file )
{
	return INFECTION_CLEAN == file.GetMetadata().GetInfectionType();
}

bool SavApTestTool::ApIsEnabled()
{
	bool ret = false;
	DWORD value = 0;

	if( ERROR_SUCCESS == m_productSettings->GetDword( "OnOff", &value ) )
		if ( 1 == value )
			ret = true;

	return ret;
}

bool SavApTestTool::FileIsIncluded( SavApTestFile& file )
{
	bool ret = false;
	DWORD dwValue = 0;
	LPCTSTR pszValue = 0;

	if( ERROR_SUCCESS == m_productSettings->GetDword( "FileType", &dwValue ) )
		if ( 0 == dwValue )
			ret = true;
	else
	{
		if ( ERROR_SUCCESS == m_productSettings->GetString( "Exts", pszValue ) )
		{
			std::string extensions( pszValue );

			if ( -1 != extensions.find( file.GetExtension() ) )
				ret = true;
		}
	}

	return ret;
}

bool SavApTestTool::FileIsExcluded( SavApTestFile& file )
{
	bool ret = false;
	DWORD dwValue = 0;
	LPCTSTR pszValue = 0;

	if( ERROR_SUCCESS == m_productSettings->GetDword( "ExcludedByExtensions", &dwValue ) )
	{
		if ( 1 == dwValue )
		{
			if ( ERROR_SUCCESS == m_productSettings->GetString( "ExcludedExtensions", pszValue ) )
			{
				std::string extensions( pszValue );

				if ( -1 != extensions.find( file.GetExtension() ) )
					ret = true;
			}
		}
	}
	else
	{
		std::string fileExceptionsKey( "FileExceptions\\" );
		fileExceptionsKey += file.GetName();

		if ( ERROR_SUCCESS == m_productSettings->GetDword( fileExceptionsKey.c_str(), &dwValue ) )
			ret = true;

		//TODO: Directory Exclusions
		
	}

	return ret;
}

bool SavApTestTool::NetworkScanIsEnabled()
{
	bool ret = false;
	DWORD value = 0;

	if( ERROR_SUCCESS == m_productSettings->GetDword( "Networks", &value ) )
		if ( 1 == value )
			ret = true;

	return ret;
}

/**
 * Check if a test file is on a remote drive.
 * @return true or false
 */
bool SavApTestTool::FileIsNetwork( SavApTestFile& file )
{
	bool ret = false;
	std::string name( file.GetName() );

	//Look for a \\ in the file name that would indicate a UNC path
	if ( -1 != name.find( "\\\\" ) )
		ret = true;
	else
	{
		int index = name.find_last_of( "\\" );

		name = name.substr( 0, index );

		//Check if the file is on a mapped drive
		if ( DRIVE_REMOTE == GetDriveType( name.c_str() ) )
			ret = true;
	}

	return ret;
}

bool SavApTestTool::ApTriggerOnRead()
{
	bool ret = false;
	DWORD value = 0;

	if( ERROR_SUCCESS == m_productSettings->GetDword( "Reads", &value ) )
		if ( 1 == value )
			ret = true;

	return ret;
}

bool SavApTestTool::AttemptedToModifyFile( SavApTestFile& file )
{
	bool ret = false;

	if ( file.GetModified() )
		ret = true;

	return ret;
}

bool SavApTestTool::FileIsUnchanged( SavApTestFile& file )
{
	//TODO
	return true;
}

bool SavApTestTool::FileIsMacro( SavApTestFile& file )
{
	return VIRUS_MACRO == file.GetMetadata().GetVirusType();
}

SYMTESTRESULT SavApTestTool::VerifyAction( LPCTSTR action )
{
	//TODO
	return SYMTEST_SUCCESS;
}

void SavApTestTool::EnableAP( bool enable )
{
	if ( enable )
		m_productSettings->Set( "OnOff", ( DWORD ) 1 );
	else
		m_productSettings->Set( "OnOff", ( DWORD )0 );

	Sleep( 5000 );
}