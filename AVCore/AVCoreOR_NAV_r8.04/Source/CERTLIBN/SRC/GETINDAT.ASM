INCLUDE CTSN.INC

UniqueSegment
;---------------------------------------------------------------------------
; Copyright 1993-1994 Symantec Corporation
;===========================================================================
;
; $Header:   S:/CERTLIBN/VCS/getindat.asv   1.9   07 Aug 1996 15:39:58   DALLEE  $
;
; This function should be called to save critical repair information while
; a virus is in memory.
; Used to be used for saving one-half repair information (new NAVEX scheme
; does not require this).
;
; return TRUE    success
;        FALSE   fail
;---------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/getindat.asv  $
;  
;     Rev 1.9   07 Aug 1996 15:39:58   DALLEE
;  *** &? LOOK back on rev or ONE HALF repair info code! ***
;  
;  Removed code to save one-half info - new navex repair doesn't require this.
;  Left function as a placeholder in case in-memory infection info is
;  required in another inline.
;---------------------------------------------------------------------------

GetInfectionData PROC,
     lpWorkBuffer:LPBYTE,
     lpDatafile: LPSTR,
     dwCookie: DWORD

     xor     XAX, XAX
     ret
GetInfectionData ENDP

UniqueSegmentEnd
end

