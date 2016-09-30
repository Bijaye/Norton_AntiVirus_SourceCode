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
#include ".\aptesttoolsettings.h"

ApTestToolSettings::ApTestToolSettings(void)
{
}

ApTestToolSettings::~ApTestToolSettings(void)
{
}

/**
 * Reads the list of actions into a member structure.  Expects the actions to be a comma-separated list.
 * @return ERROR_SUCCESS or the failure error code
 */
DWORD ApTestToolSettings::SetFileActions( LPCTSTR actions )
{
	DWORD ret = ERROR_SUCCESS;
	int index = 0;
	std::string actionsString( actions );
	std::string action;

	//No need to run on an empty list
	if ( actionsString.empty() )
	{
		std::cout << "Error: No actions were specified (ApTestToolSettings::SetFileActions()) ." << std::endl;
		ret = ERROR_INVALID_PARAMETER;
	}
	else
	{
		//Tokenize the actions and put them in the vector
		do
		{
			//Index of the first comma
			index = actionsString.find_first_of( "," );
			//The action at the start of the vector
			action = actionsString.substr( 0, index );
			//The rest of the vector
			actionsString = actionsString.substr( index + 1 );

			try
			{
				m_fileActions.push_back( action );
				debug << "DEBUG_SUCCESS: ApTestToolSettings::SetFileActions() added action " << action << std::endl;
			}
			catch( std::bad_alloc& )
			{
				ret = ERROR_NOT_ENOUGH_MEMORY;
				std::cout << "Error: failed to add file action to vector: " << action << std::endl;
			}
		}
		while ( -1 != index );
	}

	return ret;
}

/**
 * Return an interator to the beginning of the actions.
 * @return ERROR_SUCCESS or the failure error code
 */
ActionIter ApTestToolSettings::GetActionBegin()
{
	return m_fileActions.begin();
}

/**
 * Return an interator to the end of the actions.
 * @return ERROR_SUCCESS or the failure error code
 */
ActionIter ApTestToolSettings::GetActionEnd()
{
	return m_fileActions.end();
}
