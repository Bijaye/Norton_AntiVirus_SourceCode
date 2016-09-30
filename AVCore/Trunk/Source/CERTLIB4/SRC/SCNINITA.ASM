; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\certlib4\vcs\scninita.asv   1.7   29 Nov 1994 13:09:10   DALLEE  $
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
; $Log:   S:\certlib4\vcs\scninita.asv  $
;  
;     Rev 1.7   29 Nov 1994 13:09:10   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.5.1.0   19 Oct 1993 07:16:16   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.5   12 Oct 1993 16:25:50   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.4.2.0   03 Sep 1993 19:27:42   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.4   06 Jul 1993 21:00:34   JMILLARD
;  add decryption of check bytes on load into memory
;  
;     Rev 1.3   20 May 1993 08:11:12   DDREW
;  Changed all UINT to WORD
;  
;     Rev 1.2   11 May 1993 11:16:10   CJKUO
;  No change.
;  
;     Rev 1.1   11 May 1993 11:02:58   CJKUO
;  Enable skipping first 256 bytes of VIRSCAN.DAT file.
;  
;     Rev 1.0   17 Mar 1993 17:53:20   GUC_C
;  Initial revision.
;  
;     Rev 1.0   16 Mar 1993 20:19:44   JMILLARD
;  Initial revision.
;  
;     Rev 1.0   08 Feb 1993 11:33:08   JMILLARD
;  Initial revision.
;  
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCN4_LOAD_CRC_CHECK_DATA                 1/15/93                         CAG
;
;       Load the CRC check data and control data from the database
;
; UINT WINAPI SCN4_LOAD_CRC_CHECK_DATA(UINT Handle,LPSTR Buffer,LPSTR Work);
;
; Input:
;       1)  Handle of opened file
;       2)  Buffer where to store
;       3)  Work buffer, size of an Database Entry
; Output:
;       1)  Count (?)
;       2)  ERROR (-1)
;
; NOTES: THIS ROUTINE IS USED TO BUILD THE IN-MEMORY CHECK DATABASE. The
;        code here needs to be coordinated with scn_20.asm, the routine
;        that does verification from the disk file.
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
SCN4_LOAD_CRC_CHECK_DATA PROC USES XCX SDS SES XSI XDI,
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

        mov     ax,WORD PTR [XSI].N30.lpSig
        dec     ah                      ; *CAREY -> decrypt
        dec     al                      ; *CAREY -> decrypt
        mov     XES[XDI],ax
        inc     XDI
        inc     XDI

        mov     ax,[XSI].N30.wControl
        mov     XES[XDI],ax
        inc     XDI
        inc     XDI

        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
J_Error:
        ret

SCN4_LOAD_CRC_CHECK_DATA ENDP
;-----------------------------------------------------------------------------
END
