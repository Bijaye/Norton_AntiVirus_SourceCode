;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_CAL.ASv   1.2   10 Apr 1996 15:10:46   DALLEE  $
;;
;; Description: Defines REP4_CALLS function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_CAL.ASv  $
;  
;     Rev 1.2   10 Apr 1996 15:10:46   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:26:06   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   23 May 1994 09:08:50   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_CALLS
;
; Decides on repair system and invokes it.
;
; UINT WINAPI REP4_CALLS ( UINT HostHandle, UINT uBuffer, LPVOID lpBuffer );
;
; Input:
;       1) Handle
;       2) Buffer size
;       2) Buffer, filled with data
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
;----------------------------------------------------------------------------
REP4_CALLS  PROC  CODEDISTANCE  USES XBX XCX XDX XSI,
        HostHandle:UINT,                ; Handle of infected file
        uBuffer:UINT,                   ; Size of work buffer
        lpBuffer:LPBYTE                 ; Ptr to my work buffer

        LDS_SI  lpBuffer

        cmp     [XSI].RepairStruct.wMZ,WORD PTR 0FFFFh
        jne     RC_1
        cmp     [XSI + 2].RepairStruct.wMZ,WORD PTR 0FFFFh
        jne     RC_1

        INVOKE  REP4_SYS_NORM, HostHandle, uBuffer, lpBuffer
        jmp     RC_OUT

RC_1:
        mov     ax,[XSI].RepairStruct.xN30.wControl  ;Get ctrl word into ax

        cmp     [XSI].RepairStruct.wMZ,WORD PTR "ZM"
        je      RC_EXE
        cmp     [XSI].RepairStruct.wMZ,WORD PTR "MZ"
        je      RC_EXE

        and     al,GENERIC_COM             ;Decide on COM cleaner
        cmp     al,GENERIC_COM

        je      RC_GENERIC

RC_PCOM:
        INVOKE  REP4_PREPENT, HostHandle, uBuffer, lpBuffer
        jmp     RC_OUT

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

RC_GENERIC:

        INVOKE  REP4_GENERIC, HostHandle, uBuffer, lpBuffer
        jmp     RC_OUT

RC_EXE_NORM:

        INVOKE  REP4_EXE_NORM, HostHandle, uBuffer, lpBuffer

RC_OUT:
        ret

REP4_CALLS  ENDP

UniqueSegmentEnd
END
