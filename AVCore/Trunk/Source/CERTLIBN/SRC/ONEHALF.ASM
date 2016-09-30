;-----------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/onehalf.asv   1.4   27 Jun 1996 17:21:38   DALLEE  $
; repair code for one_half virus.
;-----------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/onehalf.asv  $
;  
;     Rev 1.4   27 Jun 1996 17:21:38   DALLEE
;  LuigiPlus port:
;  Mostly changes to make DX safe.
;  
;     Rev 1.3   08 Mar 1996 14:57:16   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   17 Dec 1994 00:30:42   SZIADEH
;  added UniqueSegmentEnd
;  
;     Rev 1.1   16 Dec 1994 19:45:22   SZIADEH
;  included CTSN.INC
;  
;     Rev 1.0   16 Dec 1994 18:43:36   SZIADEH
;  Initial revision into certlibn
;  
;-----------------------------------------------------------
include  CTSN.INC
include  1HALF.INC

if FARCALL
VerifyData         PROTO FAR,:LPBYTE, :LPBYTE
ProcessDisk        PROTO FAR,:LPBYTE, :LPBYTE
UpdateMBR          PROTO FAR,:LPBYTE, :WORD, :WORD, :BYTE, :BYTE
GetDiskGeometry    PROTO FAR,:LPBYTE, :LPBYTE
repair_mbr         PROTO FAR,:LPBYTE, :LPBYTE
GetDecryptionData  PROTO FAR,:LPBYTE, :LPBYTE
DecryptSector      PROTO FAR,:WORD
display_counter    PROTO FAR,:DWORD
ZeroOutSavedSector PROTO FAR,:LPBYTE, :LPBYTE

else

VerifyData         PROTO NEAR,:LPBYTE, :LPBYTE
ProcessDisk        PROTO NEAR,:LPBYTE, :LPBYTE
UpdateMBR          PROTO NEAR,:LPBYTE, :WORD, :WORD, :BYTE, :BYTE
GetDiskGeometry    PROTO NEAR,:LPBYTE, :LPBYTE
repair_mbr         PROTO NEAR,:LPBYTE, :LPBYTE
GetDecryptionData  PROTO NEAR,:LPBYTE, :LPBYTE
DecryptSector      PROTO NEAR,:WORD
display_counter    PROTO NEAR,:DWORD
ZeroOutSavedSector PROTO NEAR,:LPBYTE, :LPBYTE

endif


UniqueSegment
;-----------------------------------------------------------

IFNDEF DOS16

;-- One Half Repair is NAV.EXE-centric!!  Fail for everyone else.

OneHalfRestoreDisk PROC,
    lpBuffer:LPBYTE

    xor     ax, ax
    ret
OneHalfRestoreDisk ENDP

ELSE
;-- This is the legit repair code...

OneHalfRestoreDisk PROC USES XSI XDI XDX XCX XBX SES SDS,
    lpBuffer:LPBYTE


    LOCAL RestoreInfo:RESTORE_INFO
    ;---------------------------------------

    LES_DI  lpBuffer

    INVOKE  GetDiskGeometry, lpBuffer, ADDR RestoreInfo
    or      ax, ax
    jz      restore_error

    INVOKE  GetDecryptionData, lpBuffer, ADDR RestoreInfo
    or      ax, ax
    jz      restore_error


    INVOKE  VerifyData, lpBuffer, ADDR RestoreInfo
    or      ax, ax
    jz      restore_error

    INVOKE  ProcessDisk, lpBuffer, ADDR RestoreInfo
    or      ax, ax
    jnz     restore_done


restore_error:
    xor     ax, ax
restore_done:
    ret

OneHalfRestoreDisk ENDP

;-----------------------------------------------------------
;-----------------------------------------------------------
VerifyData PROC USES SES SDS,
     lpBuffer:LPBYTE,            ; in
     lpRestoreInfo:LPBYTE        ; in, out

     LOCAL   CylSec:WORD
     LOCAL   MbrOffset:WORD
     LOCAL   Key:WORD
     LOCAL   lpSEG_OFF:LPBYTE
     ;---------------------------

     push    word ptr lpBuffer          ; offset of work buffer
     pop     di                         ; load it up
     add     di, 200h                   ;   point to a new "block"
     mov     word ptr lpSEG_OFF, di     ;   and put this into another ptr
     push    word ptr lpBuffer+2
     pop     word ptr lpSEG_OFF + 2

     ;--------------------
     LES_DI  lpBuffer
     LDS_SI  lpRestoreInfo

     mov     ax, [XSI].RESTORE_INFO.MbrOffset
     mov     MbrOffset, ax

     mov     ax, [XSI].RESTORE_INFO.Key
     mov     Key, ax

     mov     ax, [XSI].RESTORE_INFO.MaxCyl
     sub     ax, 2                  ; (last cylinder - 2)
     xchg    ah, al
     mov     cl, 6
     shl     al, cl

     or      al, [XSI].RESTORE_INFO.MaxSec
     mov     cx, ax
     mov     dh, [XSI].RESTORE_INFO.MaxHead

     mov     dl, 80h                ;
     mov     ax, 0201h
     mov     bx, di

     INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;
     or      ax, ax
     jnz     verify_error

                                    ; now read stored sector and verify!
     mov     ax, 0201h              ; read cyl=0, head=0
     add     bx, 200h               ; sector = SecPerCyl - 9
     xor     dh, dh                 ;
     xor     ch, ch                 ;                                    ;
     and     cl, 03fh               ;
     sub     cl, 8                  ;
     cmp     cl, 2                  ;
     jb      verify_error           ; cl must be at least 2

     mov     CylSec, cx             ; save for later use

     INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;

                                    ; si=bx: decrypted sector
     LDS_SI  lpBuffer               ; di   : encrypted sector
     mov     si, bx
     mov     ax, word ptr [XSI+NAVSIG_OFFSET]
     cmp     ax, 'AN'
     jne     verify_error

     mov     al, byte ptr [XSI+NAVSIG_OFFSET+2]
     cmp     al, 'V'
     jne     verify_error

     mov     ax, word ptr [XSI]      ;
     xor     ax, word ptr [XDI]      ;
     cmp     ax, Key
     jne     verify_error

     mov     ax, word ptr [XSI+2]
     xor     ax, word ptr [XDI+2]
     cmp     ax, Key
     jne     verify_error

     mov     ax, word ptr [XSI+4]
     xor     ax, word ptr [XDI+4]
     cmp     ax, Key
     jne     verify_error

     mov     al, byte ptr [XSI+ FLAG_OFFSET]
     or      al, al
     jz      verify_first_attempt

     mov     di, MbrOffset
     mov     ax, word ptr [XDI+ HEAD_SECTOR_OFFSET]
     jmp     @F

verify_first_attempt:

     mov     word ptr [XSI + FLAG_OFFSET], 1
     mov     ax, 0301h
     mov     bx, si
     mov     cx, CylSec
     mov     dx, 0080h
     INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;
     or      ax, ax
     jnz     Verify_error

     mov     ax, 1

@@:
     LDS_SI  lpRestoreInfo
     mov     [XSI].RESTORE_INFO.StartHead, ah
     mov     [XSI].RESTORE_INFO.StartSec,  al

     jmp     verify_done
verify_error:
     xor     ax, ax

verify_done:
     ret
VerifyData endp

;-----------------------------------------------------------
;-----------------------------------------------------------
ProcessDisk PROC,
    lpBuffer:LPBYTE,
    lpRestoreInfo: LPBYTE

    LOCAL  CylCount:WORD
    LOCAL  HeadCount:BYTE
    LOCAL  SecCount:BYTE
    LOCAL  Key:WORD
    LOCAL  dwCounter: DWORD

;--------------------------
    LES_DI  lpBuffer
    LDS_SI  lpRestoreInfo
                                          ; calculate counter
    xor     dx, dx
    xor     cx, cx

    mov     ax, [XSI].RESTORE_INFO.MaxCyl
    sub     ax, [XSI].RESTORE_INFO.StartCyl

    mov     cl, [XSI].RESTORE_INFO.MaxHead
    inc     cl
    mul     cx

    mov     cl, [XSI].RESTORE_INFO.MaxSec
    mul     cx

    mov     word ptr [dwCounter], ax
    mov     word ptr [dwCounter+2], dx

                                        ; Adjust for a restarted repair
    mov     al, [XSI].RESTORE_INFO.StartHead
    mov     cl, [XSI].RESTORE_INFO.MaxSec
    mul     cl

    xor     cx, cx
    mov     cl, [XSI].RESTORE_INFO.StartSec
    add     ax, cx
    dec     ax

    sub     word ptr [dwCounter], ax
    sbb     word ptr [dwCounter+2], 0

    ;--------------------------------------
    ; setup for display

    PUSH    DS
    PUSH    BX

ifdef OS_16
        mov     bx, Seg hex
else
        mov     bx, cs
endif
        mov     ds, bx

    SetupDisplay

    POP     BX
    POP     DS

    mov     ax, [XSI].RESTORE_INFO.Key
    mov     Key, ax

    mov     ax, [XSI].RESTORE_INFO.StartCyl
    mov     CylCount, ax

    mov     bx, di
    mov     dl, 080h

cyl_loop:
    mov     al, [XSI].RESTORE_INFO.StartHead
    mov     HeadCount, al

head_loop:
    mov     al, [XSI].RESTORE_INFO.StartSec
    mov     SecCount, al

sec_loop:
    mov     ax, CylCount
    xchg    ah, al
    mov     cl, 6
    shl     al, cl
    or      al, SecCount
    mov     cx, ax
    mov     dh, HeadCount
    mov     dl, 080h
    mov     ax, 0201h
    INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;

    INVOKE  DecryptSector, Key

    INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;

    mov     ax, [XSI].RESTORE_INFO.MbrOffset
    INVOKE  UpdateMBR, lpBuffer, ax, CylCount, HeadCount, SecCount

    dec_dword  dwCounter
    INVOKE  display_counter, dwCounter

    inc     SecCount
    mov     al, SecCount
    cmp     al, [XSI].RESTORE_INFO.MaxSec

    jle     sec_loop                            ;;>> sec loop

    mov     [XSI].RESTORE_INFO.StartSec, 1      ;; init start sector

    inc     HeadCount                           ;;
    mov     al, HeadCount                       ;;>> head loop
    cmp     al, [XSI].RESTORE_INFO.MaxHead

    jle     head_loop

    mov     [XSI].RESTORE_INFO.StartHead, 0      ;; init start head
    inc     CylCount
    mov     ax, CylCount
    cmp     ax, [XSI].RESTORE_INFO.MaxCyl
                                        ; jl: Last cylinder is NOT encrypted!!
    jl      cyl_loop                    ;;>> cyl loop


    INVOKE  repair_mbr, lpBuffer, lpRestoreInfo
    or      ax, ax
    jz      process_cyl_error

    INVOKE  ZeroOutSavedSector, lpBuffer, lpRestoreInfo
    jmp     process_cyl_done

process_cyl_error:
    xor     ax, ax
process_cyl_done:
     ret
ProcessDisk endp

;#####################################################

UpdateMBR PROC USES SES SDS XDI XDX,
    lpBuffer:LPBYTE,
    MbrOffset:WORD,
    Cyl:WORD,
    Head:BYTE,
    Sec:BYTE
    ;--------------------
    LES_DI lpBuffer
    push   es
    pop    ds

    mov    ax, word ptr [MbrOffset]
    mov    di, ax

    cmp    word ptr [XDI+01feh], 0AA55h
    jne    update_error

    mov    ax, word ptr [Cyl]
    mov    word ptr [XDI+ 029h],  ax                     ; last head decrypted

    mov    ch, byte ptr [Head]
    mov    cl, byte ptr [Sec]
    inc    cl
    mov    word ptr [XDI+ HEAD_SECTOR_OFFSET], cx         ; sector

    mov    ax, 0301h
    mov    bx, di
    mov    cx, 0001h
    mov    dx, 0080h

    INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;
    or      ax, ax
    jz      update_done

update_error:
    stc
update_done:
    ret
UpdateMBR endp

;-------------------------
GetDiskGeometry PROC,
     lpBuffer:LPBYTE,
     lpRestoreInfo:LPBYTE
     ;--------------------

     mov     ax, 0201h
     mov     bx, di
     mov     cx, 0001h
     mov     dx, 0080h

     INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,0,1,0, 080H  ;Cyl, Sec, Head, Drive;
     or      ax, ax
     jnz     get_disk_geometry_error

     LES_DI  lpBuffer

     add     XDI, 01EEh     ; point to end of partition table
     mov     cx,4

get_active:

     mov     bl, byte ptr es:[XDI+4]   ; get system type

     cmp     bl, 1                  ; 12-bit fat
     je      found_it

     cmp     bl, 4                  ; skip 2 and 3
     jb      get_continue

     cmp     bl, 6                  ; 4=16-bit fat, 5=extended DOS part
     jbe     found_it               ; 6=32-bit fat

get_continue:
     sub     XDI, 10h
     loop    get_active

found_it:

     mov     ax, es:[XDI+ 06h]       ; end cyl/sec
     mov     dh, es:[XDI+ 01h]       ;

     xchg    ah, al                  ; bits 00 through 07
     mov     cl, 6                   ;
     shr     ah, cl                  ; get bits 08 and 09
     and     dh, 0C0h                ; get bits 10 and 11
     mov     cl, 4
     shr     dh, cl
     or      ah, dh                  ; add bits 10 and 11

     LDS_SI lpRestoreInfo
     mov    [XSI].RESTORE_INFO.MaxCyl,  ax

     mov     ah,8h
     mov     dl,80h
     int     13h
     jc      get_disk_geometry_error

     and     cl, 03fh
     and     dh, 03fh

     mov    [XSI].RESTORE_INFO.MaxHead, dh
     mov    [XSI].RESTORE_INFO.MaxSec,  cl

     mov    ax, TRUE
     jmp    get_disk_geometry_done

get_disk_geometry_error:
     xor    ax, ax

get_disk_geometry_done:
     ret

GetDiskGeometry endp


;-------------------------
repair_mbr PROC ,
    lpBuffer:LPBYTE,
    lpRestoreInfo:LPBYTE

    ;---------------------
    LES_DI  lpBuffer
    LDS_SI  lpRestoreInfo

    xor    cx, cx
    mov    cl, [XSI].RESTORE_INFO.MaxSec
    sub    cx, 7

    mov    ax, 0201h
    mov    bx, di
    mov    dx, 0080h
    INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;

    cmp    word ptr es:[bx+01feh], 0AA55h
    jne    repair_mbr_error

    mov    ax, 0301h
    mov    cx, 0001h

    INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;

    mov    ax, 1
    jmp    repair_mbr_done

repair_mbr_error:
    xor    ax, ax
repair_mbr_done:
    ret
repair_mbr endp

;-------------------------
GetDecryptionData PROC,
     lpBuffer:LPBYTE,
     lpRestoreInfo:LPBYTE

     LOCAL   lpSEG_OFF: LPBYTE
     ;-----------------------------

     push    word ptr lpBuffer
     pop     di
     add     di, 400h
     mov     WORD ptr lpSEG_OFF, di
     push    word ptr lpBuffer+2
     pop     word ptr lpSEG_OFF + 2

     LES_DI lpBuffer
     LDS_SI lpRestoreInfo

     mov    bx, di
     add    bx, 400h
     mov    [XSI].RESTORE_INFO.MbrOffset, bx

     mov    ax, 0201h
     mov    cx, 0001h
     mov    dx, 0080h

     INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;

     mov    cx, word ptr es:[bx + 029h]
     mov    [XSI].RESTORE_INFO.StartCyl, cx      ; this is starting cylinder #

     xor    cx, cx
     mov    cl, [XSI].RESTORE_INFO.MaxSec
     sub    cl, 3

     mov    ax, 0201h
     mov    bx, di
     mov    dx, 0080h

     INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;

     mov    ax, word ptr es:[bx+1d1h]
     mov    [XSI].RESTORE_INFO.Key, ax

get_dec_done:
     mov    ax, 1
     ret
GetDecryptionData  endp


;------------------------------------------------
ZeroOutSavedSector PROC,
     lpBuffer:LPBYTE,
     lpRepairInfo:LPBYTE
     ;-------------------
     LES_DI lpBuffer
     LDS_SI lpRepairInfo

     mov     al, [XSI].RESTORE_INFO.MaxSec

     mov    cx, 100h
     push   di

zero_loop:
     mov    word ptr es:[di], 0
     inc    di
     inc    di
     loop   zero_loop

     pop    bx

     mov    cl, al
     sub    cx, 8
     mov    dx, 0080h

     INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,ch,cl,dh, 080h  ;Cyl, Sec, Head, Drive;
     or      ax, ax
     jnz     z_error

     inc    ax
     jmp    z_done

z_error:
     xor  ax, ax
z_done:
     ret
ZeroOutSavedSector endp

;------------------------------------------------
DecryptSector PROC USES XDI XCX XDX XBX,
    Key:WORD

    mov     cx, 100h
    mov     dx, word ptr [Key]

dec_loop:
    xor     word ptr es:[bx], dx
    inc     bx
    inc     bx

    loop    dec_loop

    ret
DecryptSector endp

;-------------------------
display_counter PROC USES XDX XCX,
    dwCounter:DWORD

    locate  row, col

    xor     ax, ax
    xor     cx, cx
    xor     dx, dx

    mov     cx, word ptr [dwCounter+2]
    call    printword
    mov     cx, word ptr [dwCounter]
    call    printword

    ret
display_counter endp

;-------------------------
printword PROC,    ; word in CX

        push    cx
        mov     al, ch
        call    printnum

        pop     ax
        call    printnum
        ret
printword endp

;-------------------------
printnum        PROC USES XAX XBX XCX XDX SDS, ;al has hex byte

ifdef OS_16
        mov     bx, Seg hex
else
        mov     bx, cs
endif
        mov     ds, bx

        mov     ah,al
        mov     bx,offset hex           ; xlat table
        and     al,11110000b            ; get high
        mov     cl,4                    ; move to low
        shr     al,cl                   ;
        xlat                            ; xlat
        push    ax
        push    bx                      ;
        mov     ah,0Eh                  ;
        mov     bh,vpage                ;
        int     10h                     ; print high
        pop     bx
        pop     ax                      ;
                                
        mov     al,ah                   ; restore al
        and     al,1111b                ; mask out high
        xlat                            ; xlat
        mov     ah,0Eh                  ;
        mov     bh,vpage                ;
        int     10h                     ; print low
        ret

printnum        endp


ifdef OS_16
.data
endif

hex     BYTE    "01234567"              ; Xlat table for printnum proc
        BYTE    "89ABCDEF"              ;



row     BYTE     ?
col     BYTE     ?
smode   BYTE     ?
vpage   BYTE     ?

ENDIF
UniqueSegmentEnd

END
