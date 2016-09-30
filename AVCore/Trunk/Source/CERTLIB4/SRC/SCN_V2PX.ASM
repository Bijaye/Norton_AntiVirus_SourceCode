include CTS4.INC

UniqueSegment
;
; (C) Copyright 1993-1994 Symantec, Peter Norton Group                          
;===========================================================================
;
; $Header:   S:\certlib4\vcs\scn_v2px.asv   1.7   29 Nov 1994 13:10:38   DALLEE  $
;
; SCN4_V2P                          3/25/93                            JOE
;
;       Modified detection from Novi for Mark Washburn's viruses.
;
; UINT WINAPI SCN4_V2P(LPSTR lpBuffer, LPSTR lpDATA, STARTCODE Struct)
;
; Input:
;       1) ptr Buffer to examine
;       2) ptr to DATA
;       3) ptr to FileInfo struct from FSTART
;
; Output:
;       1) No Match = 0
;       2) VID from fixed ID in DATA
;=========================================================================
;
; $Log:   S:\certlib4\vcs\scn_v2px.asv  $
;  
;     Rev 1.7   29 Nov 1994 13:10:38   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.4.1.1   05 Jun 1994 13:22:44   JMILLARD
;  fix problem with saving only di that Radoslav found (thanks)
;  add standard file header information
;
;=========================================================================

SCN4_V2P        PROC USES XBX XCX XDX XSI XDI SDS SES,
                lpBuffer:LPSTR,
                uBuffer:WORD,
                lpDATA:LPSTR,
                StartStruct:LPSTR

                LOCAL VID:WORD
;---------------------------------------------------------------------------
        xor     XAX,XAX
        xor     XBX,XBX
        xor     XCX,XCX
        xor     XDX,XDX
        xor     XSI,XSI
        xor     XDI,XDI

;---------------------------------------------------------------------------
; Point to FStart info. Exit if not verified COM. Add 127h and firstlanding

        LDS_SI  lpDATA                             
        mov     ax,[XSI].HEADER.wFixedIDs + FID_V2PX
        mov     VID,ax

        LES_DI  StartStruct
        cmp     XES[XDI].STARTCODE.Filetype,IS_VCOM
        je      @F
        jmp     NoW
@@:
        mov     ax,word ptr XES[XDI].STARTCODE.Firstlanding
        mov     dx,ax
        add     dx,127h                 ; this value is placed in ptr

;---------------------------------------------------------------------------
; Point DS:SI to DATA to get Fixed ID
; Point ES:DI to FStart read buffer

        LES_DI  lpBuffer
        LDS_SI  lpDATA
	
        mov     al,0E2h                 ; Look for Loop
        mov     cx,27h
W1:
        repne   scasb

        jcxz    NoW
W2:     
        mov     al,byte ptr XES[XDI]    ; Neg greater than 0E0h
        cmp     al,0E0h                 ; Is value e0 or above
        jl      W1
        
        LES_DI  lpBuffer                ; restore buffer pointer after 1st search
        mov     cx,25h                  ; range to check

Wloop:
        inc     XDI
        cmp     dx,word ptr XES[XDI]    ; Value Firstlanding + 127h
        je      Wout                 ; does occur in all V2P viruses
        loop    Wloop                ; as start point for decryptor.
        jmp     NoW
Wout:        
        mov     al,byte ptr XES[XDI-1]
        and     al,0F8h                 ; Is it preceeded by a mov reg,word
        cmp     al,0B8h
        je      @F
        loop    Wloop                   ; May be common value so test until
        jmp     NoW                     ; cx = zero to exit.
@@:
        jmp     IsW                     ; Have a washburn

NoW:             
        xor     ax,ax
        jmp     Exunt

IsW:
        mov     ax,VID
Exunt:
        ret

SCN4_V2P        ENDP

        END
