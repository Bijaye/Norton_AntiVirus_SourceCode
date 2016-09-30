/////////////////////////////////////////////////////////////////////
// DLLMAIN.CPP - Created - 06-20-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVRPC/VCS/dllmain.cpv   1.0   23 Jun 1997 01:17:58   DSACKIN  $
//
// Description:
//    This is just the entry point for this DLL.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVRPC/VCS/dllmain.cpv  $
// 
//    Rev 1.0   23 Jun 1997 01:17:58   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"

/////////////////////////////////////////////////////////////////////
// Global Variables

HINSTANCE   ghInst = NULL;

/////////////////////////////////////////////////////////////////////
// Function Prototypes


/////////////////////////////////////////////////////////////////////
// DllMain
//
// Purpose:
//    This function is called when attaching or detaching a process
//    from this dll.
//
// Parameters:
//    HINSTANCE hInstDLL - [IN] Instance of this dll.
//    DWORD dwReason     - [IN] Reason why this function is being
//                              called.  This will be either
//                              DLL_PROCESS_ATTACH or
//                              DLL_PROCESS_DETACH.
//    LPVOID lpvReserved - [IN] Reserved value.
//
// Returns:
//    BOOLEAN as to the success of the function.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/29/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(
   HINSTANCE hInstDLL,
   DWORD dwReason,
   LPVOID lpvReserved)
{
BOOL bResult = TRUE;

                                        // Switch of reason for this call.
   switch (dwReason)
   {
      case DLL_PROCESS_ATTACH:

                                        // Take a copy in case we will need it
                                        // someday.
         ghInst = hInstDLL;

         break;

      case DLL_PROCESS_DETACH:
         break;
   }

   return bResult;
}

