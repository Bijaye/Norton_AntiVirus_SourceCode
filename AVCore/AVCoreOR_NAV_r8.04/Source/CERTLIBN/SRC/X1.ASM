;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/X1.ASv   1.2   10 May 1996 20:45:04   YVANG  $
;;
;; Description: Defines EXCLUDECHECK function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/X1.ASv  $
;  
;     Rev 1.2   10 May 1996 20:45:04   YVANG
;  DBCS enable.
;  
;     Rev 1.1   17 Aug 1994 13:26:32   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   24 Jun 1994 14:30:24   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; ExcludeCheck
;
; Check exclude element if item is excluded.
;
; BOOL WINAPI ExcludeCheck ( FAR *EXCLUDE, LPSTR Filename, WORD Bits );
;
; Input:
;       1)  Exclude structure to compare Filename/Bits to
;       2)  Filename, must include full path
;       3)  Bits of what type of exclude are we looking for.
; Output:
;       1)  TRUE - Is in exclude element.
;       2)  FALSE - Not in exclude element.
;
; Note:
;       1)  CASE MUST be UPPER, in both STRUCTURE and FILENAME
;
; Conditions:
;       EXCLUDE         TYPE            Filename
;       C:              0               C:\DIRECTORY\FILENAME   TRUE
;       C:              0               C:\FILENAME             TRUE
;       C:\             0               C:\DIRECTORY\FILENAME   FALSE
;       C:\             0               C:\FILENAME             TRUE
;       C:\             1               C:\DIRECTORY\FILENAME   TRUE
;       C:\DIR          0               C:\DIR\FILENAME         TRUE
;       C:\DIR          0               C:\DIR\DIR\FILENAME     FALSE
;       C:\DIR          1               C:\DIR\DIR\FILENAME     TRUE
;       C:\FILE         0               C:\FILE                 TRUE
;       C:\DIR\FILE     0               C:\DIR\FILE             TRUE
;       FILE            0               C:\DIR\FILE             TRUE
;
;       \DIR\DIR        0               C:\DIR\DIR\FILE         TRUE
;       \DIR            1               C:\DIR\DIR\FILE         TRUE
;
; Whereas:
;       FILE    any DOS valid wildcard
;               ABC*.EXE
;               ABC?.EXE
;               AB?C.EXE
;               AB*.*
;               *AB.EXE         Invalid
;
;      2) For DBCS case, it is assumed that a user will specify '?' marks
;         properly for wildcard, (i.e., two consecutive '?' marks are 
;         replaced with one DBCS char.  For example, '?A' means two separate
;         char bytes, not lead byte wildcard followed by 'A'.)
;
;----------------------------------------------------------------------------

bitSUBDIRS      EQU     01h
bitWILDCARDS    EQU     02h

MatchOneSection  PROTO  CODEDISTANCE
FindFileOffset   PROTO  CODEDISTANCE,:LPSTR

ExcludeCheck PROC USES XCX SDS SES XSI XDI,
        lpExclude:LPSTR,
        lpFilename:LPSTR,
        uBits:WORD

        LOCAL   uType:UINT              ; Type of exclusion. Subdirs? Wildcards?
        LOCAL   offExclFile:UINT        ; Offset of filename portion of Exclude
        LOCAL   offFileFile:UINT        ; Offset of filename portion of Filename


        LDS_SI  lpExclude               ; Get exclude element
        LES_DI  lpFilename              ; Get filename to check

        mov     uType,0
        cmp     [XSI].EXCLUDE.bSubDirectory,FALSE
        je      @F
        or      uType,bitSUBDIRS
@@:
        lea     XAX,[XSI + EXCLUDE.szText]

        INVOKE  FindFileOffset,vSDS XAX
        jnc     @F
        jmp     J_NoMatch
@@:
        mov     offExclFile,XAX
        or      uType,XDX

        INVOKE  FindFileOffset,lpFilename
        jnc     @F
        jmp     J_NoMatch
@@:
        mov     offFileFile,XAX
        mov     ax,uBits                ; Determine what bits are to be tested
        and     ax,[XSI].EXCLUDE.wBits
        jz      J_NoMatch               ; Nothing in this entry matches
        cmp     ax,uBits
        jne     J_NoMatch


        cld                             ; Prepare for string compares

IF EXCLUDE.szText
        add     XSI,EXCLUDE.szText
ENDIF

CheckForDrive:                          ; Is there a drive in the exclusion?
        cmp     [XSI+1], BYTE PTR ':'
        je      J_DriveCompare
        cmp     XES[XDI+1], BYTE PTR ':'
        jne     J_DirCompare
        inc     XDI                     ; Advance past '<drive>:' in filename
        inc     XDI
        jmp     J_DirCompare

J_DriveCompare:
        cmpsw                           ; Compare '<drive>:'
        je      J_DirCompare
        jne     J_NoMatch

                                        ; Compare directory versus directory
                                        ; portion of exclude item.  This must
                                        ; match exactly.
J_DirCompare:
        mov     XAX,offExclFile

IF EXCLUDE.szText
        sub     XAX,EXCLUDE.szText
ENDIF

J_DirCompareLoop:
        cmp     XSI,XAX                 ; Did we reach the end of the dir spec?
        je      J_LastSectionCompare

        cmpsb
        je      J_DirCompareLoop
        jmp     J_NoMatch

                                        ; Compare the last section of the
                                        ; exclusion versus the next section of
                                        ; the filename.
                                        ; We have four cases to check:
                                        ;  0) No Subdirs, No Wildcards
                                        ;  1) Subdirs,    No Wildcards
                                        ;  2) No Subdirs, Wildcards
                                        ;  3) Subdirs,    Wildcards
J_LastSectionCompare:
                                        ; Case 3: We don't care about the next
                                        ; section - skip right to file compare.
        cmp     uType,bitSUBDIRS OR bitWILDCARDS
        je      J_FileCompare

                                        ; Happens if exclusion is 'C:\DOG\'.
        cmp     [XSI],BYTE PTR 0        ; This is the same as having the
        je      J_LastSectionMatch      ; last section match.

        INVOKE  MatchOneSection
        je      J_LastSectionMatch

LastSectionNoMatch:
        test    uType,bitSUBDIRS        ; If we didn't match and subdirs are
        jz      J_NoMatch               ; not included, then no match.

        jmp     J_FileCompare           ; Else, test the filename portion.

J_LastSectionMatch:
        cmp     XES[XDI],BYTE PTR 0     ; All cases: if we're at the end of
        je      J_Match                 ; the filename, then we matched.


        cmp     uType,bitSUBDIRS        ; Case 1:  If we include subdirs, and
        je      J_Match                 ; this matched w/o wild, then match.

                                        ; Case 2:  Wildcard must be a filespec.
        cmp     uType,bitWILDCARDS      ; Since this is not the end of the
        je      J_NoMatch               ; filename, then no match.

                                        ; Case 0:  If we're now at the start
        cmp     XDI,offFileFile         ; of the file, then the whole exclusion
        je      J_Match                 ; was directories only and we matched,
        jmp     J_NoMatch               ; otherwise no match.


                                        ; At this point, we have only two
                                        ; of our previous cases remaining:
                                        ;  1) Subdirs, No Wildcards
                                        ;  3) Subdirs, Wildcards
J_FileCompare:
        mov     XSI,offExclFile         ; Setup registers to do the file
        mov     XDI,offFileFile         ; compare

        INVOKE  MatchOneSection

        jne     J_NoMatch
        ; Fall through to J_Match

J_Match:
        mov     XAX,TRUE
        jmp     J_Done

J_NoMatch:
        mov     XAX,FALSE

J_Done:
        ret

ExcludeCheck  ENDP


;----------------------------------------------------------------------------
; FindFileOffset
;
; Returns the offset of the beginning of the filename portion of the path
;
; Input:
;       lpFilename:LPSTR        Full pathname
;
; Output:
;       NC      Found filename portion of path
;       CY      Error encountered
;
;       AX      offset of filename portion
;       DX      bit flags - bitSUBDIRS & bitWILDCARDS
;----------------------------------------------------------------------------
; 11/12/93 DALLEE, Function created.
;----------------------------------------------------------------------------

FindFileOffset  PROC  CODEDISTANCE  USES SES XDI XBX XDX,
        lpFilename:LPSTR

        xor     XDX,XDX

        LES_DI  lpFilename
        mov     XBX,XDI                 ; Save XDI

                                        ; Set the subdirs flag if there is
                                        ; no directory.  ex. "C:MOJO.COM"
J_FindExcludeFileLoop:
        cmp     XES[XDI],BYTE PTR 0
        je      J_CheckFail
        INVOKE  DBCS_DBCSIsLeadByte, BYTE PTR XES[XDI]
        cmp     XAX,FALSE
        je      J_CheckWild
        inc     XDI
        inc     XDI
        jmp     J_FindExcludeFileLoop

J_CheckWild:
        cmp     XES[XDI],BYTE PTR '?'
        je      @@J_Wild
        cmp     XES[XDI],BYTE PTR '*'
        jne     @@J_NotWild
@@J_Wild:
        or      XDX,bitWILDCARDS
        inc     XDI
        jmp     J_FindExcludeFileLoop

@@J_NotWild:
        cmp     XES[XDI],BYTE PTR '\'
        je      J_KeepPtr
        cmp     XES[XDI],BYTE PTR ':'
        jne     J_IncXDI

J_KeepPtr:
        mov     XBX,XDI                 ; Keep a pointer to the last ':' or
                                        ;  '\' found
J_IncXDI:
        inc     XDI
        jmp     J_FindExcludeFileLoop

J_CheckFail:
        cmp     XBX,XDI                 ; If XBX == XDI, then XDI was never
                                        ;  incremented. Thus must be error
        je      J_FindFileFail

        cmp     [XBX],BYTE PTR '\'
        je      J_NextCharPos
        or      XDX,bitSUBDIRS          ; Set the subdirs flag since no dir.
                                        ;  if we reached the start of string.
        cmp     [XBX],BYTE PTR ':'
        jne     J_SetOutput
J_NextCharPos:
        inc     XBX                     ; Offset of char after '\' or ':'.

J_SetOutput:
        mov     XAX,XBX
        clc
        jmp     J_FindFileDone

J_FindFileFail:
        xor     XAX,XAX
        mov     XDX,XAX
        stc

J_FindFileDone:
        ret

FindFileOffset  ENDP


;----------------------------------------------------------------------------
; MatchOneSection
;
; Check if one section of a path name matches one section of an exclude record.
; Checks from current position to next '\' or EOS.
;
; Input:
;       DS:SI   Start of exclude record section.
;       ES:DI   Start of path name section.
;     Start of each section should be first char after '\', NOT '\'.
; Output:
;       ZR      Zero flag set on match
;       NZ      No match
;
;       DS:SI   Start of next section (Will point to EOS or char after '\')
;       ES:DI   Start of next section (Will point to EOS or char after '\')
;----------------------------------------------------------------------------
; 11/11/93 DALLEE, Function created.
;----------------------------------------------------------------------------

MatchOneSection  PROC  CODEDISTANCE

        cld

J_MatchOneStart:
                                        ; Is this the end of this section of
                                        ; the exclude struct?
        INVOKE  DBCS_DBCSIsLeadByte, BYTE PTR [XSI].EXCLUDE.szText
        cmp     XAX,FALSE
        je      @F
        cmpsw
        je      J_MatchOneStart
        jmp     J_MatchOneFail

@@:
        cmp     [XSI].EXCLUDE.szText,BYTE PTR '\'
        je      J_EndExclude
        cmp     [XSI].EXCLUDE.szText,BYTE PTR 0
        je      J_EndExclude

                                        ; Check for wildcards
        cmp     [XSI].EXCLUDE.szText,BYTE PTR '*'
        je      J_SkipToDelimiter
        cmp     [XSI].EXCLUDE.szText,BYTE PTR '?'
        je      J_SkipOne

        cmpsb
        je      J_MatchOneStart
                                        ; Mismatch at this point, but
                                        ; a null or wildcard extension should
                                        ; match a filename with no extension
        cmp     [XSI].EXCLUDE.szText-1,BYTE PTR '.'
        jne     J_MatchOneFail

        dec     XDI                     ; cmpsb inc'd DI, Look back one...
        cmp     XES[XDI],BYTE PTR 0     ; Did we find end in filename?
        je      J_MatchOneStart

        jmp     J_MatchOneFail

J_SkipToDelimiter:
        inc     XSI                     ; Over * in Structure

J_LookForDelimiter:
        cmp     XES[XDI],BYTE PTR 0
        je      J_MatchOneStart
        INVOKE  DBCS_DBCSIsLeadByte, BYTE PTR XES[XDI]
        cmp     XAX,FALSE
        je      @F
        inc     XDI
        inc     XDI
        jmp     J_LookForDelimiter
@@:
        cmp     XES[XDI],BYTE PTR 5Ch   ; Backslash '\'
        je      J_MatchOneStart
        cmp     XES[XDI],BYTE PTR '.'   ; Point
        je      J_MatchOneStart
        inc     XDI
        jmp     J_LookForDelimiter

J_SkipOne:
        inc     XSI                     ; Over ? in Structure

        cmp     XES[XDI],BYTE PTR 0
        je      J_MatchOneStart
        INVOKE  DBCS_DBCSIsLeadByte, BYTE PTR XES[XDI]
        cmp     XAX,FALSE
        je      @F
        inc     XDI
        inc     XDI
        jmp     J_MatchOneStart
@@:
        cmp     XES[XDI],BYTE PTR 5Ch   ; Backslash '\'
        je      J_MatchOneStart
        cmp     XES[XDI],BYTE PTR '.'   ; Point
        je      J_MatchOneStart
        inc     XDI                     ; Over CHAR in filename
        jmp     J_MatchOneStart

J_EndExclude:
        cmp     XES[XDI],BYTE PTR 0     ; Are we done?
        je      J_MatchOneSuccess
        INVOKE  DBCS_DBCSIsLeadByte, BYTE PTR XES[XDI]
        cmp     XAX,TRUE
        je      J_MatchOneFail

        cmp     XES[XDI],BYTE PTR '\'
        je      J_MatchOneSuccess
        ; Fall through to J_MatchOneFail

J_MatchOneFail:
        mov     XAX,FALSE               ; set XAX FALSE
        jmp     J_SetSIToEnd

J_MatchOneSuccess:
        mov     XAX,TRUE
        ; Fall through to J_SetSIToEnd

J_SetSIToEnd:
        cmp     [XSI].EXCLUDE.szText,BYTE PTR 0
        je      J_SetDIToEnd
        INVOKE  DBCS_DBCSIsLeadByte, BYTE PTR [XSI].EXCLUDE.szText
        cmp     XAX,FALSE
        je      @F
        inc     XSI
        inc     XSI
        jmp     J_SetSIToEnd
@@:
        inc     XSI
                                        ; Check for '/' after inc XSI since
                                        ; We should point to the char after '/'
        cmp     [XSI-1].EXCLUDE.szText,BYTE PTR '\'
        jne     J_SetSIToEnd
        ; Fall through to J_SetDIToEnd

J_SetDIToEnd:
        cmp     XES[XDI],BYTE PTR 0
        je      J_Return
        INVOKE  DBCS_DBCSIsLeadByte, BYTE PTR XES[XDI]
        cmp     XAX,FALSE
        je      @F
        inc     XDI
        inc     XDI
        jmp     J_SetDIToEnd
@@:
        inc     XDI
        cmp     XES[XDI-1],BYTE PTR '\' ; Check for '/' after inc XDI since
        jne     J_SetDIToEnd            ; We should point to the char after '/'
        ; Fall through to return

J_Return:
        cmp     XAX,TRUE                ; Set Zero flag if we matched

        ret
MatchOneSection  ENDP

;----------------------------------------------------------------------------
UniqueSegmentEnd
END
