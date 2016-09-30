; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\certlib4\vcs\ir_main.asv   1.11   29 Nov 1994 13:21:26   DALLEE  $
;
; Filename:     IR_MAIN.ASM
; Programmer:   Carey Nachenberg
; Assembler:    MASM 6.0+
; Project:      NAV 3.0
;
; Contents:     INOC_REP_MAIN
;
; Note:         main() for inoculation repair
;
; $Log:   S:\certlib4\vcs\ir_main.asv  $
;  
;     Rev 1.11   29 Nov 1994 13:21:26   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.8.1.1   01 Nov 1993 15:40:12   MFALLEN
;  Added a global flag that is being checked to see if Ansi to Oem and vice versa
;  conversion should be done on the filename prior to processing.
;  
;     Rev 1.8.1.0   19 Oct 1993 07:27:04   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.8   12 Oct 1993 16:37:16   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.7.2.1   12 Oct 1993 12:00:24   MFALLEN
;  Problems with Ansi/Oem conversion since FIL4_PROTECT is being called 
;  recursivelly.
;  
;     Rev 1.7.2.0   03 Sep 1993 19:30:26   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.7   30 Jul 1993 09:12:36   BRADR
;  Added external buffer to save on stack space.
;  
;     Rev 1.6   28 Jul 1993 16:36:32   CJKUO
;  Fixed re-use of WorkBuf.  FIL4_PROTECT blows away full buffer up to 1K.
;  Could not reuse.  Thus must have own buffer.
;  
;  Stack concerns...
;  
;     Rev 1.5   26 Jul 1993 12:10:50   BRADR
;  Reflects changes made to FIL4_OPEN: -1 on error instead of 0.
;  
;     Rev 1.4   22 Jul 1993 21:19:38   CJKUO
;  Inoculation repair verification now includes a complete Entry verification
;  
;     Rev 1.3   29 Jun 1993 20:30:42   CJKUO
;  If the original file was larger than the file now, don't
;  try to repair.
;  
;     Rev 1.2   20 May 1993 08:14:42   DDREW
;  No change.
;  
;     Rev 1.1   24 Apr 1993 18:51:50   CJKUO
;  Part 1 of verify inoc repair: check magic bytes of infected version to
;  make sure they don't exist in the repaired version (to make sure things
;  that write into the middle of the file do not pass the verification).
;  
;     Rev 1.0   21 Apr 1993 21:20:14   CJKUO
;  Initial revision.
include CTS4.INC

;-----------------------------------------------------------------------------
; INOC_REP_MAIN                         17APR93
;
;       Main routine for inoculation repair.
;
; WORD WINAPI INOC_REP_MAIN(LPSTR Filename,LPSTR Entry,LPSTR WorkBuf,
;                           LPSTR Buffer256);
;
; Input:
;       1)  Filename : Name of file in question
;       2)  Entry    : Inoculation data entry
;       3)  WorkBuf  : Buffer of memory to work in, must be at least 1K.
;       4)  Buffer256: Buffer of 256 bytes
; Output:
;       0       OK
;       nonzero Error
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------
UniqueSegment
INOC_REP_MAIN PROC USES XBX XCX XDX SDS SES XSI XDI,
        Filename:LPSTR,
        Entry:LPSTR,
        WorkBuf:LPSTR,
        Buffer256:LPSTR

        LOCAL Handle:WORD
        LOCAL RepairRetval:WORD
        LOCAL LocalFileSize:DWORD
        LOCAL StartStruct:STARTCODE
;        LOCAL OrigMagic[36]:BYTE        ; Virus file's magic bytes
; Use LocalEntry since it is bigger and does not clash.
;        LOCAL LocalEntry[sFile_Protect]:BYTE ; Place to put new Entry

        ; Change attributes of file so we can repair it!

        INVOKE  FIL4_ATTR, Entry, 0, SET_INFO

        ; Get size of file from inoculation database
        LDS_SI  Entry
        mov     ax,word ptr XDS[XSI].File_Protect.Filesize
        mov     dx,word ptr XDS[XSI].File_Protect.Filesize+2
        mov     word ptr LocalFileSize,ax
        mov     word ptr LocalFileSize+2,dx

        ; Open the file for read/write so we can determine the virus type
        ; and attempt a repair.

        INVOKE  FIL4_OPEN,Entry,fREAD_WRITE
        cmp     ax, -1
        jz      J_Error
        mov     Handle,ax

        INVOKE  FIL4_SIZE,Handle

        ; Did the file grow?  If smaller, forget it.
        sub     dx,word ptr LocalFileSize+2
        jb      J_Error
        ja      J_GoTry
        sub     ax,word ptr LocalFileSize
        jb      J_Error

J_GoTry:
        INVOKE  FIL4_GET_START,Handle,WorkBuf,ADDR StartStruct

        cld
	LDS_SI  WorkBuf
;        SMOV    es,ss                   ; Setup LOCAL variables
;        lea     XDI,OrigMagic
;        lea     XDI,LocalEntry
        LES_DI  Buffer256

        mov     cx,18
        rep     movsw                   ; mov 36 bytes

        ; Call the _repair_type function to determine which type of repair
        ; to do.
        ; AX    = repair type to do
        ; DX:CX = offset to use for repair in prepending/overwriting cases

        LDS_SI  Entry
        mov     ax,word ptr LocalFileSize
        mov     dx,word ptr LocalFileSize+2
        add     XSI,File_Protect.Header
        INVOKE  INOC_REP_TYPE,Handle,vSDS XSI,WorkBuf,dx::ax

        cmp     ax,     1               ; Are we doing a PREPEND type repair
        jne     J_COM_JUMP              ; No - go to COM_JUMP

J_PREPEND:

        ; Use the prepending repair code

        INVOKE  INOC_REP_COM_PREPEND,Handle,WorkBuf,LocalFileSize,dx::cx
        jmp     J_Close

J_COM_JUMP:

        ; Use the com jump (generic) repair code

        LDS_SI  Entry
        add     XSI,File_Protect.Header
        INVOKE  INOC_REP_COM_APPEND,Handle,vSDS XSI,WorkBuf,LocalFileSize

J_Close:
        ; Now we change the date/time back to the orig date/time

        LES_DI  Entry
        INVOKE  FIL4_DATETIME,Handle, XES[XDI].File_Protect.Filedate,
                              XES[XDI].File_Protect.Filetime,SET_INFO

;        INVOKE  INOC_REP_FIND35,Handle,0,ADDR OrigMagic,WorkBuf
;        INVOKE  INOC_REP_FIND35,Handle,0,ADDR LocalEntry,WorkBuf
        INVOKE  INOC_REP_FIND35,Handle,0,Buffer256,WorkBuf
	or      ax,dx
        mov     RepairRetval,ax

        INVOKE  FIL4_CLOSE,Handle

        ; now reset the attributes on the file

        INVOKE  FIL4_ATTR, Entry, XES[XDI].File_Protect.Fileattr, SET_INFO

        mov     ax,RepairRetval
        or      ax,ax
        jnz     J_Error

        ; verify that the new Entry matches the old one.
        ; Must be done after file close, else the file will be opened while
        ;  already open.

                                        ; ------------------------------------
                                        ; Convert from Oem to Ansi since inside
                                        ; FIL4_PROTECT the AnsiToOem conversion
                                        ; is being done.
                                        ; ------------------------------------
IFDEF   WIN16
        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  OEMTOANSI, Entry, Entry
@@:
ENDIF
        INVOKE  FIL4_PROTECT,Entry,PROTECT_CREATE,Buffer256,WorkBuf,Buffer256
                                        ; ------------------------------------
                                        ; Convert from Ansi to Oem so we can
                                        ; continue to work with the file name.
                                        ; ------------------------------------
IFDEF   WIN16
        push    ax

        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  ANSITOOEM, Entry, Entry
@@:
        pop     ax
ENDIF
        or      ax,ax
        jnz     J_Error

;        SMOV    ds,ss
;        lea     XSI,LocalEntry
        LDS_SI  Buffer256

        mov     ax, word ptr XES[XDI].File_Protect.Filesize
        cmp     ax, word ptr [XSI].File_Protect.Filesize
        jne     J_Error
        mov     ax, word ptr XES[XDI].File_Protect.Filesize+2
        cmp     ax, word ptr [XSI].File_Protect.Filesize+2
        jne     J_Error
        mov     ax, XES[XDI].File_Protect.StartCRC
        cmp     ax, [XSI].File_Protect.StartCRC
        jne     J_Error
        mov     ax, XES[XDI].File_Protect.CRCAllBuf
        cmp     ax, [XSI].File_Protect.CRCAllBuf
        jne     J_Error
        mov     ax, XES[XDI].File_Protect.CRCLastK
        cmp     ax, [XSI].File_Protect.CRCLastK
        jne     J_Error
        add     XSI,File_Protect.Header
        add     XDI,File_Protect.Header
        INVOKE  INOC_REP_CMP35,vSDS XSI,vSES XDI
        or      ax,ax
        jnz     J_Error
;       return 0 if OK
        ret

J_Error:
        mov     ax,4
        ret

INOC_REP_MAIN ENDP
;-----------------------------------------------------------------------------
END
