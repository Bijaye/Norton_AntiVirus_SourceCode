; Copyright 1998 Symantec, Peter Norton Product Group
;****************************************************************************
;
; $Header:   S:/APEVENT/VCS/ASYNCECB.ASv   1.0   08 Jul 1998 15:59:44   RStanev  $
;
; Description:
;       This file contains functions to assist EVENTCBS.CPP with setting up
;       asynchronous processing of events.
;
; Contains:
;       TimeoutCBK()
;       RestrictedEventCBK()
;       ScheduleAsyncEventNotification()
;       CancelAsyncEventNotification()
;
;****************************************************************************
; $Log:   S:/APEVENT/VCS/ASYNCECB.ASv  $
;  
;     Rev 1.0   08 Jul 1998 15:59:44   RStanev
;  Initial revision.
;
;****************************************************************************

        .XLIST
        INCLUDE platform.inc
        .LIST

;----------------------------------------------------------------------------
;
; Define the ASYNCEVENTDATA offsets needed by this module.
;
;----------------------------------------------------------------------------
aed_dwCancelTimeout     EQU     04h
aed_dwEventHandle       EQU     08h
aed_pfnCancel           EQU     0Ch
aed_pfnCallback         EQU     10h
aed_uRefData            EQU     14h
aed_rED                 EQU     18h

;----------------------------------------------------------------------------
;
; It is important to define all code and data for working with asynchronous
; event notifications such that we don't get any page faults and thus get
; interrupted while traversing linked lists...
;
;----------------------------------------------------------------------------
VxD_LOCKED_CODE_SEG

;----------------------------------------------------------------------------
;
; TimeoutCBK() - VMM callback
;
; This function is called by VMM when the timeout specified by
; RestrictedEventCBK has expired.  It calls the cancel routine, which should
; remove the entry which caused RestrictedEventCBK to schedule this timeout
; from the list of asynchronous event notifications, and move it to a list
; to be freed up when memory operations are safe.
;
; Entry:
;       Registers set up as defined by VMM.  The user data (edx) contains a
;       pointer to ASYNCEVENTDATA structure, as defined in EVENTCBS.CPP;
;       the offsets of this structure are hard-coded in this ASM file, so
;       be careful.
;
; Exit:
;       -
;
;----------------------------------------------------------------------------
        OPTION LANGUAGE:SYSCALL
BeginProc  TimeoutCBK
        pusha
        push    edx
        call    DWORD PTR [edx + aed_pfnCancel]
        add     esp,4
        popa
        ret
EndProc  TimeoutCBK

;----------------------------------------------------------------------------
;
; RestrictedEventCBK() - VMM callback
;
; This function is called by VMM when the restrictions set up by
; ScheduleAsyncEventNotification() have been met.  It notifies the SYMEvent
; callback, as speficied by its ASYNCEVENTDATA packet, and then schedules
; a global timeout for calling the routine to cancel the event notification.
; The ASYNCEVENTDATA packet must be kept in the list of event notifications
; until the global timeout calls the cancel callback.
;
; Entry:
;       Registers set up as defined by VMM.  The user data (edx) contains a
;       pointer to ASYNCEVENTDATA structure, as defined in EVENTCBS.CPP;
;       the offsets of this structure are hard-coded in this ASM file, so
;       be careful.
;
; Exit:
;       -
;
;----------------------------------------------------------------------------
        OPTION LANGUAGE:SYSCALL
BeginProc  RestrictedEventCBK

        ; Immediately clear the event handle so that we don't attempt to
        ; cancel it.  From now on this event driven code only is going to be
        ; able to free up the entry.

        xor     eax,eax
        xchg    [edx + aed_dwEventHandle],eax

        ; If we did not have a handle to begin with, then we should not be
        ; processing anything.  Someone else is working on canceling the
        ; entry.

        test    eax,eax
        jz      NoProcessingForThisCBK

        ; Save all registers, eventhough the docs say that it is redundant
        ; for the most part.

        pushad

        ; Save client's state because we can't guarantee that noone is going
        ; to muck around with it.

        Push_Client_State USES_ESI

        ; Save the user defined data (the pointer to our ASYNCEVENTDATA) so
        ; that we can use it later for scheduling global timeout for freeing
        ; up the entry.

        push    edx

        ; Push the parameters to a standard SYMEvent callback.  Last first,
        ; first last: __cdecl secallback ( refdata, lped );

        lea     eax,[edx + aed_rED]
        push    eax
        push    [edx + aed_uRefData]

        ; Call the SYMEvent callback.

        call    DWORD PTR [edx + aed_pfnCallback]
        add     esp,8

        ; Schedule a global timeout for canceling the entry.  If no timeout
        ; was specified, or if the timeout could not be scheduled, then call
        ; the cancel routine right away.

        pop     edx
        mov     eax,[edx + aed_dwCancelTimeout]
        test    eax,eax
        jz      CancelItNow

        lea     esi,[TimeoutCBK]
        VMMCall Set_Global_Time_Out
        test    esi,esi
        jnz     SkipCancelItNow

CancelItNow:

        push    edx
        call    DWORD PTR [edx + aed_pfnCancel]
        add     esp,4

SkipCancelItNow:

        ; Restore client's state.

        Pop_Client_State USES_ESI

        ; Restore all registers.

        popad
        cld

        ; Return to VMM.

NoProcessingForThisCBK:
        ret

EndProc  RestrictedEventCBK

;----------------------------------------------------------------------------
;
; DWORD __cdecl ScheduleAsyncEventNotification (
;     PASYNCEVENTDATA prNew );
;
; This function schedules an asynchronous event notification.
;
; Entry:
;       prNew - non-NULL pointer to an ASYNCEVENTDATA structure, whos
;               members are initialized to contain the event information.
;               The structure should not be linked into the list of
;               scheduled event notifications at the time this function
;               is called.
;
; Exit:
;       Returns 0 if the event notification failed to be scheduled (possibly
;       out of memory), or non-0 if the event notification was successfully
;       scheduled.  In the latter case, the client must link prNew into the
;       list of scheduled event notifications.
;
;----------------------------------------------------------------------------
        OPTION LANGUAGE:C
        PUBLIC ScheduleAsyncEventNotification
ScheduleAsyncEventNotification  PROC, prNew:DWORD
        push    ebx
        push    esi
        push    edi

        ; Schedule an asynchronous event, and the success of it will be
        ; determined by the status of the handle returned in prNew.

        xor     eax,eax
        xor     ebx,ebx
        mov     ecx,PEF_ALWAYS_SCHED OR PEF_WAIT_FOR_STI OR PEF_WAIT_NOT_HW_INT OR PEF_WAIT_NOT_CRIT
        mov     edx,prNew
        lea     esi,[RestrictedEventCBK]
        xor     edi,edi
        VMMCall Call_Restricted_Event
        mov     ebx,prNew
        mov     [ebx + aed_dwEventHandle],esi

        mov     eax,esi

        pop     edi
        pop     esi
        pop     ebx
        ret
ScheduleAsyncEventNotification  ENDP

;----------------------------------------------------------------------------
;
; DWORD __cdecl CancelAsyncEventNotification (
;     PASYNCEVENTDATA prCancel );
;
; This function cancels an asynchronous event notification, which had
; previously been scheduled by ScheduleAsyncEventNotification(), and it is
; currently in the list of scheduled event notifications.
;
; Entry:
;       prCancel - non-NULL pointer to an ASYNCEVENTDATA structure which is
;                  to be canceled.
;
; Exit:
;       0 if the cancel failed (the event notification is already being
;       serviced by someone else). Non-0 if the event notification has been
;       canceled, in which case the client must remove it from the list of
;       event notifications and free any memory associated with it.
;
;----------------------------------------------------------------------------
        PUBLIC CancelAsyncEventNotification
CancelAsyncEventNotification  PROC, prCancel:DWORD
        push    ebx
        push    esi
        push    edi

        ; Cancel the event if it is not being processed by anyone.

        mov     edi,prCancel
        xor     esi,esi
        xchg    [edi + aed_dwEventHandle],esi
        push    esi
        VMMCall Cancel_Restricted_Event

        ; Return 0 if event is being processed by someone else, non-0 if
        ; we had cancelled it.

        pop     eax

        pop     edi
        pop     esi
        pop     ebx
        ret
CancelAsyncEventNotification  ENDP

VxD_LOCKED_CODE_ENDS

        END
