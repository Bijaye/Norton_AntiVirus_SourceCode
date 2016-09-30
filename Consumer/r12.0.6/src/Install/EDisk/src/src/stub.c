// Copyright 1992 Symantec Corporation                                    
// **********************************************************************
//                                                                       
//  $Header:   S:/SRC/NORTON/VCS/STUB.C_V   1.1   17 Mar 1993 12:03:22   JOEP  $ *
//                                                                       
//  Description:                                                         
//                                                                       
//  Contains:                                                            
//                                                                       
//  See Also:                                                            
//                                                                       
// **********************************************************************
/*  $Log:   S:/SRC/NORTON/VCS/STUB.C_V  $ *
 * 
 *    Rev 1.1   17 Mar 1993 12:03:22   JOEP
 * quick fix
 * 
 *    Rev 1.0   17 Mar 1993 11:03:28   JOEP
 * Initial revision.
 * 
 *    Rev 1.0   27 Oct 1992 18:20:54   SCOTTP
 * Initial revision.
 * 
 *    Rev 1.0   18 Oct 1992 18:59:52   JOEP
 * Initial revision.
   **********************************************************************/

#include    <winmgr.h>
#include    <types.h>
#include    <stddlg.h>
#include	<file.h>
#include    <exeview.h>
#include    <lint.h>

MODULE_NAME;

static void MyAboutHook0(void);
static void MyAboutHook(void);


extern Byte *aszAboutLines[];
extern Word gwNAboutLines;
static Word wCount;
CRASHPROC lpAboutHackNU = NULL;


LPVOID PASCAL GetRSBuf(void);
// Function definitions in rs386.obj
unsigned int FAR PASCAL PlaySound(WORD SoundOption, BYTE FAR *DataAddr,
                                  BYTE playback, BYTE far *CallbackAddr,
                                  WORD callbackfreq);

Boolean PASCAL NDAboutHack(void)
{
    extern	Word far	wKeyboardFlags;
    extern	BYTE		szProductVar[];

    auto	Word		wRow;
    auto	Word		wCol;
    auto	MWindowRecPtr	lpWnd = ActiveWindow();
    auto    BOOL        bPlaySound = FALSE;
    auto    BYTE        far *SaveArea;
    
    if ((SaveArea = GetRSBuf()) != NULL)
        {
            bPlaySound = TRUE;
        }

    wCount = 0;
    IdleHookRegister(MyAboutHook0, 2);
    
    while (wCount < 10)
	IdleHookCall();
    
    IdleHookUnregister(MyAboutHook0);
    wCount = 0;
    IdleHookRegister(MyAboutHook, 9);
    
    while (KBHit() != -1)
	{
	IdleHookCall();
	KBRead();
	}

 LOOP:
    
    while (KBHit() == -1 && ! MouseButtons(&wRow, &wCol))
	{
	IdleHookCall();
	
	if (wCount >= gwNAboutLines)
        {
        if (bPlaySound)
            {
            PlaySound(0,SaveArea+32,2,NULL,NULL);
            PlaySound(0,SaveArea+32,2,NULL,NULL);
            }
        wCount = 0;
        }
	}
    
    switch (KBHit())
	{
	case '-':
	    wCount -= 10;
	    KBRead();
	    goto LOOP;
	case '+':
	    wCount += 10;
	case 'n':
	case 'u':
	case 'N':
	case 'U':
	    KBRead();
	    goto LOOP;
	}
    
    IdleHookUnregister(MyAboutHook);
    
    wKeyboardFlags = 0;
    
    if (SaveArea != NULL)
        VMMFreeFixed(SaveArea);

    return (FALSE);
}

static void MyAboutHook0(void)
{
    auto	MWindowRecPtr	lpWnd = ActiveWindow();
    
    WinScrollUp(6, 1, lpWnd->height-2, lpWnd->width-2, 1);
    wCount++;
}

static void MyAboutHook(void)
{
    auto	MWindowRecPtr	lpWnd = ActiveWindow();
    
    WinScrollUp(6, 1, lpWnd->height-2, lpWnd->width-2, 1);
    
    WinSetRowCol((Byte) (lpWnd->height-2), 3);
    
    FastPrint(aszAboutLines[wCount++]);
}

/************************************************************************
 *                                                                      *
 * Description:  Check for the /$_CREDITS_ command line switch, and     *
 *               view the credits bitmap if necessary                   *
 *                                                                      *
 * Parameters:   szFileName - The buffer of the file to be processed by *
 *                            the viewer                                *
 *                                                                      *
 * Return Value: None                                                   *
 *                                                                      *
 * NOTE:   This code will not work if the viewer was linked with debug  *
 *         information, because it is tacked on at the end of the file  *
 *         where we expect to find the picture.                         *
 *                                                                      *
 *         Also, the file extension in the szCreditTempFile string has  *
 *         to match the actual file type of the picture, or life will   *
 *         not be good...                                               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 04/03/1992 TORSTEN Function Created.                                 *
 * 03/16/1993 BASIL Modified to return pointer to real sound buffer     *
 ************************************************************************/

LPVOID PASCAL GetRSBuf(void)
{

#define COPY_BUFFER_SIZE     (36944)

    extern	Word		trapping_enabled;

    auto        int             nEXEHandle;
    auto        Byte            szProgramName[MAX_PATH_NAME + 1];
    auto        Ulong           ulPictureOffset;
    auto        Byte far *      lpbyCopyBuffer;
    auto        WORD            wActualRead;

    trapping_enabled = FALSE;

    lpbyCopyBuffer = (Byte far *) VMMAllocFixed(COPY_BUFFER_SIZE);

    if (lpbyCopyBuffer==NULL)
        return(NULL);

    DOSGetProgName(szProgramName);

    nEXEHandle = FileOpen(szProgramName, READ_ONLY_FILE);

    if (nEXEHandle != ERROR)
        {
        ulPictureOffset = FileSize(nEXEHandle) - COPY_BUFFER_SIZE;
        
        FileSeek(nEXEHandle, ulPictureOffset, SEEK_FROM_BEGINNING);

        wActualRead=FileRead(nEXEHandle, lpbyCopyBuffer, (WORD)COPY_BUFFER_SIZE);
                            // suck the keyboard buffer empty...
        KBFlush();

        FileClose(nEXEHandle);
        }
    else
        {
        VMMFreeFixed(lpbyCopyBuffer);
        return(NULL);
        }

    return(lpbyCopyBuffer);

}

