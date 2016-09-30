include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

sPER            EQU     6                ; Number of NULL bytes per entry
                                         ; If you ever change this number,
                                         ; there is code below that *effects*
                                         ; a multiply by 6 that needs to be
                                         ; rewritten.

IFDEF   SDOS16

MKDIR_INOC_DB           PROTO NEAR,:LPSTR
sZeroBuf        EQU     32 * sPER        ; multiplied by size per entry
SHIFTN          EQU     5                ; Number of shifts
                                         ; Must be greater than 3
REMAINBITS      EQU     01fh             ; Bits to AND with to get remainder

ELSE

MKDIR_INOC_DB           PROTO FAR,:LPSTR
sZeroBuf        EQU     64 * sPER        ; multiplied by size per entry
SHIFTN          EQU     6                ; Number of shifts
                                         ; Must be greater than 3
REMAINBITS      EQU     03fh             ; Bits to AND with to get remainder

ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_CREATE                          12/31/92                        CAG
;
; $Header:   S:\certlib4\vcs\dbcreat.asv   1.17   29 Nov 1994 13:08:56   DALLEE  $
;
;      Create a HASHED database, and leave open for access.
;
; UINT WINAPI DBC4_CREATE(LPSTR Filename, DWORD IndexSize, WORD ElemSize,
;                         FARPROC CompairFunction(void FAR *,void FAR *),
;                         FARPROC HashFunction(void FAR *),
;                         FARPROC QuickFunction(void FAR *),
;                         DBC4_HEADER FAR *Info);
; Limitation:
;       Index must be smaller than 32k
;
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
; $Log:   S:\certlib4\vcs\dbcreat.asv  $
;  
;     Rev 1.17   29 Nov 1994 13:08:56   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.13.1.0   19 Oct 1993 07:15:58   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.13   12 Oct 1993 16:25:24   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.11.1.1   30 Sep 1993 22:20:04   keith
;  Change all INT 21 calls for Windows and OS/2 Mirrors to CALL DOS3Call.
;  Stub the low level disk accessing so that it doesn't GP under OS/2
;  until the functionality is added.
;  
;     Rev 1.11.1.0   03 Sep 1993 19:30:16   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.11   26 Aug 1993 18:52:38   DALLEE
;  Added fSHARE_DENY_WRITE flag to file open.
;  
;     Rev 1.10   26 Jul 1993 12:10:06   BRADR
;  Reflects changes made to FIL4_OPEN: -1 on error instead of 0.
;  
;     Rev 1.9   18 Jun 1993 17:42:34   CJKUO
;  Initialize Handle
;  
;     Rev 1.8   24 May 1993 16:20:46   CJKUO
;  Changed error checking.  If cannot create, close and erase file.
;  
;     Rev 1.7   24 May 1993 13:33:20   CJKUO
;  Check for write error.
;  
;     Rev 1.6   23 May 1993 19:23:54   CJKUO
;  Before: write 4 bytes of NULL 16K times,
;          write 2 bytes of NULL 16K times
;  
;  		      C: 4 seconds
;          1.44: 40 seconds
;          1.2: 35-40 seconds
;  
;  Now: TSR: write in 192 byte chunks (buffer taken from stack)
;       reg:          384
;  
;       C: instant
;  	    1.44: TSR: 15 seconds, reg: 5 seconds
;       1.2: 35-40 seconds
;  
;       If you change it to 768 or 1536, it'll take: 1, 18, 15 respectively.
;  
;     Rev 1.5   11 May 1993 13:33:28   CGUC
;  Added SDOS16 switch to fix small library problem.
;  
;     Rev 1.4   06 May 1993 16:54:16   CJKUO
;  Now can create directories of any number of levels.
;  
;     Rev 1.3   05 May 1993 21:35:50   CJKUO
;  Kludge addition that forces the creation of *a* directory for
;  the inoculation database.  Assumes, in essense, that the directory
;  will be NCDTREE. - cjk
;  
;  
;     Rev 1.2   19 Apr 1993 14:57:50   DALLEE
;  Fixed 32 bit addition error when finding QuickStart.
;-----------------------------------------------------------------------------
DBC4_CREATE PROC USES SDS SES XCX XDI XSI,
        Filename:LPSTR,
        IndexSize:DWORD,
        ElemSize:WORD,
        CompairFunction:LPSTR,
        HashFunction:LPSTR,
        QuickFunction:LPSTR,
        Info:LPSTR

        LOCAL Handle:WORD
        LOCAL ZeroBuf[sZeroBuf]:BYTE   ; A buffer of zeros

        mov     Handle,0
        INVOKE  MKDIR_INOC_DB,Filename
        INVOKE  FIL4_OPEN,Filename,fCREATE OR fSHARE_DENY_WRITE
        cmp     ax, -1
        jz      J_Error
        mov     Handle,ax

        LDS_SI  Info
; Save handle 
        mov     [XSI].DBC4_HEADER.FileHandle,ax

; Save free pointer
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer+2,0
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer,0

; Save element size
        mov     ax,ElemSize
        mov     [XSI].DBC4_HEADER.Elem_Size,ax

; Save count of index
        mov     dx,word ptr IndexSize+2
        mov     ax,word ptr IndexSize
        mov     word ptr [XSI].DBC4_HEADER.IndexSize+2,dx
        mov     word ptr [XSI].DBC4_HEADER.IndexSize,ax

; Save start of Data
        mov     cx,4                    ; Size of DWORD
        MUL32   cx
        mov     word ptr [XSI].DBC4_HEADER.DataStart+2,dx
        mov     word ptr [XSI].DBC4_HEADER.DataStart,ax
        mov     dx,word ptr IndexSize+2
        mov     ax,word ptr IndexSize
        mov     cx,2
        MUL32   cx                      ; Size of WORD
        add     word ptr [XSI].DBC4_HEADER.DataStart,ax
        adc     word ptr [XSI].DBC4_HEADER.DataStart+2,dx
        add     word ptr [XSI].DBC4_HEADER.DataStart,sDBC4_HEADER
        adc     word ptr [XSI].DBC4_HEADER.DataStart+2,0

; Save Compare function
        mov     ax,word ptr CompairFunction
        mov     dx,word ptr CompairFunction+2
        mov     word ptr [XSI].DBC4_HEADER.Comp_Func,ax
        mov     word ptr [XSI].DBC4_HEADER.Comp_Func+2,dx

; Save Hash function
        mov     ax,word ptr HashFunction
        mov     dx,word ptr HashFunction+2
        mov     word ptr [XSI].DBC4_HEADER.Hash_Func,ax
        mov     word ptr [XSI].DBC4_HEADER.Hash_Func+2,dx

; Save Quick function
        mov     ax,word ptr QuickFunction
        mov     dx,word ptr QuickFunction+2
        mov     word ptr [XSI].DBC4_HEADER.Quick_Func,ax
        mov     word ptr [XSI].DBC4_HEADER.Quick_Func+2,dx

; Save Index start
        mov     word ptr [XSI].DBC4_HEADER.IndexStart+2,0
        mov     word ptr [XSI].DBC4_HEADER.IndexStart,sDBC4_HEADER

; Save Quick start
        mov     dx,word ptr IndexSize+2
        mov     ax,word ptr IndexSize
        mov     cx,4
        MUL32   cx                      ; Size of DWORD
        add     ax,sDBC4_HEADER
        adc     dx,0
        mov     word ptr [XSI].DBC4_HEADER.QuickStart+2,dx
        mov     word ptr [XSI].DBC4_HEADER.QuickStart,ax

; Save total element size
        mov     ax,ElemSize
        add     ax,sCHAIN_TYPE
        mov     [XSI].DBC4_HEADER.Total_Elem_Size,ax

; Write header to file
        INVOKE  FIL4_WRITE,Handle,Info,sDBC4_HEADER
        cmp     ax,sDBC4_HEADER
        jne     J_Error

; Write NULLs into buffer
        mov     XCX,sZeroBuf
        xor	XAX,XAX
        cld
        SMOV    es,ss                   ; Setup LOCAL variables
        lea     XDI,ZeroBuf
        rep     stosb

; Write NULLs to file
        mov     cx,word ptr IndexSize
        and     XCX,REMAINBITS
        jz      NoRemainder

        shl     XCX,1                   ; Multiply ax by 6 (sPER)
        mov     XAX,XCX
        shl     XCX,1
        add     XCX,XAX

        INVOKE  FIL4_WRITE,Handle,ADDR ZeroBuf,XCX
        cmp     ax,cx
        jne     J_Error
NoRemainder:
        mov     ax,word ptr IndexSize
        mov     cl,SHIFTN
        shr     ax,cl
        mov     cx,ax
WriteNulls:
        INVOKE  FIL4_WRITE,Handle,ADDR ZeroBuf,sZeroBuf
        cmp     ax,sZeroBuf
        jne     J_Error
        loop    WriteNulls

        mov     ax,TRUE
J_Done:
        ret
J_Error:
        cmp     Handle,0
        je      J_NoFile
        INVOKE  FIL4_CLOSE,Handle
        INVOKE  FIL4_DELETE,Filename
J_NoFile:
        mov     ax,FALSE
        jmp     J_Done

DBC4_CREATE ENDP
;-----------------------------------------------------------------------------


MKDIR_INOC_DB PROC USES XBX XCX XDX SDS SES XDI XSI,
        Filename:LPSTR

        LDS_SI  Filename
        mov     XDX,XSI                 ; Put original SI in DX
        cld
        add     XSI,3                   ; Move past "Drive:\"
        mov     ax,word ptr '\'         ; load ah with 0, al = backslash
J_KeepScanning:
        cmp     [XSI],byte ptr al       ; Is it a \
        je      J_FoundSlash
        cmp     [XSI],byte ptr ah       ; Is it a NULL?
        je      J_FoundNull
        inc     XSI
        jmp     J_KeepScanning

J_FoundSlash:
        mov     [XSI],byte ptr ah       ; Swap out \ with Null
        push    XDX                     ; Just in case DX is destroyed by
                                        ; INT 21
        push    XSI                     ; Save location of replacement
        mov     ah,byte ptr 039h        ; MKDIR
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        pop     XSI
        mov     ax,word ptr '\'         ; load ah with 0, al = backslash
        mov     [XSI], byte ptr al      ; put the \ back
        inc     XSI
        pop     XDX
        jmp     J_KeepScanning

J_FoundNull:
        ret
MKDIR_INOC_DB ENDP
;-----------------------------------------------------------------------------
END


