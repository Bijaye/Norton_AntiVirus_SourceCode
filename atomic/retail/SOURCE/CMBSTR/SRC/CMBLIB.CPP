#include "platform.h"
#include "xapi.h"
#include "file.h"

#ifdef SYM_WIN32
    #include "ctsn.h"
#else
    #include "cts4.h"
#endif

#include "cmbstr.h"
#include "navutil.h"

M_STRING MStr;
static UINT gNumUsers = 0;

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure: String_Init()                                      **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:  This function specifies how strings will be   **** //
// ****     obtained from within Windows. If the bCertus flag is TRUE  **** //
// ****     then .CMB files will be used for strings, otherwise        **** //
// ****     Windows string tables will be used.                        **** //
// ****                                                                **** //
// ****  Input: HINSTANCE hIns, Module Instance Handler                **** //
// ****         BOOL bCertus, TRUE to use .CMB file                    **** //
// ****                       FALSE to use Windows String Table        **** //
// ****  Output:                                                       **** //
// ****        If bCertus is FALSE then this value is irrelevant,      **** //
// ****        otherwise TRUE implies that the CMB file was opened.    **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL WINAPI String_Init(HINSTANCE hIns, LPSTR lpCmbName)
{
    BOOL bStatus = TRUE;

    if ( gNumUsers == 0 )
        {
        MStr.hInstance = hIns;
        MStr.hCmb = 0;

        MStr.lpString = (LPSTR)MemAllocPtr (GHND, SIZE_STRING);

        if ( MStr.lpString )
            {
            if (hIns && lpCmbName)
                {
                GetStartDir(MStr.hInstance, MStr.lpString, SIZE_WORK);
                NameAppendFile(MStr.lpString, lpCmbName);

                if ((MStr.hCmb = FileOpen(MStr.lpString, OF_READ)) == HFILE_ERROR)
                    bStatus = FALSE;
                else
                    gNumUsers ++;
                }
            }
        else
            bStatus = FALSE;
        }
    else
        gNumUsers ++;

    return (bStatus);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:  String_End()                                      **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality: Closes the default CMB file if there was       **** //
// ****                 one opened.                                    **** //
// ****                                                                **** //
// ****  Input:  none                                                  **** //
// ****                                                                **** //
// ****  Output: none                                                  **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID WINAPI String_End(VOID)
{
    if ( gNumUsers )
        gNumUsers --;

    if ( gNumUsers == 0 )
        {
        if (MStr.hInstance && MStr.hCmb)
            FileClose(MStr.hCmb);

        if (MStr.lpString)
            {
            MemFreePtr (MStr.lpString);
            }
        }
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:  String_Get()                                      **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality: Obtains a string from the specified source     **** //
// ****                 in String_Init();                              **** //
// ****                                                                **** //
// ****  Input:  UINT uMsgNum; Message Number to obtain                **** //
// ****          LPSTR lpBuff; Buffer where to store the strin         **** //
// ****          UINT  uSize;  Size of the buffer                      **** //
// ****                                                                **** //
// ****  Output: Number of bytes read into the buffer. 0 == Error      **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

UINT WINAPI String_Get(UINT uMsgNum, LPSTR lpBuffer, UINT uSize)
{
    auto    UINT    uNumBytes = 0;
    auto    HPSTR   hpStr;
    auto    char    szString [ SYM_MAX_PATH+1 ];

    if (MStr.hCmb)
        ;
// MARTIN        uNumBytes = CMB4_READ(uMsgNum, MStr.hCmb, lpBuffer, uSize);
    else if (MStr.hInstance)
        {
        uNumBytes = LoadString (MStr.hInstance, uMsgNum, szString,
                                sizeof(szString));

        if (uNumBytes <= uSize)
            {
            hpStr = (HPSTR)lpBuffer;
            STRCPY (hpStr, szString);
            }
        else
            uNumBytes = 0;
        }

    return (uNumBytes);
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:  String_Ptr()                                      **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality: Obtains a string from the specified source     **** //
// ****                 in String_Init();                              **** //
// ****                                                                **** //
// ****  Input:  UINT uMsgNum; Message Number to obtain                **** //
// ****                                                                **** //
// ****  Output: Far Pointer to where to ASCIIZ text is.               **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

LPSTR WINAPI String_Ptr(UINT uMsgNum)
{
   UINT uNumBytes = 0;

   if (MStr.hCmb)
        ;
// MARTIN      uNumBytes = CMB4_READ(uMsgNum, MStr.hCmb, MStr.lpString, SIZE_STRING);
   else if (MStr.hInstance)
      uNumBytes = LoadString (MStr.hInstance, uMsgNum, MStr.lpString, SIZE_STRING);

   return ((uNumBytes) ? MStr.lpString : NULL);
}
