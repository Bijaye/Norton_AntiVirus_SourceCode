include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_OPEN_CREATE                     12/31/92                        CAG
;
;      Open a HASHED database, create if not found.
;
; UINT WINAPI DBC4_OPEN_CREATE(LPSTR Filename, DWORD IndexSize, WORD ElemSize,
;                       FARPROC CompairFunction(void FAR *,void FAR *),
;                       FARPROC HashFunction(void FAR *),
;                       FARPROC QuickFunction(void FAR *),
;                       DBC4_HEADER FAR *Info);
; Input:
;       1)  Filename - Name of database to open.
;       2)  IndexSize - size of its index       - Not used
;       3)  ElemSize - size of a single record  - Not used
;       4)  CompairFunction - Routine that will compair the two items
;                             and return if they are the same.
;       5)  HashFunction - Returns the Hash value of the inputed string.
;                          Example - CRC.
;       6)  QuickFunction - Returns a WORD that will be placed in the
;                           quick look up table.
;       7)  DBC4_HEADER structure for database header.
; Output:
;       1)  UINT = FALSE(0) Database not found or error.
;                  TRUE(1)  Database found
;
; LIBRARY:{INTEL}.LIB
;
; Assembly code was modeled from code Carey wrote in 'C'
;-----------------------------------------------------------------------------
DBC4_OPEN_CREATE PROC USES SDS XSI,
        Filename:LPSTR,
        IndexSize:DWORD,
        ElemSize:WORD,
        CompairFunction:LPSTR,
        HashFunction:LPSTR,
        QuickFunction:LPSTR,
        Info:LPSTR

        LOCAL Attrib:WORD
                                        ; ------------------------------------
                                        ; Convert from Ansi to Oem
                                        ; ------------------------------------
IFDEF   WIN16
        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  ANSITOOEM, Filename, Filename
@@:
ENDIF

                                        ; Check if exists
        INVOKE  FIL4_ATTR,Filename,Attrib,GET_INFO
        cmp     ax,-1                   ; -1 is "doesn't exist"
        jne     J_Exists

                                        ; If doesn't exist, create
        INVOKE  DBC4_CREATE,Filename,IndexSize,ElemSize,CompairFunction,
                          HashFunction,QUickFunction,Info
        jmp     J_Done

J_Exists:
        mov     Attrib,ax

                                        ; Check if write protected
        INVOKE  FIL4_ATTR,Filename,Attrib,SET_INFO
        cmp     ax,-1                   ; -1 is unable to write
        mov     ax,FALSE                ; If write protected, forget it
        je      J_Done

                                        ; If not write protected and exists,
                                        ; open for R/W
        INVOKE  DBC4_OPEN,Filename,IndexSize,ElemSize,CompairFunction,
                          HashFunction,QuickFunction,Info,TRUE

J_Done:

                                        ; Reset INT 24 vector, DOS only
ifdef DOS16
_CheckErrorFar PROTO FAR C

        push    ax
        INVOKE  _CheckErrorFar
        pop     ax
endif
                                        ; ------------------------------------
                                        ; Convert from Oem to Ansi
                                        ; ------------------------------------
IFDEF   WIN16
        push    ax

        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  OEMTOANSI, Filename, Filename
@@:
        pop     ax
ENDIF

        ret

DBC4_OPEN_CREATE ENDP
;-----------------------------------------------------------------------------
END
