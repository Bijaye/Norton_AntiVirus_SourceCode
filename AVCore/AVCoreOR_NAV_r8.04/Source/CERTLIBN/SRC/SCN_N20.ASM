;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\scn_n20.asv   1.3   29 Nov 1995 14:24:50   DCHI  $
;;
;; Description: Defines SCN4_VERIFY_NAME function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\scn_n20.asv  $
;  
;     Rev 1.3   29 Nov 1995 14:24:50   DCHI
;  SCN4_VERIFY_NAME always checks the first word now.
;  SCN4_GET_NAME added to only get the virus information and name.
;  
;     Rev 1.2   17 Aug 1994 13:26:30   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   27 May 1994 09:15:34   RSTANEV
;  Minor changes.
;
;     Rev 1.0   02 May 1994 18:25:30   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SCN4_VERIFY_NAME
;
; Open the verification database to verify a file, and retrieve the
; virus name.
;
; DWORD WINAPI SCN4_VERIFY_NAME ( UINT Handle, WORD First, WORD VID,
;                                 LPSTR Buffer, LPSTR Data, LPSTR Extra );
;
; Input:
;       1)  First word of string
;       2)  VID = Virus ID.
;       3)  Buffer to work with.
;       4)  Size of buffer
;       5)  Data to use
;       6)  Where to place extra message
; Output:
;       1)  LOWORD No Virus = 0 or Virus ID=?
;           HIWORD Virus Control Bits
;       2)  Extra message with name of virus
;----------------------------------------------------------------------------
SCN4_VERIFY_NAME  PROC  CODEDISTANCE  USES XBX XCX SDS SES XSI XDI,
        hHandle:UINT,
        firstword:WORD,
        vid:WORD,
        lpBuffer:LPSTR,
        lpData:LPSTR,
        lpExtra:LPSTR

        INVOKE  SCN4_GET_ENTRY,hHandle,vid,lpBuffer
IF OS_16
        xor     XDX,XDX                 ; high word of possible error
ENDIF
        or      XAX,XAX
        jz      J_Error

        LDS_SI  lpBuffer
        mov     ax,WORD PTR [XSI].N30.lpSig
        dec     ah                      ; *CAREY -> decrypt
        dec     al                      ; *CAREY -> decrypt
        cmp     ax,firstword
        je      J_0
        xor     XAX,XAX                 ; XDX is already 0 for OS_16
        jmp     J_Error
J_0:

        mov     dx,WORD PTR [XSI].N30.wControl
        mov     bx,WORD PTR [XSI].N30.wVirID

IF OS_16
        cmp     WORD PTR lpExtra + 2,0
ELSE
        cmp     lpExtra, 0
ENDIF
        je      J_Done                  ; don't copy if no buffer

        LES_DI  lpExtra
        lea     XSI,[XSI].N30.lpVName

        mov     XCX,SIZE_VIRUSNAME
        cld
        ECHO Warning: This function destroys AL upon return if it copied virus name.
J_Loop:
        cmp     BYTE PTR [XSI],0
        je      J_1
        cmp     WORD PTR [XSI],'  '     ; Look for first double space
        je      J_1
        lodsb			        ; move each
        stosb			        ; byte
        loop    J_Loop
J_1:
        xor     al,al                   ; null terminate name
        stosb

J_Done:
        ECHO Warning: Something is wrong with the last operation: xchg bx,ax
        xchg    bx,ax

J_Error:
        ret

SCN4_VERIFY_NAME  ENDP

;----------------------------------------------------------------------------
; SCN4_GET_NAME
;
; Open the verification database to verify a file, and retrieve the
; virus name.
;
; DWORD WINAPI SCN4_GET_NAME ( UINT Handle, WORD VID,
;                              LPSTR Buffer, LPSTR Data, LPSTR Extra );
;
; Input:
;       1)  File handle
;       2)  VID = Virus ID.
;       3)  Buffer to work with.
;       4)  Data to use
;       5)  Where to place extra message
; Output:
;       1)  LOWORD No Virus = 0 or Virus ID=?
;           HIWORD Virus Control Bits
;       2)  Extra message with name of virus
;----------------------------------------------------------------------------
SCN4_GET_NAME  PROC  CODEDISTANCE  USES XBX XCX SDS SES XSI XDI,
        hHandle:UINT,
        vid:WORD,
        lpBuffer:LPSTR,
        lpData:LPSTR,
        lpExtra:LPSTR

        INVOKE  SCN4_GET_ENTRY,hHandle,vid,lpBuffer
IF OS_16
        xor     XDX,XDX                 ; high word of possible error
ENDIF
        or      XAX,XAX
        jz      JN_Error

        LDS_SI  lpBuffer
JN_0:

        mov     dx,WORD PTR [XSI].N30.wControl
        mov     bx,WORD PTR [XSI].N30.wVirID

IF OS_16
        cmp     WORD PTR lpExtra + 2,0
ELSE
        cmp     lpExtra, 0
ENDIF
        je      JN_Done                 ; don't copy if no buffer

        LES_DI  lpExtra
        lea     XSI,[XSI].N30.lpVName

        mov     XCX,SIZE_VIRUSNAME
        cld
        ECHO Warning: This function destroys AL upon return if it copied virus name.
JN_Loop:
        cmp     BYTE PTR [XSI],0
        je      JN_1
        cmp     WORD PTR [XSI],'  '     ; Look for first double space
        je      JN_1
        lodsb			        ; move each
        stosb			        ; byte
        loop    JN_Loop
JN_1:
        xor     al,al                   ; null terminate name
        stosb

JN_Done:
        ECHO Warning: Something is wrong with the last operation: xchg bx,ax
        xchg    bx,ax

JN_Error:
        ret

SCN4_GET_NAME  ENDP

UniqueSegmentEnd
END
