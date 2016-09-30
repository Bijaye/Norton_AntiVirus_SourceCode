;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\scn_malt.asv   1.2   17 Aug 1994 13:26:26   CAREY  $
;;
;; Description: Defines SCN4_MALTA function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\scn_malt.asv  $
;  
;     Rev 1.2   17 Aug 1994 13:26:26   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   19 Jul 1994 16:24:48   CAREY
;  Changed si/di references to XSI/XDI to handle 32-bit correctly.
;  
;     Rev 1.0   31 May 1994 16:43:24   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SCN4_MALTA
;
; ???
;
; WORD WINAPI SCN4_MALTA ( LPSTR lpBuffer, UINT uBuffer,
;                          LPSTR lpDATA, LPSTR lpStartStruct );
;
; Input:
;       1) ptr Buffer to examine
;       2) ptr to DATA
;
; Output:
;       1) No Match = 0
;       2) VID from fixed ID in DATA
;----------------------------------------------------------------------------
SCN4_MALTA  PROC  CODEDISTANCE  USES XBX XCX XDX XSI XDI SES SDS,
        lpBuffer:LPSTR,
        uBuffer:UINT,
        lpDATA:LPSTR,
        StartStruct:LPSTR

        xor     XDI,XDI
        xor     XSI,XSI
        xor     XCX,XCX
        xor     XBX,XBX

        LES_DI  lpBuffer
        mov     XSI,XDI                 ; si->XSI, di->XDI   -Carey
        mov     cx,10
        mov     al,9Ch
        cld
        repne   scasb
        jcxz    NoMalt
        mov     cx,4
        mov     al,50h
        repne   scasb
        jcxz    NoMalt
Malt:
        xor     bx,bx
        mov     ax,0E48Ah
        call    INCBX
        mov     ax,0C08Ah
        call    INCBX
        mov     ax,0D28Ah
        call    INCBX
        mov     ax,0C98Bh
        call    INCBX
        mov     ax,0C08Bh
        call    INCBX
        mov     ax,0DB8Bh
        call    INCBX
        mov     ax,09191h
        call    INCBX
        cmp     bx,5
        jb      NoMalt

        LDS_SI  lpDATA
        mov     ax,[XSI].HEADER.wFixedIDs + FID_MALTESE
        jmp     IsMalt
NoMalt:
        xor     ax,ax
IsMalt:
        ret

SCN4_MALTA  ENDP

INCBX  PROC  NEAR                       ; routine to locate mutation pads
        cld
        mov     XDI,XSI                 ; di->XDI, si->XSI        -Carey
        mov     cx,48
IBX:
        cmp     ax,WORD PTR XES[XDI]
        jne     IBX2
        inc     bx
IBX2:
        inc     XDI                     ; di->XDI                 -Carey
        loop    IBX
        ret

INCBX   ENDP

UniqueSegmentEnd
END
