;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DBOPEN.ASv   1.3   08 Mar 1996 14:56:34   DALLEE  $
;;
;; Description: Defines DBC4_OPEN function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DBOPEN.ASv  $
;  
;     Rev 1.3   08 Mar 1996 14:56:34   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   17 Aug 1994 13:25:38   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   31 May 1994 13:33:22   RSTANEV
;  Fixed a stupid error.
;
;     Rev 1.0   30 May 1994 09:54:10   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_OPEN
;
; Open a HASHED database.
;
; BOOL WINAPI DBC4_OPEN ( LPSTR Filename, DWORD IndexSize, WORD ElemSize,
;                         FARPROC CompareFunction ( void FAR *, void FAR * ),
;                         FARPROC HashFunction ( void FAR * ),
;                         FARPROC QuickFunction ( void FAR * ),
;                         DBC4_HEADER FAR *Info,
;                         BOOL OpenWrite );
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
;       8)  OpenWrite: TRUE if you want to write to the database.
; Output:
;       1)  BOOL = FALSE(0) Database not found or error.
;                  TRUE(1)  Database found
;----------------------------------------------------------------------------
DBC4_OPEN  PROC  CODEDISTANCE  USES  XDX XSI SDS,
        Filename:LPSTR,
        IndexSize:DWORD,
        ElemSize:WORD,
        CompareFunction:LPBYTE,
        HashFunction:LPBYTE,
        QuickFunction:LPBYTE,
        Info:LPBYTE,
        OpenWrite:BOOL

        LOCAL Handle:UINT

        cmp     OpenWrite,FALSE
        jne     J_OpenReadWrite
        INVOKE  FIL4_OPEN,Filename,fREAD OR fSHARE_DENY_NONE
        jmp     SHORT J_Opened
  J_OpenReadWrite:
        INVOKE  FIL4_ATTR,Filename,Handle,GET_INFO
        cmp     XAX,-1
        je      J_Error
        mov     Handle,XAX
        INVOKE  FIL4_ATTR,Filename,Handle,SET_INFO
        cmp     XAX,-1
        je      J_Error
        INVOKE  FIL4_OPEN,Filename,fREAD_WRITE OR fSHARE_DENY_WRITE
  J_Opened:
        cmp     XAX,-1
        jz      J_Error
        mov     Handle,XAX

                                        ; Read the header into memory
        INVOKE  FIL4_READ,XAX,Info,sDBC4_HEADER
        cmp     XAX,sDBC4_HEADER
        je      SHORT J_GoodDatabase

                                        ; &? Invalid Database.  [Enh] add CRC
                                        ; or checksum to validate database.
        INVOKE  FIL4_CLOSE,Handle
        INVOKE  FIL4_DELETE,Filename
        jmp     SHORT J_Error

  J_GoodDatabase:
                                        ; Setup the important data in the
                                        ; structure
        LDS_SI  Info
        mov     XAX,Handle
        mov     [XSI].DBC4_HEADER.Filehandle,XAX

IF OS_16
        mov     ax,WORD PTR CompareFunction
        mov     dx,WORD PTR CompareFunction+2
        mov     WORD PTR [XSI].DBC4_HEADER.Comp_Func,ax
        mov     WORD PTR [XSI].DBC4_HEADER.Comp_Func+2,dx

        mov     ax,WORD PTR HashFunction
        mov     dx,WORD PTR HashFunction+2
        mov     WORD PTR [XSI].DBC4_HEADER.Hash_Func,ax
        mov     WORD PTR [XSI].DBC4_HEADER.Hash_Func+2,dx

        mov     ax,WORD PTR QuickFunction
        mov     dx,WORD PTR QuickFunction+2
        mov     WORD PTR [XSI].DBC4_HEADER.Quick_Func,ax
        mov     WORD PTR [XSI].DBC4_HEADER.Quick_Func+2,dx
ELSE
        mov     eax,CompareFunction
        mov     [XSI].DBC4_HEADER.Comp_Func,eax
        mov     eax,HashFunction
        mov     [XSI].DBC4_HEADER.Hash_Func,eax
        mov     eax,QuickFunction
        mov     [XSI].DBC4_HEADER.Quick_Func,eax
ENDIF
        mov     XAX,TRUE
        jmp     SHORT J_Done
  J_Error:
                                        ; Reset INT 24 vector, DOS only
IFDEF DOS16
        _CheckErrorFar  PROTO  FAR  C

        INVOKE  _CheckErrorFar
ENDIF
        xor     XAX,XAX
  J_Done:
        ret

DBC4_OPEN  ENDP

UniqueSegmentEnd
END
