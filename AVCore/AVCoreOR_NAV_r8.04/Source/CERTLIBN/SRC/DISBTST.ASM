;-----------------------------------------------------------------------------
; DIS4_BOOTLOCKED_STATE
;
;       Returns the current bootlocked/not bootlocked state of the machine.
;
; WORD WINAPI DIS4_BOOTLOCKED_STATE(BYTE Drive,LPBYTE Buffer);
;
; Input:
;       1)  Drive to examine. 
;       2)  Work Buffer of at least 1024 bytes.     
; Output:
;       1) (-1) on error 
;       2) (0000 0000) not bootlocked at all 
;       3) (0000 0001) MBR locked 
;       4) (0000 0010) INT 13h trap in memory
;
;
;-----------------------------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/DISBTST.ASv   1.3   08 Mar 1996 14:56:36   DALLEE  $
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/DISBTST.ASv  $
;  
;     Rev 1.3   08 Mar 1996 14:56:36   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   29 Apr 1995 13:31:12   SZIADEH
;  commented out the code because it's not needed.
;  
;     Rev 1.1   27 Jan 1995 19:17:54   SZIADEH
;  work under WIN32 platform.
;  
;     Rev 1.0   19 Jan 1995 15:13:56   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
include CTSN.INC
UniqueSegment

DIS4_BOOTLOCKED_STATE PROC USES XBX XCX XDX XSI XDI SES SDS,
        Drive:BYTE,
        Buffer:LPBYTE

        XOR    XAX, XAX
        ret
IF 0


        xor     XBX, XBX
;------------------------------
; Is the MBR locked?
;

        INVOKE  DIS4_IS_BOOTLOCKED,Drive,Buffer
        or      XAX, XAX
        jz      NotLocked
        or      XBX, 1
NotLocked:
ifndef MIRRORS
;------------------------------
; Is INT 13h trapped?
;
        mov     XAX, ISR13seg

IF OS_32
        mov     edi, eax
ELSE
IF bSYM_WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax,ax
        jz      Err
ENDIF
        mov     es, ax
        xor     XDI,XDI
ENDIF

; look for the CMP  AH,03 / JMP
        cld
        mov     XAX, 0FC80h
        mov     XCX,32
        repne   scasw
        jcxz    NotInMem                         ; couldn't find
        cmp     byte ptr XES[XDI+1],0ebh         ; disble JMP there?
        jz      NotInMem
        or      XBX,2
NotInMem:        
        jcxz    CleanMachine
        jmp     Done

Err:    
        mov     XBX, -1
        jmp     done

CleanMachine:
Done:
endif ; MIRRORS
        mov     XAX, XBX
        ret
ENDIF    ; if 0 from above

DIS4_BOOTLOCKED_STATE endp
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END

