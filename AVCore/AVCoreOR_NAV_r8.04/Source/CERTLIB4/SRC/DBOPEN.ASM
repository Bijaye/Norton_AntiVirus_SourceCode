include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_OPEN                           12/31/92                        CAG
;
; $Header:   S:\certlib4\vcs\dbopen.asv   1.10   29 Nov 1994 13:08:40   DALLEE  $
;
;      Open a HASHED database.
;
; UINT WINAPI DBC4_OPEN(LPSTR Filename, DWORD IndexSize, WORD ElemSize,
;                       FARPROC CompairFunction(void FAR *,void FAR *),
;                       FARPROC HashFunction(void FAR *),
;                       FARPROC QuickFunction(void FAR *),
;                       DBC4_HEADER FAR *Info,
;                       BOOL OpenWrite);
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
;       8)  OpenWrite: TRUE if you want to write to the database.
; Output:
;       1)  UINT = FALSE(0) Database not found or error.
;                  TRUE(1)  Database found
;
; LIBRARY:{INTEL}.LIB
;
; Assembly code was modeled from code Carey wrote in 'C'
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\dbopen.asv  $
;  
;     Rev 1.10   29 Nov 1994 13:08:40   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.7.1.1   01 Nov 1993 15:40:14   MFALLEN
;  Added a global flag that is being checked to see if Ansi to Oem and vice versa
;  conversion should be done on the filename prior to processing.
;  
;     Rev 1.7.1.0   19 Oct 1993 07:17:36   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.7   12 Oct 1993 16:28:28   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.6.1.1   11 Oct 1993 18:54:48   MFALLEN
;  Function expects ANSI filename under Windows.
;  
;     Rev 1.6.1.0   03 Sep 1993 19:27:28   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.6   26 Aug 1993 18:54:26   DALLEE
;  Added fSHARE_DENY_??? flags to file opens.
;  Error out if DBC4_HEADER can't be read from file.
;-----------------------------------------------------------------------------

DBC4_OPEN PROC USES SDS XSI,
        Filename:LPSTR,
        IndexSize:DWORD,
        ElemSize:WORD,
        CompairFunction:LPSTR,
        HashFunction:LPSTR,
        QuickFunction:LPSTR,
        Info:LPSTR,
        OpenWrite:WORD

        LOCAL Handle:WORD

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

        cmp     OpenWrite,TRUE
        je      J_OpenReadWrite
        INVOKE  FIL4_OPEN,Filename,fREAD OR fSHARE_DENY_NONE
        jmp     J_Opened
J_OpenReadWrite:
        INVOKE  FIL4_ATTR,Filename,Handle,GET_INFO
        cmp     ax,-1
        je      J_Error
        mov     Handle,ax
        INVOKE  FIL4_ATTR,Filename,Handle,SET_INFO
        cmp     ax,-1
        je      J_Error
        INVOKE  FIL4_OPEN,Filename,fREAD_WRITE OR fSHARE_DENY_WRITE
J_Opened:
        cmp     ax, -1
        jz      J_Error
        mov     Handle,ax

; Read the header into memory
        INVOKE  FIL4_READ,ax,Info,sDBC4_HEADER
        cmp     ax, sDBC4_HEADER
        je      J_GoodDatabase

;&? Invalid Database.  [Enh] add CRC or checksum to validate database.
        INVOKE  FIL4_CLOSE,Handle
        INVOKE  FIL4_DELETE,Filename
        jmp     J_Error

J_GoodDatabase:
; Setup the important data in the structure
        LDS_SI  Info
        mov     ax,Handle               ; Handle to use for file access
        mov     [XSI].DBC4_HEADER.Filehandle,ax

        mov     ax,word ptr CompairFunction
        mov     dx,word ptr CompairFunction+2
        mov     word ptr [XSI].DBC4_HEADER.Comp_Func,ax
        mov     word ptr [XSI].DBC4_HEADER.Comp_Func+2,dx

        mov     ax,word ptr HashFunction
        mov     dx,word ptr HashFunction+2
        mov     word ptr [XSI].DBC4_HEADER.Hash_Func,ax
        mov     word ptr [XSI].DBC4_HEADER.Hash_Func+2,dx

        mov     ax,word ptr QuickFunction
        mov     dx,word ptr QuickFunction+2
        mov     word ptr [XSI].DBC4_HEADER.Quick_Func,ax
        mov     word ptr [XSI].DBC4_HEADER.Quick_Func+2,dx

        mov     ax,TRUE
        jmp     J_Done
J_Error:
                                        ; Reset INT 24 vector, DOS only
ifdef DOS16
_CheckErrorFar PROTO FAR C

        INVOKE  _CheckErrorFar
endif
        mov     ax,FALSE
J_Done:
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

DBC4_OPEN ENDP
;-----------------------------------------------------------------------------
END

