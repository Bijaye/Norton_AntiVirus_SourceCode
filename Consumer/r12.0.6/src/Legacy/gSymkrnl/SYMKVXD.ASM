;=============================================================================
;
; $Header:   S:/SYMKRNL/VCS/SYMKVXD.ASv   1.0   26 Jan 1996 20:21:42   JREARDON  
;
; Description:
;       This is the main source module for SYMKVxD.VxD.  It includes
;       entry points to the control procedure.
;
;=============================================================================
; $Log:   S:/SYMKRNL/VCS/SYMKVXD.ASv  $
;  
;     Rev 1.0   26 Jan 1996 20:21:42   JREARDON
;  Initial revision.
;  
;     Rev 1.9   28 Jul 1995 15:44:40   BILL
;  Merged to trunk
;  
;     Rev 1.8.1.2   27 Jul 1995 04:31:30   LCOHEN
;  Set a flag if we have patched VWin32.
;
;     Rev 1.8.1.1   27 Jul 1995 03:14:16   LCOHEN
;  Added a service which patches VWin32 and fixes a deadlock situation when entering a level 3 lock.
;
;     Rev 1.8.1.0   27 Jun 1995 18:48:54   RJACKSO
;  Branch base for version QAK8
;
;     Rev 1.8   23 Jun 1995 21:00:10   LCOHEN
;  Fixed a problem w/V86MMgr by calling its service in the target VMs context.
;
;     Rev 1.7   19 May 1995 17:37:48   SCOTTP
;  Changed to use symkrnl_modulename
;
;     Rev 1.6   25 Apr 1995 14:09:24   MARKK
;  Bumped version
;
;     Rev 1.5   08 Mar 1995 22:29:28   LCOHEN
;  Added code to clear prefetch queue after self modify code
;
;     Rev 1.4   05 Mar 1995 12:31:40   HENRI
;  Implemented (with the fixes from branch) GetPhysicalInfo VxD function
;
;     Rev 1.3   02 Feb 1995 22:19:04   LCOHEN
;  Converted code to build with Build 314
;
;=============================================================================
.386p
	page 58,132
;=============================================================================
        title   S Y M K R N L   V x D  (SYMKVxD.VxD)
;=============================================================================
;==
;== (C) Copyright SYMANTEC Corp. 1995
;==
;==     Title:   SYMKVxD.VxD - SYMKrnl VxD
;==
;==     Version: 1.00
;==
;==     Date:    February 1, 1995
;==
;==     Author:  Leo Cohen
;==
;=============================================================================
;==
;==	Change Log:
;==
;==     DATE     REVISION DESCRIPTION
;==     -------- -------- ----------------------------------------------------
;==     02/01/95 1.00.01X Original
;==
;=============================================================================
;==
;=============================================================================
SYM_VXD_NONEWSTRUCTS equ TRUE
;=============================================================================
;==	I N C L U D E S
;=============================================================================
        .xlist
        include platform.inc
        include vwin32.inc
        include DCB.inc
        include V86MMgr.inc
        .list
        include SYMKVxD.inc
        include IOSSrv.inc
;=============================================================================
;==     D E V I C E  D E S C R I P T O R  B L O C K
;=============================================================================
Declare_Virtual_Device  %SYMKRNL_MODULENAME, \
                        SYMKVxD_MAJOR_VER,   \
                        SYMKVxD_MINOR_VER,   \
                        SYMKVxD_Control,     \
                        SYMKVxD_Device_ID,   \
                        SYMKVxD_Init_Order,  \
                        SYMKVxD_API,         \
                        SYMKVxD_API
;=============================================================================
;==     P R O T E C T E D   M O D E
;=============================================================================
;=============================================================================
;==	P R O T E C T E D   M O D E   D A T A
;=============================================================================
VxD_PAGEABLE_DATA_SEG

rbcVxDService   dd      0       ; register based current service
sbcVxDService   dd      0       ; stack based current service
sbcStackSpace   dd      0       ; stack based current stack space used

hRegSemaphore   dd      0       ; register based semaphore
hStkSemaphore   dd      0       ; stack based semaphore

VxD_PAGEABLE_DATA_ENDS
;=============================================================================
;==	D E V I C E   C O N T R O L   P R O C E D U R E
;=============================================================================
VXD_LOCKED_CODE_SEG
;=============================================================================
;==
;==  SYMKVxD_Control:
;==
;==  Entry:
;==	EAX = Control call ID
;==
;==  Exit:
;==	CY = 0 OK
;==	     1 error
;==
;==============================================================================
BeginProc SYMKVxD_Control

        cmp     eax, W32_DeviceIOControl
        je      SYMKVxD_DevIOCtrl

        cmp     eax, Device_Init
        je      SYMKVxD_Init

        cmp     eax, SYS_DYNAMIC_DEVICE_INIT
        je      SYMKVxD_Init

        cmp     eax, SYS_DYNAMIC_DEVICE_EXIT
        je      SYMKVxD_Exit

	clc
	ret

EndProc SYMKVxD_Control
VXD_LOCKED_CODE_ENDS

VxD_PAGEABLE_CODE_SEG
;=============================================================================
;==     C A L L   T A B L E   F O R   A P I s : W32, PM, and V86
;=============================================================================
APITable        label   dword
        dd      offset32 APIGetVersion
        dd      offset32 RegBasedCall    ; 1st service: Reg based call
        dd      offset32 StkBasedCall    ; 2nd service: stack based call
        dd      offset32 ExecVxDInt      ; 3rd service: ExecVxDInt
        dd      offset32 IOSOperation    ; 4th service: IOSOperation
        dd      offset32 GetPhysicalInfo ; 5th service: GetPhysicalInfo
        dd      offset32 PatchVWin32     ; 6th service: PatchVWin32
APIMaxFunction  equ  ($-APITable)/4
;=============================================================================
;==     W 3 2  A P I: API to Win32 Applications
;=============================================================================
;=============================================================================
;==
;==  SYMKVxD_DevIOCtrl
;==
;==  Entry:
;==     ESI = ptr to DIOCparams
;==     ECX = dwIOControlCode
;==
;==  Exit:
;==
;=============================================================================
BeginProc SYMKVxD_DevIOCtrl

        cmp     ecx, DIOC_OPEN          ;Q: Is this an open/get version?
        je      short DICopen           ; Y: process

        cmp     ecx, APIMaxFunction     ;Q: Is this function supported
        jae     short DICexit           ; N: ignore

        call    [APITable][ecx*4]       ; call proper routine

        xor     eax, eax                ; indicate success
DICexit:
        ret

DICopen:
        call    APIGetVersion
        mov     ecx, eax        ; version number in ECX
        xor     eax, eax        ; clear EAX to indicate success
        jmp     short DICexit

DICInvalidFunc:
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD - Invalid function called from ring Win32."
endif
        xor     eax, eax                ; indicate error/NULL
        jmp     short DICexit

EndProc SYMKVxD_DevIOCtrl

;=============================================================================
;==     P M  &  V 8 6  A P I: API to Win16 and DOS Applications
;=============================================================================
;=============================================================================
;==
;==  SYMKVxD_API:
;==
;==     This is the entry point for all the services provided to the ring 3
;==     software via the PM or V86 APIs.
;==
;==  Entry:
;==
;==  Exit:
;==
;=============================================================================
BeginProc SYMKVxD_API

        movzx   eax, [ebp].Client_AX    ; get function number in AX
        cmp     ax, APIMaxFunction      ;Q: Valid function
        jae     short AInvalidFunc      ; N: return NULL

        call    [APITable][eax*4]       ; call proper routines
;
;  Place return value in client registers
;
Aexit:
        mov     [ebp].Client_EAX, eax   ; return NULL or handle
	ret

AInvalidFunc:
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD - Invalid function called from V86 or PM API."
endif
        xor     eax, eax                ; indicate error/NULL
        jmp     short Aexit

EndProc SYMKVxD_API

;=============================================================================
;==     A P I   F U N C T I O N S
;=============================================================================
;=============================================================================
;==
;==  APIGetVersion: (SYMKVxD_FN_GETVERSION)
;==
;==  Entry:
;==     none
;==
;==  Exit:
;==     EAX = (major version shl 8) + minor version
;==
;=============================================================================
BeginProc APIGetVersion

        mov     eax, (SYMKVxD_MAJOR_VER shl 8) + SYMKVxD_MINOR_VER
        ret

EndProc APIGetVersion

;=============================================================================
;==
;==  RegBasedCall: (SK_VXD_FN_REGBASED)
;==
;==  Call VMM or VxD function with register parameters
;==
;==  Entry:
;==     [ESI].lpvInBuffer = ptr to VXDParams struct to be used
;==
;==  Exit:
;==     VxDParams OUT regs changed
;==
;=============================================================================
BeginProc RegBasedCall
;
;  Allow only one caller at a time
;
        mov     eax, [hRegSemaphore]
        test    eax, eax                ;Q: Valid semaphore?
        jne     short @f                ; Y: continue
;
;  We need to allocate a semaphore with a token count of one.
;  This protects the routine from being reentered
;
        mov     ecx, 1                  ; single token
        VMMcall Create_Semaphore
        mov     [hRegSemaphore], eax
@@:
        mov     ecx, Block_Svc_Ints
        VMMcall Wait_Semaphore

        mov     esi, [esi].lpvInBuffer  ; esi -> VxDParams
        push    esi

        mov     eax, [esi].CallNum      ; get requested function call number
        cmp     eax, [rbcVxDcall][2]    ;Q: Same as previous call?
        je      short @f                ; Y: make the call
;
;  Due to a bug in V86MMgr_Get_EMS_XMS_Avail, we need to special case it
;
        cmp     eax, @@V86MMgr_Get_EMS_XMS_Avail
        je      short rbcV86MMgr_Get_EMS_XMS_Avail
;
;  Self modify code to make the proper service request
;
        mov     [rbcVxDService], eax
        mov     word ptr  [rbcVxDcall][0], 020CDh
        mov     dword ptr [rbcVxDcall][2], eax
        jmp     short @f                ; this clears prefetch queue
@@:
;
;  Setup registers for the call
;
        mov     eax, [esi].InEAX
        mov     ebx, [esi].InEBX
        mov     ecx, [esi].InECX
        mov     edx, [esi].InEDX
        mov     ebp, [esi].InEBP
        mov     edi, [esi].InEDI
        mov     esi, [esi].InESI

rbcVxDcall:
        dw      20CDh                   ; Int 20h
        dd      0                       ; VxD ID and service number

        xchg    esi, [esp]              ; we need ESI restored immediately
        pushfd                          ; and we don't want to damage the flags
        pop     [esi].OutEFLAGS
        mov     [esi].OutEAX, eax
        mov     [esi].OutEBX, ebx
        mov     [esi].OutECX, ecx
        mov     [esi].OutEDX, edx
        mov     [esi].OutEBP, ebp
        mov     [esi].OutEDI, edi
        mov     [esi].OutFS, fs
        mov     [esi].OutGS, gs
        pop     eax
        mov     [esi].OutESI, eax       ; TODO: This is not necessary
rbcExit:
        mov     eax, [hRegSemaphore]
        VMMcall Signal_Semaphore
        ret
;
;  V86MMgr_Get_EMS_XMS_Avail trashes the VMs registers.  Therefore, we will
;  schedule an event on that VM and execute the function under the VM's context.
;
rbcV86MMgr_Get_EMS_XMS_Avail:
        push    esi                     ; save client VxDParams
        sub     esp, size VxDParams     ; local VxDParams
;
;  Copy client VxDParams to local
;
        mov     edi, esp
        mov     ecx, VxDParams/4
.errnz ((size VxDParams) mod 4)
        rep movsd                       ; copy client VxDParams to local
;
;  Initialize assuming an error or time-out
;
        mov     esi, esp
        xor     eax, eax
        mov     [esi].OutEAX, eax
        mov     [esi].OutECX, eax
        mov     [esi].OutEDX, eax
        mov     [esi].OutEDI, eax
;
;  Insure that client passed a valid VM
;
        mov     ebx, [esi].InEBX        ; VM
        VMMcall Validate_VM_Handle      ;Q: Valid VM?
        jc      short rcbDone           ; N: return

;
;  Schedule a priority VM event
;
        mov     eax, Critical_Section_Boost
;       mov     ebx, [esi].InEBX        ; VM
        mov     ecx, PEF_Time_Out+PEF_Always_Sched ; time-out value in EDI
        mov     edx, esp                ; use local VxDParams as reference data
        mov     esi, offset32 rbcVMcb   ; callback
        mov     edi, 100                ; 1/10th of a second time-out
        VMMcall Call_Priority_VM_Event  ; schedule a priority event on VM
;
;  Block until this event has been completed or timed out
;
        mov     esi, esp
        xor     eax, eax
        xchg    [esi].CallNum, eax      ; indicate no notification
@@:
        VMMcall _BlockOnID, <esi, BLOCK_THREAD_IDLE>
        cmp     [esi].CallNum, 0        ;Q: Did we do the signal?
        jz      short @b                ; N: continue blocking
        mov     [esi].CallNum, eax
;
;  Copy information to client VxDParams
;
rcbDone:
        mov     edi, [esi][size VxDParams] ; get client VxDParams
        mov     ecx, VxDParams/4
.errnz ((size VxDParams) mod 4)
        rep movsd                       ; copy local VxDParams into client

        add     esp, (size VxDParams)+4 ; restore stack
        pop     esi                     ; restore ESI
        jmp     short rbcExit

;
;  If we did not time-out, we are now operating under the VM's context
;
rbcVMcb:
        mov     esi, edx                ; VxDParams
        jc      short @f                ; if CY, we timed out
;
;  Make the VxD service call and record results.
;  Also, to fix the V86MMgr bug, save and restore Client_ECX
;
        mov     ebx, [esi].InEBX        ; VM handle
        push    esi                     ; preserve VxDParams
        push    [ebp].Client_ECX        ; save client ECX
        VxDcall V86MMgr_Get_EMS_XMS_Avail
        pop     [ebp].Client_ECX        ; restore client ECX
        pop     esi                     ; restore VxDParams
        mov     [esi].OutEAX, eax
        mov     [esi].OutEBX, ebx
        mov     [esi].OutECX, ecx
        mov     [esi].OutEDX, edx
        mov     [esi].OutEDI, edi
@@:
;
;  Signal blocked client thread and return
;
        mov     [esi].CallNum, 1        ; indicate we are signaling
        VMMcall _SignalID, <esi>        ; release calling thread
        ret

EndProc RegBasedCall

;=============================================================================
;==
;==  StkBasedCall: (SK_VXD_FN_STKBASED)
;==
;==  Call VMM or VxD function with stack based parameters
;==
;==  Entry:
;==     [ESI].lpvInBuffer = ptr to VXDPushParams struct to be used
;==
;==  Exit:
;==     VxDPushParams OUT regs changed
;==
;=============================================================================
BeginProc StkBasedCall
;
;  Allow only one caller at a time
;
        mov     eax, [hStkSemaphore]
        test    eax, eax                ;Q: Valid semaphore?
        jne     short @f                ; Y: continue
;
;  We need to allocate a semaphore with a token count of one.
;  This protects the routine from being reentered
;
        mov     ecx, 1                  ; single token
        VMMcall Create_Semaphore
        mov     [hStkSemaphore], eax
@@:
        mov     ecx, Block_Svc_Ints
        VMMcall Wait_Semaphore

        mov     esi, [esi].lpvInBuffer  ; esi -> VxDPushParams

        mov     eax, [esi].S_CallNum    ; get requested function call number
        cmp     eax, [sbcVxDcall][2]    ;Q: Same as previous call?
        je      short @f                ; Y: make the call
;
;  Self modify code to make the proper service request
;
        mov     [sbcVxDService], eax
        mov     word ptr  [sbcVxDcall][0], 020CDh
        mov     dword ptr [sbcVxDcall][2], eax
        jmp     short @f                ; this clears prefetch queue
@@:
        mov     ecx, [esi].S_NumP       ; number of arguments
        mov     eax, ecx
        shl     eax, 2                  ; number of bytes
        mov     [sbcStackSpace], eax    ; save to add later to ESP
        jecxz   short sbcVxDcall
        lea     ebx, [esi].S_P          ; EBX = offset into regs struct
        add     ebx, eax                ; going to push in reverse order
@@:
        sub     ebx, 4                  ; get offset to next arg
        push    [ebx]                   ; push next arg
        dec     ecx
        jnz     short @b

sbcVxDcall:
        dw      20CDh                   ; Int 20h
        dd      0                       ; VxD ID and service number

        add     esp, [sbcStackSpace]    ; C calling convention: clean up
        mov     [esi].S_OutEAX, eax
        mov     [esi].S_OutEDX, edx

        mov     eax, [hStkSemaphore]
        VMMcall Signal_Semaphore
        ret

EndProc StkBasedCall

;=============================================================================
;==
;==  ExecVxDInt: (SK_VXD_FN_VXDINT)
;==
;==  Executes a specified int (e.g. int 2f, int 21 ..etc)
;==
;==  Entry:
;==     ESI = ptr to DIOCParams struct
;==           lpvInBuffer = ptr to VXDParams struct to be used
;==
;==  Exit:
;==     VxDParams OUT regs changed
;==
;=============================================================================
BeginProc ExecVxDInt

        mov     esi, [esi].lpvInBuffer  ; esi -> VxDIntParams
        push    esi

        mov     eax, [esi].CallNum
        push    eax

        mov     eax, [esi].InEAX
        mov     ebx, [esi].InEBX
        mov     ecx, [esi].InECX
        mov     edx, [esi].InEDX
        mov     ebp, [esi].InEBP
        mov     edi, [esi].InEDI
        mov     esi, [esi].InESI        ; do ESI last

        VMMcall Exec_VxD_Int

        xchg    esi, [esp]              ; we need ESI restored immediately
        pushfd                          ; and we don't want to damage the flags
        pop     [esi].OutEFLAGS
        mov     [esi].OutEAX, eax
        mov     [esi].OutEBX, ebx
        mov     [esi].OutECX, ecx
        mov     [esi].OutEDX, edx
        mov     [esi].OutEBP, ebp
        mov     [esi].OutEDI, edi
        mov     [esi].OutFS, fs
        mov     [esi].OutGS, gs
        pop     eax
        mov     [esi].OutESI, eax       ; TODO: This is not necessary
        ret

EndProc ExecVxDInt

;=============================================================================
;==
;==  IOSOperation: (SK_VXD_FN_IOSOPERATION)
;==
;==  Entry:
;==
;==  Exit:
;==
;=============================================================================
fPageableBit    equ     0       ; buffer passed needs to be locked
fPageable       equ 1 shl fPageableBit
BeginProc       IOSOperation

        mov     edi, [esi].lpvInBuffer
        mov     ah, byte ptr [edi].IOSCmd
        mov     al, fPageable
        mov     dl, byte ptr [edi].Int13Drv
        mov     ebx, [edi].Sector
        mov     ecx, [edi].SectorCnt
        mov     esi, [edi].Buffer

        call    IOSSrv_DoIO

        mov     [edi].Result, eax

        ret

EndProc IOSOperation

;=============================================================================
;==
;==  GetPhysicalInfo: (SYMKVxD_FN_GETPHYSICALINFO)
;==
;==  Entry:
;==
;==  Exit:
;==
;=============================================================================
fPageableBit    equ     0       ; buffer passed needs to be locked
fPageable       equ 1 shl fPageableBit
BeginProc       GetPhysicalInfo

        mov     edi, [esi].lpvInBuffer
        mov     al, fPageable
        mov     dl, byte ptr [edi].GetDCB_Int13Drv
        mov     ebx, [edi].GetDCB_pDCB
        mov     ecx, [edi].GetDCB_pBDD

        call    IOSSrv_GetInfo

        mov     [edi].GetDCB_numDrives, ecx
        mov     [edi].GetDCB_Result, eax

        ret

EndProc GetPhysicalInfo

;=============================================================================
;==
;==  PatchVWin32: (SYMKVxD_FN_PATCHVWIN32)
;==
;==  Entry:
;==
;==  Exit:
;==
;=============================================================================
pvFlags         dd      0
  fCheckedBit   equ     0       ; set when we attempt to patch
  fPatchedBit   equ     1       ; set if patch succeeded
  fAlreadyBit   equ     2       ; set if VWin32 was already patched????
  fDebugBit     equ     3       ; set if patch was to debug version of VWin32
   fChecked     equ  1 shl fCheckedBit
   fPatched     equ  1 shl fPatchedBit
   fAlready     equ  1 shl fAlreadyBit
   fDebug       equ  1 shl fDebugBit

BeginProc PatchVWin32
        push    esi

        xor     eax, eax                        ; assume failure
        bts     [pvFlags], fCheckedBit          ;Q: Have we tried patching?
        jnc     short PVpatch                   ; N: try patching
        test    [pvFlags], fPatched+fAlready    ;Q: Is VWin32 OK?
        jz      short PVexit                    ; N: return FALSE
        mov     eax, [pvFlags]                  ; Y: return TRUE
PVexit:
        pop     esi
        mov     esi, [esi].lpvInBuffer  ; esi -> BOOL (TRUE if OK)
        mov     [esi], eax              ; result of patch (TRUE or FALSE)
        ret
PVverLow:
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD:PatchVWin32 - VWin32 version is lower than expected -- bad VWin32, not patch."
int 1;debug
endif
        xor     eax, eax                ; return FALSE, do not patch VWin32
        jmp     short PVexit
PVverHi:
ifdef DEBUG
  Trace_Out "WARNING: SYMKVxD:PatchVWin32 - VWin32 version is greater than expected -- assume VWin32 is fixed."
int 1;debug
endif
        mov     eax, [pvFlags]
        bts     eax, fAlreadyBit        ; assume MS fixed the problem
        mov     [pvFlags], eax          ; return TRUE
        jmp     short PVexit
PVnoLocList:
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD:PatchVWin32 - GetVxDLocationList failed -- VWin32 not patched."
int 1;debug
endif
        xor     eax, eax                ; return FALSE, do not patch VWin32
        jmp     short PVexit

;
;  We need to patch VWin32 so that the Kernel32 lock is taken and released
;  during a transition into, and out of, level 2 lock.  The initial release
;  of Win95 (VWin32 version 102h) took and released the Kernel32 lock during
;  a transition into, and out of, level 3 lock.
;
PVpatch:
        VxDcall VWIN32_Get_Version      ; get version for VWin32
        cmp     eax, VWIN32_VERSION     ;Q: Is this the correct version?
        jb      short PVverLow          ; N: if lower version, return FALSE
        ja      short PVverHi           ; N: if higher version, return TRUE
;
;  We have determined that this is the correct version of VWin32 to patch.
;  Get starting address of VWin32 (section 4) by calling VMM_GetVxDLocationList
;
        VMMcall VMM_GetVxDLocationList  ;Q: Get VxD location list?
        jz      short PVnoLocList       ; N: return failure
;
;  Traverse this list until we find VWin32
;
@@:
        mov     ecx, [eax].DLL_DDB              ; get DDB for this VxD
        cmp     [ecx].DDB_Req_Device_Number, VWIN32_DEVICE_ID   ;Q: VWin32?
        je      short @f                                        ; Y: found VWin32
        movzx   ecx, [eax].DLL_NumObjects       ; number of objects
        imul    ecx, size ObjectLocation        ; size of all the structure
        add     ecx, DLL_ObjLocation            ; add offset into structure
        add     eax, ecx                        ; next VxD location list
        dec     edx                             ;Q: Last VxD?
        jnz     short @b                        ; N: try next VxD
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD:PatchVWin32 - Could not find VWin32 in VxD Location List."
int 1;debug
endif
        xor     eax, eax                ; return FALSE, do not patch VWin32
        jmp     PVexit
@@:
;
;  Found VWin32 Device_Location_List, get address for section 4.
;
        movzx   ecx, [eax].DLL_NumObjects       ; number of objects
        cmp     ecx, VWIN32_SECTION             ;Q: Section 4 valid?
        jae     short @f                        ; Y: continue
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD:PatchVWin32 - Could not find section 4 of VWin32 in VxD Location List."
int 1;debug
endif
        xor     eax, eax                ; return FALSE, do not patch VWin32
        jmp     PVexit
@@:
        mov     ecx, VWIN32_SECTION-1   ; section 4 is 4th entry in list
        imul    ecx, size ObjectLocation; calculate offset into section 4
        add     ecx, DLL_ObjLocation    ; add offset into structure
        add     eax, ecx                ; ptr to ObjectLocation
;
;  Make sure our patch is within the section's range
;
        cmp     [eax].OL_Size, DEBUGPATCH2 ;Q: Within range?
        ja      short @f                   ; Y: continue
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD:PatchVWin32 - VWin32 section 4 is too small for patch."
int 1;debug
endif
        xor     eax, eax                ; return FALSE, do not patch VWin32
        jmp     PVexit
@@:
;
;  Get address of section 4 and compare strings
;
        mov     esi, [eax].OL_LinearAddr; get address for this section
        add     esi, RETAILSEQUENCE
        mov     edi, offset32 PVPstart2
        mov     ecx, PVPend2 - PVPstart2
        repe    cmpsb
        jz      short @f
        or      [pvFlags], fDebug       ; assume debug version
        mov     esi, [eax].OL_LinearAddr; get address for this section
        add     esi, DEBUGSEQUENCE
        mov     edi, offset32 PVPstart2
        mov     ecx, PVPend2 - PVPstart2
        repe    cmpsb
        jz      short @f
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD:PatchVWin32 - VWin32 section 4 code sequence did not compare."
int 1;debug
endif
        btr     [pvFlags], fDebugBit    ; assumption was wrong
        xor     eax, eax                ; return FALSE, do not patch VWin32
        jmp     PVexit
@@:
;
;   The code sequence compared, we are reasonably sure that we can patch.
;
        mov     esi, [eax].OL_LinearAddr; get address for this section
        add     esi, RETAILPATCH1       ; assume retail
        test    [pvFlags], fDebug       ;Q: Debug version?
        jz      short @f                ; N: continue
        add     esi, DEBUGPATCH1-RETAILPATCH1
@@:
;
;  Check to see if these bytes have already been patched
;
        cmp     byte ptr [esi], LEVEL2LOCK      ;Q: Patched
        jne     short @f                        ; N: continue
        add     esi, RETAILPATCH2 - RETAILPATCH1
.errnz (DEBUGPATCH2-DEBUGPATCH1)-(RETAILPATCH2-RETAILPATCH1)
        mov     eax, [pvFlags]
        bts     eax, fAlreadyBit                ; indicate VWin32 was already patched
        mov     [pvFlags], eax                  ; if already patched, return TRUE
        cmp     byte ptr [esi], LEVEL1LOCK      ;Q: Patched?
        je      PVexit                          ; Y: return TRUE
        btr     [pvFlags], fAlreadyBit          ; N: already patched
        sub     esi, RETAILPATCH2 - RETAILPATCH1
.errnz (DEBUGPATCH2-DEBUGPATCH1)-(RETAILPATCH2-RETAILPATCH1)
@@:
;
;  Check to make sure first patch matches what we expect
;
        cmp     byte ptr [esi], LEVEL3LOCK      ;Q: This must be a 3?
        je      short @f                        ; Y: continue
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD:PatchVWin32 - VWin32 first patch did not match."
int 1;debug
endif
        xor     eax, eax                ; return FALSE, do not patch VWin32
        jmp     PVexit
@@:
        add     esi, RETAILPATCH2 - RETAILPATCH1
.errnz (DEBUGPATCH2-DEBUGPATCH1)-(RETAILPATCH2-RETAILPATCH1)
;
;  One last check -- check the second patch byte
;
        cmp     byte ptr [esi], LEVEL2LOCK      ;Q: This must be a 2?
        je      short @f                        ; Y: continue
ifdef DEBUG
  Trace_Out "ERROR: SYMKVxD:PatchVWin32 - VWin32 second patch did not match."
int 1;debug
endif
        xor     eax, eax                ; return FALSE, do not patch VWin32
        jmp     PVexit
@@:
;
;  Finally, modify both bytes.
;
;  We should page lock the pages for both of these bytes to insure that no
;  preemption will occur after modifying the first byte, but before the second.
;  This logic only applies if these bytes reside in different pages.
;  NOTE:  The reason we will NOT page lock the pages is because we just touched
;         the bytes we are going to patch.  Therefore, it would take an extremely,
;         and I mean EXTREMELY, low memory condition to cause these pages to not
;         be resident.  In addition,  the proximity of the two bytes would make
;         it unusual that they are not in the same page.  Finally, for the
;         following modification to be a problem, not only would the two bytes
;         reside in different pages, the second page would have to be not
;         resident, and a second thread would have to enter this code path.
;         No other thread will enter this code path because we will own the
;         level 1 lock.  Thus, nobody will be taking the level 2 or 3 except
;         for our app after this has been patched.
;
;
;  Modify patch 2 and 1
;
        mov     byte ptr [esi], LEVEL1LOCK      ; rel Kernel32Lock on releasing level 2
        sub     esi, RETAILPATCH2 - RETAILPATCH1
.errnz (DEBUGPATCH2-DEBUGPATCH1)-(RETAILPATCH2-RETAILPATCH1)
        mov     byte ptr [esi], LEVEL2LOCK      ; aquire Kernel32Lock on level 2

        mov     eax, [pvFlags]
        bts     eax, fPatchedBit                ; indicate we patched VWin32
        mov     [pvFlags], eax                  ; return TRUE
        jmp     PVexit

VWIN32_VERSION  equ     0102h   ; version of VWin32 to patch
VWIN32_SECTION  equ     0004h   ; section of VWin32 we will patch
LEVEL3LOCK      equ     0003h   ; level 3 lock
LEVEL2LOCK      equ     0002h   ; level 2 lock
LEVEL1LOCK      equ     0001h   ; level 1 lock

RETAILSIZE      equ     496Fh   ; size of VWin32, section 4
DEBUGSIZE       equ     5BC2h   ; size of VWin32, section 4

RETAILPATCH1    equ     0A2Bh   ; patch the byte from a value of 3 to 2.
DEBUGPATCH1     equ     0C3Fh   ; patch the byte from a value of 3 to 2.

RETAILPATCH2    equ     0A8Eh   ; patch the byte from a value of 2 to 1.
DEBUGPATCH2     equ     0CA2h   ; patch the byte from a value of 2 to 1.

RETAILSEQUENCE  equ     0A2Ch   ; offset into 4th section of VWin32 (retail)
DEBUGSEQUENCE   equ     0C40h   ; offset into 4th section of VWin32 (debug)

;
;  The following is code which exists in VWin32.
;
PVPstart:
        cmp     edx, 21h
        jne     PVPnotIOCTL
        mov     byte ptr [edi+1Dh], 44h
        cmp     word ptr [edi+1Ch], 440Dh
        jne     PVPnotIOCTL
        cmp     byte ptr [edi+11h], LEVEL3LOCK
.errnz ((RETAILPATCH1-RETAILSEQUENCE)-($-PVPstart2-1))
.errnz (DEBUGPATCH1-DEBUGSEQUENCE)-($-PVPstart2-1)
PVPstart2:
        jnz     short PVPcont
        cmp     word ptr [edi+18h], 84Ah
        jz      short PVPlock
        cmp     word ptr [edi+18h], 84Bh
        jz      short PVPlock
PVPcont:
        cmp     word ptr [edi+18h], 86Ah
        jz      short PVPunlock
        cmp     word ptr [edi+18h], 86Bh
        jz      short PVPunlock
PVPend2:
        jmp     short PVPnotIOCTL
PVPlock:
        db 32h dup (?)
PVPunlock:
        db 3Ch dup (?)
PVPnotIOCTL:

EndProc PatchVWin32

;=============================================================================
;==     N O T I F I C A T I O N  C A L L   B A C K S
;=============================================================================

;=============================================================================
;==     L O C A L   R O U T I N E S
;=============================================================================

;=============================================================================
;==     P R O T E C T E D  M O D E  I N I T / E X I T   C O D E
;=============================================================================
;=============================================================================
;==
;==  SYMKVxD_Init:
;==
;==  Entry:
;==
;==  Exit:
;==       Carry flag clear to indicate success
;==
;=============================================================================
BeginProc SYMKVxD_Init

        clc
        ret

EndProc SYMKVxD_Init

;=============================================================================
;==
;==  SYMKVxD_Exit:
;==
;==  Entry:
;==
;==  Exit:
;==       Carry flag clear to indicate success
;==
;==============================================================================
BeginProc SYMKVxD_Exit

        mov     eax, [hRegSemaphore]
        test    eax, eax                ;Q: Did we allocate this semaphore?
        jz      short @f                ; N: continue
        VMMcall Destroy_Semaphore       ; Y: clean up
@@:
        mov     eax, [hStkSemaphore]
        test    eax, eax                ;Q: Did we allocate this semaphore?
        jz      short @f                ; N: continue
        VMMcall Destroy_Semaphore       ; Y: clean up
@@:
        clc
	ret

EndProc SYMKVxD_Exit

VxD_PAGEABLE_CODE_ENDS
end






