;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/dbcreat.asv   1.10   25 Jun 1996 10:47:02   MZAREMB  $
;;
;; Description: Defines DBC4_CREATE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/dbcreat.asv  $
;  
;     Rev 1.10   25 Jun 1996 10:47:02   MZAREMB
;  Reloaded AX with backslash since DBCS_ANSINext was changing it.
;  
;     Rev 1.9   10 May 1996 20:44:38   YVANG
;  DBCS enable.
;  
;     Rev 1.8   08 Mar 1996 14:56:32   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.7   06 Jun 1995 18:03:32   SZIADEH
;  ifdefd out MKDIR_INOC_DB for VXD platform. the func for the vxd plat can 
;  be found in MKINOCD.CPP
;  
;     Rev 1.6   27 Apr 1995 01:58:58   SZIADEH
;  took out the previous modifications.
;  
;     Rev 1.4   18 Apr 1995 21:33:32   DALLEE
;  Fixed 32-bit conversion bug - MUL 32 macro is only defined for OS_16.
;  
;     Rev 1.3   22 Mar 1995 11:29:24   SZIADEH
;  added UNC support.
;  
;     Rev 1.2   18 Aug 1994 22:49:02   RSTANEV
;  Fixed a compilation error for VxD platform.
;
;     Rev 1.1   17 Aug 1994 13:25:34   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.0   30 May 1994 10:45:36   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

sPER            EQU     6                ; Number of NULL bytes per entry
                                         ; If you ever change this number,
                                         ; there is code below that *effects*
                                         ; a multiply by 6 that needs to be
                                         ; rewritten.

MKDIR_INOC_DB  PROTO  CODEDISTANCE,:LPSTR

IF bSYM_TSR

  sZeroBuf    EQU  32 * sPER            ; multiplied by size per entry
  SHIFTN      EQU  5                    ; Number of shifts
                                        ; Must be greater than 3
  REMAINBITS  EQU  01Fh                 ; Bits to AND with to get remainder

ELSE

  sZeroBuf    EQU  64 * sPER            ; multiplied by size per entry
  SHIFTN      EQU  6                    ; Number of shifts
                                        ; Must be greater than 3
  REMAINBITS  EQU  03Fh                 ; Bits to AND with to get remainder

ENDIF

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_CREATE
;
; Create a HASHED database, and leave open for access.
;
; BOOL WINAPI DBC4_CREATE ( LPSTR Filename, DWORD IndexSize, WORD ElemSize,
;                           FARPROC CompareFunction (void FAR *, void FAR *),
;                           FARPROC HashFunction (void FAR *),
;                           FARPROC QuickFunction (void FAR *),
;                           DBC4_HEADER FAR *Info );
; Limitation:
;       Index must be smaller than 32k
;
; Input:
;       1)  Filename - Name of database to open.
;       2)  IndexSize - size of its index       - Not used
;       3)  ElemSize - size of a single record  - Not used
;       4)  CompareFunction - Routine that will compare the two items
;                             and return if they are the same.
;       5)  HashFunction - Returns the Hash value of the inputed string.
;                          Example - CRC.
;       6)  QuickFunction - Returns a WORD that will be placed in the
;                           quick look up table.
;       7)  DBC4_HEADER structure for database header.
; Output:
;       1)  BOOL = FALSE(0) Database not found or error.
;                  TRUE(1)  Database found
;----------------------------------------------------------------------------
DBC4_CREATE  PROC  CODEDISTANCE  USES XCX XDI XSI SDS SES,
        Filename:LPSTR,
        IndexSize:DWORD,
        ElemSize:WORD,
        CompareFunction:LPBYTE,
        HashFunction:LPBYTE,
        QuickFunction:LPBYTE,
        Info:LPBYTE

        LOCAL   Handle:UINT
        LOCAL   ZeroBuf[sZeroBuf]:BYTE  ; A buffer of zeros

        mov     Handle,0

        INVOKE  MKDIR_INOC_DB,Filename
        cmp     XAX, -1
        jz      J_Error

        INVOKE  FIL4_OPEN,Filename,fCREATE OR fSHARE_DENY_WRITE
        cmp     XAX,-1
        jz      J_Error
        mov     Handle,XAX

        LDS_SI  Info
                                        ; Save handle
        mov     [XSI].DBC4_HEADER.FileHandle,XAX

                                        ; Save free pointer
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer+2,0
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer,0

                                        ; Save element size
        mov     ax,ElemSize
        mov     [XSI].DBC4_HEADER.Elem_Size,ax

                                        ; Save count of index
IF OS_16
        mov     dx,word ptr IndexSize+2
        mov     ax,word ptr IndexSize
        mov     word ptr [XSI].DBC4_HEADER.IndexSize+2,dx
        mov     word ptr [XSI].DBC4_HEADER.IndexSize,ax
ELSE
        mov     eax,IndexSize
        mov     [XSI].DBC4_HEADER.IndexSize,eax
ENDIF

                                        ; Save start of Data
        mov     XCX,6                   ; Size of DWORD + WORD
IF OS_16
        MUL32   cx
        mov     word ptr [XSI].DBC4_HEADER.DataStart+2,dx
        mov     word ptr [XSI].DBC4_HEADER.DataStart,ax
        add     word ptr [XSI].DBC4_HEADER.DataStart,sDBC4_HEADER
        adc     word ptr [XSI].DBC4_HEADER.DataStart+2,0
ELSE
        mul     ecx
        mov     [XSI].DBC4_HEADER.DataStart,eax
        add     [XSI].DBC4_HEADER.DAtaStart,sDBC4_HEADER
ENDIF

                                        ; Save Compare function
        mov     ax,word ptr CompareFunction
        mov     dx,word ptr CompareFunction+2
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
        mov     XCX,4                   ; Size of DWORD
IF OS_16
        mov     dx,word ptr IndexSize+2
        mov     ax,word ptr IndexSize
        MUL32   cx
        add     ax,sDBC4_HEADER
        adc     dx,0
        mov     word ptr [XSI].DBC4_HEADER.QuickStart+2,dx
        mov     word ptr [XSI].DBC4_HEADER.QuickStart,ax
ELSE
        mov     eax,IndexSize
        mul     ecx
        add     eax,sDBC4_HEADER
        mov     [XSI].DBC4_HEADER.QuickStart,eax
ENDIF

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
        xor     XAX,XAX
        cld
        SMOV    es,ss                   ; Setup LOCAL variables
        lea     XDI,ZeroBuf
        rep     stosb

                                        ; Write NULLs to file
        movuw   XCX,<word ptr IndexSize>
        and     XCX,REMAINBITS
        jz      NoRemainder

        shl     XCX,1                   ; Multiply ax by 6 (sPER)
        mov     XAX,XCX
        shl     XCX,1
        add     XCX,XAX

        INVOKE  FIL4_WRITE,Handle,ADDR ZeroBuf,XCX
        cmp     XAX,XCX
        jne     J_Error
  NoRemainder:
        mov     ax,word ptr IndexSize
        mov     cl,SHIFTN
        shr     ax,cl
        mov     cx,ax
  WriteNulls:
        INVOKE  FIL4_WRITE,Handle,ADDR ZeroBuf,sZeroBuf
        cmp     XAX,sZeroBuf
        jne     J_Error
        loop    WriteNulls

        mov     XAX,TRUE
        jmp     SHORT J_Done
J_Error:
        cmp     Handle,0
        je      J_NoFile
        INVOKE  FIL4_CLOSE,Handle
        INVOKE  FIL4_DELETE,Filename
J_NoFile:
        xor     XAX,XAX
J_Done:
        ret
DBC4_CREATE  ENDP

;----------------------------------------------------------------------------
; ???
;----------------------------------------------------------------------------
; The code for the VxD platform is in mkinocd.cpp

IF bSYM_VXD EQ FALSE
MKDIR_INOC_DB  PROC  CODEDISTANCE  USES SDS XSI XDI XDX,
      Filename:LPSTR

      LDS_SI  Filename
      mov     XDI,XSI                 ; Save original SI in DI

      INVOKE  IsUNCName, FileName
      cmp     XAX, 0                  ; 0 implies NO UNC
      je      J_MKDIR_NoUNC

      INVOKE  UNC_SkipServerVolumeName, FileName
      cmp     XAX, -1
      je      J_MKDIR_ERROR

      inc     XAX                     ; skip first '\' in "\path\filename"
      mov     XSI, XAX
      jmp     J_MKDIR_INOC

J_MKDIR_NoUNC:
      add     XSI,3                   ; Move past "Drive:\"

J_MKDIR_INOC:
      cld
J_KeepScanning:
      mov     ax,WORD PTR '\'         ; load ah with 0, al = backslash
      cmp     [XSI],BYTE PTR al       ; Is it a \
                                      ; YVANG: Since it started at valid
                                      ;  DBCS boundary, DBCS_AnsiNext()
                                      ;  will be handle pointer increment
                                      ;  properly.
      je      J_FoundSlash
      cmp     [XSI],BYTE PTR ah       ; Is it a NULL?
      je      J_FoundNull
ScanNextChar:
      INVOKE  DBCS_AnsiNext,vSDS XSI
      MOV_DS  dx
      mov     XSI,XAX
      jmp     J_KeepScanning

J_FoundSlash:
      mov     [XSI],BYTE PTR ah       ; Swap out \ with Null
      INVOKE  FIL4_CREATE_DIR,vSDS XDI
      mov     ax,WORD PTR '\'
      mov     [XSI],BYTE PTR al       ; put the \ back
      jmp     SHORT ScanNextChar

J_MKDIR_ERROR:
      mov     XAX, -1

J_FoundNull:
      ret

MKDIR_INOC_DB  ENDP
;----------------------------------------------------------------------------
ENDIF

UniqueSegmentEnd
END
