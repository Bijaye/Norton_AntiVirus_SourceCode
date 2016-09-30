;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DBSRCH.ASv   1.7   08 Mar 1996 14:56:34   DALLEE  $
;;
;; Description: Defines DBC4_SEARCH function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DBSRCH.ASv  $
;  
;     Rev 1.7   08 Mar 1996 14:56:34   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.6   05 Apr 1995 13:48:06   DALLEE
;  Fixed bug for 32 bit platforms.
;  Calling INVOKE with a WORD in place of a DWORD parameter assembles to
;   PUSH dword ptr 0, PUSH word ptr WORD.  A total of 6 bytes, not the
;   expected 4.
;  This is bad.
;  
;     Rev 1.5   30 Jan 1995 17:35:38   BRAD
;  Changed some functions to take a size record paramater
;  
;     Rev 1.4   22 Dec 1994 13:56:30   BRAD
;  Added LFN CRC check
;  
;     Rev 1.3   19 Dec 1994 16:33:52   BRAD
;  Fixed up modulo function for 32-bit
;  
;     Rev 1.2   16 Dec 1994 19:14:54   BRAD
;  Converted to 32-bit
;  
;     Rev 1.1   17 Aug 1994 13:25:38   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   30 May 1994 11:16:56   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_SEARCH
;
; Search thru the hashed database for a match.
;
; BOOL WINAPI DBC4_SEARCH ( DBC4_HEADER FAR *Info, void FAR *LookFor,
;                           UINT uLookForSize, void FAR *PutHere );
;
; Input:
;       1)  DBC4_HEADER structure for database header.
;       2)  LookFor, what we need to look for.
;       3)  Size of 'LookFor' item.
;       3)  PutHere, where we'll put it when found.
; Output:
;       1)  BOOL = FALSE(0) Entry not found in database
;                  TRUE(1)  Entry found in database
;----------------------------------------------------------------------------
DBC4_SEARCH  PROC  CODEDISTANCE  USES SDS SES XBX XCX XDX XDI XSI,
        Info:LPBYTE,
        LookFor:LPBYTE,
        uEntrySize:UINT,
        PutHere:LPBYTE

        LOCAL Index_Addr:DWORD
        LOCAL Data_Addr:DWORD
        LOCAL Chain:CHAIN_TYPE
        LOCAL uEntrySizeDifference:UINT


        mov     XAX, uEntrySize
        sub     XAX, sFile_Protect
        mov     uEntrySizeDifference,XAX

        LDS_SI  Info
                                        ; Get the hash value (CRC) of what
                                        ; we're looking for
        INVOKE  DBC4_HASH PTR [XSI].DBC4_HEADER.Hash_Func,LookFor
IF OS_16
        xor     dx, dx
        INVOKE  NUM4_MODULUS,dx::ax,[XSI].DBC4_HEADER.IndexSize
        add     ax,1
        adc     dx,0
        mov     word ptr Index_Addr+2,dx
        mov     word ptr Index_Addr,ax
ELSE
        INVOKE  NUM4_MODULUS,XAX,[XSI].DBC4_HEADER.IndexSize
        inc     eax
        mov     Index_Addr,eax
ENDIF
                                        ; Seek to index entry
        INVOKE  Seek_to_index_entry,Info,Index_Addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Data_Addr,4

IF OS_16
        cmp     word ptr Data_Addr,0
        jnz     J_0
        cmp     word ptr Data_Addr+2,0
        jz      J_Error
ELSE
        cmp     Data_Addr,0
        jz      J_Error
ENDIF
J_0:

                                        ; Seek to data record
        INVOKE  Seek_to_data_record,Info,Data_Addr

        movuw   XAX, [XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,PutHere,XAX

        INVOKE  DBC4_COMP PTR [XSI].DBC4_HEADER.Comp_Func,LookFor,PutHere
        or      XAX,XAX
        jz      J_Success

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Chain,sCHAIN_TYPE
        or      XAX,XAX
        jz      J_Error

IF OS_16
        mov     ax,word ptr Chain.Next
        mov     dx,word ptr Chain.Next+2

        mov     word ptr Data_Addr,ax
        mov     word ptr Data_Addr+2,dx

        cmp     ax,0
        jnz     J_0
        cmp     dx,0
        jnz     J_0
ELSE
        mov     eax,Chain.Next
        mov     Data_Addr,eax
        or      eax,eax
        jnz     J_0
ENDIF

J_Error:
        xor     XAX,XAX
        jmp     SHORT J_Done

J_Success:
        ; If we have LFN CRCs for BOTH entries, make sure they also match.
        mov     XAX,TRUE
                
        LDS_SI  PutHere
        add     XSI,uEntrySizeDifference
        cmp     [XSI].File_Protect.wReserved2,0
        je      J_Done

        mov     bx,[XSI].File_Protect.wReserved2

        LDS_SI  LookFor
        add     XSI,uEntrySizeDifference
        cmp     [XSI].File_Protect.wReserved2,0
        je      J_Done

        cmp     [XSI].File_Protect.wReserved2,bx
        jne     J_Error
        
J_Done:
        ret

DBC4_SEARCH  ENDP

UniqueSegmentEnd
END

