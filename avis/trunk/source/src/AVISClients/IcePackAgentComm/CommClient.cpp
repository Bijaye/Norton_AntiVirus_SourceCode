//
//  IBM AntiVirus Immune System
//
//  File Name:  CommClient.cpp
//  Author:   Milosz Muszynski
//
//  This class is based on the Thread class
//  and provides a thread for a specific communication function.
//  This is a pure abstract class, intended to be subclassed by
//  concrete sample or signature related communication classes.
//  This class provides methods to control behavior of a thread
//  performing communication functions.
//  This class also maintains data common for all communication tasks
//  like gateway URL, agent name, response headers, etc.
//
//  $Log: $
//


// ------ headers -----------------------------------------------------
//#include "stdafx.h"
#include "base.h"
//#include "Transactions.h"
#include "CommClient.h"
#include "ZExcept.h"
#include <string>
#include "StrUtil.h"
#include "Config.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#include <time.h>

/* ----- */

/*
 * WAKE UP AND READ ME.
 *
 * This is the list of endings which are acceptable as gateway
 * names.  IcePack will refuse to talk to a gateway if the
 * double-resolved name (see comments on Transaction::doDoubleLookup())
 * does not end with one of these strings.
 *
 * Searching is done in a very dumb way, because this list is short.
 * It is NOT INTENDED that you put the full name of every gateway
 * in here (besides, in these times of gateway arrays, that is not
 * possible).  All your gateways should belong to the same domain --
 * I am assuming that this is 'dis.symantec.com'.
 *
 * If you do not both (a) follow the rules with gateway names; and
 * (b) have correct forward and reverse DNS entries for all machines
 * in all gateways, then you deserve what you will get.
 *
 * In addition, this list must end with a NULL entry.  If this is
 * removed, the code walking the list won't notice that it's shot
 * over the end and into witchspace.
 */
static const char * const s_gatewayEndsWith[] =
{
  ".dis.symantec.com",
  ".watson.ibm.com",
  NULL
};

static void initialiseRandometer();

// ------ CommClient --------------------------------------------------


//
// constructor
//

CommClient::CommClient(const std::string&                   logFileName,
                       const std::string&                   traceFileName,
                       const std::string&                   agentName,
                       RawEvent&                            eventStop,
                       MyTraceClient&                       traceClient,
                       const std::string&                   target,
                       const HttpConnection::connectionVerb verb)
  : 
    Thread(traceClient),
    _logFileName          (logFileName),
    _traceFileName        (traceFileName),
    _agentName            (agentName),
    _gatewayURL           (),
    _responseHeaders      (),
    _tc                   (traceClient),
    _eventGo              (),
    _eventDone            (),
    _eventTerminate       (),
    _eventStop            (eventStop ),
    _errorCode            (IcePackAgentComm::NoError),
    _systemExceptionCode  (0),
    _useSSL               (false),
    _defaultTarget        (target),
    _defaultVerb          (verb)
{
  _tc.enter("Entering CommClient constructor...");
  _tc.exit("Leaving CommClient constructor.");
}

/* ----- */

//
// destructor
//

CommClient::~CommClient()
{
  _tc.enter("Entering CommClient destructor...");
  _tc.exit("Leaving CommClient destructor.");
}

/* ----- */

//
// Go
//

void CommClient::Go()
{
  if (StrUtil::containsAnyCase(_gatewayURL, CFG_secure_http_URL_prefix))
    _useSSL = true;
  else
    StrUtil::makeSureStartsWith(_gatewayURL, CFG_default_URL_prefix);

  _eventGo.set();
}

/* ----- */

//
// WaitForGo
//

bool CommClient::WaitForGo()
{
  bool terminated = _eventGo.wait(_eventTerminate); // returns true if terminated

  if (terminated)
    _tc.debug("terminated client for %s", _gatewayURL.c_str());
  else
    _tc.msg("started client for %s", _gatewayURL.c_str());

  return terminated;
}

/* ----- */

/*
 * Launch() is a new method, and is the single point through which
 * all different transaction types (based on classes derived from
 * CommClient) are sent.  Classes derived from CommClient are
 * required to provide a method called _reallyLaunch(), which this
 * method uses.
 *
 * Launch handles redirection.  That's why it's here.  However,
 * note that it does not support partial-URL redirection.  Such
 * redirection is outside the bounds of the HTTP specification
 * anyway, but is usually supported by browsers.  However, it
 * would be a lot of work to support it, and it's not going to
 * happen anyway (we control the server).
 */
IcePackAgentComm::ErrorCode CommClient::Launch()
{
  IcePackAgentComm::ErrorCode    retVal;
  std::string                    copyOfInboundResponseHeaders;
  HttpConnection::connectionVerb verb;
  std::string                    target;
  std::string                    gateway;
  std::string                    origPort;
  std::string                    origProtocol;
  std::string                    newGateway;
  std::string                    newHostName;
  std::string                    origHostName;
  int                            q;
  int                            r;
  int                            howManyTimesAround = 0;
  const int                      maxTimesAround     = 4;
  
  /*
   * Because _responseHeaders is actually used for the request
   * headers as well as the response headers, it's necessary
   * to save the request headers before issuing the first
   * request.  We will need to put them back in the event of
   * a redirect.
   */
  copyOfInboundResponseHeaders = _responseHeaders;

  /*
   * Initialise these with the defaults for this transaction.
   */
  gateway = _gatewayURL;
  target  = _defaultTarget;
  verb    = _defaultVerb;

  /*
   * Determine what the original port number is.  origPort
   * will include the colon!  First, find the last colon,
   * then take a substring from there.
   *
   * Purists will note that, in the event that the inbound gateway
   * does not include a colon, origPort will be left empty.  This
   * is by design -- the first request will fail, and execute
   * will proceed through the correct error path.
   */
  q = gateway.rfind(":");
  if (std::string::npos != q)
    origPort = gateway.substr(q);

  /*
   * Same again with the protocol.
   */
  r = gateway.find("://");
  if (std::string::npos != r)
    origProtocol = gateway.substr(0, r+3);

  /*
   * Preserve the original machine name.  This is useful when it
   * comes to handling the round robin addresses, which we will
   * do in a moment.
   *
   * Note that we then copy the original gateway name into the
   * same variable that we'll use to hold the temporary gateway
   * names as we go around the loop.  Yet another string copy, I know,
   * I know.
   */
  origHostName = gateway.substr((r + 3), (q - r - 3));
  newHostName  = origHostName;

  /*
   * Start up the randometer.
   */
  initialiseRandometer();

  do
  {
    std::string s;
    std::string newTarget;
    std::string scratch;

    /*
     * Keep the loop counter current.
     */
    howManyTimesAround++;

    /*
     * Use the location we just found to extract the gateway
     * and target strings.
     */
    scratch = doDoubleLookup(newHostName);
    if (0 == scratch.size())
    {
      /*
       * It's all gone horribly wrong.  Drop a log message,
       * set the return code, and escape.
       */
      _tc.error("Failed to do double resolve of %s, giving up.", newHostName.c_str());
      retVal = IcePackAgentComm::NetworkCriticalError;
      break;
    }

    gateway = origProtocol + scratch + origPort;

    /*
     * Attempt to do the work...
     */
    retVal = _reallyLaunch(gateway, target, verb);

    /*
     * If it wasn't a redirect, escape.
     */
    if (IcePackAgentComm::YouveBeenRedirected != retVal)
      break;

    /*
     * Prepare for the worst...
     */
    retVal = IcePackAgentComm::InvalidRedirectError;

    /*
     * We got redirected -- find out where to go.
     * Look for the Location: header.
     */
    q = _responseHeaders.find("Location: ");
    if (std::string::npos == q)
      break;

    /*
     * Move past the Location: string.
     */
    q += strlen("Location: ");

    /*
     * Now look for the newline.
     */
    r = _responseHeaders.find("\r", q);
    if (std::string::npos == r)
      break;

    /*
     * Grab the string between the two points (this is the redirect URL).
     */
    s = _responseHeaders.substr(q, (r - q));
    if (0 == s.length())
      break;

    /*
     * Now, in s, we have the full redirect URL.  Do the split.
     * Find the double slash (as in https://), move one past that,
     * and find the next single slash.
     */
    q = s.find("//");
    if (std::string::npos == q)
      break;
    q += 2;
    r = s.find("/", q);
    if (std::string::npos == r)
      break;

    /*
     * Use the location we just found to extract the gateway
     * and target strings.
     */
    newHostName = s.substr(q, (r - q));
    if (0 == gateway.length())
      break;
    newTarget  = s.substr(r);
    if (0 == target.length())
      break;

    /*
     * Find the colon in the gateway name.  If there is one,
     * then remove it, and everything after it.
     */
    q = newHostName.find(":");
    if (std::string::npos != q)
      newHostName = newHostName.substr(0, q);

    /*
     * Prepend the original protocol type, and append the
     * original port number.
     */
    newGateway = origProtocol + newHostName + origPort;

    /*
     * Restore the copy of the so-called 'response' headers
     * we made earlier, and reset the error code so we go
     * round again...
     */
    _responseHeaders = copyOfInboundResponseHeaders;
    retVal = IcePackAgentComm::YouveBeenRedirected;

    /*
     * Ummmm... are the new things the same as the old things?
     * If they are, that means we have pre-detected a redirection
     * loop, and can give up.  Set the loop counter to the maximum
     * (to trigger the conditional message), and escape from the
     * loop.
     */
    if ((gateway == newGateway) && (target == newTarget))
    {
      howManyTimesAround = maxTimesAround;
      break;
    }

    /*
     * Let the log-file know what's happening.
     */
    _tc.msg("Redirecting to '%s'.", s.c_str());

    /*
     * Everything is okay.  Copy the new fields over, and go around
     * again.
     */
    gateway = newGateway;
    target  = newTarget;
  }
  while ((IcePackAgentComm::YouveBeenRedirected == retVal) && (howManyTimesAround < maxTimesAround));

  /*
   * If we got an invalid redirection, comment on it.
   */
  if (IcePackAgentComm::InvalidRedirectError == retVal)
    _tc.error("Received an invalid redirection response.");
  /*
   * Or maybe we kept going around and around and around...
   */
  else if (! (howManyTimesAround < maxTimesAround))
  {
    _tc.error("Circular redirect encountered!");
    retVal = IcePackAgentComm::InvalidRedirectError;
  }

  /*
   * JD.
   */
  return retVal;
}

/* ----- */

//
// Wait
//

void CommClient::Wait()
{
  _eventDone.wait();
}

/* ----- */

//
// Done
//

void CommClient::Done()
{
  _eventDone.set();
}

/* ----- */

//
// Terminate
//

void CommClient::Terminate()
{
  _eventTerminate.set();
}

/* ----- */

//
// Terminated
//

//bool CommClient::Terminated()
//{
//  return _eventTerminate.isSet();
//}

/* ----- */

//
// ThreadProc
//

DWORD CommClient::ThreadProc(LPVOID)
{
  ZExceptionTranslator exceptionTranslator;
  exceptionTranslator.init();

  try
  {
    /*
     * This is an ugly hack to attempt to get rid of log files appearing
     * when they shouldn't.  See config.h for more information.
     */
    if (_logFileName != "")
      _tc.RegisterThread( _logFileName, _traceFileName, _agentName );

    _tc.info("begin -------------------------------------------------");
    //_tc.msg("started client for %s", _gatewayURL.c_str() );

    _errorCode = ClientMain();

    _tc.debug( "terminating" );
    _tc.UnregisterThread();

    Done();

  }
  catch( InternetException& exc )
  {
    _errorCode        = IcePackAgentComm::LibraryException;
    _systemExceptionCode  = exc.m_dwError;
    _tc.critical( "InternetException caught: %d", exc.m_dwError );
    _tc.debug( "terminating" );
    _tc.UnregisterThread();
    Done();
  }
//  catch( AVISException& exc )
//  {
//    _errorCode        = IcePackAgentComm::LibraryException;
//    _systemExceptionCode  = exc.GetCause();
//    _tc.critical( "AVISException caught: %s", exc.DetailedInfo().c_str() );
//    Done();
//  }
//  catch( CAVISTransactionException& exc)
//  {
//    _errorCode        = IcePackAgentComm::LibraryException;
//    _systemExceptionCode  = exc.GetCause();
//    _tc.critical( "CAVISTransactionException caught: %s", exc.GetReasonText() );
//    Done();
//  }
  catch( ZException& exc )
  {
    _errorCode        = IcePackAgentComm::SystemException;
    _systemExceptionCode  = exc.code();
    _tc.critical( "system exception caught - code:%8.8x", _systemExceptionCode );
    _tc.debug( "terminating" );
    _tc.UnregisterThread();
    Done();
  }
  catch(...)
  {
    _errorCode        = IcePackAgentComm::LibraryException;
    _systemExceptionCode  = 0;
    _tc.critical("an exception of unknown type occurred.");
    _tc.debug( "terminating" );
    _tc.UnregisterThread();
    Done();
  }

  return (unsigned int)_errorCode;
}

/* ----- */

//
// GetAgentName
//

const char *CommClient::GetAgentName()
{
  return _agentName.c_str();
}

/* ----- */

/*
 * This function is called by CommClient::Launch().  If
 * configured to do so at compile-time (via CALL_SRAND),
 * it attempts to select a suitable seed for the random
 * number generator (i.e. parameter for srand()), and then
 * calls srand() with it.
 *
 * It doesn't return anything.
 *
 * The default (as per CALL_SRAND) is not to do anything.
 * This is because in tests [inw 2000-06-04], not calling
 * srand() resulted in a much more even gateway load than
 * calling it.  This is a bit odd, but it could well be
 * related to calling it too many times (basically once
 * per call to rand()!).  At any rates, in my tests, not
 * calling srand() results in an approximately even spread
 * between the two gateways pointed to by the test round-robin
 * DNS entry.
 *
 * [inw 2000-06-04]
 */
static void initialiseRandometer()
{
#ifdef CALL_SRAND
  DWORD         threadID;
  DWORD         procID;
  time_t        currentTime;
  unsigned int  num;

  /*
   * Get certain system information.
   */
  threadID    = GetCurrentThreadId();
  procID      = GetCurrentProcessId();
  time(&currentTime);

  /*
   * Use an essentially arbitrary algorithm to combine
   * the three pieces of information.
   */
  num = ((currentTime ^ (threadID * 1000000)) ^ (procID * 1000));

  /*
   * Use the combination to spin up the randometer.
   */
  srand(num);
#endif /* defined(CALL_SRAND) */

  /*
   * JD.
   */
  return;
}

/* ----- */

/*
 * This method prevents SSL connection setup from going wrong.  The problem
 * is related to the use of round-robin DNS, coupled with the use of SSL
 * for authentication.
 *
 * Picture the scene -- the Symantec forward-DNS entries look like this:
 *
 *     entry.cupertino.gateways.dis.symantec.com.   IN   A  10.1.1.1
 *     entry.reston.gateways.dis.symantec.com.      IN   A  10.1.1.2
 *     entry.london.gateways.dis.symantec.com.      IN   A  10.1.1.3
 *     entry.moscow.gateways.dis.symantec.com.      IN   A  10.1.1.4
 *     entry.ulan-bator.gateways.dis.symantec.com.  IN   A  10.1.1.5
 *
 *     entry.gateways.dis.symantec.com              IN   A  10.1.1.1
 *     entry.gateways.dis.symantec.com              IN   A  10.1.1.2
 *     entry.gateways.dis.symantec.com              IN   A  10.1.1.3
 *     entry.gateways.dis.symantec.com              IN   A  10.1.1.4
 *     entry.gateways.dis.symantec.com              IN   A  10.1.1.5
 *
 * And the Symantec reverse-DNS entries (for the 10.1.1. subnet)
 * look like this:
 *
 *     1    IN   PTR  entry.cupertino.gateways.dis.symantec.com.
 *     2    IN   PTR  entry.reston.gateways.dis.symantec.com.
 *     3    IN   PTR  entry.london.gateways.dis.symantec.com.
 *     4    IN   PTR  entry.moscow.gateways.dis.symantec.com.
 *     5    IN   PTR  entry.ulan-bator.gateways.dis.symantec.com.
 *
 * Now, when IcePack attempts to connect to the registry-configured
 * value (entry.gateways.dis.symantec.com), it would be round-robined
 * to one of the five entry-point machines.  It will get (say) 10.1.1.4,
 * and will connect to entry.moscow.gateways.dis.symantec.com.  It will
 * then do an SSL handshake, discover that the machine's certificate
 * is for entry.moscow.gateways.dis.symantec.com, but it (IcePack)
 * thought it was connecting to entry.gateways.dis.symantec.com.
 * The hostname check fails.  This is Bad[TM].
 *
 * And so it was that we decided to do a double lookup.  First,
 * we translate the gateway name (from the registry) into a number.
 * then we translate that number into a name.  This, in the above
 * example, goes like this:
 *
 *  entry.gateways.dis.symantec.com -> 10.1.1.4 -> entry.moscow.gateways[...]
 *
 * There appears to be some oddities of the round-robin support --
 * if I (programmatically) lookup entry.gateways.dis.symantec.com
 * 20 times in succession, I get the same IP every time.  Consequently,
 * I have implemented round-robin manually (randomly).  This may get
 * layered on top of real round-robin, but I can't convince myself
 * that will matter -- in fact, this is why the number in the list of
 * available addresses is chosen randomly.
 *
 * inw 2000-03-14.
 */
std::string CommClient::doDoubleLookup(std::string gateway)
{
  std::string     realName;
  std::string     endOfRealName;
  struct hostent *hostInfo;
  int             lenRealName;
  int             lenMustEnd;
  int             match;
  int             num;
  int             i;
  WSADATA         wsa;
  int             wsaStartErr;
  int             wsInit;
  char            ipString[3+1+3+1+3+1+3+5]; /* XXX.XXX.XXX.XXX + safety */

  wsInit = 0;

  /*
   * Give up immediately if the gateway address is all
   * screwed up.
   */
  if (0 == gateway.size())
  {
    _tc.error("Unexpected lack of gateway address");
    goto bail;
  }

  /*
   * Attempt to initialise winsock.  The version number (2.0)
   * is essentially arbitrary here.  We know (from other
   * IcePack requirements) that it must be at least this!
   */
  wsaStartErr = WSAStartup(MAKEWORD(2, 0), &wsa);
  if (wsaStartErr)
  {
    _tc.error("Failed to initialise winsock [%d]", wsaStartErr);
    goto bail;
  }

  /*
   * Flag that we have started our winsock session.
   */
  wsInit = 1;

  /*
   * Call gethostbyname() on the gateway name, to
   * get back a HOSTENT structure.
   */
  hostInfo = gethostbyname(gateway.c_str());
  
  /*
   * Check to see if it worked.
   */
  if ((NULL == hostInfo) || (NULL == hostInfo->h_addr_list[0]))
  {
    _tc.error("Failed to determine IP address of '%s' [%ld]", gateway.c_str(), WSAGetLastError());
    goto bail;
  }

  /*
   * Check to see if we got a valid, IPv4, address.  We are heading
   * for the days of IPv6... having said that, we've been heading
   * that way for the last six years...
   */
  if ((AF_INET != hostInfo->h_addrtype) || (4 != hostInfo->h_length))
  {
    _tc.error("'%s' does not have an IPv4 address", gateway.c_str());
    goto bail;
  }

  /*
   * How many entries in the list (this doubles as a logging loop).
   */
  for (i = 0; hostInfo->h_addr_list[i]; i++)
  {
    sprintf(ipString, "%u.%u.%u.%u", (unsigned char)((char *)(hostInfo->h_addr_list[i])[0]),
                                     (unsigned char)((char *)(hostInfo->h_addr_list[i])[1]),
                                     (unsigned char)((char *)(hostInfo->h_addr_list[i])[2]),
                                     (unsigned char)((char *)(hostInfo->h_addr_list[i])[3]));

    _tc.info("  [%d] %s", i, ipString);
  }

  /*
   * Pick a 'random' number from 0 to (i - 1) in
   * the approved fashion.
   *
   * Note that as the random number generator has not
   * been seeded, this number won't be even a little bit
   * random.  That doesn't matter.
   */
  num = (int)(((float)i) * rand() / (RAND_MAX + 1.0));

  /*
   * Work this out for later...
   */
  sprintf(ipString, "%u.%u.%u.%u", (unsigned char)((char *)(hostInfo->h_addr_list[num])[0]),
                                   (unsigned char)((char *)(hostInfo->h_addr_list[num])[1]),
                                   (unsigned char)((char *)(hostInfo->h_addr_list[num])[2]),
                                   (unsigned char)((char *)(hostInfo->h_addr_list[num])[3]));

  /*
   * Drop a message showing the IP address.
   */
  _tc.info("Chosen IP address '%s' [0 <= %d <= %d]", ipString, num, (i - 1));

  /*
   * Now, attempt to use the address at offset num to get a name.  Thus, we are
   * getting the 'primary' name of ONE of the gateway machines pointed to by
   * the (possibly) round-robined registry names.  Clear?
   */
  hostInfo = gethostbyaddr((const char *)hostInfo->h_addr_list[num], 4, AF_INET);

  /*
   * Check to see if we got a valid name back.
   */
  if ((NULL == hostInfo) || (NULL == hostInfo->h_name) || (0 == hostInfo->h_name[0]))
  {
    _tc.error("'%s' does not have a DNS name", ipString);
    goto bail;
  }

  /*
   * Copy the name from the HOSTENT structure into a nice fluffy std::string.
   */
  realName = hostInfo->h_name;

  /*
   * Another helpful message.
   */
  _tc.info("Resolved '%s' into '%s'", ipString, realName.c_str());

  /*
   * Get the length of the real name we just got back.
   */
  lenRealName = realName.size();

  /*
   * Walk through the list of gateways, looking for one that has
   * the right ending.
   */
  match = 0;
  for (i = 0; s_gatewayEndsWith[i]; i++)
  {
    const char * const name = s_gatewayEndsWith[i];

    /*
     * Get the length of this ending.
     */
    lenMustEnd  = strlen(name);

    /*
     * If the name is longer than the supposed suffix, try to match.
     */
    if (lenRealName > lenMustEnd)
    {
      /*
       * Now, copy the end of the real name.  Use the number of characters
       * required to match the suffix.
       */
      endOfRealName = realName.substr((lenRealName - lenMustEnd), lenMustEnd);

      /*
       * Compare the known-good suffix with the end of the name from DNS.
       * If it doesn't match, then this is not the ending we are looking for.
       */
      if (! stricmp(name, endOfRealName.c_str()))
      {
        match = 1;
        break;
      }
    }
  }

  /*
   * Did we get a match?
   */
  if (! match)
  {
    _tc.error("'%s' is not in the correct domain!", realName.c_str());
    realName = "";
    goto bail;
  }

  /*
   * Yadda yadda.
   */
  _tc.info("Derived valid gateway name of '%s', which will be used for this transaction.", realName.c_str());

bail :

  /*
   * Make sure to drop our winsock connection.
   */
  if (wsInit)
    WSACleanup();

  /*
   * JD.
   */
  return realName;
}
