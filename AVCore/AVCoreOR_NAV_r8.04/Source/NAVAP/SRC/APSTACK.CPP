// Copyright 1997 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/apstack.cpv   1.3   16 Sep 1998 14:08:30   RCHINTA  $
//
// Description:
//      This file defines NAVAP's stack swapping functions.
//
// Contains:
//      StackFunction1()        (local function)
//      StackFunction2()        (local function)
//      StackFunction3()        (local function)
//      StackFunction4()        (local function)
//      StackFunction5()        (local function)
//      StackFunction6()        (local function)
//      StackFunction7()        (local function)
//      CallOnNAVAPStack()
//      LinkStack()
//      UnlinkStack()
//      StackCleanupForThread()
//
// See Also:
//      -
//
//***************************************************************************

#include "platform.h"
#include "navap.h"

//---------------------------------------------------------------------------
//
// Define stack swapping related constants.
//
// STACK_SIZE defines the length of the stack that NAVAP needs.
//
// STACK_TOP is a helper macro used locally to calculate the top of newly
// allocated stack with size STACK_SIZE.
//
// ALIGN_STACK is a helper macro for STACK_TOP.  The stack alignment for
// Alpha (NTK only) platforms needs to be paragraph (0x10), while the stack
// alignment for all other supported platforms needs to be DWORD (0x04).
//
//---------------------------------------------------------------------------
#define STACK_SIZE      0x3F80


#if defined(SYM_VXD)

#define STACK_TOP(p)    ( (PVOID) ( ALIGN_STACK(p) + STACK_SIZE - sizeof(STACK_LINK) ) )

#else

#define STACK_TOP(p)    ( (PVOID) ( ALIGN_STACK(p) + STACK_SIZE ) )

#endif


#if defined(SYM_NTK) && defined(_ALPHA_)

#define ALIGN_STACK(p)  ( (DWORD)p & 0xFFFFFFF0 )

#else

#define ALIGN_STACK(p)  ( (DWORD)p & 0xFFFFFFFC )

#endif

//---------------------------------------------------------------------------
//
// Define a structure for keeping stacks in a linked list and a pointer to
// the first linked stack.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

typedef struct tagSTACK_LINK
    {
    PVOID         pvReserved1;
    tagSTACK_LINK *prNext;
    PVOID         pvStack;
    PVOID         pvThread;
    PVOID         pvReserved2;
    } STACK_LINK, *PSTACK_LINK;

PSTACK_LINK prStackLink = NULL;

VOID LinkStack ( PVOID pvStack );
VOID UnlinkStack ( PVOID pvStack );

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

#else

#define LinkStack(x) ( (VOID)0 )
#define UnlinkStack(x) ( (VOID)0 )

#endif

//---------------------------------------------------------------------------
//
// Define a generic structure type for passing parameters from
// CallOnNAVAPStack() to StackFunction?().
//
//---------------------------------------------------------------------------
typedef struct
    {
    PVOID         pfnSwappedStackFunction;
    PBOOL         pbParam1;
    HEXCLUDEN     hExcludeN;
    PEDEXTRA      prEDExtra;
    REFDATA       uRefData;
    LPED          lpEventData;
    PNAVAP_CONFIG prConfig;
    PSTR          pszString;
    } SWAPPEDSTACKPARAMS;

//---------------------------------------------------------------------------
//
// VOID __cdecl StackFunction? (
//      PVOID pvParam
//      );
//
// Each one of these functions is called by one of the CallOnNAVAPStack()
// functions.  They all transfer control to the specified swapped stack
// function, passing it the parameters that were passed originally to
// CallOnNAVAPStack().
//
// Entry:
//      pvParam - pointer to a SWAPPEDSTACKPARAMS structure, which was set
//                up by one of the CallOnNAVAPStack() functions.  Since only
//                one StackFunction?() has a corresponding CallOnNAVAPStack()
//                function, there is no confusion on how the contents of the
//                members of that structure are inerpreted.
//
// Exit:
//      -
//
// Note:
//      The pvParams macro is used to access the structure at pvParam
//      without the need for typing an explicit type cast.
//
//---------------------------------------------------------------------------
#define prParams ( (SWAPPEDSTACKPARAMS*)pvParam)

VOID __cdecl StackFunction1 (
    PVOID pvParam
    )
    {
    ((PFNSWAPPEDSTACK1)pvParam)();
    }

VOID __cdecl StackFunction2 (
    PVOID pvParam
    )
    {
    ((PFNSWAPPEDSTACK2)prParams->pfnSwappedStackFunction) (
        prParams->pbParam1
        );
    }

VOID __cdecl StackFunction3 (
    PVOID pvParam
    )
    {
    ((PFNSWAPPEDSTACK3)prParams->pfnSwappedStackFunction) (
        prParams->hExcludeN
        );
    }

VOID __cdecl StackFunction4 (
    PVOID pvParam
    )
    {
    ((PFNSWAPPEDSTACK4)prParams->pfnSwappedStackFunction) (
        prParams->prEDExtra,
        prParams->uRefData,
        prParams->lpEventData
        );
    }

VOID __cdecl StackFunction5 (
    PVOID pvParam
    )
    {
    ((PFNSWAPPEDSTACK5)prParams->pfnSwappedStackFunction) (
        prParams->pbParam1,
        prParams->prEDExtra,
        prParams->uRefData,
        prParams->lpEventData
        );
    }

VOID __cdecl StackFunction6 (
    PVOID pvParam
    )
    {
    ((PFNSWAPPEDSTACK6)prParams->pfnSwappedStackFunction) (
        prParams->prConfig,
        prParams->pbParam1
        );
    }

VOID __cdecl StackFunction7 (
    PVOID pvParam
    )
    {
    ((PFNSWAPPEDSTACK7)prParams->pfnSwappedStackFunction) (
        prParams->pszString,
        prParams->pbParam1
        );
    }

//---------------------------------------------------------------------------
//
// BOOL CallOnNAVAPStack (
//      PFNSWAPPEDSTACK? pfnSwappedStackFunction,
//      ...
//      );
//
// These overloaded functions allocate new stack and call the function
// pointed to by pfnSwappedStackFunction on the new stack.
//
// Entry:
//      pfnSwappedStackFunction - pointer to a function to call on the newly
//                                allocate stack.
//
//      All other parameters depend on the type of the function passed to
//      pfnSwappedStackFunction (PFNSWAPPEDSTACK?).
//
// Exit:
//      TRUE if pfnSwappedStackFunction was successfully called, FALSE if
//      stack allocation failed.  If the function failed, the error is
//      communicated to the user.
//
//---------------------------------------------------------------------------
BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK1 pfnSwappedStackFunction
    )
    {
    auto PVOID pvStack;

    if ( pvStack = MemAllocPtr ( GMEM_FIXED, STACK_SIZE ) )
        {
        LinkStack ( pvStack );

        CallOnUserStack ( STACK_TOP(pvStack),
                          StackFunction1,
                          pfnSwappedStackFunction );

        UnlinkStack ( pvStack );

        MemFreePtr ( pvStack );

        return ( TRUE );
        }

    UIOutOfMemoryError();

    return ( FALSE );
}

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK2 pfnSwappedStackFunction,
    PBOOL            pbParam1
    )
    {
    auto PVOID pvStack;

    if ( pvStack = MemAllocPtr ( GMEM_FIXED, STACK_SIZE ) )
        {
        auto SWAPPEDSTACKPARAMS rParams;

        LinkStack ( pvStack );

        rParams.pfnSwappedStackFunction = pfnSwappedStackFunction;

        rParams.pbParam1 = pbParam1;

        CallOnUserStack ( STACK_TOP(pvStack),
                          StackFunction2,
                          &rParams );

        UnlinkStack ( pvStack );

        MemFreePtr ( pvStack );

        return ( TRUE );
        }

    UIOutOfMemoryError();

    return ( FALSE );
    }

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK3 pfnSwappedStackFunction,
    HEXCLUDEN        hExcludeN
    )
    {
    auto PVOID pvStack;

    if ( pvStack = MemAllocPtr ( GMEM_FIXED, STACK_SIZE ) )
        {
        auto SWAPPEDSTACKPARAMS rParams;

        LinkStack ( pvStack );

        rParams.pfnSwappedStackFunction = pfnSwappedStackFunction;

        rParams.hExcludeN = hExcludeN;

        CallOnUserStack ( STACK_TOP(pvStack),
                          StackFunction3,
                          &rParams );

        UnlinkStack ( pvStack );

        MemFreePtr ( pvStack );

        return ( TRUE );
        }

    UIOutOfMemoryError();

    return ( FALSE );
    }

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK4 pfnSwappedStackFunction,
    PEDEXTRA         prEDExtra,
    REFDATA          uRefData,
    LPED             lpEventData
    )
    {
    auto PVOID pvStack;

    if ( pvStack = MemAllocPtr ( GMEM_FIXED, STACK_SIZE ) )
        {
        auto SWAPPEDSTACKPARAMS rParams;

        LinkStack ( pvStack );

        rParams.pfnSwappedStackFunction = pfnSwappedStackFunction;

        rParams.prEDExtra   = prEDExtra;
        rParams.uRefData    = uRefData;
        rParams.lpEventData = lpEventData;

        CallOnUserStack ( STACK_TOP(pvStack),
                          StackFunction4,
                          &rParams );

        UnlinkStack ( pvStack );

        MemFreePtr ( pvStack );

        return ( TRUE );
        }

    UIOutOfMemoryError();

    return ( FALSE );
    }

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK5 pfnSwappedStackFunction,
    PBOOL            pbParam1,
    PEDEXTRA         prEDExtra,
    REFDATA          uRefData,
    LPED             lpEventData
    )
    {
    auto PVOID pvStack;

    if ( pvStack = MemAllocPtr ( GMEM_FIXED, STACK_SIZE ) )
        {
        auto SWAPPEDSTACKPARAMS rParams;

        LinkStack ( pvStack );

        rParams.pfnSwappedStackFunction = pfnSwappedStackFunction;

        rParams.pbParam1    = pbParam1;
        rParams.prEDExtra   = prEDExtra;
        rParams.uRefData    = uRefData;
        rParams.lpEventData = lpEventData;

        CallOnUserStack ( STACK_TOP(pvStack),
                          StackFunction5,
                          &rParams );

        UnlinkStack ( pvStack );

        MemFreePtr ( pvStack );

        return ( TRUE );
        }

    UIOutOfMemoryError();

    return ( FALSE );
    }

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK6 pfnSwappedStackFunction,
    PNAVAP_CONFIG    prConfig,
    PBOOL            pbParam1
    )
    {
    auto PVOID pvStack;

    if ( pvStack = MemAllocPtr ( GMEM_FIXED, STACK_SIZE ) )
        {
        auto SWAPPEDSTACKPARAMS rParams;

        LinkStack ( pvStack );

        rParams.pfnSwappedStackFunction = pfnSwappedStackFunction;

        rParams.prConfig = prConfig;
        rParams.pbParam1 = pbParam1;

        CallOnUserStack ( STACK_TOP(pvStack),
                          StackFunction6,
                          &rParams );

        UnlinkStack ( pvStack );

        MemFreePtr ( pvStack );

        return ( TRUE );
        }

    UIOutOfMemoryError();

    return ( FALSE );
    }

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK7 pfnSwappedStackFunction,
    PSTR             pszString,
    PBOOL            pbParam1
    )
    {
    auto PVOID pvStack;

    if ( pvStack = MemAllocPtr ( GMEM_FIXED, STACK_SIZE ) )
        {
        auto SWAPPEDSTACKPARAMS rParams;

        LinkStack ( pvStack );

        rParams.pfnSwappedStackFunction = pfnSwappedStackFunction;

        rParams.pszString = pszString;
        rParams.pbParam1  = pbParam1;

        CallOnUserStack ( STACK_TOP(pvStack),
                          StackFunction7,
                          &rParams );

        UnlinkStack ( pvStack );

        MemFreePtr ( pvStack );

        return ( TRUE );
        }

    UIOutOfMemoryError();

    return ( FALSE );
    }


#define DRIVER_LOCKED_CODE_SEGMENT
#include "drvseg.h"

#if defined(SYM_VXD)

VOID LinkStack (
    PVOID pvStack
    )
    {
    auto PSTACK_LINK prLink = (PSTACK_LINK) STACK_TOP(pvStack);

    prLink->pvThread = (PVOID) Get_Cur_Thread_Handle();
    prLink->pvStack = pvStack;
    prLink->prNext = prStackLink;

    prStackLink = prLink;
    }

VOID UnlinkStack (
    PVOID pvStack
    )
    {
    auto PSTACK_LINK prPrev, prCur;

    for ( prPrev = NULL, prCur = prStackLink;
          prCur && prCur->pvStack != pvStack;
          prPrev = prCur, prCur = prCur->prNext );

    if ( prCur )
        {
        if ( prPrev )
            prPrev->prNext = prCur->prNext;
        else
            prStackLink = prCur->prNext;
        }
    }

#endif

VOID StackCleanupForThread (
    PVOID pvThread
    )
    {
#if defined(SYM_VXD)

    auto PSTACK_LINK prPrev, prCur;

    do
        {
        for ( prPrev = NULL, prCur = prStackLink;
              prCur && prCur->pvThread != pvThread;
              prPrev = prCur, prCur = prCur->prNext );

        if ( prCur )
            {
                if ( prPrev )
                    prPrev->prNext = prCur->prNext;
                else
                    prStackLink = prCur->prNext;

                MemFreePtr ( prCur->pvStack );
            }

        } while ( prCur );

#endif
    }
