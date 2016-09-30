; Copyright 1993 Symantec, Peter Norton Product Group
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\filst.asv   1.12   29 Nov 1994 13:10:24   DALLEE  $
;
; Program self check routine.
;
; See Also:
;       filcrcfl.asm                    contains FIL4_CRCFILE.
;       \SOURCE\TOOLS\EXECRC.EXE        stamps EXE's with correct CRC value.
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\filst.asv  $
;  
;     Rev 1.12   29 Nov 1994 13:10:24   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.9.1.1   01 Nov 1993 15:38:38   MFALLEN
;  Added a global flag that is being checked to see if Ansi to Oem and vice versa
;  conversion should be done on the filename prior to processing.
;  
;     Rev 1.9.1.0   19 Oct 1993 07:26:04   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.9   12 Oct 1993 16:36:20   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.8.2.1   07 Oct 1993 18:43:32   MFALLEN
;  Function accepting ANSI string in Windows.
;  
;     Rev 1.8.2.0   03 Sep 1993 19:26:50   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.8   26 Jul 1993 12:10:38   BRADR
;  Reflects changes made to FIL4_OPEN: -1 on error instead of 0.
;  
;     Rev 1.7   28 Jun 1993 16:53:54   TORSTEN
;  Changed the file mode so that overlayed programs can be opened under SHARE.
;  
;     Rev 1.6   24 Jun 1993 15:45:04   DALLEE
;  
;     Rev 1.5   24 Jun 1993 12:45:20   DALLEE
;  Use SELF_TEST_XXX return codes.
;  If == 0, fudge computed CRC's to non-zero value (SELF_TEST_ZERO_CRC = 1).
;  Use SELF_TEST_BUFFER_SIZE as size of work buffer, number of bytes to CRC.
;  
;     Rev 1.4   14 Jun 1993 18:37:42   CGUC
;  We now pass in the filename.
;  
;     Rev 1.3   10 Jun 1993 16:48:22   DALLEE
;  Removed call to MEM4_CONV_ALLOCATE.  Pass pointer to work buffer instead.
;-----------------------------------------------------------------------------

include CTS4.INC

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
; WORD WINAPI FIL4_SELF_TEST(LPSTR Filename, LPSTR Buffer);
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
        Buffer:LPSTR,
                                        ; ------------------------------------
                                        ; Convert from Ansi to Oem
                                        ; ------------------------------------
IFDEF   WIN16
        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  ANSITOOEM, lpFilename, lpFilename
@@:
ENDIF

;----- open file
        INVOKE  FIL4_OPEN,lpFilename,(fREAD OR 020h)
        cmp     ax, -1
        jz      FileError
        mov     bx,ax

;------ read the header into a buffer
        INVOKE  FIL4_READ,bx,Buffer,SELF_TEST_BUFFER_SIZE
        mov     cx,ax
        INVOKE  FIL4_CLOSE,bx
        cmp     cx,SELF_TEST_BUFFER_SIZE
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
                                        ; ------------------------------------
                                        ; Convert from Oem to Ansi
                                        ; ------------------------------------
IFDEF   WIN16
        push    ax

        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  OEMTOANSI, lpFilename, lpFilename
@@:
        pop     ax
ENDIF
        ret

FIL4_SELF_TEST ENDP
;----------------------------------------------------------------------------
END
