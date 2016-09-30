include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_ABSOLUTE_TO_LOGICAL             1/8/93                          CAG
;
;       Convert an ABSOLUTE disk sector to a LOGICAL sector.
;
; DWORD WINAPI DIS4_ABSOLUTE_TO_LOGICAL(WORD Cylinder,BYTE Sector,
;                               BYTE Head,BYTE Drive);
;
; Input:
;       1)  Cylinder 0-? 
;       2)  Sector  1-?
;       3)  Head   0-?
;       4)  Drive  (80h = C:)
; Output:
;       1)  ERROR (-1)
;       2)  Logical Sector number
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; Note:
;       ASB = CYL/SEC/HEAD
;       LS  = SECTOR of calculated ABS
;               (Sector-1)+(Head-1)*SECperTRK + Cyl*SECperTRK*Heads
;
;-----------------------------------------------------------------------------
DIS4_ABSOLUTE_TO_LOGICAL PROC USES XBX XCX,
        Cylinder:WORD,
        Sector:BYTE,
        Head:BYTE,
        Drive:BYTE

        LOCAL   nStatus:DWORD

        mov     word ptr nStatus+2,0
        mov     word ptr nStatus,0

; SECTOR = (SECTOR-1)+(HEAD-1)*SECPERTRACK+CYL*SECPERTRACK*HEADPERDISK
; Get sector information on drive in question
        mov     ah,08h
        mov     dl,Drive
        int     13h                     ; Get Current Drive Parameters
        jc      J_ERROR

; DH = Heads
; CL = Sector
; CH+2bits from CL = Cylinders

        and     cl,00111111b

; (SECTOR - 1)
        mov     al,Sector               ; Sector to convert
        xor     ah,ah
        dec     al
        mov     word ptr nStatus,ax     ; SECTOR - 1

; (HEAD - 1)*SecPerTrack
        push    dx                      ; Save heads
        mov     al,cl
        mov     dh,Head
        dec     dh
        imul    dh
        add     word ptr nStatus,ax     ; HEAD*SECPERTRACK
        adc     word ptr nStatus+2,0
        pop     dx

; (CYL*SecPerTrack*Heads)
        mov     ax,Cylinder
        inc     dh
        mul     dh                      ; Total heads
        xor     ch,ch
        mul     cx                      ; Total sectors
        add     word ptr nStatus,ax     ; CYL*SECPERTRACK*HEADPERDISK
        adc     word ptr nStatus+2,dx
        mov     ax,word ptr nStatus
        mov     dx,word ptr nStatus+2
        jmp     J_Done

J_ERROR:
        mov     dx,-1
        mov     ax,-1
J_Done:

        ret


DIS4_ABSOLUTE_TO_LOGICAL ENDP
;-----------------------------------------------------------------------------
END
