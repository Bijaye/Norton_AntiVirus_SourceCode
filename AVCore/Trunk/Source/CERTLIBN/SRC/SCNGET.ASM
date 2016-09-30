;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\scnget.asv   1.2   17 Aug 1994 13:26:18   CAREY  $
;;
;; Description: Defines SCN4_GET_ENTRY function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\scnget.asv  $
;  
;     Rev 1.2   17 Aug 1994 13:26:18   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   27 May 1994 09:11:44   RSTANEV
;  Minor changes.
;
;     Rev 1.0   02 May 1994 18:22:04   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SCN4_GET_ENTRY
;
; Retrieve an entry from the virus database.
;
; BOOL WINAPI SCN4_GET_ENTRY ( UINT Handle, WORD Entry, LPSTR Buffer );
;
; Input:
;       1)  Handle to opened database
;       2)  Entry number to lookup
;       3)  Buffer to store entry at (SIZE OF N30 structure)
; Output:
;       1)  TRUE (1) Passed
;       2)  FALSE (0) Failed
;
;----------------------------------------------------------------------------
SCN4_GET_ENTRY  PROC  CODEDISTANCE  USES XCX XDX,
        hHandle:UINT,
        uEntry:WORD,
        lpBuffer:LPSTR

        mov     ax,uEntry
        dec     ax
        xor     dx,dx
        mov     cx,sN30
        mul     cx
        add     ax,uSizeVirscanHeader           ; added to compensate
        adc     dx,0                            ; for virscan header

        INVOKE  FIL4_SEEK,hHandle,dx::ax,fSEEK_BEG
        INVOKE	FIL4_READ,hHandle,lpBuffer,sN30

        cmp     XAX,sN30
        jne     J_Error

        mov     XAX,TRUE
        jmp     SHORT J_Done
J_Error:
        xor     XAX,XAX
J_Done:
        ret

SCN4_GET_ENTRY  ENDP

UniqueSegmentEnd
END
