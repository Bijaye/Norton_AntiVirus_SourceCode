; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIBN/VCS/IR_CPBAK.ASv   1.5   08 Mar 1996 14:57:12   DALLEE  $
;
; Filename:     IR_CPBAK.ASM
; Programmer:   Jimmy Kuo
; Assembler:    MASM 6.0+
; Project:      NAV 3.0
;
; Contents:     INOC_REP_COPY_BACK
;
; Note:         Copies stuff back to the beginning of the file
;
; $Log:   S:/CERTLIBN/VCS/IR_CPBAK.ASv  $
;  
;     Rev 1.5   08 Mar 1996 14:57:12   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.4   12 Apr 1995 11:35:46   DALLEE
;  Cleanup for 32-bit platforms.  Mostly to handle 32-bit return values.
;  
;     Rev 1.3   27 Dec 1994 15:59:06   BRAD
;  Changed handle parameters from WORD to UINT
;  
;     Rev 1.2   09 Dec 1994 20:30:02   SZIADEH
;  added UniqueSegmentEnd.
;  
;     Rev 1.1   08 Dec 1994 20:27:02   SZIADEH
;  ported from CERTLIB4 to CERTLIBN.
;  
;     Rev 1.4   29 Nov 1994 13:11:30   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:20:32   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:31:06   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.0   03 Sep 1993 19:29:18   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.1   20 May 1993 08:14:44   DDREW
;  No change.
;  
;     Rev 1.0   21 Apr 1993 21:14:36   CJKUO
;  Initial revision.
;-----------------------------------------------------------------------------
; INOC_REP_COPY_BACK                    17APR93
;
;       Given a location, copies from location to beginning of file,
;       length = end-of-file - initial location.
;       NOTE: Does NOT truncate file when finished.
;
; WORD WINAPI INOC_REP_CMP35(WORD Handle,LPVOID WorkBuf,DWORD OffsetPtr);
;
; Input:
;       1)  Handle : File Handle
;       2)  WorkBuf : Buffer to work in, must be at least 1024 bytes.
;       3)  ReadPtr : Location to start copying to beginning of file.
; Output:
;       No error code because the routines I call don't give me an error
;       code.
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------

include CTSN.INC

UniqueSegment

INOC_REP_COPY_BACK PROC USES XDX SDS SES XSI XDI,
        Handle:UINT,
        WorkBuffer:LPBYTE,
        ReadPtr:DWORD

        LOCAL uCountRead:UINT
        LOCAL WritePtr:DWORD

IF OS_16
        mov     word ptr WritePtr,0
        mov     word ptr WritePtr+2,0
ELSE
        mov     WritePtr,0
ENDIF


J_Read:
        INVOKE  FIL4_SEEK,Handle,ReadPtr,fSEEK_BEG
        INVOKE  FIL4_READ,Handle,WorkBuffer,1024
        or      XAX,XAX                 ; Bytes read
        jz      J_Done
        mov     uCountRead,XAX

        INVOKE  FIL4_SEEK,Handle,WritePtr,fSEEK_BEG
        INVOKE  FIL4_WRITE,Handle,WorkBuffer,uCountRead

        mov     XAX,uCountRead
IF OS_16
        add     word ptr WritePtr, XAX
        adc     word ptr WritePtr+2, 0
        add     word ptr ReadPtr, XAX
        adc     word ptr ReadPtr+2, 0
ELSE
        add     WritePtr, XAX
        add     ReadPtr, XAX
ENDIF
        jmp     J_Read

;--------------------------------------
J_Done:
        ret

INOC_REP_COPY_BACK ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
