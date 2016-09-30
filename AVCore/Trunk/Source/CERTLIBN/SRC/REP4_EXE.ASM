;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_EXE.ASv   1.2   10 Apr 1996 15:10:48   DALLEE  $
;;
;; Description: Defines REP4_EXE_NORM function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_EXE.ASv  $
;  
;     Rev 1.2   10 Apr 1996 15:10:48   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:26:14   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   23 May 1994 19:43:36   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_EXE_NORM
;
; Repairs EXEs by rebuilding the EXE header.
;
; BOOL WINAPI REP4_EXE ( UINT HostHandle, UINT uBuffer, LPVOID lpBuffer );
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
REP4_EXE_NORM  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Handle of infected file
        uBuffer:UINT,                   ; Size of work buffer
        lpBuffer:LPBYTE                 ; work buffer contains:
                                        ;  (1) clean information from DB
                                        ;  (2) variables used by routines?
                                        ;  (3) header read from infected host

        LOCAL dwVirStart:DWORD          ; holds the offset of start of the
                                        ; virus in infected file

        LDS_SI  lpBuffer                ; point to buffer


                                        ; next calculate the infected file size

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_END
IF OS_16
                                        ; find start of infection

        sub     ax,[XSI].RepairStruct.xN30.wInfSize
        sbb     dx,0


        mov     WORD PTR dwVirStart+2,dx; remember start of virus code offset
        mov     WORD PTR dwVirStart,ax  ; ASSUMPTION: This code assumes that
                                        ; the CS:IP/SS:SP were stored as
                                        ; specified in the database *EVEN*
                                        ; if we had to slide in order to find
                                        ; our bookmark.

                                        ; obtain offset of CS stored in vir

        add     ax, [XSI].RepairStruct.xN30.wCS_REG
        adc     dx, 0

                                        ; seek from top of file to CS loc
                                        ; read the CS value and place in
                                        ; the EXE header buffer read from
                                        ; infected file, overwriting virus's
                                        ; CS value.

        INVOKE  FIL4_SEEK,HostHandle,dx::ax,fSEEK_BEG
ELSE
                                        ; Same as above, but 32-bit
        movuw   XDX,[XSI].RepairStruct.xN30.wInfSize
        sub     XAX,XDX
        mov     dwVirStart,XAX
        movuw   XDX,[XSI].RepairStruct.xN30.wCS_REG
        add     XAX,XDX
        INVOKE  FIL4_SEEK,HostHandle,XAX,fSEEK_BEG

ENDIF
        INVOKE  FIL4_READ,HostHandle,ADDR [XSI].RepairStruct.w_cs,2
        cmp     XAX,2
        jne     R_ERROR

IF OS_16
                                        ; now time to retrieve the IP value

        mov     ax, WORD PTR dwVirStart ; DX:AX points to start of virus again
        mov     dx, WORD PTR dwVirStart + 2

        add     ax,[XSI].RepairStruct.xN30.wIP_REG
        adc     dx,0

        INVOKE  FIL4_SEEK,HostHandle,dx::ax,fSEEK_BEG
ELSE
                                        ; Same as above, but 32-bit
        movuw   XAX,[XSI].RepairStruct.xN30.wIP_REG
        add     XAX,dwVirStart
        INVOKE  FIL4_SEEK,HostHandle,XAX,fSEEK_BEG
ENDIF
        INVOKE  FIL4_READ,HostHandle,ADDR [XSI].RepairStruct.w_ip, 2
        cmp     XAX,2
        jne     R_ERROR

                                        ; now time to retrieve the SS value
                                        ; in this case, we must check to see
                                        ; if we NEED to fix the SS value
                                        ; (perhaps the virus didn't change it)

        cmp     [XSI].RepairStruct.xN30.wSS_REG, -1
        je      NO_SS
IF OS_16
        mov     ax, WORD PTR dwVirStart ; DX:AX points to start of virus again
        mov     dx, WORD PTR dwVirStart + 2

        add     ax,[XSI].RepairStruct.xN30.wSS_REG
        adc     dx,0

        INVOKE  FIL4_SEEK,HostHandle,dx::ax,fSEEK_BEG
ELSE
                                        ; Same as above, but 32-bit
        movuw   XAX,[XSI].RepairStruct.xN30.wSS_REG
        add     XAX,dwVirStart
        INVOKE  FIL4_SEEK,HostHandle,XAX,fSEEK_BEG
ENDIF
        INVOKE  FIL4_READ,HostHandle,ADDR [XSI].RepairStruct.w_ss, 2
        cmp     XAX,2
        jne     R_ERROR

NO_SS:

                                        ; now time to retrieve the SP value
                                        ; in this case, we must check to see
                                        ; if we NEED to fix the SP value
                                        ; (perhaps the virus didn't change it)

        cmp     [XSI].RepairStruct.xN30.wSP_REG, -1
        je      NO_SP
IF OS_16
        mov     ax, WORD PTR dwVirStart ; DX:AX points to start of virus again
        mov     dx, WORD PTR dwVirStart+2

        add     ax,[XSI].RepairStruct.xN30.wSP_REG
        adc     dx,0

        INVOKE  FIL4_SEEK,HostHandle,dx::ax,fSEEK_BEG
ELSE
        movuw   XAX,[XSI].RepairStruct.xN30.wSP_REG
        add     XAX,dwVirStart
        INVOKE  FIL4_SEEK,HostHandle,XAX,fSEEK_BEG
ENDIF
        INVOKE  FIL4_READ,HostHandle,ADDR [XSI].RepairStruct.w_sp, 2
        cmp     XAX,2
        jne     R_ERROR

NO_SP:

                                        ; time to recalculate the uninfected
                                        ; file size and place this information
                                        ; back into the header.

        mov     dx,WORD PTR dwVirStart+2; grab our back-up copy!
        mov     ax,WORD PTR dwVirStart  ; ASK JOE: \/\/\/
        mov     bx,512                  ; divide by 512 to find out how big we
        div     bx                      ; are.  AX=#pages, DX=#MOD bytes

        cmp     dx,0                    ; if MOD=0 then we inc the number of
        je      @F                      ; pages (IE, if the file is a multiple
        inc     ax                      ; of 512 bytes, we add one)
                                        ; for whatever reason?!?!
@@:
        mov     [XSI].RepairStruct.wParalen,ax
        mov     [XSI].RepairStruct.wModlen,dx

        ; now lets take care of tweaking the CS/SS values by -10h

        test    [XSI].RepairStruct.xN30.wControl2, TWEEK_CS_10
        jz      @F
        sub     [XSI].RepairStruct.w_cs, 10h
@@:
        test    [XSI].RepairStruct.xN30.wControl2, TWEEK_SS_10
        jz      @F
        sub     [XSI].RepairStruct.w_ss, 10h

@@:

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG
        INVOKE  FIL4_WRITE,HostHandle,ADDR [XSI].RepairStruct.wMZ,24
        cmp     XAX,24
        jne     R_ERROR

        mov     XAX,TRUE
        jmp     R_DONE
R_ERROR:
        xor     XAX,XAX
R_DONE:
        ret

REP4_EXE_NORM  ENDP

UniqueSegmentEnd
END
