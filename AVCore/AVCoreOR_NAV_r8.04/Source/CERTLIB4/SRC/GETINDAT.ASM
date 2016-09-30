INCLUDE CTS4.INC

UniqueSegment
;----------------------------------------------------------------
; Copyright 1993-1994 Symantec Corporation
;===========================================================================
;
; $Header:   S:/CERTLIB4/VCS/getindat.asv   1.1   07 Aug 1996 12:36:50   DALLEE  $
;
; UINT GetInfectionData (LPSTR lpWorkBuffer, LPSTR lpFileName, DWORD dwCookie );
;
; This function should be called to save critical repair information while
; a virus is in memory.
; Used to be used for saving one-half repair information (new NAVEX scheme
; does not require this).
;
; return TRUE    success
;        FALSE   fail
;
;
;---------------------------------------------------------------------------
; $Log:   S:/CERTLIB4/VCS/getindat.asv  $
;  
;     Rev 1.1   07 Aug 1996 12:36:50   DALLEE
;  *** &? LOOK back one rev for ONE HALF repair info code! ***
;  
;  Removed code to save one-half info - new navex repair doesn't require this.
;  Left function as a placeholder in case in-memory infection info is
;  required in another inline for a different virus.
;  
;  
;     Rev 1.0   29 Nov 1994 13:21:28   DALLEE
;  Reget from Qak2 post NAV.305
;---------------------------------------------------------------------------

GetInfectionData PROC,
     lpWorkBuffer:LPSTR,
     lpDatafile: LPSTR,
     dwCookie: DWORD

     xor     XAX, XAX
     ret
GetInfectionData ENDP

end
