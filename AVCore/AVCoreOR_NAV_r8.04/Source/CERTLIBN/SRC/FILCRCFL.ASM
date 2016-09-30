;-----------------------------------------------------------------------------
; FIL4_CRCFILE                03/16/93                     bAd
;
;     Performs a CRC on an .EXE file and then places the CRC word into the
;      .EXE header at offset 12h.
;
; WORD WINAPI FIL4_CRCFILE(LPSTR Filename,LPSTR Buffer);
;
; Input:
;       1)  Filename of file to perfrom CRC on
;       2)  Work buffer of SELF_TEST_BUFFER_SIZE (512) bytes.
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/FILCRCFL.ASv  $
;  
;     Rev 1.1   08 Mar 1996 14:56:54   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.0   19 Jan 1995 15:14:00   SZIADEH
;  Initial revision.
;  
;     Rev 1.6   29 Nov 1994 13:04:34   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.4.1.0   19 Oct 1993 07:23:56   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.4   12 Oct 1993 16:34:04   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.3.2.0   03 Sep 1993 19:26:56   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.3   26 Jul 1993 12:10:24   BRADR
;  Reflects changes made to FIL4_OPEN: -1 on error instead of 0.
;  
;     Rev 1.2   24 Jun 1993 12:41:14   DALLEE
;  Check number of bytes returned on FIL4_READ rather than ERR,
;  Use SELF_TEST_BUFFER_SIZE for number of bytes to CRC.
;  If == 0, fudge computed CRC's to non-zero value (SELF_TEST_ZERO_CRC = 1).
;-----------------------------------------------------------------------------
include CTSN.INC

UniqueSegment

FIL4_CRCFILE PROC USES XCX XDI SES,
        Filename:LPSTR,
        Buffer:LPBYTE,

        LOCAL   Handle:UINT

        LES_DI  Buffer

        INVOKE  FIL4_OPEN,Filename,fREAD_WRITE
        cmp     XAX, -1
        jz      Err

        mov     Handle, XAX
                                                  ; read in header
        INVOKE  FIL4_READ, Handle, vSES XDI, SELF_TEST_BUFFER_SIZE
        cmp     XAX, SELF_TEST_BUFFER_SIZE
        jne     Err

        mov     word ptr XES[XDI+12h], 0000h      ; null out checksum values
                                                  ; do CRC
        INVOKE  MEM4_CRC,0,vSES XDI,SELF_TEST_BUFFER_SIZE
        or      XAX, XAX
        jnz     J_Good_CRC

        mov     XAX, SELF_TEST_ZERO_CRC            ; fudge the value to non-zero

J_Good_CRC:
        mov     byte ptr XES[XDI+0], al           ; the checksum values
        mov     byte ptr XES[XDI+1], ah           ; (byte reversed order)
        
        INVOKE  FIL4_SEEK, Handle, 12h, fSEEK_BEG ; seek to checksum location
        cmp     XAX, -1
        jz      Err

        INVOKE  FIL4_WRITE,Handle,vSES XDI,2      ; write values out to checksum
        cmp     XAX, -1                            ; locations
        jz      Err

        xor     XAX, XAX
        jmp     Done
Err:
        mov     XAX,-1
Done:
        push    XAX
        INVOKE  FIL4_CLOSE, Handle
        pop     XAX

        ret

FIL4_CRCFILE ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
