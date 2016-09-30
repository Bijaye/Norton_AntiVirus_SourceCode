include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_SEARCH_QUICK                    12/31/92                        CAG
;
;       Search thru the hashed database for a match, but use the QUICK
;       lookup buffer that was loaded.
;
; UINT WINAPI DBC4_SEARCH_QUICK(DBC4_HEADER FAR *Info, void FAR *LookFor,
;                       void FAR *QuickBuffer);
;
; Input:
;       1)  DBC4_HEADER structure for database header.
;       2)  LookFor, what we need to look for.
;       3)  Pointer to the QUICK data in memory.
; Output:
;       1)  UINT = FALSE(0) Entry not found in database
;                  TRUE(1)  Entry found in database
;                  (2)      Not found, look with DBC4_SEARCH
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DBC4_SEARCH_QUICK PROC USES SDS SES XBX XCX XDX XDI XSI,
        Info:LPSTR,
        LookFor:LPSTR,
        lpBuffer:LPSTR

        LOCAL   uTestMe:WORD

        xor     XAX,XAX
        LDS_SI  Info

; Get what we should look for
        INVOKE  DBC4_QUICK PTR [XSI].DBC4_HEADER.Quick_Func,LookFor
        mov     uTestMe,ax

; Get the hash value (CRC) of what we're looking for
        INVOKE  DBC4_HASH PTR [XSI].DBC4_HEADER.Hash_Func,LookFor
        mov     bx,ax
        INVOKE  NUM4_MODULUS,bx,[XSI].DBC4_HEADER.IndexSize

        ; Normally we would "ADD AX,1  ADC DX,0" here if we were going to
        ; access the file.  All Seek_to_xxx functions expect #'s to
        ; be 1 based.  For checking the buffer in memory, we don't want
        ; to do this.   -DALLEE

; Look in the quick lookup buffer now
        LDS_SI  lpBuffer
        add     XSI,XAX
        add     XSI,XAX

        cmp     [XSI],word ptr 0
        je      NotFound
        mov     ax,uTestMe
        cmp     [XSI],ax
        je      PerfectMatch
        mov     ax,2
        jmp     J_Done
NotFound:
        xor     ax,ax
        jmp     J_Done
PerfectMatch:
        mov     ax,1
J_Done:
        ret

DBC4_SEARCH_QUICK ENDP
;-----------------------------------------------------------------------------
END

