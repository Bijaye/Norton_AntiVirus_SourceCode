// Copyright 1992-1996 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/NAVC/VCS/navstub.c_v   1.1   22 Aug 1997 15:28:22   TCASHIN  $
//
// Description:
//  This module launches the NAVDX.OVL file.
//
// See Also:
//***********************************************************************
// $Log:   S:/NAVC/VCS/navstub.c_v  $
// 
//    Rev 1.1   22 Aug 1997 15:28:22   TCASHIN
// Set DOS16M=2 only if an XMS driver is loaded. #92374.
// 
//    Rev 1.0   13 Aug 1997 21:38:20   TCASHIN
// Initial revision.
// 
//    Rev 1.11   21 May 1997 12:14:14   MZAREMB
// Added DOS16M=2 if no DOS16M environment variable is set.
// 
//    Rev 1.10   12 Nov 1996 17:22:36   MZAREMB
// Removed a STRNICMP quake call with the run-time strnicmp() from MS.
// 
//    Rev 1.9   06 Aug 1996 16:58:06   MZAREMB
// Put in code to check executable file name and pass a new argument to the OVL.
// 
//    Rev 1.8   19 Jul 1996 10:31:02   JALLEE
// Added NavClearMem to clear unallocated memory.  Stop memory false positives 
// 
//    Rev 1.7   15 Jul 1996 10:14:10   MZAREMB
// The name of the OVL for Nirvana Plus is NAV.OVL
// 
//    Rev 1.6   09 Apr 1996 11:44:08   MZAREMB
// Updated minimum memory requirements to take into consideration the size
// of the stub program.
// 
//    Rev 1.5   08 Apr 1996 10:34:18   MZAREMB
// Added a check for (minimum) memory free and supporting message.
// 
//    Rev 1.4   04 Apr 1996 16:30:52   MZAREMB
// Added exit() at the end of main() to return the exit code from navboot.ovl.
// 
//    Rev 1.3   02 Apr 1996 15:54:14   MZAREMB
// Activated self-checking CRC code.
//***********************************************************************

#include <platform.h>
#include <xapi.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <fcntl.h>
#include <share.h>
#include <sys\locking.h>
#include <sys\stat.h>
#include "navboot.str"

#define ERRORLEVEL_EXECUTETSR   100
#define APPROX_STUB_SIZE_PARA   75
#define FREE_MIN_PARA           ((200 - APPROX_STUB_SIZE_PARA) * (1024/16))

// Local prototypes
#ifdef VERIFY_CHECKSUM

#define SELF_TEST_BUFFER_SIZE   512
unsigned int VerifyChecksum( int, char *[]);                    
unsigned int CalculateCRC(int seed, char *buffer, unsigned int count);

#endif

unsigned int DOSGetFreeMem( void );
void NavClearMem( void );

// magic command-line arguments needed for loading TSR.
char  *lpArgs[] = { " ", "/%NAVBOOT", "/ERRLVL ***", NULL };

//************************************************************************
// Main()
//
// This program is the "stub" used to launch NAV.OVL. If the name of this
//  executable is NAVBOOT, then we set a new argument for the overlay to
//  take on another personality.
//
//************************************************************************
// 3/26/96 MZAREMB   Function created.
//************************************************************************

void main( int argc, char *argv[])
{
    int returnCode, nCount;                 // Return code from NAVBOOT
    unsigned int iDOSAvailPara;             // Paragraphs of memory free
    unsigned long dwMore;                   // Additional memory required
    char    szFullPath[260];                // This will hold the full path
    char    szFile[260];                    // This will hold the path & file
    unsigned int bNavboot = FALSE;
    WORD    wQueryXMS;

    strcpy(szFullPath, argv[0]);            // get the full program name/path

#ifdef VERIFY_CHECKSUM
    if (FALSE == VerifyChecksum( argc, argv )) // Check ourselves
    {
        exit(0);                            // exit if checksum fails.
    }     
#endif

    iDOSAvailPara   = DOSGetFreeMem( );     // How much memory is available?

    if ( iDOSAvailPara < FREE_MIN_PARA )    // Is it enough?
        {
                                            // how much more is needed?
        dwMore  = (unsigned long) (FREE_MIN_PARA-iDOSAvailPara) * 16 ;
                                            // let the user know...
        printf( INSUFFICIENT_MEM_CONSTANT, dwMore ) ;
                                            // ...and then leave.
        exit(0);
        }

    NavClearMem();

    nCount = strlen(szFullPath);            // get the full size of the path
                                            // look for slashes and stop
    while (( szFullPath[nCount] != 92 ) && (szFullPath[nCount] != 47))
    {
        nCount--;                           // check next character back
        if ( !nCount )                      // start of string?
            break;
    }
    
    szFullPath[nCount] = '\0';              // Terminate the original path

    strcpy(szFile,szFullPath);              // load in the path name
    strcat(szFile,"\\NAVDX.OVL");           //  and then load the filename


    printf(SZ_PLEASEWAIT_CONSTANT);         // Inform the user that this will
                                            //  take some time to load.

#ifndef NCD
                                            //  Always set DOS16M=2 as a
                                            //  workaround to avoid HIMEM.SYS
                                            //  errors on warm boots.
                                            //  We only do this on non-NEC; on NEC, this
                                            //  will cause corrupted characters on NAVDX
                                            //  startup.

    if ( getenv("DOS16M") == NULL )         //  Force DOS16M=2 unless another
        putenv("DOS16M=2");                 //  setting is specified.
#endif


                                            // Spawn the DX program and return
    returnCode = spawnvp(P_WAIT, szFile, argv);

    if ( returnCode == -1 ) 
    {
   
        switch (errno)                          // do the appropriate thing
        {
            case ENOENT:                        // the file is not there
                printf(SZ_CANNOTEXEC_CONSTANT);
                printf(SZ_NOENT_CONSTANT);
                break;
            case ENOEXEC:                       // the file is not a valid
                printf(SZ_CANNOTEXEC_CONSTANT); //   executable
                printf(SZ_NOEXEC_CONSTANT);
                break;
            case ENOMEM:                        // Not enough mem to load
                printf(SZ_CANNOTEXEC_CONSTANT);
                printf(SZ_NOMEM_CONSTANT);
                break;
            default:
                printf(SZ_CANNOTEXEC_CONSTANT); // Let the user know that 
                printf("unknown problem.");     //  all was not well.
        }        
    }
    
    exit (returnCode);
}

//************************************************************************
// VerifyChecksum()
//
// This routine was more or less copied from the certlib FileSelfTest
//   routine. It was included in this manner, as was the CRC calculations,
//   to avoid all the overhead of including the certlib routines just to 
//   do a checksum.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 4/1/96 MZAREMB   Function created.
//************************************************************************
#ifdef VERIFY_CHECKSUM
unsigned int VerifyChecksum(int nArgs, char *pArgs[])
{
    unsigned int uCount, uBytesRead, wPrevCRC, wNewCRC;
    int nFileHandle;
    char szFile[128]; 
    char *lpBuffer;
    
    uCount = 1;                         // Skip over the program name
    while (uCount < nArgs)              // look at all arguments
    {                                   // and find "/NOCRC"
        if ( strnicmp(pArgs[uCount],"/NOCRC",6) == 0 ) 
        {
            return (TRUE);              // Send "OK" from VerifyChecksum()
        }
        uCount++;                       // next program argument
    }

    nFileHandle = _sopen(pArgs[0], (_O_BINARY|_O_RDONLY), SH_DENYNO);

    if ( nFileHandle == -1 )            // Problem opening self
    {
        printf(SZ_CANNOTOPEN_CONSTANT);
        return (FALSE);
    }
    
                                        // Allocate some memory
    lpBuffer = (char *)calloc(1, SELF_TEST_BUFFER_SIZE);
    if ( lpBuffer == (char *)0 )        // Problem allocating
    {
        printf(SZ_NOCRCMEM_CONSTANT);
        return (FALSE);
    }
                                        // Read in the file header
    uBytesRead = read ( nFileHandle, (char *)lpBuffer, SELF_TEST_BUFFER_SIZE );

                                        // Close file
    close(nFileHandle);

                                        // Make sure that the file was read
                                        // correctly

    if ( uBytesRead != SELF_TEST_BUFFER_SIZE )
        {
        printf(SZ_READERROR_CONSTANT);
        free(lpBuffer);
        return ( FALSE );
        }

                                        // Save previously calculated CRC

    wPrevCRC = *(unsigned int *) &((char *)lpBuffer)[0x12];

                                        // Check if there was a CRC

    if ( wPrevCRC == 0 )
        {
        printf(SZ_NOCHECKSUM_CONSTANT);
        free(lpBuffer);
        return (FALSE);
        }

                                        // Calculate current CRC

    *(unsigned int *) &((char *)lpBuffer)[0x12] = 0;

    wNewCRC = CalculateCRC ( 0, (char *)lpBuffer, SELF_TEST_BUFFER_SIZE );

                                        // Compare the CRCs
    free(lpBuffer);

    if ( wNewCRC != wPrevCRC )
        {
        printf(SZ_BADCHECKSUM_CONSTANT);
        return ( FALSE );
        }

                                        // All OK.

    return ( TRUE );
}    

//************************************************************************
// CalculateCRC()
//
// This routine was copied from the certlib MEM4_CRC routine. It was 
//  included in this manner to avoid all the overhead of including the 
//  certlib routines just to do a checksum. This routine was directly
//  "cut and paste" so as to minimize possible contamination of the
//  routine. 
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 4/1/96 MZAREMB   Function created.
//************************************************************************

unsigned int CalculateCRC(int Seed, char *Buffer, unsigned int Count)
{
    _asm
    {
        push    ds
        push    es
        
        mov     si, Seed
        cmp     Count, 0
        je      $EXIT_ONE522

        mov     bx, Buffer
        xor     ax, ax
$I518:
        mov     al,byte ptr ds:[bx]
        cbw
        mov     di,ax
        mov     ax,si
        mov     cl,8
        shl     ax,cl
        mov     dx,si
        shr     dx,cl
        add     ax,dx
        xor     ax,di
        mov     si,ax
        sub     ah,ah
        mov     cl,4
        shr     ax,cl
        xor     si,ax
        mov     ax,si
        mov     dh,al
        sub     dl,dl
        sub     ah,ah
        mov     cl,8
        shr     ax,cl
        add     dx,ax
        mov     cl,4
        shl     dx,cl
        mov     ax,si
        sub     ah,ah
        mov     cl,5
        shl     ax,cl
        xor     dx,ax
        xor     si,dx
        dec     Count
        jz      $EXIT_ONE522
        inc     bx
        jmp     $I518

$EXIT_ONE522:
        mov     ax,si

        pop     es
        pop     ds
    }
}
#endif

/////////////////////////////////////////////////////////////////////////////
// DOSGetFreeMem
//
// Returns the amount of DOS memory available in paragraphs.
//
// This routine was copied from the dosvmm DOSMEM routine. It was 
//  included in this manner to avoid all the overhead of including the 
//  entire Norton libraries. 
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 4/7/96 MZAREMB   Function created.
//************************************************************************

unsigned int DOSGetFreeMem( void )
{
    unsigned int       segAddr;

     _asm
	 {
	 mov	ah,48H
     mov    bx,-1
	 int	21H
     mov    segAddr,bx
     jc     done
	 mov	es, ax
	 mov	ah, 49H
	 int	21H
	 }

done:
    return (segAddr);
}
//************************************************************************
// NavClearMem()
//
// Clear unallocated memory to minimize liklihood of memory false positive.
//
//
// Parameters:
//      None
//
// Returns:
//      Nothing
//************************************************************************
// 7/16/96 JALLEE created.
//************************************************************************
#pragma warning(disable:4704)           // In-line ASM
void NavClearMem( void )
{
    MemoryBlockRecord far   *lpMCB = NULL;

                                        // Get First MCB from DOS list of lists
    _asm
    {
        mov ax, 5200h
        int 21h
        mov ax, es:[bx-2]
        mov [word ptr lpMCB + 2], ax
        mov [word ptr lpMCB], 0
    }
                                        // Travers MCB chain.
    while ('M' == lpMCB->id)
	{
                                        // Clear memory free memory.
                                        // There is probably no free memory
                                        // here, but check anyway.
        if (lpMCB->pspSeg == 0)
        {
                                        // Set memory to N for NAV.
            _fmemset( ((char far *)lpMCB)+0x10, 'N', lpMCB->size * 0x10);
        }
                                        // Go to next mcb.
        lpMCB = MAKELP(FP_SEG(lpMCB) + lpMCB->size + 1, 0);
	}

    if ('Z' == lpMCB->id)
    {
                                        // Clear last block.
        if (lpMCB->pspSeg == 0)
        {
            _fmemset( ((char far *)lpMCB)+0x10, 'N', lpMCB->size * 0x10);
        }
    }
    return;
}
