; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\navex\vcs\chk.asv   1.0   26 Oct 1995 13:26:16   MLELE  $
;
; Filename:	
; Programmer:	
; Assembler:	MASM 6.0+
; Project:	NAV 3.0
;
; Contents:	
;
; Note:		
;		
; Copyright (C) 1992-1993 Symantec Corporation
;
; $Log:   S:\navex\vcs\chk.asv  $
;  
;     Rev 1.0   26 Oct 1995 13:26:16   MLELE
;  Initial revision.
;  
;     Rev 1.15   11 Aug 1995 06:49:44   JMILLARD
;  STS 41492 - make stack access DEBUG_MEMORY_STRICT so I don't have
;  problems with protected domain on 4.x
;  
;     Rev 1.14   07 Jul 1995 16:25:38   JMILLARD
;  don't nuke stack for production build
;  
;     Rev 1.13   25 May 1995 16:28:52   GDZIECI
;  __CHK needed some data.
;  
;     Rev 1.12   25 May 1995 16:24:28   GDZIECI
;  __CHK function code generated always.
;  
;     Rev 1.11   17 May 1995 16:34:28   GDZIECI
;  Ifdef'd out GetFreeStack().
;  
;     Rev 1.10   15 Dec 1994 19:06:14   keith
;  Use CTSN.INC for CTS4.INC
;  
;     Rev 1.9   24 Nov 1993 09:01:16   DDREW
;  Attach "NAV" for exclusive export stuff
;  
;     Rev 1.8   08 Oct 1993 14:20:12   EHU
;  Ifdef'd out everything except GETFREESTACK.
;  
;     Rev 1.7   09 Sep 1993 08:43:12   JMILLARD
;  check running process handle for ok before using
;  
;     Rev 1.6   08 Sep 1993 19:27:56   JMILLARD
;  add GETFREESTACK
;  
;     Rev 1.5   01 Jun 1993 02:24:56   JMILLARD
;  Convert reference to RunningProcess to GetThreadId
;  
;     Rev 1.4   10 Feb 1993 17:19:32   JMILLARD
;  Add INITALLSTACK and GETUNUSEDSTACK
;  
;     Rev 1.3   10 Feb 1993 12:30:26   JMILLARD
;  move data to data segment, where god intended
;  
;     Rev 1.2   08 Feb 1993 18:22:16   JMILLARD
;  cld before leaving as required by WATCOM
;  
;     Rev 1.1   03 Feb 1993 16:07:40   JMILLARD
;  stack check doesn't nuke stack unless initialized
;  
;     Rev 1.0   28 Jan 1993 22:51:42   JMILLARD
;  Initial revision.
;  
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


include CTSN.INC

GetThreadID     PROTO NEAR SYSCALL      ; OS routine to get thread id

UniqueSegment

;-----------------------------------------------------------------------------
;
;-----------------------------------------------------------------------------
__CHK PROC USES XAX XCX XDI,
        dwStackNeeded:DWORD,

        test    bNukeStack,001h
        jz      @F                      ; don't nuke
        
        mov     ecx,dwStackNeeded
        or      ecx,ecx                 ; count of zero - no initialize
        jz      @F

        mov     eax,esp
        dec     eax                     ; first free byte of stack
        mov     edi,eax
        mov     eax,dwStackInit         ; stack init value
        std                             ; count down the stack

        rep     stosb                   ; nuke the stack
        cld                             ; restore direction flag, so
                                        ; CLIB string routines don't 
                                        ; explode
@@:
        ret
__CHK        ENDP


ifdef DEBUG_MEMORY
INITSTACK PROC USES XAX, 
        dwStackInitValue:DWORD,

        mov     eax,dwStackInitValue
        mov     dwStackInit,eax

        mov     al,001h                 ; set nuke stack flag
;@@@@@@@mov     bNukeStack,al

        ret
INITSTACK ENDP


;-----------------------------------------------------------------------------
;
; 11aug95 JMILLARD - STS 41492 - don't do illegal stack access - this crashes
;                    protected domain - do only if running in strict mode (a
;                    a debug mode
;
;-----------------------------------------------------------------------------

INITALLSTACK PROC USES XAX XBX,

ifdef DEBUG_MEMORY_STRICT

        INVOKE  GetThreadID             ; get current process structure
        mov     ebx,eax                 
        mov     ebx,[ebx+00ch]          ; get stack limit

        mov     eax,esp
        sub     eax,ebx                 ; amount to initialize

        sub     eax,01ch                ; leave a little slack for __CHK

        INVOKE  __CHK,eax               ; initialize stack

endif   ; ifdef DEBUG_MEMORY_STRICT

        ret
INITALLSTACK ENDP

;-----------------------------------------------------------------------------
;
; 11aug95 JMILLARD - STS 41492 - don't do illegal stack access - this crashed
;                    protected domain - do only if running in strict mode (a
;                    a debug mode
;
;-----------------------------------------------------------------------------

GETUNUSEDSTACK PROC USES XBX XCX XDI,

ifdef DEBUG_MEMORY_STRICT

        INVOKE  GetThreadID             ; get current process structure
        mov     ebx,eax                 
        mov     ebx,[ebx+00ch]          ; get stack limit
        add     ebx,4                   ; skip past stack fence

        mov     edi,ebx                 ; set up search
        mov     al,[edi]                ; look for not this byte
        mov     ecx,08000h              ; but don't look very far

        cld                     
        repe    scasb                   ; 

        inc     ecx                     ; correct for overrotation
        sub     ecx,08000h              
        neg     ecx

        mov     eax,ecx                 ; and return this

endif   ; ifdef DEBUG_MEMORY_STRICT

        ret
GETUNUSEDSTACK ENDP
endif   ; ifdef DEBUG_MEMORY

ifdef UNUSED_CODE
NAVGETFREESTACK PROC USES XBX,

        INVOKE  GetThreadID             ; get current process structure
        or      eax,eax                 ; make sure valid (for 4.0)
        jnz     @F
        mov     eax,08000h              ; pretend there is this much
        jmp     AllDone
@@:
        mov     ebx,eax                 
        mov     ebx,[ebx+00ch]          ; get stack limit
        add     ebx,4                   ; skip past stack fence

        mov     eax,esp
        sub     eax,ebx
AllDone:
        ret
NAVGETFREESTACK ENDP
endif

.data

dwStackInit     dd      0fefefefeh      ; stack initialize value
bNukeStack      db      00h             ; nuke stack flag
;-----------------------------------------------------------------------------
END
