include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CHR4_TO_ALT_CASE                8/31/93                        bAd
;
;       Converts a character (even above 7fh) to uppercase or lowercase.
;
; BYTE WINAPI CHR4_TO_ALT_CASE(BYTE bChar,BYTE fCase);
;
; Input:
;       1)  Character to convert.
;       2)  Case to change character to:
;               CHR_UPPER - make uppercase
;               CHR_LOWER - make lowercase
;
; Output:
;       1)  BYTE - modified character
;
; LIBRARY:{INTEL}.LIB
;
; NOTE:  Characters that are above 7fh, and are uppercase, will NOT be
;        converted to lowercase.  This function uses an interrupt to get
;        the uppercase table, but there is no interrupt to get a lowercase
;        table.
;
;-----------------------------------------------------------------------------
CHR4_TO_ALT_CASE PROC USES XBX XCX XDX XSI XDI SDS SES,
        bChar:BYTE,
        fCase:BYTE

        LOCAL   bModifyChar:BYTE
        LOCAL   bCountryInfo[5]:BYTE

        mov     al, bChar
        mov     bModifyChar, al          


        cmp     fCase, CHR_UPPER         ; determine what case to conver to
        jz      ConvertToUpper

ConvertToLower:
        cmp     bChar, 'A'
        jb      Done                    ; skip if not a letter
        cmp     bChar, 'Z'
        jbe     MakeLower               ; it's uppercase, make lower

        jmp     CheckExt                ; check for extended character

ConvertToUpper:
        cmp     bChar, 'a'              ; skip if below the lowercase letters
        jb      Done
        cmp     bChar, 'z'
        jbe     MakeUpper               ; make lower if upper

CheckExt:
        test    bChar, 80h              ; look for extended ASCII
        jz      Done                    ; it was not extended

Extended:
        ;---- must be DOS 3.30 or higher
        mov     ax, 3001h
        int     21h                     ; get dos version number
        xchg    ah, al                  ; put in Major.Minor order
        cmp     ax, 031Eh
        jb      Done

        ;---- get uppercase table
        ; ES:DI will point to 5 byte table: 1 byte ID, 4 bytes (SEG:OFF)
        ; pointer to lookup table
        mov     ax, ss
        mov     SES, ax
        lea     XDI, bCountryInfo
        mov     ax, 6502h
        mov     bx, -1                  ; current code page
        mov     cx, 5                   ; size of buffer
        mov     dx, -1                  ; current country
        int     21h     
        jc      Done                    ; Error

        mov     ax, ss
        mov     SES, ax
        lea     XDI, bCountryInfo
        
        ; table is 130 bytes; two bytes length, 128 bytes uppercase values
        ;
        mov     ax, word ptr XES[XDI+3] ; DS:BX points to table
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL,ax
endif
        mov     SDS, ax
        mov     bx, word ptr XES[XDI+1]
        inc     bx                      ; skip over length
        inc     bx

        mov     al, bChar
        and     al, 07fh                ; clear high bit
        xlat                            ; find the other-case value
        mov     bModifyChar, al
        jmp     Done

MakeLower:
        or      bModifyChar, 00100000b   ; set lowercase bit
        jmp     Done

MakeUpper:
        and     bModifyChar, 11011111b   ; clear lowercase bit
Done:
        mov     al, bModifyChar             
        ret

CHR4_TO_ALT_CASE ENDP
;-----------------------------------------------------------------------------
END
