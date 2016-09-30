include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_IS_AT                           12/23/92                        CAG
;
; $Header:   S:\certlib4\vcs\fisat.asv   1.7   29 Nov 1994 13:11:00   DALLEE  $
;
;       Test if a file extension matches the AT_LIST.
;
; UINT WINAPI FIL4_IS_AT(LPSTR Filename, LPSTR AT_List)
;
; Input:
;       1)  Filename  FILENAME.EXT
;       2)  AT List   COM SYS EXE OV? D*
; Output:
;       1)  FALSE (0) Not in AT list
;       2)  TRUE (1) Is in AT list
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\fisat.asv  $
;  
;     Rev 1.7   29 Nov 1994 13:11:00   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.5.1.0   19 Oct 1993 07:15:42   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.5   12 Oct 1993 16:25:04   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.4.2.0   03 Sep 1993 19:26:50   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.4   28 May 1993 14:26:12   CGUC
;  Fixed problem with name like ..\DIR.30\FILENAME.EXE
;  Now skips to last period.
;  
;     Rev 1.3   23 May 1993 22:26:34   BRAD
;  Put initial '.' in a loop.  Also, insure that CLD
;  is set, before string op.
;  
;     Rev 1.2   20 May 1993 15:27:10   CGUC
;  Fixed problem with files starting with . or .. 
;  
;     Rev 1.1   18 May 1993 17:18:58   DALLEE
;  Added 2 kludges to the Test2: section of code:
;    1. If the 2nd letter in the program extension is a wildcard, and the
;       extension we're checking has only 2 characters, make sure we only
;       match if the 3rd letter is a wildcard or program extension has only
;       2 characters, also.
;    2. If the 2nd letter matches and we are at the end of both strings, 
;       we have a match.
;  Also, reload AL with the first letter when we go to the next comparison.
;-----------------------------------------------------------------------------
FIL4_IS_AT PROC USES XCX SDS SES XSI XDI,
        Filename:LPSTR,
        AtList:LPSTR

        LDS_SI  AtList
        LES_DI  Filename
        
; Get the last DOT in file - ..\DIR.30\FILENAME.TXT                                
;                                            --^
        xor     XAX,XAX
        INVOKE  STR4_LENGTH_TO_LAST,Filename,'.'
        or      XAX,XAX
        jz      NotHere
        add     XDI,XAX
        inc     XDI

CompareAgain:
        cmp     [XSI],byte ptr '*'
        je      IsHere
        cmp     [XSI],byte ptr '?'
        je      Test2
        mov     al,XES[XDI]             ; Get first character
        cmp     [XSI],al                ; Test first character to match
        jne     NextATEntry

Test2:
        mov     al,XES[XDI+1]           ; Get next character
        cmp     [XSI+1],byte ptr '*'    ; Must be a match
        je      IsHere
        cmp     [XSI+1],byte ptr '?'    ; Wildcard
        jne     NoWildcard2
        or      al,al                   ; DALLEE's Kludge: if this is the end
        jnz     Test3                   ; of the extension, put a NULL in the
        mov     XES[XDI+2], byte ptr 0  ; 3rd position so Test3 works.
        jmp     Test3

NoWildcard2:
        cmp     [XSI+1],al              ; Test the second character
        jne     EndExtension2
        or      al,al                   ; If we're at the end of both the
        jz      IsHere                  ; extension and the list of extensions,
        jmp     Test3                   ; then we've matched.

EndExtension2:
        or      al,al
        jnz     NextATEntry
        cmp     [XSI+1],byte ptr 20h    ; Is space
        je      IsHere

; DALLEE:  If we're here, al == 0, and we've already done 'cmp [XSI+1],al'
;        cmp     [XSI+1],byte ptr 0      ; Is null
;        je      IsHere

        jmp     NextATEntry

Test3:
        cmp     [XSI+2],byte ptr '*'    ; Must be a match
        je      IsHere
        cmp     [XSI+2],byte ptr '?'    ; Wildcard
        je      IsHere

        mov     al,XES[XDI+2]           ; Get next character
        cmp     [XSI+2],al              ; Test the third character
        je      IsHere
        or      al,al
        jnz     NextATEntry
        cmp     [XSI+2],byte ptr 20h    ; Is space
        je      IsHere

; DALLEE:  If we're here, al == 0, and we've already done 'cmp [XSI+2],al'
;        cmp     [XSI+2],byte ptr 0      ; Is null
;        je      IsHere

        jmp     NextATEntry

NextATEntry:
        cmp     [XSI],byte ptr 0        ; End of AT List
        je      NotHere
        cmp     [XSI],byte ptr ' '      ; Space, must be next entry
        je      LookMore
        inc     XSI
        jmp     NextATEntry

LookMore:
        inc     XSI
        jmp     CompareAgain

IsHere:
        mov     ax,TRUE
        jmp     J_Done

NotHere:
        mov     ax,FALSE

J_Done:
        ret

FIL4_IS_AT ENDP
;-----------------------------------------------------------------------------
END
