include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; ExcludeCheck                          4/28/93                         CAG
;
; $Header:   S:\certlib4\vcs\x1.asv   1.10   29 Nov 1994 13:07:38   DALLEE  $
;
;       Check exclude element if item is excluded.
;
; BOOL WINAPI ExcludeCheck(FAR *EXCLUDE, LPSTR Filename, WORD Bits);
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
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\x1.asv  $
;  
;     Rev 1.10   29 Nov 1994 13:07:38   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.7.1.3   09 Dec 1993 13:11:38   DALLEE
;  Fixed bug with exclusions ending in '\' with no subdirectories.
;  
;     Rev 1.7.1.2   30 Nov 1993 14:14:32   DALLEE
;  Fixed GPF in Windows.  FindFileOffset was searching back too many characters
;  for the beginning of the filename and pointing XDI to an invalid address.
;  
;     Rev 1.7.1.1   17 Nov 1993 18:29:14   DALLEE
;  Rewrite to allow wildcard filemask to be used with subdirectories flag.
;  Also handles ambiguous end of exclude spec - tries it as both a last 
;    directory to check, or a filemask.
;  
;     Rev 1.7.1.0   19 Oct 1993 07:21:08   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.7   12 Oct 1993 16:31:48   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.6.1.2   09 Sep 1993 23:32:48   DALLEE
;  Allow for trailing '\'
;  
;     Rev 1.6.1.1   09 Sep 1993 18:44:24   DALLEE
;  Make sure sub-dirs flag gets set.
;  
;     Rev 1.6.1.0   03 Sep 1993 19:29:32   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.6   16 Aug 1993 20:50:44   DALLEE
;  [enh] Now allows exclusions to start from root directory with no drive spec.
;            \DIR\SUBDIR matches C:\DIR\SUBDIR\FILENAME.EXT, etc.
;  [mod] On full path compares with drives letters, do not skip to 3rd
;            byte to begin compare - possible to miss EOS on the filename.
;  
;     Rev 1.5   17 Jul 1993 19:56:16   DALLEE
;  Handle cases where the filename has ended, but the exclude spec has only
;  wildcards remaining, and cases where the exclude spec ends with a '.'
;     'C:\NOTVIRUS' now matches 'NOTVIRUS.???'
;  
;     Rev 1.4   24 Jun 1993 15:34:32   CGUC
;  Fixed problem of matches matching when they shouldn't
;  
;     Rev 1.3   13 May 1993 17:56:26   DALLEE
;  inc XDI in LookMoreSlash loop.  Was hanging here.
;-----------------------------------------------------------------------------

bitSUBDIRS      EQU     01h
bitWILDCARDS    EQU     02h

if FARCALL
MatchOneSection PROTO FAR
FindFileOffset  PROTO FAR,:LPSTR
else
MatchOneSection PROTO NEAR
FindFileOffset  PROTO NEAR,:LPSTR
endif

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
        mov     dx,SDS
        mov     XAX,XSI
        add     XAX,EXCLUDE.szText


        INVOKE  FindFileOffset,dx::ax
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

        mov     XAX,uBits               ; Determine what bits are to be tested
        and     XAX,[XSI].EXCLUDE.wBits
        jz      J_NoMatch               ; Nothing in this entry matches
        cmp     XAX,uBits
        jne     J_NoMatch


        cld                             ; Prepare for string compares
        add     XSI,EXCLUDE.szText

CheckForDrive:                          ; Is there a drive in the exclusion?
        cmp     [XSI+1], byte ptr ':'
        je      J_DriveCompare
        cmp     XES[XDI+1], byte ptr ':'
        jne     J_DirCompare
        add     XDI,2                   ; Advance past '<drive>:' in filename
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
        sub     XAX,EXCLUDE.szText
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
        cmp     [XSI],byte ptr 0        ; This is the same as having the
        je      J_LastSectionMatch      ; last section match.

        INVOKE  MatchOneSection
        je      J_LastSectionMatch

LastSectionNoMatch:
        test    uType,bitSUBDIRS        ; If we didn't match and subdirs are
        jz      J_NoMatch               ; not included, then no match.

        jmp     J_FileCompare           ; Else, test the filename portion.

J_LastSectionMatch:
        cmp     XES[XDI],byte ptr 0     ; All cases: if we're at the end of
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

ExcludeCheck ENDP


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

FindFileOffset PROC USES SES XDI XCX,
        lpFilename:LPSTR

        xor     XDX,XDX
                                        ; Search for end of filename
        cld

        LES_DI  lpFilename
        mov     XCX,MAX_NET_PATH_SIZE
        mov     al,0
        repne   scasb
        je      J_FindExcludeFile       ; Found end of exclusion.
        jmp     J_FindFileFail          ; Invalid exclusion.

                                        ; Search for the first char of the
                                        ; filemask portion of the exclusion.
                                        ; Set the wilcards flag if any are
                                        ; found.
                                        ; Set the subdirs flag if there is
                                        ; no directory.  ex. "C:MOJO.COM"
J_FindExcludeFile:
        dec     XDI                     ; XDI was one past EOS, back up one
                                        ;  and one more when we start the loop.
        neg     XCX
        add     XCX,MAX_NET_PATH_SIZE-1 ; Set max number of bytes to look back
        jcxz    J_FindFileFail          ; Invalid exclusion since this was
                                        ;  a zero length string.
J_FindExcludeFileLoop:
        dec     XDI
        cmp     XES[XDI],byte ptr '?'
        je      @@J_Wild
        cmp     XES[XDI],byte ptr '*'
        jne     @@J_NotWild
@@J_Wild:
        or      XDX,bitWILDCARDS

@@J_NotWild:
        cmp     XES[XDI],byte ptr '\'
        je      J_FoundExcludeFile
        cmp     XES[XDI],byte ptr ':'
        loopne  J_FindExcludeFileLoop

J_FoundExcludeFileNoDir:
        or      XDX,bitSUBDIRS          ; Set the subdirs flag since no dir.
        jcxz    J_NoDrive               ; Inc XDI if we stopped on ':', not
                                        ;  if we reached the start of string.

J_FoundExcludeFile:
        inc     XDI                     ; Offset of char after '\' or ':'.

J_NoDrive:
        mov     XAX,XDI
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

MatchOneSection PROC

        cld

J_MatchOneStart:
                                        ; Is this the end of this section of
                                        ; the exclude struct?
        cmp     [XSI].EXCLUDE.szText,byte ptr '\'
        je      J_EndExclude
        cmp     [XSI].EXCLUDE.szText,byte ptr 0
        je      J_EndExclude

                                        ; Check for wilcards
        cmp     [XSI].EXCLUDE.szText,byte ptr '*'
        je      J_SkipToDelimiter
        cmp     [XSI].EXCLUDE.szText,byte ptr '?'
        je      J_SkipOne

        cmpsb
        je      J_MatchOneStart

                                        ; Mismatch at this point, but
                                        ; a null or wildcard extension should
                                        ; match a filename with no extension
        cmp     [XSI].EXCLUDE.szText-1,byte ptr '.'
        jne     J_MatchOneFail

        dec     XDI                     ; cmpsb inc'd DI, Look back one...
        cmp     XES[XDI],byte ptr 0     ; Did we find end in filename?
        je      J_MatchOneStart

        jmp     J_MatchOneFail

J_SkipToDelimiter:
        inc     XSI                     ; Over * in Structure

J_LookForDelimiter:
        cmp     XES[XDI],byte ptr 0
        je      J_MatchOneStart
        cmp     XES[XDI],byte ptr 5Ch   ; Slash
        je      J_MatchOneStart
        cmp     XES[XDI],byte ptr '.'   ; Point
        je      J_MatchOneStart
        inc     XDI
        jmp     J_LookForDelimiter

J_SkipOne:
        inc     XSI                     ; Over ? in Structure

        cmp     XES[XDI],byte ptr 0
        je      J_MatchOneStart
        cmp     XES[XDI],byte ptr 5Ch   ; Slash
        je      J_MatchOneStart
        cmp     XES[XDI],byte ptr '.'   ; Point
        je      J_MatchOneStart
        inc     XDI                     ; Over CHAR in filename
        jmp     J_MatchOneStart

J_EndExclude:
        cmp     XES[XDI],byte ptr 0     ; Are we done?
        je      J_MatchOneSuccess
        cmp     XES[XDI],byte ptr '\'
        je      J_MatchOneSuccess
        ; Fall through to J_MatchOneFail

J_MatchOneFail:
        mov     XAX,FALSE               ; set XAX FALSE
        jmp     J_SetSIToEnd

J_MatchOneSuccess:
        mov     XAX,TRUE
        ; Fall through to J_SetSIToEnd

J_SetSIToEnd:
        cmp     [XSI].EXCLUDE.szText,byte ptr 0
        je      J_SetDIToEnd
        inc     XSI
                                        ; Check for '/' after inc XSI since
                                        ; We should point to the char after '/'
        cmp     [XSI-1].EXCLUDE.szText,byte ptr '\'
        jne     J_SetSIToEnd
        ; Fall through to J_SetDIToEnd

J_SetDIToEnd:
        cmp     XES[XDI],byte ptr 0
        je      J_Return
        inc     XDI
        cmp     XES[XDI-1],byte ptr '\' ; Check for '/' after inc XDI since
        jne     J_SetDIToEnd            ; We should point to the char after '/'
        ; Fall through to return

J_Return:
        cmp     XAX,TRUE                ; Set Zero flag if we matched

        ret
MatchOneSection ENDP

;-----------------------------------------------------------------------------
END

