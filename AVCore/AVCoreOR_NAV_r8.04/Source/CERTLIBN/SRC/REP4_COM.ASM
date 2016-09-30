;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_COM.ASv   1.6   10 Apr 1996 15:10:46   DALLEE  $
;;
;; Description: Defines REP4_PREPENT function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_COM.ASv  $
;  
;     Rev 1.6   10 Apr 1996 15:10:46   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.5   05 Apr 1995 13:48:04   DALLEE
;  Fixed bug for 32 bit platforms.
;  Calling INVOKE with a WORD in place of a DWORD parameter assembles to
;   PUSH dword ptr 0, PUSH word ptr WORD.  A total of 6 bytes, not the
;   expected 4.
;  This is bad.
;  
;     Rev 1.4   17 Aug 1994 13:26:06   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.3   18 Jul 1994 16:42:50   CAREY
;  Fixed the real problem with PREPENDING com repairs in 16 bit after 32bit port
;  
;     Rev 1.2   18 Jul 1994 15:39:14   CAREY
;  Fixed 32 bit port bug (DX and AX were switched)
;  
;     Rev 1.1   29 Jun 1994 17:19:52   CAREY
;  Fixed a bug which caused improved overwriting repairs to fail on viruses with
;  a size evenly divisible by 512.
;  
;     Rev 1.0   23 May 1994 19:43:50   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_PREPENT
;
; Removes a prepending or improved overwriting virus from the host.
;
; BOOL WINAPI REP4_PREPENT ( UINT HostHandle, UINT uBuffer, LPVOID lpBuffer );
;
; Input:
;       1) Handle
;       2) Size of Buffer
;       3) Buffer, filled with data
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;----------------------------------------------------------------------------
REP4_PREPENT  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Handle of infected file
        uBuffer:UINT,                   ; Size of work buffer
        lpBuffer:LPBYTE                 ; Ptr to my work buffer

        LOCAL lpBUF:LPSTR
        LOCAL wBYTES:WORD
        LOCAL wPASS:WORD
        LOCAL wREST:WORD
        LOCAL dwREAD:DWORD
        LOCAL dwWRITE:DWORD

        LDS_SI  lpBuffer

IF OS_16
        mov     WORD PTR lpBUF + 2,ds
        mov     ax,si
        add     ax,sREPAIR
        mov     WORD PTR lpBUF,ax
ELSE
        mov     XAX,XSI
        add     XAX,sREPAIR
        mov     lpBuf,XAX
ENDIF
                                        ; Store size in dwREAD for SEEKs
        movuw   XAX,[XSI].RepairStruct.xN30.wInfSize
IF OS_16
        mov     WORD PTR dwREAD, ax     ; this line and the next needed their
        mov     WORD PTR dwREAD + 2,0   ; source operands swapped -Carey
        mov     WORD PTR dwWRITE+2,0
        mov     WORD PTR dwWRITE,0
ELSE
        mov     dwREAD,XAX
        mov     dwWRITE,0
ENDIF

        mov     ax,[XSI].RepairStruct.xN30.wControl

        and     ax,IMPROVED             ; Is it improved overwriting?
        jz      RP_3
;--------------------------------------------------------------
; For improved overwriters get virus size to calculate the
; passes since only VIRUS_SIZE bytes of host are moved.
;--------------------------------------------------------------
        mov     ax,[XSI].RepairStruct.xN30.wInfSize

        cmp     ax,512                  ; Check size
        jg      RP_1
        mov     WORD PTR wPASS,1        ; If size < 512 then PASS = 1
        mov     WORD PTR wREST,ax       ;                    REST = size
        jmp     RP_2
RP_1:
        xor     dx,dx                   ; Else Calculate
        mov     bx,512
        div     bx

        inc     ax
        mov     WORD PTR wPASS,ax       ; PASS = Quotient
        mov     WORD PTR wREST,dx       ; REST = Remainder
RP_2:
        mov     dx,fSEEK_END            ; SEEK from end
        jmp     RP_SEEK

;----------------------------------------------------------------
; For prepending viruses get file size - virus size to calculate
; original host size. Passes are based on this since all of host
; has to be moved forward.
;----------------------------------------------------------------
RP_3:
        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_END

IF OS_32
        ror     XAX,10h
        mov     dx,ax
        ror     XAX,10h
ENDIF
        sub     ax,[XSI].RepairStruct.xN30.wInfSize
        sbb     dx,0

        jc      RP_EXIT                 ; we should NEVER carry out of the
                                        ; high word unless the sample is too
                                        ; small, indicating error (carey)

        mov     bx,512
        div     bx

        inc     ax
        mov     WORD PTR wPASS,ax       ; PASS = Quotient
        mov     WORD PTR wREST,dx       ; REST = Remainder

        mov     dx,fSEEK_BEG            ; SEEK from start

RP_SEEK:

        INVOKE  FIL4_SEEK,HostHandle,dwREAD,dx          ; dx based on type
IF OS_16
        mov     WORD PTR dwREAD+2,dx                    ; Set Initial READ
        mov     WORD PTR dwREAD,ax                      ; Initial WRITE is
ELSE
        mov     dwREAD,XAX
ENDIF
        mov     WORD PTR wBYTES,512                     ;  start of file.

;----------------------------------------------------------------------------
;  The read pointer points to the host code.
;  The write pointer points to virus code.
;----------------------------------------------------------------------------
RP_LOOP:
        cmp     wPASS,1                                 ; If last PASS
        jne     RP_4                                    ;
        mov     ax,WORD PTR wREST                       ; Use remainder
        mov     WORD PTR wBYTES,ax

        or      ax, ax                                  ; make sure we don't
        jz      RP_OUT                                  ; have a remainder of
                                                        ; 0, because this will
                                                        ; cause a WRITE of 0,
                                                        ; and a truncation of
                                                        ; the file...

;----------------------------------------------------------------------------
; READ HOST
RP_4:
        INVOKE  FIL4_SEEK,HostHandle,dwREAD,fSEEK_BEG   ; Set Ptr
        movuw   XAX,wBYTES
        INVOKE  FIL4_READ,HostHandle,lpBUF,XAX          ; Read
IF OS_16
        add     WORD PTR dwREAD,512                     ; Move Ptr
        adc     WORD PTR dwREAD+2, 0                    ; *CAREY
ELSE
        add     dwREAD,512
ENDIF
;----------------------------------------------------------------------------
; WRITE HOST
        INVOKE  FIL4_SEEK,HostHandle,dwWRITE,fSEEK_BEG  ; Set Ptr
        movuw   XAX,wBYTES
        INVOKE  FIL4_WRITE,HostHandle,lpBUF,XAX         ; Write
IF OS_16
        add     WORD PTR dwWRITE,512                    ; Move Ptr
        adc     WORD PTR dwWRITE+2, 0                   ; *CAREY
ELSE
        add     dwWRITE,512
ENDIF
;----------------------------------------------------------------------------
        dec     wPASS                                   ; Dec PASSes
        jnz     RP_LOOP                                 ; Exit on last
RP_OUT:
        mov     XAX,TRUE
        jmp     RP_EXIT

RP_ERROR:

        xor     XAX,XAX

RP_EXIT:
        ret

REP4_PREPENT  ENDP

UniqueSegmentEnd
END
