include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_TEST                            1/29/93                         CAG
;
;       Test if the Novell network shell is up and running.
;
; BOOL WINAPI NOV4_TEST(VOID);
;
; Output:
;       1)  TRUE (1) Novell Network found to be running
;       2)  FALSE (0) No Novell Network
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_TEST PROC USES SES XDI

        mov     ax,7A00h
        int     2Fh
        or      al,al
        jz      NoIPX
        mov     XAX,TRUE
        jmp     J_Done
NoIPX:
        mov     XAX,FALSE
J_Done:
        ret

NOV4_TEST ENDP
;-----------------------------------------------------------------------------
END
