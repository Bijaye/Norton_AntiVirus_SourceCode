//------ pragmas ------------------------------------------------------
#pragma warning ( disable : 4786 ) // identifier truncated

#include "base.h"
#include "TransactionExecutor.h"


/*****/
/* Execute the request. Content may be specified to be sent. Will open the Http request,
   then add any headers specified in the constructor, then call the StartRequest()
   method. StartRequest() is a virtual method which can be overriden if the default
   does not suffice. The default will simply perform a default SendRequest(), sending
   no content and receiving no content.

*/
int TransactionExecutor::Execute(
  int cbSendContent,    // how much data to send
  PVOID pvSendContent)  // data to be sent
{
  std::string   _gatewayError;
  DWORD         _serverSaid;
  DWORD         _errCode;

  // create a new request (an HttpFile object).  This must be done to ensure that
  // headers added to a previous request do not carry over when reusing that
  // request (the HttpFile object). Since there is no automatic way to clear the
  // header from the request, we must simply delete the old and create a new one.
  // i.e. a new request/HttpFile object must be created each time the transaction
  // is executed (each time this method is called).
  _errCode = OpenRequest();

  /*
   * Did the open work?
   */
  if (AVISTX_SUCCESS != _errCode)
  {
    _tc.error("error occurred opening request");

    /*
     * Don't call EndRequest() here, the request was never created,
     * and it fails ungracefully.
     */

    /*
     * This method, and most of the rest of the code, uses DWORDs and
     * ints interchangeably.  This sort of behaviour should not be
     * tolerated in a rational society, but there's nothing I can do
     * about it now.  Hence, close eyes, assume the brace position,
     * and cast away.
     */
    return (int)_errCode;
  }

  // allow derived classes to do any initialization before the request is executed
  _tc.enter("initializing request...");
  int rc = InitRequest();
  _tc.exit("initialized request");
  if (AVISTX_SUCCESS != rc)
  {
    _tc.error("error occurred while initializing request");
    EndRequest();
    return rc;
  }

  _tc.debug("executing request...");

  AddRequestHeaders();
  // virtual call; should be overriden ONLY if need to send request content
  // data in blocks; default sends the request with the specified request
  // content, if any
  _tc.enter("starting request...");
  rc = StartRequest(cbSendContent, pvSendContent);
  _tc.exit("started request");

  // don't need to read response content if error during request
  if (rc != AVISTX_SUCCESS)
  {
    _tc.error("error occurred while starting request");
    EndRequest();
    return rc;
  }

  // check for the "X-Error" header
  if ( GetErrorString( _gatewayError ) )
  {
    _tc.error("gateway returned error: %s", _gatewayError.c_str() );
    EndRequest();
    return AVISTX_GATEWAY_ERROR;
  }

  _serverSaid = GetResponseStatusCode();

  if ((HTTP_STATUS_MOVED           == _serverSaid) ||
      (HTTP_STATUS_REDIRECT        == _serverSaid))
  {
    rc = AVISTX_YOUVE_BEEN_REDIRECTED;
  }
  // Only need to read response content if http return code says 'ok' and there
  // is a 'content-length' header present.
  else if ((_serverSaid == HTTP_STATUS_OK) && (GetResponseContentLength() != -1))
  {
    // virtual call; should be overriden ONLY if need to read response content
    // default does nothing
    _tc.debug("reading response content...");
    rc = ReadResponseContent();
    //_tc.debug("read response content");
  }
  else if (HTTP_STATUS_NO_CONTENT == _serverSaid)
  {
    _tc.debug("no content in response");
  }
  else /* Some other code, not allowed. */
  {
    rc = AVISTX_CRITICAL_ERROR;
    _tc.debug("unexpected error [%d] whilst attempting to communicate with gateway", _serverSaid);
  }


  EndRequest();
  return rc;
}



/*****/
DWORD TransactionExecutor::InitRequest()
{
  // does nothing
  return AVISTX_SUCCESS;
}


/*****/
/* This function performs a WinInet SendRequest(), which will send any content 
   specified, in one large block write.  Override this function in a derived class 
   in order to send content in blocks. */
int TransactionExecutor::StartRequest(
  DWORD cbSendContent,  // how much data to send
  PVOID pvSendContent)  // data to send
{
  return SendRequest(cbSendContent, pvSendContent);
}
/*****/
void TransactionExecutor::EndRequest()
{
}


/*****/
int TransactionExecutor::ReadResponseContent()
{
  return AVISTX_SUCCESS;
}
