include CTS4.INC
include repair.inc

UniqueSegment
;---------------------------------------------------------------------------
; REP4_PREPENT                     1/21/93                             JOE
;
;     Removes a prepending or improved overwriting virus from the host.
; $Header:   S:\certlib4\vcs\rep4_com.asv   1.13   29 Nov 1994 13:10:50   DALLEE  $
;
; UINT WINAPI REP4_PREPENT(WORD HostHandle, WORD uBuffer,LPSTR lpBuffer);
;
; Input:
;       1) Handle
;       2) Buffer, filled with data
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;---------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_com.asv  $
;  
;     Rev 1.13   29 Nov 1994 13:10:50   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.11.1.1   29 Jun 1994 17:11:48   CAREY
;  Fixed a bug in repairing improved overwriting viruses who's size was a
;  multiple of 512 bytes.  This bug caused NAV to truncate the infected
;  file instead of repairing it correctly.
;  
;     Rev 1.11.1.0   19 Oct 1993 07:24:22   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.11   12 Oct 1993 16:34:26   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.10.1.0   03 Sep 1993 19:30:38   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.10   23 Aug 1993 19:14:42   CAREY
;  Fixed bug in repair of prepending viruses.
;  
;     Rev 1.9   29 Jul 1993 12:29:26   CAREY
;  Fixed bug in improved overwriting repair.
;  
;     Rev 1.8   12 May 1993 16:54:24   JWELLS
;  Fixed TagByte-added problem.
;  
;     Rev 1.7   10 May 1993 19:02:24   SZIADEH
;  Included "repair.inc"
;---------------------------------------------------------------------------
REP4_PREPENT    PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                uBuffer:WORD,           ; Size of work buffer
                lpBuffer:LPSTR          ; Ptr to my work buffer

                LOCAL lpBUF:LPSTR
                LOCAL wBYTES:WORD
                LOCAL wPASS:WORD
                LOCAL wREST:WORD
                LOCAL dwREAD:DWORD
                LOCAL dwWRITE:DWORD
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        LDS_SI  lpBuffer
        mov     ax,ds

        mov     word ptr lpBUF+2,ax
        mov     ax,si
        add     ax,sREPAIR
        mov     word ptr lpBUF,ax
        
        xor     ax,ax                   ; Store size in dwREAD for SEEKs
        mov     word ptr dwREAD+2,ax
        mov     ax,[XSI].RepairStruct.xN30.wInfSize
;        sub     ax,[XSI].RepairStruct.xN30.wTagSize ; Remove Jerusalem Tag
        mov     word ptr dwREAD,ax

        mov     word ptr dwWRITE+2,0
        mov     word ptr dwWRITE,0

        mov     ax,[XSI].RepairStruct.xN30.wControl

        and     ax,IMPROVED             ; Is it improved overwriting?
        cmp     ax,IMPROVED
        jne     RP_3
;--------------------------------------------------------------
; For improved overwriters get virus size to calculate the
; passes since only VIRUS_SIZE bytes of host are moved.
;--------------------------------------------------------------

        mov     ax, [XSI].RepairStruct.xN30.wInfSize

        cmp     ax,512                  ; Check size
        jg      RP_1
        mov     word ptr wPASS,1        ; If size < 512 then PASS = 1
        mov     word ptr wREST,ax       ;                    REST = size
        jmp     RP_2
RP_1:
        xor     dx,dx                   ; Else Calculate
        mov     bx,512
        div     bx
        
        inc     ax
        mov     word ptr wPASS,ax       ; PASS = Quotient
        mov     word ptr wREST,dx       ; REST = Remainder
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
        
        sub     ax,[XSI].RepairStruct.xN30.wInfSize
        sbb     dx,0

        jc      RP_EXIT                 ; we should NEVER carry out of the
                                        ; high word unless the sample is too
                                        ; small, indicating error (carey)

        mov     bx,512
        div     bx
        
        inc     ax
        mov     word ptr wPASS,ax       ; PASS = Quotient
        mov     word ptr wREST,dx       ; REST = Remainder

        mov     dx,fSEEK_BEG            ; SEEK from start

RP_SEEK:

        INVOKE  FIL4_SEEK,HostHandle,dwREAD,dx          ; dx based on type
        mov     word ptr dwREAD+2,dx                    ; Set Initial READ
        mov     word ptr dwREAD,ax                      ; Initial WRITE is
        mov     word ptr wBYTES,512                     ;  start of file.

;-----------------------------------------------------------------------------
;  The read pointer points to the host code.
;  The write pointer points to virus code.
;-----------------------------------------------------------------------------
RP_LOOP:
        cmp     wPASS,1                                 ; If last PASS
        jne     RP_4                                    ;
        mov     ax,word ptr wREST                       ; Use remainder
        mov     word ptr wBYTES,ax

        or      ax, ax                                  ; make sure that our
        jz      RP_OUT                                  ; remainder is not 0
                                                        ; or we'll trunc the
                                                        ; file!

;---------------------------------------------------------------------------
; READ HOST
RP_4:
        INVOKE  FIL4_SEEK,HostHandle,dwREAD,fSEEK_BEG   ; Set Ptr
        INVOKE  FIL4_READ,HostHandle,lpBUF,wBYTES       ; Read
        add     word ptr dwREAD,512                     ; Move Ptr
        adc     word ptr dwREAD+2, 0                    ; *CAREY
;-----------------------------------------------------------------------------
; WRITE HOST
        INVOKE  FIL4_SEEK,HostHandle,dwWRITE,fSEEK_BEG  ; Set Ptr
        INVOKE  FIL4_WRITE,HostHandle,lpBUF,wBYTES      ; Read
        add     word ptr dwWRITE,512                    ; Move Ptr
        adc     word ptr dwWRITE+2, 0                   ; *CAREY
;-----------------------------------------------------------------------------
        dec     wPASS                                   ; Dec PASSes
        cmp     wPASS,0                                 ;
        jne     RP_LOOP                                 ; Exit on last
RP_OUT:
        mov     ax,1
        jmp     RP_EXIT

RP_ERROR:

        xor     ax, ax

RP_EXIT:
        ret

REP4_PREPENT    ENDP
;--------------------------------------------------------------------------
END

