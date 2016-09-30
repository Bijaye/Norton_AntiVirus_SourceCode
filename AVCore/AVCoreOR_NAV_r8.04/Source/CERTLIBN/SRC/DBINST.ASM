;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DBINST.ASv   1.13   08 Mar 1996 14:56:32   DALLEE  $
;;
;; Description: Defines DBC4_INSERT function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DBINST.ASv  $
;  
;     Rev 1.13   08 Mar 1996 14:56:32   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.12   11 Jan 1996 17:59:06   JWORDEN
;  Port changes from Quake 8
;  
;     Rev 1.11.1.1   13 Jul 1995 16:49:54   DALLEE
;  Modified find_free_data_entry to perform the space check and only if 
;  we're adding a new entry.  Allows us to remove the seek back and truncate
;  which causes problems with Win 95's NWREDIR.
;  (They've promised to fix, but only in the first in-line).
;  
;     Rev 1.11.1.0   12 Jul 1995 18:58:40   MPYE
;  Branch base for version QAK8
;  
;     Rev 1.11   10 Jul 1995 18:25:10   DALLEE
;  Go back to r1.9 with the crash scanning Win95 NetWare emulators.
;  r1.10 leaves unlinked entries in the middle of NAVINOC.DAT.
;  
;     Rev 1.10   09 Jul 1995 21:10:26   DALLEE
;  Was writing past end of file to check for available space, then seeking
;  backwards and truncating to original length with a 0 byte write.
;  The client of Win95's netware server emulation would chunk hard in this
;  case.  Need more research to convincingly point the finger, though.
;  The truncation is now only done when the first write fails -- this will
;  hide the problem in most cases, but we still need the fix from MS.
;  
;     Rev 1.9   05 Apr 1995 13:48:08   DALLEE
;  Fixed bug for 32 bit platforms.
;  Calling INVOKE with a WORD in place of a DWORD parameter assembles to
;   PUSH dword ptr 0, PUSH word ptr WORD.  A total of 6 bytes, not the
;   expected 4.
;  This is bad.
;  
;     Rev 1.8   30 Jan 1995 17:35:42   BRAD
;  Changed some functions to take a size record paramater
;  
;     Rev 1.7   19 Dec 1994 16:36:22   BRAD
;  Fixed typo
;  
;     Rev 1.6   19 Dec 1994 16:33:50   BRAD
;  Fixed up modulo function for 32-bit
;  
;     Rev 1.5   17 Dec 1994 00:49:02   SZIADEH
;  Fixed error calling NUM4_MODULUS in 16-bit environment.
;  
;     Rev 1.4   16 Dec 1994 21:38:54   BRAD
;  Wasn't passing DWORD to NUM4_MODULUS
;  
;     Rev 1.3   16 Dec 1994 18:30:44   BRAD
;  Converted to 32-bit
;  
;     Rev 1.2   17 Aug 1994 13:25:08   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   30 Jun 1994 17:16:52   CAREY
;  Merged with changes made to QUAKE 2, CERTLIB4 version.
;  
;     Rev 1.0   30 May 1994 11:01:22   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_INSERT
;
; Insert an entry into the database.
;
; BOOL WINAPI DBC4_INSERT ( DBC4_HEADER *Info, void FAR *AddThis,
;                           void FAR *Work );
;
; Input:
;       1)  DBC4_HEADER structure for database header.
;       2)  AddThis, what we need to add
;       3)  Work buffer for add stuff.
; Output:
;       1)  BOOL = FALSE(0) Entry not added.
;                  TRUE(1)  Entry added.
;----------------------------------------------------------------------------
DBC4_INSERT  PROC  CODEDISTANCE  USES SDS SES XBX XCX XDX XDI XSI,
        Info:LPBYTE,
        AddThis:LPBYTE,
        Work:LPBYTE

        LOCAL   Index_Addr:DWORD
        LOCAL   Data_Addr:DWORD
        LOCAL   data_num:DWORD
        LOCAL   new_addr:DWORD
        LOCAL   Retry:UINT
        LOCAL   Depth:UINT
        LOCAL   Quick_Data:WORD
        LOCAL   Chain:CHAIN_TYPE
        LOCAL   dwFilePointer:DWORD
        LOCAL   lpEntry:LPBYTE
        LOCAL   uEntrySizeDifference:UINT


        ;--------------------------------------
        ; If we run out of room on the hard drive,
        ; we must not allow any thing to modify
        ; the header or we will corrupt the database.
        ; Space test occurs in find_free_data_entry and
        ; returns 0 on error.  Assumes nothing writes
        ; to database header before it gets called.
        ;--------------------------------------

        LDS_SI  Info

        XOR     XAX,XAX
        mov     ax, [XSI].DBC4_HEADER.Elem_Size
        sub     ax, sFile_Protect
        mov     uEntrySizeDifference,XAX

                                        ; Get the hash value (CRC) of what
                                        ; we're looking for
        INVOKE  DBC4_HASH PTR [XSI].DBC4_HEADER.Hash_Func,AddThis

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

        INVOKE  Seek_to_index_entry,Info,Index_Addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Data_Addr,4
        cmp     XAX,4
        jne     J_Error

IF OS_16
        cmp     word ptr Data_Addr,0
        jnz     NotFirstEntry
        cmp     word ptr Data_Addr+2,0
        jnz     NotFirstEntry
ELSE
        cmp     Data_Addr,0
        jnz     NotFirstEntry
ENDIF

FirstEntry:

                                        ; Save index
        INVOKE  Find_free_data_entry,info
IF OS_16
        cmp     ax,0
        jne     J_GoodEntry
        cmp     dx,0
        je      J_Error
J_GoodEntry:
        mov     word ptr data_num,ax
        mov     word ptr data_num+2,dx
ELSE
        cmp     eax,0
        je      J_Error

        mov     data_num,eax
ENDIF
        INVOKE  Seek_to_index_entry,info,index_addr
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR data_num,4
        cmp     XAX,4
        jne     J_Error

                                        ; Save quick look up
        INVOKE  Seek_to_quick_entry,info,index_addr

IF OS_16
        mov     ax, word ptr AddThis
        mov     dx, word ptr AddThis+2
        add     ax, uEntrySizeDifference
        mov     word ptr lpEntry, ax
        mov     word ptr lpEntry+2, dx
ELSE
        mov     eax, AddThis
        add     eax, uEntrySizeDifference
        mov     lpEntry, eax
ENDIF
        INVOKE  DBC4_QUICK PTR [XSI].DBC4_HEADER.Quick_Func,lpEntry
        mov     Quick_Data,ax
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR Quick_Data,2
        cmp     XAX,2
        jne     J_Error

                                        ; Save the data record
        INVOKE  Seek_to_data_record,info,data_num

        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,AddThis,XAX
; Fix for W32. Original line: cmp     XAX,[XSI].DBC4_HEADER.Elem_Size
        cmp     ax,[XSI].DBC4_HEADER.Elem_Size
        jne     J_Error

                                        ; Save the chain info
        mov     chain.active,TRUE
IF OS_16
        mov     word ptr chain.next,0
        mov     word ptr chain.next+2,0
ELSE
        mov     chain.next,0
ENDIF
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        jne     J_Error
        jmp     J_Success


NotFirstEntry:
        mov     Retry,50
        mov     Depth,0

dowhile:
        INVOKE  Seek_to_data_record,info,data_addr

        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,Work,XAX

; Fix for W32. Original line: cmp     XAX,[XSI].DBC4_HEADER.Elem_Size
        cmp     ax,[XSI].DBC4_HEADER.Elem_Size
        jne     J_Error

        INVOKE  DBC4_COMP PTR [XSI].DBC4_HEADER.Comp_Func,Work,AddThis
        or      XAX,XAX
        jnz     NotSameKeyField

        INVOKE  Seek_to_data_record,info,data_addr
        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,AddThis,XAX
; Fix for W32. Original line: cmp     XAX,[XSI].DBC4_HEADER.Elem_Size
        cmp     ax,[XSI].DBC4_HEADER.Elem_Size
        jne     J_Error

        cmp     Depth,0                 ; If we replace the top entry,
        jne     J_Success               ; we need to update the Quick lookup.
        INVOKE  Seek_to_quick_entry,info,index_addr

IF OS_16
        mov     ax, word ptr AddThis
        mov     dx, word ptr AddThis+2
        add     ax, uEntrySizeDifference
        mov     word ptr lpEntry, ax
        mov     word ptr lpEntry+2, dx
ELSE
        mov     eax, AddThis
        add     eax, uEntrySizeDifference
        mov     lpEntry, eax
ENDIF
        INVOKE  DBC4_QUICK PTR [XSI].DBC4_HEADER.Quick_Func,lpEntry
        mov     Quick_Data,ax
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR Quick_Data,2
        cmp     XAX,2
        jne     J_Error
        jmp     J_Success

NotSameKeyField:
        INVOKE  Seek_to_data_chain,info,data_addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        jne     J_Error

IF OS_16
        cmp     word ptr chain.next,0
        jnz     NextChain
        cmp     word ptr chain.next+2,0
        jnz     NextChain
ELSE
        cmp     chain.next,0
        jnz     NextChain
ENDIF
        INVOKE  Find_free_data_entry,info
IF OS_16
        cmp     ax,0
        jne     J_GoodEntry2
        cmp     dx,0
        je      J_Error
J_GoodEntry2:
        mov     word ptr new_addr,ax
        mov     word ptr new_addr+2,dx
        mov     word ptr chain.next,ax
        mov     word ptr chain.next+2,dx
ELSE
        cmp     eax,0
        je      J_Error

        mov     new_addr,eax
        mov     chain.next,eax
ENDIF
                                        ; Save new link
        INVOKE  Seek_to_data_chain,info,data_addr
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        jne     J_Error

                                        ; Save data record
        INVOKE  Seek_to_data_record,info,new_addr
        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,AddThis,XAX
; Fix for W32. Original line: cmp     XAX,[XSI].DBC4_HEADER.Elem_Size
        cmp     ax,[XSI].DBC4_HEADER.Elem_Size
        jne     J_Error

                                        ; Save chain record
        mov     chain.active,TRUE
IF OS_16
        mov     word ptr chain.next,0
        mov     word ptr chain.next+2,0
ELSE
        mov     chain.next,0
ENDIF

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        jne     J_Error
        jmp     J_Success

NextChain:

IF OS_16
        mov     ax,word ptr chain.next
        mov     dx,word ptr chain.next+2
        mov     word ptr data_addr,ax
        mov     word ptr data_addr+2,dx
ELSE
        mov     eax,chain.next
        mov     data_addr,eax
ENDIF
        inc     depth

        dec     retry
        cmp     retry,0
        jnz     DoWhile

J_Error:
        xor     XAX,XAX
        jmp     SHORT J_Done
J_Success:
        mov     XAX,TRUE
J_Done:
        ret

DBC4_INSERT  ENDP

UniqueSegmentEnd
END
