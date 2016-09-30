; Copyright (C) 1992-1993 Symantec, Peter Norton Product Group                          
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:/CERTLIB4/VCS/scn_me.asv   1.49   09 Dec 1996 11:59:06   JALLEE  $
;
; Filename:	
; Programmer:	
; Assembler:	MASM 6.0+
; Project:	NAV 3.0
;
; Contents:	
;
; Note:		
;		
; Copyright (C) 1992-1993 Symantec Corporation
;
; $Log:   S:/CERTLIB4/VCS/scn_me.asv  $
;  
;     Rev 1.49   09 Dec 1996 11:59:06   JALLEE
;  Ported from Qak2
;  ;     Rev 1.43.1.10   04 Dec 1995 11:43:08   DCHI
;  ;  Added initialization of count of external requests to zero.
;  ;  Removed IS_ZIP check.
;  ;  
;  ;     Rev 1.43.1.9   09 Nov 1995 15:39:34   DCHI
;  ;  Changed calls to SCN4_VERIFY_NAME to SCN4_GET_NAME when checking
;  ;  of the first word is not necessary.
;  ;  
;  ;     Rev 1.43.1.8   23 Oct 1995 15:48:36   DCHI
;  ;  Readded original SCN4_EXEC function for NONAVEX platforms.
;  ;  
;  ;     Rev 1.43.1.7   10 Oct 1995 15:26:20   DCHI
;  ;  SCN4_EXEC now just returns zero.
;  ;  
;  ;     Rev 1.43.1.6   18 Sep 1995 17:23:52   DCHI
;  ;  Changed calls of FIL4_GET_START to FIL4_NEW_GET_START.
;  
;     Rev 1.48   29 Nov 1994 13:11:28   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.43.1.5   09 Oct 1994 19:45:54   SZIADEH
;  FirstLanding does is not necessarily equate to the offset (in the file) 
;  of the work buffer. The correct offset in the file is required for the
;  implementation of FollowJump(). Thus, a new component was added to 
;  STARTCODE structure called BufferOffset to keep track of such offset.
;  Files affected are: CTS4.INC CTS4.H FSTART.ASM SCN_ME.ASM SCN_ALG.ASM.
;  
;  
;     Rev 1.43.1.4   15 Sep 1994 01:13:42   SZIADEH
;  added FollowJump and ScanMask functionality. 
;  
;     Rev 1.43.1.3   09 Jun 1994 14:17:16   CJKUO
;  Removed code that blows away ExecPath buffer.
;  
;  
;     Rev 1.43.1.2   06 Dec 1993 17:19:28   SZIADEH
;  Added code to support execution of external code in boot detection
;  and repair provided via VIRSPxx.DAT files.
;  
;     Rev 1.43.1.1   01 Nov 1993 15:39:56   MFALLEN
;  Added a global flag that is being checked to see if Ansi to Oem and vice versa
;  conversion should be done on the filename prior to processing.
;  
;     Rev 1.43.1.0   19 Oct 1993 07:26:58   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.43   12 Oct 1993 16:37:12   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.40.1.5   07 Oct 1993 18:43:36   MFALLEN
;  Function accepting ANSI string in Windows.
;  
;     Rev 1.40.1.4   30 Sep 1993 22:20:06   keith
;  Change all INT 21 calls for Windows and OS/2 Mirrors to CALL DOS3Call.
;  Stub the low level disk accessing so that it doesn't GP under OS/2
;  until the functionality is added.
;  
;     Rev 1.40.1.3   08 Sep 1993 15:41:26   CJKUO
;  No more minimum size requirement for buffer, now that there will be
;  at least 32 NULLs
;  
;     Rev 1.40.1.2   07 Sep 1993 19:03:34   CJKUO
;  Reduced buffer requirements to 10 characters from 16 (see 1.39)
;  
;     Rev 1.40.1.1   05 Sep 1993 16:21:22   CGUC
;  Removed the code to turn on HMA since the TSR should have 
;  already dealt with this.  If we attempt to do the HMA code,
;  we'll actually confuse 386MAX and the scan for the virus
;  will not work.
;  
;     Rev 1.40   01 Sep 1993 23:40:02   CAREY
;  fixed callback bug by passing only IP in TSR as opposed to CS:IP
;  
;     Rev 1.39   01 Sep 1993 20:44:06   CAREY
;  Verified that buffer was large enough to scan in (16+) and in the case of
;  whale detection, we must have at least 256 bytes (STARTSIZE).
;  
;     Rev 1.38   25 Aug 1993 13:34:32   CAREY
;  Callbacks are now ONLY applied to EXECUTABLE files (ie, those files which have
;  a wExecFile value of 1).
;  
;     Rev 1.37   25 Aug 1993 13:12:06   CGUC
;  Added comments for new flag that was added.
;  
;     Rev 1.36   25 Aug 1993 12:18:30   CAREY
;  Added new argument to SCN4_ME.
;  
;     Rev 1.35   20 Aug 1993 23:31:08   CJKUO
;  Finished DF_EXEC function with verification!
;  
;     Rev 1.34   19 Aug 1993 23:45:12   CJKUO
;  All the stuff we need for SCN4_EXEC
;  
;     Rev 1.33   09 Aug 1993 06:44:56   BRADR
;  Removed unnecessary XMS code.
;  
;     Rev 1.32   03 Aug 1993 17:58:02   CAREY
;  Fixed CALLBACK's so INSUFF and other MTE samples are correctly being detected.
;  Before the callback was NOT activated if the file was IS_COM (only if IS_VCOM).
;  This has been commented out!
;  
;     Rev 1.31   02 Aug 1993 07:50:24   BRADR
;  Added XMS support to the SDOS16 version of function for TSR.
;  
;     Rev 1.30   26 Jul 1993 12:11:06   BRADR
;  Reflects changes made to FIL4_OPEN: -1 on error instead of 0.
;  
;     Rev 1.29   13 Jul 1993 06:14:30   BRADR
;  Added handling of HMA memory through MEM4_HMA.
;  
;     Rev 1.28   09 Jul 1993 00:43:40   CJKUO
;  Change call to SCN4_ALG to pass handle
;  
;     Rev 1.27   07 Jul 1993 20:26:30   CJKUO
;  If CRC matches, ALG will fail.  --  Fixed.
;  
;     Rev 1.26   25 Jun 1993 15:27:10   CJKUO
;  Save 16 bytes before blowing away buffer, non-NLM
;  
;     Rev 1.25   24 Jun 1993 14:02:06   CJKUO
;  Ifdef'ed so NLM does not re-read file since NLM doesn't blow away the
;  buffers
;  
;     Rev 1.24   15 Jun 1993 10:57:48   CGUC
;  Added near/far to external scan.
;  
;     Rev 1.23   09 Jun 1993 19:11:12   CJKUO
;  Not optimal fix of "continue scan if first CRC match is not a match"
;  
;     Rev 1.22   20 May 1993 09:18:14   CGUC
;  Added check for DEVICE.  If trying to scan a DEVICE in DOS/WIN !NLM, it'll
;  return that there is NO virus found.
;  
;     Rev 1.21   20 May 1993 08:17:42   DDREW
;  No change.
;  
;     Rev 1.20   19 May 1993 18:19:10   DDREW
;  No change.
;  
;     Rev 1.19   16 May 1993 16:36:00   JWELLS
;  Added 4th param to SCNPTR call back
;  
;     Rev 1.18   28 Apr 1993 12:06:50   JMILLARD
;  return internal dos error if virus id of zero detected - this is
;  either a missing fixed id, or some other database problem
;  
;     Rev 1.17   21 Apr 1993 18:26:52   CJKUO
;  Fixed "unreachable code" section.  Fixes "continue CRC check if 
;  first CRC is verified to be incorrect."
;  
;     Rev 1.16   15 Apr 1993 21:50:56   BRAD
;  Changes from Carlton and also incorporated the
;  changes that Joe made.
;  
;     Rev 1.15   13 Apr 1993 16:28:00   GUC_C
;  Removed GLOBAL open of file - NON-NLM of verify database and
;  changed to a pointer of filename passed in.
;  
;     Rev 1.14   23 Mar 1993 14:08:08   JMILLARD
;  Check in Joe's changes
;  Fix report of Whale
;  Add dead and fade custom ids to flag non-virus
;  
;     Rev 1.13   17 Mar 1993 17:54:06   GUC_C
;  Update from J.M.
;  
;     Rev 1.13   16 Mar 1993 20:21:32   JMILLARD
;  bullet-proof null pointer checks for 32-bit land
;  add if NLM virus verify call
;  fix re-check CRC code
;  
;     Rev 1.12   09 Mar 1993 11:01:10   WELLS_J
;  No change.
;  
;     Rev 1.11   05 Mar 1993 12:24:54   WELLS_J
;  Passing of start crc
;  
;     Rev 1.10   26 Feb 1993 17:25:32   GUC_C
;  
;     Rev 1.9   26 Feb 1993 17:20:30   WELLS_J
;  No change.
;  
;     Rev 1.8   26 Feb 1993 17:11:24   WELLS_J
;  No change.
;  
;     Rev 1.7   26 Feb 1993 17:07:30   WELLS_J
;  
;     Rev 1.6   26 Feb 1993 17:03:22   WELLS_J
;  No change.
;  
;     Rev 1.5   26 Feb 1993 17:02:28   WELLS_J
;  
;     Rev 1.4   26 Feb 1993 16:59:50   WELLS_J
;  
;     Rev 1.3   26 Feb 1993 15:01:58   GUC_C
;  No change.
;  
;     Rev 1.2   26 Feb 1993 14:47:38   WELLS_J
;  scn_me.asm -> S:\SOURCE\CERTLIB4\VCS\SCN_ME.ASV
;  Added Whale etc. Skip callbacks for nonverified COMs,SYSs,ZIPs.
;  
;     Rev 1.1   26 Feb 1993 12:59:22   WELLS_J
;  
;     Rev 1.0   23 Feb 1993 15:31:20   GUC_C
;  Initial revision.
;  
;     Rev 1.0   07 Jan 1993 15:13:28   JMILLARD
;  Initial revision.
;  
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; SCN4_ME                                9/18/92                         JOE
;
;       Scan a file for a known virus, using all possible methods.
;
; DWORD WINAPI SCN4_ME(WORD Handle,LPSTR Database,LPSTR Buffer, 
;                       WORD BufferSize,LPSTR Data, LPSTR Extra,
;                       LPSTR Callback,WORD wExecFlag);
;
; Input:
;       1)  Handle of program to scan
;       2)  Pointer to verify database
;       3)  Buffer to work in
;       4)  Size of buffer
;       5)  Data to scan with
;       6)  Extra buffer to place message in
;       7)  Scan callback functions to add to scan ability
;       8)  If file was in EXECUTE list, this is set to TRUE, otherwise FALSE
; Output:
;       1)  LOWORD No Virus = 0 or Virus ID=?
;           HIWORD Virus Control Bits
;       2)  Extra buffer filled
;       3)  LOWORD -1 for error
;-----------------------------------------------------------------------------
SCN4_ME PROC USES XBX XCX XDX SDS SES XSI XDI,
        fhandle:WORD,
        lpVerifyName:LPSTR,
        lpBuffer:LPSTR,
        uBuffer:WORD,
        lpData:LPSTR,
        lpExtra:LPSTR,
        lpCallback:LPSTR,
        wExecFile:WORD

        LOCAL   GetStart:STARTCODE
        LOCAL   Vid:WORD
ifndef NLM
        LOCAL   Magic[16]:BYTE          ; 16 is the length required by
endif
                                        ; ------------------------------------
                                        ; Convert from Ansi to Oem
                                        ; ------------------------------------
IFDEF   WIN16
        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  ANSITOOEM, lpVerifyName, lpVerifyName
@@:
ENDIF

ifndef NLM
        mov     Magic[0],0              ; Note "unused"
        mov     bx,fHandle
        mov     ax,4400h
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        jc      IsDevice
        test    dx,80h
        jz      IsNOTDevice
IsDevice:
        jmp     J_OK
IsNOTDevice:

endif
;-------------------------------------

;ifdef SDOS16
;;----------------------------
;; are we scanning using HMA?
;;----------------------------
;        cmp     word ptr lpData+2, 0ffffh
;        jnz     NotHMA
;
;;----------------------------
;; turn on HMA if not already on
;;----------------------------
;        INVOKE  MEM4_HMA,HMA_STATUS
;        or      ax, ax
;        jnz     HMAOn
;        
;        INVOKE  MEM4_HMA,HMA_ON
;        or      ax, ax
;        jnz     HMAOn
;
;        mov     ax, -1
;        jmp     J_ERROR
;
;HMAOn:
;
;NotHMA:
;endif

ifdef DOS16
; save location of VIRSCAN.DAT, for use with scn_alg's EXEC function
        mov     al,byte ptr cs:ExecPath
        or      al,al
        jnz     @F
        mov     cx,MAX_PATH_SIZE+1
        LDS_SI  lpVerifyName
        push    cs
        pop     es
        mov     di,offset ExecPath
        rep     movsb
@@:
endif

        INVOKE  FIL4_NEW_GET_START,fhandle,lpBuffer,ADDR GetStart

        LDS_SI  lpBuffer                ; point to string
        mov     word ptr [XSI+300h], 0  ; zero out # of external scan requests

        or      ax,ax
        jnz     J_0

;       mov     ax,-1                   ; Generates internal DOS error.
        dec     ax
        jmp     J_ERROR

J_0:
        cmp     GetStart.FileType, IS_WHALE
        jnz     J_W
        LES_DI  lpData
        mov     ax,XES[XDI].HEADER.wFixedIDs + FID_WHALE
        jmp     J_VNOV
J_W:
        xor     XBX,XBX                 ; Start with CRC 0
J_CRCLOOP:
        INVOKE  SCN4_CRC,lpBuffer,uBuffer,lpData,bx
        or      ax,ax
        jz      J_1
        LDS_SI  lpBuffer
        mov     cx,[XSI]                ; First word from BUFFER, must verify
        jmp     J_VIRUS
J_1:    

ifndef NLM
        cmp     Magic[0],0              ; Was lpBuffer blown away?
        je      J_1A
        INVOKE  FIL4_SEEK,fhandle,0,fSEEK_BEG ; If so, refresh
        INVOKE  FIL4_NEW_GET_START,fhandle,lpBuffer,ADDR GetStart
        or      ax,ax
        jnz     J_1A
                                        ; Having been error-free once,
                                        ; nothing should cause an error.
        mov     ax,-1                   ; Generates internal DOS error.
        jmp     J_ERROR

J_1A:
endif

        mov     ax, word ptr GetStart.BufferOffset
        mov     bx, word ptr GetStart.BufferOffset+2

        INVOKE  SCN4_ALG,fhandle,lpBuffer,uBuffer,lpData, bx::ax
        or      ax,ax
        jz      J_2
        jmp     J_VNOV
J_2:
;        cmp     GetStart.FileType,IS_COM
;        je      J_OK
        cmp     GetStart.FileType,IS_SYS
        je      J_OK

; since callbacks are the last detection applied to a file, if we have a
; NON executable file, we just go to J_OK and exit normally.  All other
; detection has been done at this point and we don't have a virus.

        cmp     wExecFile, 0            ; if this is 0, don't do callbacks
        je      J_OK

;------------------------------------- 
        mov     ax,word ptr lpCallback          ; lpCallback NULL?
        or      ax,word ptr lpCallback+2
        jz      J_OK

        LES_DI  lpCallback
ScanMore:
        mov     ax,word ptr XES[XDI]            ; next function pointer NULL?
        or      ax,word ptr XES[XDI+2]
        jz      J_OK
ifdef SDOS16
        INVOKE  SCNPTRTSR PTR XES[XDI],lpBuffer,uBuffer,lpData,ADDR GetStart
else
        INVOKE  SCNPTR PTR XES[XDI],lpBuffer,uBuffer,lpData,ADDR GetStart
endif
        or      ax,ax
        jz      J_3
        jmp     J_VNOV
J_3:

ifdef SDOS16
        add     XDI, 2                  ; IP only
else
        add     XDI, 4                  ; CS:IP
endif
        jmp     ScanMore

;-------------------------------------
;; Get virus name, do verify first word

J_VIRUS:
        cmp     ax,0
        jne     J_4
                                        ; id of zero is invalid - either 
                                        ; a fixed id is missing or some
                                        ; other database error occured.
        mov     ax,-1                   ; Generates internal DOS error.
        jmp     J_ERROR

J_4:
        mov     Vid,ax
         
IFDEF NLM         

        INVOKE  SCN4_VERIFY_VIRUS,cx,Vid,lpData,lpExtra

ELSE

        ; Save 16 bytes
        push    cx
        cld
	LDS_SI  lpBuffer
        SMOV    es,ss                   ; Setup LOCAL variables
        lea     XDI,Magic
        mov     cx,8
        rep     movsw                   ; mov 16 bytes
        pop     cx

;CJK: move the file opens and closes inside SCN4_VERIFY_NAME

        INVOKE  FIL4_OPEN,lpVerifyName,fREAD
        cmp     ax, -1
        jz      J_Error
        push    cx
        push    bx
        mov     bx,ax

        INVOKE  SCN4_VERIFY_NAME,bx,cx,Vid,lpBuffer,lpData,lpExtra

        push    ax
        INVOKE  FIL4_CLOSE,bx
        pop     ax
        pop     bx                      ; bx has virus id
        pop     cx                      ; cx has verify or 0

ENDIF
        cmp     ax,0FADEh               ; Is this a NOT-A-VIRUS sig?
        je      J_OK

        or      ax,ax                   ; was it fail (test or read)
        jz      J_X                     ; not then leave w/vid

J_retEntry:
        mov     ax, Vid                 ; MUST return entry number
        mov     dx, bx                  ; NOT virus ID from N30
        jmp     J_ERROR

;; Get virus name, no verification of first word needed

J_VNOV:
        mov     Vid,ax

IFDEF SYM_NLM
        INVOKE  SCN4_GET_VIRUS,Vid,lpData,lpExtra
ELSE

                                        ; Save 16 bytes
        cld

        LDS_SI  lpBuffer
        SMOV    es,ss                   ; Setup LOCAL variables
        lea     XDI,Magic
        mov     XCX,8
        rep     movsw                   ; mov 16 bytes

        INVOKE  FIL4_OPEN,lpVerifyName,fREAD
        cmp     XAX,-1
        jz      J_Error
        push    XBX
        mov     XBX,XAX

        INVOKE  SCN4_GET_NAME,bx,Vid,lpBuffer,lpData,lpExtra

        push    XAX
        INVOKE  FIL4_CLOSE,XBX
        pop     XAX                     ; ax has virus id
        pop     XBX

ENDIF
        cmp     ax,0FADEh               ; Is this a NOT-A-VIRUS sig?
        je      J_OK

        jmp     J_retEntry              ; Valid sig

J_X:
                                        ; NLM doesn't blow away its buffer
ifndef NLM
        ; Restore 16 bytes
	LES_DI  lpBuffer
        SMOV    ds,ss                   ; Setup LOCAL variables
        lea     XSI,Magic
        mov     cx,8
        rep     movsw                   ; mov 16 bytes
endif

        mov     bx,Vid                  ; Vid is 1-based - when I pass
                                        ; it back to CRC it is treated as
                                        ; an index which effectively 
                                        ; increments it to the next CRC
        LES_DI  lpData
        sub     bx,XES[XDI].HEADER.uNumBOOs
        sub     bx,XES[XDI].HEADER.uNumALGs
        jmp     J_CRCLOOP
;-------------------------------------
J_OK:
        xor     XAX,XAX
        cwd
J_ERROR:
                                        ; ------------------------------------
                                        ; Convert from Oem to Ansi
                                        ; ------------------------------------
IFDEF   WIN16
        push    ax
        push    dx

        INVOKE  GetAnsiFlag             ; Should ANSI to OEM conversion be done
        or      ax, ax                  ;
        jz      @F                      ; NO

        INVOKE  OEMTOANSI, lpVerifyName, lpVerifyName
@@:
        pop     dx
        pop     ax
ENDIF
        ret

SCN4_ME ENDP
;-----------------------------------------------------------------------------

ifdef DOS16

; for use with scn_alg's EXEC function
SCN4_EXEC PROC USES XBX XCX XDX SDS SES XSI XDI,
        Handle:WORD,
        Buffer:LPSTR,
        CurrentBuffer:LPSTR,
        CurrentSig:LPSTR

ifdef NONAVEX

        LOCAL   OurDatHandle:WORD
        LOCAL   ExecCodeStart:DWORD

;check if ExecPath has been set
        cld
        push    cs
        pop     es
        mov     di, offset ExecPath
        cmp     byte ptr es:[di], 0
        jne     @F              ; it has been set
;set it if not.
        LDS_SI  Buffer          ; start of buffer contains path\VIRSCAN.DAT
        mov     cx,MAX_PATH_SIZE+1
; this loop was place on purpose replacing "rep movsb" which caused
; a problem. (snz)

loop_here:
        lodsb
        stosb
        loop loop_here

; search for NULL at end of ...path\VIRSCAN.DAT
@@:
        mov     di,offset ExecPath
        mov     cx,MAX_PATH_SIZE+1
        xor     al,al
        repne   scasb
        sub     di,8                 ; put us at C of VIRSCAN
        mov     al,"P"
        stosb
        LDS_SI  CurrentSig
        inc     si
        lodsb
        dec     al
        stosb
        lodsb
        dec     al
        stosb
        mov     di,offset ExecPath
        INVOKE  FIL4_OPEN,vSES XDI,fREAD
        cmp     ax, -1
        jz      ExecError
        mov     OurDatHandle,ax

; Compute xxxx:0

        LDS_SI  Buffer
        add     si,STARTSIZE*4+15
        and     si,0FFF0h
        mov     cl,4
        shr     si,cl
        mov     cx,ds
        add     si,cx
        mov     word ptr ExecCodeStart,0
        mov     word ptr ExecCodeStart+2,si

; Read in first section and do verification
;from virscan.h
;typedef struct tagVIRSCANDATHEADER
;    {
;    char        szSignature[16];        // Signature "VIRSCAN.DAT"
;    DWORD       dwVersion;              // Numeric version number
;    BYTE        byDay;                  // Day of this version
;    BYTE        byMonth;                // Month of this version
;    WORD        wYear;                  // Year of this version
;    WORD        wNumSigs;               // Number of sigs total
;    WORD        wNumALGs;               // Number of ALG sigs
;    WORD        wNumBOOs;               // Number of BOO sigs
;    WORD        wNumCRCs;               // Number of CRC sigs
;    WORD        wNumREMs;               // Number of REMed out sigs
;    BYTE        byFiller[222];          // Filler to add extra 'stuff'
;    } VIRSCANDATHEADER;

        ; read in VIRSPxx.DAT header
        INVOKE  FIL4_READ, OurDatHandle, ExecCodeStart, uSizeVirscanHeader
        cmp     ax,uSizeVirscanHeader
        jne     ExecError

        LDS_SI  ExecCodeStart
        cmp     [si+4], byte ptr 'P'      ; verifying "VIRSPxx.DAT"
        jne     ExecError
        cmp     [si+8], word ptr 'AD'     ; verifying "VIRSPxx.DAT"
        jne     ExecError

        cmp     word ptr [si+16],4       ; verify version number
        jne     ExecError

        mov     ax,[si+24]                ; get numsigs
        mov     bx,word ptr sN30
        mul     bx
        add     ax,uSizeVirscanHeader
        mov     bx,ax
        adc     dx,0

        ; read past all sigs
        INVOKE  FIL4_SEEK, OurDatHandle, dx::bx,fSEEK_BEG
        cmp     ax,bx
        jne     ExecError

; we only have 3K - 16 bytes to play with
        INVOKE  FIL4_READ, OurDatHandle, ExecCodeStart, 3072-16
        mov     cx,ax                     ; save length
        INVOKE  FIL4_CLOSE, OurDatHandle

        LES_DI  CurrentSig
        inc     di
        mov     ax,es:[di]
        cmp     ax,[si]                   ; first two bytes must match name
                                          ;  both sides of this cmp have +1
        jne     ExecError

; Decrypt code to run
;        LDS_SI  ExecCodeStart
ExecDecrypt:
        sub     [si],byte ptr 1
        inc     si
        loop    ExecDecrypt

        mov     word ptr ExecCodeStart,2  ; start execution at xxxx:2

        mov     dx,Handle
        LDS_SI  Buffer
        LES_DI  CurrentBuffer

        call    ExecCodeStart
        jmp     ExecOK

ExecError:
        xor     ax,ax
ExecOK:
        ret

else

        xor     ax,ax
        ret

endif   ; NONAVEX

SCN4_EXEC ENDP

ExecPath label byte
        db      MAX_PATH_SIZE+1 dup(0)

endif   ; DOS16

END

