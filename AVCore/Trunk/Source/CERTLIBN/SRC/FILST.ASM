; Copyright 1993 Symantec, Peter Norton Product Group
;-----------------------------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/FILST.ASv   1.5   08 Mar 1996 14:56:52   DALLEE  $
;
; Program self check routine.
;
; See Also:
;       filcrcfl.asm                    contains FIL4_CRCFILE.
;       \SOURCE\TOOLS\EXECRC.EXE        stamps EXE's with correct CRC value.
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/FILST.ASv  $
;  
;     Rev 1.5   08 Mar 1996 14:56:52   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.4   05 Apr 1995 13:48:02   DALLEE
;  Fixed bug for 32 bit platforms.
;  Calling INVOKE with a WORD in place of a DWORD parameter assembles to
;   PUSH dword ptr 0, PUSH word ptr WORD.  A total of 6 bytes, not the
;   expected 4.
;  This is bad.
;  
;     Rev 1.3   09 Dec 1994 20:29:40   SZIADEH
;  added UniqueSegmentEnd.
;  
;     Rev 1.2   09 Dec 1994 17:47:58   SZIADEH
;  deleted all references to ANSITOOEM and OEMTOANSI stuff.
;  
;     Rev 1.1   08 Dec 1994 20:24:36   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
;  
;     Rev 1.12   29 Nov 1994 13:10:24   DALLEE
;  Reget from Qak2 post NAV.305
;-----------------------------------------------------------------------------

include CTSN.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_SELF_TEST                      1/26/93                        bAd
;
;    Performs a CRC test on a file previously stamped by FIL4_CRCFILE.
;    Use EXECRC.EXE in Quake \SOURCE\TOOLS to stamp an executable file with
;    the correct CRC value.
;
;    It checks the WORD value in bytes 12h and 13h in the file's header
;    versus the CRC value computed for the first SELF_TEST_BUFFER_SIZE bytes
;    of the file (disregarding bytes 12h and 13h).
;
; WORD WINAPI FIL4_SELF_TEST(LPSTR Filename, LPBYTE Buffer);
;
; Input:
;       1) Pointer to filename to perform self test on.
;       2) Work buffer of SELF_TEST_BUFFER_SIZE (512) bytes
; Output:
;       1) 0 = SELF_TEST_SUCCESS    - Success, CRC STAMP matched.
;       2) 1 = SELF_TEST_NO_CRC     - File has not been CRC STAMPED.
;       3) 2 = SELF_TEST_FAIL       - Failed CRC has CHANGED, stamp is different
;       4) 3 = SELF_TEST_FILE_ERROR - File IO error
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_SELF_TEST PROC USES XBX XCX XDI XSI SES,
        lpFilename:LPSTR,
        Buffer:LPBYTE

;----- open file
        INVOKE  FIL4_OPEN,lpFilename,(fREAD OR 020h)
        cmp     XAX, -1
        jz      FileError
        mov     XBX,XAX

;------ read the header into a buffer
        INVOKE  FIL4_READ,XBX,Buffer,SELF_TEST_BUFFER_SIZE
        mov     XCX,XAX
        INVOKE  FIL4_CLOSE,XBX
        cmp     XCX,SELF_TEST_BUFFER_SIZE
        jne     FileError

;----- store what's at the CHECKSUM location 12h-13h
        LES_DI  Buffer
        mov     bx,word ptr XES[XDI+12h]
        cmp     bx,0                    ; Never INITIALIZED?
        je      NeverInitialized

;----- make them NULL
        mov     word ptr XES[XDI+12h],0000h

;----- perform a CRC of the header (Fudge it to non-zero, if necessary)
        INVOKE  MEM4_CRC,0,Buffer,SELF_TEST_BUFFER_SIZE
        or      ax, ax
        jnz     J_Good_CRC

        mov     ax, SELF_TEST_ZERO_CRC

J_Good_CRC:

;----- does the CRC == CHECKSUM?
        cmp     ax,bx
        jz      OkDone

BadCRC:
        mov     ax, SELF_TEST_FAIL
        jmp     done

NeverInitialized:
        mov     ax, SELF_TEST_NO_CRC
        jmp     done

OkDone:
        mov     ax, SELF_TEST_SUCCESS
        jmp     done

FileError:
        mov     ax, SELF_TEST_FILE_ERROR

done:
        ret

FIL4_SELF_TEST ENDP
;----------------------------------------------------------------------------
UniqueSegmentEnd
END
