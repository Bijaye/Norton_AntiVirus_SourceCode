// Copyright 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/stack.cpv   1.9   18 Feb 1998 10:02:22   mdunn  $
//
// Description:
//      This file defines SYM_VXD and SYM_NTK functions for stack swapping.
//
// Contains:
//      CallOnUserStack()               (i386, Alpha)
//      CallOnKernelStack()             (i386, Alpha)
//      CallOnNewStack()                (local function)
//      GetStackSwapThreadArea()        (local function)
//
//      CoreSwapStackIn()               (i386 only)
//      CoreSwapStackOut()              (i386 only)
//      CoreSwapStackOutTemp()          (i386 only)
//      CoreSwapStackInTemp()           (i386 only)
//      CoreAllocateStack()             (local function)
//      CoreFreeStack()                 (local function)
//
//      CoreGetSwappedStack()           (local function)
//      CoreSetSwappedStack()           (local function)
//
//      IsSPInKernelStack()             (SYM_NTK: i386 only)
//      GetRemainingStackSize()         (SYM_NTK: i386 only)
//
// See also:
//      thdata.cpp, drvmisc.cpp
//
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/stack.cpv  $
//
//    Rev 1.9   18 Feb 1998 10:02:22   mdunn
// Ported ROSWELL and ROSNEC changes from the K branch.
//
//    Rev 1.8   10 Jul 1997 12:43:30   RStanev
// Fixed comments for CallOnUserStack().
//
//    Rev 1.7   08 Jul 1997 14:40:14   ksackin
// Fixed the definition of SetStack().
//
//    Rev 1.6   08 Jul 1997 12:48:02   RStanev
// Added Alpha support to CallOnNewStack().
//
//    Rev 1.5   06 Jul 1997 23:52:18   RStanev
// Changed CallOnUserStack() and CallOnKernelStack() to use a common stack
// swapping function.
//
//    Rev 1.4   06 Jul 1997 03:53:56   RStanev
// Added new stack swapping functions.
//
//    Rev 1.3   20 May 1997 13:34:22   RStanev
// Non-Intel compilations don't seem to like __declspec(naked)...
//
//    Rev 1.2   20 May 1997 12:29:52   RStanev
// The i386 assembly code in this file now compiles for i386 platforms
// only.  All other platforms will SYM_ASSERT ( FALSE ).
//
//    Rev 1.1   16 May 1997 21:33:02   RStanev
// Added "int 1" before both "int 3" instructions in the code.  This way
// SoftIce/Win will break with either I1HERE ON or I3HERE ON.
//
//    Rev 1.0   25 Feb 1997 00:52:14   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"

//---------------------------------------------------------------------------
//
// Local function prototypes.
//
//---------------------------------------------------------------------------
VOID CallOnNewStack (
    PVOID         pvTopOfStack,
    PSWAPPEDSTACK pfnSwappedStack,
    PVOID         pvParam,
    PVOID         *ppvOldStackPointer
    );

PVOID* GetStackSwapThreadArea (
    VOID
    );

//---------------------------------------------------------------------------
//
// Stack swapping control variables.
//
//---------------------------------------------------------------------------
BOOL bRestoreKernelStack = TRUE;

//---------------------------------------------------------------------------
//
// VOID CallOnUserStack (
//      PVOID         pvTopOfStack,
//      PSWAPPEDSTACK pfnSwappedStack,
//      PVOID         pvParam
//      );
//
// This function switches the current stack pointer to the memory block
// pointed to by pvTopOfStack, and then calls the pfnSwappedStack function
// using the new stack.  CallOnUserStack() works together with the
// per-thread data engine to ensure that the original kernel mode stack is
// saved, and used later by calls to CallOnKernelStack().
//
// Entry:
//      pvTopOfStack    - pointer to the byte following the last valid
//                        byte of the new stack.  This value should be
//                        aligned according to platform specifications
//                        (DWORD for Intel, PARAGRAPH for Alpha).
//      pfnSwappedStack - pointer to a function to be called on the new
//                        stack.
//      pvParam         - value to be passed as parameter to pfnSwappedStack.
//
// Synchronization:
//      This function is non-reentrant from the current thread.  The client
//      must ensure that during events (such as paging), which could
//      interrupt its execution, the CallOn...Stack() functions don't get
//      reentered.
//      CallOnUserStack() is fully reentrant from different threads.
//      Local Core instance must be initialized before calling this API.

//---------------------------------------------------------------------------
VOID CallOnUserStack (
    PVOID         pvTopOfStack,
    PSWAPPEDSTACK pfnSwappedStack,
    PVOID         pvParam
    )
    {
    auto PVOID pvSwappedOutTemp, pvPreviousStack, *ppvThreadDataStack;

    if ( ppvThreadDataStack = GetStackSwapThreadArea() )
        {
        pvPreviousStack = *ppvThreadDataStack;

        CallOnNewStack ( pvTopOfStack,
                         pfnSwappedStack,
                         pvParam,
                         pvPreviousStack ? &pvSwappedOutTemp :
                                           ppvThreadDataStack );

        *ppvThreadDataStack = pvPreviousStack;
        }
    else
        {
        SYM_ASSERT ( FALSE );
        }
    }

//---------------------------------------------------------------------------
//
// VOID CallOnKernelStack (
//      PSWAPPEDSTACK pfnSwappedStack,
//      PVOID         pvParam
//      );
//
// This function switches the current stack pointer to the kernel mode stack
// swapped out by CallOnUserStack(), and then calls the pfnSwappedStack
// function using that stack.  If the stack was not swapped by
// CallOnUserStack(), then pfnSwappedStack will be called without changes to
// the stack.
//
// Entry:
//      pfnSwappedStack - pointer to a function to be called on the kernel
//                        stack.
//      pvParam         - value to be passed as parameter to pfnSwappedStack.
//
// Synchronization:
//      This function is non-reentrant from the current thread.  The client
//      must ensure that during events (such as paging), which could
//      interrupt its execution, the CallOn...Stack() functions don't get
//      reentered.
//      CallOnKernelStack() is fully reentrant from different threads.
//      Alpha: This is only a stub!
//      Local Core instance must be initialized before calling this API.
//
//---------------------------------------------------------------------------
VOID CallOnKernelStack (
    PSWAPPEDSTACK pfnSwappedStack,
    PVOID         pvParam
    )
    {
    auto PVOID pvSwappedOutTemp, pvPreviousStack, *ppvThreadDataStack;

    if ( bRestoreKernelStack )
        {
        if ( ppvThreadDataStack = GetStackSwapThreadArea() )
            {
            if ( pvPreviousStack = *ppvThreadDataStack )
                {
                *ppvThreadDataStack = NULL;

                CallOnNewStack ( pvPreviousStack,
                                pfnSwappedStack,
                                pvParam,
                                &pvSwappedOutTemp );

                *ppvThreadDataStack = pvPreviousStack;
                }
            else
                {
                pfnSwappedStack ( pvParam );
                }
            }
        else
            {
            SYM_ASSERT ( FALSE );
            }
        }
    else
        {
        pfnSwappedStack ( pvParam );
        }
    }

//---------------------------------------------------------------------------
//
// VOID CallOnNewStack (
//      PVOID         pvTopOfStack,
//      PSWAPPEDSTACK pfnSwappedStack,
//      PVOID         pvParam,
//      PVOID         *ppvOldStackPointer
//      );
//
// This function switches the current stack pointer to the memory block
// pointed to by pvTopOfStack, and then calls the pfnSwappedStack function
// using the new stack.  Before calling pfnSwappedStack, this function will
// save the value of the swapped out stack to *ppvOldStackPointer.  This
// value will be safe to be used as pvTopOfStack parameter in recursive
// calls to CallOnNewStack().
//
// Entry:
//      pvTopOfStack       - pointer to the byte following the last valid
//                           byte of the new stack.  This value should be
//                           aligned according to platform specifications
//                           (my guess is DWORD for both SYM_NTK and
//                           SYM_VXD).
//      pfnSwappedStack    - pointer to a function to be called on the new
//                           stack.
//      pvParam            - value to be passed as parameter to
//                           pfnSwappedStack.
//      ppvOldStackPointer - pointer to a PVOID for storing the value of the
//                           swapped out stack.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(_X86_)

#pragma optimize("y",off)

VOID CallOnNewStack (
    PVOID         pvTopOfStack,
    PSWAPPEDSTACK pfnSwappedStack,
    PVOID         pvParam,
    PVOID         *ppvOldStackPointer
    )
    {
    __asm
        {
        mov     edx,esp
        mov     eax,ppvOldStackPointer
        mov     [eax],edx
        mov     eax,pvTopOfStack
        mov     esp,eax
        push    edx
        push    pvParam
        call    [pfnSwappedStack]
        add     esp,4
        pop     eax
        mov     esp,eax
        }
    }

#pragma optimize("y",on)

#elif defined(_ALPHA_)

#ifdef __cplusplus
extern "C" { __asm(char *, ...);};
#pragma intrinsic (__asm)
#endif

#define SetStack(a,b,c,d) __asm  (     \
               "beq a3, skip;"         \
               "mov sp, r0;"           \
               "lda r0, -0x10 (r0);"   \
               "stl r0, 0 (a3);"       \
               "skip:;"                \
               "mov sp, r0;"           \
               "mov %0, sp;"           \
               "lda sp, -0x30 (sp);"   \
               "stq r0, 0x00 (sp);"    \
               "stq a0, 0x10 (sp);"    \
               "stq a1, 0x18 (sp);"    \
               "stq a2, 0x20 (sp);"    \
               "stq a3, 0x28 (sp);"    \
               , a, b, c, d);

#define RestoreStack() __asm (  \
        "ldq r0, 0x00 (sp);"    \
        "mov r0, sp;"           \
        );

VOID CallOnNewStack (
    PVOID         pvTopOfStack,
    PSWAPPEDSTACK pfnSwappedStack,
    PVOID         pvParam,
    PVOID         *ppvOldStackPointer
    )
    {
    //
    //  The following macro inserts ALPHA AXP assembler instructions which:
    //  1) Saves the parameters in registers
    //  2) Saves the current stack pointer in a register
    //  3) Switches the stack pointer to the passed in value (new stack)
    //  4) Copies the parameters into the same relative locations off the new stack.
    //  5) Saves the old stack pointer on the new stack and in *ppvOldStackPointer (if not NULL)
    //
    SetStack(pvTopOfStack, pfnSwappedStack, pvParam, ppvOldStackPointer);

    //
    //  We are now on the new stack and are about to call the user specified service
    //
    (pfnSwappedStack)(pvParam);

    //
    //  The following macro inserts ALPHA AXP assembler instructions which:
    //  1) Take the old stack pointer from a location off the new stack
    //  2) Restores the old stack pointer
    //
    RestoreStack();
    }

#endif

//---------------------------------------------------------------------------
//
// PVOID* GetStackSwapThreadArea (
//      VOID
//      );
//
// This function returns a pointer to per-thread data area containing a
// PVOID variable used by the stack swapping functions.
//
//---------------------------------------------------------------------------
PVOID* GetStackSwapThreadArea (
    VOID
    )
    {
    auto PCOREPERTHREADDATA prPerThreadData;
    auto PVOID              *result;

                                        // Lock the per-thread data for this
                                        // thread (just in case).  This will
                                        // complete really fast if the data
                                        // was already locked, as it
                                        // should've been.

    SYM_ASSERT ( hCorePerThreadData );

    prPerThreadData = (PCOREPERTHREADDATA) _CoreLockThreadDataPtr (
                                           hCorePerThreadData );

    SYM_ASSERT ( prPerThreadData );

                                        // If data was successfully locked,
                                        // then return a pointer to the
                                        // variable used for stack swapping.

    if ( prPerThreadData )
        {
        result = &prPerThreadData->pvSwappedStack;

        _CoreUnLockThreadDataPtr ( hCorePerThreadData, prPerThreadData );
        }
    else
        {
        result = NULL;
        }

    return ( result );
    }



//---------------------------------------------------------------------------
//
// Define local function prototypes.
//
//---------------------------------------------------------------------------
static void* __stdcall CoreAllocateStack ( long stacksize );
static void __stdcall CoreFreeStack ( void *stackptr );

static void* __stdcall CoreGetSwappedStack ( void );
static void __stdcall CoreSetSwappedStack ( void* stackptr );

//---------------------------------------------------------------------------
//
// __declspec(naked) void* __stdcall CoreSwapStackIn ( long stacksize );
//
// Allocates a new stack and swaps to it.  stacksize should contain the non-
// zero size of the new stack (in bytes) and it should be DWORD-aligned.  The
// function returns a non-null value if successful, NULL if error.  If the
// kernel stack is already swapped for the current thread, this function
// will succeed without allocating new stack.
//
// If this function returns a non-null value, the caller's stack frame will
// not be accessible through esp anymore.  This means that the caller can
// access its local variables only through ebp.  For every successful call to
// CoreSwapStackIn(), a call to CoreSwapStackOut() has to be made.
//
// Upon successful completion, with new stack allocated, the stack pointer
// will be set to point just a DWORD below the top of the new stack.  The top
// DWORD of the stack is reserved, and is set to caller's original stack
// pointer. Caller's stack pointer, as pushed on top of the new stack, points
// to a DWORD containing the starting address of the newly allocated stack.
// That DWORD has the same address as stacksize, when CoreAllocateStack() was
// called.  The pvSwappedStack member of Core's per-thread data will be set
// to the same value that was pushed on top of the newly allocated stack.
//
// Upon successful completion, with stack not allocated, the function will
// return without cleaning up the stack frame allocated for stacksize.
// stacksize itself will be replaced by 0.  That 0 will indicate to
// CoreSwapStackOut() later, that no stack swap has been done, and that
// only the stack frame should be cleaned up.
//
// Notes:
//      Leave the "int 1/3" instructions in the code (including released
//      versions).  In fact, we should even bugcheck the machine if invalid
//      stacksize is given to us.
//
//      STACK FRAME OPTIMIZATION SHOULD BE TURNED OFF FOR FUNCTIONS WHICH
//      CALL CoreSwapStack...().  This can be done with
//      #pragma optimize ("y",off)
//
//      The CoreSwapStackIn()/CoreSwapStackOut() pair of functions can be
//      called only between calls to CoreLocalInit() and CoreLocalDeInit(),
//      and it can have only a perfect match of calls to the
//      CoreSwapStackIn...()/CoreSwapStackOut...() pairs in between.
//
// Synchronization:
//      This function is non-reentrant from the current thread.  The client
//      must ensure that during events (such as paging), which could
//      interrupt its execution, the CoreSwapStack...() functions don't get
//      reentered.
//      CoreSwapStackIn() is fully reentrant from different threads.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(_X86_)

__declspec(naked) void* __stdcall CoreSwapStackIn ( long stacksize )
    {
    __asm
        {
        call    CoreGetSwappedStack
        test    eax,eax                 // if ( !CoreGetSwappedStack() )
        jnz     coreswapstackskip       //     goto coreswapstackskip;
        mov     eax,[esp + 4]           // eax = stacksize;
        test    al,3                    // if ( stacksize & 3 )
        jnz     coreswapstackendi3      //     goto coreswapstackendi3;
        test    eax,eax                 // if (!stacksize)
        jz      coreswapstackendi3      //     goto coreswapstackendi3;
        add     eax,4
        push    eax
        call    CoreAllocateStack       // eax = CoreAllocateStack ( stacksize + sizeof(DWORD) );
        test    eax,eax                 // if (!eax)
        jz      coreswapstackend        //     goto coreswapstackend;
        pop     edx                     // edx = client_return_address;
        pop     ecx                     // ecx = stacksize;
        push    eax                     // *top_of_old_stack = start_of_new_stack;
        add     eax,ecx
        mov     [eax],esp               // *top_of_new_stack = top_of_old_stack;
        mov     ecx,esp                 // ecx = top_of_old_stack;
        mov     esp,eax                 // esp = top_of_new_stack;
        push    edx                     // push client_return_address;
        push    ecx
        call    CoreSetSwappedStack     // CoreSetSwappedStack ( top_of_old_stack );
        mov     eax,-1
        ret                             // return (~NULL);
    coreswapstackskip:
        xor     eax,eax
        mov     [esp + 4],eax           // stacksize = NULL;
        dec     eax
        ret                             // return (~NULL);
    coreswapstackendi3:
        int     1                       // instead of bugchecking...
        int     3                       // instead of bugchecking...
        xor     eax,eax                 // return (NULL);
    coreswapstackend:
        ret     4
        }
    }

#endif // #if defined(SYM_VXD) || defined(_X86_)

//---------------------------------------------------------------------------
//
// __declspec(naked) void __stdcall CoreSwapStackOut ( void );
//
// Swaps back to the stack previously swapped out by CoreSwapStackIn().  At
// the time the client calls this function, its esp should be exactly the
// same as when CoreSwapStackIn() returned successfully.  No error checking
// is provided, so the clients must be very careful.
//
// Notes:
//      STACK FRAME OPTIMIZATION SHOULD BE TURNED OFF FOR FUNCTIONS WHICH
//      CALL CoreSwapStack...().  This can be done with
//      #pragma optimize ("y",off)
//
//      The CoreSwapStackIn()/CoreSwapStackOut() pair of functions can be
//      called only between calls to CoreLocalInit() and CoreLocalDeInit(),
//      and it can have only a perfect match of calls to the
//      CoreSwapStackIn...()/CoreSwapStackOut...() pairs in between.
//
// Synchronization:
//      This function is non-reentrant from the current thread.  The client
//      must ensure that during events (such as paging), which could
//      interrupt its execution, the CoreSwapStack...() functions don't get
//      reentered.
//      CoreSwapStackOut() is fully reentrant from different threads.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(_X86_)

__declspec(naked) void __stdcall CoreSwapStackOut ( void )
    {
    __asm
        {
        pop     edx                     // edx = client_return_address;
        pop     eax                     // eax = *top_of_current_stack;
        test    eax,eax                 // if ( *top_of_current_stack == NULL )
        jz      coreswapstackoutnone    //     goto coreswapstackoutnone;
        mov     esp,eax                 // current_stack = eax;
        xchg    [esp],edx               // push client_return_address, edx = allocated_stack;
        push    edx
        call    CoreFreeStack           // CoreFreeStack ( allocated_stack );
        push    dword ptr 0
        call    CoreSetSwappedStack     // CoreSetSwappedStack ( NULL );
        ret                             // return;
    coreswapstackoutnone:
        jmp     edx                     // return;
        }
    }

#endif // #if defined(SYM_VXD) || defined(_X86_)

//---------------------------------------------------------------------------
//
// __declspec(naked) void* __stdcall CoreSwapStackOutTemp ( void );
//
// This function checks if the stack of the current thread is swapped,
// and if so, it restores temporarily back the original kernel stack.  Upon
// completion of this function, the client must save the return value, and
// then pass it to CoreSwapStackInTemp() when the kernel mode stack should
// be swapped in again, and when the allocated stack is needed again.
//
// Upon return from this function, the caller's stack frame will not be
// accessible through esp anymore.  This means that the caller can
// access its local variables only through ebp.
//
// Notes:
//      STACK FRAME OPTIMIZATION SHOULD BE TURNED OFF FOR FUNCTIONS WHICH
//      CALL CoreSwapStack...().  This can be done with
//      #pragma optimize ("y",off)
//
//      The CoreSwapStackInTemp()/CoreSwapStackOutTemp() pair of functions
//      can be called only between calls to CoreLocalInit() and
//      CoreLocalDeInit(), and it can have only a perfect match of calls to
//      the CoreSwapStackIn...()/CoreSwapStackOut...() pairs in between.
//
// Synchronization:
//      This function is non-reentrant from the current thread.  The client
//      must ensure that during events (such as paging), which could
//      interrupt its execution, the CoreSwapStack...() functions don't get
//      reentered.
//      CoreSwapStackOutTemp() is fully reentrant from different threads.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(_X86_)

__declspec(naked) void* __stdcall CoreSwapStackOutTemp ( void )
    {
    __asm
        {
        cmp     bRestoreKernelStack,0
        je      coreswapstackouttempend
        call    CoreGetSwappedStack     // eax = CoreGetSwappedStack();
        test    eax,eax                 // if ( !eax )
        jz      coreswapstackouttempend //     goto coreswapstackouttempend;
        pop     edx                     // edx = client_return_address;
        xchg    esp,eax                 // esp = old_stack_pointer, eax = stack_ptr_to_return;
        push    edx                     // simulate call in the new stack frame
        push    eax                     // save stack_ptr_to_return
        push    dword ptr 0
        call    CoreSetSwappedStack     // CoreSetSwappedStack ( NULL );
        pop     eax                     // restore stack_ptr_to_return
    coreswapstackouttempend:
        ret                             // return ( eax );
        }
    }

#endif // #if defined(SYM_VXD) || defined(_X86_)

//---------------------------------------------------------------------------
//
// __declspec(naked) void __stdcall CoreSwapStackInTemp ( void* savedstack );
//
// This function restores back the swapped stack after CoreSwapStackOutTemp()
// has been called.  savedstack should be the same value which
// was returned by CoreSwapStackOutTemp().  CoreSwapStackInTemp() should be
// called with esp at the same level when CoreSwapStackOutTemp() was called.
//
// Notes:
//      Leave the "int 1/3" instructions in the code (including released
//      versions).  In fact, we should even bugcheck the machine if this
//      function is called while we are using stack allocated by
//      CoreSwapStackIn().
//
//      STACK FRAME OPTIMIZATION SHOULD BE TURNED OFF FOR FUNCTIONS WHICH
//      CALL CoreSwapStack...().  This can be done with
//      #pragma optimize ("y",off)
//
//      The CoreSwapStackInTemp()/CoreSwapStackOutTemp() pair of functions
//      can be called only between calls to CoreLocalInit() and
//      CoreLocalDeInit(), and it can have only a perfect match of calls to
//      the CoreSwapStackIn...()/CoreSwapStackOut...() pairs in between.
//
// Synchronization:
//      This function is non-reentrant from the current thread.  The client
//      must ensure that during events (such as paging), which could
//      interrupt its execution, the CoreSwapStack...() functions don't get
//      reentered.
//      CoreSwapStackInTemp () is fully reentrant from different threads.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(_X86_)

__declspec(naked) void __stdcall CoreSwapStackInTemp ( void* savedstack )
    {
    __asm
        {
        cmp     bRestoreKernelStack,0
        je      coreswapstackintempendskip
        call    CoreGetSwappedStack     // eax = CoreGetSwappedStack();
        test    eax,eax                 // if ( eax )
        jnz     coreswapstackintempbad  //     goto coreswapstackintempbad;
        pop     edx                     // edx = client_return_address;
        pop     eax                     // eax = savedstack;
        test    eax,eax                 // if ( !eax )
        jz      coreswapstackintempend  //     goto coreswapstackintempend;
        xchg    esp,eax                 // esp = savedstack, eax = allocated_stack;
        push    edx                     // simulate call
        push    eax
        call    CoreSetSwappedStack     // CoreSetSwappedStack ( savedstack );
        ret                             // return;
    coreswapstackintempend:
        jmp     edx                     // return;
    coreswapstackintempbad:
        int     1                       // instead of bugchecking...
        int     3                       // instead of bugchecking...
    coreswapstackintempendskip:
        ret     4                       // return;
        }
    }

#endif // #if defined(SYM_VXD) || defined(_X86_)

//---------------------------------------------------------------------------
//
// static void* __stdcall CoreAllocateStack ( long stacksize );
//
// Allocates kernel mode stack.  This is an internal worker routine for
// CoreSwapStackIn().  stacksize should contain the number of bytes
// to allocate for stack.  The function returns a non-null pointer to a
// newly allocated stack, or NULL if error.
//
// MemAllocPtrX, under SYM_NTK, allocates memory bypassing any stack switching
// logic.  Hence, this call should be made under the context of a kernel stack.
//---------------------------------------------------------------------------
static void* __stdcall CoreAllocateStack ( long stacksize )
    {
    return ( MemAllocPtrX ( GMEM_FIXED, stacksize ) );
    }

//---------------------------------------------------------------------------
//
// static void __stdcall CoreFreeStack ( void *stackptr );
//
// Frees kernel mode stack.  This is an internal worker routine for
// CoreSwapStackOut().  stackptr should point to a non-null value returned
// by CoreAllocateStack().
//
// MemFreePtrX, under SYM_NTK, releases memory bypassing any stack switching
// logic.  Hence, this call should be made under the context of a kernel stack.
//---------------------------------------------------------------------------
static void __stdcall CoreFreeStack ( void *stackptr )
    {
    MemFreePtrX ( stackptr );
    }

//---------------------------------------------------------------------------
//
// static void* __stdcall CoreGetSwappedStack ( void );
//
// This function returns the swapped stack value for the current thread.
// If the stack is not swapped, then the returned value is NULL.  If the
// stack is swapped, then the return value is the swapped stack pointer.
//
//---------------------------------------------------------------------------
static void* __stdcall CoreGetSwappedStack ( void )
    {
    auto PCOREPERTHREADDATA prPerThreadData;

                                        // Lock the per-thread data for this
                                        // thread (just in case).  This will
                                        // complete really fast if the data
                                        // was already locked, as it
                                        // should've been.

    SYM_ASSERT ( hCorePerThreadData );

    prPerThreadData = (PCOREPERTHREADDATA) _CoreLockThreadDataPtr (
                                           hCorePerThreadData );

    SYM_ASSERT ( prPerThreadData );

                                        // If data was successfully locked,
                                        // then return the status of the
                                        // swapped stack.  Don't forget to
                                        // unlock the per-thread data.

    if ( prPerThreadData )
        {
        auto void* result;

        result = prPerThreadData->pvSwappedStack;

        _CoreUnLockThreadDataPtr ( hCorePerThreadData, prPerThreadData );

        return ( result );
        }

    return ( NULL );
    }

//---------------------------------------------------------------------------
//
// static void __stdcall CoreSetSwappedStack ( void *stackptr );
//
// This function sets the swapped stack value for the current thread.
// Whenever the stack is getting swapped in, then stackptr should be set to
// the original stack pointer.  If the stack is getting swapped out, then
// stackptr should be set to NULL.
//
//---------------------------------------------------------------------------
static void __stdcall CoreSetSwappedStack ( void *stackptr )
    {
    auto PCOREPERTHREADDATA prPerThreadData;

                                        // Lock the per-thread data for this
                                        // thread (just in case).  This will
                                        // complete really fast if the data
                                        // was already locked, as it
                                        // should've been.

    SYM_ASSERT ( hCorePerThreadData );

    prPerThreadData = (PCOREPERTHREADDATA) _CoreLockThreadDataPtr (
                                           hCorePerThreadData );

    SYM_ASSERT ( prPerThreadData );

                                        // If data was successfully locked,
                                        // then save the new swapped stack
                                        // status.  Don't forget to unlock
                                        // the per-thread data.

    if ( prPerThreadData )
        {
        prPerThreadData->pvSwappedStack = stackptr;

        _CoreUnLockThreadDataPtr ( hCorePerThreadData, prPerThreadData );
        }
    }

#if defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// BOOLEAN IsSPInKernelStack ( void )  (NTK and i386 only)
//
// This function returns TRUE if the stack pointer is within the kernel stack
//---------------------------------------------------------------------------
#define TebStackBase   (0x04)            // will not change: field in _NT_TIB
#define TebStackLimit  (0x08)            // will not change: field in _NT_TIB

BOOLEAN IsSPInKernelStack(void)
{
    BOOLEAN bRet = TRUE;
#ifdef _X86_
    _asm
    {
        push    edx
        xor     eax, eax        // assume failure
        mov     edx, esp
        cmp     edx, fs:[TebStackLimit]
        jbe     ISPWKSdone
        cmp     edx, fs:[TebStackBase]
        ja      ISPWKSdone
        inc     eax             // indicate it is within kernel stack
    ISPWKSdone:
        pop     edx
        mov     [bRet], al
    }
#endif
    return bRet;
}

//---------------------------------------------------------------------------
//
// ULONG GetRemainingStackSize ( void )  (NTK and i386 only)
//
// This function returns the number of bytes left on a kernel stack.
//---------------------------------------------------------------------------

ULONG GetRemainingStackSize(void)
{
    ULONG   uRet = 0;
#ifdef _X86_
    _asm
    {
        mov     eax, esp
        sub     eax, fs:[TebStackLimit]
        mov     [uRet], eax
    }
#endif
    return uRet;
}

#endif
