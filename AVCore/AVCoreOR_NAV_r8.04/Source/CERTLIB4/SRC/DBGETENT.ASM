; Copyright 1993 Symantec, Peter Norton Product Group
;-----------------------------------------------------------------------------
;
; $Header:   S:\certlib4\vcs\dbgetent.asv   1.4   29 Nov 1994 13:07:34   DALLEE  $
;
; Filename:     dbgetent.asm
;
; Contains:     DBC4_GET_NUM_ENTRIES
;               DBC4_GET_ENTRY
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\dbgetent.asv  $
;  
;     Rev 1.4   29 Nov 1994 13:07:34   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:22:06   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:32:42   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.0   03 Sep 1993 19:29:54   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.1   27 May 1993 13:38:02   DALLEE
;  Check divide by zero.
;  
;     Rev 1.0   25 May 1993 00:44:18   DALLEE
;  Initial revision.
;-----------------------------------------------------------------------------


include CTS4.INC


UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_GET_NUM_ENTRIES
;
;       Return the number of entries in the database, including deleted/blank
;       entries.
;
; DWORD WINAPI DBC4_GET_NUM_ENTRIES(DBC4_HEADER FAR *Info);
;
; Input:
;       1)  DBC4_HEADER structure for database header.
; Output:
;       1)  DWORD = Number of entries in the database.
;
;-----------------------------------------------------------------------------
; 5/24/93 DALLEE, Function created
;-----------------------------------------------------------------------------
DBC4_GET_NUM_ENTRIES PROC USES XCX SDS XSI,
        Info:LPSTR

        LDS_SI  Info
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,0,fSEEK_END
        sub     ax,word ptr [XSI].DBC4_HEADER.DataStart
        sbb     dx,word ptr [XSI].DBC4_HEADER.DataStart+2

        mov     cx,[XSI].DBC4_HEADER.Total_Elem_Size
        cmp     cx,0
        je      J_DivideByZero

        div     cx
        jmp     J_Done

J_DivideByZero:
        xor     ax,ax
        xor     dx,dx

J_Done:
        ret

DBC4_GET_NUM_ENTRIES ENDP


;-----------------------------------------------------------------------------
; DBC4_GET_ENTRY
;
;       Retrieves the specified database entry and writes it to the
;       buffer provided.
;
;
; BOOL WINAPI DBC4_GET_ENTRY(DBC4_HEADER FAR *Info,
;                            DWORD Num,
;                            void FAR *Buffer);
;
; Input:
;       1)  Info  :  structure for database header.
;       2)  Num   :  number of database entry to retrieve.
;                    NOTE: Num = 1 to retrieve the first entry.
;       3)  Buffer:  buffer to receive the entry.
; Output:
;       1)  BOOL = FALSE(0) Could not retrieve entry.
;                  TRUE(1)  Entry found.
;-----------------------------------------------------------------------------
; 5/24/93 DALLEE, Function created
;-----------------------------------------------------------------------------
DBC4_GET_ENTRY PROC USES SDS XSI,
        Info:LPSTR,
        Num:DWORD,
        Buffer:LPSTR

        LDS_SI  Info
        INVOKE  Seek_to_data_record,Info,Num
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,Buffer,
                          [XSI].DBC4_HEADER.Elem_Size

        cmp     ax, [XSI].DBC4_HEADER.Elem_Size
        jne     J_ReadError

        mov     ax, 1
        jmp     J_Done

J_ReadError:
        INVOKE  MEM4_SET,Buffer,0,[XSI].DBC4_HEADER.Elem_Size
        xor     ax,ax

J_Done:
        ret

DBC4_GET_ENTRY ENDP
;-----------------------------------------------------------------------------
END
