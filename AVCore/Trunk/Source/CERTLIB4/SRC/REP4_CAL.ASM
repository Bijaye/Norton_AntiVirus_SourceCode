include CTS4.INC
include repair.inc

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_CALLS                       1/21/93                             JOE 
;
;     Decides on repair system and invokes it.
; $Header:   S:\certlib4\vcs\rep4_cal.asv   1.8   29 Nov 1994 13:05:42   DALLEE  $
;
; UINT WINAPI REP4_CALLS(WORD HostHandle, WORD uBuffer, LPSTR lpBuffer);
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
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_cal.asv  $
;  
;     Rev 1.8   29 Nov 1994 13:05:42   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.6.1.0   19 Oct 1993 07:24:42   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.6   12 Oct 1993 16:34:46   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.5.1.0   03 Sep 1993 19:30:48   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.5   24 Aug 1993 13:36:50   CAREY
;  Fixed check for .SYS files.
;  
;     Rev 1.4   10 May 1993 18:53:06   SZIADEH
;  
;  Included "repair.inc"
;-----------------------------------------------------------------------------
REP4_CALLS      PROC USES XBX XCX XDX XSI,
                HostHandle:WORD,        ; Handle of infected file
                uBuffer:WORD,           ; Size of work buffer
                lpBuffer:LPSTR          ; Ptr to my work buffer

;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        LDS_SI  lpBuffer

        cmp     [XSI].RepairStruct.wMZ,word ptr 0FFFFh
        jne     RC_1
        cmp     [XSI+2].RepairStruct.wMZ,word ptr 0FFFFh
        jne     RC_1

        INVOKE  REP4_SYS_NORM,HostHandle,uBuffer,lpBuffer
        jmp     RC_OUT

RC_1:
        mov     ax,[XSI].RepairStruct.xN30.wControl  ;Get ctrl word into ax

        cmp     [XSI].RepairStruct.wMZ,word ptr "ZM"
        je      RC_EXE
        cmp     [XSI].RepairStruct.wMZ,word ptr "MZ"
        je      RC_EXE

        and     al,GENERIC_COM             ;Decide on COM cleaner
        cmp     al,GENERIC_COM

        je      RC_GENERIC

;--------------------------------------
RC_PCOM:
        INVOKE  REP4_PREPENT,HostHandle,uBuffer,lpBuffer
        jmp     RC_OUT
        
;--------------------------------------
RC_EXE:
        and     al,GENERIC_EXE
        cmp     al,GENERIC_EXE
        jne     RC_EXE_NORM

        mov     ax,[XSI].RepairStruct.xN30.wHeader
        mov     [XSI].RepairStruct.xN30.wHostJmp,ax
        mov     ax,[XSI].RepairStruct.xN30.wHeadLen
        mov     [XSI].RepairStruct.xN30.wJmpLen,ax
        mov     ax,[XSI].RepairStruct.xN30.wInfSize     ; Fix for tag bytes
        add     ax,[XSI].RepairStruct.xN30.wTagSize     ; in Jerusalem group
        mov     [XSI].RepairStruct.xN30.wInfSize,ax
;--------------------------------------
RC_GENERIC:
        
        INVOKE  REP4_GENERIC,HostHandle,uBuffer,lpBuffer
        jmp     RC_OUT

;--------------------------------------
RC_EXE_NORM:

        INVOKE  REP4_EXE_NORM,HostHandle,uBuffer,lpBuffer

;--------------------------------------
RC_OUT:
        ret

REP4_CALLS      ENDP
;--------------------------------------------------------------------------
END

