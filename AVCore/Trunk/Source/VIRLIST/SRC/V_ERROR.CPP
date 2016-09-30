#include "platform.h"
#include "ngui.h"
#include "virusdbf.h"

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

VOID WINAPI ErrorHandler (
    HWND        hDlg,
    VISTATUS    Status )
{
    extern   HINSTANCE   hInstance;
    auto     TCHAR       szTitle [ SYM_MAX_PATH ];
    auto     TCHAR       szWork  [ SYM_MAX_PATH ];

    struct
        {
        VISTATUS    ErrorID;
        UINT        uError;
        } Error[] =
            {
                {VERR_DATAFILES,        IDS_N30},
                {VERR_MEMALLOC,         IDS_MEMALLOC},
                {VERR_FILEREAD,         IDS_FILEREAD},
                {VERR_FILEWRITE,        IDS_FILEWRITE},
                {VERR_LISTBOXCREATE,    IDS_LISTBOXCREATE},
                {VERR_WRONGPARENT,      IDS_WRONGPARENT},
                {VERR_INTERNAL,         IDS_INTERNAL},
                {0,                     0}
            };

    for ( UINT i=0; Error[i].ErrorID != Status && Error[i].uError; i++)
        ;

    if ( Error[i].ErrorID )
        {
        LoadString (hInstance, IDS_NAVDBF, szTitle, sizeof(szTitle) );
        LoadString (hInstance, Error[i].uError, szWork, sizeof(szWork) );

        MessageBox (hDlg, szWork, szTitle, MB_OK | MB_ICONINFORMATION );
        }
}
