include CTSN.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/fmedia.asv   1.3   08 Mar 1996 14:32:46   DALLEE  $
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/fmedia.asv  $
;  
;     Rev 1.3   08 Mar 1996 14:32:46   DALLEE
;  Only compile for TSR, DOS and W16.
;  Code won't work for NLM or W32.
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; FIL4_MEDIA                           1/26/93                         CAG
;
;       Determine what media a file is residing on.
;
; WORD WINAPI FIL4_MEDIA(LPSTR Filename);
;
; Input:
;       1)  Pointer to filename
; Output:
;       1)  Removable Media file (0)
;       2)  Local NON Removable file (1)
;       3)  Network file (2)
;       4)  ERROR (-1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
IF bSYM_TSR OR bSYM_DOS OR bSYM_WIN16

  FIL4_MEDIA PROC USES XBX XDX SDS XSI,
        lpFilename:LPSTR

        LDS_SI  lpFilename

        mov     bl,[XSI]
        and     bl,5Fh
        sub     bl,'A'
        inc     bl                      ; Drive in BL

  ; First determine if we are a network drive
        mov     ax,4409h
  IFDEF WIN16
        call    DOS3CALL
  ELSE
        int     21h
  ENDIF
        jc      J_ERROR
        test    dx,1000h
        jz      IsLocal
        mov     ax,4408h
  IFDEF WIN16
        call    DOS3CALL
  ELSE
        int     21h
  ENDIF
        jnc     IsLocal
  IsNetwork:
        mov     ax,2                    ; Is a network drive
        jmp     J_Done

  ; Now determine if this LOCAL media is REMOVABLE or NOT
  IsLocal:
        mov     ax,4408h
  IFDEF WIN16
        call    DOS3CALL
  ELSE
        int     21h                     ; AX = 0 Removable
  ENDIF
        jnc     J_Done                  ; AX = 1 Fixed

  J_Error:
        mov     ax,-1
  J_Done:
        ret

  FIL4_MEDIA ENDP


ELSE
        ECHO Warning: Target platform does not support FIL4_MEDIA
ENDIF

;-----------------------------------------------------------------------------
UniqueSegmentEnd
END

