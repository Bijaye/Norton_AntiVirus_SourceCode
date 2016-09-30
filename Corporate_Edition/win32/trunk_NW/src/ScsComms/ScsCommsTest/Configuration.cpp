// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//Configuration.cpp

#include "stdafx.h"
#include "Configuration.h"

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

//Return the member configuration map
StringMap& Configuration::getMap()
{
	return m_configurationMap;
}

//Return one value from the member configuration map
//If the value doesn't exist, return NULL
std::string Configuration::getValue( const std::string& key )
{
	StringMap::iterator mapIterator;

	mapIterator = m_configurationMap.find( key );

	//If our iterator equals end(), then the key did not exist in the map
	if ( mapIterator != m_configurationMap.end() )
	{
		return mapIterator->second;
	}
	else
	{
		return "";
	}
}

//Return one value from the member configuration map
//If the value doesn't exist, return the default value that gets passed in
//This version avoids exceptions when the caller passes this result into a method without checking
//if its return is NULL.
std::string Configuration::getValue( const std::string& key, const char* defaultValue )
{
	StringMap::iterator mapIterator;

	mapIterator = m_configurationMap.find( key );

	//If our iterator equals end(), then the key did not exist in the map
	if ( mapIterator != m_configurationMap.end() )
	{
		return mapIterator->second;
	}
	else
	{
		return defaultValue;
	}
}

//Read in the configuration file and place it in the member map
BOOL Configuration::read()
{
	const char*			CONFIG_FILE_NAME = "ScsCommsTestConfig.txt";
		  char			lineFromFile[1024];
		  std::ifstream configFile( CONFIG_FILE_NAME );

	//If the config file is null, then it wasn't found.  We are reading parameters in from the
	//command line
	if ( ( configFile != NULL ) )
	{
		if ( configFile.good() )
		{
			//While we are not at the end of file, read in lines asd store them
			while( ! configFile.eof() )
			{
				configFile.getline( lineFromFile, 1024 );

				//Skip comments and blank strings.
				//Comments start with the # character
				if ( ( *lineFromFile == '#' ) ||
					 ( *lineFromFile == '\0' ) )
				{
					continue;
				}

				parseKeyValuePair( lineFromFile );
			}

			return TRUE;
		}
		else
		{
			std::cout << CONFIG_FILE_NAME << " was found, but there was an error opening it.";
		}
	}

	return FALSE;
}

//Write the member map out to the configuration file
BOOL Configuration::write() const
{
	return FALSE;
}

//Parse the key/value pair for a configuration line and store it in our member map
void Configuration::parseKeyValuePair( const char* const lineFromFile )
{
	//Put the line into a string object for each tokenizing
	std::string line = lineFromFile;

	unsigned int equalsPosition = line.find( "=" );
	unsigned int lineLength	    = line.length();

	//If the = character was not found, this line is not a well formed value pair, so inform the user
	//and ignore it.  If it was found, we have a good key/value pair
	if ( equalsPosition == std::string::npos )
	{
		std::cout << "Illegal line was found in configuraion file, ignoring: " << line;
	}
	else
	{
		std::string key   = line.substr( 0, equalsPosition );
		std::string value = line.substr( equalsPosition + 1, lineLength );

		//Insert the key value pair into the member map
		m_configurationMap.insert( StringPair( key, value ) );
	}
}