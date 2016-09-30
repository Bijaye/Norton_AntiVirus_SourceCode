// TimeHelp.c

#include <windows.h>
#include <string.h>

// Real-mode call structure for making DPMI Simulate Real Mode
// Interrupt calls.
typedef struct tagRMCS { 
   DWORD edi, esi, ebp, RESERVED, ebx, edx, ecx, eax;
   WORD  wFlags, es, ds, fs, gs, ip, cs, sp, ss;
} RMCS, FAR* LPRMCS; 


BOOL FAR PASCAL SimulateRM_Int (BYTE bIntNum, LPRMCS lpCallStruct); 
void FAR PASCAL BuildRMCS (LPRMCS lpCallStruct);
BOOL FAR PASCAL __export thk_ThunkConnect16(LPSTR pszDll16, LPSTR pszDll32, 
	WORD hInst, DWORD dwReason);

int CALLBACK LibMain(HINSTANCE hinst, WORD wDataSeg, WORD cbHeap,
    LPSTR lpszCmdLine )
{
	return 1;
}

BOOL FAR PASCAL __export DLLEntryPoint(DWORD dwReason, WORD hInst, WORD wDS, 
	WORD wHeapSize, DWORD dwReserved1, WORD wReserved2) 
{

	if( !(thk_ThunkConnect16( 
		"TimeHelp.DLL",		// name of 16-bit DLL 
		"TmHelp32.DLL",		// name of 32-bit DLL 
		hInst, dwReason)) )     
	{
			return FALSE;
	}

	return TRUE; 
} 

DWORD FAR PASCAL __export GetTime()
{	RMCS rmcs;
	DWORD dwTime;

	BuildRMCS(&rmcs);

	rmcs.eax = 0x0200;

	if(!SimulateRM_Int ( (BYTE)0x1A,  &rmcs))
		return 0;
	dwTime = LOBYTE(LOWORD(rmcs.edx)) + 
		(0x100 * HIBYTE(LOWORD(rmcs.edx))) +
		(0x10000 * LOBYTE(LOWORD(rmcs.ecx))) +
		(0x1000000 * HIBYTE(LOWORD(rmcs.ecx)));


	return dwTime;
}

/*-------------------------------------------------------------------
  SimulateRM_Int()

  Allows protected-mode software to execute real-mode interrupts
  such as calls to MS-DOS, MS-DOS TSRs, MS-DOS device drivers.

  This function implements the "Simulate Real Mode Interrupt"
  function of the DPMI specification v0.9 and later.

  Parameters:

     bIntNum
        Number of the interrupt to simulate.

     lpCallStruct
        Call structure that contains params (register values)
        for bIntNum.

  Return Value
     SimulateRM_Int returns TRUE if it succeeded or FALSE if
     it failed.

  Comments
     lpCallStruct is a protected-mode selector:offset address, not
     a real-mode segment:offset address.
-------------------------------------------------------------------*/ 
BOOL FAR PASCAL SimulateRM_Int (BYTE bIntNum, LPRMCS lpCallStruct) { 

   BOOL fRetVal = FALSE;        // Assume failure

   _asm {
         push di
         mov  ax, 0300h         ; DPMI Simulate Real Mode Interrupt
         mov  bl, bIntNum       ; Number of the interrupt to simulate
         mov  bh, 01h           ; Bit 0 = 1; all other bits must be 0
         xor  cx, cx            ; No words to copy from PM to RM stack
         les  di, lpCallStruct  ; Real mode call structure
         int  31h               ; Call DPMI
         jc   END1              ; CF set if error occurred

         mov  fRetVal, TRUE
     END1:
         pop di
        }
   return (fRetVal);
} 

/*-------------------------------------------------------------------
   BuildRMCS()

   Initializes a real-mode call structure by zeroing all its members.

   Parameters:

      lpCallStruct
         Points to a real-mode call structure

   Return Value
         None.

   Comments
         lpCallStruct is a protected-mode selector:offset address,
         not a real-mode segment:offset address.
-------------------------------------------------------------------*/ 

void FAR PASCAL BuildRMCS (LPRMCS lpCallStruct)
{
   _fmemset (lpCallStruct, 0, sizeof(RMCS));
}
