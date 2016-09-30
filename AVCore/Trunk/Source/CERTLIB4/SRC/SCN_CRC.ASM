; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIB4/VCS/scn_crc.asv   1.8   09 Dec 1996 11:57:16   JALLEE  $
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
; $Log:   S:/CERTLIB4/VCS/scn_crc.asv  $
;  
;     Rev 1.8   09 Dec 1996 11:57:16   JALLEE
;  Updated from Qak2
;  ;     Rev 1.5.1.3   06 Oct 1995 13:55:04   DCHI
;  ;  Changed SYM_TSR to SDOS16.
;  ;  
;  ;     Rev 1.5.1.2   18 Sep 1995 13:15:44   DCHI
;  ;  Added IFDEF SYM_TSR so that TSR does not get new 64K barrier code.
;  ;  
;  ;     Rev 1.5.1.1   12 Sep 1995 18:16:38   DCHI
;  ;  With new support for INSCANF greater than 64K, CRCs now come
;  ;  right after the header.
;  
;     Rev 1.7   29 Nov 1994 13:09:04   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.5.1.0   19 Oct 1993 07:21:30   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.5   12 Oct 1993 16:32:08   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.4.2.0   03 Sep 1993 19:29:40   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.4   20 May 1993 08:17:34   DDREW
;  No change.
;  
;     Rev 1.3   17 Mar 1993 17:53:56   GUC_C
;  Update from J.M.
;  
;     Rev 1.3   16 Mar 1993 20:20:54   JMILLARD
;  fix problem with missing last signature - broken by 1-base changes
;  
;     Rev 1.2   09 Mar 1993 11:01:14   WELLS_J
;  No change.
;  
;     Rev 1.1   05 Mar 1993 12:25:58   WELLS_J
;  multipass chex
;  
;     Rev 1.0   23 Feb 1993 15:31:16   GUC_C
;  Initial revision.
;  
;     Rev 1.1   07 Jan 1993 22:40:20   JMILLARD
;  Fix 32-bit problems - clearing high word of loop counter, high word of eax
;  added to an address
;  
;     Rev 1.0   07 Jan 1993 15:12:40   JMILLARD
;  Initial revision.
;  
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCN4_CRC                              9/17/92                         JOE
;
;       Scan for a match in the CRC'ed string.
;
; UINT WINAPI SCN4_CRC(LPSTR Buffer, WORD BufferSize, LPSTR Data);
;
; Input:
;       1)  Buffer to look in
;       2)  Size of buffer
;       3)  Data to work with
; Output:
;       1)  No Virus = 0 or Virus ID=?
;
;-----------------------------------------------------------------------------
; UPDATE                               12/1/92				CAG
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;       32 bit, clean.
;
;-----------------------------------------------------------------------------
SCN4_CRC PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        uBuffer:WORD,
        lpData:LPSTR,
        uStartAt:WORD           ; uStartAt will be 0 unless this
                                ; is a second(+) lookup.
        LOCAL crc:WORD
         
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        INVOKE  MEM4_CRC,0,lpBuffer,16

        or      ax,ax                   ; if returned CRC is 0 exit
        jnz     @F
        jmp     OKout
@@:
        mov     crc,ax
;-------------------------------------
        LES_DI  lpData
        xor     XCX,XCX
        mov     cx,XES[XDI].HEADER.uNumCRCs
        sub     cx,uStartAt             ; sub CRCs already checked

IFDEF SDOS16
        xor     XAX,XAX
        mov     XAX,XES[XDI].HEADER.uNumALGs
        mov     XBX,SIZE_SIG
;********************************************************************
;** May be problem when ALGS forces DX to be used in multiplication
;** That will occur when we have over 3855 algorithmic sigs.
;********************************************************************
        mul     XBX
        add     XDI,XAX
ENDIF

        add     XDI,sHEADER             ; ES:DI Now points to where CRC start
;-------------------------------------
        mov     ax,uStartAt             ; Add for a second lookup
        shl     ax,1                    ; Double for sizeof WORD
        add     XDI,XAX

        cld                             ; Scan memory for CRC match
        mov     ax,crc
        repne   scasw
        jne     OKout

;-------------------------------------
        LDS_SI  lpData
        mov     ax,[XSI].HEADER.uNumCRCs
        inc     cx                      ; cx is decremented before compare
                                        ; so it is always one too far
        sub     ax,cx
        add     ax,[XSI].HEADER.uNumBOOs
        add     ax,[XSI].HEADER.uNumALGs
        inc     ax                      ; 1-base virus id

        jmp     STout
OKout:
        xor     ax,ax
STout:
        ret
SCN4_CRC        ENDP
;-----------------------------------------------------------------------------
END

