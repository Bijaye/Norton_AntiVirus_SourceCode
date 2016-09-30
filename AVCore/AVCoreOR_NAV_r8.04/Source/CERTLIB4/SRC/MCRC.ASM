; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\certlib4\vcs\mcrc.asv   1.3   29 Nov 1994 13:09:44   DALLEE  $
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
; $Log:   S:\certlib4\vcs\mcrc.asv  $
;  
;     Rev 1.3   29 Nov 1994 13:09:44   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.1.1.0   19 Oct 1993 07:17:26   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.1   12 Oct 1993 16:28:14   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.0.2.0   03 Sep 1993 19:28:10   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.0   23 Feb 1993 15:28:58   GUC_C
;  Initial revision.
;  
;     Rev 1.0   07 Jan 1993 22:07:00   JMILLARD
;  Initial revision.
;  
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_CRC                             12/28/92                        CAG
;
;       Create a CRC of a memory block.
;
; UINT WINAPI MEM4_CRC(WORD Seed,LPSTR Buffer, WORD Count);
;
; Input:
;       1)  Current CRC
;       2)  Buffer to CRC
;       3)  Number of bytes
; Output:
;       1)  New CRC
;
;-----------------------------------------------------------------------------
MEM4_CRC PROC USES XBX XCX XDX SDS XSI XDI,
        Seed:WORD,
        Buffer:LPSTR,
        Count:WORD

        mov     si,Seed
        LDS_BX  Buffer
        xor     ax,ax
$I518:
        mov     al,BYTE PTR XDS[XBX]
        cbw     
        mov     di,ax 
        mov     ax,si
        mov     cl,8
        shl     ax,cl
        mov     dx,si
        shr     dx,cl
        add     ax,dx
        xor     ax,di
        mov     si,ax
        sub     ah,ah
        mov     cl,4
        shr     ax,cl
        xor     si,ax
        mov     ax,si
        mov     dh,al
        sub     dl,dl
        sub     ah,ah
        mov     cl,8
        shr     ax,cl
        add     dx,ax
        mov     cl,4
        shl     dx,cl
        mov     ax,si
        sub     ah,ah
        mov     cl,5
        shl     ax,cl
        xor     dx,ax
        xor     si,dx
        sub     Count,1                ; stop_point
        cmp     Count,0                ; End?
        je      $EXIT_ONE522
        inc     bx
        jmp     $I518


$EXIT_ONE522:
        mov     ax,si

        ret

MEM4_CRC ENDP
;-----------------------------------------------------------------------------
END
