;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DBLQUICK.ASv   1.4   08 Mar 1996 14:56:32   DALLEE  $
;;
;; Description: Defines DBC4_LOAD_QUICK function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DBLQUICK.ASv  $
;  
;     Rev 1.4   08 Mar 1996 14:56:32   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.3   05 Apr 1995 13:48:10   DALLEE
;  Fixed bug for 32 bit platforms.
;  Calling INVOKE with a WORD in place of a DWORD parameter assembles to
;   PUSH dword ptr 0, PUSH word ptr WORD.  A total of 6 bytes, not the
;   expected 4.
;  This is bad.
;  
;     Rev 1.2   17 Aug 1994 13:25:36   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   31 May 1994 13:28:44   RSTANEV
;  Fixed error and added warning.
;
;     Rev 1.0   30 May 1994 11:23:02   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_LOAD_QUICK
;
; Load the quick lookup data for fast access from memory.
;
; BOOL WINAPI DBC4_LOAD_QUICK ( DBC4_HEADER FAR *Info, WORD FAR *Buffer );
;
; Input:
;       1)  Header of opened DBC file
;       2)  FAR Buffer (size of quick data from header), no checking done
; Output:
;       1)  FALSE (0) Failed to load
;       2)  TRUE (1)  Loaded success
;----------------------------------------------------------------------------
DBC4_LOAD_QUICK  PROC  CODEDISTANCE  USES XCX SDS XSI,
        lpInfo:LPBYTE,
        lpBuffer:LPBYTE

        LDS_SI  lpInfo

        INVOKE  Seek_to_quick_entry,lpInfo,1

        ECHO Warning: DBC4_LOAD_QUICK uses only LOWORD of DBC4_HEADER.IndexSize
IF OS_16
        mov     cx,WORD PTR [XSI].DBC4_HEADER.IndexSize
ELSE
        mov     ecx,[XSI].DBC4_HEADER.IndexSize
ENDIF
        shl     XCX,1                    ; Multiply by two
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.FileHandle,lpBuffer,XCX
        cmp     XAX,-1
        je      J_Error
        cmp     XAX,XCX
        jne     J_Error
        mov     XAX,TRUE
        jmp     SHORT J_Done
J_Error:
        xor     XAX,XAX
J_Done:
        ret

DBC4_LOAD_QUICK  ENDP

UniqueSegmentEnd
END
