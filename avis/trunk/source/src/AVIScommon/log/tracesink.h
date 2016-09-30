// tracesink.h - Prototype for the TraceSink abstract class

#if !defined (__TRACESINK_H__)
#define __TRACESINK_H__


//: This just defines the simple interface for sending a single line to 
//: a trace "sink" or destination.
// There just ain't nothin else to it.

// Note that any implementation of the TraceSink interface is fully
// responsible for all thread safety, and concrete construction and
// destruction semantics.  TraceSink objects will be passed by
// reference only into the MTrace class.  It is the responsibility of
// the client programmer to make sure these objects are not
// prematurely destroyed or otherwise invalidated.

#include "aviscommon.h"

class AVISCOMMON_API TraceSink
{
public:
  virtual void sendTraceLine(const char*) = 0;
};

#endif // __TRACESINK_H__ Sentinel
