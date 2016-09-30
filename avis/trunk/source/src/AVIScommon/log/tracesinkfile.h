// tracesinkfile.h - Prototype of the TraceSinkFile concrete class

#if !defined (__TRACESINKFILE_H__)
#define __TRACESINKFILE_H__

#include "aviscommon.h"
#include "tracesink.h"
#include <string>
#include <time.h>

class NamedMutex;

class AVISCOMMON_API TraceSinkFile : public TraceSink
{
public:
  //: Construct with filename
  TraceSinkFile(const char* tfile_,
                bool hourly_rollover_ = true);

  //: Virtual Destructor
  virtual ~TraceSinkFile();

  // Implementation of TraceSink pure virtual interface
  virtual void sendTraceLine(const char*);

private:

  // this will generate a filename based on the time value
  // in _time.  It relies on two things (for performance, not as a
  // true precondition):
  // 1) that a separate function has determined that a new filename
  //    NEED be created (i.e. the date has changed)
  // 2) that the object-scope _time var already have been updated.

  std::string create_filename() const;


  // generate_timestamp generates either "MM-DD-HH" or "MM-DD"
  // depending on the value of the _hourly_rollover flag in object
  // scope below.  This value is used in create_filename().
  std::string generate_timestamp() const;

  // rollover_test returns true if we've rolled past the rollover time
  // (be it top of the hour or end of day)
  bool rollover_test();

  // bool hour_change() const returns true if we've rolled past
  // an hour since the last change. Either way, it resets the _time var
  bool hour_change();

  // If this flag is true, 2 pieces of behavior change:
  // 1) The filenames will include "MM-DD-HH" instead of "MM-DD"
  // 2) New Files will be created at each hour (on demand only) not
  //    daily.
  bool _hourly_rollover;

  // This is actually timestamp of last date rollover NOT the current time
  struct tm _time;
  std::string _filename;
  std::string _base_file;
  NamedMutex* _mutex;

};

#endif // __TRACESINKFILE_H__ Sentinel

