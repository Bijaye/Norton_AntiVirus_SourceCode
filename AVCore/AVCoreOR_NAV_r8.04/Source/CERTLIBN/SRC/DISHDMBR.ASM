;----------------------------------------------------------------------------
; DIS4_LOC_HIDDEN_MBR
;
; Returns the cyl, side, sector of the real MBR.
;
; AX:DX WINAPI DIS4_LOC_HIDDEN_MBR ( BYTE Drive, LPSTR Buffer );
;
; Input:
;       1)  Bootable drive (80h or 81h)
;       3)  Work buffer of at least 1024 bytes.
; Output:
;       1)  AX:DX contain appropriate information
;                 AL = contains Head
;                 AH = physical drive number
;                 DL = low 6-bits contain sector number
;                 DH = cylinder number. two hi bits in dl
;
;       2)  AX = -1 on failure
;----------------------------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/DISHDMBR.ASv   1.5   08 Mar 1996 14:56:36   DALLEE  $
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/DISHDMBR.ASv  $
;  
;     Rev 1.5   08 Mar 1996 14:56:36   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.4   29 Apr 1995 13:36:42   SZIADEH
;  commented out unused code.
;  
;     Rev 1.3   27 Jan 1995 19:17:52   SZIADEH
;  work under WIN32 platform.
;  
;     Rev 1.2   19 Jan 1995 15:15:00   SZIADEH
;  uncommented the code.
;  
;     Rev 1.1   17 Aug 1994 13:25:42   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   26 May 1994 08:28:08   RSTANEV
;  Initial revision.
;----------------------------------------------------------------------------
INCLUDE CTSN.INC

UniqueSegment

DIS4_LOC_HIDDEN_MBR  PROC  CODEDISTANCE  USES XBX XCX XSI XDI SDS SES,
        Drive:BYTE,
        Buffer:LPBYTE

        LOCAL   CylSec:UINT
        LOCAL   Side:BYTE

IF 0
                                        ; are we bootlocked at all?
        INVOKE  DIS4_BOOTLOCKED_STATE,Drive,Buffer
        or      XAX,XAX
        jz      NotBootlocked

        cmp     XAX,3                   ; MBR replaced & TSR in memory
        jz      J_0
        jnz     NoTSR                   ; only MBR replaced
J_0:
                                        ; search the int 13h handler
                                        ; for the real MBR's location
        mov     XAX,ISR13seg

IF OS_32
        mov     edi, eax
ELSE
IF bSYM_WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax,ax
        jz      Err
ENDIF
        mov     es, ax
        xor     XDI, XDI
ENDIF
        jmp     J_1


NoTSR:
                                        ; must get the disk location
                                        ; from the Bootcode already
                                        ; on disk

        INVOKE  DIS4_READ_PHYSICAL,Buffer,2,0,1,0,Drive
        or      XAX,XAX
        jnz     Err

ifdef SYM_WIN16
        mov     ax, word ptr Buffer+2
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
        mov     word ptr Buffer+2, ax
endif
        LES_DI  Buffer

        cld
        mov     XCX,100h
        mov     al,0FCh                 ; CMP
Search:
                                        ; look for "MOV AH"
        repnz   scasb
        cmp     BYTE PTR XES[XDI], 03h  ; AH
        jcxz    Err                     ; should have found it
        jne     Search
J_1:
                                        ; look for "MOV CX"
        mov     XCX,32
        mov     al,0B9h                 ; MOV CX
        repnz   scasb
        jcxz    Err                     ; should have found it
        mov     ax,WORD PTR XES[XDI+0]  ; Cyl/Sec component
        mov     CylSec, XAX
        mov     al,BYTE PTR XES[XDI+3]  ; Side component
        mov     Side,al

        mov     ah,Drive
        mov     al,Side
        mov     XDX,CylSec
        jmp     SHORT Done

ENDIF   ; end if 0 from above

NotBootlocked:

        xor     XAX, XAX
        mov     ah,Drive                ; return the location of the real MBR
        xor     al,al                   ; head
        mov     XDX,0001h               ; cyl/sec
        jmp     SHORT Done
Err:
        mov     XAX, -1
        mov     XDX, XAX
Done:
        ret

DIS4_LOC_HIDDEN_MBR  ENDP

UniqueSegmentEnd
END
