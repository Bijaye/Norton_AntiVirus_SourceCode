;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/MCRC.ASv   1.3   08 Mar 1996 14:57:14   DALLEE  $
;;
;; Description: Defines MEM4_CRC function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/MCRC.ASv  $
;  
;     Rev 1.3   08 Mar 1996 14:57:14   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   21 Dec 1994 18:56:12   BRAD
;  Allow count of 0
;  
;     Rev 1.1   17 Aug 1994 13:26:00   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   02 May 1994 18:20:58   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; MEM4_CRC
;
; Create a CRC of a memory block.
;
; WORD WINAPI MEM4_CRC ( WORD Seed, LPVOID Buffer, UINT Count );
;
; Input:
;       1)  Current CRC
;       2)  Buffer to CRC
;       3)  Number of bytes
; Output:
;       1)  New CRC
;
;----------------------------------------------------------------------------
MEM4_CRC  PROC  CODEDISTANCE  USES XBX XCX XDX SDS XSI XDI,
        Seed:WORD,
        Buffer:LPBYTE,
        Count:UINT

        mov     si,Seed
        cmp     Count,0
        je      $EXIT_ONE522

        LDS_BX  Buffer
        xor     XAX,XAX
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
        dec     Count
        jz      $EXIT_ONE522
        inc     XBX
        jmp     $I518

$EXIT_ONE522:
        mov     ax,si

        ret

MEM4_CRC  ENDP

UniqueSegmentEnd
END
