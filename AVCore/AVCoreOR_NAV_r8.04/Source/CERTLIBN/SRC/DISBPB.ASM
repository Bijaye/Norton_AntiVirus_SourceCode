;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DISBPB.ASv   1.2   08 Mar 1996 14:56:38   DALLEE  $
;;
;; Description: Defines DIS4_GET_BPB function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DISBPB.ASv  $
;  
;     Rev 1.2   08 Mar 1996 14:56:38   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:25:42   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   25 May 1994 11:49:00   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DIS4_GET_BPB
;
; Fills in the Bios Parameter Block struct for Drive.
;
; UINT WINAPI DIS4_GET_BPB ( BYTE Drive, BPB_struc *BPB, LPSTR Buffer );
;
; Input:
;       1)  Drive to read bios parameter block.
;       2)  Pointer to BPB struc
;       3)  Work buffer of at least 1024 bytes.
;
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;----------------------------------------------------------------------------
DIS4_GET_BPB  PROC  CODEDISTANCE  USES XSI XDI SDS SES,
        Drive:BYTE,
        BPB:LPBYTE,
        Buffer:LPBYTE

        LDS_SI  Buffer

                                        ; read the bootsector
        INVOKE  DIS4_READ_BOOTSECTOR,Drive,vSDS XSI
        cmp     XAX,-1
        je      Done

                                        ; fill in the Bios Parameter Block
        LES_DI  BPB
        add     XSI,BPB_OFFSET
        mov     XCX,sBPB_struc
        cld
        rep     movsb

        xor     XAX,XAX                 ; all is well
  Done:
        ret

DIS4_GET_BPB  ENDP

UniqueSegmentEnd
END
