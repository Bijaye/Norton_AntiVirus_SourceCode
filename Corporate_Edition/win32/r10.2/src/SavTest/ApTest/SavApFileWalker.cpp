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
#include ".\SavApFileWalker.h"

SavApFileWalker::SavApFileWalker(void): 
m_actions(0)
{
}

SavApFileWalker::~SavApFileWalker(void)
{
}

/**
 * Add an action to the file action sequence.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApFileWalker::AddFileAction( FILEACTION action )
{
	DWORD ret = ERROR_SUCCESS;

	try
	{
		m_actions.push_back( action );
		debug << "DEBUG_SUCCESS: SavApFileWalker::AddFileAction() added action " << action << std::endl;
	}
	catch( std::bad_alloc& )
	{
		ret = ERROR_NOT_ENOUGH_MEMORY;
		std::cout << "Error: failed to add FILEACTION to vector: " << action << std::endl;
	}

	return ret;
}

/**
 * Start execution.  Walks through each file, performing actions.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApFileWalker::Execute()
{
	DWORD ret = ERROR_SUCCESS;
	int fileCount = 0;
	
	//Continue running actions as long as there are more test files
	for ( TestFileIter iter = m_testFileVectorPtr->begin(); iter != m_testFileVectorPtr->end(); ++iter )
	{
		++fileCount;

		//Run the sequence of actions for our file.  Don't check the return code because these actions
		//are expected to fail as AP blocks access to infected files
		RunActions( *iter );
	}

	std::cout << fileCount << " files walked" << std::endl;

	return ret;
}

/**
 * Run the sequence of actions on a given file.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD SavApFileWalker::RunActions( SavApTestFile& file )
{
	DWORD ret = ERROR_SUCCESS;
	std::string originalName( file.GetName() );

	debug << "DEBUG: SavApFileWalker::RunActions() for  " <<  file.GetName() << std::endl;

	//Create a "new" file name based on the original for copying and renaming
	//ex. c:\file.txt becomes c:\file.txt.new
	std::string newName = originalName;
	newName.append( ".new" );

	//Create a "new" location that includes the original file name for moving
	//ex. c:\virs\vir.com will move to c:\vir.com
	std::string newLocation( "c:\\" );
	int index = originalName.find_last_of( "\\" ) + 1;
	std::string fileName = originalName.substr( index );
	newLocation.append( fileName );

	//Take each action in sequence
	for ( FileActionIterator it = m_actions.begin(); it != m_actions.end(); ++it )
	{
		//The iterator points to an enumeration, which are switchable
		switch ( ( FILEACTION ) *it )
		{
			case ACTION_COPY:
				//Make a copy of the file with a new name
				ret = file.Copy( newName.c_str() );

				//Copying is modification
				if ( ERROR_SUCCESS == ret )
					file.SetModified( true );

				break;
			case ACTION_CLOSE:
				ret = file.Close();
				break;
			case ACTION_DELETE:
				ret = file.Delete();

				//Deleting is modification
				if ( ERROR_SUCCESS == ret )
					file.SetModified( true );

				break;
			case ACTION_EXECUTE:
				ret = file.Execute();
				break;
			case ACTION_MOVE:
				//Move the file to a new location.  If successful, move it
				//back afterwards for cleanliness
				ret = file.Move( newLocation.c_str() );
				if ( ERROR_SUCCESS == ret )
				{
					//Moving is modification
					file.SetModified( true );
					ret = file.Move( originalName.c_str() );
				}
				break;
			case ACTION_OPEN:
				ret = file.Open();
				break;
			case ACTION_READ:
				ret = file.Read();
				break;
			case ACTION_RENAME:
				//Rename the file to a new name.  If successful, rename it
				//back afterwards for cleanliness
				ret = file.Rename( newName.c_str() );
				if ( ERROR_SUCCESS == ret )
				{
					//Renaming is modification
					file.SetModified( true );
					ret = file.Rename( originalName.c_str() );
				}
				break;
			default:
				//Invalid file action
				ret = ERROR_INVALID_PARAMETER;
				std::cout << "Error: invalid file action: " << ( FILEACTION ) *it << "." << std::endl;
		}
	}

	return ret;
}