// Copyright 1996 Symantec, Peter Norton Computing Group                  
////////////////////////////////////////////////////////////////////////  
// MSGWAIT.CPP
//   
// $Header:   S:/SYMKRNL/VCS/msgwait.cpv   1.2   27 Nov 1996 10:38:28   MSPIEGE  $ 
//   
// Description:
//   
//   This file contains wait functions that are safe for use in OLE/COM
//   threads.  The standard WaitForSingle/MultipleObject() calls cause
//   problems in OLE threads because CoInitialize() creates a hidden
//   window.  If messages get sent or otherwise broadcast to that window,
//   they have to be processed or the system will hang.  
//
//   
//   
// Contains:
//   SymMsgWaitSingle()
//   SymMsgWaitMultiple()
//   
// See Also:
//   KB Article: OLE Threads Must Dispatch Messages
//   MsgWaitForMultipleObjects()
////////////////////////////////////////////////////////////////////////  
// $Log:   S:/SYMKRNL/VCS/msgwait.cpv  $ 
// 
//    Rev 1.2   27 Nov 1996 10:38:28   MSPIEGE
// 1) Changed the way the SymMsgWaitMultiple() function treats its timeout
//    value to more closely approximate MsgWaitMultiple()
// 2) Removed the message loop and replaced it with a single call to 
//    PeekMessage() so that only SendMessage() messages will get processed.
//    See MSDN, Books .../Advanced Windows NT/Chapter 6/Thread Info...
// 
//    Rev 1.1   18 Sep 1996 14:52:56   BMCCORK
// Convered SymMsgWaitMultiple to return MsgWaitForMultipleObjects return codes
// 
//    Rev 1.0   17 Sep 1996 18:39:02   BMCCORK
// Initial revision.
////////////////////////////////////////////////////////////////////////  


#include "platform.h"

#ifdef SYM_WIN32                        // Only supported under Win32 environments

////////////////////////////////////////////////////////////////////////  
//   
//  SymMsgWaitMultiple()
//   
// Description:
//   
// Wiat for one of the objects given to be signaled, while allowing 
// messages to be dispatched.
// 
// Returns:
//     Returns the same values as MsgWaitForMultipleObjects() with fWaitAll = FALSE.
//     Will not return WAIT_OBJECT_0 + dwCount, since this function will perform a PeekMessage loop each
//     time there is input in the queue.
//   
// Return Values
//  If the function succeeds, the return value indicates the event that caused the function to return. 
//  The successful return value is one of the following: 
//  Value	                                            Meaning
//  WAIT_OBJECT_0 to (WAIT_OBJECT_0 + nCount - 1)       The return value minus WAIT_OBJECT_0 indicates the 
//                                                      lpHandles array index of the object that satisfied the wait. 
//  WAIT_ABANDONED_0 to (WAIT_ABANDONED_0 + nCount - 1) The return value minus WAIT_ABANDONED_0 indicates the lpHandles 
//                                                      array index of an abandoned mutex object that satisfied the wait.
//  WAIT_TIMEOUT	                                    The time-out interval elapsed and no objects were signaled.
//  
// If the function fails, the return valueis 0xFFFFFFFF. To get extended error information, call GetLastError. 
//
// See Also:
//   MsgWaitForMultipleObjects()
//   SymMsgWaitSingle() inline/macro
//   SYM_WAIT_XXX() and SYM_WAIT_GET_XXX() inline/macro helpers
//   
////////////////////////////////////////////////////////////////////////  
//  9/17/96 BEM Function created. 
////////////////////////////////////////////////////////////////////////  

// Wait for one of the given objects to be signaled, while allowing messages to be dispatched:
// Returns same as MsgWaitForMultipleObjects()
EXTERNC DWORD SYM_EXPORT WINAPI SymMsgWaitMultiple(
    DWORD dwCount,                      // Number of handles in lpHandles
    LPHANDLE lpHandles,                 // Array of handles - wiat for a signal from 
                                        // one of these. 
    DWORD dwMilliseconds)               // Timeout value (use INFINITE for none) 
{
    //MWS CODE CHANGE...11/25/96  This comment covers all individual changes.
    //Made the following modifications to this function.  First, it now manages
    //  the remaining timeout value when a message is received so that if a lot
    //  of messages are coming in, the function will stay with the spirit of 
    //  the original timeout.
    //  Second, there is no longer a message loop.  The only messages that we 
    //  are really concerned with here are messages received as a result of a 
    //  call to SendMessage().  Turns out that merely calling PeekMessage 
    //  causes these messages to be dispatched to the appropriate winproc() 
    //  without any further work on our part.  Also, by not processing messages
    //  that are not necessary to process, we are lowering the danger of 
    //  problems calling our code in a reentrant manner.  (Symdb at least!)  
    DWORD       dwRet;
    MSG         msg;
    DWORD       dwWaitRemaining = dwMilliseconds;
    __int64     i64StartTime = (__int64)GetTickCount();
    __int64     i64NowTime;
    
    while(TRUE)
        {
        dwRet = MsgWaitForMultipleObjects( 
            dwCount,                    // Number of events to wait for
            lpHandles,                  // The array of events
            FALSE,                      // Don't wait for all events since we need to 
                                        // know as soon as there are messages.
            dwWaitRemaining,            // Timeout value
            QS_ALLINPUT);               // Any message wakes up


        //Check and act on the return of the wait function.
        if (dwRet == WAIT_OBJECT_0 + dwCount)
            {
            //There is a window message available.  Note that due to the way
            //  that Win95 and WinNT appear to work, merely making the call
            //  to PeekMessage() causes a message created with SendMessage()
            //  to be dispatched to the appropriate winproc().  Since the real
            //  problem we are trying to address has to deal with messages
            //  recieved as a result of a call to SendMessage() we don't have 
            //  to do any more work.
            PeekMessage(&msg,NULL,(UINT)0,(UINT)0,PM_NOREMOVE);
            } 
        else 
            {
            // An event was signaled, wait was abandoned, or timeout
            return dwRet;
            } 

        //If the timeout is infinite, we can continue right away.  All the 
        //  subsequent tests and calc are meaningless.
        if (dwWaitRemaining == INFINITE)
            continue;

        //If dwWaitRemaining is 0, either the user was just testing the objects
        //  or we have already been here and the timeout has expired, but 
        //  someone has managed to stick another message in our queue while we 
        //  were deciding that the dwMilliseconds value should be 0 and we 
        //  retried the wait.  If someone is pounding us hard enough with 
        //  messages, this could happen not letting the  WaitForMultipleObjects
        //  call return timeout for an extended  period, not letting this 
        //  function return.  This is unlikely but possible, so we'll break and
        //  return WAIT_TIMEOUT forcing ourselves out to the caller.
        if (dwWaitRemaining == (DWORD)0)
            return WAIT_TIMEOUT;

        //Get the current tick count being careful to watch for a rollover.
        i64NowTime = (__int64)GetTickCount();
        if (i64NowTime < i64StartTime)
            i64NowTime += 0x100000000;

        //Recalculate a new time out value based on how much of the existing
        //  timeout was used.  If the timeout is exhausted, it may have happened
        //  due to the processing of messages so set it to 0 and try the 
        //  wait one more time.  That will give us the handles we are waiting on
        //  a fair chance at being signalled or abandoned.  
        if (i64NowTime >= i64StartTime + dwMilliseconds)
            {
            dwWaitRemaining = 0;
            }
        else
            {
            //NOTE: The right hand side of the subtraction can never be larger 
            //  than dwMilliseconds due to the inequality that gets us here.
            dwWaitRemaining = dwMilliseconds - (DWORD)(i64NowTime - i64StartTime);           
            }
        } 
}

#endif                                  // #ifdef SYM_WIN32
