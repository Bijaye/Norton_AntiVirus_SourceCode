#include "platform.h"
#include "options.h"
#include "ctl3d.h"                  // Microsoft 3D library
#include "cmbstr.h"
#include "xapi.h"
#include "inoc.h"

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI InocClose (VOID)
{
#ifdef _ALPHA_
	return FALSE;
#else
#ifdef SYM_WIN32
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return FALSE;
#endif

   LPINOCULATE  Inoc;
   UINT uStatus = 0;


   if ((Inoc = GetInocStruct(0, FALSE)) == NULL)
        return(TRUE);

   // MFallen: Moved cleanup code into ReleaseInocStruct
   // &? BarryG: but lost detection of DBC4_CLOSE() errors
   ReleaseInocStruct(Inoc);

   return ((uStatus) ? FALSE : TRUE);
#endif
}

