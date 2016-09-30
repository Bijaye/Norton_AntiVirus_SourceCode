/////////////////////////////////////////////////////////////////////
// SERIAL.CPP - Created - 08/01/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/FILELINK/VCS/SERIAL.CPv   1.0   17 Mar 1998 19:45:18   SEDWARD  $
//
// Description:
//    This contains the implementation of the CLinkClientSerializer
//    class.
//
//    Please look in the Header file for an explanation of how to
//    use this class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/FILELINK/VCS/SERIAL.CPv  $
// 
//    Rev 1.0   17 Mar 1998 19:45:18   SEDWARD
// Initial revision.
// 
//    Rev 1.0   27 Jan 1998 12:42:20   KSACKIN
// Initial revision.
//
//    Rev 1.2   17 Dec 1997 17:28:00   DBuches
// Change _asm int 3 calls to DebugBreak().
//
//    Rev 1.1   02 Dec 1997 14:45:34   DSACKIN
// Removed SYM_ASSERTs
//
//    Rev 1.0   02 Dec 1997 10:44:16   DSACKIN
// Initial revision.
//
//    Rev 1.0   02 Aug 1997 02:39:20   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <platform.h>

#include "LinkCli.h"
#include "LinkVal.h"

#include "LinkInfo.h"
#include "Serial.h"


/////////////////////////////////////////////////////////////////////
// Macros

#ifdef _DEBUG
#define MY_ASSERT(xP) if( (LPVOID)xP == NULL ) DebugBreak();
#else
#define MY_ASSERT(xP)
#endif

/////////////////////////////////////////////////////////////////////
// Globals

                                        // This is the critical section used in
                                        // all of the communication.
CRITICAL_SECTION  gLinkCriticalSection;
CLinkClient      *glpcLinkClient = NULL;



/////////////////////////////////////////////////////////////////////
// CLinkSerializer()
//
// Purpose:
//    Constructor for the class.
//    Main purpose is to Enter the Critical Section
//
// Parameters:
//    none.
//
// Returns:
//    none
//
/////////////////////////////////////////////////////////////////////
// 08/01/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CLinkSerializer::CLinkSerializer()
{
                                        // Make sure that only ONE RPC call can
                                        // be made at any one time.
   EnterCriticalSection(&gLinkCriticalSection);
}

/////////////////////////////////////////////////////////////////////
// CLinkSerializer::~CLinkSerializer()
//
// Purpose:
//    This makes sure that we clean up and close our connection
//    before allowing the next function to make the RPC connection.
//
// Parameters:
//    none
//
// Returns:
//    none
//
/////////////////////////////////////////////////////////////////////
// 08/01/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CLinkSerializer::~CLinkSerializer()
{
   if (NULL != glpcLinkClient)
   {
      delete glpcLinkClient;
      glpcLinkClient = NULL;
   }
                                        // Don't forget to let the next RPC
                                        // call through.
   LeaveCriticalSection(
      &gLinkCriticalSection);
}


/////////////////////////////////////////////////////////////////////
// BOOL Connect
//
// Purpose:
//    This is the function that should be called to make the RPC
//    connection.  This will ensure that ONLY one function can
//    make an RPC call at any given time.
//
// Parameters:
//   PRPC_LINK_INFO   pLinkInfo - [in] - Where to connect to and how
//
// Returns:
//   TRUE  - We are connected.  Go ahead and make your calls
//   FALSE - Something happened to the connection.
//
/////////////////////////////////////////////////////////////////////
// 08/01/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CLinkSerializer::Connect(
   PRPC_LINK_INFO      pLinkInfo,
   RPC_BINDING_HANDLE* pBindingHandle)
{
BOOL bReturn = FALSE;

                                        // If this is NOT the case, someone
                                        // didn't clean up correctly.  Once we
                                        // acquire the Critical Section, this
                                        // variable NEEDS to be NULL.
   MY_ASSERT (NULL == glpcLinkClient);

                                        // Create a LinkClient that we can use
                                        // to connect to the remote machine.
   glpcLinkClient = new CLinkClient;

   if (!glpcLinkClient)
   {
      goto Cleanup;
   }
                                        // Start by setting the protocol
   glpcLinkClient->SetProtocol(
      pLinkInfo->pszProtocol,
      pLinkInfo->pszAddress,
      pLinkInfo->pszEndpoint);

   bReturn = glpcLinkClient->Connect(
                    pBindingHandle );


Cleanup:

   if (!bReturn)
   {
      if (NULL != glpcLinkClient)
      {
         delete glpcLinkClient;
         glpcLinkClient = NULL;
      }
   }

   return bReturn;
}



