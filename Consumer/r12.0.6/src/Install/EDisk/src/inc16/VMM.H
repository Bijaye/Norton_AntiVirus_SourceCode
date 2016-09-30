// Copyright 1991-1993 Symantec Corporation
/************************************************************************
 *                                                                      *
 * $Header:   V:/INCLUDE/VCS/VMM.H_V   1.20   17 Mar 1993 17:13:26   JOHN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   V:/INCLUDE/VCS/VMM.H_V  $ *
 * 
 *    Rev 1.20   17 Mar 1993 17:13:26   JOHN
 * Disable VMM_DEBUGGER and DETAILED_VMM_ERRORS prior to product ship.
 * 
 *    Rev 1.19   09 Mar 1993 09:46:10   CRAIG
 * Re-ported from Quake.
 ************************************************************************/

#ifndef  INCL_VMM
#define  INCL_VMM

#ifndef	INCL_WINGLUE
#include <winglue.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * Define MEMORY_TRACKING to enable filename/line number tracking   *
 ************************************************************************/

// #define MEMORY_TRACKING

/************************************************************************
 * Define VMM_DEBUGGER to enable the VMM debugger dialog		*
 ************************************************************************/

// #define VMM_DEBUGGER

/************************************************************************
 * Define DETAILED_VMM_ERRORS to display descriptive VMM error dialogs	*
 ************************************************************************/

// #define DETAILED_VMM_ERRORS

/************************************************************************
 * VMM Flags (legal range: 0x10 to 0x2000, because the low-order nibble *
 * is the slack and the high-order two bits are the pool location)      *
 ************************************************************************/

#define	VMF_LOCK_LOW		0x0010	// Always lock this handle low
#define	VMF_LOCK_IN_PLACE	0x0020	// Always lock this handle in place
#define	VMF_NOSWAP		0x0040	// Never swap this handle out of
    					// memory
#define	VMF_HANDLE_TABLE	0x0080	// Block is a handle table
#define VMF_TRY_TO_KEEP         0x0100  // Try to keep this handle in
                                        // memory when we need to move it

#define VM_FLAGS_MASK           0x3FF0  // Mask for all the flag bits

/************************************************************************
 * Misc. Constants                                                      *
 ************************************************************************/

#define ALL_MEMORY              0xFFFFFFFF      /* Use all available memory     */
#define MALLOC_THE_FARM         ALL_MEMORY

enum __VMErrors
    {
    VM_IS_OK = 0,
    VM_BAD_SIGNATURE,
    VM_HANDLE_MISMATCH,
    VM_BAD_FREE_CHAIN,
    VM_LOST_FREEBLOCK,
    VM_ADJACENT_FREEBLOCKS,
    VM_SHORT_ALLOCATION,
    VM_LOCK_OF_FREE,
    VM_FLAGS_MISMATCH,
    VM_LONG_BLOCK,
    VM_FREE_OF_FREED,
    VM_UNLOCK_OF_UNLOCKED,
    VM_NULL_HANDLE_ACCESS,
    VM_LOCK_FAILED,
    VM_N_ERRORS
    };

struct XMSMoveRegion;
struct EMSMoveRegion;

/************************************************************************
 * Prototypes                                                           *
 ************************************************************************/

/* Function definitions in vmmconv.c */

HGLOBAL WINAPI		      VMMAlloc		 (DWORD dwSize);
HGLOBAL WINAPI                VMMAllocFlags      (DWORD dwSize, WORD wFlags);
LPVOID WINAPI		      _VMMLock		 (HGLOBAL hHandle);
LPVOID WINAPI		      VMMLock		 (HGLOBAL hHandle);
LPVOID WINAPI		      VMMLockLow	 (HGLOBAL hHandle);
BOOL WINAPI		      VMMReleasePtr 	 (HGLOBAL hHandle, LPVOID lpData);
BOOL WINAPI		      VMMRelease	 (HGLOBAL hHandle);
BOOL WINAPI                   VMMForceSwapOut    (HGLOBAL hHandle);
BOOL WINAPI		      VMMFree		 (HGLOBAL hHandle);
BOOL WINAPI		      VMMFreePtr	 (HGLOBAL *lphHandle);
HGLOBAL WINAPI		      VMMReAlloc	 (HGLOBAL hHandle,DWORD dwSize);
HGLOBAL WINAPI		      VMMHandleFromPtr 	 (void far *lpData);
DWORD WINAPI		      VMMSize 		 (HGLOBAL hHandle);
BYTE WINAPI		      VMMAvail 		 (DWORD *lpdwAvailWithPurge, DWORD *lpdwAvail, DWORD *lpdwTotalAvail);
DWORD WINAPI                  VMMAvailWithPurge  (void);
UINT WINAPI                   VMMCompact         (void);
UINT WINAPI		      VMOK		 (void);
UINT WINAPI		      CheckOutVMM	 (void);
LPVOID WINAPI		      VMMAllocFixed	 (DWORD size);
UINT WINAPI		      VMMFreeFixed	 (LPVOID buffer);
void WINAPI                   VMMSetFlags        (HGLOBAL hHandle, WORD wFlags);
WORD WINAPI                   VMMGetFlags        (HANDLE hHandle);

WORD WINAPI     VMMInit(WORD nHandles, DWORD *convVMM,
                                DWORD *emsVMM, DWORD *xmsVMM, DWORD *diskVMM);
WORD WINAPI     VMMTerminate(void);
WORD WINAPI     VMMAbort(void);
WORD WINAPI     VMMReleaseUpperMemory(void);
LPVOID WINAPI   VMSwapOut(WORD wAddr, WORD wSize);
WORD WINAPI     VMSwapIn(LPVOID lpClaimCheck, WORD wAddr, WORD wSize);
WORD WINAPI     VMInitEMS (DWORD dwSize);
WORD WINAPI     VMShutdownEMS(void);
WORD WINAPI     VMInitXMS (DWORD dwSize);
WORD WINAPI     VMShutdownXMS(void);
WORD WINAPI     VMInitDSK (DWORD dwSize);
WORD WINAPI     VMShutdownDSK(void);
WORD WINAPI 	CheckPool(BYTE *pbUseEMS,
			  BYTE *pbUseXMS,
			  DWORD *emsMem,
			  DWORD *xmsMem);

WORD WINAPI     _XMSInstalled (void);
WORD WINAPI     _XMSAlloc (WORD wBlockSize, WORD FAR *lpwHandle);
WORD WINAPI     _XMSFree (WORD wHandle);
UINT WINAPI 	_XMSMove(struct XMSMoveRegion FAR *lpInfo);
WORD WINAPI     _XMSGetFreeMemory (WORD FAR *lpwBytesFree, WORD FAR *lpwLargestBlock);

BOOL WINAPI     _EMSManagerInstalled (void);
WORD WINAPI     _EMSGetEMMVersion (void);
WORD WINAPI     _EMSAlloc (WORD wLogicalPages, WORD FAR * lpwHandle);
WORD WINAPI     _EMSFree (WORD wHandle, WORD wRetryCount);
WORD WINAPI     _EMSGetPageFrame (WORD FAR *lpwSegment);
WORD WINAPI     _EMSMapMemory (WORD wHandle, WORD wPhysicalPage, WORD wLogicalPage);
UINT WINAPI 	_EMSMoveMemoryRegion (struct EMSMoveRegion FAR *parameterBlock);
WORD WINAPI     _EMSGetPageCount (WORD FAR *lpwTotalPages, WORD FAR *wFreePages);

WORD WINAPI     _DOSAlloc (DWORD FAR *reqSize, LPVOID retPtr);
WORD WINAPI     _DOSFree (LPVOID addr);
LPVOID WINAPI   _UMBAlloc (DWORD byteSize);
DWORD WINAPI    _UMBLargestAvail (void);
WORD WINAPI     _UMBFree (LPVOID pointer);

#ifdef __cplusplus
}
#endif

#ifdef  MEMORY_TRACKING

#define	VMM_MACROS

#define MODULE_NAME static char near szMemoryTrackFileName[] = __FILE__

HGLOBAL WINAPI		      VMMAllocDBG	 (DWORD dwSize, const NPSTR szFileName, WORD wLine);
HGLOBAL WINAPI		      VMMReAllocDBG	 (HGLOBAL h, DWORD size, const NPSTR szFileName, WORD wLine);
BOOL WINAPI		      VMMFreeDBG	 (HGLOBAL *h, const NPSTR szFileName, WORD wLine);
BOOL WINAPI		      VMMReleaseDBG	 (HGLOBAL h, LPVOID lpVoid, const NPSTR szFileName, WORD wLine);
LPVOID WINAPI		      VMMLockDBG	 (HGLOBAL h, const NPSTR szFileName, WORD wLine);
LPVOID WINAPI		      VMMLockLowDBG	 (HGLOBAL h, const NPSTR szFileName, WORD wLine);
LPVOID WINAPI		      _VMMLockDBG	 (HGLOBAL h, const NPSTR szFileName, WORD wLine);
HGLOBAL WINAPI		      VMMHandleFromPtrDBG(LPVOID ptr, const NPSTR szFileName, WORD wLine);
UINT WINAPI		      VMMFreeFixedDBG	 (LPVOID *buffer);

#define VMMAlloc(h) VMMAllocDBG(h, szMemoryTrackFileName, __LINE__)
#define VMMReAlloc(h, s) VMMReAllocDBG(h, s, szMemoryTrackFileName, __LINE__)
#define VMMFree(h) VMMFreeDBG(&(h), szMemoryTrackFileName, __LINE__)
#define VMMUnlock(h, p) VMMReleaseDBG(h, &(p), szMemoryTrackFileName, __LINE__)
#define VMMLock(h)  VMMLockDBG(h, szMemoryTrackFileName, __LINE__)
#define VMMLockLow(h)  VMMLockLowDBG(h, szMemoryTrackFileName, __LINE__)
#define _VMMLock(h) _VMMLockDBG(h, szMemoryTrackFileName, __LINE__)
#define VMMHandleFromPtr(p) VMMHandleFromPtrDBG(p, szMemoryTrackFileName, __LINE__)
#define	VMMFreeFixed(p)		VMMFreeFixedDBG(&(p))

#else

#define VMMUnlock(h, p) VMMReleasePtr(h, &(p))
#define VMMFree(h) VMMFreePtr(&(h))
#define MODULE_NAME

#endif

#endif          // INCL_VMM
