include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_LOAD_QUICK                      2/1/93                          CAG
;
; $Header:   S:\certlib4\vcs\dblquick.asv   1.4   29 Nov 1994 13:06:50   DALLEE  $
;
;       Load the quick lookup data for fast access from memory.
;
; BOOL WINAPI DBC4_LOAD_QUICK(DBC4_HEADER FAR *Info,WORD FAR *Buffer);
;
; Input:
;       1)  Header of opened DBC file
;       2)  FAR Buffer (size of quick data from header), no checking done
; Output:
;       1)  FALSE (0) Failed to load
;       2)  TRUE (1)  Loaded success
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\dblquick.asv  $
;  
;     Rev 1.4   29 Nov 1994 13:06:50   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:18:42   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:29:58   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.1   20 Sep 1993 02:12:48   DALLEE
;  Fixed so all indexes are now 1 based.  Corrected error with 
;  dec ax   sbb dx,0   which should be  sub ax,1  sbb dx,0  since
;  dec doesn't set CY.
;-----------------------------------------------------------------------------

DBC4_LOAD_QUICK PROC USES XCX SDS XSI,
        lpInfo:LPSTR,
        lpBuffer:LPSTR

        LDS_SI  lpInfo

        INVOKE  Seek_to_quick_entry,lpInfo,1

        mov     cx,word ptr [XSI].DBC4_HEADER.IndexSize
        shl     cx,1                    ; Multiply by two
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.FileHandle,lpBuffer,cx
        cmp     ax,-1
        je      J_Error
        cmp     ax,cx
        jne     J_Error
        mov     ax,TRUE
        jmp     J_Done
J_Error:
        mov     ax,FALSE
J_Done:
        ret

DBC4_LOAD_QUICK ENDP
;-----------------------------------------------------------------------------
END
