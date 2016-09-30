;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_DCR.ASv   1.4   10 Apr 1996 15:10:46   DALLEE  $
;;
;; Description: Defines the following functions:
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_DCR.ASv  $
;  
;     Rev 1.4   10 Apr 1996 15:10:46   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.3   10 Nov 1995 16:07:40   GDZIECI
;  Ifdef'd out unused code for NLM.
;  
;     Rev 1.2   17 Aug 1994 13:26:08   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   11 May 1994 16:48:20   RSTANEV
;  Converted all functions for WIN32.
;
;     Rev 1.0   05 May 1994 12:48:00   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_DECRYPT
;
; FLAGS REP4_DECRYPT ( UINT wHostHandle, LPVOID lpBuffer );
;
; Input:
;       1) Host handle
;       2) Entry after FIL4_GET_ENTRY
; Output:
;       1) CARRY SET if cannot locate sig
;       2) NO CARRY if it did its job
;
;----------------------------------------------------------------------------

LOCBUFSIZE      EQU     8

IFNDEF SYM_NLM
REP4_DECRYPT  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Handle of infected file
        lpBuffer:LPBYTE                 ; Entry after FIL4_GET_ENTRY

        LOCAL   LocBuf[LOCBUFSIZE]:BYTE

        xor     XCX,XCX

        LDS_SI  lpBuffer                ; SDS[XSI] pts to Buffer base

        mov     dx,[XSI].RepairStruct.xN30.wInfSize
        sub     dx,[XSI].RepairStruct.xN30.wBookLocation

        test    [XSI].RepairStruct.xN30.wControl,SECONDBOOK
        jz      @F
        dec     XDX                     ; seek backwards one less
        inc     [XSI].RepairStruct.xN30.wBookLocation ; look 1 further
        inc     XSI                     ; move us one byte further in sig
@@:
        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,LOCBUFSIZE
        cmp     XAX,LOCBUFSIZE
        jne     J_EXIT_ERROR

        add     XSI,N30.lpMem + 2       ; Use memory sig area, starting
                                        ; with 3rd byte.
        INVOKE  DETERMINE_XOR_BYTE,ADDR LocBuf,vSDS XSI
        jc      J_LOOK4ADDBYTE

; At this point, we add a complexity.  In the case where the virus itself
; is not encrypted, but the repair data is, the XOR determination would have
; gone through and found the "memsig" with base and delta = 0.  At this point,
; check the BookMark.  If there is a BookMark value, this designates the
; situation we're looking for, that of a location to pick up a key (base).
; If there is a value, start checking flags to determine what to do.

; Remember that AX has base/delta.

        LES_DI  lpBuffer
        mov     bx,XES[XDI].RepairStruct.xN30.wBookMark
        or      bx,bx
        jz      J_DOXORBYTE

; BX has the offset from the beginning of the virus of the decryption key
        mov     dx,XES[XDI].RepairStruct.xN30.wInfSize
        sub     dx,bx
        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
; Get decryption key
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        cmp     XAX,2
        jne     J_EXIT_ERROR
; AX now has decryption key
        mov     ax,WORD PTR LocBuf
; Determine decryption type to use
        mov     cx,XES[XDI].RepairStruct.xN30.wControl2
        test    cx,DECRYPT_XOR
        jnz     J_SETUPXOR
        test    cx,DECRYPT_ADD
        jnz     J_SETUPADD
; Fallthrough means use subtract.  But we only add, so negate the value
        test    cx,DECRYPT_WORD
        jnz     @F
        xor     ah,ah                   ; Force Delta to 0
        neg     al
        jmp     J_DOADDBYTE
@@:
        neg     ax
J_PREADDWORD:
        xor     XDX,XDX
        jmp     J_DOADDWORD
J_SETUPADD:
        test    cx,DECRYPT_WORD
        jnz     J_PREADDWORD
        xor     ah,ah                   ; Force Delta to 0
        jmp     J_DOADDBYTE
J_PREXORWORD:
        xor     XDX,XDX
        jmp     J_DOXORWORD
J_SETUPXOR:
        test    cx,DECRYPT_WORD
        jnz     J_PREXORWORD
        xor     ah,ah                   ; Force Delta to 0

J_DOXORBYTE:
        INVOKE  DECRYPT_XOR_BYTE,HostHandle,lpBuffer,ax
        jmp     J_EXIT_GOOD
J_LOOK4ADDBYTE:
        INVOKE  DETERMINE_ADD_BYTE,ADDR LocBuf,vSDS XSI
        jc      @F
J_DOADDBYTE:
        INVOKE  DECRYPT_ADD_BYTE,HostHandle,lpBuffer,ax
        jmp     J_EXIT_GOOD
@@:
        INVOKE  DETERMINE_XOR_WORD,ADDR LocBuf,vSDS XSI
        jc      @F
J_DOXORWORD:
        INVOKE  DECRYPT_XOR_WORD,HostHandle,lpBuffer,ax,dx
        jmp     J_EXIT_GOOD
@@:
        INVOKE  DETERMINE_ADD_WORD,ADDR LocBuf,vSDS XSI
        jc      @F
J_DOADDWORD:
        INVOKE  DECRYPT_ADD_WORD,HostHandle,lpBuffer,ax,dx
        jmp     J_EXIT_GOOD
@@:
        INVOKE  DETERMINE_ADD_OSCIL_WORD,ADDR LocBuf,vSDS XSI
        jc      @F
        INVOKE  DECRYPT_ADD_OSCIL_WORD,HostHandle,lpBuffer,ax,dx
        jmp     J_EXIT_GOOD
@@:
        INVOKE  DETERMINE_XOR_OSCIL_WORD,ADDR LocBuf,vSDS XSI
        jc      @F
        INVOKE  DECRYPT_XOR_OSCIL_WORD,HostHandle,lpBuffer,ax,dx
        jmp     J_EXIT_GOOD

@@:

J_EXIT_ERROR:
        stc
        jmp     J_EXIT
J_EXIT_GOOD:
        clc
J_EXIT:
        ret

REP4_DECRYPT  ENDP
ENDIF

;----------------------------------------------------------------------------
; DETERMINE_XOR_BYTE
;
; ? DETERMINE_XOR_BYTE ( LPVOID Buf, LPVOID Sig );
;
; Input:
;       1) ?
;       2) ?
; Output:
;       ?
;
;----------------------------------------------------------------------------
DETERMINE_XOR_BYTE  PROC  CODEDISTANCE  USES XCX XDX SDS SES XSI XDI,
        Buf:LPBYTE,                     ; Buffer to look in
        Sig:LPBYTE                      ; Signature to look for

        LDS_SI  Buf
        LES_DI  Sig
        mov     ax,XES[XDI]             ; our sig
        dec     al                      ; remove +1 encoding
        dec     ah                      ; remove +1 encoding
        xor     ax,[XSI]                ; Base is in AL
        sub     ah,al                   ; Delta is in AH
        mov     XDX,XAX                 ; use DX
        add     dl,dh                   ; create next xor
        add     dl,dh
        mov     cx,dx                   ; save key information
        mov     dh,XES[XDI+2]           ; get next sig byte
        dec     dh                      ; remove +1 encoding
        xor     dl,dh                   ; xor and see
        cmp     dl,[XSI+2]
        jnz     XOR_BYTE_FAIL
        mov     dx, cx
        add     dl, dh                  ; prepare to test 4th byte
        mov     dh,XES[XDI+3]           ; get next sig byte
        dec     dh                      ; remove +1 encoding
        xor     dl,dh                   ; xor and see
        cmp     dl,[XSI+3]
        je      @F                      ; if equal, yea!

XOR_BYTE_FAIL:

        stc
@@:
                                        ; AH has Delta, AL has Base
        ret

DETERMINE_XOR_BYTE  ENDP


IFNDEF SYM_NLM
;----------------------------------------------------------------------------
; DECRYPT_XOR_BYTE
;
; ? DECRYPT_XOR_BYTE ( UINT HostHandle, LPVOID lpBuffer, WORD DeltaBase );
;
; Input:
;       1) ?
;       2) ?
;       3) ?
; Output:
;       ?
;
;----------------------------------------------------------------------------

DECRYPT_XOR_BYTE  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Infected file
        lpBuffer:LPBYTE,                ; Entry after FIL4_GET_ENTRY
        DeltaBase:WORD                  ; Delta:Base

        LOCAL   LocBuf[2]:BYTE

        xor     XCX,XCX

        LDS_SI  lpBuffer                 ; SDS[XSI] pts to Buffer base

        mov     dx,[XSI].RepairStruct.xN30.wInfSize
        sub     dx,[XSI].RepairStruct.xN30.wBookLocation

        mov     bx,DeltaBase             ; BH has Delta, BL has base

        push    XDX                      ; save location of bookmark
        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
DXB_LOOP1:
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,1
        cmp     XAX,1
        jne     @F
        xor     LocBuf,bl
        add     bl,bh
        INVOKE  FIL4_SEEK,HostHandle,1,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,1
        jmp     DXB_LOOP1

@@:
        pop     XDX                      ; get back location of bookmark
        inc     XDX

        xor     XCX,XCX

        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
        mov     bx,DeltaBase             ; BH has Delta, BL has base
        mov     cx,[XSI].RepairStruct.xN30.wBookLocation
DXB_LOOP2:
        ; If we got this far and are getting errors, we'll just keep
        ; destroying the first byte.  But then, something weird will
        ; have occurred to make us repair a file that didn't match our
        ; other checks, thus an errant repair would have occurred...
        ; And that's what we would have, an errant repair.
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,1
        sub     bl,bh
        xor     LocBuf,bl
        INVOKE  FIL4_SEEK,HostHandle,1,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,1
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        loop    DXB_LOOP2

        ret

DECRYPT_XOR_BYTE  ENDP
ENDIF


;----------------------------------------------------------------------------
; DETERMINE_ADD_BYTE
;
; ? DETERMINE_ADD_BYTE ( LPVOID Buf, LPVOID Sig );
;
; Input:
;       1) ?
;       2) ?
; Output:
;       ?
;
;----------------------------------------------------------------------------

DETERMINE_ADD_BYTE  PROC  CODEDISTANCE  USES XCX XDX SDS SES XSI XDI,
        Buf:LPBYTE,                     ; Buffer to look in
        Sig:LPBYTE                      ; Signature to look for

        LDS_SI  Buf
        LES_DI  Sig
        mov     ax,XES[XDI]             ; our sig
        dec     al                      ; remove +1 encoding
        dec     ah                      ; remove +1 encoding
        sub     al,[XSI]                ; Base is in AL
        sub     ah,[XSI+1]
        sub     ah,al                   ; Delta is in AH
        mov     XDX,XAX                 ; use DX
        add     dl,dh                   ; create next xor
        add     dl,dh
        mov     cx, dx                  ; save for later
        mov     dh,XES[XDI+2]           ; get next sig byte
        dec     dh                      ; remove +1 encoding
        sub     dh,dl                   ; xor and see
        cmp     dh,[XSI+2]
        jne     ADD_BYTE_FAIL
        mov     dx, cx
        add     dl, dh
        mov     dh,XES[XDI+3]           ; get next sig byte
        dec     dh                      ; remove +1 encoding
        sub     dh,dl                   ; xor and see
        cmp     dh,[XSI+3]
        je      @F                      ; if equal, yea!

ADD_BYTE_FAIL:

        stc
@@:
        ; AH has Delta, AL has Base
        ret

DETERMINE_ADD_BYTE  ENDP


IFNDEF SYM_NLM
;----------------------------------------------------------------------------
; DECRYPT_ADD_BYTE
;
; ? DECRYPT_ADD_BYTE ( UINT HostHandle, LPVOID lpBuffer, WORD DeltaBase );
;
; Input:
;       1) ?
;       2) ?
;       3) ?
; Output:
;       ?
;
;----------------------------------------------------------------------------

DECRYPT_ADD_BYTE  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Infected file
        lpBuffer:LPBYTE,                ; Entry after FIL4_GET_ENTRY
        DeltaBase:WORD                  ; Delta:Base

        LOCAL   LocBuf[2]:BYTE

        xor     XCX,XCX

        LDS_SI  lpBuffer                 ; SDS[XSI] pts to Buffer base
        mov     dx,[XSI].RepairStruct.xN30.wInfSize
        sub     dx,[XSI].RepairStruct.xN30.wBookLocation
        mov     bx,DeltaBase             ; BH has Delta, BL has base

        push    XDX                      ; save location of bookmark
        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
DAB_LOOP1:
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,1
        cmp     XAX,1
        jne     @F
        add     LocBuf,bl
        add     bl,bh
        INVOKE  FIL4_SEEK,HostHandle,1,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,1
        jmp     DAB_LOOP1

@@:
        pop     XDX                      ; get back location of bookmark
        inc     XDX

        xor     XCX,XCX

        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
        mov     bx,DeltaBase             ; BH has Delta, BL has base

        mov     cx,[XSI].RepairStruct.xN30.wBookLocation
DAB_LOOP2:
        ; If we got this far and are getting errors, we'll just keep
        ; destroying the first byte.  But then, something weird will
        ; have occurred to make us repair a file that didn't match our
        ; other checks, thus an errant repair would have occurred...
        ; And that's what we would have, an errant repair.
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,1
        sub     bl,bh
        add     LocBuf,bl
        INVOKE  FIL4_SEEK,HostHandle,1,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,1
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        loop    DAB_LOOP2

        ret

DECRYPT_ADD_BYTE  ENDP
ENDIF


;----------------------------------------------------------------------------
; DETERMINE_XOR_WORD
;
; ? DETERMINE_XOR_WORD ( LPVOID Buf, LPVOID Sig );
;
; Input:
;       1) ?
;       2) ?
; Output:
;       ?
;
;----------------------------------------------------------------------------

DETERMINE_XOR_WORD  PROC  CODEDISTANCE  USES XBX XCX SDS SES XSI XDI,
        Buf:LPBYTE,                     ; Buffer to look in
        Sig:LPBYTE                      ; Signature to look for

        LDS_SI  Buf
        LES_DI  Sig
        mov     bx,XES[XDI]             ; our sig
        dec     bl                      ; remove +1 encoding
        dec     bh                      ; remove +1 encoding
        xor     bx,[XSI]                ; Base is in BX
        mov     dx,XES[XDI+2]           ; our sig
        dec     dl                      ; remove +1 encoding
        dec     dh                      ; remove +1 encoding
        xor     dx,[XSI+2]              ; xor next word
        sub     dx,bx                   ; Delta is in DX
        mov     ax,bx                   ; use BX, let AX now have Base
        add     bx,dx                   ; create next xor
        add     bx,dx

        mov     cx,XES[XDI+4]           ; get next sig word
        dec     ch                      ; remove +1 encoding
        dec     cl                      ; remove +1 encoding
        xor     cx,bx                   ; xor and see
        cmp     cx,[XSI+4]
        jne     DXW_NO                  ; if equal, yea!

        add     bx, dx                  ; test one last word
        mov     cx,XES[XDI+6]           ; get next sig word
        dec     ch                      ; remove +1 encoding
        dec     cl                      ; remove +1 encoding
        xor     cx,bx                   ; xor and see
        cmp     cx,[XSI+6]
        je      DXW_YES

DXW_NO:
        stc
DXW_YES:
        ; DX has Delta, AX has Base
        ret

DETERMINE_XOR_WORD  ENDP


IFNDEF SYM_NLM
;----------------------------------------------------------------------------
; DECRYPT_XOR_WORD
;
; ? DECRYPT_XOR_WORD ( UINT HostHandle, LPVOID lpBuffer,
;                      WORD Base, WORD Delta );
;
; Input:
;       1) ?
;       2) ?
;       3) ?
;       4) ?
; Output:
;       ?
;
;----------------------------------------------------------------------------

DECRYPT_XOR_WORD  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Infected file
        lpBuffer:LPBYTE,                ; Entry after FIL4_GET_ENTRY
        Base:WORD,                      ; Base
        Delta:WORD                      ; Delta

        LOCAL   LocBuf:WORD

        xor     XCX,XCX

        LDS_SI  lpBuffer                ; SDS[XSI] pts to Buffer base
        mov     dx,[XSI].RepairStruct.xN30.wInfSize
        sub     dx,[XSI].RepairStruct.xN30.wBookLocation

        mov     bx,Base
        mov     di,Delta

        push    XDX                      ; save location of bookmark
        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
DXW_LOOP1:
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        cmp     XAX,2
        jne     @F
        xor     LocBuf,bx
        add     bx,di
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,2
        jmp     DXW_LOOP1

@@:
        pop     XDX                      ; get back location of bookmark
        add     XDX,2

        xor     XCX,XCX

        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
        mov     bx,Base

        mov     cx,[XSI].RepairStruct.xN30.wBookLocation
        shr     cx,1
DXW_LOOP2:
        ; see comments in similar block above
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        sub     bx,di
        xor     LocBuf,bx
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,2
        INVOKE  FIL4_SEEK,HostHandle,4,fSEEK_BAK
        loop    DXW_LOOP2

        ret

DECRYPT_XOR_WORD  ENDP
ENDIF


;----------------------------------------------------------------------------
; DETERMINE_ADD_WORD
;
; ? DETERMINE_ADD_WORD ( LPVOID Buf, LPVOID Sig );
;
; Input:
;       1) ?
;       2) ?
; Output:
;       ?
;
;----------------------------------------------------------------------------

DETERMINE_ADD_WORD  PROC  CODEDISTANCE  USES XBX XCX SDS SES XSI XDI,
        Buf:LPBYTE,                     ; Buffer to look in
        Sig:LPBYTE                      ; Signature to look for

        LDS_SI  Buf
        LES_DI  Sig
        mov     bx,XES[XDI]             ; our sig
        dec     bl                      ; remove +1 encoding
        dec     bh                      ; remove +1 encoding
        sub     bx,[XSI]                ; Base is in BX
        mov     dx,XES[XDI+2]           ; our sig
        dec     dl                      ; remove +1 encoding
        dec     dh                      ; remove +1 encoding
        sub     dx,[XSI+2]              ; sub next word
        sub     dx,bx                   ; Delta is in DX
        mov     ax,bx                   ; use BX, let AX now have Base
        add     bx,dx                   ; create next xor

        add     bx,dx
        mov     cx,XES[XDI+4]           ; get next sig word
        dec     ch                      ; remove +1 encoding
        dec     cl                      ; remove +1 encoding
        sub     cx,bx                   ; sub and see
        cmp     cx,[XSI+4]
        jne     DAW_NO                  ; if equal, yea!

        add     bx,dx                   ; add in delta again
        mov     cx,XES[XDI+6]           ; get next sig word
        dec     ch                      ; remove +1 encoding
        dec     cl                      ; remove +1 encoding
        sub     cx,bx                   ; sub and see
        cmp     cx,[XSI+6]
        je      DAW_YES

DAW_NO:
        stc

DAW_YES:
        ; DX has Delta, AX has Base
        ret

DETERMINE_ADD_WORD  ENDP


IFNDEF SYM_NLM
;----------------------------------------------------------------------------
; DECRYPT_ADD_WORD
;
; ? DECRYPT_ADD_WORD ( UINT HostHandle, LPBYTE lpBuffer,
;                      WORD Base, WORD Delta );
;
; Input:
;       1) ?
;       2) ?
;       3) ?
;       4) ?
; Output:
;       ?
;
;----------------------------------------------------------------------------

DECRYPT_ADD_WORD  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Infected file
        lpBuffer:LPBYTE,                ; Entry after FIL4_GET_ENTRY
        Base:WORD,                      ; Base
        Delta:WORD                      ; Delta

        LOCAL   LocBuf:WORD

        xor     XCX,XCX

        LDS_SI  lpBuffer                ; SDS[XSI] pts to Buffer base
        mov     dx,[XSI].RepairStruct.xN30.wInfSize
        sub     dx,[XSI].RepairStruct.xN30.wBookLocation

        mov     bx,Base
        mov     di,Delta

        push    XDX                      ; save location of bookmark
        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
DAW_LOOP1:
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        cmp     XAX,2
        jne     @F
        add     WORD PTR LocBuf,bx
        add     bx,di
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,2
        jmp     DAW_LOOP1

@@:
        pop     XDX                      ; get back location of bookmark
        add     XDX,2

        xor     XCX,XCX

        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
        mov     bx,Base

        mov     cx,[XSI].RepairStruct.xN30.wBookLocation
        shr     cx,1
DAW_LOOP2:
        ; see comments in similar block above
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        sub     bx,di
        add     WORD PTR LocBuf,bx
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,2
        INVOKE  FIL4_SEEK,HostHandle,4,fSEEK_BAK
        loop    DAW_LOOP2

        ret

DECRYPT_ADD_WORD  ENDP

;----------------------------------------------------------------------------
; Code for the oscillating decryptors
;----------------------------------------------------------------------------
DETERMINE_ADD_OSCIL_WORD  PROC  CODEDISTANCE  USES XBX XCX SDS SES XSI XDI,
        Buf:LPBYTE,                     ; Buffer to look in
        Sig:LPBYTE                      ; Signature to look for

        LDS_SI  Buf
        LES_DI  Sig
        mov     ax,XES[XDI]             ; our sig
        dec     al                      ; remove +1 encoding
        dec     ah                      ; remove +1 encoding
        sub     ax,[XSI]                ; Base is in AX
        mov     dx,XES[XDI+2]           ; our sig
        dec     dl                      ; remove +1 encoding
        dec     dh                      ; remove +1 encoding
        sub     dx,[XSI+2]              ; sub next word
        mov     bx, dx                  ; * store in BX for later
        xor     dx, ax                  ; DX now holds secondary XOR value

        mov     cx,XES[XDI+4]
        dec     cl
        dec     ch
        sub     cx, [XSI+4]
        cmp     cx, ax
        jne     DAOW_NO                 ; if not equal...

        mov     cx,XES[XDI+6]
        dec     cl
        dec     ch
        sub     cx, [XSI+6]
        cmp     cx, bx
        je      DAOW_YES                ; if equal, yea!

DAOW_NO:
        stc

DAOW_YES:
        ; DX has Delta, AX has Base
        ret

DETERMINE_ADD_OSCIL_WORD  ENDP

;----------------------------------------------------------------------------

DECRYPT_ADD_OSCIL_WORD  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Infected file
        lpBuffer:LPBYTE,                ; Entry after FIL4_GET_ENTRY
        Base:WORD,                      ; Base
        Delta:WORD                      ; Delta

        LOCAL   LocBuf:WORD

        xor     XCX,XCX

        LDS_SI  lpBuffer                 ; SDS[XSI] pts to Buffer base
        mov     dx,[XSI].RepairStruct.xN30.wInfSize
        sub     dx,[XSI].RepairStruct.xN30.wBookLocation
        mov     bx,Base
        mov     di,Delta

        push    XDX                      ; save location of bookmark
        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
DAOW_LOOP1:
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        cmp     XAX,2
        jne     @F
        add     WORD PTR LocBuf,bx
        xor     bx,di
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,2
        jmp     DAOW_LOOP1

@@:
        pop     XDX                      ; get back location of bookmark
        add     XDX,2

        xor     XCX,XCX

        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
        mov     bx,Base

        mov     cx,[XSI].RepairStruct.xN30.wBookLocation
        shr     cx,1
DAOW_LOOP2:
        ; see comments in similar block above
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        xor     bx,di
        add     LocBuf,bx
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,2
        INVOKE  FIL4_SEEK,HostHandle,4,fSEEK_BAK
        loop    DAOW_LOOP2

        ret

DECRYPT_ADD_OSCIL_WORD  ENDP

;----------------------------------------------------------------------------

DETERMINE_XOR_OSCIL_WORD  PROC  CODEDISTANCE  USES XBX XCX SDS SES XSI XDI,
        Buf:LPBYTE,                     ; Buffer to look in
        Sig:LPBYTE                      ; Signature to look for

        LDS_SI  Buf
        LES_DI  Sig
        mov     ax,XES[XDI]             ; our sig
        dec     al                      ; remove +1 encoding
        dec     ah                      ; remove +1 encoding
        xor     ax,[XSI]                ; Base is in BX

        mov     dx,XES[XDI+2]           ; our sig
        dec     dl                      ; remove +1 encoding
        dec     dh                      ; remove +1 encoding
        xor     dx,[XSI+2]              ; xor next word
        mov     bx, dx                  ; store 2nd XOR value in BX for later
        xor     dx,ax                   ; Delta is in DX

        mov     cx,XES[XDI+4]           ; get next sig word
        dec     ch                      ; remove +1 encoding
        dec     cl                      ; remove +1 encoding
        xor     cx,ax                   ; xor and see
        cmp     cx,[XSI+4]
        jne     DXOW_NO                 ; if equal, yea!

        mov     cx,XES[XDI+6]           ; get next sig word
        dec     ch                      ; remove +1 encoding
        dec     cl                      ; remove +1 encoding
        xor     cx,bx                   ; xor and see
        cmp     cx,[XSI+6]
        je      DXOW_YES                ; if equal, yea!

DXOW_NO:
        stc

DXOW_YES:
        ; DX has Delta, AX has Base
        ret

DETERMINE_XOR_OSCIL_WORD  ENDP

;----------------------------------------------------------------------------

DECRYPT_XOR_OSCIL_WORD  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        HostHandle:UINT,                ; Infected file
        lpBuffer:LPBYTE,                ; Entry after FIL4_GET_ENTRY
        Base:WORD,                      ; Base
        Delta:WORD                      ; Delta

        LOCAL   LocBuf:WORD

        xor     XCX,XCX
        LDS_SI  lpBuffer                 ; SDS[XSI] pts to Buffer base
        mov     dx,[XSI].RepairStruct.xN30.wInfSize
        sub     dx,[XSI].RepairStruct.xN30.wBookLocation
        mov     bx,Base
        mov     di,Delta

        push    XDX                      ; save location of bookmark
        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
DXOW_LOOP1:
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        cmp     XAX,2
        jne     @F
        xor     WORD PTR LocBuf,bx
        xor     bx,di
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,2
        jmp     DXOW_LOOP1

@@:
        pop     XDX                      ; get back location of bookmark
        add     XDX,2

        xor     XCX,XCX

        INVOKE  FIL4_SEEK,HostHandle,cx::dx,fSEEK_END
        mov     bx,Base

        mov     cx,WORD PTR[XSI].RepairStruct.xN30.wBookLocation
        shr     cx,1
DXOW_LOOP2:
        ; see comments in similar block above
        INVOKE  FIL4_READ,HostHandle,ADDR LocBuf,2
        xor     bx,di
        xor     LocBuf,bx
        INVOKE  FIL4_SEEK,HostHandle,2,fSEEK_BAK
        INVOKE  FIL4_WRITE,HostHandle,ADDR LocBuf,2
        INVOKE  FIL4_SEEK,HostHandle,4,fSEEK_BAK
        loop    DXOW_LOOP2

        ret

DECRYPT_XOR_OSCIL_WORD  ENDP
ENDIF                           ; IFNDEF SYM_NLM

;----------------------------------------------------------------------------

UniqueSegmentEnd
END
