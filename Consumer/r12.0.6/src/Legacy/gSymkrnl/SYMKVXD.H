//----------------------------------------------------------------------
// Copyright 1995 Symantec Corporation
//----------------------------------------------------------------------
//
// $Header:   S:/SYMKRNL/VCS/SYMKVXD.H_v   1.0   26 Jan 1996 20:21:32   JREARDON  
//
// Description:
//   Header file for calling SYMKVxD.VxD services
//
//----------------------------------------------------------------------
// $Log:   S:/SYMKRNL/VCS/SYMKVXD.H_v  $
// 
//    Rev 1.0   26 Jan 1996 20:21:32   JREARDON
// Initial revision.
// 
//    Rev 1.5   28 Jul 1995 15:44:32   BILL
// Merged to trunk
// 
//    Rev 1.4.2.1   27 Jul 1995 03:14:22   LCOHEN
// Added a service which patches VWin32 and fixes a deadlock situation when entering a level 3 lock.
//
//    Rev 1.4.2.0   27 Jun 1995 18:50:06   RJACKSO
// Branch base for version QAK8
//
//    Rev 1.4   05 Mar 1995 12:31:42   HENRI
// Implemented (with the fixes from branch) GetPhysicalInfo VxD function
//
//    Rev 1.3   27 Feb 1995 14:22:44   LCOHEN
// Added internal errors so that SYMKrnl.DLL can process appropriately
//
//    Rev 1.2   02 Feb 1995 21:52:14   LCOHEN
// Updated structures
//
//----------------------------------------------------------------------
#ifndef INCL_VXDCALLS
#define INCL_VXDCALLS

//----------------------------------------------------------------------
// Function numbers
//----------------------------------------------------------------------

#define SYMKVxD_FN_GETVERSION       0
#define SYMKVxD_FN_REGBASED         1
#define SYMKVxD_FN_STKBASED         2
#define SYMKVxD_FN_VXDINT           3
#define SYMKVxD_FN_IOSSRV           4
#define SYMKVxD_FN_GETPHYSICALINFO  5
#define SYMKVxD_FN_PATCHVWIN32      6
//----------------------------------------------------------------------
// Structure used with RegBasedCall and ExecVxDInt (REGBASED and VXDINT)
//----------------------------------------------------------------------
typedef struct
{
    DWORD   CallNum;                    // service or Interrupt number
    DWORD   InEAX, InEBX, InECX, InEDX, InEBP, InESI, InEDI;
    DWORD   OutEAX, OutEBX, OutECX, OutEDX, OutEBP, OutESI, OutEDI;
    DWORD   OutFS, OutGS, OutEFLAGS;
} VxDParams;

//----------------------------------------------------------------------
// Structure used with StkBasedCall (STKBASED)
//----------------------------------------------------------------------
typedef struct
{
    DWORD   S_CallNum;                  // service number
    DWORD   S_NumP;                     // number of parameters
    DWORD   S_P[10];                    // parameter array
    DWORD   S_OutEAX, S_OutEDX;         // results: EAX & EDX
} VxDPushParams;

//----------------------------------------------------------------------
// Structure used with IOSOperation
//----------------------------------------------------------------------
typedef struct
{
    WORD    IOSCmd;                     // IOR.H: IOR_READ or IOR_WRITE
    WORD    Int13Drv;                   // 80h, 81h, 82h... or 00h, 01h,
    DWORD   Sector;                     // starting sector number
    DWORD   SectorCnt;                  // number of sectors to read/write
    LPVOID  Buffer;                     // address of buffer
    DWORD   Result;                     // error codes from IOR.H: IORS_???
} IOSOp;


//----------------------------------------------------------------------
// Structure used with GetPhysicalInfo
//
// NOTE: PDCB is defined in DCB.H in the DDK
//----------------------------------------------------------------------
#ifdef DCB_SUPPORT
typedef struct
{
    WORD            Int13Drv;           // 80h, 81h, 82h... or 00h, 01h,
    PDCB            pDCB;               // Pointer to a Device Control Block structure
    PDCB_BLOCKDEV   pBDD;               // Pointer to BlockDev Device
    DWORD           dwNumDrives;        // number of drives
    DWORD           Result;             // error codes from IOR.H: IORS_???
} VxDDiskGetPhysicalInfo;
#endif

//----------------------------------------------------------------------
// Define IOSOp internal errors (other than the ones defined by IOR.H)
//----------------------------------------------------------------------
#define IOSOP_INTERNALERROR 0xFFFFFFF0
#define IOSOP_NO32BITDRIVER (IOSOP_INTERNALERROR + 0)
#define IOSOP_PAGELOCKERROR (IOSOP_INTERNALERROR + 1)

#define CARRYFLAG           (1 << 0)
#define ZEROFLAG            (1 << 6)
#define OVERFLOWFLAG        (1 << 11)

#endif
