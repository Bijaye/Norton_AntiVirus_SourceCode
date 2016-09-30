;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/fstart.asv   1.16   21 Aug 1996 10:21:12   CNACHEN  $
;;
;; Description: Defines FIL4_GET_START function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/fstart.asv  $
;  
;     Rev 1.16   21 Aug 1996 10:21:12   CNACHEN
;  Fixed fstart 256 byte buffer boundary bug.
;  
;     Rev 1.15   08 Mar 1996 14:56:48   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.14   04 Dec 1995 12:02:54   DCHI
;  Removed "PK" check from both GET_START functions.
;  
;     Rev 1.13   29 Nov 1995 14:23:24   DCHI
;  Removed PKLITE check and added FIL4_NEW_GET_START which does not
;  skip opcode bytes 0x00 and 0x33.
;  
;     Rev 1.12   29 Mar 1995 17:34:08   SZIADEH
;  correction on error checking after fseek. 
;  
;     Rev 1.11   16 Dec 1994 22:00:42   SZIADEH
;  added 3.05 changes.
;  
;     Rev 1.10   19 Aug 1994 21:57:08   RSTANEV
;  Fixed so that the data at the end of the file is always under a segment.
;
;     Rev 1.9   19 Aug 1994 21:50:04   RSTANEV
;  Using new data segment conventions.
;
;     Rev 1.8   18 Aug 1994 23:20:26   RSTANEV
;  Fixed SYM_VXD compilation errors.
;
;     Rev 1.7   17 Aug 1994 13:26:00   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.6   08 Jul 1994 16:43:46   CAREY
;  Added John Millard's changes to handle 32 bit firstlanding problem.
;
;     Rev 1.5   26 May 1994 15:48:22   RSTANEV
;  Using .DATA segment for all OS_32.
;
;     Rev 1.4   26 May 1994 15:47:32   RSTANEV
;  Minor changes.
;
;     Rev 1.3   20 May 1994 12:09:06   RSTANEV
;  Now initializing WhaleFlag and correctly checking for PKLITE.
;
;     Rev 1.2   17 May 1994 14:38:30   RSTANEV
;  Added a warning message.
;
;     Rev 1.1   05 May 1994 12:43:12   RSTANEV
;  Fixed a couple of bugs when scanning in 32-bit mode.
;
;     Rev 1.0   02 May 1994 18:10:24   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; FIL4_GET_START
;
; Get the starting code of an executable program.
;
; UINT WINPAI FIL4_GET_START ( UINT Handle, LPBYTE Buffer, STARTCODE Struct );
;
; Input:
;       1)  Handle of file
;       2)  Buffer to use (256 bytes) (STARTSIZE)
;       3)  Structure of EXE start
; Output:
;       1)  Number of bytes read, 0 for error.
;       2)  Buffer filled
;       3)  Structure filled
;
;----------------------------------------------------------------------------
FIL4_GET_START  PROC  CODEDISTANCE  USES XBX XCX XSI XDI SDS SES,
        Handle:UINT,
        Buffer:LPBYTE,
        StartStruct:LPBYTE

MAXDEPTH  EQU  100

        LOCAL   Depth:WORD
        LOCAL   Retval:WORD
        LOCAL   BytesRead:WORD
        LOCAL   ByteIn:WORD             ; offset in buffer
        LOCAL   MyIP:WORD               ; current IP
        LOCAL   WhaleFlag:BOOL
        LOCAL   SetFirst:BOOL           ; have we set the first landing yet?

        mov     SetFirst,FALSE          ; we have not hit the first landing yet
        mov     WhaleFlag,FALSE         ; neither have we a Whale virus

        LDS_SI  Buffer
        LES_DI  StartStruct

        mov     ByteIn,0
        mov     Depth,0

        ; Initialize structure returns to zero.
        mov     XES[XDI].STARTCODE.Filetype,0

        ; make sure both HIGH & LOW word's of first landing are zero.
IF OS_16
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding,0
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding + 2,0

        mov     word ptr XES[XDI].STARTCODE.BufferOffset,0
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,0
ELSE
        mov     XES[XDI].STARTCODE.Firstlanding,0
        mov     XES[XDI].STARTCODE.BufferOffset,0
ENDIF

        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE

        cmp     XAX,1                   ; Minimum read of 1 bytes
        jl      J_INVALID               ; is needed for testing.

        mov     BytesRead,ax            ; Number of bytes read(EOB)
        mov     Retval,ax

        LES_DI  Buffer
        add     XDI,STARTSIZE
        cld
        mov     XCX,32                  ; save 64 bytes of header
        rep     movsw
        LES_DI  StartStruct
        sub     XSI,64                  ; restore SI back to head of buffer

        ; Determine type of program (COM,EXE,SYS,Bulgarian,Unknown)
        xor     XAX,XAX
        xor     XDX,XDX

        mov     MyIP,100h               ; default start IP

        cmp     [XSI],WORD PTR 'ZM'     ; Native EXE
        je      J_EXE

        cmp     [XSI],WORD PTR 'MZ'     ; European EXE
        je      J_EXE

        cmp     [XSI],WORD PTR 0E94Dh   ; Bulgarian marked COM
        je      J_BCOM

        cmp     [XSI],BYTE PTR 0E9h     ; Verified COM
        je      J_VCOM

        cmp     [XSI],WORD PTR -1       ; Test if it is a DEVICE Driver
        jne     J_COM
        cmp     [XSI+2],WORD PTR -1     ; Test if it is a DEVICE Driver
        jne     J_COM

        jmp     J_SYS

J_COM:
        mov     XES[XDI].STARTCODE.Filetype,IS_COM
        jmp     J_START

J_BCOM:
        mov     XES[XDI].STARTCODE.Filetype,IS_BCOM
        jmp     J_START

J_VCOM:
        mov     XES[XDI].STARTCODE.Filetype,IS_VCOM

                                        ;-------------------------------
                                        ; Whale Test for COMs
                                        ;-------------------------------
        cmp     BytesRead, STARTSIZE    ; must have at least 5 bytes to verify!
        jl      J_E8

        mov     ax,[XSI + 4]            ; Get identity word
        xor     ax,1501h                ; xor with 'FI' and 'SH'
        add     ax,[XSI + 2]            ; add second and
        add     ax,[XSI]                ; first words
        or      ax,ax                   ; If zero then Whale
        jz      J_Moby

        jmp     J_E8

J_Moby:
        mov     XES[XDI].STARTCODE.Filetype,IS_WHALE
        jmp     J_OUT

J_SYS:
        mov     XES[XDI].STARTCODE.Filetype,IS_SYS
        xor     ax,ax                   ; drivers start at 0
        mov     MyIP,ax                 ; set up initial ip
        mov     ax,[XSI + 6]            ; address to jump to
        jmp     J_FJ3                   ; and treat as jump to continue

J_EXE:
        cmp     WORD PTR [XSI + 14h],1  ; IP = 1 in Whale
        jne     J_EXE1
        mov     WhaleFlag,TRUE
J_EXE1:
        mov     XES[XDI].STARTCODE.Filetype,IS_EXE

        cmp     [XSI + 1Ch],WORD PTR 'ZL'; LZEXE
        jne     @F
        mov     XES[XDI].STARTCODE.Filetype,IS_LZEXE
@@:
        mov     ax,[XSI + 16h]          ; CS of header
        add     ax,[XSI + 08h]          ; Number of paragraphs in header
        mov     dx,10h
        mul     dx                      ; DX:AX = Location of code
        add     ax,[XSI + 14h]          ; IP DX:AX = Location of code
        adc     dx,0

        ; The and instruction is used in the case where a wrap around
        ; occures. As in ital_805 where cs=fff1 and ip=100h
        ; In all other cases, dx:ax is the correct offset of the programs
        ; entry point.

        and     dx, 000fh               ; clear high nibbles.
        sub     ax,BytesRead
        sbb     dx,0
        mov     cx,dx
        mov     dx,ax

        mov     ax,[XSI + 14h]          ; save starting IP
        mov     MyIP,ax

        jmp     J_ReRead

J_START:
J_TestAgain:

        mov     ah,MAXPAD               ; Look for PADDING bytes first
        cld                             ; clear for scan
        SMOV    SES,SCS
J_TB:
        lea     XDI,TestByteList
        mov     XCX,TestByteListLength
        mov     al,[XSI]
        repne   scasb
        jne     J_TB0
        dec     ah                      ; Padding bytes
        jz      J_DONE
        inc     XSI
        inc     ByteIn
        inc     MyIP
        mov     cx,BytesRead
        cmp     ByteIn,cx
        jb      J_TB
        jmp     J_FillBuffer
J_TB0:

                                        ;-------------------------------
                                        ; Look for JUMP/CALL
                                        ;-------------------------------
        xor     XAX,XAX
        cmp     [XSI],BYTE PTR 0EBh
        je      J_EB
        cmp     [XSI],BYTE PTR 0E8h
        je      J_E8
        cmp     [XSI],BYTE PTR 0E9h
        je      J_E8
        LES_DI  StartStruct
        cmp     XES[XDI].STARTCODE.Filetype,IS_COM

        jne     J_DONE                  ; Skip ret trace if not COM

                                        ; New code to trace initial
                                        ; mov/push/ret virus system

        cmp     [XSI + 4],BYTE PTR 0C3h         ; Either RET
        je      J_PUSHRET
        cmp     [XSI + 4],WORD PTR 0C3FAh       ; or CLI RET
        jne     J_DONE

J_PUSHRET:
        mov     al,[XSI]                        ; XOR the MOV and PUSH
        mov     ah,[XSI + 3]                    ;  together to make sure
        xor     al,ah                           ;  it uses the same reg.
        cmp     al,BYTE PTR 0E8h                ;  Save XSI+3 in ah.
        jne     J_DONE
        and     ah,BYTE PTR 0F8h                ; Wipe bottom 3 bits
        cmp     ah,BYTE PTR 050h                ; Make sure it was a PUSH
        jne     J_DONE
        mov     ax,[XSI + 1]                    ; Get landing and convert
        sub     ax,100h                         ; absolute to relative.

        ; since we're doing an absolute jump from TOF, we want to zero
        ; our buffer start and bytes-in

        mov     MyIP, 100h                      ; reset IP (non-relative jmp)
        LDS_SI  Buffer                          ; definitely do a re-read
        mov     ByteIn, 0                       ; 0 bytes into buffer.

        jmp     J_FJ3

; So the validation is: find the RET, verify the PUSH right before the
; RET, verify a MOV instruction to the same register.  The only thing I
; don't check is buffer out-of-bounds but then I don't think this
; circumstance could be fulfilled with random data. -- CJKuo
;

J_E8:
        mov     ax,[XSI + 1]
        add     ax,3
        jmp     J_FJ3
J_EB:
        mov     al,[XSI + 1]
        cbw                             ; this is a signed offset!
        add     ax,2
J_FJ3:
        inc     Depth                   ; jumping another level deeper
        cmp     Depth,MAXDEPTH          ; allow only MAXDEPTH levels
        ja      J_ERROR
        add     ax,MyIP                 ; new IP and test direction of seek
        mov     bx,ax
        xor     cx,cx                   ; assume positive seek
        sub     ax,MyIP                 ; back to relative, test direction
        mov     MyIP,bx                 ; and set new IP
        jae     F2                      ; forward
        mov     cx,-1                   ; negative seek
F2:
IF OS_32                                ; sign extend ax if needed
        cwde                            ; make offset 32-bit
ENDIF                                   ; XAX signed offset in segment
        add     XSI,XAX                 ; new memory offset (possibly)
        add     ByteIn,ax               ; offset to seek
        js      J_Backward
        mov     ax,BytesRead
        cmp     ByteIn,ax

        pushf                           ; save flags from compare

        mov     ax, ByteIn              ; send this over to J_TestAgain for
                                        ; first landing calculation! *CAREY*


        cmp     SetFirst,FALSE          ; is this the first landing?
        jnz     @F

        ; we should only be setting FirstLanding here in the case where a JMP
        ; destination was within our original buffer.  In this case, the SEEK
        ; by J_ReRead is not performed until later (or never).  If SetFirst is
        ; 0 and we are setting the first landing here, it will mean that we
        ; jumped from the JB at J_FJ3 instead of doing the standard SEEK.  AX
        ; will contain the low word of the first landing, and the high word
        ; will be zero.

        LES_DI  StartStruct
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding, ax
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding + 2,0

        mov     SetFirst,TRUE           ; no longer the first landing

@@:

        popf                            ; restore flags!

        jnc     J_Backward

        ; new behavior: always reload if we're following a jump C&D 8/19

        mov     cx, -1
        mov     dx, ByteIn
        sub     dx, BytesRead
        jmp     J_ReRead

J_Backward:
        mov     dx,ByteIn               ; current position
        sub     dx,BytesRead            ; rememer we are past this point now
        sbb     cx,0

J_ReRead:

        INVOKE  FIL4_SEEK,Handle,cx::dx,fSEEK_FOR
if OS_16
        cmp     dx, -1                  ; on seek error, both ax and dx are -1
        jne     @F                      ; don't both checking ax if dx != -1
endif
        cmp     XAX,-1
        je      J_INVALID               ; start of the file *OR* have an error
@@:
        LES_DI  StartStruct

if OS_32
        mov     XES[XDI].STARTCODE.BufferOffset,  XAX
else
        mov     word ptr XES[XDI].STARTCODE.BufferOffset,  ax
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,dx
endif

        cmp     SetFirst, FALSE         ; is this the first landing?
        jnz     @F

if OS_32
        mov     XES[XDI].STARTCODE.Firstlanding, XAX
else
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding,ax
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding + 2,dx
endif

        mov     SetFirst,TRUE           ; no longer the first landing

@@:

J_FillBuffer:
        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE
        cmp     XAX, -1
        je      J_INVALID

        or      XAX, XAX
        je      J_INVALID

        mov     Retval,ax
        mov     BytesRead,ax            ; May be needed by final seek

        LDS_SI  Buffer                  ; recover SI which may have changed
        cmp     WhaleFlag,FALSE

                                        ;-------------------------------
                                        ; Whale Test for EXEs
                                        ;-------------------------------
        je      J_GOON
        cmp     BYTE PTR [XSI],0E9h
        jne     J_GOON
        cmp     WORD PTR [XSI + 1],023B8h
        jne     J_GOON
        cmp     WORD PTR [XSI + 3],05A4Dh
        jne     J_GOON
        LES_DI  StartStruct
        mov     XES[XDI].STARTCODE.Filetype,IS_WHALE
        mov     ax,1
        jmp     J_OUT

J_GOON:
        mov     WhaleFlag,FALSE
        mov     BytesRead,ax
        mov     ByteIn,0
        jmp     J_TestAgain

J_DONE:
        cmp     ByteIn,0
        je      J_OUT
        mov     dx,BytesRead
        sub     dx,ByteIn
        xor     XCX, XCX
        INVOKE  FIL4_SEEK,Handle,cx::dx,fSEEK_BAK
if OS_16
        cmp     dx, -1
        jne     @F                      ; no error occured on seek
endif
        cmp     XAX,-1
        je      J_INVALID               ; start of the file *OR* have an error
                                        ;added in order to implement
                                        ;FollowJump(). Sami
@@:
        LES_DI  StartStruct

if OS_32
        mov     XES[XDI].STARTCODE.BufferOffset, XAX
else
        mov     word ptr XES[XDI].STARTCODE.BufferOffset, ax
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,dx
endif

        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE
        cmp     XAX, -1                   ; correctly handle an error, otherwise
        je      J_INVALID               ; I end up returning 0xffff bytes
                                        ; instead of an error (0)
        mov     Retval,ax
        jmp     J_OUT

J_INVALID:                              ; Handles zero bytes read
        LES_DI  StartStruct
        mov     XES[XDI].STARTCODE.Filetype,IS_INVALID

J_ERROR:                                ; note that this doesn't actually
        mov     Retval,0                ; return anything the caller will
                                        ; see as an error.

J_OUT:

IF OS_16
        mov     ax, Retval
ELSE
        movzx   eax, Retval
ENDIF
        cmp     XAX, 32
        jae     J_Go
        cld
        LES_DI  Buffer
        add     XDI,XAX
        xor     XAX,XAX
        mov     XCX,16
        rep     stosw
        mov     ax, Retval
J_Go:
        ret

FIL4_GET_START  ENDP

;----------------------------------------------------------------------------
; FIL4_NEW_GET_START
;
; Get the starting code of an executable program.  Do not skip 0x00 and 0x33.
;
; UINT WINPAI FIL4_NEW_GET_START ( UINT Handle,
;                                  LPBYTE Buffer,
;                                  STARTCODE Struct );
;
; Input:
;       1)  Handle of file
;       2)  Buffer to use (256 bytes) (STARTSIZE)
;       3)  Structure of EXE start
; Output:
;       1)  Number of bytes read, 0 for error.
;       2)  Buffer filled
;       3)  Structure filled
;
;----------------------------------------------------------------------------
FIL4_NEW_GET_START  PROC  CODEDISTANCE  USES XBX XCX XSI XDI SDS SES,
        Handle:UINT,
        Buffer:LPBYTE,
        StartStruct:LPBYTE

MAXDEPTH  EQU  100

        LOCAL   Depth:WORD
        LOCAL   Retval:WORD
        LOCAL   BytesRead:WORD
        LOCAL   ByteIn:WORD             ; offset in buffer
        LOCAL   MyIP:WORD               ; current IP
        LOCAL   WhaleFlag:BOOL
        LOCAL   SetFirst:BOOL           ; have we set the first landing yet?

        mov     SetFirst,FALSE          ; we have not hit the first landing yet
        mov     WhaleFlag,FALSE         ; neither have we a Whale virus

        LDS_SI  Buffer
        LES_DI  StartStruct

        mov     ByteIn,0
        mov     Depth,0

        ; Initialize structure returns to zero.
        mov     XES[XDI].STARTCODE.Filetype,0

        ; make sure both HIGH & LOW word's of first landing are zero.
IF OS_16
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding,0
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding + 2,0

        mov     word ptr XES[XDI].STARTCODE.BufferOffset,0
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,0
ELSE
        mov     XES[XDI].STARTCODE.Firstlanding,0
        mov     XES[XDI].STARTCODE.BufferOffset,0
ENDIF

        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE

        cmp     XAX,1                   ; Minimum read of 1 bytes
        jl      JN_INVALID              ; is needed for testing.

        mov     BytesRead,ax            ; Number of bytes read(EOB)
        mov     Retval,ax

        LES_DI  Buffer
        add     XDI,STARTSIZE
        cld
        mov     XCX,32                  ; save 64 bytes of header
        rep     movsw
        LES_DI  StartStruct
        sub     XSI,64                  ; restore SI back to head of buffer

        ; Determine type of program (COM,EXE,SYS,Bulgarian,Unknown)
        xor     XAX,XAX
        xor     XDX,XDX

        mov     MyIP,100h               ; default start IP

        cmp     [XSI],WORD PTR 'ZM'     ; Native EXE
        je      JN_EXE

        cmp     [XSI],WORD PTR 'MZ'     ; European EXE
        je      JN_EXE

        cmp     [XSI],WORD PTR 0E94Dh   ; Bulgarian marked COM
        je      JN_BCOM

        cmp     [XSI],BYTE PTR 0E9h     ; Verified COM
        je      JN_VCOM

        cmp     [XSI],WORD PTR -1       ; Test if it is a DEVICE Driver
        jne     JN_COM
        cmp     [XSI+2],WORD PTR -1     ; Test if it is a DEVICE Driver
        jne     JN_COM

        jmp     JN_SYS

JN_COM:
        mov     XES[XDI].STARTCODE.Filetype,IS_COM
        jmp     JN_START

JN_BCOM:
        mov     XES[XDI].STARTCODE.Filetype,IS_BCOM
        jmp     JN_START

JN_VCOM:
        mov     XES[XDI].STARTCODE.Filetype,IS_VCOM

                                        ;-------------------------------
                                        ; Whale Test for COMs
                                        ;-------------------------------
        cmp     BytesRead, STARTSIZE    ; must have at least 5 bytes to verify!
        jl      JN_E8

        mov     ax,[XSI + 4]            ; Get identity word
        xor     ax,1501h                ; xor with 'FI' and 'SH'
        add     ax,[XSI + 2]            ; add second and
        add     ax,[XSI]                ; first words
        or      ax,ax                   ; If zero then Whale
        jz      JN_Moby

        jmp     JN_E8

JN_Moby:
        mov     XES[XDI].STARTCODE.Filetype,IS_WHALE
        jmp     JN_OUT

JN_SYS:
        mov     XES[XDI].STARTCODE.Filetype,IS_SYS
        xor     ax,ax                   ; drivers start at 0
        mov     MyIP,ax                 ; set up initial ip
        mov     ax,[XSI + 6]            ; address to jump to
        jmp     JN_FJ3                  ; and treat as jump to continue

JN_EXE:
        cmp     WORD PTR [XSI + 14h],1  ; IP = 1 in Whale
        jne     JN_EXE1
        mov     WhaleFlag,TRUE
JN_EXE1:
        mov     XES[XDI].STARTCODE.Filetype,IS_EXE

        cmp     [XSI + 1Ch],WORD PTR 'ZL'; LZEXE
        jne     @F
        mov     XES[XDI].STARTCODE.Filetype,IS_LZEXE
@@:
        mov     ax,[XSI + 16h]          ; CS of header
        add     ax,[XSI + 08h]          ; Number of paragraphs in header
        mov     dx,10h
        mul     dx                      ; DX:AX = Location of code
        add     ax,[XSI + 14h]          ; IP DX:AX = Location of code
        adc     dx,0

        ; The and instruction is used in the case where a wrap around
        ; occures. As in ital_805 where cs=fff1 and ip=100h
        ; In all other cases, dx:ax is the correct offset of the programs
        ; entry point.

        and     dx, 000fh               ; clear high nibbles.
        sub     ax,BytesRead
        sbb     dx,0
        mov     cx,dx
        mov     dx,ax

        mov     ax,[XSI + 14h]          ; save starting IP
        mov     MyIP,ax

        jmp     JN_ReRead

JN_START:
JN_TestAgain:

        mov     ah,MAXPAD               ; Look for PADDING bytes first
        cld                             ; clear for scan
        SMOV    SES,SCS
JN_TB:
        lea     XDI,TestByteNewList
        mov     XCX,TestByteNewListLength
        mov     al,[XSI]
        repne   scasb
        jne     JN_TB0
        dec     ah                      ; Padding bytes
        jz      JN_DONE
        inc     XSI
        inc     ByteIn
        inc     MyIP
        mov     cx,BytesRead
        cmp     ByteIn,cx
        jb      JN_TB
        jmp     JN_FillBuffer

JN_TB0:

                                        ;-------------------------------
                                        ; Look for JUMP/CALL
                                        ;-------------------------------
        xor     XAX,XAX
        cmp     [XSI],BYTE PTR 0EBh
        je      JN_EB
        cmp     [XSI],BYTE PTR 0E8h
        je      JN_E8
        cmp     [XSI],BYTE PTR 0E9h
        je      JN_E8
        LES_DI  StartStruct
        cmp     XES[XDI].STARTCODE.Filetype,IS_COM

        jne     JN_DONE                 ; Skip ret trace if not COM

                                        ; New code to trace initial
                                        ; mov/push/ret virus system

        cmp     [XSI + 4],BYTE PTR 0C3h         ; Either RET
        je      JN_PUSHRET
        cmp     [XSI + 4],WORD PTR 0C3FAh       ; or CLI RET
        jne     JN_DONE

JN_PUSHRET:
        mov     al,[XSI]                        ; XOR the MOV and PUSH
        mov     ah,[XSI + 3]                    ;  together to make sure
        xor     al,ah                           ;  it uses the same reg.
        cmp     al,BYTE PTR 0E8h                ;  Save XSI+3 in ah.
        jne     JN_DONE
        and     ah,BYTE PTR 0F8h                ; Wipe bottom 3 bits
        cmp     ah,BYTE PTR 050h                ; Make sure it was a PUSH
        jne     JN_DONE
        mov     ax,[XSI + 1]                    ; Get landing and convert
        sub     ax,100h                         ; absolute to relative.

        ; since we're doing an absolute jump from TOF, we want to zero
        ; our buffer start and bytes-in

        mov     MyIP, 100h                      ; reset IP (non-relative jmp)
        LDS_SI  Buffer                          ; definitely do a re-read
        mov     ByteIn, 0                       ; 0 bytes into buffer.

        jmp     JN_FJ3

; So the validation is: find the RET, verify the PUSH right before the
; RET, verify a MOV instruction to the same register.  The only thing I
; don't check is buffer out-of-bounds but then I don't think this
; circumstance could be fulfilled with random data. -- CJKuo
;

JN_E8:
        mov     ax,[XSI + 1]
        add     ax,3
        jmp     JN_FJ3
JN_EB:
        mov     al,[XSI + 1]
        cbw                             ; this is a signed offset!
        add     ax,2
JN_FJ3:
        inc     Depth                   ; jumping another level deeper
        cmp     Depth,MAXDEPTH          ; allow only MAXDEPTH levels
        ja      JN_ERROR
        add     ax,MyIP                 ; new IP and test direction of seek
        mov     bx,ax
        xor     cx,cx                   ; assume positive seek
        sub     ax,MyIP                 ; back to relative, test direction
        mov     MyIP,bx                 ; and set new IP
        jae     NF2                     ; forward
        mov     cx,-1                   ; negative seek
NF2:
IF OS_32                                ; sign extend ax if needed
        cwde                            ; make offset 32-bit
ENDIF                                   ; XAX signed offset in segment
        add     XSI,XAX                 ; new memory offset (possibly)
        add     ByteIn,ax               ; offset to seek
        js      JN_Backward
        mov     ax,BytesRead
        cmp     ByteIn,ax

        pushf                           ; save flags from compare

        mov     ax, ByteIn              ; send this over to J_TestAgain for
                                        ; first landing calculation! *CAREY*


        cmp     SetFirst,FALSE          ; is this the first landing?
        jnz     @F

        ; we should only be setting FirstLanding here in the case where a JMP
        ; destination was within our original buffer.  In this case, the SEEK
        ; by J_ReRead is not performed until later (or never).  If SetFirst is
        ; 0 and we are setting the first landing here, it will mean that we
        ; jumped from the JB at J_FJ3 instead of doing the standard SEEK.  AX
        ; will contain the low word of the first landing, and the high word
        ; will be zero.

        LES_DI  StartStruct
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding, ax
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding + 2,0

        mov     SetFirst,TRUE           ; no longer the first landing

@@:

        popf                            ; restore flags!

        jnc     JN_Backward

        ; new behavior: always reload if we're following a jump C&D 8/19

        mov     cx, -1
        mov     dx, ByteIn
        sub     dx, BytesRead
        jmp     JN_ReRead

JN_Backward:
        mov     dx,ByteIn               ; current position
        sub     dx,BytesRead            ; rememer we are past this point now
        sbb     cx,0

JN_ReRead:

        INVOKE  FIL4_SEEK,Handle,cx::dx,fSEEK_FOR
if OS_16
        cmp     dx, -1                  ; on seek error, both ax and dx are -1
        jne     @F                      ; don't both checking ax if dx != -1
endif
        cmp     XAX,-1
        je      JN_INVALID              ; start of the file *OR* have an error
@@:
        LES_DI  StartStruct

if OS_32
        mov     XES[XDI].STARTCODE.BufferOffset,  XAX
else
        mov     word ptr XES[XDI].STARTCODE.BufferOffset,  ax
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,dx
endif

        cmp     SetFirst, FALSE         ; is this the first landing?
        jnz     @F

if OS_32
        mov     XES[XDI].STARTCODE.Firstlanding, XAX
else
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding,ax
        mov     WORD PTR XES[XDI].STARTCODE.Firstlanding + 2,dx
endif

        mov     SetFirst,TRUE           ; no longer the first landing

@@:

JN_FillBuffer:
        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE
        cmp     XAX, -1
        je      JN_INVALID

        or      XAX, XAX
        je      JN_INVALID

        mov     Retval,ax
        mov     BytesRead,ax            ; May be needed by final seek

        LDS_SI  Buffer                  ; recover SI which may have changed
        cmp     WhaleFlag,FALSE

                                        ;-------------------------------
                                        ; Whale Test for EXEs
                                        ;-------------------------------
        je      JN_GOON
        cmp     BYTE PTR [XSI],0E9h
        jne     JN_GOON
        cmp     WORD PTR [XSI + 1],023B8h
        jne     JN_GOON
        cmp     WORD PTR [XSI + 3],05A4Dh
        jne     JN_GOON
        LES_DI  StartStruct
        mov     XES[XDI].STARTCODE.Filetype,IS_WHALE
        mov     ax,1
        jmp     JN_OUT

JN_GOON:
        mov     WhaleFlag,FALSE
        mov     BytesRead,ax
        mov     ByteIn,0
        jmp     JN_TestAgain

JN_DONE:
        cmp     ByteIn,0
        je      JN_OUT
        mov     dx,BytesRead
        sub     dx,ByteIn
        xor     XCX, XCX
        INVOKE  FIL4_SEEK,Handle,cx::dx,fSEEK_BAK
if OS_16
        cmp     dx, -1
        jne     @F                      ; no error occured on seek
endif
        cmp     XAX,-1
        je      JN_INVALID              ; start of the file *OR* have an error
                                        ;added in order to implement
                                        ;FollowJump(). Sami
@@:
        LES_DI  StartStruct

if OS_32
        mov     XES[XDI].STARTCODE.BufferOffset, XAX
else
        mov     word ptr XES[XDI].STARTCODE.BufferOffset, ax
        mov     word ptr XES[XDI].STARTCODE.BufferOffset+2,dx
endif

        INVOKE  FIL4_READ,Handle,Buffer,STARTSIZE
        cmp     XAX, -1                 ; correctly handle an error, otherwise
        je      JN_INVALID              ; I end up returning 0xffff bytes
                                        ; instead of an error (0)
        mov     Retval,ax
        jmp     JN_OUT

JN_INVALID:                             ; Handles zero bytes read
        LES_DI  StartStruct
        mov     XES[XDI].STARTCODE.Filetype,IS_INVALID

JN_ERROR:                               ; note that this doesn't actually
        mov     Retval,0                ; return anything the caller will
                                        ; see as an error.

JN_OUT:

IF OS_16
        mov     ax, Retval
ELSE
        movzx   eax, Retval
ENDIF
        cmp     XAX, 32
        jae     JN_Go
        cld
        LES_DI  Buffer
        add     XDI,XAX
        xor     XAX,XAX
        mov     XCX,16
        rep     stosw
        mov     ax, Retval
JN_Go:
        ret

FIL4_NEW_GET_START  ENDP


IF OS_32
UniqueSegmentEnd
UniqueDataSegment
ENDIF

TestByteList  LABEL  BYTE
        DB      0F5h,0F8h,0F9h,0FBh,0FCh,033h,046h,04Bh,05Eh,090h,042h,092h
        DB      0FAh,0FDh,09Fh,09Eh,058h,059h,05Ah,05Dh,04Dh,000h

TestByteListLength  EQU  ($-TestByteList)

TestByteNewList  LABEL  BYTE
        DB      0F5h,0F8h,0F9h,0FBh,0FCh,046h,04Bh,05Eh,090h,042h,092h
        DB      0FAh,0FDh,09Fh,09Eh,058h,059h,05Ah,05Dh,04Dh

TestByteNewListLength  EQU  ($-TestByteNewList)

IF OS_32
UniqueDataSegmentEnd
ELSE
UniqueSegmentEnd
ENDIF

END
