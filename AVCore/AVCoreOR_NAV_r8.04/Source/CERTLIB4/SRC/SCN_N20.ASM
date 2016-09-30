; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIB4/VCS/scn_n20.asv   1.8   09 Dec 1996 12:00:50   JALLEE  $
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
; $Log:   S:/CERTLIB4/VCS/scn_n20.asv  $
;  
;     Rev 1.8   09 Dec 1996 12:00:50   JALLEE
;  Ported from Qak2
;  ;     Rev 1.5.1.2   09 Nov 1995 15:54:08   DCHI
;  ;  Made labels unique in SCN4_GET_NAME.
;  ;  
;  ;     Rev 1.5.1.1   09 Nov 1995 15:40:34   DCHI
;  ;  Changed SCN4_VERIFY_NAME to always check the first word.
;  ;  Added SCN4_GET_NAME to get virus information and name only.
;  
;     Rev 1.7   29 Nov 1994 13:07:20   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.5.1.0   19 Oct 1993 07:25:30   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.5   12 Oct 1993 16:35:48   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.4.2.0   03 Sep 1993 19:26:42   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.4   01 Jul 1993 13:40:24   CJKUO
;  Changed logic to support +1 encryption
;  
;     Rev 1.3   20 May 1993 08:17:44   DDREW
;  No change.
;  
;     Rev 1.2   19 May 1993 18:19:22   DDREW
;  ????
;  
;     Rev 1.1   09 Mar 1993 11:01:56   WELLS_J
;  No change.
;  
;     Rev 1.0   23 Feb 1993 15:31:26   GUC_C
;  Initial revision.
;  
;     Rev 1.2   07 Jan 1993 22:51:32   JMILLARD
;  Add standard header
;  
;     Rev 1.1   07 Jan 1993 22:32:22   JMILLARD
;
;  Fix problem with Motzkin name not reported - if there was no double check
;  of the virus, then we didn't load the pointer to the buffer to get the
;  name
;
;     Rev 1.0   07 Jan 1993 15:12:40   JMILLARD
;  Initial revision.
;  
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCN4_VERIFY_NAME                     9/17/92                         JOE
;
;       Open the verification database to verify a file, and
;       retrieve the virus name.
;
; DWORD WINAPI SCN4_VERIFY_NAME(WORD Handle,WORD First,WORD VID,
;                               LPSTR Buffer,LPSTR Data,LPSTR Extra);
;
; Input:
;       1)  File handle
;       2)  First word of string
;       3)  VID = Virus ID.
;       4)  Buffer to work with.
;       5)  Data to use
;       6)  Where to place extra message
; Output:
;       1)  LOWORD No Virus = 0 or Virus ID=?
;           HIWORD Virus Control Bits
;       2)  Extra message with name of virus
;
;-----------------------------------------------------------------------------
SCN4_VERIFY_NAME PROC USES XBX XCX SDS SES XSI XDI,
        hHandle:WORD,
        firstword:WORD,
        vid:WORD,
        lpBuffer:LPSTR,
        lpData:LPSTR,
        lpExtra:LPSTR

        INVOKE  SCN4_GET_ENTRY,hHandle,vid,lpBuffer
        or      ax,ax
        jz      J_Error

        LDS_SI  lpBuffer                ; need SI in any case, was 3 lines down
        mov     ax,word ptr [XSI].N30.lpSig
        dec     ah                      ; *CAREY -> decrypt
        dec     al                      ; *CAREY -> decrypt
        cmp     ax,firstword    
        je      J_0
        xor     ax,ax
        jmp     J_Error
J_0:
        mov     dx,word ptr [XSI].N30.wControl
        mov     bx,word ptr [XSI].N30.wVirID

        cmp     word ptr lpExtra+2,0    ; Copy name only if buffer exists
        je      J_Done

        LES_DI  lpExtra
        lea     XSI,[XSI].N30.lpVName

        mov     XCX,SIZE_VIRUSNAME
        cld
J_Loop:
        cmp     byte ptr[XSI],0
        je      J_1
        cmp     word ptr[XSI],'  '      ; Look for first double space	
        je      J_1		
        lodsb			        ; move each
        stosb			        ; byte
        loop    J_Loop			
J_1:
        xor     al,al		        ; null terminate name
        stosb

J_Done:
        xchg    bx,ax

J_Error:
        ret

SCN4_VERIFY_NAME ENDP
;---------------------------------------------------------------------------


;-----------------------------------------------------------------------------
; SCN4_GET_NAME                       11/08/95                         DCHI
;
;       Open to retrieve the virus name.
;
; DWORD WINAPI SCN4_GET_NAME(WORD Handle, WORD VID,
;                            LPSTR Buffer, LPSTR Data, LPSTR Extra);
;
; Input:
;       1)  File handle
;       2)  VID = Virus ID.
;       3)  Buffer to work with.
;       4)  Data to use
;       5)  Where to place extra message
; Output:
;       1)  LOWORD No Virus = 0 or Virus ID=?
;           HIWORD Virus Control Bits
;       2)  Extra message with name of virus
;
;-----------------------------------------------------------------------------
SCN4_GET_NAME PROC USES XBX XCX SDS SES XSI XDI,
        hHandle:WORD,
        vid:WORD,
        lpBuffer:LPSTR,
        lpData:LPSTR,
        lpExtra:LPSTR

        INVOKE  SCN4_GET_ENTRY,hHandle,vid,lpBuffer
        or      ax,ax
        jz      JN_Error

        LDS_SI  lpBuffer                ; need SI in any case, was 3 lines down
JN_0:
        mov     dx,word ptr [XSI].N30.wControl
        mov     bx,word ptr [XSI].N30.wVirID

        cmp     word ptr lpExtra+2,0    ; Copy name only if buffer exists
        je      JN_Done

        LES_DI  lpExtra
        lea     XSI,[XSI].N30.lpVName

        mov     XCX,SIZE_VIRUSNAME
        cld
JN_Loop:
        cmp     byte ptr[XSI],0
        je      JN_1
        cmp     word ptr[XSI],'  '      ; Look for first double space	
        je      JN_1
        lodsb			        ; move each
        stosb			        ; byte
        loop    JN_Loop
JN_1:
        xor     al,al		        ; null terminate name
        stosb
JN_Done:
        xchg    bx,ax

JN_Error:
        ret

SCN4_GET_NAME ENDP
;---------------------------------------------------------------------------



END



