;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DBSRQ.ASv   1.4   08 Mar 1996 14:56:34   DALLEE  $
;;
;; Description: Defines DBC4_SEARCH_QUICK function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DBSRQ.ASv  $
;  
;     Rev 1.4   08 Mar 1996 14:56:34   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.3   30 Jan 1995 17:35:42   BRAD
;  Changed some functions to take a size record paramater
;  
;     Rev 1.2   19 Dec 1994 16:33:54   BRAD
;  Fixed up modulo function for 32-bit
;  
;     Rev 1.1   17 Aug 1994 13:25:40   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   30 May 1994 11:20:32   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_SEARCH_QUICK
;
; Search thru the hashed database for a match, but use the QUICK
; lookup buffer that was loaded.
;
; UINT WINAPI DBC4_SEARCH_QUICK ( DBC4_HEADER FAR *Info, void FAR *LookFor,
;                                 void FAR *QuickBuffer );
;
; Input:
;       1)  DBC4_HEADER structure for database header.
;       2)  LookFor, what we need to look for.
;       3)  Pointer to the QUICK data in memory.
; Output:
;       1)  UINT = 0 - Entry not found in database
;                  1 - Entry found in database
;                  2 - Not found, look with DBC4_SEARCH
;----------------------------------------------------------------------------
DBC4_SEARCH_QUICK  PROC  CODEDISTANCE  USES SDS SES XBX XCX XDX XDI XSI,
        Info:LPBYTE,
        LookFor:LPBYTE,
        lpBuffer:LPBYTE

        LOCAL   uTestMe:WORD
        LOCAL   lpEntry:LPBYTE
        LOCAL   uEntrySizeDifference:UINT



        LDS_SI  Info

        xor     XAX,XAX
        mov     ax, [XSI].DBC4_HEADER.Elem_Size
        sub     ax, sFile_Protect
        mov     uEntrySizeDifference,XAX
                                        ; Get what we should look for
IF OS_16
        mov     ax, word ptr LookFor
        mov     dx, word ptr LookFor+2
        add     ax, uEntrySizeDifference
        mov     word ptr lpEntry, ax
        mov     word ptr lpEntry+2, dx
ELSE
        mov     eax, LookFor
        add     eax, uEntrySizeDifference
        mov     lpEntry, eax
ENDIF
        INVOKE  DBC4_QUICK PTR [XSI].DBC4_HEADER.Quick_Func,lpEntry
        mov     uTestMe,ax

                                        ; Get the hash value (CRC) of what
                                        ; we're looking for
        INVOKE  DBC4_HASH PTR [XSI].DBC4_HEADER.Hash_Func,LookFor
IF OS_16
        xor     dx, dx
        INVOKE  NUM4_MODULUS,dx::ax,[XSI].DBC4_HEADER.IndexSize
ELSE
        INVOKE  NUM4_MODULUS,XAX,[XSI].DBC4_HEADER.IndexSize
ENDIF

        ; Normally we would "ADD AX,1  ADC DX,0" here if we were going to
        ; access the file.  All Seek_to_xxx functions expect #'s to
        ; be 1 based.  For checking the buffer in memory, we don't want
        ; to do this.   -DALLEE

                                        ; Look in the quick lookup buffer now
        LDS_SI  lpBuffer
        add     XSI,XAX
        add     XSI,XAX

        cmp     [XSI],WORD PTR 0
        je      NotFound
        mov     ax,uTestMe
        cmp     [XSI],ax
        je      PerfectMatch
        mov     XAX,2
        jmp     SHORT J_Done
NotFound:
        xor     XAX,XAX
        jmp     J_Done
PerfectMatch:
        mov     XAX,1
J_Done:
        ret

DBC4_SEARCH_QUICK  ENDP

UniqueSegmentEnd
END
