;-----------------------------------------------------------------------------
; DIS4_IS_BOOTLOCKED                  3/8/93
;
;       Returns 1 if drive is already bootloked.       
;
; WORD WINAPI DIS4_IS_BOOTLOCKED(BYTE Drive,LPSTR WorkBuffer);
;
; Input:
;       1)  Drive to read.
;       2)  Work buffer of at least 1024 bytes.
;
; Output:
;       1)  (-1) on error
;       2)  (1) if bootlocked
;       3)  (0) if not boolocked
;       4)  WorkBuffer contains MBR and next sector.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/disisbt.asv   1.1   08 Mar 1996 14:31:00   DALLEE  $
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/disisbt.asv  $
;  
;     Rev 1.1   08 Mar 1996 14:31:00   DALLEE
;  Removed W16 seg to sel code.  Physical disk read/writes all take
;  protected mode pointers.
;  Fixed possible buffer overrun.
;  Changed LPSTR's to LPBYTE's for non-string data.
;  
;     Rev 1.0   19 Jan 1995 15:13:58   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
include CTSN.INC

UniqueDataSegment

szCopyRight     byte  'Copyright (C) Symantec 1993.' ,0

UniqueDataSegmentEnd


UniqueSegment

DIS4_IS_BOOTLOCKED PROC USES XCX XDX XSI XDI SDS SES,
        Drive:BYTE,
        Buffer:LPBYTE

;-----------------------------
; read the first TWO sectors of the MBR -
; TWO to circumvent the INT 13h
; handler
;

        INVOKE  DIS4_READ_PHYSICAL,Buffer,2,0,1,0,Drive
        or      XAX, XAX
        jnz     Err

        LDS_SI  Buffer
        add     XSI,2                            ; skip the jump instructions

        INVOKE  MEM4_SRCH_STRING, vSDS XSI,ADDR szCopyRight,512-2
        or      ax,ax
        jnz     ItsBootlocked

        xor     XAX, XAX
        jmp     ReallyDone

ItsBootlocked:
        mov     XAX,1
        jmp     ReallyDone
Err:
        mov     XAX,-1
ReallyDone:
        ret

DIS4_IS_BOOTLOCKED ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
