include CTS4.INC
include repair.inc

UniqueSegment
;---------------------------------------------------------------------------
; REP4_EXE_NORM                    1/21/93                             JOE
;
;     Repairs EXEs by rebuilding the EXE header.
; $Header:   S:\certlib4\vcs\rep4_exe.asv   1.11   29 Nov 1994 13:06:12   DALLEE  $
;
; UINT WINAPI REP4_EXE(WORD HostHandle, WORD uBuffer,LPSTR lpBuffer);
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
; $Log:   S:\certlib4\vcs\rep4_exe.asv  $
;  
;     Rev 1.11   29 Nov 1994 13:06:12   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.9.1.0   19 Oct 1993 07:23:46   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.9   12 Oct 1993 16:33:56   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.8.2.0   03 Sep 1993 19:30:24   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.8   28 Jul 1993 17:27:48   CJKUO
;  Change exe_repair's don't care condition to -1 from 0
;  
;     Rev 1.7   16 Jul 1993 23:19:38   dallee
;  Added tweaking of CS and SS fields if proper bits are set.
;  
;     Rev 1.6   16 May 1993 20:31:14   JWELLS
;  Simplified and tested
;  
;     Rev 1.5   10 May 1993 19:04:58   SZIADEH
;  Included "repair.inc"
;---------------------------------------------------------------------------
REP4_EXE_NORM   PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                uBuffer:WORD,           ; Size of work buffer
                lpBuffer:LPSTR          ; work buffer contains:
                                        ;  (1) clean information from DB
                                        ;  (2) variables used by routines?
                                        ;  (3) header read from infected host

                LOCAL dwVirStart:DWORD  ; holds the offset of start of the
                                        ; virus in infected file


        LDS_SI  lpBuffer                ; point to buffer


                                        ; next calculate the infected file size

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_END

                                        ; find start of infection

        sub     ax,[XSI].RepairStruct.xN30.wInfSize
        sbb     dx,0


        mov     word ptr dwVirStart+2,dx; remember start of virus code offset
        mov     word ptr dwVirStart,ax  ; ASSUMPTION: This code assumes that
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
        INVOKE  FIL4_READ,HostHandle,ADDR [XSI].RepairStruct.w_cs, 2
        cmp     ax,2
        jne     R_ERROR

                                        ; now time to retrieve the IP value

        mov     ax, word ptr dwVirStart ; DX:AX points to start of virus again
        mov     dx, word ptr dwVirStart+2

        add     ax, [XSI].RepairStruct.xN30.wIP_REG
        adc     dx, 0

        INVOKE  FIL4_SEEK,HostHandle,dx::ax,fSEEK_BEG
        INVOKE  FIL4_READ,HostHandle,ADDR [XSI].RepairStruct.w_ip, 2
        cmp     ax,2
        jne     R_ERROR

                                        ; now time to retrieve the SS value
                                        ; in this case, we must check to see
                                        ; if we NEED to fix the SS value
                                        ; (perhaps the virus didn't change it)

        cmp     [XSI].RepairStruct.xN30.wSS_REG, -1
        je      NO_SS

        mov     ax, word ptr dwVirStart ; DX:AX points to start of virus again
        mov     dx, word ptr dwVirStart+2

        add     ax, [XSI].RepairStruct.xN30.wSS_REG
        adc     dx, 0

        INVOKE  FIL4_SEEK,HostHandle,dx::ax,fSEEK_BEG
        INVOKE  FIL4_READ,HostHandle,ADDR [XSI].RepairStruct.w_ss, 2
        cmp     ax,2
        jne     R_ERROR

NO_SS:

                                        ; now time to retrieve the SP value
                                        ; in this case, we must check to see
                                        ; if we NEED to fix the SP value
                                        ; (perhaps the virus didn't change it)

        cmp     [XSI].RepairStruct.xN30.wSP_REG, -1
        je      NO_SP

        mov     ax, word ptr dwVirStart ; DX:AX points to start of virus again
        mov     dx, word ptr dwVirStart+2

        add     ax, [XSI].RepairStruct.xN30.wSP_REG
        adc     dx, 0

        INVOKE  FIL4_SEEK,HostHandle,dx::ax,fSEEK_BEG
        INVOKE  FIL4_READ,HostHandle,ADDR [XSI].RepairStruct.w_sp, 2
        cmp     ax,2
        jne     R_ERROR

NO_SP:

                                        ; time to recalculate the uninfected
                                        ; file size and place this information
                                        ; back into the header.

        mov     dx,word ptr dwVirStart+2; grab our back-up copy!
        mov     ax,word ptr dwVirStart  ; ASK JOE: \/\/\/
        mov     bx,512                  ; divide by 512 to find out how big we
        div     bx                      ; are.  AX=#pages, DX=#MOD bytes

        cmp     dx,0                    ; if MOD=0 then we inc the number of
        je      @F                      ; pages (IE, if the file is a multiple
        inc     ax                      ; of 512 bytes, we add one)
                                        ; for whatever reason?!?!
@@:
        mov     word ptr[XSI].RepairStruct.wParalen,ax
        mov     word ptr[XSI].RepairStruct.wModlen,dx


        ; now lets take care of tweaking the CS/SS values by -10h

        test    word ptr[XSI].RepairStruct.xN30.wControl2, TWEEK_CS_10
        jz      @F
        sub     word ptr[XSI].RepairStruct.w_cs, 10h
@@:
        test    word ptr[XSI].RepairStruct.xN30.wControl2, TWEEK_SS_10
        jz      @F
        sub     word ptr[XSI].RepairStruct.w_ss, 10h

@@:

        INVOKE  FIL4_SEEK,HostHandle,0,fSEEK_BEG
        INVOKE  FIL4_WRITE,HostHandle,ADDR [XSI].RepairStruct.wMZ,24
        cmp     ax,24
        jne     R_ERROR

        mov     ax,1
        jmp     R_DONE
R_ERROR:           
        xor     ax,ax
R_DONE:
        ret
   
REP4_EXE_NORM   ENDP
;--------------------------------------------------------------------------
END
