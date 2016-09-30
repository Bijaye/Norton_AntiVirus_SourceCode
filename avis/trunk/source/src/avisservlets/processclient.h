// processclient.h - Prototype for the ProcessClient class
#if !defined (__PROCESSCLIENT_H__)
#define __PROCESSCLIENT_H__

#include "sessionclient.h"
class TraceClient;
class SessionHost;

class ProcessClient : public SessionClient
{
public:
  ProcessClient(SessionHost& host_);
  virtual ~ProcessClient();
  
  TraceClient& tracer() { return *_tracer; };
  
private:
  TraceClient* _tracer;

};
#endif // __PROCESSCLIENT_H__ Sentinel
