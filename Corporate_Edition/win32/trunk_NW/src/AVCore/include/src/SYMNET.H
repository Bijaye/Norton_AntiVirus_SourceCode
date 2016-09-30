//------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.
//------------------------------------------------------------------------
// SYMNET.H
//
// $Header:   S:/INCLUDE/VCS/symnet.h_v   1.47   22 Aug 1997 11:40:42   Ddrew.Admin1.Network.Santa Monica.California.No  $
//
// Description:
//    Symantec Network Independent API Include File
//
//------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/symnet.h_v  $
// 
//    Rev 1.47   22 Aug 1997 11:40:42   Ddrew.Admin1.Network.Santa Monica.California.No
// Moved a bunch more things out from under the #ifndef SYM_NLM cover
// 
//    Rev 1.46   28 Jan 1997 12:57:02   PMARTIN
// Added NetGetProcessCount().
//
//    Rev 1.45   10 Dec 1996 16:02:50   PMARTIN
// Added functions to handle connected process list.
//
//    Rev 1.44   21 Nov 1996 16:30:46   PMARTIN
//
//    Rev 1.43   12 Nov 1996 12:32:56   PMARTIN
// Changed name of passed-in parm of NEtUnRegister() to hSymnet from lphSymnet.
//
//    Rev 1.42   05 Nov 1996 14:29:32   PMARTIN
// Added NETERR_XPC_LOCK_FAILED
//
//    Rev 1.41   31 Oct 1996 10:58:08   PMARTIN
// Changed return values of NetGetCommonDir and NetSetCommonDir to WORD.
//
//    Rev 1.40   21 Oct 1996 10:02:04   GWESTER
// Removed vcs comments.
//
//    Rev 1.39   23 Sep 1996 18:38:10   GWESTER
// Fixed build problem.
//
//    Rev 1.37   09 Sep 1996 18:30:52   PMARTIN
// Updated enum field sizes.
//
//    Rev 1.36   03 Aug 1996 12:29:44   MBARNES
// Disabled optimizations for certain function prototypes
//
//    Rev 1.35   02 Aug 1996 15:22:06   PMARTIN
// Changed for NDS implementation
//
//------------------------------------------------------------------------

#ifndef _SYMNET_H
#define _SYMNET_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONN_INFO_DEFD
#   define  CONN_INFO_DEFD      1
#   define  OBJECT_NAME_LEN    47
typedef char    SERVERTABLE_REC[OBJECT_NAME_LEN + 1];
typedef struct
    {
    BYTE            Year;               // 80 = 1980, etc.
    BYTE            Month;              // 1..12
    BYTE            Fay;                // 1..31
    BYTE            Hour;               // 0..23
    BYTE            Minute;             // 0..59
    BYTE            Second;             // 0..59
    BYTE            DayOfWeek;          // 0..6, 0 = Sunday
    }               BINDERY_OBJECT_TIME;

typedef struct
    {
    WORD            wConnectionNumber;
    BYTE            szObjectName[OBJECT_NAME_LEN];
    WORD            wObjectType;
    DWORD           ulObjectID;
    BINDERY_OBJECT_TIME rLoginTime;
    }               CONNECTION_INFO2;

#endif  //  #ifndef CONN_INFO_DEFD

//------------------------------------------------------------------------
// Define the function i.d.s (the first few of which are actually
// ordinals.
//------------------------------------------------------------------------
                                        // Define the function  i.d.s for the
                                        // network specific libraries/dlls

#define FCN_NETREGISTER                 2   // - MUST BE SUPPORTED AND USE THIS ORDINAL!!!
#define FCN_NETUNREGISTER               3   // - MUST BE SUPPORTED AND USE THIS ORDINAL!!!
#define FCN_NETFUNCTIONSUPPORTED        4   // - MUST BE SUPPORTED AND USE THIS ORDINAL!!!
#define FCN_NETGETFUNCTION              5   // - MUST BE SUPPORTED AND USE THIS ORDINAL!!!

#define FCN_NETGETCURRENTUSERNAME       6
#define FCN_NETGETSERVERDATEANDTIME     7
#define FCN_NETISMEMBEROFGROUP          8
#define FCN_NETGROUPSIMINENUM           9
#define FCN_NETVOLENUM                  10
#define FCN_NETGETWORKSTATIONNAME       11
#define FCN_NETGETNICADDRESS            12
#define FCN_NETGETCONNECTIONINFO        13
#define FCN_NETVERIFYCONNECTION         14
#define FCN_NETMESSAGESEND              15
#define FCN_NETGROUPSENUM               16
#define FCN_NETUSERSENUM                17
#define FCN_NETSERVERSENUM              18
#define FCN_NETGETPRIMARYSERVER         19
#define FCN_NETMESSAGESENDTOCONSOLE     20
#define FCN_NETVOLGETINFO               21
#define FCN_NETGETEVERYONEGROUP         22

//*** Functions added for OS/2 and Anvil - should work in       ***
//*** DOS and Windows when they move over to the new NW_API SDK.***
#ifdef SYM_OS2
#   define FCN_NETGETPRIMESERVER        26
#   define FCN_NETGETBROADCASTMODE      27
#   define FCN_NETGETCONNECTIONNUMBER   28
#   define FCN_NETSETBROADCASTMODE      29
#   define FCN_NETGETDIRPATHHANDLE      30
#   define FCN_NETGETINTNETADDRESS      31
#   define FCN_NETGETCONNINFO2          32
#   define FCN_NETGETSERVERNAME         33
#   define FCN_NETSETTTSIMPLICITOFF     34
#   define FCN_NETCHECKTTSAVAIL         35
#   define FCN_NETGETEXTATTRIBUTES      36
#   define FCN_NETSTARTTRANSACTION      37
#   define FCN_NETENDTRANSACTION        38
#   define FCN_NETGETMACHINENAME        39
#   define FCN_NETGETUNCFROMLETTER      40
#   define FCN_NETGETIPXSPXVERSION      41
#endif

// Functions added to support Lan Manager & NT Domains

#define FCN_NETSETPRIMARYDOMAIN         45
#define FCN_NETGETPRIMARYDOMAIN         46
#define FCN_NETDOMAINSENUM              47
#define FCN_NETDOMAINSETFULLNAMES       48

                                        // New commands, specific to NetWare
#define FCN_NETATTACH                   51
#define FCN_NETDETACH                   52
#define FCN_NETMAPROOTTOPATH            53
#define FCN_NETVALIDATEPASSWORD         54
#define FCN_NETOBJECTSENUM              55

                                        // New network commands
#define FCN_NETLOGINTONETWORK           62
#define FCN_NETLOGOUTFROMNETWORK        63
#define FCN_NETMESSAGESENDALL           64
#define FCN_NETADDMEMBERTOGROUP         65
#define FCN_NETGETCONNECTIONSTATE       66
#define FCN_NETRESTORECONNECTIONSTATE   67
#define FCN_NETRENAMEDIRECTORY          68
#define FCN_NETGETPROCESSCOUNT          69

                                        // New command, specific to Banyan
#define FCN_NETENUMSCOPEDOBJECTS        80

// 32-bit RPC-only functions
#define FCN_NETUSERSENUM32              100
#define FCN_NETGROUPSENUM32             101
#define FCN_NETSERVERSENUM32            102
#define FCN_NETVOLENUM32                103
#define FCN_NETOBJECTSENUM32            104
#define FCN_NETGROUPSIMINENUM32         105
#define FCN_NETDOMAINSENUM32            106
#define FCN_NETGETCURRENTUSERNAME32     107
#define FCN_NETGETWORKSTATIONNAME32     108
#define FCN_NETSETPRIMARYDOMAIN32       109
#define FCN_NETGETPRIMARYDOMAIN32       110
#define FCN_NETDOMAINSETFULLNAMES32     111
#define FCN_NETGETPRIMARYSERVER32       112
#define FCN_NETGETEVERYONEGROUP32       113
#define FCN_NETMAPROOTTOPATH32          114
#define FCN_NETRESTORECONNECTIONSTATE32 115
#define FCN_NETGETFILESERVERDESCRIPTION 116

#define FCN_NETDIRECTORYCONTAINERSENUM      130
#define FCN_NETDIRECTORYGETCONTEXT          131
#define FCN_NETDIRECTORYGETDEFNAMECONTEXT   132
#define FCN_NETDIRECTORYGETFULLNAMES        133
#define FCN_NETDIRECTORYGETLOGGEDINOBJECT   134
#define FCN_NETDIRECTORYGETTREEMODE         135
#define FCN_NETDIRECTORYGETTREENAME         136
#define FCN_NETDIRECTORYGROUPSENUM          137
#define FCN_NETDIRECTORYINIT                138
#define FCN_NETDIRECTORYISROOT              139
#define FCN_NETDIRECTORYISSUPPORTED         140
#define FCN_NETDIRECTORYSERVERSENUM         141
#define FCN_NETDIRECTORYSETFULLNAMES        142
#define FCN_NETDIRECTORYSETSERVERCONTEXT    143
#define FCN_NETDIRECTORYSETTREEMODE         144
#define FCN_NETDIRECTORYTEST                145
#define FCN_NETDIRECTORYTREEDOWN            146
#define FCN_NETDIRECTORYTREEUP              147
#define FCN_NETDIRECTORYUNINIT              148
#define FCN_NETDIRECTORYUSERSENUM           149

//------------------------------------------------------------------------
// PLATFORM Independent definitions
//------------------------------------------------------------------------

                                        // Network Type Definitions

                                        // default network types
#define NETTYPE_DEFAULT             0
#define NETTYPE_DEFAULT_REMOTE      255

#define NETTYPE_NONE                0
#define NETTYPE_MSNET               1
#define NETTYPE_LANMAN2             2
#define NETTYPE_NETWARE3            3
#define NETTYPE_VINES4              4
#define NETTYPE_10NET               5
#define NETTYPE_LOCUS               6
#define NETTYPE_SUN_PC_NFS          7
#define NETTYPE_LANSTEP             8
#define NETTYPE_9TILES              9
#define NETTYPE_LANTASTIC           10
#define NETTYPE_AS400               11
#define NETTYPE_FTP_NFS             12
#define NETTYPE_PATHWORKS           13
#define NETTYPE_MULTINET            14
#define NETTYPE_NETWARE4            15
#define NETTYPE_REMOTE              16  // net pipe based
#define NETTYPE_WINWORKGROUPS       17  // Windows for Workgroups
#define NETTYPE_NETWARE_NDS         18
#define NETTYPE_NT_LANMAN2_DOMAINS  19  // Domain support is provided thru LM 2.x API
#define NETTYPE_XPC                 20  // Win32/Win16 Homegrown RPC support

#define NETTYPE_SIMULATE            99  // Simulate Network Module

#define NETTYPE_UNLOAD              0xffff

                                        // Define the HANDLE to be used
                                        // for loading a special instance
                                        // of symnet
//typedef HANDLE          HSYMNET;
//typedef HANDLE FAR *    LPHSYMNET;

// HSYMNET used to be a handle, now its a 32-bit ptr to be consistent
//   on both the 32 and 16-bit platforms.
typedef LPVOID          HSYMNET, FAR * LPHSYMNET;



                                        // Standard error return codes for
                                        // this module
#define NETERR_INVALIDPARAMETER         0x0100
#define NETERR_UNKNOWN                  0x0101
#define NETERR_NONETWORK                0x0102
#define NETERR_NODRIVEMAPPING           0x0103
#define NETERR_NONETWORKPATH            0x0104
#define NETERR_SERVERNOTFOUND           0x0105
#define NETERR_INVALIDCONNECTION        0x0106
#define NETERR_NOTSUPPORTED             0x0107
#define NETERR_NOTTHESAME               0x0108
#define NETERR_INVALIDNETWORKMODULE     0x0109
#define NETERR_CANTLOADSUPPORTDLL       0x010A
#define NETERR_NOTMEMBEROFGROUP         0x010B
#define NETERR_NOPRIMARYSERVER          0x010C
#define NETERR_NOTENOUGHMEMORY          0x010D
#define NETERR_DRIVENOTREDIRECTED       0x010E
#define NETERR_PARAMSTRUCTSIZEINVALID   0x010F
#define NETERR_REGISTEREDNETWORKINUSE   0x0110
#define NETERR_NSOSTRUCTNOTINITIALZED   0x0111
#define NETERR_NSO_CMD_GETSITEDATADIR   0x0112
#define NETERR_NOSITELISTOBJECT         0x0113
#define NETERR_NSO_CMD_GETCURRENTSITEID 0x0114
#define NETERR_CONNECTION_NONE          0x0115
#define NETERR_CONNECTION_ATTACHED      0x0116
#define NETERR_CONNECTION_LOGGED        0x0117
#define NETERR_CANTALLOCATEINSTANCE     0x0118
#define NETERR_NOHANDLE                 0x0119
#define NETERR_CANTLOADXPC              0x011A
#define NETERR_INVALIDHANDLE            0x011B
#define NETERR_NO_SYMXPC                0x011C
#define NETERR_XPC_LOCK_FAILURE         0x011D
#define NETERR_INVALIDPATH              0x011E


// Maximum user name size


// Original defines from QUAKE
//#define NETMAXUSERNAME              64
//#define NETMAXSERVERNAME            64
//#define NETMAXGROUPNAME             64
//#define NETMAXVOLUMENAME            64

// These needed for compatibility with 16 bit UI for NAVNLM
// (...because NLM is now on CORE)
#define NAV_NETMAXUSERNAME          64
#define NAV_NETMAXSERVERNAME        64
#define NAV_NETMAXGROUPNAME         64
#define NAV_NETMAXVOLUMENAME        64

#define NETMAXUSERNAME              256
#define NETMAXSERVERNAME            256
#define NETMAXGROUPNAME             256
#define NETMAXVOLUMENAME            256

#define NETMAXOBJECTNAME            256
#define NETMAXPASSWORD              64
#define NETMAXBUFFER                SYM_MAX_PATH
#define NETMAXVOLUMECOUNT           64
#define NETLOCALBUFFER              16
#define NETMAXWORKSTATIONNAME       16
#define NETMAXMODULENAME            15
#define NETMAXDATATYPE              256
#define NETMAXDATADIR               256
#define NETMAXEXTUSERNAME           64+64-4



#define NETMAXNETWORKADDRESS        4
#define NETMAXNICADDRESS            6
#define NETMAXNICADDRESSTEXT        ((2*NETMAXNICADDRESS)+(2*NETMAXNETWORKADDRESS))
#define NETMAXMESSAGELENGTH         64

VOID SYM_EXPORT WINAPI NetOpenDebugFile(void);
VOID SYM_EXPORT WINAPI NetCloseDebugFile(void);
VOID SYM_EXPORT NetOutputDebugData(char *lpszFormat, ...);

#ifdef SYM_DEBUG
    #define dprintf(x) NetOutputDebugData x
#else
    #define dprintf(x)
#endif
                                        // NetGetServerDateAndTime
typedef struct {
    BYTE        byYear;                 // 80 = 1980, etc.
    BYTE        byMonth;                // 1..12
    BYTE        byDay;                  // 1..31
    BYTE        byHour;                 // 0..23
    BYTE        byMinute;               // 0..59
    BYTE        bySecond;               // 0..59
} NETDATETIME, NEAR * NPNETDATETIME, FAR * LPNETDATETIME;

                                        // Define the workstation nic address
typedef struct {
    BYTE        byNetwork [ NETMAXNETWORKADDRESS ];
    BYTE        byNIC [ NETMAXNICADDRESS ];
    BYTE        byReserved [ 16 ];      // Reserved 16 bytes for future
} NETNICADDRESS, FAR * LPNETNICADDRESS;

                                        // Define the connection structure
typedef struct {
    DWORD           dwStructSize;
    DWORD           dwID;
    BYTE            szUserName[NETMAXUSERNAME+1];
    NETNICADDRESS   rNICAddress;
    NETDATETIME     rLoginDateTime;
} NETCONNECTION, FAR * LPNETCONNECTION;


typedef struct {

    char    szCopyright[128];
    WORD    wNetType;
    BYTE    wReserved[512-128-2];

} SYMNET_GLOBAL_CONFIG, FAR * LPSYMNET_GLOBAL_CONFIG;

typedef struct {
    BYTE szCompany[81];
    BYTE szRevision[81];
    BYTE szRevisionDate[81];
    BYTE szCopyrightNotice[81];
} NETFILESERVERDESCRIPTIONREC, *LPNETFILESERVERDESCRIPTIONREC;

                                        // NetVolEnum callback structure
                                        // For each volume on a server, this
                                        // structure gets passed to the call-
                                        // back.
typedef struct {
    DWORD       dwStructSize;           // [out]    Size of structure
    DWORD       dwUserData;             // [out]    User data for callback
    LPCSTR      lpszServerName;         // [out]    Returns server name
    LPCSTR      lpszVolumeName;         // [out]    Returns volume name
    DWORD       dwTotalBlocks;
    DWORD       dwSectorsPerBlock;
    DWORD       dwAvailableBlocks;
    DWORD       dwTotalDirectorySlots;
    DWORD       dwAvailableDirectorySlots;
    DWORD       dwBytesPerSector;
    HSYMNET     hSymnet;                // [in]     Handle To Data or NULL
    SHORTBOOL   bVolumeIsRemovable;
} NETVOLENUMSTRUCT, NEAR *NPNETVOLENUMSTRUCT, FAR *LPNETVOLENUMSTRUCT;

                                        // NetGroupsEnum Structure for callback
typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    LPCSTR  lpszServerName;             // [in]     Server Name
    LPCSTR  lpszGroupName;              // [in]     Group Name
    LPCSTR  lpszUserName;               // [in]     User Name
    DWORD   dwUserData;                 // [in]     User data passed through
    HSYMNET hSymnet;                    // [in]     Handle To Data or NULL
} NETGROUPSENUMSTRUCT, NEAR * NPNETGROUPSENUMSTRUCT, FAR * LPNETGROUPSENUMSTRUCT;

                                        // NetUsersEnum Structure for callback
typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    LPCSTR  lpszServerName;             // [in]     Server Name
    LPCSTR  lpszUserName;               // [in]     User Name
    DWORD   dwSpecialID;                // [in]     Special User I.d.
    DWORD   dwUserData;                 // [in]     User data passed through
    HSYMNET hSymnet;                    // [in]     Handle To Data or NULL
} NETUSERSENUMSTRUCT, NEAR * NPNETUSERSENUMSTRUCT, FAR * LPNETUSERSENUMSTRUCT;

#define MAX_REGISTERED_NETWORKS 20

#ifdef SYM_WIN
/*****************************************************************************
 These two structures are used by the 32-bit "RPC'd" version of SYMNET for
 performance reasons.
*****************************************************************************/

#define MAX_ENUM_ARRAY_LEN              6
//#define MAX_ENUM_ARRAY_LEN              16
//#define OBJECT_NAME_BUFFER_LEN          64

                                        // Struct used for fast user enum
typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    DWORD   dwSpecialID;                // [in]     Special User I.d.
    DWORD   dwUserData;                 // [in]     User data passed through
    HSYMNET hSymnet;                    // [in]     Handle To Data or NULL
    BYTE    szServerName[NETMAXSERVERNAME + 1];               // [in]     Server Name
    BYTE    szUserName[NETMAXUSERNAME + 3];                   // [in]     User Name
} NETUSERSENUM32STRUCT, NEAR * NPNETUSERSENUM32STRUCT, FAR * LPNETUSERSENUM32STRUCT;

typedef struct {
    DWORD                   dwUserCount;
    NETUSERSENUM32STRUCT    sList[MAX_ENUM_ARRAY_LEN];
} NETUSERSENUM32ARRAY, NEAR * NPNETUSERSENUM32ARRAY, FAR * LPNETUSERSENUM32ARRAY;

                                        // Struct used for fast volume enum
typedef struct {
    DWORD       dwStructSize;           // [out]    Size of structure
    DWORD       dwUserData;             // [out]    User data for callback
    DWORD       dwTotalBlocks;
    DWORD       dwSectorsPerBlock;
    DWORD       dwAvailableBlocks;
    DWORD       dwTotalDirectorySlots;
    DWORD       dwAvailableDirectorySlots;
    DWORD       dwBytesPerSector;
    DWORD       dwVolumeIsRemovable;
    HSYMNET     hSymnet;                // [in]     Handle To Data or NULL
    BYTE        szServerName[NETMAXSERVERNAME + 1];// [out]    Returns server name
    BYTE        szVolumeName[NETMAXVOLUMENAME + 3];// [out]    Returns volume name
} NETVOLENUM32STRUCT, NEAR *NPNETVOLENUM32STRUCT, FAR *LPNETVOLENUM32STRUCT;

typedef struct {
    DWORD               dwVolCount;
    NETVOLENUM32STRUCT  sList[MAX_ENUM_ARRAY_LEN];
} NETVOLENUM32ARRAY, NEAR * NPNETVOLENUM32ARRAY, FAR * LPNETVOLENUM32ARRAY;

                                        // Struct used for fast group enum
typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    DWORD   dwUserData;                 // [in]     User data passed through
    HSYMNET hSymnet;                    // [in]     Handle To Data or NULL
    BYTE    szServerName[NETMAXSERVERNAME + 1];             // [in]  Server Name
    BYTE    szGroupName[NETMAXGROUPNAME + 1];               // [in]  Group Name
    BYTE    szUserName[NETMAXUSERNAME + 5];                   // [in]  User Name
} NETGROUPSENUM32STRUCT, NEAR * NPNETGROUPSENUM32STRUCT, FAR * LPNETGROUPSENUM32STRUCT;

typedef struct {
    DWORD                   dwGroupCount;
    NETGROUPSENUM32STRUCT   sList[MAX_ENUM_ARRAY_LEN];
} NETGROUPSENUM32ARRAY, NEAR * NPNETGROUPSENUM32ARRAY, FAR * LPNETGROUPSENUM32ARRAY;

                                        // Used for NetGroupsImInEnum32()
typedef struct {
    DWORD                   dwGroupCount;
    DWORD                   dwSegment;
    DWORD                   dwMoreSegments;
    NETGROUPSENUM32STRUCT   sList[MAX_ENUM_ARRAY_LEN];
} NETGROUPSIMINENUM32ARRAY, NEAR * NPNETGROUPSIMINENUM32ARRAY, FAR * LPNETGROUPSIMINENUM32ARRAY;

                                        // Struct used for fast server enum
typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    DWORD   dwUserData;                 // [in]     User data passed through
    HSYMNET hSymnet;                    // [in]     Handle To Data or NULL
    BYTE    szServerName[NETMAXSERVERNAME + 3];      // [in]     Server Name
} NETSERVERSENUM32STRUCT, NEAR * NPNETSERVERSENUM32STRUCT, FAR * LPNETSERVERSENUM32STRUCT;

typedef struct {
    DWORD                   dwServerCount;
    NETSERVERSENUM32STRUCT  sList[MAX_ENUM_ARRAY_LEN];
} NETSERVERSENUM32ARRAY, NEAR * NPNETSERVERSENUM32ARRAY, FAR * LPNETSERVERSENUM32ARRAY;

typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    DWORD   dwUserData;                 // [in]     User data passed through
    HSYMNET hSymnet;                    // [in]     Handle To Data or NULL
    BYTE    szObjectName[NETMAXOBJECTNAME + 3];       // [in]     Server Name
} NETOBJECTSENUM32STRUCT, NEAR * NPNETOBJECTSENUM32STRUCT, FAR * LPNETOBJECTSENUM32STRUCT;

typedef struct {
    DWORD                   dwObjectCount;
    NETOBJECTSENUM32STRUCT  sList[MAX_ENUM_ARRAY_LEN];
} NETOBJECTSENUM32ARRAY, NEAR * NPNETOBJECTSENUM32ARRAY, FAR * LPNETOBJECTSENUM32ARRAY;

#endif

/*****************************************************************************/

                                        // NetServersEnum Structure for callback
typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    LPCSTR  lpszServerName;             // [in]     Server Name
    DWORD   dwUserData;                 // [in]     User data passed through
    HSYMNET hSymnet;                    // [in]     Handle To Data or NULL
} NETSERVERSENUMSTRUCT, NEAR * NPNETSERVERSENUMSTRUCT, FAR * LPNETSERVERSENUMSTRUCT;

typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    LPCSTR  lpszObjectName;             // [in]     Server Name
    DWORD   dwUserData;                 // [in]     User data passed through
    HSYMNET hSymnet;                    // [in]     Handle To Data or NULL
} NETOBJECTSENUMSTRUCT, NEAR * NPNETOBJECTSENUMSTRUCT, FAR * LPNETOBJECTSENUMSTRUCT;


#ifdef SYM_OS2
                                        // Prototype for NetServersEnum callback
typedef BOOL (*CALLBACK NETSERVERSENUMPROC)(LPNETSERVERSENUMSTRUCT);

                                        // Prototype for NetObjectsEnum callback
typedef BOOL (*CALLBACK NETOBJECTSENUMPROC)(LPNETOBJECTSENUMSTRUCT);

                                        // Prototype for NetUsersEnum callback
typedef BOOL (*CALLBACK NETUSERSENUMPROC)(LPNETUSERSENUMSTRUCT);

                                        // Prototype for NetGroupsImInEnum callback
                                        // and NetGroupsEnum callback
typedef BOOL (*CALLBACK NETGROUPSENUMPROC)(LPNETGROUPSENUMSTRUCT);


                                        // Prototype for NetVolEnum callback
typedef BOOL (*CALLBACK NETVOLENUMPROC)(LPNETVOLENUMSTRUCT);

typedef WORD (*CALLBACK NETENUMSCOPEPROC)(
    LPSTR       lpszScope,              // [in]     Scope
    LPSTR       lpszObjectName,         // [in]     Object Name
    WORD        wObjectType,            // [in]     Object Type
    DWORD       dwParam                 // [in]     Luggage
);

#else
                                        // Prototype for NetServersEnum callback
typedef BOOL (CALLBACK* NETSERVERSENUMPROC)(LPNETSERVERSENUMSTRUCT);


                                        // Prototype for NetObjectsEnum callback
typedef BOOL (CALLBACK* NETOBJECTSENUMPROC)(LPNETOBJECTSENUMSTRUCT);

                                        // Prototype for NetUsersEnum callback
typedef BOOL (CALLBACK* NETUSERSENUMPROC)(LPNETUSERSENUMSTRUCT);

                                        // Prototype for NetGroupsImInEnum callback
                                        // and NetGroupsEnum callback
typedef BOOL (CALLBACK* NETGROUPSENUMPROC)(LPNETGROUPSENUMSTRUCT);


                                        // Prototype for NetVolEnum callback
typedef BOOL (CALLBACK* NETVOLENUMPROC)(LPNETVOLENUMSTRUCT);

typedef WORD (CALLBACK* NETENUMSCOPEPROC)(
    LPSTR       lpszScope,              // [in]     Scope
    LPSTR       lpszObjectName,         // [in]     Object Name
    WORD        wObjectType,            // [in]     Object Type
    DWORD       dwParam                 // [in]     Luggage
);

#endif // #ifdef _OS2

                                        // Function to lock XPC mechanism
BOOL WINAPI LockXPC();
VOID WINAPI UnlockXPC();

void SYM_EXPORT WINAPI NetAllowMessage (
    SHORTBOOL       bState              // [in]     TRUE to display messages
);


WORD SYM_EXPORT WINAPI NetRegisterExEx (
    LPHSYMNET   lphSymnet,              // [in/out] Pointer Handle To Data or NULL
    WORD        wNetType,               // [in]     Net Type (NULL is default)
    LPSTR       lpszSimulationModule,   // [in]     Simulation module to use
    LPSTR       lpszCommonDir           // [in]     Directory where global overrides are kept (DATA)
);

WORD SYM_EXPORT WINAPI NetRegisterEx (
    LPHSYMNET   lphSymnet,              // [in/out] Pointer Handle To Data or NULL
    WORD        wNetType,               // [in]     Net Type (NULL is default)
    LPSTR       lpszSimulationModule    // [in]     Simulation module to use
);

WORD SYM_EXPORT WINAPI NetRegister (
    LPHSYMNET   lphSymnet,              // [in/out] Pointer Handle To Data or NULL
    WORD        wNetType                // [in]     Net Type (NULL is default)
);

WORD SYM_EXPORT WINAPI NetUnRegister (
    HSYMNET     hSymnet                 // [in]     Handle To Data or NULL
);

WORD SYM_EXPORT WINAPI NetRegisterByName (
    LPHSYMNET lphSymnet,                // [in/out] Pointer to SYMNET handle
    LPSTR lpszProvider,                 // [in]     Provider name
    DWORD       dwPrimary               // [in]     32-bit boolean indicating if primary
);

WORD SYM_EXPORT WINAPI NetRegisterByType (
    LPHSYMNET   lphSymnet,              // [in/out] Pointer Handle To Data or NULL
    WORD        wNetType                // [in]     Net Type (NULL is default)
);

#ifdef SYM_WIN32
BOOL SYM_EXPORT WINAPI NetIsClient32Loaded();
#endif

WORD SYM_EXPORT WINAPI NetAllocateRegistrationInstance (
    LPHSYMNET   lphSymnet               // [in/out] Pointer to SYMNET handle
);

WORD SYM_EXPORT WINAPI NetFreeRegistrationInstance (
    HSYMNET     hSymnet                 // [in]     SYMNET handle
);

WORD SYM_EXPORT WINAPI NetRenameDirectory (
    HSYMNET     hSymnet,                // [in]     SYMNET handle
    LPCSTR      lpszDirectoryPath,      // [in]     Path of dir to be renamed
    LPCSTR      lpszNewDirName          // [in]     New directory name
);

WORD SYM_EXPORT WINAPI NetVolEnum (
    HSYMNET     hSymnet,                // [in] Handle To Data or NULL
    LPCSTR      lpszServerName,         // [in] Server name to look up
    DWORD       dwUserData,             // [in] Userdata to return in callback
    NETVOLENUMPROC lpfnNetVolEnumProc   // [in] Callback to enum vols to
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetVolEnum32 (
    HSYMNET     hSymnet,                // [in] Handle To Data or NULL
    LPCSTR      lpszServerName,         // [in] Server name to look up
    DWORD       dwUserData,             // [in] Userdata to return in callback
    LPDWORD     lpdwObjectID,                   // [in/out] Seed value for bindery search
    DWORD       dwStartVolumeNumber,
    LPNETVOLENUM32ARRAY lpVolArray              // [out] Buffer to hold user name structs
);
#endif

                                        // SYMNET DOS/WINDOWS function prototypes

WORD SYM_EXPORT WINAPI NetGetNetType (
    HSYMNET     hSymnet                 // [in]     Handle To Data or NULL
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetAddProcess(DWORD dwProcessID);
WORD SYM_EXPORT WINAPI NetRemoveProcess(DWORD dwProcessID);
WORD SYM_EXPORT WINAPI NetGetProcessCount();
#endif

#ifdef SYM_WIN16
VOID SYM_EXPORT WINAPI NetGetConnectedProcessList(DWORD *lpdwList, WORD wLen);
#endif

WORD SYM_EXPORT WINAPI NetGetRegisteredNetworks (
    HSYMNET     hSymnet,                // [in]     Handle To Data or NULL
    LPSTR       lpszNetList,            // [out,size_is(dwCount)] Holds the list of registered networks
    DWORD       dwCount                 // [in]     Length of dword buffer
);

WORD SYM_EXPORT WINAPI NetGetNetworkName (
    HSYMNET     hSymnet,                // [in]     Handle To Data or NULL
    DWORD       dwNetType,              // [in]     Net type to get name of.
    LPSTR       lpszNetName,            // [out,size_is(dwNetNameLen)] Buffer to hold net name
    DWORD       dwNetNameLen            // [in]     Length of buffer
);

WORD SYM_EXPORT WINAPI NetSetNetTypeEx (
    HSYMNET     hSymnet,                // [in]     Handle To Data or NULL
    WORD        wNetType,               // [in]     Net Type (NULL is default)
    LPSTR       lpszSimulationModule    // [in]     Simulation module to use
);

WORD SYM_EXPORT WINAPI NetSetNetTypeExEx (
    HSYMNET     hSymnet,                // [in]     Handle To Data or NULL
    WORD        wNetType,               // [in]     Net Type (NULL is default)
    LPSTR       lpszSimulationModule,   // [in]     Simulation module to use
    LPSTR       lpszCommonDir           // [in]     Directory where global overrides are stored (DATA)
);

WORD SYM_EXPORT WINAPI NetSetNetType (
    HSYMNET     hSymnet,                // [in]     Handle To Data or NULL
    WORD        wNetType                // [in]     Net Type (NULL is default)
);

WORD SYM_EXPORT WINAPI NetSetCurrentNetwork(
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    DWORD dwNetType                     // [in]     Net Type
);

#if 0
WORD SYM_EXPORT WINAPI NetGetNetDataFromNetHandle(
    HSYMNET hSymnet,                    // [in]     Handle to Data or NULL
    LPNETWORKDATA lpNetData             // [out]    Net data structure
);
#endif

WORD SYM_EXPORT WINAPI NetGetErrorString (
    HSYMNET     hSymnet,                // [in]     Handle To Data or NULL
    WORD        wErrorCode,             // [in]     Error code
    LPSTR       lpszBuffer,             // [out,size_is(wBufSize)]    Error string buffer to fill
    WORD        wBufSize                // [in]     Maximum buffer size
);

                                        // Prototype for NetGetServerDateAndTime
WORD SYM_EXPORT WINAPI NetGetServerDateAndTime (
    HSYMNET         hSymnet,            // [in]     Handle To Data or NULL
    LPNETDATETIME   lpdt                // [out]    Date/Time structure filled
);

WORD SYM_EXPORT WINAPI NetGetCurrentUserName (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszName,                   // [out,size_is(wBufSize)]    User Name buffer
    WORD    wBufSize                    // [in]     Size of username buffer
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetGetCurrentUserName32 (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszName,                   // [out,size_is(dwBufSize)]    User Name buffer
    DWORD   dwBufSize                   // [in]     Size of username buffer
);
#endif

WORD SYM_EXPORT WINAPI NetAddConnection (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszNetPath,                // [in]     Network Path to use
    LPSTR   lpszPassword,               // [in]     Password to use
    LPSTR   lpszLocal                   // [in]     Local device to map
);

WORD SYM_EXPORT WINAPI NetGetConnection (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszLocal,                  // [in]     Local device
    LPSTR   lpszNetPath,                // [out,size_is(*lpwSize)]    Net Path found
    LPWORD  lpwSize                     // [in/out] Max size of buffer and actual
);

WORD SYM_EXPORT WINAPI NetCancelConnection (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszLocal,                  // [in]     Local device to cancel
    SHORTBOOL  bForce                      // [in]     Force cancel
);

WORD SYM_EXPORT WINAPI NetGetLocalDevice (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszNetPath,                // [in]     Net Path
    LPSTR   lpszLocal                   // [out]    Local device found
);

WORD SYM_EXPORT WINAPI NetGetAvailableDrive (
    HSYMNET hSymnet,                    // [in] Handle To Data or NULL
    LPSTR   lpszLocal                   // [out]    Local Device available
);

WORD SYM_EXPORT WINAPI NetParseNetPath (
    HSYMNET hSymnet,                    // [in] Handle To Data or NULL
    LPSTR   lpszPath,                   // [in]     Path to parse
    LPSTR   lpszNetPath,                // [out,size_is(wSize)]    Parsed path
    WORD    wSize                       // [in]     Size of buffer
);

WORD SYM_EXPORT WINAPI NetDrivePathFromNetPath (
    HSYMNET hSymnet,                    // [in] Handle To Data or NULL
    LPSTR   lpszNetPath,                // [in]     Network Path use
    LPSTR   lpszDrivePath,              // [out,size_is(wSize)]    New Path
    WORD    wSize                       // [in]     Buffer Size
);


WORD SYM_EXPORT WINAPI NetServerFromPath (
    HSYMNET     hSymnet,                // [in] Handle To Data or NULL
    LPCSTR      lpszPath,               // [in]     Path to use
    LPSTR       lpszServer,             // [out,size_is(wSize)]    Server Name
    WORD        wBufSize                // [in]     Size of buffer for server name
);

WORD SYM_EXPORT WINAPI NetGetWorkstationName (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszName,                   // [out,size_is(wBufSize)]    Workstation Name Buffer
    WORD    wBufSize                    // [in]     Maximum size of buffer
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetGetWorkstationName32 (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszName,                   // [out,size_is(dwBufSize)]    Workstation Name Buffer
    DWORD   dwBufSize                   // [in]     Maximum size of buffer
);
#endif

WORD SYM_EXPORT WINAPI NetGetNICAddress (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPNETNICADDRESS     lpnic           // [out]    Workstation NIC Address
);

VOID SYM_EXPORT WINAPI NetFormatNICAddress (
    HSYMNET             hSymnet,        // [in] Handle To Data or NULL
    LPNETNICADDRESS     lpnic,          // [in]     Workstation NIC Address
    SHORTBOOL           bIncludeNet,    // [in]     Include network number flag
    LPSTR               lpszAddress,    // [out,size_is(wBufSize)]    Address of formed string
    WORD                wBufSize        // [in]     Buffer size for formatted text
);

VOID SYM_EXPORT WINAPI NetFormatNICAddressEx (
    HSYMNET             hSymnet,        // [in] Handle To Data or NULL
    LPNETNICADDRESS     lpnic,          // [in]     Workstation NIC Address
    SHORTBOOL           bIncludeNet,    // [in]     Include network number flag
    LPSTR               lpszAddress,    // [out,size_is(wBufSize)]    Address of formed string
    WORD                wBufSize,       // [in]     Buffer size for formatted text
    WORD                wNetAddrFormat  // [in]     Format of NIC address (0 = Ethernet, 1=Token Ring)
);

WORD SYM_EXPORT WINAPI NetIsMemberOfGroup (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszGroup,                  // [in]     Group Name
    LPSTR   lpszUserName                // [in]     User Name in question
);

WORD SYM_EXPORT WINAPI NetGetFileServerDescription(
    HSYMNET                         hSymnet,// [in]     Handle To Data or NULL
    LPSTR                           lpszServer,// [in]     Server name
    LPNETFILESERVERDESCRIPTIONREC   lpDescRec// [out] Description strings
);

WORD SYM_EXPORT WINAPI NetGetConnectionInfo (
    HSYMNET         hSymnet,            // [in]     Handle To Data or NULL
    LPSTR           lpszServer,         // [in]     Server
    LPNETCONNECTION lpConnection        // [out]    Connection Struct
);

WORD SYM_EXPORT WINAPI NetVerifyConnection (
    HSYMNET         hSymnet,            // [in]     Handle To Data or NULL
    LPSTR           lpszServer,         // [in]     Server
    LPNETCONNECTION lpConnection        // [in]     Connection Struct
);

WORD SYM_EXPORT WINAPI NetVerifyNICAddress (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPNETNICADDRESS     lpnic           // [in]     Workstation NIC Address
);

WORD SYM_EXPORT WINAPI NetFunctionSupported (
    HSYMNET     hSymnet,                // [in]     Handle To Data or NULL
    WORD        wFunctionID             // [in]     Function I.D.
);

WORD SYM_EXPORT WINAPI NetGroupsImInEnum (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to look up
    LPCSTR              lpszUserName,           // [in] UserName to lookup
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETGROUPSENUMPROC   lpfnNetGroupsEnumProc   // [in] Callback
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetGroupsImInEnum32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to look up
    LPCSTR              lpszUserName,           // [in] UserName to lookup
    DWORD               dwUserData,              // [in/out] Seed value for bindery search
    LPNETGROUPSIMINENUM32ARRAY  lpGroupImInArray        // [out] Buffer to hold user name structs
);
#endif

WORD SYM_EXPORT WINAPI NetGroupsEnum (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETGROUPSENUMPROC   lpfnNetGroupsEnumProc
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetGroupsEnum32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    LPDWORD             lpdwObjectID,           // [in/out] Seed value for bindery search
    LPNETGROUPSENUM32ARRAY  lpGroupArray        // [out] Buffer to hold user name structs
);
#endif

WORD SYM_EXPORT WINAPI NetMessageSend (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszDestUser,               // [in]     User to send to
    LPSTR   lpszMessage,                // [in]     Message to send (NETMAXMESSAGE length)
    LPSTR   lpszFromUser                // [in] (optional)  From UserName
);

WORD SYM_EXPORT WINAPI NetMessageSendGroup (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszGroupUser,              // [in]     Group to send to
    LPSTR   lpszMessage,                // [in]     Message to send (NETMAXMESSAGE length)
    LPSTR   lpszFromUser                // [in] (optional)  From UserName
);

WORD SYM_EXPORT WINAPI NetMessageSendToConsole (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszServer,                 // [in]     Server console to use
    LPSTR   lpszMessage,                // [in]     Message to send (NETMAXMESSAGE length)
    LPSTR   lpszFromUser                // [in] (optional)  From UserName
);

WORD SYM_EXPORT WINAPI NetUsersEnum (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETUSERSENUMPROC    lpfnNetUsersEnumProc
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetUsersEnum32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    LPDWORD             lpdwObjectID,           // [in/out] Seed value for bindery search
    LPNETUSERSENUM32ARRAY  lpUserArray          // [out] Buffer to hold user name structs
);
#endif

WORD SYM_EXPORT WINAPI NetServersEnum (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETSERVERSENUMPROC  lpfnNetServersEnumProc
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetServersEnum32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    LPDWORD             lpdwObjectID,           // [in/out] Seed value for bindery search
    LPNETSERVERSENUM32ARRAY  lpServerArray      // [out] Buffer to hold server name structs
);
#endif

WORD SYM_EXPORT WINAPI NetObjectsEnum (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    DWORD               dwSearchType,           // [in] Enum this type
    NETOBJECTSENUMPROC  lpfnNetObjectsEnumProc
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetObjectsEnum32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    DWORD               dwSearchType,           // [in] Enum this type
    LPDWORD             lpdwObjectID,           // [in/out] Seed value for bindery search
    LPNETOBJECTSENUM32ARRAY  lpObjectArray      // [out] Buffer to hold server name structs
);
#endif

WORD SYM_EXPORT WINAPI NetDomainsEnum (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETSERVERSENUMPROC  lpfnNetServersEnumProc  // NOTE: Use the same callback as NetServersEnum.
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetDomainsEnum32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    DWORD               dwUserData,             // [in] Userdata to return in callback
    DWORD               dwStart,                // [in/out] Seed value for bindery search
    LPNETSERVERSENUM32ARRAY  lpDomainArray      // [out] Buffer to hold server name structs
);
#endif

WORD SYM_EXPORT WINAPI NetGetPrimaryDomain (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPSTR               lpszDomain,             // [out,size_is(wBufSize)]    Returns current enumeration domain name
    WORD                wBufSize                // [in]     Buffer Size
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetGetPrimaryDomain32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPSTR               lpszDomain,             // [out,size_is(dwBufSize)]    Returns current enumeration domain name
    DWORD               dwBufSize               // [in]     Buffer Size
);
#endif

WORD SYM_EXPORT WINAPI NetSetPrimaryDomain (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPSTR               lpszNewDomain,          // [in]     New enumeration domain
    SHORTBOOL           bFromServerName         // [in]     if TRUE, lpszNewDomain == a server name
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetSetPrimaryDomain32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPSTR               lpszNewDomain,          // [in]     New enumeration domain
    DWORD               dwFromServerName        // [in]     if TRUE, lpszNewDomain == a server name
);
#endif

WORD SYM_EXPORT WINAPI NetDomainSetFullNames (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    SHORTBOOL           bUseFullNames           // [in] Insert domain in object names?
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetDomainSetFullNames32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    DWORD               dwUseFullNames          // [in] Insert domain in object names?
);
#endif

WORD SYM_EXPORT WINAPI NetGetPrimaryServer (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPSTR               lpszServer,             // [out,size_is(wBufSize)]    Server name found
    WORD                wBufSize                // [in]     Buffer Size
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetGetPrimaryServer32 (
    HSYMNET             hSymnet,                // [in]     Handle To Data or NULL
    LPSTR               lpszServer,             // [out,size_is(dwBufSize)]    Server name found
    DWORD               dwBufSize               // [in]     Buffer Size
);
#endif

WORD SYM_EXPORT WINAPI NetPathFromDrivePath (
    HSYMNET hSymnet,                    // [in] Handle To Data or NULL
    LPSTR   lpszDrivePath,              // [in]     Drive Path
    LPSTR   lpszNetPath,                // [out,size_is(wSize)]    Network Path
    WORD    wSize                       // [in]     Buffer Size
);

WORD SYM_EXPORT WINAPI NetPathNormalize (
    HSYMNET hSymnet,                    // [in] Handle To Data or NULL
    LPSTR   lpszNetPath,                // [in]     Original Net Path
    LPSTR   lpszNormalPath,             // [out,size_is(wSize)]    Network Path
    WORD    wSize                       // [in]     Buffer Size
);

WORD SYM_EXPORT WINAPI NetVolGetInfo (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPCSTR              lpszServer,     // [in]     Server name to look up
    LPCSTR              lpszVolume,     // [in]     Name of volume to inquire about
    LPNETVOLENUMSTRUCT  lpvolinfo       // [out]    Volume Info Struct to fill
);

WORD SYM_EXPORT WINAPI NetGetEveryoneGroup (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszGroup,      // [out,size_is(wBufSize)]    Group Name
    WORD                wBufSize        // [in]     Size of buffer
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetGetEveryoneGroup32 (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszGroup,      // [out,size_is(dwBufSize)]    Group Name
    DWORD               dwBufSize       // [in]     Size of buffer
);
#endif

WORD SYM_EXPORT WINAPI NetGetServerAndVolume (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    BYTE    byDrive,                    // [in]     Drive Letter 'A'-'Z'
    LPSTR   lpszServer,                 // [out,size_is(wServerSize)]    Server Name
    WORD    wServerSize,                // [in]     Size of Servername buffer
    LPSTR   lpszVol,                    // [out]    Volume Name
    WORD    wVolSize                    // [in,size_is(wVolSize)]     Size of Volumename buffer
);

WORD SYM_EXPORT WINAPI NetSubstEnvironment(
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszData                    // [in/out]     Buffer
);

VOID SYM_EXPORT WINAPI NetSetConfigDir(
    HSYMNET hSymnet,                    // [in]     Handle to Data or NULL
    LPSTR   lpszDir                     // [in]     Directory
);

VOID SYM_EXPORT WINAPI NetGetConfigDir(
    HSYMNET hSymnet,                    // [in]     Handle to Data or NULL
    LPSTR   lpszDir,                    // [out,size_is(wBufSize)]    Directory
    WORD    wBufSize                    // [in]     Buffer Size
);

WORD SYM_EXPORT WINAPI NetSetCommonDir(
    HSYMNET hSymnet,                    // [in]     Handle to Data or NULL
    LPSTR   lpszDir                     // [in]     Directory
);

WORD SYM_EXPORT WINAPI NetGetCommonDir(
    HSYMNET hSymnet,                    // [in]     Handle to Data or NULL
    LPSTR   lpszDir,                    // [out,size_is(wBufSize)]    Directory
    WORD    wBufSize                    // [in]     Buffer Size
);

WORD SYM_EXPORT WINAPI NetSetConfigNIC(
    HSYMNET         hSymnet,            // [in]     Handle to Data or NULL
    LPNETNICADDRESS lpnic               // [in]     Workstation NIC Address
);

WORD SYM_EXPORT WINAPI NetGetConfigNIC(
    HSYMNET         hSymnet,            // [in]     Handle to Data or NULL
    LPNETNICADDRESS lpnic               // [out]    Workstation NIC Address
);

BYTE SYM_EXPORT WINAPI NetGetPrimeServer (
    HSYMNET             hSymnet         // [in]     Handle To Data or NULL
);

BYTE SYM_EXPORT WINAPI NetGetBroadcastMode (
    HSYMNET             hSymnet         // [in]     Handle To Data or NULL
);

BYTE SYM_EXPORT WINAPI NetSetBroadcastMode (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    BYTE                byMode          //  [in]    Mode to set to
);
BYTE SYM_EXPORT WINAPI NetGetConnectionNumber (
    HSYMNET             hSymnet         // [in]     Handle To Data or NULL
);

BYTE SYM_EXPORT WINAPI NetGetDirPathHandle (
    HSYMNET             hSymnet,
    BYTE                byHandle,       //  [in]    Handle for which get path
    BYTE far *          szDirectoryPath //  [out]   Path.
);

WORD SYM_EXPORT WINAPI NetGetIntnetAddress (
    HSYMNET             hSymnet,
    WORD                wConnection,
    BYTE * prAddress
);

BYTE SYM_EXPORT WINAPI NetGetConnInfo2 (
    HSYMNET             hSymnet,
    WORD                wConnectionNumber,
    CONNECTION_INFO2 *   prConnectionInfo
);

WORD SYM_EXPORT WINAPI NetGetServerName (
    HSYMNET             hSymnet,
    SERVERTABLE_REC far * prConnectionInfo
);

WORD SYM_EXPORT WINAPI NetSetTTSImplicitOff (
    HSYMNET             hSymnet         // [in]     Handle To Data or NULL
);

BYTE SYM_EXPORT WINAPI NetCheckTTSAvail (
    HSYMNET             hSymnet         // [in]     Handle To Data or NULL
);

BYTE SYM_EXPORT WINAPI NetGetExtAttributes(
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    BYTE *              szFileName
);

WORD SYM_EXPORT WINAPI NetStartTransaction (
    HSYMNET             hSymnet         // [in]     Handle To Data or NULL
);

WORD SYM_EXPORT WINAPI NetEndTransaction (
    HSYMNET             hSymnet         // [in]     Handle To Data or NULL
);

#ifdef SYM_OS2
WORD SYM_EXPORT WINAPI NetGetUNCFromLetter (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    WORD                wDrvNum,        //  in - drive number to check
    BYTE *              pszNetPath,     //  out - UNC name on network
    USHORT              usBufSize       //  in - size of buffer for net path
);
#endif


WORD SYM_EXPORT WINAPI NetGetIPXSPXVersion(
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    BYTE *              bIPXMaj,        //  [out]   Pointer to IPX Major Version
    BYTE *              bIPXMin,        //  [out]   Pointer to IPX Minor Version
    BYTE *              bSPXMaj,        //  [out]   Pointer to SPX Major Version
    BYTE *              bSPXMin         //  [out]   Pointer to SPX Minor Version
);


WORD SYM_EXPORT WINAPI NetGetMachineName (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    BYTE *              pszName,
    unsigned short      usBufSize
);

WORD SYM_EXPORT WINAPI NetAttach (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszFileServer,
    LPSTR               lpszUserName,
    LPSTR               lpszPassword
);

WORD SYM_EXPORT WINAPI NetDetach (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszFileServer
);

WORD SYM_EXPORT WINAPI NetMapRootToPath (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    BYTE                bDrive,         // [in]     Drive Number (A=0, B=1, etc.)
    LPSTR               lpszPath        // [in]     Drive to map to
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetMapRootToPath32 (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    DWORD               dwDrive,        // [in]     Drive Number (A=0, B=1, etc.)
    LPSTR               lpszPath        // [in]     Drive to map to
);
#endif

WORD SYM_EXPORT WINAPI NetDisplayShareAsDialog (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    HWND                hWndParent,     // [in]     Parent Window
    WORD                wType,          // [in]     Type of resource (WNTYPE_DRIVE)
    LPSTR               lpszPath        // [in]     Full path to resource to share
);

WORD SYM_EXPORT WINAPI NetDisplayStopShareDialog (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    HWND                hWndParent,     // [in]     Parent Window
    WORD                wType,          // [in]     Type of resource (WNTYPE_DRIVE)
    LPSTR               lpszPath        // [in]     Full path to resource to share
);

WORD SYM_EXPORT WINAPI NetLoginToNetwork (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszName,
    LPSTR               lpszPassword,
    LPSTR               lpszNetwork
);

WORD SYM_EXPORT WINAPI NetLogoutFromNetwork (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszName,
    LPSTR               lpszNetwork
);

WORD SYM_EXPORT WINAPI NetMessageSendAll (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszServer,     // [in]     Server to query
    LPSTR               lpszMessage,    // [in]     Message to send (NETMAXMESSAGE length)
    LPSTR               lpszFromUser    // [in]     (optional)  From UserName
);

WORD SYM_EXPORT WINAPI NetAddMembertoGroup (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszServer,                 // [in]     Server
    LPSTR   lpszGroup,                  // [in]     Group Name
    LPSTR   lpszUserName                // [in]     User Name to add
);

WORD SYM_EXPORT WINAPI NetValidatePassword (
    HSYMNET hSymnet,                    // [in]     Handle To Data or NULL
    LPSTR   lpszUserName,               // [in]     User name
    LPSTR   lpszPassword                // [in]     Password to validate
);

WORD SYM_EXPORT WINAPI NetGetConnectionState (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszName,       // [in]
    LPSTR               lpszFileServer  // [in]     File server of interest
);

WORD SYM_EXPORT WINAPI NetRestoreConnectionState (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszName,       // [in]
    LPSTR               lpszFileServer, // [in]     File server of interest
    WORD                wOldState       // [in]     State to restore to
);

#ifdef SYM_WIN
WORD SYM_EXPORT WINAPI NetRestoreConnectionState32 (
    HSYMNET             hSymnet,        // [in]     Handle To Data or NULL
    LPSTR               lpszName,       // [in]
    LPSTR               lpszFileServer, // [in]     File server of interest
    DWORD               dwOldState      // [in]     State to restore to
);
#endif

WORD SYM_EXPORT WINAPI NetCheckType ();

/****************************************************************************/
/* The following is from NETDIR.H. It is the stuff needed for NDS calls     */
/****************************************************************************/
// import ordinals - must match symnds.def and oslib.h/oslib.c in NAV

#define NDS_INIT_ORD                175
#define NDS_TREEUP_ORD              176
#define NDS_TREEDOWN_ORD            177
#define NDS_ISROOT_ORD              178
#define NDS_GETCONTEXT_ORD          179
#define NDS_CONTAINERSENUM_ORD      180
#define NDS_SERVERSENUM_ORD         181
#define NDS_USERSENUM_ORD           182
#define NDS_GROUPSENUM_ORD          183
#define NDS_UNINT_ORD               184
#define NDS_ISSUPPORTED_ORD         185
#define NDS_GETLOGGEDINOBJECT_ORD   186
#define NDS_SETSERVERCONTEXT_ORD    187
#define NDS_GETTREENAME_ORD         188
#define NDS_GETDEFNAMECONTEXT_ORD   189

#define NDIR_TYPE_NONE          0
#define NDIR_TYPE_NDS           1
#define NDIR_TYPE_STREETTALK    2

// provide windows macros in NLM versions

#ifdef SYM_NLM
#define MAKEINTRESOURCE(n) ((LPCSTR)(n))
#endif

typedef struct tagNETDIRDATA {                  // network directory structure
    DWORD                   dwStructSize;       // Size of structure
    DWORD                   dwExtra;            // platform stuff can go here
    DWORD                   dwContext;          // directory context
    char                    szModuleName [NETMAXMODULENAME+1];
    DWORD                   iNetDirType;        // Type of network directory
    DWORD                   bTreeMode;          // server supports tree mode
    DWORD                   bUseFullNames;      // when enum net resources
    HINSTANCE               hInst;              // DLL Instance Handle
    } NETDIRDATA, FAR *LPNETDIRDATA;

typedef struct {
    DWORD   dwStructSize;               // [in]     Size of this structure
    LPCSTR  lpszContainerName;          // [in]     Container Name
    DWORD   dwUserData;                 // [in]     User data passed through
} NDIRCONTAINERSENUMSTRUCT, FAR * LPNDIRCONTAINERSENUMSTRUCT;

typedef LPNETDIRDATA FAR * LPLPNETDIRDATA;

typedef BOOL (CALLBACK * NETDIRECTORYCONTAINERSENUMPROC)(LPNDIRCONTAINERSENUMSTRUCT);

typedef BOOL (CALLBACK* NETDIRECTORYSERVERSENUMPROC)(LPNETSERVERSENUMSTRUCT);


                                        // Prototype for NetObjectsEnum callback
typedef BOOL (CALLBACK* NETDIRECTORYOBJECTSENUMPROC)(LPNETOBJECTSENUMSTRUCT);

                                        // Prototype for NetUsersEnum callback
typedef BOOL (CALLBACK* NETDIRECTORYUSERSENUMPROC)(LPNETUSERSENUMSTRUCT);

                                        // Prototype for NetGroupsImInEnum callback
                                        // and NetGroupsEnum callback
typedef BOOL (CALLBACK* NETDIRECTORYGROUPSENUMPROC)(LPNETGROUPSENUMSTRUCT);


#ifndef _SYMND2_H
#include "symnd2.h"
#endif

BOOL SYM_EXPORT WINAPI NetDirectoryInit (
    LPLPNETDIRDATA lplpNetDir               // [in/out]  network directory data
);

int SYM_EXPORT WINAPI NetDirectoryGetContext (
    LPNETDIRDATA    lpNetDir,               // [in/out]  network directory data
    LPSTR           lpszContext,            // [out,size_is(wLen)] current name context
    WORD            wLen                    // [in]
);

BOOL SYM_EXPORT WINAPI NetDirectoryIsRoot (
    LPNETDIRDATA lpNetDir                   // [in/out]  network directory data
);

int SYM_EXPORT WINAPI NetDirectoryTreeUp (
    LPNETDIRDATA lpNetDir                   // [in/out]  network directory data
);

int SYM_EXPORT WINAPI NetDirectoryTreeDown (
    LPNETDIRDATA    lpNetDir,               // [in/out]  network directory data
    LPSTR           lpszContainerName       // [in] container name
);

int SYM_EXPORT WINAPI NetDirectoryContainersEnum (
    LPNETDIRDATA    lpNetDir,           // [in/out]  network directory data
    DWORD           dwUserData,         // [in] Userdata to return in callback
    NETDIRECTORYCONTAINERSENUMPROC    lpfnNetDirContainersEnumProc // [in] Enum callback fcn ptr
);

int SYM_EXPORT WINAPI NetDirectoryServersEnum (
    LPNETDIRDATA    lpNetDir,           // [in/out]  network directory data
    DWORD           dwUserData,         // [in] Userdata to return in callback
    NETDIRECTORYSERVERSENUMPROC  lpfnNetServersEnumProc// [in] Enum callback fcn ptr
);

int SYM_EXPORT WINAPI NetDirectoryUsersEnum (
    LPNETDIRDATA    lpNetDir,           // [in/out]  network directory data
    DWORD           dwUserData,         // [in] Userdata to return in callback
    NETDIRECTORYUSERSENUMPROC    lpfnNetUsersEnumProc// [in] Enum callback fcn ptr
);

int SYM_EXPORT WINAPI NetDirectoryGroupsEnum (
    LPNETDIRDATA    lpNetDir,           // [in/out]  network directory data
    DWORD           dwUserData,         // [in] Userdata to return in callback
    NETDIRECTORYGROUPSENUMPROC   lpfnNetGroupsEnumProc// [in] Enum callback fcn ptr
);

int SYM_EXPORT WINAPI NetDirectoryGetLoggedInObject (
    LPNETDIRDATA    lpNetDir,           // [in/out] network directory data
    LPSTR           lpszName,           // [out,size_is(nNameSize)] name of object
    WORD            nNameSize           // [in] max name size
);

BOOL SYM_EXPORT WINAPI NetDirectoryIsSupported (
    LPNETDIRDATA    lpNetDir,           // [in/out]  network directory data
    LPSTR           lpszServer,         // [out,size_is(wLen)] current name context
    WORD            wLen                // [in]
);

BOOL SYM_EXPORT WINAPI NetDirectoryGetTreeName (
    LPNETDIRDATA    lpNetDir,           // [in/out]  network directory data
    LPSTR           lpszTreeName,       // [out,size_is(wLen)] current name context
    WORD            wLen                // [in]
);

BOOL SYM_EXPORT WINAPI NetDirectorySetServerContext (
    LPNETDIRDATA    lpNetDir,           // [in/out]  network directory data
    LPSTR           lpszServer          // [in] current name context
);

VOID SYM_EXPORT WINAPI NetDirectorySetTreeMode (
    LPNETDIRDATA    lpNetDir,      // [in/out]  network directory data
    BOOL            bTree          // [in]  tree mode on
);

BOOL SYM_EXPORT WINAPI NetDirectoryGetTreeMode (
    LPNETDIRDATA    lpNetDir       // [in/out]  network directory data
);

VOID SYM_EXPORT WINAPI NetDirectorySetFullNames (
    LPNETDIRDATA    lpNetDir,      // [in/out]  network directory data
    BOOL            bFullNames     // [in]  use long names
);

BOOL SYM_EXPORT WINAPI NetDirectoryGetFullNames (
    LPNETDIRDATA    lpNetDir       // [in/out]  network directory data
);

VOID SYM_EXPORT WINAPI NetDirectoryUninit (
    LPNETDIRDATA lpNetDir          // [in/out]  network directory data
);

VOID SYM_EXPORT WINAPI NetDirectoryGetDefNameContext (
    LPNETDIRDATA    lpNetDir,           // [in/out]  network directory data
    LPSTR           lpszContext,        // [out,size_is(wLen)]  server name
    WORD            wLen                // [in]      buffer len
);

VOID SYM_EXPORT WINAPI NetDirectoryTest (
    LPSTR           lpszTest,           // [out,size_is(wLen)] test name
    WORD            wLen                // [in] len
);


#ifdef __cplusplus
} // extern "C"
#endif

//------------------------------------------------------------------------
// Define inlines to original SYMNET API that doesn't require the first
// parameter.
//
// Passing NULL for the first parameter using the base configuration
// of symnet, the first parameter is only needed for special instance
// usage of symnet dll.
//------------------------------------------------------------------------

#ifdef __cplusplus  //{

inline WORD NetRegister (
    WORD        wNetType                // [in]     Net Type (NULL is default)
)
{
    return ::NetRegister (NULL, wNetType);
}

inline WORD NetUnRegister (VOID)
{
    return ::NetUnRegister (NULL);
}

inline WORD NetVolEnum (
    LPCSTR      lpszServerName,         // [in] Server name to look up
    DWORD       dwUserData,             // [in] Userdata to return in callback
    NETVOLENUMPROC lpfnNetVolEnumProc   // [in] Callback to enum vols to
)
{
    return ::NetVolEnum (NULL, lpszServerName, dwUserData, lpfnNetVolEnumProc);
}

#ifdef SYM_WIN
inline WORD NetVolEnum32 (
    LPCSTR      lpszServerName,         // [in] Server name to look up
    DWORD       dwUserData,             // [in] Userdata to return in callback
    LPDWORD     lpdwObjectID,           // [in/out] Seed value for bindery search
    DWORD       dwStartVolumeNumber,
    LPNETVOLENUM32ARRAY lpVolArray              // [out] Buffer to hold user name structs
)
{
    return ::NetVolEnum32 (NULL, lpszServerName, dwUserData, lpdwObjectID, dwStartVolumeNumber, lpVolArray);
}
#endif

                                        // SYMNET DOS/WINDOWS function prototypes

inline WORD  NetGetNetType (VOID)
{
    return ::NetGetNetType (NULL);
}

inline WORD NetSetNetType (
    WORD        wNetType                // [in]     Net Type (NULL is default)
)
{
    return ::NetSetNetType (NULL, wNetType);
}

inline WORD NetGetErrorString (
    WORD        wErrorCode,             // [in]     Error code
    LPSTR       lpszBuffer,             // [out]    Error string buffer to fill
    WORD        wBufSize                // [in]     Maximum buffer size
)
{
    return ::NetGetErrorString (NULL, wErrorCode, lpszBuffer, wBufSize);
}

                                        // Prototype for NetGetServerDateAndTime
inline WORD  NetGetServerDateAndTime (
    LPNETDATETIME   lpdt                // [out]    Date/Time structure filled
)
{
    return ::NetGetServerDateAndTime (NULL, lpdt);
}

inline WORD NetGetCurrentUserName (
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszName,                   // [out]    User Name buffer
    WORD    wBufSize                    // [in]     Size of username buffer
)
{
    return ::NetGetCurrentUserName (NULL, lpszServer, lpszName, wBufSize);
}

#ifdef SYM_WIN
inline WORD NetGetCurrentUserName32 (
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszName,                   // [out]    User Name buffer
    DWORD   dwBufSize                   // [in]     Size of username buffer
)
{
    return ::NetGetCurrentUserName32 (NULL, lpszServer, lpszName, dwBufSize);
}
#endif

inline WORD NetAddConnection (
    LPSTR   lpszNetPath,                // [in]     Network Path to use
    LPSTR   lpszPassword,               // [in]     Password to use
    LPSTR   lpszLocal                   // [in]     Local device to map
)
{
    return ::NetAddConnection (NULL, lpszNetPath, lpszPassword, lpszLocal);
}

inline WORD NetGetConnection (
    LPSTR   lpszLocal,                  // [in]     Local device
    LPSTR   lpszNetPath,                // [out]    Net Path found
    LPWORD  lpwSize                     // [in/out] Max size of buffer and actual
)
{
    return ::NetGetConnection (NULL, lpszLocal, lpszNetPath, lpwSize);
}

inline WORD NetCancelConnection (
    LPSTR   lpszLocal,                  // [in]     Local device to cancel
    SHORTBOOL  bForce                      // [in]     Force cancel
)
{
    return ::NetCancelConnection (NULL, lpszLocal, bForce);
}

inline WORD NetGetLocalDevice (
    LPSTR   lpszNetPath,                // [in]     Net Path
    LPSTR   lpszLocal                   // [out]    Local device found
)
{
    return ::NetGetLocalDevice (NULL, lpszNetPath, lpszLocal);
}

inline WORD  NetGetAvailableDrive (
    LPSTR   lpszLocal                   // [out]    Local Device available
)
{
    return ::NetGetAvailableDrive (NULL, lpszLocal);
}

inline WORD  NetParseNetPath (
    LPSTR   lpszPath,                   // [in]     Path to parse
    LPSTR   lpszNetPath,                // [out]    Parsed path
    WORD    wSize                       // [in]     Size of buffer
)
{
    return ::NetParseNetPath (NULL, lpszPath, lpszNetPath, wSize);
}

inline WORD  NetDrivePathFromNetPath (
    LPSTR   lpszNetPath,                // [in]     Network Path use
    LPSTR   lpszDrivePath,              // [out]    New Path
    WORD    wSize                       // [in]     Buffer Size
)
{
    return ::NetDrivePathFromNetPath (NULL, lpszNetPath, lpszDrivePath, wSize);
}


inline WORD  NetServerFromPath (
    LPCSTR      lpszPath,               // [in]     Path to use
    LPSTR       lpszServer,             // [out]    Server Name
    WORD        wBufSize                // [in]     Size of buffer for server name
)
{
    return ::NetServerFromPath (NULL, lpszPath, lpszServer, wBufSize);
}

inline WORD  NetGetWorkstationName (
    LPSTR   lpszName,                   // [out]    Workstation Name Buffer
    WORD    wBufSize                    // [in]     Maximum size of buffer
)
{
    return ::NetGetWorkstationName (NULL, lpszName, wBufSize);
}

#ifdef SYM_WIN
inline WORD  NetGetWorkstationName32 (
    LPSTR   lpszName,                   // [out]    Workstation Name Buffer
    DWORD   dwBufSize                   // [in]     Maximum size of buffer
)
{
    return ::NetGetWorkstationName32 (NULL, lpszName, dwBufSize);
}
#endif

inline WORD  NetGetNICAddress (
    LPNETNICADDRESS     lpnic           // [out]    Workstation NIC Address
)
{
    return ::NetGetNICAddress (NULL, lpnic);
}

#pragma optimize( "", off )

inline VOID  NetFormatNICAddress (
    LPNETNICADDRESS     lpnic,          // [in]     Workstation NIC Address
    SHORTBOOL           bIncludeNet,    // [in]     Include network number flag
    LPSTR               lpszAddress,    // [out]    Address of formed string
    WORD                wBufSize        // [in]     Buffer size for formatted text
)
{
    ::NetFormatNICAddress (NULL, lpnic, bIncludeNet, lpszAddress, wBufSize);
}

inline VOID  NetFormatNICAddressEx (
    LPNETNICADDRESS     lpnic,          // [in]     Workstation NIC Address
    SHORTBOOL           bIncludeNet,    // [in]     Include network number flag
    LPSTR               lpszAddress,    // [out]    Address of formed string
    WORD                wBufSize,       // [in]     Buffer size for formatted text
    WORD                wNetAddrFormat  // [in]     Format of NIC address (0 = Ethernet, 1=Token Ring)
)
{
    ::NetFormatNICAddressEx (NULL, lpnic, bIncludeNet, lpszAddress, wBufSize, wNetAddrFormat);
}

#pragma optimize( "", on )

inline WORD  NetIsMemberOfGroup (
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszGroup,                  // [in]     Group Name
    LPSTR   lpszUserName                // [in]     User Name in question
)
{
    return ::NetIsMemberOfGroup (NULL, lpszServer, lpszGroup, lpszUserName);
}

inline WORD  NetGetConnectionInfo (
    LPSTR           lpszServer,         // [in]     Server
    LPNETCONNECTION lpConnection        // [out]    Connection Struct
)
{
    return ::NetGetConnectionInfo (NULL, lpszServer, lpConnection);
}

inline WORD  NetVerifyConnection (
    LPSTR           lpszServer,         // [in]     Server
    LPNETCONNECTION lpConnection        // [in]     Connection Struct
)
{
    return ::NetVerifyConnection (NULL, lpszServer, lpConnection);
}

inline WORD  NetVerifyNICAddress (
    LPNETNICADDRESS     lpnic           // [in]     Workstation NIC Address
)
{
    return ::NetVerifyNICAddress (NULL, lpnic);
}

inline WORD  NetFunctionSupported (
    WORD        wFunctionID             // [in]     Function I.D.
)
{
    return ::NetFunctionSupported (NULL, wFunctionID);
}

inline WORD  NetGroupsImInEnum (
    LPCSTR              lpszServerName,         // [in] Server name to look up
    LPCSTR              lpszUserName,           // [in] UserName to lookup
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETGROUPSENUMPROC   lpfnNetGroupsEnumProc   // [in] Callback
)
{
    return ::NetGroupsImInEnum (NULL, lpszServerName, lpszUserName, dwUserData, lpfnNetGroupsEnumProc);
}

#ifdef SYM_WIN
inline WORD  NetGroupsImInEnum32 (
    LPCSTR              lpszServerName,         // [in] Server name to look up
    LPCSTR              lpszUserName,           // [in] UserName to lookup
    DWORD               dwUserData,             // [in] Userdata to return in callback
    LPNETGROUPSIMINENUM32ARRAY  lpGroupArray      // [out] Buffer to hold server name structs
)
{
    return ::NetGroupsImInEnum32 (NULL, lpszServerName, lpszUserName, dwUserData, lpGroupArray);
}
#endif

inline WORD  NetGroupsEnum (
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETGROUPSENUMPROC   lpfnNetGroupsEnumProc
)
{
    return ::NetGroupsEnum (NULL, lpszServerName, dwUserData, lpfnNetGroupsEnumProc);
}

#ifdef SYM_WIN
inline WORD NetGroupsEnum32 (
    LPCSTR      lpszServerName,         // [in] Server name to look up
    DWORD       dwUserData,             // [in] Userdata to return in callback
    LPDWORD     lpdwObjectID,           // [in/out] Seed value for bindery search
    LPNETGROUPSENUM32ARRAY  lpGroupArray      // [out] Buffer to hold server name structs
)
{
    return ::NetGroupsEnum32 (NULL, lpszServerName, dwUserData, lpdwObjectID, lpGroupArray);
}
#endif


inline WORD  NetMessageSend (
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszDestUser,               // [in]     User to send to
    LPSTR   lpszMessage,                // [in]     Message to send (NETMAXMESSAGE length)
    LPSTR   lpszFromUser                // [in] (optional)  From UserName
)
{
    return ::NetMessageSend (NULL, lpszServer, lpszDestUser, lpszMessage, lpszFromUser);
}

inline WORD  NetMessageSendGroup (
    LPSTR   lpszServer,                 // [in]     Server to query
    LPSTR   lpszGroupUser,              // [in]     Group to send to
    LPSTR   lpszMessage,                // [in]     Message to send (NETMAXMESSAGE length)
    LPSTR   lpszFromUser                // [in] (optional)  From UserName
)
{
    return ::NetMessageSendGroup (NULL, lpszServer, lpszGroupUser, lpszMessage, lpszFromUser);
}

inline WORD  NetMessageSendToConsole (
    LPSTR   lpszServer,                 // [in]     Server console to use
    LPSTR   lpszMessage,                // [in]     Message to send (NETMAXMESSAGE length)
    LPSTR   lpszFromUser                // [in] (optional)  From UserName
)
{
    return ::NetMessageSendToConsole (lpszServer, lpszMessage, lpszFromUser);
}

inline WORD  NetUsersEnum (
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETUSERSENUMPROC    lpfnNetUsersEnumProc
)
{
    return ::NetUsersEnum (NULL, lpszServerName, dwUserData, lpfnNetUsersEnumProc);
}

#ifdef SYM_WIN
inline WORD NetUsersEnum32 (
    LPCSTR      lpszServerName,         // [in] Server name to look up
    DWORD       dwUserData,             // [in] Userdata to return in callback
    LPDWORD     lpdwObjectID,           // [in/out] Seed value for bindery search
    LPNETUSERSENUM32ARRAY  lpUserArray      // [out] Buffer to hold server name structs
)
{
    return ::NetUsersEnum32 (NULL, lpszServerName, dwUserData, lpdwObjectID, lpUserArray);
}
#endif

inline WORD  NetServersEnum (
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETSERVERSENUMPROC  lpfnNetServersEnumProc
)
{
    return ::NetServersEnum (NULL, lpszServerName, dwUserData, lpfnNetServersEnumProc);
}

#ifdef SYM_WIN
inline WORD NetServersEnum32 (
    LPCSTR      lpszServerName,         // [in] Server name to look up
    DWORD       dwUserData,             // [in] Userdata to return in callback
    LPDWORD     lpdwObjectID,           // [in/out] Seed value for bindery search
    LPNETSERVERSENUM32ARRAY  lpServerArray      // [out] Buffer to hold server name structs
)
{
    return ::NetServersEnum32 (NULL, lpszServerName, dwUserData, lpdwObjectID, lpServerArray);
}
#endif

inline WORD  NetObjectsEnum (
    LPCSTR              lpszServerName,         // [in] Server name to use
    DWORD               dwUserData,             // [in] Userdata to return in callback
    DWORD               dwSearchType,           // [in] Enum this type
    NETOBJECTSENUMPROC  lpfnNetObjectsEnumProc
)
{
    return ::NetObjectsEnum (NULL, lpszServerName, dwUserData, dwSearchType, lpfnNetObjectsEnumProc);
}

#ifdef SYM_WIN
inline WORD NetObjectsEnum32 (
    LPCSTR      lpszServerName,         // [in] Server name to look up
    DWORD       dwUserData,             // [in] Userdata to return in callback
    DWORD       dwSearchType,           // [in] Enum this type
    LPDWORD     lpdwObjectID,           // [in/out] Seed value for bindery search
    LPNETOBJECTSENUM32ARRAY  lpObjectArray      // [out] Buffer to hold server name structs
)
{
    return ::NetObjectsEnum32 (NULL, lpszServerName, dwUserData, dwSearchType, lpdwObjectID, lpObjectArray);
}
#endif


inline WORD  NetDomainsEnum (
    DWORD               dwUserData,             // [in] Userdata to return in callback
    NETSERVERSENUMPROC  lpfnNetServersEnumProc
)
{
    return ::NetDomainsEnum (NULL, dwUserData, lpfnNetServersEnumProc);
}

#ifdef SYM_WIN
inline WORD  NetDomainsEnum32 (
    DWORD               dwUserData,             // [in] Userdata to return in callback
    DWORD               dwStart,                // [in/out] Seed value for bindery search
    LPNETSERVERSENUM32ARRAY  lpDomainArray      // [out] Buffer to hold server name structs
)
{
    return ::NetDomainsEnum32 (NULL, dwUserData, dwStart, lpDomainArray);
}
#endif

inline WORD  NetGetPrimaryDomain (
    LPSTR               lpszDomain,             // [out]    Enumeration domain name found
    WORD                wBufSize                // [in]     Buffer Size
)
{
    return ::NetGetPrimaryDomain (NULL, lpszDomain, wBufSize);
}

#ifdef SYM_WIN
inline WORD  NetGetPrimaryDomain32 (
    LPSTR               lpszDomain,             // [out]    Enumeration domain name found
    DWORD               dwBufSize               // [in]     Buffer Size
)
{
    return ::NetGetPrimaryDomain32 (NULL, lpszDomain, dwBufSize);
}
#endif

inline WORD  NetSetPrimaryDomain (
    LPSTR               lpszNewDomain,          // [in]     New enumeration domain
    SHORTBOOL           bFromServerName         // [in]     if TRUE, lpszNewDomain == a server name
)
{
    return ::NetSetPrimaryDomain (NULL, lpszNewDomain, bFromServerName);
}

#ifdef SYM_WIN
inline WORD  NetSetPrimaryDomain32 (
    LPSTR               lpszNewDomain,          // [in]     New enumeration domain
    DWORD               dwFromServerName        // [in]     if TRUE, lpszNewDomain == a server name
)
{
    return ::NetSetPrimaryDomain32 (NULL, lpszNewDomain, dwFromServerName);
}
#endif

inline WORD NetDomainSetFullNames (
    SHORTBOOL           bUseFullNames           // [in] Insert domain in object names?
)
{
    return ::NetDomainSetFullNames (NULL, bUseFullNames);
}

#ifdef SYM_WIN
inline WORD NetDomainSetFullNames32 (
    DWORD               dwUseFullNames          // [in] Insert domain in object names?
)
{
    return ::NetDomainSetFullNames32 (NULL, dwUseFullNames);
}
#endif

inline WORD  NetGetPrimaryServer (
    LPSTR               lpszServer,             // [out]    Server name found
    WORD                wBufSize                // [in]     Buffer Size
)
{
    return ::NetGetPrimaryServer (NULL, lpszServer, wBufSize);
}

#ifdef SYM_WIN
inline WORD  NetGetPrimaryServer32 (
    LPSTR               lpszServer,             // [out]    Server name found
    DWORD               dwBufSize               // [in]     Buffer Size
)
{
    return ::NetGetPrimaryServer32 (NULL, lpszServer, dwBufSize);
}
#endif

inline WORD  NetPathFromDrivePath (
    LPSTR   lpszDrivePath,              // [in]     Drive Path
    LPSTR   lpszNetPath,                // [out]    Network Path
    WORD    wSize                       // [in]     Buffer Size
)
{
    return ::NetPathFromDrivePath (NULL, lpszDrivePath, lpszNetPath, wSize);
}

inline WORD  NetPathNormalize (
    LPSTR   lpszNetPath,                // [in]     Original Net Path
    LPSTR   lpszNormalPath,             // [out]    Network Path
    WORD    wSize                       // [in]     Buffer Size
)
{
    return ::NetPathNormalize (NULL, lpszNetPath, lpszNormalPath, wSize);
}

inline WORD  NetVolGetInfo (
    LPCSTR              lpszServer,     // [in]     Server name to look up
    LPCSTR              lpszVolume,     // [in]     Name of volume to inquire about
    LPNETVOLENUMSTRUCT  lpvolinfo       // [out]    Volume Info Struct to fill
)
{
    return ::NetVolGetInfo (NULL, lpszServer, lpszVolume, lpvolinfo);
}

inline WORD  NetGetEveryoneGroup (
    LPSTR               lpszGroup,      // [out]    Group Name
    WORD                wBufSize        // [in]     Size of buffer
)
{
    return ::NetGetEveryoneGroup (NULL, lpszGroup, wBufSize);
}

#ifdef SYM_WIN
inline WORD  NetGetEveryoneGroup32 (
    LPSTR               lpszGroup,      // [out]    Group Name
    DWORD               dwBufSize       // [in]     Size of buffer
)
{
    return ::NetGetEveryoneGroup32 (NULL, lpszGroup, dwBufSize);
}
#endif

inline WORD  NetGetServerAndVolume (
    BYTE    byDrive,                    // [in]     Drive Letter 'A'-'Z'
    LPSTR   lpszServer,                 // [out]    Server Name
    WORD    wServerSize,                // [in]     Size of Servername buffer
    LPSTR   lpszVol,                    // [out]    Volume Name
    WORD    wVolSize                    // [in]     Size of Volumename buffer
)
{
    return ::NetGetServerAndVolume (NULL, byDrive, lpszServer, wServerSize, lpszVol, wVolSize);
}

inline WORD  NetSubstEnvironment (
    LPSTR   lpszData                    // [in]      Data to examine
)
{
    return ::NetSubstEnvironment (NULL, lpszData);
}

inline VOID  NetSetConfigDir(
    LPSTR   lpszDir                     // [in]     Directory
)
{
    ::NetSetConfigDir (NULL,lpszDir);
}

inline VOID  NetGetConfigDir(
    LPSTR   lpszDir,                    // [out]    Directory
    WORD    wBufSize                    // [in]     Buffer Size
)
{
    ::NetGetConfigDir(NULL,lpszDir,wBufSize);
}

inline WORD  NetSetConfigNIC(
    LPNETNICADDRESS lpnic               // [in]     Workstation NIC Address
)
{
    return ::NetSetConfigNIC(NULL,lpnic);
}

inline WORD  NetGetConfigNIC(
    LPNETNICADDRESS lpnic               // [out]    Workstation NIC Address
)
{
    return ::NetGetConfigNIC(NULL,lpnic);
}


#endif //} __cplusplus


#endif

