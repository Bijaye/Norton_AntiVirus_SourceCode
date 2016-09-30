;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/FISAT.ASv   1.2   10 May 1996 20:44:50   YVANG  $
;;
;; Description: Defines FIL4_IS_AT function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/FISAT.ASv  $
;  
;     Rev 1.2   10 May 1996 20:44:50   YVANG
;  DBCS enable.
;  
;     Rev 1.1   17 Aug 1994 13:26:36   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   22 Jun 1994 09:12:50   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_IS_AT
;
; Test if a file extension matches the AT_LIST.
;
; BOOL WINAPI FIL4_IS_AT ( LPSTR Filename, LPSTR AT_List )
;
; Input:
;       1)  Filename  FILENAME.EXT
;       2)  AT List   COM SYS EXE OV? D*
; Output:
;       1)  FALSE (0) Not in AT list
;       2)  TRUE (1) Is in AT list
;
; Notes:
;    This thing should be rewritten.
;
; Notes for DBCS enable:
;    The following is a description of how file extension three bytes
;       are tested against AT List character pattern with '?' mark(s) 
;       for DBCS enabled match.  (x0, x1, and x2 could have same ANSI value, 
;       so could c0, c1, and c2.)
;
;       AT List Pattern  File Ext chars  Conditions
;       ---------------  --------------  ------------------------------------
;    0) x0-x1-x2         c0-c1-c2        No need to test
;    1) x0-x1-?2         c0-c1-c2        if c1 != lead byte
;    2) x0-?1-x2         c0-c1-c2        if c0 != lead byte && c1 != lead byte
;    3) x0-?1-?2         c0-c1-c2        if c0 != lead byte 
;    4) ?0-x1-x2         c0-c1-c2        if c0 != lead byte
;    5) ?0-x1-?2         c0-c1-c2        if c0 != lead byte && c1 != lead byte
;    6) ?0-?1-x2         c0-c1-c2        if c1 != lead byte
;    7) ?0-?1-?2         c0-c1-c2        No need to test
;----------------------------------------------------------------------------

IF bSYM_TSR OR bSYM_DOS OR bSYM_DOS32 OR bSYM_WIN16 OR bSYM_WIN32 OR bSYM_OS2 OR bSYM_NLM OR bSYM_VXD

  FIL4_IS_AT  PROC  CODEDISTANCE  USES XCX SDS SES XSI XDI,
        Filename:LPSTR,
        AtList:LPSTR

        LOCAL   ?0Found:BOOL            ; '?' mark at first position
                                        ;    (= ?0 above) found
        LOCAL   ?1Found:BOOL            ; '?' mark at second position
                                        ;    (= ?1 above) found
        LOCAL   x0Match:BOOL            ; First byte position char 
                                        ;    (= x0 above) matched
        LOCAL   x1Match:BOOL            ; Second byte position char 
                                        ;    (= x1 above) matched

        LDS_SI  AtList
        LES_DI  Filename

        ;
        ; Get the last DOT in file - ..\DIR.30\FILENAME.TXT
        ;                                            --^
        xor     XAX,XAX
        INVOKE  STR4_LENGTH_TO_LAST,Filename,'.'
        or      XAX,XAX
        jz      NotHere
        add     XDI,XAX
        inc     XDI

  CompareAgain:
        mov     ?0Found,FALSE            
        mov     ?1Found,FALSE
        mov     x0Match,FALSE
        mov     x1Match,FALSE

        cmp     [XSI],BYTE PTR '*'
        je      IsHere
        cmp     [XSI],BYTE PTR '?'
        je      First?Found
        mov     al,XES[XDI]             ; Get first character
        cmp     [XSI],al                ; Test first character to match
        jne     NextATEntry
        mov     x0Match,TRUE            ; Set flag of first byte position 
                                        ;    char matched
        jmp     Test2

  First?Found:
        mov     ?0Found,TRUE            ; Set flag of first '?' mark found        
  Test2:
        mov     al,XES[XDI+1]           ; Get next character
        cmp     [XSI+1],BYTE PTR '*'    ; Must be a match
        je      IsHere
        cmp     [XSI+1],BYTE PTR '?'    ; Wildcard
        jne     NoWildcard2
                                        ; At this point, the first 2
                                        ;   chars file extension matched 
                                        ;   either ?0-?1 or x0-?1 AtList
                                        ;   pattern.
        mov     ?1Found, TRUE           ; Set flag of second '?' mark found
        cmp     ?0Found, TRUE           ; First '?' mark found?
        je      TestEndExt              ; Jump if yes
                                        ; At this point, the first
                                        ;   2 chars file extension has
                                        ;   x0-?1 AtList pattern.  So check if
                                        ;   the first char (c0) is lead byte.
  LeadByteCheckx0?1:
        INVOKE DBCS_DBCSIsLeadByte, XES[XDI]  ; 
        cmp     XAX, TRUE
        je      NextATEntry             ; Jump to check NextATEntry if it is.
                                        ;   c0 is lead byte since the first 
                                        ;   two chars (c0-c1) are a DBCS pair
                                        ;   and, for a DBCS pair match
                                        ;   both byte positions in AtList
                                        ;   pattern should have '?' marks.
  TestEndExt:
    
        or      al,al                   ; DALLEE's Kludge: if this is the end
        jnz     Test3                   ; of the extension, put a NULL in the
        mov     XES[XDI+2], BYTE PTR 0  ; 3rd position so Test3 works.
        jmp     Test3

  NoWildcard2:
        cmp     [XSI+1],al              ; Test the second character
        jne     EndExtension2
        cmp     ?0Found,FALSE           ; Second char matched.
                                        ;   At this point, the first 2 chars 
                                        ;   file extention (c0-c1) matched 
                                        ;   either ?0-x1 or x0-x1 AtList 
                                        ;   pattern. So check if the pattern 
                                        ;   is ?0-x1.
        je      TestNull2               ; Jump if not.
                                        ; The first two chars file
                                        ;   extenstion (c0-c1) has ?0-x1 
                                        ;   AtList pattern.
                                        ;   Check if first char (c0) is lead byte
  LeadByteCheck?0x1:
        INVOKE  DBCS_DBCSIsLeadByte, XES[XDI] ; 
        cmp     XAX, TRUE               ; 
        je      NextATEntry             ; Jump to check NextATEntry if it is.
                                        ;   c0 is lead byte since the first 
                                        ;   two chars (c0-c1) are a DBCS pair
                                        ;   and, for a DBCS pair match
                                        ;   both byte positions in AtList
                                        ;   pattern should have '?' marks.
  TestNull2:
        or      al,al                   ; If we're at the end of both the
        jz      IsHere                  ; extension and the list of extensions,
        mov     x1Match, TRUE           ; then set flag of second byte matched
        jmp     Test3                   ; 

  EndExtension2:
        or      al,al
        jnz     NextATEntry
        cmp     [XSI+1],BYTE PTR 20H    ; Is space
        je      IsHere

        ; DALLEE:  If we're here, al == 0, and we've already done 'cmp [XSI+1],al'
        ;        cmp     [XSI+1],byte ptr 0      ; Is null
        ;                je      IsHere

        jmp     NextATEntry

  Test3:
        cmp     [XSI+2],BYTE PTR '*'    ; Must be a match
        je      IsHere
        cmp     [XSI+2],BYTE PTR '?'    ; Wildcard
        jne     TestThirdChar
                                        ; At this point, the file 
                                        ;   extention matched either case
                                        ;   1) x0-x1-?2, 3) x0-?1-?2, 
                                        ;   5) ?0-x1-?2,
                                        ;   or 7) ?0-?1-?2 AtList pattern.
                                        ;   No need to test the 7) ?0-?1-?2 case.
                                        ;   The case 3) x0-?1-?2 will match
                                        ;   as long as first char (c0) is not
                                        ;   lead byte which is already 
                                        ;   checked at 'LeadByteCheckx0?1'.
                                        ; 
        cmp     x1Match, TRUE           ; Test 1) x0-x1-?2 and 5) ?0-x1-?2 
        jne     IsHere

  LeadByteCheckx1?2:                    ; Test only x1-?2 part since ?0-x1
                                        ;   part of case 5) ?0-x1-x2 is
                                        ;   already checked at 
                                        ;   LeadByteCheck?0x1.
        INVOKE  DBCS_DBCSIsLeadByte, XES[XDI+1]
        cmp     XAX, TRUE
        jne     IsHere
        jmp     NextATEntry

  TestThirdChar:
        mov     al,XES[XDI+2]           ; Get next character
        cmp     [XSI+2],al              ; Test the third character
        jne     TestNull3
                                        ; At this point, the file 
                                        ;   extention matched either
                                        ;   0) x0-x1-x2, 2) x0-?1-x2, 
                                        ;   4) ?0-x1-x2, or 6) ?0-?1-x2 
                                        ;   AtList pattern.
                                        ;   No need to test 0) the x0-x1-x2 case.
                                        ;   The case 4) ?0-x1-x2 will match
                                        ;   as long as first char (c0) is not
                                        ;   a lead byte which is already
                                        ;   checked at 'LeadByteCheck?0x1'.                                         
                                        ;                                        
        cmp     ?1Found, TRUE           ; Test 2) x0-?1-x2 and 6) ?0-?1-x2 
        jne     IsHere
        
  LeadByteCheck?1x2:                    ; Test only ?1-x2 part since x0-?1
                                        ;   part of case 2) x0-?1-x2 is
                                        ;   already checked at 
                                        ;   LeadByteCheckx0?1.
        INVOKE  DBCS_DBCSIsLeadByte, XES[XDI+1]
        cmp     XAX, TRUE
        jne     IsHere
        jmp     NextATEntry

  TestNull3:
        or      al,al
        jnz     NextATEntry
        cmp     [XSI+2],BYTE PTR 20h    ; Is space
        je      IsHere

        ; DALLEE:  If we're here, al == 0, and we've already done 'cmp [XSI+2],al'
        ;        cmp     [XSI+2],byte ptr 0      ; Is null
        ;        je      IsHere

        jmp     NextATEntry

  NextATEntry:
        cmp     [XSI],BYTE PTR 0        ; End of AT List
        je      NotHerE
        cmp     [XSI],BYTE PTR ' '      ; Space, must be next entry
        je      LookMore
        inc     XSI
        jmp     NextATEntry

  LookMore:
        inc     XSI
        jmp     CompareAgain

  IsHere:
        mov     XAX,TRUE
        jmp     SHORT J_Done

  NotHere:

        xor     XAX,XAX

  J_Done:

        ret

  FIL4_IS_AT  ENDP

ELSE
  ECHO Warning: Target platform does not support FIL4_IS_AT.
ENDIF

UniqueSegmentEnd
END
