// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//ConfigReader.h

#ifndef SCS_COMMS_TEST_CONFIGURATION_H
#define SCS_COMMS_TEST_CONFIGURATION_H

#include <fstream>
#include <iostream>
#include <map>
#include <string>

typedef std::map<std::string, std::string> StringMap;
typedef std::pair<std::string, std::string> StringPair;

class Configuration
{
public:
	Configuration();
	~Configuration();
	virtual StringMap& getMap();
	virtual std::string getValue( const std::string& key );
	virtual std::string getValue( const std::string& key, const char* defaultValue );
	virtual BOOL read();
	virtual BOOL write() const;

private:
	StringMap m_configurationMap;
	void parseKeyValuePair( const char* const );
};

#endif //SCS_COMMS_TEST_CONFIGURATION_H