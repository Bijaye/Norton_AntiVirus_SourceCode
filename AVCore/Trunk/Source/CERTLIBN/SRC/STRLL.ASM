;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/strll.asv   1.4   25 Jun 1996 18:38:24   MZAREMB  $
;;
;; Description: Defines STR4_LENGTH_TO_LAST function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/strll.asv  $
;  
;     Rev 1.4   25 Jun 1996 18:38:24   MZAREMB
;  Reload BL with byChar as it gets smashed in the DBCS call.
;  
;     Rev 1.3   25 Jun 1996 10:47:48   MZAREMB
;  Added "ret" to end of function.
;  
;     Rev 1.2   10 May 1996 20:44:54   YVANG
;  DBCS enable.
;  
;     Rev 1.1   17 Aug 1994 13:26:32   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   22 Jun 1994 09:44:10   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; STR4_LENGTH_TO_LAST
;
; Return the length of a string searching to the last occurance of
; a character.
;
; UINT WINAPI STR4_LENGTH_TO_LAST ( LPSTR lpString, BYTE Char );
;
; Input:
;       1)  The string to look for the character
;       2)  The character to look for
; Output:
;       1)  Length of the string, excluding character.
;       2)  Character not found 0
;
;----------------------------------------------------------------------------
STR4_LENGTH_TO_LAST  PROC  CODEDISTANCE  USES XBX XCX XDX SDS XSI,
        lpString:LPSTR,
        byChar:BYTE

                                        ; Step through string, remember in XCX
                                        ; the last instance of byChar found
        LDS_SI  lpString

        xor     XCX, XCX
        xor     XDX, XDX

J_StartSearch:
        mov     bl, byChar
        cmp     byte ptr [XSI], 0       ; Check for end of string.
        je      J_Done


        cmp     [XSI], bl
        jne     J_NotThisChar

        mov     XCX, XDX                ; Found an occurance of character.

J_NotThisChar:
        INVOKE  DBCS_DBCSIsLeadByte, byte ptr [XSI]
        cmp     XAX, FALSE
        je      J_NotLeadByte

        inc     XSI
        inc     XDX
        cmp     byte ptr [XSI], 0       ; Guard against badly formed strings.
        je      J_Done

J_NotLeadByte:
        inc     XSI
        inc     XDX
        jmp     J_StartSearch

J_Done:
        mov     XAX, XCX
        ret

STR4_LENGTH_TO_LAST  ENDP

UniqueSegmentEnd
END
