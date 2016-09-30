;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/drph.asv   1.11   27 Jun 1996 15:56:46   DALLEE  $
;;
;; Description: Defines DIS4_READ_PHYSICAL function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/drph.asv  $
;  
;     Rev 1.11   27 Jun 1996 15:56:46   DALLEE
;  LuigiPlus port:
;  Preserve ES
;  
;     Rev 1.10   08 Mar 1996 14:56:40   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.9   19 Aug 1994 21:39:58   RSTANEV
;  Using new data segment conventions.
;
;     Rev 1.8   17 Aug 1994 13:25:58   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.7   08 Aug 1994 18:56:08   CAREY
;
;  Fixed 16 bit icky bug!
;
;
;     Rev 1.6   01 Aug 1994 18:08:52   CAREY
;  Added proper code for windows to extern lpgCallBack
;
;     Rev 1.5   01 Aug 1994 17:17:34   DALLEE
;  Added "C" to extern statements
;
;     Rev 1.4   29 Jul 1994 13:20:50   CAREY
;
;     Rev 1.3   27 Jul 1994 13:08:36   CAREY
;  Changed core routines to use callbacks!
;
;
;     Rev 1.2   26 May 1994 13:58:00   RSTANEV
;  No need to save XDX.
;
;     Rev 1.1   26 May 1994 13:53:14   RSTANEV
;  Minor changes.
;
;     Rev 1.0   25 May 1994 17:11:44   RSTANEV
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
; DIS4_READ_PHYSICAL
;
; Read a PHYSICAL sector from a drive.
;
; UINT WINAPI DIS4_READ_PHYSICAL ( LPBYTE Buffer, BYTE Count, WORD Cylinder,
;                                  BYTE Sector, BYTE Head, BYTE Drive );
;
; Input:
;       1)  Buffer to store sectors in, must be allocated in DOS memory.
;       2)  Count, number of sectors to read.
;       3)  Cylinder to read
;       4)  Sector to read
;       5)  Head, to read
;       6)  Drive to read
; Output:
;       1)  PASSED (0)
;       2)  ERROR (?) BIOS Error message
;----------------------------------------------------------------------------

  DIS4_READ_PHYSICAL  PROC  CODEDISTANCE  USES XBX XCX XDX XSI XDI SDS SES,
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
        INVOKE  [XSI].CALLBACKREV1.SDiskReadPhysical,lpBuffer, byCount, uCylinder, bySector, byHead, byDrive

        xor     ah, ah

IF OS_32
        cwde
ENDIF

        ret

  DIS4_READ_PHYSICAL  ENDP

UniqueSegmentEnd

END
