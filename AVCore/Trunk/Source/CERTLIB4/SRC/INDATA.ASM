include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; INSCANB                              1/21/93                         CAG
;
;       Initialize the data segment for evSCANBOOT.
;
; UINT WINAPI INSCANB(LPSTR lpBuffer);
;
; Input:
;       1)  Buffer to place data into.
; Output:
;       1)  Size of buffer used
;       2)  ERROR (-1)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
INSCANB PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR

        LOCAL   FileName[12]:BYTE
        LOCAL   lpN30:N30
        LOCAL   nStatus:WORD

        mov     nStatus,-1
        mov     word ptr FileName[0],'IV'
        mov     word ptr FileName[2],'SR'
        mov     word ptr FileName[4],'AC'
        mov     word ptr FileName[6],'.N'
        mov     word ptr FileName[8],'AD'
        mov     word ptr FileName[10],'T'
        mov     byte ptr FileName[11],0

        INVOKE  MEM4_SET,lpBuffer,0,sHEADER

        INVOKE  FIL4_OPEN,ADDR FileName,fREAD
        cmp     ax, -1
        jz      J_Error
        mov     bx,ax

        LDS_SI  lpBuffer
        add     XSI,sHEADER
        INVOKE  SCN4_LOAD_BOOT,bx,vSDS XSI,ADDR lpN30

        LDS_SI  lpBuffer
        mov     [XSI].HEADER.uNumALGs,ax

        mov     cx,SIZE_BOOTSIG+4       ; 2 for control word & 2 for index
        mul     cx
        add     ax,sHEADER
        mov     nStatus,ax

        INVOKE  FIL4_CLOSE,bx

J_Error:
        mov     ax,nStatus

        ret

INSCANB ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; INSCANM                              1/21/93                         CAG
;
;       Initialize the data segment for evSCANMEMORY.
;
; UINT WINAPI INSCANM(LPSTR lpBuffer);
;
; Input:
;       1)  Buffer to place data into.
; Output:
;       1)  Size of buffer used
;       2)  ERROR (-1)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
INSCANM PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR

        LOCAL   FileName[12]:BYTE
        LOCAL   lpN30:N30
        LOCAL   nStatus:WORD

        mov     nStatus,-1
        mov     word ptr FileName[0],'IV'
        mov     word ptr FileName[2],'SR'
        mov     word ptr FileName[4],'AC'
        mov     word ptr FileName[6],'.N'
        mov     word ptr FileName[8],'AD'
        mov     word ptr FileName[10],'T'
        mov     byte ptr FileName[11],0

        INVOKE  MEM4_SET,lpBuffer,0,sHEADER

        INVOKE  FIL4_OPEN,ADDR FileName,fREAD
        cmp     ax, -1
        jz      J_Error
        mov     bx,ax

        LDS_SI  lpBuffer
        add     XSI,sHEADER
        INVOKE  SCN4_LOAD_MEMORY,bx,vSDS XSI,ADDR lpN30

        LDS_SI  lpBuffer
        mov     [XSI].HEADER.uNumALGs,ax

        mov     cx,SIZE_MEM+2
        mul     cx
        add     ax,sHEADER
        mov     nStatus,ax

        INVOKE  FIL4_CLOSE,bx

J_Error:
        mov     ax,nStatus

        ret

INSCANM ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; INSCANF                              1/21/93                         CAG
;
;       Initialize the data segment for evSCANFILE.
;
; UINT WINAPI INSCANF(LPSTR lpBuffer);
;
; Input:
;       1)  Buffer to place data into.
; Output:
;       1)  Size of buffer used
;       2)  ERROR (-1)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
INSCANF PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR

        LOCAL   FileName[12]:BYTE
        LOCAL   lpN30:N30
        LOCAL   nStatus:WORD
        LOCAL   lpSignatures:DWORD
        LOCAL   lpHeader:FAR PTR HEADER

        mov     nStatus,-1
        mov     word ptr FileName[0],'IV'
        mov     word ptr FileName[2],'SR'
        mov     word ptr FileName[4],'AC'
        mov     word ptr FileName[6],'.N'
        mov     word ptr FileName[8],'AD'
        mov     word ptr FileName[10],'T'
        mov     byte ptr FileName[11],0

        INVOKE  MEM4_SET,lpBuffer,0,sHEADER

        INVOKE  FIL4_OPEN,ADDR FileName,fREAD
        cmp     ax, -1
        jz      J_Error
        mov     bx,ax

        mov     nStatus,sHEADER

; CAG added this to allow for scan on close to store the filename
; in the DATA segment and a additional flag.
        add     nStatus,78              ; Length of filename + path
        add     nStatus,1               ; Length of byte flag

        mov     ax,word ptr lpBuffer+2
        mov     word ptr lpHeader+2,ax
        mov     ax,word ptr lpBuffer
        mov     word ptr lpHeader,ax

        mov     ax,word ptr lpBuffer+2
        mov     word ptr lpSignatures+2,ax
        mov     ax,word ptr lpBuffer
        add     ax,sHEADER
        mov     word ptr lpSignatures,ax

        mov     ax,word ptr lpHeader
        mov     dx,word ptr lpHeader+2
        add     ax,HEADER.wFixedIDs


        INVOKE  SCN4_LOAD_ALG,bx,dx::ax,lpSignatures,ADDR lpN30
        LDS_SI  lpBuffer
        mov     [XSI].HEADER.uNumALGs,ax
        mov     cx,SIZE_SIG
        mul     cx
        add     nStatus,ax
        add     word ptr lpSignatures,ax
        adc     word ptr lpSignatures+2,0

        INVOKE  SCN4_LOAD_CRC,bx,lpSignatures,ADDR lpN30
        LDS_SI  lpBuffer
        mov     [XSI].HEADER.uNumCRCs,ax
        mov     cx,2
        mul     cx
        add     nStatus,ax

        INVOKE  SCN4_COUNT_BOOT,bx,ADDR lpN30
        LDS_SI  lpBuffer
        mov     [XSI].HEADER.uNumBOOs,ax

        INVOKE  SCN4_COUNT_REM,bx,ADDR lpN30
        LDS_SI  lpBuffer
        mov     [XSI].HEADER.uNumREMs,ax

        INVOKE  FIL4_CLOSE,bx

J_Error:
        mov     ax,nStatus

        ret

INSCANF ENDP
;-----------------------------------------------------------------------------
END


