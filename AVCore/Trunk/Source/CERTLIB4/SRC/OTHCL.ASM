include CTS4.INC

GetString       PROTO NEAR,:LPSTR
StoreString     PROTO NEAR,:LPSTR,:LPSTR
DoFlag          PROTO NEAR,:LPSTR,:LPSTR

UniqueSegment
;-----------------------------------------------------------------------------
; OTH4_COMMAND_LINE                    1/13/93                         CAG
;
;       Parse a command line into a structure.
;
; WORD WINAPI OTH4_COMMAND_LINE(COMMANDLINE FAR *);
;
; Input:
;       1)  Pointer to a command line structure
; Output:
;       1)  PASSED (0)
;       2)  NoCommandLine (1)
;       3)  ERROR (-1)
;
; WORD WINAPI SYS4_PARSE_CL(COMMANDLINE FAR *,LPSTR Line);
;
; Input:
;       1)  Pointer to a command line structure
;       2)  A string to translate the command line from
; Output:
;       1)  PASSED (0)
;       2)  NoCommandLine (1)
;       3)  ERROR (-1)
;
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
; Note:
;       This routine contains a counterpart that will work with
;       the DEVICE Driver.  The LINE that gets passed into
;       the function SYS4_PARSE_CL must start with the first
;       flag, and end with a NULL.
;
;-----------------------------------------------------------------------------
OTH4_COMMAND_LINE PROC USES XBX XCX XDX SDS XSI,
        lpCommandLine:LPSTR

; Get pointer to command line
        mov     ah,62h                  ; Get Current PSP segment.
        int     21h
        mov     ds,bx
        mov     si,80h                  ; Start of Command Line.

; Get length of command line
        xor     ax,ax
        lodsb

        cmp     al,1
        jle     NoCommandLine

        push    si
        add     si,ax
        mov     [XSI],word ptr 0        ; Make end of line NULL terminated
        pop     si

        inc     si                      ; To start of command line
        mov     cx,ax                   ; Length in CX
        inc     cx
        INVOKE  MEM4_REPLACE,vSDS XSI,CR,0,cx

        INVOKE  SYS4_PARSE_CL,lpCommandline,vSDS XSI
        jmp     Done
NoCommandLine:
        mov     ax,1
Done:

        ret

OTH4_COMMAND_LINE ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
SYS4_PARSE_CL PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpCommandLine:LPSTR,
        lpThisString:LPSTR

        LDS_SI  lpThisString
        cld                             ; We're doing string stuff, make
                                        ; sure we're going the right way.

RemoveSpace:
        cmp     [XSI],byte ptr 0
        je      Done
        cmp     [XSI],byte ptr 20h
        jne     NoSpace
        inc     XSI
        jmp     RemoveSpace
NoSpace:
        cmp     [XSI],byte ptr 2Fh      ; Check for slash
        je      IsSlash
        cmp     [XSI],byte ptr 5Ch
        je      IsSlash
        cmp     [XSI],byte ptr 0
        je      Done

; Must be string or filename
        INVOKE  GetString,lpCommandLine
        or      ax,ax
        jz      Error

        INVOKE  StoreString,vSDS XSI,dx::ax
        or      ax,ax
        jz      Error
        add     si,ax
        jmp     RemoveSpace

IsSlash:
        inc     si
        INVOKE  DoFlag,lpCommandLine,vSDS XSI
        or      ax,ax
        jz      Error
        add     si,ax
        jmp     RemoveSpace

Error:
        mov     ax,-1
        jmp     JOut
Done:
        mov     ax,0
        jmp     JOut
NoCommandLine:  
        mov     ax,1
JOut:
        ret

SYS4_PARSE_CL   ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
GetString PROC NEAR USES SDS XSI,
        lpCommand:LPSTR

        LDS_SI  lpCommand               ; Pointer to structure

LookMore:
        cmp     [XSI].COMMANDLINE.CL_TYPE,0
        je      NoItem

        test    [XSI].COMMANDLINE.CL_TYPE,10000000b
        jnz     Used

        cmp     [XSI].COMMANDLINE.CL_TYPE,CL_STRING
        jne     NotFound

        or      [XSI].COMMANDLINE.CL_TYPE,10000000b
        LDS_SI  [XSI].COMMANDLINE.CL_STORE
        mov     dx,ds
        mov     ax,si
        jmp     Found

NotFound:
Used:
        add     XSI,sCOMMANDLINE
        jmp     LookMore        
NoItem:
        xor     ax,ax
Found:
        ret

GetString ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
StoreString PROC NEAR USES XBX XCX SDS SES XSI XDI,
        lpFrom:LPSTR,
        lpTo:LPSTR

        LDS_SI  lpFrom
        LES_DI  lpTo

        xor     bx,bx                   ; Quote flag
        xor     cx,cx

        cmp     [XSI],byte ptr '"'
        jne     NoQuote
        inc     bx
        inc     si
NoQuote:

MoreString:
        lodsb
        cmp     al,'"'
        je      IsQuote
        cmp     al,0
        je      IsEnd
        cmp     al,' '
        je      IsTryEnd
StoreIt:
        stosb
        inc     cx
        jmp     MoreString
IsTryEnd:
        or      bx,bx
        jz      Done
        jmp     Storeit

IsEnd:
        dec     si
        or      bx,bx
        jz      Done
        xor     ax,ax
        jmp     Error
IsQuote:
Done:
        mov     ax,cx
Error:
        ret

StoreString ENDP
;-----------------------------------------------------------------------------
DoFlag PROC NEAR USES XBX SDS SES XSI XDI,
        lpCommand:LPSTR,
        lpLookMe:LPSTR

        xor     XCX,XCX
        LES_DI  lpCommand               ; Pointer to structure

LookMore:
        cmp     XES[XDI].COMMANDLINE.CL_TYPE,0
        je      NoItem

        test    XES[XDI].COMMANDLINE.CL_TYPE,10000000b
        jnz     Used

        cmp     XES[XDI].COMMANDLINE.CL_TYPE,CL_STRING
        je      NotFound

; Determine if its what we want
        push    es
        push    di
        LDS_SI  XES[XDI].COMMANDLINE.CL_MASK
        LES_DI  lpLookMe

MoreMe:
        lodsb
        or      al,al
        jz      IsMe

        cmp     XES[XDI],al             ; First check case as is
        je      IsMeCont                ; Then uppercase it

        and     al,5Fh
        and     XES[XDI],byte ptr 5Fh   ; Make uppercase

        cmp     XES[XDI],al
        jne     NotMe
IsMeCont:
        inc     XDI
        inc     XCX
        jmp     MoreMe
IsMe:
        pop     di
        pop     es
        jmp     DoCopy
NotMe:
        pop     di
        pop     es
        jmp     NotFound

DoCopy:

; Determine type
        cmp     XES[XDI].COMMANDLINE.CL_TYPE,CL_FLAG
        je      IsFlag

; Reload DS:SI to equal command to parse
        LDS_SI  lpLookMe

; No look for space after command to get second part
; of the command:  ie.  /FLAG 1

LookMoreForSpace:
        lodsb

        or      al,al                   ; Really an error, but use it like
        jz      IsFlag                  ; it was a flag.
        cmp     al,20h
        jne     LookMoreForSpace

        inc     cx                      ; Allow for the space to be in command line

; Now DS:SI should equal a pointer to 1

        cmp     XES[XDI].COMMANDLINE.CL_TYPE,CL_NUMERIC
        je      IsNumber
        cmp     XES[XDI].COMMANDLINE.CL_TYPE,CL_HEX
        je      IsHEX

; Must be string!
        or      XES[XDI].COMMANDLINE.CL_TYPE,10000000b
        INVOKE  StoreString,vSDS XSI,XES[XDI].COMMANDLINE.CL_STORE
        jmp     Found

IsFlag:
        or      XES[XDI].COMMANDLINE.CL_TYPE,10000000b
        LES_DI  XES[XDI].COMMANDLINE.CL_STORE
        mov     XES[XDI],byte ptr TRUE
        mov     XAX,XCX
        jmp     Found
IsHEX:
        or      XES[XDI].COMMANDLINE.CL_TYPE,10000000b
        INVOKE  STR4_ASCII_TO_WORD,vSDS XSI
        jmp     DoMore

IsNumber:
        or      XES[XDI].COMMANDLINE.CL_TYPE,10000000b
        INVOKE  STR4_ASCII_TO_INT,vSDS XSI
DoMore:
        LES_DI  XES[XDI].COMMANDLINE.CL_STORE
        mov     XES[XDI],ax
NotDoneDone:
        lodsb

        or      al,al
        jz      DoneDone

        cmp     al,20h
        jz      DoneDone
        inc     XCX
        jmp     NotDoneDone
DoneDone:
        mov     XAX,XCX
        jmp     Found

NotFound:
Used:
        xor     XCX,XCX
        add     XDI,sCOMMANDLINE
        jmp     LookMore        
NoItem:
        xor     XAX,XAX
Found:
        ret

DoFlag ENDP
;-----------------------------------------------------------------------------
END
