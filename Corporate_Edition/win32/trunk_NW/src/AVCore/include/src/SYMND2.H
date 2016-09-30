// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1994 - 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/symnd2.h_v   1.3   20 Sep 1996 17:12:52   PMARTIN  $
//
// Description:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/symnd2.h_v  $
// 
//    Rev 1.3   20 Sep 1996 17:12:52   PMARTIN
// Added length parm to several NetDirectory...() functions.
//
//    Rev 1.2   20 Aug 1996 11:25:14   PMARTIN
// Changed NDSGetDefNameContext() parms.
//
//    Rev 1.1   02 Aug 1996 15:21:52   PMARTIN
// Changed for NDS implementation
//
//    Rev 1.0   22 Jul 1996 14:32:32   PMARTIN
// Initial revision.
//
//***************************************************************************
#ifndef __SYMND2_H
#define __SYMND2_H




typedef int (CALLBACK* NDIRINIT_FCN )( LPNETDIRDATA );

typedef int (CALLBACK* NDIRGETCONTEXT_FCN )(
    LPNETDIRDATA,           // [in]  network directory data
    LPSTR,                  // [out] current name context
    WORD
);

typedef BOOL (CALLBACK* NDIRISROOT_FCN )( LPNETDIRDATA );

typedef int (CALLBACK* NDIRTREEUP_FCN )( LPNETDIRDATA );

typedef int (CALLBACK* NDIRTREEDOWN_FCN )(
    LPNETDIRDATA,           // [in]  network directory data
    LPSTR                   // [in] container to enter
);

typedef int ( CALLBACK * NDIRCONTAINERSENUM_FCN )(
    LPNETDIRDATA,           // [in] network directory data
    DWORD,                  // [in] Userdata to return in callback
    NETDIRECTORYCONTAINERSENUMPROC// [in] Enum callback fcn ptr
);

typedef int (CALLBACK * NDIRSERVERSENUM_FCN) (
    LPNETDIRDATA,           // [in] network directory data
    DWORD,                  // [in] Userdata to return in callback
    NETDIRECTORYSERVERSENUMPROC// [in] Callback
);

typedef int (CALLBACK * NDIRUSERSENUM_FCN) (
    LPNETDIRDATA,           // [in] network directory data
    DWORD,                  // [in] Userdata to return in callback
    NETDIRECTORYUSERSENUMPROC// [in] Callback
);

typedef int (CALLBACK * NDIRGROUPSENUM_FCN) (
    LPNETDIRDATA,           // [in] network directory data
    DWORD,                  // [in] Userdata to return in callback
    NETDIRECTORYGROUPSENUMPROC// [in] Callback
);

typedef int (CALLBACK * NDIRGETLOGGEDINOBJECT_FCN) (
    LPNETDIRDATA    lpNetDir,           // [in] network directory data
    LPSTR           lpszName,           // [out] object name
    int             nNameSize           // [in] max name size
);

typedef BOOL (CALLBACK* NDIRISSUPPORTED_FCN )( LPSTR, WORD );

typedef BOOL (CALLBACK* NDIRGETTREENAME_FCN )( LPSTR lpszTreeName, WORD );

typedef BOOL (CALLBACK* NDIRSETSERVERCONTEXT_FCN) (
    LPNETDIRDATA    lpNetDir,           // [in] network directory data
    LPSTR           lpszServer          // [in] server name
);

typedef VOID (CALLBACK* NDIRUNINIT_FCN )( LPNETDIRDATA );

typedef VOID (CALLBACK* NDIRGETDEFNAMECONTEXT_FCN) (
    LPNETDIRDATA    lpNetDir,           // [in] network directory data
    LPSTR           lpszContext,        // [in] server name
    WORD            wLen
);


typedef struct tagNETDIRDATAFP {                // network directory structure
    NDIRINIT_FCN            Init;
    NDIRTREEUP_FCN          TreeUp;
    NDIRTREEDOWN_FCN        TreeDown;
    NDIRISROOT_FCN          IsRoot;
    NDIRGETCONTEXT_FCN      GetContext;
    NDIRCONTAINERSENUM_FCN  EnumContainers;
    NDIRSERVERSENUM_FCN     EnumServers;
    NDIRUSERSENUM_FCN       EnumUsers;
    NDIRGROUPSENUM_FCN      EnumGroups;
    NDIRUNINIT_FCN          Uninit;
    NDIRISSUPPORTED_FCN     IsSupported;
    NDIRGETLOGGEDINOBJECT_FCN GetLoggedInObject;
    NDIRSETSERVERCONTEXT_FCN  SetServerContext;
    NDIRISSUPPORTED_FCN     GetTreeName;
    NDIRGETDEFNAMECONTEXT_FCN GetDefNameContext;
} NETDIRDATAFP;




#endif

