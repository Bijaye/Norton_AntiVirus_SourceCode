;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\scn_crc.asv   1.2   29 Nov 1995 16:01:48   DCHI  $
;;
;; Description: Defines SCN4_CRC function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\scn_crc.asv  $
;  
;     Rev 1.2   29 Nov 1995 16:01:48   DCHI
;  With new support for INSCANF greater than 64K, CRCs now come
;  right after the Header.
;  
;     Rev 1.1   17 Aug 1994 13:26:24   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   02 May 1994 18:23:18   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SCN4_CRC
;
; Scan for a match in the CRC'ed string.
;
; WORD WINAPI SCN4_CRC ( LPSTR Buffer, UINT BufferSize,
;                        LPSTR Data, WORD uStartAt );
;
; Input:
;       1)  Buffer to look in
;       2)  Size of buffer
;       3)  Data to work with
;       4)  Number of CRCs already checked (not sure about that)
; Output:
;       1)  No Virus = 0 or Virus ID=?
;
;----------------------------------------------------------------------------
SCN4_CRC  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        uBuffer:UINT,
        lpData:LPSTR,
        uStartAt:WORD                   ; uStartAt will be 0 unless this
                                        ; is a second(+) lookup.

        LOCAL crc:WORD

        INVOKE  MEM4_CRC,0,lpBuffer,16

        or      ax,ax                   ; if returned CRC is 0 exit
        jz      OKout

        mov     crc,ax

        LES_DI  lpData
        movuw   XCX,XES[XDI].HEADER.uNumCRCs
        sub     cx,uStartAt             ; sub CRCs already checked

IFDEF SYM_TSR
        movuw   XAX,XES[XDI].HEADER.uNumALGs
        mov     XBX,SIZE_SIG
;********************************************************************
;** May be problem when ALGS forces DX to be used in multiplication
;** That will occur when we have over 3855 algorithmic sigs.
;********************************************************************
        mul     XBX
        add     XDI,XAX
ENDIF

        add     XDI,sHEADER             ; ES:DI Now points to where CRC start

        movuw   XAX,uStartAt            ; Add for a second lookup
        shl     XAX,1                   ; Double for sizeof WORD
        add     XDI,XAX

        cld                             ; Scan memory for CRC match
        mov     ax,crc
        repne   scasw
        jne     OKout

        LDS_SI  lpData
        mov     ax,[XSI].HEADER.uNumCRCs
        inc     cx                      ; cx is decremented before compare
                                        ; so it is always one too far
        sub     ax,cx
        add     ax,[XSI].HEADER.uNumBOOs
        add     ax,[XSI].HEADER.uNumALGs
        inc     ax                      ; 1-base virus id

        jmp     STout
OKout:
        xor     ax,ax
STout:
        ret

SCN4_CRC  ENDP

UniqueSegmentEnd
END
