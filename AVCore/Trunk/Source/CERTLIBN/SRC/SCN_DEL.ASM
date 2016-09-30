;-----------------------------------------------------------------------------
; SCN4_DELETE                   MAY 24, 93                          SAMIZ
; $Header:   S:\certlibn\vcs\scn_del.asv   1.2   19 Aug 1994 21:59:22   RSTANEV  $
;
;   Delete definition from VIRSCAN.DAT
; INT WINAPI SCN4_DELETE(UINT Handle,LPSTR lpBuffer, LPSTR lpVirusName);
;
; Input:
;       1)  Handle of opened file
;       2)  Work Buffer
;       3)  Name of Virus to Delete. Must be NULL terminated!
;       4)  Length of virus name.
; Output:
;        1)  AX= -1 if error
;        2)  AX= number of entries deleted (with the same virus name).
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlibn\vcs\scn_del.asv  $
;  
;     Rev 1.2   19 Aug 1994 21:59:22   RSTANEV
;  Closing UniqueSegment.
;
;     Rev 1.1   18 Aug 1994 14:28:18   MFALLEN
;  No change.
;
;     Rev 1.0   21 Jul 1994 17:17:12   MFALLEN
;  Initial revision.
;
;     Rev 1.5   11 Feb 1994 15:24:40   MARKL
;  Qak 2 changes moved to trunk
;
;     Rev 1.4.1.0   19 Oct 1993 07:15:50   SCOTTP
;  Branch base for version QAK2
;
;     Rev 1.4   12 Oct 1993 16:25:10   PATRICKM
;  Baseline from QAK1 branch
;
;     Rev 1.3.2.4   16 Sep 1993 20:48:28   CAREY
;  Now we put in CDCD for CRC in CRC sigs, and 0 in CRC in all other sigs.
;
;     Rev 1.3.2.3   16 Sep 1993 20:29:46   CAREY
;  1. Added error checking after write to virscan.dat
;  2. Move word 0 into first 2 bytes of mem sig.
;  3. Re-do checksum in header.
;
;     Rev 1.3.2.2   16 Sep 1993 06:56:20   CGUC
;  After deleting (rem'ing out) we update the header to reflect
;  the actual number of used viruses in it. (Sort of)
;
;     Rev 1.3.2.1   15 Sep 1993 13:38:56   SZIADEH
;  Corrected the code that deletes FIXED ID viruses by changing XDI to XSI.
;
;
;     Rev 1.3.2.0   03 Sep 1993 19:29:54   SCOTTP
;  Branch base for version QAK1
;
;     Rev 1.3   23 Jun 1993 11:29:14   SZIADEH
;  Added code to delete MTEs, TPE's, etc...
;
;     Rev 1.2   01 Jun 1993 00:22:28   SZIADEH
;  minor modifications.
;
;     Rev 1.1   30 May 1993 18:09:28   DALLEE
;  Changed CurrOffset to DWORD to handle larger file.
;
;     Rev 1.0   24 May 1993 23:37:40   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------

include CTSN.INC

UniqueSegment

SCN4_DELETE PROC CODEDISTANCE USES XCX XDX SDS SES XSI XDI,
        hHandle:UINT,
        lpBuffer:LPSTR,
        lpVirusName:LPSTR

        LOCAL   CurrOffset:DWORD
        LOCAL   sVirusName:BYTE
        LOCAL   wCheckSum:WORD
        LOCAL   wNextWord:WORD
        LOCAL   wOffset:WORD

   ;-----------------------------------------
        xor     XBX, XBX                        ;
        xor     XAX, XAX                        ;
        xor     XCX, XCX

        INVOKE  FIL4_SEEK,hHandle,uSizeVirScanHeader,fSEEK_BEG
IF OS_32
        sub     XAX, sN30
        mov     CurrOffset, XAX                 ; 32 bit move
ELSE
        sub     XAX, sN30
        sbb     XDX, 0
        mov     word ptr CurrOffset, ax         ; 16 bit move
        mov     word ptr CurrOffset+2, dx       ;
ENDIF

        INVOKE  STR4_LENGTH, lpVirusName        ;
        or      XAX, XAX
        jz      ERROR

        mov     sVirusName, al                  ; save length

read_again:
        LES_DI  lpBuffer
        LDS_SI  lpVirusName                     ;
                                                ;
        add     word ptr CurrOffset, sN30       ;current record offset in file
        adc     word ptr CurrOffset+2, 0        ;
        INVOKE  FIL4_READ,hHandle,lpBuffer,sN30 ;read that record
        cmp     XAX, -1                         ;problem?
        je      ERROR                           ;yes
        cmp     XAX,sN30                        ;number of bytes read
        jne     Done                            ;
                                                ;
        cld                                     ;increment
        mov     cl, sVirusName                  ;counter
        add     XDI, N30.lpVName                ;point to start of virus name
J_Dave:                                         ;What's this doing here, David?
        repe    cmpsb                           ;compare virus name
        jnz     read_again                      ;mismatch
                                                ;make sure the name is not
                                                ;simply a subset
        cmp     sVirusName, SIZE_VIRUSNAME      ;
        je      @F

        cmp     byte ptr XES[XDI], 0            ;is it the end
        jz      @F

        jmp     read_again                      ;no!

@@:                                             ;
        inc     XBX                             ;count matches

        LDS_SI  lpBuffer
        mov     word ptr [XSI].N30.wControl, 0DEADh

        mov     word ptr [XSI].N30.wCrc,  0CDCDh

        cmp     word ptr [XSI].N30.wType, CRCtype
        je      @F

        mov     word ptr [XSI].N30.wCrc,  0000h
@@:
        mov     word ptr [XSI].N30.lpSig, 0CDCDh
        mov     word ptr [XSI].N30.lpMem, 0000h
        mov     word ptr [XSI].N30.wFixedID, 0000h  ;this will delete MTEs, TPEs, etc...

        INVOKE  FIL4_SEEK,hHandle,CurrOffset,fSEEK_BEG;current record offset
        INVOKE  FIL4_WRITE,hHandle,lpBuffer,sN30;write record

        cmp     XAX, sN30               ; make sure we wrote everything!
        jne     ERROR

        jmp     read_again

Done:
; Read the one WORD at VIRSCANDATHEADER.wTotalVir.
; Dec by BX.
; Write back out to file.

        INVOKE  FIL4_SEEK,hHandle,VIRSCANDATHEADER.wTotalVir,fSEEK_BEG
        INVOKE  FIL4_READ,hHandle,lpBuffer,2

        LDS_SI  lpBuffer
        sub     [XSI],bx
        INVOKE  FIL4_SEEK,hHandle,VIRSCANDATHEADER.wTotalVir,fSEEK_BEG
        INVOKE  FIL4_WRITE,hHandle,lpBuffer,2

; Here comes our checksum of the header!  Yay!

        mov     wCheckSum,  word ptr 0

        mov     wOffset,    word ptr 0

        ; 128 = # of words in virscan header = uSizeVirScanHeader/2

        mov     XCX, uSizeVirScanHeader/2

        ; first seek to top of file!

        INVOKE  FIL4_SEEK,hHandle,0,fSEEK_BEG

CHECK_LOOP:

        INVOKE  FIL4_READ,hHandle,ADDR wNextWord,2

        cmp     wOffset, VIRSCANDATHEADER.wHeaderCheckSum
        je      SKIP_CHECK_VALUE

        mov     ax, wNextWord
        add     wCheckSum, ax

SKIP_CHECK_VALUE:

        add     wOffset, 2              ; sizeof word

        loop    CHECK_LOOP

        INVOKE  FIL4_SEEK,hHandle,VIRSCANDATHEADER.wHeaderCheckSum,fSEEK_BEG
        INVOKE  FIL4_WRITE,hHandle,ADDR wCheckSum,2

        jmp     Exit

ERROR:
        mov     XBX, -1
EXIT:
        mov     XAX, XBX
        ret

SCN4_DELETE ENDP

UniqueSegmentEnd

END

