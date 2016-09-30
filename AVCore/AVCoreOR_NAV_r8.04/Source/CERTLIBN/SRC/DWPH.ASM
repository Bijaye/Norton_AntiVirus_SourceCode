;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/dwph.asv   1.14   27 Jun 1996 15:58:16   DALLEE  $
;;
;; Description: Defines DIS4_WRITE_PHYSICAL function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/dwph.asv  $
;  
;     Rev 1.14   27 Jun 1996 15:58:16   DALLEE
;  LuigiPlus port:
;  Preserve ES.
;  
;     Rev 1.13   08 Mar 1996 14:56:40   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.12   26 May 1995 00:28:18   SZIADEH
;  removed disk lock/unlock. 
;  
;     Rev 1.11   25 May 1995 09:59:38   RSTANEV
;  Got rid of disk lock/unlock calls for VxD platform.  We need to move these
;  things out.  It is up to the platform to decide whether it needs a lock
;  or not.  And since it's in CERTLIBN/AVAPI, it should be implemented as a
;  callback anyway!
;
;     Rev 1.10   25 May 1995 02:04:00   SZIADEH
;  lock the drive before writing to it.
;
;     Rev 1.9   19 Aug 1994 21:39:24   RSTANEV
;  Using new data segment conventions.
;
;     Rev 1.8   17 Aug 1994 13:26:34   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.7   14 Aug 1994 18:47:22   SZIADEH
;
;     Rev 1.6   08 Aug 1994 18:56:10   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.5   01 Aug 1994 18:08:54   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.4   01 Aug 1994 17:17:32   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.3   29 Jul 1994 13:20:10   CAREY
;
;     Rev 1.2   27 Jul 1994 13:08:52   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.1   26 May 1994 13:58:18   RSTANEV
;  Minor changes.
;
;     Rev 1.0   25 May 1994 17:15:28   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE AVAPI.INC

UniqueDataSegment

EXTERN          C lpgCallBack:DWORD

UniqueDataSegmentEnd

UniqueSegment

;----------------------------------------------------------------------------
; DIS4_WRITE_PHYSICAL
;
; Write a PHYSICAL sector to the drive.
;
; UINT WINAPI DIS4_WRITE_PHYSICAL ( LPBYTE Buffer, BYTE Count, WORD Cylinder,
;                                   BYTE Sector, BYTE Head, BYTE Drive );
;
; Input:
;       1)  Buffer to write sectors from, must be allocated in DOS memory
;       2)  Count, number of sectors to write.
;       3)  Cylinder to write
;       4)  Sector to write
;       5)  Head to write
;       6)  Drive to write
; Output:
;       1)  PASSED (0)
;       2)  ERROR (?) BIOS Error message
;----------------------------------------------------------------------------
  DIS4_WRITE_PHYSICAL  PROC  CODEDISTANCE  USES XBX XCX XDX XSI XDI SDS SES,
        lpBuffer:LPBYTE,
        byCount:BYTE,
        uCylinder:WORD,
        bySector:BYTE,
        byHead:BYTE,
        byDrive:BYTE


IF OS_16
        MOV     SI, SEG lpgCallBack
        MOV     DS, SI
ENDIF


        LDS_SI  <DWORD PTR lpgCallBack>
        INVOKE  [XSI].CALLBACKREV1.SDiskWritePhysical,lpBuffer, byCount, uCylinder, bySector, byHead, byDrive

        xor     ah,ah

IF OS_32
        cwde
ENDIF

        ret
DIS4_WRITE_PHYSICAL  ENDP

UniqueSegmentEnd

END
