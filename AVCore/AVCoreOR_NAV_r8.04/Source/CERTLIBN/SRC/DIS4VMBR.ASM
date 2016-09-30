;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/dis4vmbr.asv   1.6   15 Jul 1996 18:49:16   RAY  $
;;
;; Description: Defines DIS4_VALIDATE_MBR function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/dis4vmbr.asv  $
;  
;     Rev 1.6   15 Jul 1996 18:49:16   RAY
;  Added special case for NT partitioned drives with > 1024 cylinders which have
;  starting and ending values FF FF FF.
;  
;     Rev 1.5   08 Mar 1996 14:56:36   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.4   11 Jan 1996 18:00:50   JWORDEN
;  Port changes from Quake 8
;  
;     Rev 1.3.1.2   01 Dec 1995 20:43:50   MKEATIN
;  Now repairs mbr's wether or not the partitions are active.
;  
;     Rev 1.3.1.1   30 Nov 1995 16:22:42   MKEATIN
;  Added parameter byDrive
;  
;     Rev 1.3.1.0   12 Jul 1995 18:58:22   MPYE
;  Branch base for version QAK8
;  
;     Rev 1.3   12 Jun 1995 16:58:22   SZIADEH
;  xchanged bl,bh to correctly calculate cylinder value.
;  
;  
;     Rev 1.2   17 Aug 1994 13:25:40   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   26 May 1994 14:55:00   RSTANEV
;  Minor changes.
;
;     Rev 1.0   25 May 1994 17:42:40   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DIS4_VALIDATE_MBR
;
; BOOL WINAPI DIS4_VALIDATE_MBR ( LPSTR lpBuffer );
;
; Active DOS partitions ONLY. If the active partition is non-DOS and does
; not end with the 0x55AA, this test fails.
;
; Input:
;       byDrive         drive number
;       lpBuffer        work buffer
; Output:
;       FALSE           failure
;       TRUE            successful
;----------------------------------------------------------------------------
DIS4_VALIDATE_MBR  PROC  CODEDISTANCE  USES XBX XCX XDX XSI SDS,
        byDrive:BYTE,
        lpBuffer:LPBYTE

        LOCAL   uCyl:WORD
        LOCAL   bySec:BYTE
        LOCAL   byHead:BYTE

        LDS_SI  lpBuffer

        add     XSI,PARTITION_OFFSET    ; point to first partition
        mov     XCX,4                   ; counter

next_part:
        cmp     BYTE PTR[XSI],080h           ; Look for active partition
        je      @F                           ; found it! jmp
        cmp     BYTE PTR[XSI],0              ; not active
        jne     R_ERROR                      ; if neither 0 nor 80, jmp error
        cmp     BYTE PTR[XSI+PARTENTRY.peFileSystem],0 
                                             ; Is nonactive partition used?
        jne     @F                           ; Yes! jmp   
        add     XSI,sPARTENTRY               ; point to next partition
        loop    next_part

        jmp     R_ERROR                      ; no active partition was found

@@:
        cmp     WORD PTR[XSI+01h], 0FFFFh   ; Special case for drives with
        jne     @F                          ; > 1024 cylinders.  NT creates
        cmp     BYTE PTR[XSI+03h], 0FFh     ; partitions with the maximum
        jne     @F                          ; starting and ...

        cmp     WORD PTR[XSI+05h], 0FFFFh   ; ending values on such drives.
        jne     @F
        cmp     BYTE PTR[XSI+07h], 0FFh
        jne     @F

        push    XSI
        push    ds
        LDS_SI  lpBuffer

        cmp     WORD PTR[XSI+01FEh], 0AA55h ; Just to be sure, check that
        pop     ds                          ; this sector ends with 55AA.
        pop     XSI
        jne     @F                          ; Just go back to the old way of
                                            ; validating, cause this doesn't
                                            ; look right.

                                            ; We need a way to get the extra-
                                            ; large dimensions of this drive
                                            ; in order to find the boot sector.
        mov     XAX,TRUE                    ;
        jmp     R_EXIT                      ; For now, just return OK.

@@:
        mov     dh,BYTE PTR[XSI+01h]
        mov     byHead,dh
        mov     bx,WORD PTR[XSI+02h]
        mov     bySec,bl
        and     bySec,03Fh              ; 11 1111b

        mov     XCX,06
        shr     bl,cl
        xchg    bh,bl
        mov     uCyl,bx

        LDS_SI  lpBuffer

        INVOKE  DIS4_READ_PHYSICAL,vSDS XSI,1,uCyl,bySec,byHead,byDrive
        or      XAX,XAX
        jnz     R_ERROR

        cmp     WORD PTR [XSI+510],0AA55h
        mov     XAX,TRUE                ; assume OK
        je      R_EXIT
R_ERROR:
        xor     XAX,XAX
R_EXIT:
        ret

DIS4_VALIDATE_MBR  ENDP

UniqueSegmentEnd
END
