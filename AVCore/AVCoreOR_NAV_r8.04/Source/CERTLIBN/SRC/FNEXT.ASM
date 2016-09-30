;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/FNEXT.ASv   1.2   08 Mar 1996 14:56:50   DALLEE  $
;;
;; Description: Defines FIL4_FIND_NEXT function.
;;              Defines ex_FindNextFile function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/FNEXT.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:56:50   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:25:56   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   19 May 1994 16:40:12   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_FIND_NEXT
;
; Returns information in the current DTA of a file find next.
;
; UINT WINAPI FIL4_FIND_NEXT ( void );
;
; Input:
;       1)  DTA from previous FIND FIRST.
; Output:
;       1)  Found (0)
;       2)  FAILED (?) DOS File Find Next errors
;
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  FIL4_FIND_NEXT  PROC  CODEDISTANCE

        mov     ah,4Fh
        CALLDOS
        jc      J_Done
        xor     XAX,XAX
J_Done:
        ret

  FIL4_FIND_NEXT ENDP

ELSE
        ECHO Warning: The target platform does not support FIL4_FIND_NEXT.
ENDIF

;----------------------------------------------------------------------------
; ex_FindNextFile
;
; Uses LFN DOS support to search next file handle.
;
; UINT WINAPI ex_FindNextFile ( HANDLE32 hFFHandle,
;                               WIN32_FIND_DATA* pFindData );
;
; Input:
;       1)  HANDLE32 handle returned by ex_FindFirstFile
;       2)  Structure for result.
; Output:
;       1)  PASSED (0)
;       2)  FAILED (?) (DOS Error's)
;
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  ex_FindNextFile  PROC  CODEDISTANCE  USES XBX XDI SES,
        hHandle:HANDLE32,
        prFindData:LPBYTE

        mov     ax,714Fh
        mov     bx,hHandle
        LES_DI  prFindData
        CALLDOS
        jc      FunctionEnd
        xor     XAX,XAX
  FunctionEnd:
        ret

  ex_FindNextFile  ENDP

ELSE
        ECHO Warning: Target platform does not support ex_FindNextFile.
ENDIF

UniqueSegmentEnd
END
