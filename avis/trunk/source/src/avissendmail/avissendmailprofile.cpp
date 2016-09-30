// AVISSendMailProfile.cpp: implementation of the CAVISSendMailProfile class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning (disable:4786 )

#include "stdafx.h"
#include "AVISSendMailProfile.h"

#include "cfgprof.h"

#include "utilexception.h"

// 
ConfigProfile* CAVISSendMailProfile::s_pCfgProfile = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISSendMailProfile::CAVISSendMailProfile( )
{



}

CAVISSendMailProfile::~CAVISSendMailProfile( )
{



}

BOOL CAVISSendMailProfile::OpenProfile( LPCSTR pszProfileFileName )
{

	try
	{

		// create new Cofig Profile Object
		s_pCfgProfile = new ConfigProfile( pszProfileFileName );
		
		return TRUE;


	}
	catch(BadFilename& e_)
    {

        std::string _msg("Bad Filename:");

        _msg += e_.what();

  		return FALSE;

    }

}

void CAVISSendMailProfile::CloseProfile( )
{

	// make sure we have it before trying to delete it
	if ( s_pCfgProfile != NULL )
		delete s_pCfgProfile;

	//
	s_pCfgProfile = NULL;

}

std::string CAVISSendMailProfile::GetStringValue( LPCSTR pszKey, const std::string& strDefault )
{

	std::string strResult;


	try
	{
		// look for the key in the hash
		strResult = (*s_pCfgProfile)[pszKey];

	}
	catch ( InvalidConfigVariable& e_)
	{
		// see if we found it

        std::string _msg("Bad Configfile Variable:");

        _msg += e_.what();
		
		// not found, set to the default
		strResult = strDefault;

	} // end set to default
	
	return strResult;

}

int CAVISSendMailProfile::GetIntValue( LPCSTR pszKey, int iDefault )
{

	// set to default for starters
	int iResult = iDefault;

	std::string strValue;

	try
	{

		// look for the key string in the hash
		strValue = (*s_pCfgProfile)[pszKey];

		if ( strValue.size() != 0 )
		{
			// found it, change to integer
			iResult = ::atoi(strValue.c_str() );

		}


	}
	catch ( InvalidConfigVariable& e_)
	{

        std::string _msg("Bad ConfigFile Value:");

        _msg += e_.what();

		iResult = iDefault;

	}

	return iResult; 

}
