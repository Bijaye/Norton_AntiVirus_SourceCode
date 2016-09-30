;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/scn_me.asv   1.14   31 Jan 1996 21:49:26   MKEATIN  $
;;
;; Description: Defines SCN4_ME and SCN4_EXEC functions.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/scn_me.asv  $
;  
;     Rev 1.14   31 Jan 1996 21:49:26   MKEATIN
;  ADD DX Support - removed unnessary code
;  
;     Rev 1.13   04 Dec 1995 12:02:34   DCHI
;  Added initialization of count of external requests to zero.
;  Removed IS_ZIP check.
;  
;     Rev 1.12   29 Nov 1995 14:26:42   DCHI
;  1. Changed calls of FIL4_GET_START to FIL4_NEW_GET_START.
;  2. SCN4_EXEC now always returns zero.
;  3. Calls to SCN4_VERIFY_NAME that do not need to verify the first word
;     changed to calls to SCN4_GET_NAME.
;  
;     Rev 1.11   23 Jun 1995 13:13:14   SZIADEH
;  activated loading and execution of external DAT files.
;  
;     Rev 1.10   31 Jan 1995 16:21:18   SZIADEH
;  the upper word of eax is cleared in 32-bit mode when virus is found.
;  
;     Rev 1.9   16 Dec 1994 22:31:44   SZIADEH
;  restore  the code for exec_code.
;  
;     Rev 1.8   16 Dec 1994 18:43:50   SZIADEH
;  Ported from CERTLIB4 to CERTLIBN
;  
;     Rev 1.7   22 Aug 1994 18:38:10   DALLEE
;  Commented out SCN4_EXEC -- breaks .DX platform.
;  We're no longer using this update scheme (hopefully).
;  
;     Rev 1.6   17 Aug 1994 13:25:48   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.5   30 Jun 1994 17:13:22   CAREY
;  Merged changes in from CERTLIB4, QUAKE 2 version.
;  
;     Rev 1.4   26 May 1994 10:33:08   RSTANEV
;  Change of prototype in comments.
;
;     Rev 1.3   20 May 1994 08:40:04   RSTANEV
;  Fixed some typos.
;
;     Rev 1.2   17 May 1994 15:00:18   RSTANEV
;  Fixed international stuff.
;
;     Rev 1.1   05 May 1994 12:44:52   RSTANEV
;  Fixed some optimizations for 16/32 bit mode.
;
;     Rev 1.0   02 May 1994 18:26:20   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SCN4_ME
;
; Scan a file for a known virus, using all possible methods.
;
; WORD WINAPI SCN4_ME ( UINT Handle, LPSTR Database, LPSTR Buffer,
;                       UINT BufferSize, LPSTR Data, LPSTR Extra,
;                       LPSTR Callback, WORD wExecFlag );
;
; Input:
;       1)  Handle of program to scan (file should be seeked at position 0)
;       2)  Pointer to verify database (in platform's native character set)
;       3)  Buffer to work in
;       4)  Size of buffer
;       5)  Data to scan with
;       6)  Extra buffer to place message in
;       7)  Scan callback functions to add to scan ability
;       8)  If file was in EXECUTE list, this is set to TRUE, otherwise FALSE
; Output:
;       1)  No Virus = 0 or Virus ID=?
;       2)  Extra buffer filled
;       3)  -1 for error
;----------------------------------------------------------------------------
SCN4_ME  PROC  CODEDISTANCE  USES XBX XCX XDX SDS SES XSI XDI,
        fhandle:UINT,
        lpVerifyName:LPSTR,
        lpBuffer:LPSTR,
        uBuffer:UINT,
        lpData:LPSTR,
        lpExtra:LPSTR,
        lpCallback:LPSTR,
        wExecFile:WORD

        LOCAL   GetStart:STARTCODE
        LOCAL   Vid:WORD
IFNDEF SYM_NLM
        LOCAL   Magic[16]:BYTE          ; 16 is the length required by
                                        ;   -rad- by whom???
ENDIF

IF bSYM_NLM

; The NLM cannot open anything but files, so this code is redundant

ELSE

        mov     Magic[0],0              ; Note "unused"

  IF bSYM_WIN32

        ECHO Need to make sure that the handle is a file, not a device.

  ELSEIF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16
        mov     XBX,fHandle
        mov     ax,4400h
        CALLDOS
        jc      IsDevice
        test    dx,80h
        jz      IsNOTDevice
  IsDevice:
        jmp     J_OK
  IsNOTDevice:
  ENDIF
ENDIF


        INVOKE  FIL4_NEW_GET_START,fhandle,lpBuffer,ADDR GetStart

        LDS_SI  lpBuffer                ; point to string
        mov     word ptr [XSI+300h], 0  ; zero out # of external scan requests

        or      XAX,XAX
        jnz     J_0

        dec     XAX
        jmp     J_ERROR

J_0:
        cmp     GetStart.FileType, IS_WHALE
        jnz     J_W
        LES_DI  lpData
        mov     ax,XES[XDI].HEADER.wFixedIDs + FID_WHALE
        jmp     J_VNOV                  ; No need to verify first word

J_W:
        xor     XBX,XBX                 ; Start with CRC 0
J_CRCLOOP:
        INVOKE  SCN4_CRC,lpBuffer,uBuffer,lpData,bx
        or      ax,ax
        jz      J_1
        LDS_SI  lpBuffer
        mov     cx,[XSI]                ; First word from BUFFER, must verify
        jmp     J_VIRUS
J_1:

IFNDEF SYM_NLM
        cmp     Magic[0],0              ; was lpBuffer blown away?
        je      J_1A
        INVOKE  FIL4_SEEK,fhandle,0,fSEEK_BEG ; if so, refresh
        INVOKE  FIL4_NEW_GET_START,fhandle,lpBuffer,ADDR GetStart
        or      XAX,XAX
        jnz     J_1A                    ; theoretically, this always jmps
        dec     XAX
        jmp     J_ERROR
J_1A:
ENDIF

        mov     ax, word ptr GetStart.BufferOffset
        mov     bx, word ptr GetStart.BufferOffset+2

        INVOKE  SCN4_ALG,fhandle,lpBuffer,uBuffer,lpData, bx::ax
        or      ax,ax
        jz      J_2
        jmp     J_VNOV                  ; No need to verify first word

J_2:
        cmp     GetStart.FileType,IS_SYS
        je      J_OK

; since callbacks are the last detection applied to a file, if we have a
; NON executable file, we just go to J_OK and exit normally.  All other
; detection has been done at this point and we don't have a virus.

        cmp     wExecFile,0             ; if this is 0, don't do callbacks
        je      J_OK

IF OS_16
        mov     ax,WORD PTR lpCallback
        or      ax,WORD PTR lpCallback + 2
ELSE
        cmp     DWORD PTR lpCallBack,0
ENDIF
        jz      J_OK

        LES_DI  lpCallback
ScanMore:
IF OS_16
        mov     ax,WORD PTR XES[XDI]    ; next function pointer NULL?
        or      ax,WORD PTR XES[XDI + 2]
ELSE
        cmp     DWORD PTR XES[XDI],0
ENDIF
        jz      J_OK

ECHO Warning: Combine SCNPTR and SCNPTRTSR.

IFDEF SYM_TSR
        INVOKE  SCNPTRTSR PTR XES[XDI],lpBuffer,uBuffer,lpData,ADDR GetStart
ELSE
        INVOKE  SCNPTR PTR XES[XDI],lpBuffer,uBuffer,lpData,ADDR GetStart
ENDIF
        or      ax,ax
        jz      J_3
        jmp     J_VNOV                  ; No need to verify first word

J_3:

IFDEF SYM_TSR
        add     XDI, 2                  ; IP only
ELSE
        add     XDI, 4                  ; CS:IP
ENDIF
        jmp     ScanMore

;; Get virus name, do verify first word

J_VIRUS:
        cmp     ax,0
        jne     J_4
                                        ; id of zero is invalid - either
                                        ; a fixed id is missing or some
                                        ; other database error occured.
        mov     XAX,-1                  ; Generates internal DOS error.
        jmp     J_ERROR

J_4:
        mov     Vid,ax

IFDEF SYM_NLM
        INVOKE  SCN4_VERIFY_VIRUS,cx,Vid,lpData,lpExtra
ELSE

                                        ; Save 16 bytes
        push    XCX
        cld

        LDS_SI  lpBuffer
        SMOV    es,ss                   ; Setup LOCAL variables
        lea     XDI,Magic
        mov     XCX,8
        rep     movsw                   ; mov 16 bytes
        pop     XCX

;CJK: move the file opens and closes inside SCN4_VERIFY_NAME

        INVOKE  FIL4_OPEN,lpVerifyName,fREAD
        cmp     XAX,-1
        jz      J_Error
        rpush   XCX,XBX
        mov     XBX,XAX

        INVOKE  SCN4_VERIFY_NAME,XBX,cx,Vid,lpBuffer,lpData,lpExtra

        push    XAX
        INVOKE  FIL4_CLOSE,XBX
        rpop    XAX,XBX,XCX             ; bx has virus id, cx has verify or 0

ENDIF
        cmp     ax,0FADEh               ; Is this a NOT-A-VIRUS sig?
        je      J_OK

        or      ax,ax                   ; was it fail (test or read)
        jz      J_X                     ; not then leave w/vid

J_retEntry:
IF OS_16
        mov     ax,Vid                  ; MUST return entry number
        mov     dx,bx                   ; NOT virus ID from N30
ELSE
        ;mov     ax,bx                  ; virus control word not used
        ;shl     eax,10h                ; at this point

        xor     XAX, XAX                ;
        mov     ax,Vid
ENDIF
        jmp     J_ERROR

;; Get virus name, no verification of first word needed

J_VNOV:
        mov     Vid,ax

IFDEF SYM_NLM
        INVOKE  SCN4_GET_VIRUS,Vid,lpData,lpExtra
ELSE

                                        ; Save 16 bytes
        cld

        LDS_SI  lpBuffer
        SMOV    es,ss                   ; Setup LOCAL variables
        lea     XDI,Magic
        mov     XCX,8
        rep     movsw                   ; mov 16 bytes

        INVOKE  FIL4_OPEN,lpVerifyName,fREAD
        cmp     XAX,-1
        jz      J_Error
        push    XBX
        mov     XBX,XAX

        INVOKE  SCN4_GET_NAME,XBX,Vid,lpBuffer,lpData,lpExtra

        push    XAX
        INVOKE  FIL4_CLOSE,XBX
        pop     XAX                     ; ax has virus id
        pop     XBX

ENDIF
        cmp     ax,0FADEh               ; Is this a NOT-A-VIRUS sig?
        je      J_OK

        jmp     J_retEntry              ; Valid sig

J_X:
                                        ; NLM doesn't blow away its buffer
IFNDEF SYM_NLM
        ; Restore 16 bytes
	LES_DI  lpBuffer
        SMOV    ds,ss                   ; Setup LOCAL variables
        lea     XSI,Magic
        mov     XCX,8
        rep     movsw                   ; mov 16 bytes
ENDIF

        mov     bx,Vid                  ; Vid is 1-based - when I pass
                                        ; it back to CRC it is treated as
                                        ; an index which effectively
                                        ; increments it to the next CRC
        LES_DI  lpData
        sub     bx,XES[XDI].HEADER.uNumBOOs
        sub     bx,XES[XDI].HEADER.uNumALGs
        jmp     J_CRCLOOP

J_OK:
        xor     XAX,XAX

IF OS_16
        cwd
ENDIF

J_ERROR:

        ret

SCN4_ME  ENDP

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; SCN4_EXEC
;
; Scan a file with with scn_alg's EXEC function.
;
; DWORD WINAPI SCN4_EXEC ( UINT Handle, LPSTR Buffer,
;                          LPSTR CurrentBuffer, LPSTR CurrentSig );
;
; Input:
;       1)  Handle of program to scan (file should be seeked at position 0)
;       2)
;       3)
;       4)
; Output:
;       1)  LOWORD No Virus = 0 or Virus ID=?
;           HIWORD Virus Control Bits
;       2)  Extra buffer filled
;       3)  LOWORD -1 for error
;           HIWORD undefined
;----------------------------------------------------------------------------
;;
;; This code should be execluded for the DOS EXTENDER (bSYM_DX) platform.
;;

SCN4_EXEC PROC CODEDISTANCE,
        Handle:UINT,
        Buffer:LPSTR,
        CurrentBuffer:LPSTR,
        CurrentSig:LPSTR

        XOR  XAX, XAX
        ret

SCN4_EXEC ENDP

ExecPath  LABEL  BYTE
        DB      MAX_PATH_SIZE + 1 DUP (0)
;---------------------------------------------------------------------
UniqueSegmentEnd
END
