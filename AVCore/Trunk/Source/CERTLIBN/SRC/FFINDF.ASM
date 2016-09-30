;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/FFINDF.ASv   1.4   08 Mar 1996 14:56:50   DALLEE  $
;;
;; Description: Defines FIL4_FIND_FIRST function.
;;              Defines ex_FindFirstFile function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/FFINDF.ASv  $
;  
;     Rev 1.4   08 Mar 1996 14:56:50   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.3   20 Dec 1994 14:28:20   JMILLARD
;  
;  add SYM_NLM
;  
;     Rev 1.2   17 Aug 1994 13:25:54   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   19 May 1994 16:54:16   RSTANEV
;  Fixed a typo.
;
;     Rev 1.0   19 May 1994 16:35:16   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_FIND_FIRST
;
; Returns information in the current DTA of a file find first.
;
; UINT WINAPI FIL4_FIND_FIRST ( LPSTR Filename, WORD Attributes );
;
; Input:
;       1)  Filename to search for.
;       2)  Attributes to look for
;                    HIDDEN (02h)
;                    SYSTEM (04h)
;                    LABEL  (08h)
;                    DIR    (10h)
; Output:
;       1)  PASSED (0)
;       2)  FAILED (?) (DOS Error's)
;
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  FIL4_FIND_FIRST  PROC  CODEDISTANCE  USES XCX XDX SDS,
        Filename:LPSTR,
        Attribute:WORD

        mov     ah,4Eh
        mov     cx,Attribute
        LDS_DX  Filename
        CALLDOS
        jc      J_Done
        xor     XAX,XAX
  J_Done:
        ret

  FIL4_FIND_FIRST  ENDP

ELSEIF bSYM_NLM

  FIL4_FIND_FIRST  PROC  CODEDISTANCE  USES XCX XDX SDS,
        Filename:LPSTR,
        Attribute:WORD

        xor     XAX,XAX
  J_Done:
        ret

  FIL4_FIND_FIRST  ENDP

ELSE
        ECHO Warning: Target platform does not support FIL4_FIND_FIRST.
ENDIF

;----------------------------------------------------------------------------
; ex_FindFirstFile
;
; Uses LFN DOS support to open a FindFile handle.
;
; HANDLE32 WINAPI ex_FindFirstFile ( LPSTR pcFileSpec,
;                                    WIN32_FIND_DATA* pFindData,
;                                    WORD Attributes );
;
; Input:
;       1)  Filename to search for.
;       2)  Structure to fill.
;       3)  FILE_ATTRIBUTE_????????
; Output:
;       1)  PASSED (0)
;       2)  FAILED (?) (DOS Error's)
;
;----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  ex_FindFirstFile  PROC  CODEDISTANCE  USES XCX XDX XDI SDS SES,
        pcFileSpec:LPSTR,
        prFindData:LPBYTE,
        Attributes:WORD

        mov     ax,714Eh
        mov     cx,Attributes
        LDS_DX  pcFileSpec
        LES_DI  prFindData
        CALLDOS
        jc      FunctionEnd
        xor     XAX,XAX
  FunctionEnd:
        ret

  ex_FindFirstFile  ENDP

ELSE
        ECHO Warning: Target platform does not support ex_FindFirstFile.
ENDIF

UniqueSegmentEnd
END
