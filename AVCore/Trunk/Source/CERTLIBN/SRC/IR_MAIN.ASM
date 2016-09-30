; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIBN/VCS/IR_MAIN.ASv   1.15   08 Mar 1996 14:57:12   DALLEE  $
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
; $Log:   S:/CERTLIBN/VCS/IR_MAIN.ASv  $
;  
;     Rev 1.15   08 Mar 1996 14:57:12   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.14   11 Jan 1996 18:19:40   JWORDEN
;  Port changes from Quake 8
;  
;     Rev 1.13.1.1   14 Jul 1995 01:50:56   KEITH
;  Fix a problem where inoc repair failed to close the file again when
;  the infected file was smaller in size than the original.
;  
;  
;     Rev 1.13.1.0   12 Jul 1995 18:58:22   MPYE
;  Branch base for version QAK8
;  
;     Rev 1.13   19 Jun 1995 09:38:46   KEITH
;  Commented the code and changed a parameter name. Buffer256 can no longer 
;  simply be 256 bytes long as it has to contain the File_ProtectL buffer.
;  INOC_REP_MAIN is passed this buffer which it uses in a recursive call back
;  to FIL4_PROTECT. A bug was caused because on this second call, Buffer256
;  was used as the "Entry" parameter which changed to require the larger
;  File_ProtectL buffer. 
;  
;     Rev 1.12   18 Jun 1995 14:39:34   DALLEE
;  Fixed another 32-bit problem:  Passing BYTE to UINT param w/ INVOKE.
;  Looks like my first chkin of "fixes" was the wrong source?
;  The only change before was in a comment -- looks good now, though.
;  
;  
;     Rev 1.12   18 Jun 1995 14:30:10   DALLEE
;  Fixed another 32-bit problem:  Passing WORD to UINT param w/ INVOKE.
;  Looks as if my first chkin of 32-bit fixes wasn't quite right.
;  
;     Rev 1.11   26 Apr 1995 21:55:30   SZIADEH
;  fixed 32 bit problem.
;  
;     Rev 1.9   26 Apr 1995 17:13:44   RSTANEV
;  When opening and changing the attributes of the file, we should use
;  the I/O filename, not the OEM version!
;
;     Rev 1.8   12 Apr 1995 11:34:36   DALLEE
;  Cleanup for 32-bit platforms.  Mostly to handle 32-bit return values.
;
;     Rev 1.7   30 Jan 1995 17:35:40   BRAD
;  Changed some functions to take a size record paramater
;
;     Rev 1.6   27 Dec 1994 16:54:00   BRAD
;  Use XCX, not CX
;
;     Rev 1.5   27 Dec 1994 15:59:08   BRAD
;  Changed handle parameters from WORD to UINT
;
;     Rev 1.4   20 Dec 1994 17:30:50   BRAD
;  Added another parameter to FIL4_PROTECT
;
;     Rev 1.3   09 Dec 1994 20:30:06   SZIADEH
;  added UniqueSegmentEnd.
;
;     Rev 1.2   09 Dec 1994 17:48:28   SZIADEH
;  deleted all references to ANSITOOEM and OEMTOANSI stuff.
;
;     Rev 1.1   08 Dec 1994 20:27:58   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
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
;-----------------------------------------------------------------------------
; INOC_REP_MAIN                         17APR93
;
;       Main routine for inoculation repair.
;
; WORD WINAPI INOC_REP_MAIN(LPSTR Filename,LPVOID Entry,UINT uEntrySize,
;                           LPVOID WorkBuf,
;                           LPVOID Buffer2);
;
; Input:
;       1)  Filename : Name of file in question
;       2)  Entry    : Inoculation data entry
;       3)  EntrySize: Size of inoculation data entry
;       3)  WorkBuf  : Buffer of memory to work in, must be at least 1K.
;       4)  Buffer2  : Buffer to hold at least sizeof(File_ProtectL).
;                      This buffer is passed back to FIL4_PROTECT as the
;                      "Entry" parameter.
; Output:
;       0       OK
;       nonzero Error
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------

include CTSN.INC
UniqueSegment
INOC_REP_MAIN PROC USES XBX XCX XDX SDS SES XSI XDI,
        Filename:LPSTR,
        Entry:LPBYTE,
        uEntrySize:UINT,
        WorkBuf:LPBYTE,
        Buffer2:LPBYTE

        LOCAL Handle:UINT
        LOCAL RepairRetval:UINT
        LOCAL LocalFileSize:DWORD
        LOCAL StartStruct:STARTCODE
        LOCAL uEntrySizeDifference:UINT


        mov     XAX, uEntrySize
        sub     XAX, sFile_Protect
        mov     uEntrySizeDifference, XAX

        ; Change attributes of file so we can repair it!

        INVOKE  FIL4_ATTR, Filename, 0, SET_INFO

        ; Get size of file from inoculation database
        LDS_SI  Entry
        add     XSI,uEntrySizeDifference
IF OS_16
        mov     ax,word ptr XDS[XSI].File_Protect.Filesize
        mov     dx,word ptr XDS[XSI].File_Protect.Filesize+2
        mov     word ptr LocalFileSize,ax
        mov     word ptr LocalFileSize+2,dx
ELSE
        mov     eax,XDS[XSI].File_Protect.Filesize
        mov     LocalFileSize,eax
ENDIF

        ; Open the file for read/write so we can determine the virus type
        ; and attempt a repair.

        INVOKE  FIL4_OPEN,Filename,fREAD_WRITE
        cmp     XAX, -1
        jz      J_Error
        mov     Handle,XAX

        INVOKE  FIL4_SIZE,Handle

        ; Did the file grow?  If smaller, forget it.
IF OS_16
        sub     dx,word ptr LocalFileSize+2
        jb      J_ErrorClose
        ja      J_GoTry
        sub     ax,word ptr LocalFileSize
        jb      J_ErrorClose
ELSE
        sub     eax,LocalFileSize
        jb      J_ErrorClose
ENDIF

J_GoTry:
        INVOKE  FIL4_GET_START,Handle,WorkBuf,ADDR StartStruct

        cld
        LDS_SI  WorkBuf
        LES_DI  Buffer2

        mov     XCX,18
        rep     movsw                   ; mov 36 bytes

        ; Call the _repair_type function to determine which type of repair
        ; to do.
        ; AX    = repair type to do
        ; DX:CX = offset to use for repair in prepending/overwriting cases

        LDS_SI  Entry
        add     XSI,uEntrySizeDifference
        add     XSI,File_Protect.Header
IF OS_16
        mov     ax,word ptr LocalFileSize
        mov     dx,word ptr LocalFileSize+2
        INVOKE  INOC_REP_TYPE,Handle,vSDS XSI,WorkBuf,dx::ax
ELSE
        mov     eax,LocalFileSize
        INVOKE  INOC_REP_TYPE,Handle,vSDS XSI,WorkBuf,eax
ENDIF

        cmp     XAX, COM_PREPEND_REPAIR ; Are we doing a PREPEND type repair
        jne     J_COM_JUMP              ; No - go to COM_JUMP

J_PREPEND:

        ; Use the prepending repair code

IF OS_16
        INVOKE  INOC_REP_COM_PREPEND,Handle,WorkBuf,LocalFileSize,dx::cx
ELSE
        INVOKE  INOC_REP_COM_PREPEND,Handle,WorkBuf,LocalFileSize,ecx
ENDIF
        jmp     J_Close

J_COM_JUMP:

        ; Use the com jump (generic) repair code

        LDS_SI  Entry
        add     XSI,uEntrySizeDifference
        add     XSI,File_Protect.Header
        INVOKE  INOC_REP_COM_APPEND,Handle,vSDS XSI,WorkBuf,LocalFileSize

J_Close:
        ; Now we change the date/time back to the orig date/time

        LES_DI  Entry
        add     XDI,uEntrySizeDifference
        INVOKE  FIL4_DATETIME,Handle, XES[XDI].File_Protect.Filedate,
                              XES[XDI].File_Protect.Filetime,SET_INFO

        INVOKE  INOC_REP_FIND35,Handle,0,Buffer2,WorkBuf
IF OS_16
        or      ax,dx                   ; We just want to see if non-0.
ENDIF                                   ; Bytes should be at offset 0.
        mov     RepairRetval,XAX

        INVOKE  FIL4_CLOSE,Handle

        ; now reset the attributes on the file

        XOR     XAX, XAX
        mov     al,  XES[XDI].File_Protect.Fileattr
        INVOKE  FIL4_ATTR, Filename, XAX, SET_INFO

        mov     XAX,RepairRetval
        or      XAX,XAX
        ;jnz     J_Error

        ; verify that the new Entry matches the old one.
        ; Must be done after file close, else the file will be opened while
        ; already open.

        ; the second paramter was added to fool FIL4_PROTECT into thinking
        ; that this is the OEM version of file name. Since this entry
        ; will never be written to the database, any pointer here
        ; would suffice. No harm is done to the parameter Entry in the target
        ; function.

        INVOKE  FIL4_PROTECT,Filename,Entry,Entry,PROTECT_CREATE,Buffer2,uEntrySize,WorkBuf,Buffer2

        or      XAX,XAX
        jnz     J_Error

        LDS_SI  Buffer2
        add     XSI,uEntrySizeDifference
IF OS_16
        mov     ax, word ptr XES[XDI].File_Protect.Filesize
        cmp     ax, word ptr [XSI].File_Protect.Filesize
        jne     J_Error
        mov     ax, word ptr XES[XDI].File_Protect.Filesize+2
        cmp     ax, word ptr [XSI].File_Protect.Filesize+2
        jne     J_Error
ELSE
        mov     eax, XES[XDI].File_Protect.Filesize
        cmp     eax, [XSI].File_Protect.Filesize
        jne     J_Error
ENDIF
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
        or      XAX,XAX
        jnz     J_Error

;       return 0 if OK
        ret

J_ErrorClose:
        INVOKE  FIL4_CLOSE,Handle
J_Error:
        mov     XAX,4
        ret

INOC_REP_MAIN ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
