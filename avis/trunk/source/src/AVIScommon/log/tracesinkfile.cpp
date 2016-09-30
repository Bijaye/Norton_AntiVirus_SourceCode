// tracesinkfile.cpp - Implementation of the TraceSinkFile concrete class

#include "stdafx.h"
//

#include "tracesinkfile.h"
#include "strutil.h"
#include "lock.h"
#include "namedmutex.h"
#include <fstream>
#include <string>


TraceSinkFile::TraceSinkFile(const char* tfile_,
                             bool hourly_rollover_)
  :_hourly_rollover(hourly_rollover_),
   _mutex(0),
   _filename(tfile_),
   _base_file(tfile_)
{

  //----------------------------------------
  // set the _time var to the current time
  //----------------------------------------
  struct tm *_timestamp = 0;
  long _time_as_long = 0;

  // Get the current time
  time(&_time_as_long);

  // Convert the long to a struct tm*
  _timestamp = localtime(&_time_as_long);

  // Assign it into the object scope var;
  _time = (*_timestamp);
  //----------------------------------------

  _filename = create_filename();

  // Create the mutex based on the new filename, not the base, and not 
  // the character string

  // Set the internal _time var;
  
  // Create the "real" filename

  // Create the mutex object for this filename
  // I don't like the way this code looks, but it's been working, so
  // I'm not going to change it at this stage, things are just a
  // little too hectic at the moment.

  if (tfile_)
    {
      std::string _mutexname = tfile_;

      // build the mutex
      // The new name will be:

      // The file name as specified with...

      //  all "-" characters removed
      _mutexname = StringUtil::removeAll(_mutexname,'-');

      //  all ":" characters removed
      _mutexname = StringUtil::removeAll(_mutexname,':');

      //  all '\' characters removed
      _mutexname = StringUtil::removeAll(_mutexname,'\\');

      // forced to lowercase
      _mutexname = StringUtil::strlwr(_mutexname);

      _mutex = new NamedMutex(_mutexname.c_str());
    }
}
TraceSinkFile::~TraceSinkFile()
{
  delete _mutex;
  _mutex = 0;
}

std::string TraceSinkFile::generate_timestamp() const
{

  std::string _timestamp;

  //------------------
  // Build the "MM"
  //------------------
  // Add the month (it comes back zero relative)
  if ((_time.tm_mon + 1) < 10)
    {
      _timestamp.append("0");
    }
  _timestamp.append(StringUtil::itos(_time.tm_mon + 1));
  //------------------
  
  _timestamp.append("-");

  //------------------
  // Build the "DD"
  //------------------

  if (_time.tm_mday < 10)
    {
      _timestamp.append("0");
    }
  _timestamp.append(StringUtil::itos(_time.tm_mday));
  
  // conditionally append "-HH" depending on the value of the
  // _hourly_rollover flag up in object scope

  if (_hourly_rollover)
    {
      //------------------
      // Build the "HH"
      //------------------
      _timestamp.append("-");
      if (_time.tm_hour < 10)
        {
          _timestamp.append("0");
        }
      _timestamp.append(StringUtil::itos(_time.tm_hour));
    }

  //------------------
  return _timestamp;
}

std::string TraceSinkFile::create_filename() const
{
  std::string _new_filename("");

  // parse the existing _base_file apart.
  
  std::string::size_type i = _base_file.find(".");

  std::string _stamp(generate_timestamp());

  if (i != std::string::npos)
    {
      _new_filename = _base_file.substr(0,i);
    }
  else
    {
      _new_filename = _base_file;
    }

  _new_filename.append(".");
  _new_filename.append(_stamp);
  _new_filename.append(".");
  
  if (i != std::string::npos)
    {
      _new_filename.append(_base_file.substr((i + 1)));
    }
  
  // now new filename has the first "." of base_file replaced with
  // ".<stamp>." where <stamp> is either "MM-DD" or "MM-DD-HH"

  return _new_filename;
}

bool TraceSinkFile::rollover_test() 
{
  //----------------------------------------
  // set the _time var to the current time
  //----------------------------------------
  struct tm *_timestamp = 0;
  long _time_as_long = 0;

  // Get the current time
  time(&_time_as_long);

  // Convert the long to a struct tm*
  _timestamp = localtime(&_time_as_long);
  
  bool _changed = false;
  if (_hourly_rollover)
    {
      _changed = (_timestamp->tm_hour != _time.tm_hour);
    }
  else
    {
      _changed = (_timestamp->tm_mday != _time.tm_mday);
    }

  // Hold on to the last time we checked.
  _time = (*_timestamp);

  return _changed;
}
void TraceSinkFile::sendTraceLine(const char* message_)
{
  Lock _filelock(*_mutex);
  // Check for change of day

  if (rollover_test())
    {
      _filename = create_filename();
    }

  std::ofstream _tracefile(_filename.c_str(),std::ios::app);
  _tracefile << message_ << std::endl;
  _tracefile.close(); 
}


