include CTS4.INC
include _TSR.INC

END_OF_DATE     equ     8     
END_OF_TIME     equ     8
DEST_OFFSET     equ     100h
ENV_PTR_OFF     equ     02ch

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_FORMAT                          12/16/92                        CAREY
;
;       Format a string and insert proper information.
;
; BOOL WINAPI STR4_FORMAT(TSR_Struct FAR *fStruct, LPSTR fSource);
;
; Input:
;       1)  TSR Structure filled in.
;       2)  Source and destination string.
; Output:
;       1)  Source string + 100h will contain the new message.
;           We have a limit of 256 characters.
;       2)  ERROR (0)
;       3)  PASSED (1)
;
; Note:
;       This function takes a string which can contain several control codes
;       and formats it with the necessary information.  The following are
;       valid codes which it recognizes:
;
;       #e - Insert null terminated extra buffer
;       #d - Insert null terminated drive letter and colon (A:)
;       #D - Insert null terminated directory name, followed by trailing backslash
;       #f - Insert null terminated filename
;       #a - Insert null terminated date in the form MM/DD/YY
;       #t - Insert null terminated time in the form HH:MM:SS
;       #u - Insert null terminated user name (This needs to be completed)
;       #P - Insert null terminated program name (of cur executing program)
;       #B - Insert null terminated work buffer
;   slashn - Inserts a newline sequence (CR/LF)
;
; LIBRARY: {INTEL}.LIB
;-----------------------------------------------------------------------------
STR4_FORMAT PROC USES XBX XCX XDX SDS SES XSI XDI,
        fStruct:LPSTR,
        fSource:LPSTR

        cld
        LDS_SI  fSource
        mov     XDI,XSI
        SMOV    es,ds
        add     XDI, DEST_OFFSET

parse_loop:
        lodsb                           ; load the next byte from source
        or      al, al                  ; EOS
        jz      done_loop

        cmp     al, '#'                 ; # control character?
        jnz     not_control             ; no
        lodsb                           ; obtain subfunction
        cmp     al, 'f'                 ; filename
        jnz     not_file_name
        INVOKE  ADD_FILE, fStruct, vSES XDI
        add     XDI,XAX                 ; adjust destination pointer
        jmp     parse_loop              ; on to next character

;--------------------------------------
not_file_name:
        cmp     al, 'D'                 ; directory
        jnz     not_directory
        INVOKE  ADD_DIR, fStruct, vSES XDI
        add     XDI,XAX                 ; adjust destination pointer
        jmp     parse_loop

;--------------------------------------
not_directory:
        cmp     al, 'd'                 ; drive 
        jnz     not_drive
        INVOKE  ADD_DRIVE_LETTER, fStruct, vSES XDI
        add     XDI,XAX
        jmp     parse_loop

;--------------------------------------
not_drive:
        cmp     al, 'e'                 ; extra buffer
        jnz     not_extra
        INVOKE  ADD_EXTRA, fStruct, vSES XDI
        add     XDI,XAX
        jmp     parse_loop                  

;--------------------------------------
not_extra:
        cmp     al, 'a'                 ; date
        jnz     not_date
        INVOKE  ADD_DATE, vSES XDI
        add     XDI,XAX
        jmp     parse_loop

;--------------------------------------
not_date:
        cmp     al, 't'                 ; time
        jnz     not_time
        INVOKE  ADD_TIME, vSES XDI
        add     XDI,XAX
        jmp     parse_loop

;--------------------------------------
not_time:
        cmp     al, 'u'
        jnz     offending_program
        INVOKE  ADD_USER, fStruct, vSES XDI
        add     XDI,XAX
        jmp     parse_loop

;--------------------------------------
offending_program:
        cmp     al, 'P'
        jnz     not_guilty
        INVOKE  INSERT_GUILTY_PROGRAM, fStruct, vSES XDI
        add     XDI,XAX
        jmp     parse_loop

;--------------------------------------
not_guilty:
        cmp     al, 'B'
        jnz     not_buffer
        INVOKE  ADD_BUFFER, fStruct, vSES XDI
        add     XDI,XAX
        jmp     parse_loop

;--------------------------------------
not_buffer:

;--------------------------------------
none_of_the_above:

        dec     XSI                     ; back up 2 chars
        dec     XSI
        lodsw
        stosw                           ; place #X where X is not a,t,D,d,e,f

        jmp     parse_loop


;--------------------------------------
not_control:
        cmp     al, 5Ch                 ; slash n sequence?
        jnz     not_newline
        lodsb
        cmp     al, 'n'                 
        jnz     just_slash              ; nope
        mov     ax, 0a0dh               ; CR
        stosw
        jmp parse_loop


;--------------------------------------
just_slash:
        dec     XSI                     ; since not n, dec si to get next char
        mov     al, 5Ch

not_newline:
        stosb                           ; save character

;--------------------------------------
end_of_loop:
        jmp parse_loop

;--------------------------------------
done_loop:
        stosb                           ; store a zero (NULL terminate string)
        ret

STR4_FORMAT	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADD_FILE                             12/16/92                        CAREY
;
;	
;
; WORD ADD_FILE(struct TSR_Struct *fStruct, LPSTR fDest)
;
;
; ENTER:	Arguments: Pointer to global structure, dest pointer
;  EXIT:	AX = length of filename 
; ERROR:	NONE
;
; Registers Destroyed:	NONE
; Information:
;       This function finds the filename specified by DS:DX or DS:SI 
;       (in the case of an extended function call) and copies only the
;       filename part over to the memory specified by fDest. 
;-----------------------------------------------------------------------------
ADD_FILE PROC USES XBX SDS SES XSI XDI,
        fStruct:LPSTR,
        fDest:LPSTR


        LES_DI  fStruct
        mov     ax, XES[XDI].TSR_Struct.uLFlags
        LES_DI  XES[XDI].TSR_Struct.lpREGS

        mov     XSI,XES[XDI].TSR_REGS.r_DX
        test    ax, bitDSSI
        jz      not_extended_open

        mov     XSI,XES[XDI].TSR_REGS.r_SI

not_extended_open:

        MOV_DS  XES[XDI].TSR_REGS.r_DS
        mov     XBX,XSI

searchnull:
        cmp     [XSI],byte ptr 0
        jz      looking_for_filename
        inc     XSI
        jmp     searchnull

looking_for_filename:       

        dec     XSI
        cmp     [XSI], byte ptr 5Ch
        jz      found_filename

        cmp     XSI,XBX
        jae     looking_for_filename

found_filename:

        inc     XSI
        LES_DI  fDest
        xor     bx, bx

copy_over_filename:
                
        lodsb
        stosb
        or      al, al                  ; NULL character is appended
        jz      done_copying
        inc     bx
        jmp     copy_over_filename

done_copying:

        xchg    ax, bx                  ; place length of file in AX

        ret

ADD_FILE	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADD_DIR                              12/16/92                        CAREY
;
;
; WORD ADD_DIR(struct TSR_Struct *fStruct, LPSTR fDest)
;
;
;
; ENTER:	Arguments: Pointer to global struct, Dest. pointer
;  EXIT:	AX = length of directory name
; ERROR:	NONE
;
; Information: This will include a trailing backslash if it is found.
;
; Registers Destroyed:	NONE
;-----------------------------------------------------------------------------
ADD_DIR PROC USES XBX SDS SES XSI XDI,
        fStruct:LPSTR,
        fDest:LPSTR

        LES_DI  fStruct

        mov     ax, XES[XDI].TSR_Struct.uLFlags
        LES_DI  XES[XDI].TSR_Struct.lpREGS

        mov     XSI, XES[XDI].TSR_REGS.r_DX
        test    ax, bitDSSI
        jz      not_extended_open

        mov     XSI, XES[XDI].TSR_REGS.r_SI

not_extended_open:

        MOV_DS  XES[XDI].TSR_REGS.r_DS
        mov     XBX,XSI

        LES_DI  fDest
        xor     bx, bx

more_chars:

        lodsb
        stosb
        inc     bx
        or      al, al
        jnz     more_chars

keep_looking:

        or      bx, bx
        jz      found_last_slash

        dec     bx
        dec     XDI
        mov     al, XES[XDI]
        cmp     al, 5Ch
        jz      found_last_slash

        jmp     keep_looking
        
found_last_slash:

        stosb
        xor     al, al
        stosb                           ; null terminate string at slash

        inc     bx                      ; make sure to include slash

        xchg    ax, bx                  ; put string length in ax

        ret

ADD_DIR	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADD_DRIVE_LETTER                     12/16/92                        CAREY
;
;
; WORD ADD_DRIVE_LETTER(struct TSR_Struct *fStruct, LPSTR fDest)
;
;	
;
; ENTER:	Arguments: Pointer to global structure, dest pointer
;  EXIT:	AX = 2 (sizeof "X:")
; ERROR:	NONE
;
; Registers Destroyed:	NONE
;-----------------------------------------------------------------------------
ADD_DRIVE_LETTER PROC USES XBX SDS SES XSI XDI,
        fStruct:LPSTR,
        fDest:LPSTR

; DX contains the drive letter of the affected drive.
        LES_DI  fStruct

        LES_DI  XES[XDI].TSR_Struct.lpREGS
        mov     XAX, XES[XDI].TSR_REGS.r_DX

        test    al, 080h                ; hard drive?
        jz      floppy_drive

        and     al, 07fh                ; remove high bit
        add     al, 2                   ; this is now drive C
        
floppy_drive:

        add     al, 'A'                 ; obtain drive letter
                
        LES_DI  fDest

        stosb   

        mov     ax, 003ah               ; store :=3ah,null character=00

        stosw

        mov     ax, 2

        ret

ADD_DRIVE_LETTER	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADD_EXTRA                            12/16/92                        CAREY
;
;
;
; WORD ADD_EXTRA(struct TSR_Struct *fStruct, LPSTR fDest)
;	
;
; ENTER:	Arguments: Pointer to global struct, Dest pointer
;  EXIT:	AX = length of extra buffer
; ERROR:	NONE
;
; Registers Destroyed:	NONE
;-----------------------------------------------------------------------------
ADD_EXTRA PROC USES XBX SDS SES XSI XDI,
        fStruct:LPSTR,
        fDest:LPSTR

        LES_DI  fStruct
        LDS_SI  XES[XDI].TSR_Struct.lpExtra

        LES_DI  fDest
        xor     bx, bx

copy_extra:
        lodsb                           ; copy data
        stosb
        or      al, al
        jz      done_copying_extra

        inc     bx
        jmp     copy_extra

done_copying_extra:
        xchg    ax, bx
        ret

ADD_EXTRA	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADD_BUFFER                            12/16/92                        CAREY
;
;
;
; WORD ADD_BUFFER(struct TSR_Struct *fStruct, LPSTR fDest)
;	
;
; ENTER:	Arguments: Pointer to global struct, Dest pointer
;  EXIT:	AX = length of extra buffer
; ERROR:	NONE
;
; Registers Destroyed:	NONE
;-----------------------------------------------------------------------------
; UPDATE                               12/29/92				CAG
;
;       Copied ADD_EXTRA to make ADD_BUFFER
;
;-----------------------------------------------------------------------------
ADD_BUFFER PROC USES XBX SDS SES XSI XDI,
        fStruct:LPSTR,
        fDest:LPSTR

        LES_DI  fStruct
        LDS_SI  XES[XDI].TSR_Struct.lpBuffer

        LES_DI  fDest

        xor     bx, bx

copy_buffer:
        lodsb                           ; copy data
        stosb
        or      al, al
        jz      done_copying_buffer
        inc     bx
        jmp     copy_buffer

done_copying_buffer:
        xchg    ax, bx
        ret

ADD_BUFFER	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADD_DATE                             12/16/92                        CAREY
;
;
;
; WORD ADD_DATE(struct TSR_Struct *fStruct, LPSTR fDest)
;	
;
; ENTER:	Arguments: Dest Pointer.
;  EXIT:	AX = length of date (8 bytes)
; ERROR:	NONE
;
; Registers Destroyed:	NONE
;-----------------------------------------------------------------------------
ADD_DATE PROC,
        fDest:LPSTR

        INVOKE  DAT4_ENCODE
        INVOKE  DAT4_ENCODE_TO_ASCII,ax,fDest
        mov     ax, END_OF_DATE         ; size of a date
        ret

ADD_DATE	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADD_TIME                             12/16/92                        CAREY
;
;
;
; WORD ADD_TIME(struct TSR_Struct *fStruct, LPSTR fDest)
;	
;
; ENTER:	Arguments: Pointer to destination
;  EXIT:	AX = time length (8 bytes)
; ERROR:	NONE
;
; Registers Destroyed:	NONE
;-----------------------------------------------------------------------------
ADD_TIME PROC,
        fDest:LPSTR

        INVOKE  TIM4_ENCODE
        INVOKE  TIM4_ENCODE_TO_ASCII,ax,fDest
        mov     ax, END_OF_TIME         ; size of time string
        ret

ADD_TIME	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADD_USER                             12/17/92                        CAREY
;
;	
;
; ENTER:	WORD ADD_USER(LPSTR fDest)
;  EXIT:	AX = length of null terminated user's name
; ERROR:	NONE
;
; Registers Destroyed:	NONE
;-----------------------------------------------------------------------------
ADD_USER PROC USES SDS SES XSI XDI,
        lpStruct:LPSTR,
        fDest:LPSTR

        LOCAL   lpUserName:DWORD

        LES_DI  lpStruct
        INVOKE  MEM4_GETOBJECT,ObjectUserName,XES[XDI].TSR_STRUCT.lpGlobal
        or      dx,dx
        jz      NoUser
        mov     word ptr lpUserName+2,dx
        mov     word ptr lpUserName,ax


        INVOKE  STR4_LENGTH,lpUserName
        mov     cx,ax

        INVOKE  STR4_COPY,lpUserName,fDest
        mov     ax,cx
        jmp     J_Done
NoUser:
        xor     ax,ax
J_Done:
        ret

ADD_USER	ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; INSERT_GUILTY_PROGRAM                12/18/92                        CAREY
;
;	
;
; ENTER:	NONE
;  EXIT:	NONE
; ERROR:	NONE
;
; Registers Destroyed:	NONE
;-----------------------------------------------------------------------------
INSERT_GUILTY_PROGRAM PROC USES XBX XCX XDX SDS SES XSI XDI,
        fStruct:LPSTR,
        fDest:LPSTR

; Obtain PSP segment in ES, and LOCAL ENV PTR in DS

        LDS_SI  fStruct
        MOV_ES  [XSI].TSR_Struct.uCurrPSP
        MOV_ES  XES[ENV_PTR_OFF]

; now that we have the segment of our local environment, we want to find the
; currently executing filename...
; The local environment contains a bunch of garbage, followed by two NULL
; characters and then, shortly after the name of the executing program...

        xor     XDI,XDI                 ; ES:DI points to start of ENV
        mov     cx, 0ffffh              ; MAX count just in case
        xor     ax, ax
        
looperoo:

        repne   scasb
        cmp     byte ptr XES[XDI], 0
        jz      yee_haw
        loop    looperoo

        stc     
        xor     cx, cx
        jmp     all_done        

yee_haw:
        
        inc     XDI                     ; skip by 0
        inc     XDI                     ; skip by 1
        inc     XDI                     ; skip by 2
        xchg    XDI,XSI
        SMOV    ds,es
        xor     cx, cx                  ; count of length of prog name
        LES_DI  fDest

copy_prog:
        lodsb
        stosb
        or      al, al
        jz      all_done
        inc     cx
        jmp     copy_prog
        
all_done:

        xchg    ax, cx

        ret

INSERT_GUILTY_PROGRAM	ENDP
;-----------------------------------------------------------------------------
END
