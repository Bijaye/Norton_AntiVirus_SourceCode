;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/dbgetent.asv   1.4   08 Mar 1996 14:28:22   DALLEE  $
;;
;; Description: Defines DBC4_GET_NUM_ENTRIES function.
;;              Defines DBC4_GET_ENTRY function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/dbgetent.asv  $
;  
;     Rev 1.4   08 Mar 1996 14:28:22   DALLEE
;  Fixed OS_16 32-bit division bug.
;  Was using remainder as high word of result.
;  
;     Rev 1.3   05 Apr 1995 13:48:16   DALLEE
;  Fixed bug for 32 bit platforms.
;  Calling INVOKE with a WORD in place of a DWORD parameter assembles to
;   PUSH dword ptr 0, PUSH word ptr WORD.  A total of 6 bytes, not the
;   expected 4.
;  This is bad.
;  
;     Rev 1.2   18 Aug 1994 23:29:06   RSTANEV
;  Fixed SYM_VXD compilation errors.
;
;     Rev 1.1   17 Aug 1994 13:25:36   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.0   30 May 1994 11:14:32   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

OPTION SCOPED

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_GET_NUM_ENTRIES
;
; Return the number of entries in the database, including deleted/blank
; entries.
;
; DWORD WINAPI DBC4_GET_NUM_ENTRIES ( DBC4_HEADER FAR *Info );
;
; Input:
;       1)  DBC4_HEADER structure for database header.
; Output:
;       1)  DWORD = Number of entries in the database.
;----------------------------------------------------------------------------
DBC4_GET_NUM_ENTRIES  PROC  CODEDISTANCE  USES XBX XCX SDS XSI,
        Info:LPBYTE

        LDS_SI  Info
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,0,fSEEK_END
IF OS_16
        sub     ax,word ptr [XSI].DBC4_HEADER.DataStart
        sbb     dx,word ptr [XSI].DBC4_HEADER.DataStart+2
        movuw   XCX,[XSI].DBC4_HEADER.Total_Elem_Size
        jcxz    J_DivideByZero
                                        ;=========================
                                        ; Perform 32-bit division...
        mov     bx,ax                   ; Save low word
        mov     ax,dx                   ; Setup high word division
        xor     dx,dx
        div     cx

        xchg    bx,ax                   ; Save high word of quotient in BX
                                        ; and setup low word division.
        div     cx                      ; DX = high remainder already.

        mov     dx,bx                   ; DX::AX now 32-bit quotient
                                        ;=========================
ELSE
        sub     eax,[XSI].DBC4_HEADER.DataStart
        xor     edx,edx
        movuw   XCX,[XSI].DBC4_HEADER.Total_Elem_Size
        jecxz   J_DivideByZero
        div     ecx
ENDIF
        jmp     SHORT J_Done

J_DivideByZero:
        xor     XAX,XAX
IF OS_16
        xor     dx,dx
ENDIF

J_Done:
        ret

DBC4_GET_NUM_ENTRIES  ENDP

;----------------------------------------------------------------------------
; DBC4_GET_ENTRY
;
; Retrieves the specified database entry and writes it to the
; buffer provided.
;
;
; BOOL WINAPI DBC4_GET_ENTRY ( DBC4_HEADER FAR *Info,
;                              DWORD Num,
;                              void FAR *Buffer );
;
; Input:
;       1)  Info  :  structure for database header.
;       2)  Num   :  number of database entry to retrieve.
;                    NOTE: Num = 1 to retrieve the first entry.
;       3)  Buffer:  buffer to receive the entry.
; Output:
;       1)  BOOL = FALSE(0) Could not retrieve entry.
;                  TRUE(1)  Entry found.
;----------------------------------------------------------------------------
DBC4_GET_ENTRY  PROC  CODEDISTANCE  USES SDS XSI,
        Info:LPBYTE,
        Num:DWORD,
        Buffer:LPBYTE

        LDS_SI  Info
        INVOKE  Seek_to_data_record,Info,Num

        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,Buffer,XAX

        cmp     ax,[XSI].DBC4_HEADER.Elem_Size
        jne     J_ReadError

        mov     XAX,TRUE
        jmp     SHORT J_Done

J_ReadError:
        movuw   XAX, [XSI].DBC4_HEADER.Elem_Size
        INVOKE  MEM4_SET,Buffer,0,XAX
        xor     XAX,XAX

J_Done:
        ret

DBC4_GET_ENTRY  ENDP

UniqueSegmentEnd
END
