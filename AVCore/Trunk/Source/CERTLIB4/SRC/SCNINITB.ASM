; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\certlib4\vcs\scninitb.asv   1.6   29 Nov 1994 13:09:10   DALLEE  $
;
; Filename:	
; Programmer:	
; Assembler:	MASM 6.0+
; Project:	NAV 3.0
;
; Contents:	
;
; Note:		
;		
; Copyright (C) 1992-1993 Symantec Corporation
;
; $Log:   S:\certlib4\vcs\scninitb.asv  $
;  
;     Rev 1.6   29 Nov 1994 13:09:10   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.4.1.0   19 Oct 1993 07:16:20   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.4   12 Oct 1993 16:25:54   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.3.2.0   03 Sep 1993 19:27:44   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.3   20 May 1993 08:11:14   DDREW
;  Changed all UINT to WORD
;  
;     Rev 1.2   11 May 1993 11:15:02   CJKUO
;  No change.
;  
;     Rev 1.1   11 May 1993 11:01:52   CJKUO
;  Enable skipping first 256 bytes of VIRSCAN.DAT file.
;  
;     Rev 1.0   17 Mar 1993 17:53:38   GUC_C
;  Initial revision.
;  
;     Rev 1.0   16 Mar 1993 20:19:54   JMILLARD
;  Initial revision.
;  
;     Rev 1.0   08 Feb 1993 11:33:20   JMILLARD
;  Initial revision.
;  
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCN4_LOAD_NAMES                 1/15/93                         CAG
;
;       Load the virus names from the database into memory
;
; UINT WINAPI SCN4_LOAD_NAMES(UINT Handle,LPSTR Buffer,LPSTR Work);
;
; Input:
;       1)  Handle of opened file
;       2)  Buffer where to store
;       3)  Work buffer, size of an Database Entry
; Output:
;       1)  Count (?)
;       2)  ERROR (-1)
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
SCN4_LOAD_NAMES PROC USES XCX SDS SES XSI XDI,
        hHandle:WORD,
        lpBuffer:LPSTR,
        lpWork:LPSTR

        xor     XCX,XCX

        LDS_SI  lpWork
        LES_DI  lpBuffer

        INVOKE  FIL4_SEEK,hHandle,uSizeVirscanHeader,fSEEK_BEG

ReadMore:
        INVOKE  FIL4_READ,hHandle,lpWork,sN30
        cmp     ax,-1
        je      J_Error
        cmp     ax,sN30
        jne     Done

        push    XCX

        mov     XCX,SIZE_VIRUSNAME
        LDS_SI  lpWork
        lea     XSI,[XSI].N30.lpVName   ; virus name from here
        cld
@@:
        lodsb
        xor     al,047h                 ; mild encryption
        stosb

        loop    @B

        pop     XCX
        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
J_Error:
        ret

SCN4_LOAD_NAMES ENDP
;-----------------------------------------------------------------------------
END
