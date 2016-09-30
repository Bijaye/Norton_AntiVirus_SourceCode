// cfgprof.cpp - Implementation of the ConfigProfile class

#include "stdafx.h"
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)

#include <fstream>

#include <windows.h>
#include <process.h>

#include "utilexception.h"

#include "strutil.h"
#include "stringmap.h"
#include "lock.h"    // Synchronization lock
#include "critsec.h"




#include "cfgprof.h"

using namespace std;

CriticalSection& ConfigProfile::_critsec = CriticalSection();

ConfigProfile::ConfigProfile(const char* filename_)
  :_configfile(""),
   _loadedOk(false)
{
  if (filename_)
    {
      _configfile = filename_;
      loadFile(filename_);
    }
}

bool ConfigProfile::validObject() const
{
  bool _vo = false;
  if (_loadedOk)
    _vo = true;
  return _vo;
}

ConfigProfile::~ConfigProfile()
{}
bool ConfigProfile::exists(const string& key_) const
{
  Lock _existsLock(_critsec);
  return (_hashTable.find(key_) != _hashTable.end());
}

string ConfigProfile::operator[](const string& key_) const
{
  string _retval = "";
  Lock _operatorLock(_critsec);
  if (exists(key_))
    _retval =  (*_hashTable.find(key_)).second;
  else
    throw InvalidConfigVariable(key_.c_str());
  return _retval;
}

bool ConfigProfile::loadFile(const char* filename_)
{
  
  Lock _loadfileLock(_critsec);
  bool _retval = false;
  // This will throw if the filename is bad
  try
    {
      _hashTable = StringMapUtil::fromConfigFile(filename_);
      _retval = true;
    }
  catch (BadFilename& e_)
    {
      _loadedOk = false;
      throw e_;
    }
  return _retval;
}

string ConfigProfile::toString() const
{
  Lock _toStringLock(_critsec);
  return StringMapUtil::toDebugString(_hashTable);
}
