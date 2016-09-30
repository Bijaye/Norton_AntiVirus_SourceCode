// Copyright 1993 Symantec Corporation
//========================================================================
//
// $Header:   S:/NAVSDK/VCS/quakever.h_v   1.1   04 Dec 1997 16:25:06   DALLEE  $
//
// Description:
//  Quake Version Stamp
//
//------------------------------------------------------------------------
// $Log:   S:/NAVSDK/VCS/quakever.h_v  $
// 
//    Rev 1.1   04 Dec 1997 16:25:06   DALLEE
// Updated QMODULE_SYMKRNL_xx defines to match new naming scheme for
// our dlls. 
// 
//    Rev 1.0   02 Dec 1997 14:25:24   DALLEE
// Initial revision.
//------------------------------------------------------------------------
// From NEWVIR-NAVOEM quakever.h_v
// With changes - DLL names are now NxxSDK.DLL and NxxSDKR.DLL.
//                VXD is now NAVSDK.VXD (no version suffix).
//
//    Rev 1.2   30 Sep 1997 12:28:12   TCASHIN
// Make sure the #defines are right for the repair-enabled DLL.
//------------------------------------------------------------------------

#ifndef _QUAKEVER_H
#define _QUAKEVER_H

#define QUAKE_VERSION_MAJOR     21
#define QUAKE_VERSION_MINOR     0

#define QUAKE_BUILD_NUMBER      43
#define QUAKE_BUILD_STRING      "Quake43"

//////////////////////////////////////////////////////////////////
// Define CORE_PRFIX and DEBUG_SUFFIX
//////////////////////////////////////////////////////////////////

#ifdef SYM_WIN16
    #define PRE_CHAR1 "W"
#elif defined(SYM_WIN32)
    #ifdef SYM_VXD
        #define PRE_CHAR1 "X"
    #elif defined(SYM_NTK)
        #define PRE_CHAR1 "K"
    #elif defined(_X86_)
        #define PRE_CHAR1 "I"
    #elif defined(_M_ALPHA)
        #define PRE_CHAR1 "A"
    #elif defined(_M_PPC)
        #define PRE_CHAR1 "P"
    #elif defined(_M_MRX000)
        #define PRE_CHAR1 "M"
    #endif
#endif


#ifdef _DEBUG
#define DEBUG_SUFFIX "D"
#define PRE_CHAR2 "D"
#else
#define DEBUG_SUFFIX
#define PRE_CHAR2 "R"
#endif

#ifdef _UNICODE
#define PRE_CHAR3 "U"
#else
#define PRE_CHAR3 "A"
#endif

#define CORE_PREFIX PRE_CHAR1 PRE_CHAR2 PRE_CHAR3

#ifdef Q_VER

#ifdef SYM_OS2
#   define OSTWO                   "O"
#endif

//////////////////////////////////////////////////////////////////
// 32-bit DLLs
//////////////////////////////////////////////////////////////////

// NOTE! THE FOLLOWING MODULES ARE <EXEMPT> FROM THE BRANCH SUFFIX:
//
//       SYMEVENT
//       SYMSTAT
//       SYMTRIAL
//       SYMREG
//       WEBTRANS

// SymLean, SymTrial, SymReg, WebTrans use a naming convention based on the new platform scheme
#define QMODULE_SYMLEAN            CORE_PREFIX "LEAN"
#define QMODULE_SYMTRIAL           CORE_PREFIX "TRIAL"
#define QMODULE_SYMREG             CORE_PREFIX "REG"
#define QMODULE_WEBTRANS           CORE_PREFIX "WEBTR"

#define QMODULE_DBF_32             "S32DBF" Q_VER
#define QMODULE_DOMAIN_32          "S32DMAN" Q_VER DEBUG_SUFFIX
#define QMODULE_DOMMEM_32          "S32DMEM" Q_VER DEBUG_SUFFIX
#define QMODULE_DOSLNK_32          "S32DLNK" Q_VER DEBUG_SUFFIX
#define QMODULE_DOSDEL_32          "S32DDEL" Q_VER DEBUG_SUFFIX
#define QMODULE_DRAGDROP_32        "S32DGDP" Q_VER
#define QMODULE_DRVMAP_32          "S32DMAP" Q_VER
#define QMODULE_EBSUMMIT_32        "S32EBSU" Q_VER
#define QMODULE_EBBASIC_32         "S32EBBA" Q_VER
#define QMODULE_EBEXT_32           "S32EBBX" Q_VER
#define QMODULE_EBUTIL_32          "S32EBUT" Q_VER
#define QMODULE_EBWBT_32           "S32EBWB" Q_VER
#define QMODULE_FLST_32            "S32FLST" Q_VER DEBUG_SUFFIX
#define QMODULE_FSCTL_32           "S32FSCT" Q_VER DEBUG_SUFFIX
#define QMODULE_FSUTIL_32          "S32FSUT" Q_VER DEBUG_SUFFIX
#define QMODULE_LANMAN2_32         "S32LM2" Q_VER
#define QMODULE_LSAPI_32           "S32LS" Q_VER
#define QMODULE_NDS_32             "S32NDS" Q_VER
#define QMODULE_DS_32              "S32DS" Q_VER
#define QMODULE_NETWARE3_32        "S32NW3" Q_VER
#define QMODULE_NETWARE4_32        "S32NW4" Q_VER
#define QMODULE_NMACRO_32          "S32MCRO" Q_VER
//This where custom Quake DLLs will get used instead of the
//normal Quake DLLs.  Is there are better way to do this?
#ifdef QCUST

#ifdef NAVSDKR
#define QMODULE_SYMKRNL_32         "NAVSDR32"
#else
#define QMODULE_SYMKRNL_32         "NAVSDK32"
#endif

#define QMODULE_SYMKRNL_VXD        "NAVSDK"

#define QMODULE_SYMKRNLTHUNK_32    "TKNV32" Q_VER
#define QMODULE_NPT_32             "S32NAV" Q_VER
#define QMODULE_SYMGUI_32          "S32NAV" Q_VER
#define QMODULE_SYMUTIL_32         "S32NAV" Q_VER
#else
#define QMODULE_SYMKRNL_32         "S32KRNL" Q_VER
#define QMODULE_SYMKRNL_VXD        "SYMKRNL" Q_VER
#define QMODULE_SYMKRNLTHUNK_32    "TKKE32" Q_VER
#define QMODULE_NPT_32             "S32NPT" Q_VER
#define QMODULE_SYMGUI_32          "S32GUI" Q_VER
#define QMODULE_SYMUTIL_32         "S32UTIL" Q_VER
#endif
#define QMODULE_PANE_32            "S32PANE" Q_VER DEBUG_SUFFIX
#define QMODULE_RECDLL_32          "S32REC" Q_VER
#define QMODULE_SIMULATE_32        "S32NSM" Q_VER
#define QMODULE_SYMALOG_32         "S32ALOG" Q_VER
#define QMODULE_SYMCHART_32        "S32CHRT" Q_VER
#define QMODULE_SYMCOM_32          "S32COM" Q_VER
#define QMODULE_SYMCONST_32        "S32CNST" Q_VER
#define QMODULE_SYMCRC_32          "S32CRC" Q_VER
#define QMODULE_SYMDB_32           "S32DB" Q_VER
#define QMODULE_SYMDBG_32          "S32DBG" Q_VER
#define QMODULE_SYMEVAL_32         "S32EVAL" Q_VER
#define QMODULE_SYMEVNT_32         "S32EVNT1"
// More Custom Quake DLL stuff
#ifdef QCUST
#define QMODULE_SYMFAT_32          "S32RSQ" Q_VER
#define QMODULE_SYMFMT_32          "S32RSQ" Q_VER
#define QMODULE_SYMSYS_32          "S32RSQ" Q_VER
#else
#define QMODULE_SYMFAT_32          "S32FAT" Q_VER
#define QMODULE_SYMFMT_32          "S32FMT" Q_VER
#define QMODULE_SYMSYS_32          "S32SYS" Q_VER
#endif
#define QMODULE_SYMIPX_32          "S32IPX" Q_VER
#define QMODULE_SYMMAIL_32         "S32MAIL" Q_VER
#define QMODULE_SYMNDS_32          "S32NDS" Q_VER
#define QMODULE_SYMDS_32           "S32DS" Q_VER
#define QMODULE_SYMNET_32          "S32NET" Q_VER
#define QMODULE_SYMPIPE_32         "S32PIPE" Q_VER
#define QMODULE_SYMPUB_32          "S32PUB" Q_VER
#define QMODULE_SYMRUN_32          "S32RUN" Q_VER
#define QMODULE_SYMSERVE_32        "S32SERV" Q_VER
#define QMODULE_SYMSPX_32          "S32SPX" Q_VER

#define QMODULE_SYMTIME_32         "S32TIME" Q_VER
#define QMODULE_SYMUNAS_32         "S32UNAS" Q_VER
#define QMODULE_SYMUNZP_32         "S32UNZP" Q_VER
#define QMODULE_SYMVID_32          "S32VID" Q_VER
//#define QMODULE_NEFDB_32           "N32DB" Q_VER      // Moved to NEF30
//#define QMODULE_NEFSCAL_32         "N32SCAL" Q_VER    // Moved to NEF30
//#define QMODULE_NEFHELP_32         "N32HELP" Q_VER    // Moved to NEF30
//#define QMODULE_SYMSCAL_32         "S32SCAL" Q_VER    // Moved to NEF30
#define QMODULE_SYMVWR_32          "S32VWR" Q_VER
#define QMODULE_SYMSS_32           "S32SS" Q_VER
#define QMODULE_SYMDOC_32          "S32DOC" Q_VER
#define QMODULE_SYMWEXT_32         "S32WEXT" Q_VER
#define QMODULE_SYMWNET_32         "S32WNET" Q_VER
#define QMODULE_SYMZIP_32          "S32ZIP" Q_VER
#define QMODULE_VINES4_32          "S32BV4" Q_VER
#define QMODULE_VIRLIST_32         "S32VLST" Q_VER
#define QMODULE_VIRSCAN_32         "S32VSCN" Q_VER
#define QMODULE_WBTAPI_32          "S32WBT" Q_VER
#define QMODULE_SYMNEW_32          "S32NEW" Q_VER DEBUG_SUFFIX
#define QMODULE_SYMDEBUG_32        "S32DEBUG"
#define QMODULE_SYMSTAT_32         "S32STAT"
#define QMODULE_SYMXPC_32          "S32XPC" Q_VER
#define QMODULE_EDTPGE_32          "S32EDPG" Q_VER
#define QMODULE_EDTAPI_32          "S32EDAP" Q_VER

// Reset the macros to use the custom DLL instead of normal DLLs.
// QUAKE_CUSTOM (which is defined along with QUAKE_CUSTOM_NEF) also
// defines QCUST. So we must first reset the some DLL names which
// are over writeen by NAV back to original names.
#ifdef  QCNEF
  #ifdef  QCUST
    #undef  QMODULE_SYMKRNLTHUNK_32
    #define QMODULE_SYMKRNLTHUNK_32    "TKKNF32" Q_VER
    #undef  QMODULE_SYMKRNL_VXD
    #define QMODULE_SYMKRNL_VXD        "SYMKRNL" Q_VER
    #undef  QMODULE_SYMFAT_32
    #define QMODULE_SYMFAT_32          "S32FAT" Q_VER
    #undef  QMODULE_SYMFMT_32
    #define QMODULE_SYMFMT_32          "S32FMT" Q_VER
    #undef  QMODULE_SYMSYS_32
    #define QMODULE_SYMSYS_32          "S32SYS" Q_VER
  #endif //QCUST

  // Redefine the macros to Q32NEF custom DLL
  #undef  QMODULE_SYMKRNL_32
  #define QMODULE_SYMKRNL_32         "N32NEF" Q_VER
  #undef  QMODULE_SYMGUI_32
  #define QMODULE_SYMGUI_32          "N32NEF" Q_VER
  #undef  QMODULE_SYMUTIL_32
  #define QMODULE_SYMUTIL_32         "N32NEF" Q_VER
  #undef  QMODULE_SYMDB_32
  #define QMODULE_SYMDB_32           "N32NEF" Q_VER
  #undef  QMODULE_NPT_32
  #define QMODULE_NPT_32             "N32NEF" Q_VER

  #undef  QMODULE_SYMCRC_32
  #define QMODULE_SYMCRC_32          "N32CRC" Q_VER

  #undef  QMODULE_SYMNET_32
  #define QMODULE_SYMNET_32          "N32NET" Q_VER

  #undef  QMODULE_SYMXPC_32
  #define QMODULE_SYMXPC_32          "N32XPC" Q_VER

  #undef  QMODULE_SYMVWR_32
  #define QMODULE_SYMVWR_32          "N32VWR" Q_VER
  #undef  QMODULE_SYMDOC_32
  #define QMODULE_SYMDOC_32          "N32DOC" Q_VER
  #undef  QMODULE_SYMSS_32
  #define QMODULE_SYMSS_32           "N32SS" Q_VER

  #undef  QMODULE_DRAGDROP_32
  #define QMODULE_DRAGDROP_32        "N32EDAP" Q_VER
  #undef  QMODULE_EDTPGE_32
  #define QMODULE_EDTPGE_32          "N32EDAP" Q_VER
  #undef  QMODULE_EDTAPI_32
  #define QMODULE_EDTAPI_32          "N32EDAP" Q_VER

#endif //QCNEF

//////////////////////////////////////////////////////////////////
// 16-bit DLLs
//////////////////////////////////////////////////////////////////

#define QMODULE_DBF_16             "DBF" Q_VER
#define QMODULE_DOMAIN_16          "DOMAIN" Q_VER DEBUG_SUFFIX
#define QMODULE_DOMMEM_16          "DOMMEM" Q_VER DEBUG_SUFFIX
#define QMODULE_DOSLNK_16          "DOSLNK" Q_VER DEBUG_SUFFIX
#define QMODULE_DOSDEL_16          "DOSDEL" Q_VER DEBUG_SUFFIX
#define QMODULE_DRAGDROP_16        "DRAGDRP" Q_VER
#define QMODULE_DRVMAP_16          "DRVMAP" Q_VER
#define QMODULE_EBSUMMIT_16        "EBSUMMT" Q_VER
#define QMODULE_EBBASIC_16         "EBBASIC" Q_VER
#define QMODULE_EBEXT_16           "EBBXT" Q_VER
#define QMODULE_EBUTIL_16          "EBUTIL" Q_VER
#define QMODULE_EBWBT_16           "EBWBT" Q_VER
#define QMODULE_FLST_16            "FLST" Q_VER DEBUG_SUFFIX
#define QMODULE_FSCTL_16           "FSCTL" Q_VER DEBUG_SUFFIX
#define QMODULE_FSUTIL_16          "FSUTIL" Q_VER DEBUG_SUFFIX
#define QMODULE_LANMAN2_16         "SYMLM2" Q_VER
#define QMODULE_LSAPI_16           "LSAPI" Q_VER
#define QMODULE_NDS_16             "SYMNDS" Q_VER
#define QMODULE_DS_16              "SYMDS" Q_VER
#define QMODULE_NETWARE3_16        "SYMNW3" Q_VER
#define QMODULE_NETWARE4_16        "SYMNW4" Q_VER
#define QMODULE_NMACRO_16          "NMACRO" Q_VER
#define QMODULE_NPT_16             "NPT" Q_VER
#define QMODULE_PANE_16            "PANE" Q_VER DEBUG_SUFFIX
#define QMODULE_RECDLL_16          "RECDLL" Q_VER
#define QMODULE_SIMULATE_16        "SYMNSM" Q_VER
#define QMODULE_SYMALOG_16         "SYMALOG" Q_VER
#define QMODULE_SYMCHART_16        "SYMCHRT" Q_VER
#define QMODULE_SYMCOM_16          "SYMCOM" Q_VER
#define QMODULE_SYMCONST_16        "SYMCNST" Q_VER
#define QMODULE_SYMCRC_16          "SYMCRC" Q_VER
#define QMODULE_SYMDB_16           "SYMDB" Q_VER
#define QMODULE_SYMEVAL_16         "SYMEVAL" Q_VER
#define QMODULE_SYMEVNT_16         "SYMEVNT1"
#ifdef QCUST
#define QMODULE_SYMFAT_16          "SYMRSQ" Q_VER
#define QMODULE_SYMFMT_16          "SYMRSQ" Q_VER
#define QMODULE_SYMSYS_16          "SYMRSQ" Q_VER
#else
#define QMODULE_SYMFAT_16          "SYMFAT" Q_VER
#define QMODULE_SYMFMT_16          "SYMFMT" Q_VER
#define QMODULE_SYMSYS_16          "SYMSYS" Q_VER
#endif
#define QMODULE_SYMGUI_16          "SYMGUI" Q_VER
#define QMODULE_SYMIPX_16          "SYMIPX" Q_VER
#ifdef QCUST
#define QMODULE_SYMKRNLTHUNK_16    "TKNV16" Q_VER

#ifdef NAVSDKR
#define QMODULE_SYMKRNL_16         "NAVSDR"
#else
#define QMODULE_SYMKRNL_16         "NAVSDK"
#endif

#else
#define QMODULE_SYMKRNLTHUNK_16    "TKKE16" Q_VER
#define QMODULE_SYMKRNL_16         "SYMKRNL" Q_VER
#endif
#define QMODULE_SYMMAIL_16         "SYMMAIL" Q_VER
#define QMODULE_SYMNDS_16          "SYMNDS" Q_VER
#define QMODULE_SYMDS_16           "SYMDS" Q_VER
#define QMODULE_SYMNET_16          "SYMNET" Q_VER
#define QMODULE_SYMPIPE_16         "SYMPIPE" Q_VER
#define QMODULE_SYMPUB_16          "SYMPUB" Q_VER
#define QMODULE_SYMRUN_16          "SYMRUN" Q_VER
#define QMODULE_SYMSERVE_16        "SYMSERV" Q_VER
#define QMODULE_SYMSPX_16          "SYMSPX" Q_VER
#define QMODULE_SYMTIME_16         "SYMTIME" Q_VER
#define QMODULE_SYMUNAS_16         "SYMUNAS" Q_VER
#define QMODULE_SYMUNZP_16         "SYMUNZP" Q_VER
#define QMODULE_SYMUTIL_16         "SYMUTIL" Q_VER
#define QMODULE_SYMVID_16          "SYMVID" Q_VER
//#define QMODULE_NEFDB_16           "NEFDB" Q_VER      // Moved to NEF30
//#define QMODULE_NEFSCAL_16         "NEFSCAL" Q_VER    // Moved to NEF30
//#define QMODULE_NEFHELP_16         "NEFHELP" Q_VER    // Moved to NEF30
//#define QMODULE_SYMSCAL_16         "SYMSCAL" Q_VER    // Moved to NEF30
#define QMODULE_SYMVWR_16          "SYMVWR" Q_VER
#define QMODULE_SYMSS_16           "SYMSS" Q_VER
#define QMODULE_SYMDOC_16          "SYMDOC" Q_VER
#define QMODULE_SYMWEXT_16         "SYMWEXT" Q_VER
#define QMODULE_SYMWNET_16         "SYMWNET" Q_VER
#define QMODULE_SYMXPC_16          "SYMXPC" Q_VER
#define QMODULE_SYMZIP_16          "SYMZIP" Q_VER
#define QMODULE_VINES4_16          "SYMBV4" Q_VER
#define QMODULE_VIRLIST_16         "VIRLIST" Q_VER
#define QMODULE_VIRSCAN_16         "VIRSCAN" Q_VER
#define QMODULE_WBTAPI_16          "WBTAPI" Q_VER
#define QMODULE_SYMNEW_16          "SYMNEW" Q_VER DEBUG_SUFFIX
#define QMODULE_SYMDEBUG_16        "S16DEBUG"

// QUAKE_CUSTOM (which is defined along with QUAKE_CUSTOM_NEF) also
// defines QCUST. So we must first reset the some DLL names which
// are over writeen by NAV back to original names.
#ifdef  QCNEF
    #undef  QMODULE_SYMFAT_16
    #define QMODULE_SYMFAT_16          "SYMFAT" Q_VER

    #undef  QMODULE_SYMFMT_16
    #define QMODULE_SYMFMT_16          "SYMFMT" Q_VER

    #undef  QMODULE_SYMSYS_16
    #define QMODULE_SYMSYS_16          "SYMSYS" Q_VER

    #undef  QMODULE_SYMKRNLTHUNK_16
    #define QMODULE_SYMKRNLTHUNK_16    "TKKNF16" Q_VER

    #undef  QMODULE_SYMKRNL_16
    #define QMODULE_SYMKRNL_16         "N16KRNL" Q_VER

    #undef  QMODULE_SYMNET_16
    #define QMODULE_SYMNET_16          "N16NET" Q_VER

    #undef  QMODULE_SYMXPC_16
    #define QMODULE_SYMXPC_16          "N16XPC" Q_VER

    #undef  QMODULE_VINES4_16
    #define QMODULE_VINES4_16          "N16BV4" Q_VER

    #undef  QMODULE_SYMVWR_16
    #define QMODULE_SYMVWR_16          "N16VWR" Q_VER

    #undef  QMODULE_SYMDB_16
    #define QMODULE_SYMDB_16           "N16DB" Q_VER

    #undef  QMODULE_SYMUTIL_16
    #define QMODULE_SYMUTIL_16         "N16UTIL" Q_VER

    #undef  QMODULE_LANMAN2_16
    #define QMODULE_LANMAN2_16         "N16LM2" Q_VER

    #undef  QMODULE_NDS_16
    #define QMODULE_NDS_16             "N16NDS" Q_VER

    #undef  QMODULE_NETWARE3_16
    #define QMODULE_NETWARE3_16        "N16NW3" Q_VER

    #undef  QMODULE_NETWARE4_16
    #define QMODULE_NETWARE4_16        "N16NW4" Q_VER

#endif //QCNEF

//////////////////////////////////////////////////////////////////
// Define the type we are using
//////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32)

#   define QMODULE_DBF                  QMODULE_DBF_32
#   define QMODULE_DOMAIN               QMODULE_DOMAIN_32
#   define QMODULE_DOMMEM               QMODULE_DOMMEM_32
#   define QMODULE_DOSLNK               QMODULE_DOSLNK_32
#   define QMODULE_DOSDEL               QMODULE_DOSDEL_32
#   define QMODULE_DRAGDROP             QMODULE_DRAGDROP_32
#   define QMODULE_DRVMAP               QMODULE_DRVMAP_32
#   define QMODULE_EBSUMMIT             QMODULE_EBSUMMIT_32
#   define QMODULE_EBBASIC              QMODULE_EBBASIC_32
#   define QMODULE_EBEXT                QMODULE_EBEXT_32
#   define QMODULE_EBUTIL               QMODULE_EBUTIL_32
#   define QMODULE_EBWBT                QMODULE_EBWBT_32
#   define QMODULE_FLST                 QMODULE_FLST_32
#   define QMODULE_FSCTL                QMODULE_FSCTL_32
#   define QMODULE_FSUTIL               QMODULE_FSUTIL_32
#   define QMODULE_LANMAN2              QMODULE_LANMAN2_32
#   define QMODULE_LSAPI                QMODULE_LSAPI_32
#   define QMODULE_NDS                  QMODULE_NDS_32
#   define QMODULE_DS                   QMODULE_DS_32
#   define QMODULE_NETWARE3             QMODULE_NETWARE3_32
#   define QMODULE_NETWARE4             QMODULE_NETWARE4_32
#   define QMODULE_NMACRO               QMODULE_NMACRO_32
#   define QMODULE_NPT                  QMODULE_NPT_32
#   define QMODULE_PANE                 QMODULE_PANE_32
#   define QMODULE_RECDLL               QMODULE_RECDLL_32
#   define QMODULE_SIMULATE             QMODULE_SIMULATE_32
#   define QMODULE_SYMALOG              QMODULE_SYMALOG_32
#   define QMODULE_SYMCHART             QMODULE_SYMCHART_32
#   define QMODULE_SYMCOM               QMODULE_SYMCOM_32
#   define QMODULE_SYMCONST             QMODULE_SYMCONST_32
#   define QMODULE_SYMCRC               QMODULE_SYMCRC_32
#   define QMODULE_SYMDB                QMODULE_SYMDB_32
#   define QMODULE_SYMDBG               QMODULE_SYMDBG_32
#   define QMODULE_SYMEVAL              QMODULE_SYMEVAL_32
#   define QMODULE_SYMEVNT              QMODULE_SYMEVNT_32
#   define QMODULE_SYMFAT               QMODULE_SYMFAT_32
#   define QMODULE_SYMFMT               QMODULE_SYMFMT_32
#   define QMODULE_SYMGUI               QMODULE_SYMGUI_32
#   define QMODULE_SYMIPX               QMODULE_SYMIPX_32
#   define QMODULE_SYMKRNL              QMODULE_SYMKRNL_32
#   define QMODULE_SYMKRNLTHUNK         QMODULE_SYMKRNLTHUNK_32
#   define QMODULE_SYMMAIL              QMODULE_SYMMAIL_32
#   define QMODULE_SYMNDS               QMODULE_SYMNDS_32
#   define QMODULE_SYMDS                QMODULE_SYMDS_32
#   define QMODULE_SYMNET               QMODULE_SYMNET_32
#   define QMODULE_SYMPIPE              QMODULE_SYMPIPE_32
#   define QMODULE_SYMPUB               QMODULE_SYMPUB_32
#   define QMODULE_SYMRUN               QMODULE_SYMRUN_32
#   define QMODULE_SYMSERVE             QMODULE_SYMSERVE_32
#   define QMODULE_SYMSPX               QMODULE_SYMSPX_32
#   define QMODULE_SYMSYS               QMODULE_SYMSYS_32
#   define QMODULE_SYMTIME              QMODULE_SYMTIME_32
#   define QMODULE_SYMUNAS              QMODULE_SYMUNAS_32
#   define QMODULE_SYMUNZP              QMODULE_SYMUNZP_32
#   define QMODULE_SYMUTIL              QMODULE_SYMUTIL_32
#   define QMODULE_SYMVID               QMODULE_SYMVID_32
//#   define QMODULE_NEFDB                QMODULE_NEFDB_32   // Moved to NEF30
//#   define QMODULE_NEFSCAL              QMODULE_NEFSCAL_32 // Moved to NEF30
//#   define QMODULE_NEFHELP              QMODULE_NEFHELP_32 // Moved to NEF30
//#   define QMODULE_SYMSCAL              QMODULE_SYMSCAL_32 // Moved to NEF30
#   define QMODULE_SYMVWR               QMODULE_SYMVWR_32
#   define QMODULE_SYMSS                QMODULE_SYMSS_32
#   define QMODULE_SYMDOC               QMODULE_SYMDOC_32
#   define QMODULE_SYMWEXT              QMODULE_SYMWEXT_32
#   define QMODULE_SYMWNET              QMODULE_SYMWNET_32
#   define QMODULE_SYMZIP               QMODULE_SYMZIP_32
#   define QMODULE_VINES4               QMODULE_VINES4_32
#   define QMODULE_VIRLIST              QMODULE_VIRLIST_32
#   define QMODULE_VIRSCAN              QMODULE_VIRSCAN_32
#   define QMODULE_WBTAPI               QMODULE_WBTAPI_32
#   define QMODULE_SYMNEW               QMODULE_SYMNEW_32
#   define QMODULE_SYMDEBUG             QMODULE_SYMDEBUG_32
#   define QMODULE_SYMSTAT              QMODULE_SYMSTAT_32
#   define QMODULE_SYMXPC               QMODULE_SYMXPC_32
#   define QMODULE_EDTPGE               QMODULE_EDTPGE_32
#   define QMODULE_EDTAPI               QMODULE_EDTAPI_32

#else

#   define QMODULE_DBF                  QMODULE_DBF_16
#   define QMODULE_DOMAIN               QMODULE_DOMAIN_16
#   define QMODULE_DOMMEM               QMODULE_DOMMEM_16
#   define QMODULE_DOSLNK               QMODULE_DOSLNK_16
#   define QMODULE_DOSDEL               QMODULE_DOSDEL_16
#   define QMODULE_DRAGDROP             QMODULE_DRAGDROP_16
#   define QMODULE_DRVMAP               QMODULE_DRVMAP_16
#   define QMODULE_EBSUMMIT             QMODULE_EBSUMMIT_16
#   define QMODULE_EBBASIC              QMODULE_EBBASIC_16
#   define QMODULE_EBEXT                QMODULE_EBEXT_16
#   define QMODULE_EBUTIL               QMODULE_EBUTIL_16
#   define QMODULE_EBWBT                QMODULE_EBWBT_16
#   define QMODULE_FLST                 QMODULE_FLST_16
#   define QMODULE_FSCTL                QMODULE_FSCTL_16
#   define QMODULE_FSUTIL               QMODULE_FSUTIL_16
#ifdef SYM_OS2
#   define QMODULE_LANMAN2              QMODULE_LANMAN2_16 OSTWO
#else
#   define QMODULE_LANMAN2              QMODULE_LANMAN2_16
#endif

#   define QMODULE_LSAPI                QMODULE_LSAPI_16
#   define QMODULE_NDS                  QMODULE_NDS_16
#   define QMODULE_DS                   QMODULE_DS_16

#ifdef SYM_OS2
#   define QMODULE_NETWARE3             QMODULE_NETWARE3_16 OSTWO
#else
#   define QMODULE_NETWARE3             QMODULE_NETWARE3_16
#   define QMODULE_NETWARE4             QMODULE_NETWARE4_16
#endif
#   define QMODULE_NMACRO               QMODULE_NMACRO_16
#   define QMODULE_NPT                  QMODULE_NPT_16
#   define QMODULE_PANE                 QMODULE_PANE_16
#   define QMODULE_RECDLL               QMODULE_RECDLL_16
#   define QMODULE_SIMULATE             QMODULE_SIMULATE_16
#   define QMODULE_SYMALOG              QMODULE_SYMALOG_16
#   define QMODULE_SYMCHART             QMODULE_SYMCHART_16
#   define QMODULE_SYMCOM               QMODULE_SYMCOM_16
#   define QMODULE_SYMCONST             QMODULE_SYMCONST_16
#   define QMODULE_SYMCRC               QMODULE_SYMCRC_16
#   define QMODULE_SYMDB                QMODULE_SYMDB_16
#   define QMODULE_SYMEVAL              QMODULE_SYMEVAL_16
#   define QMODULE_SYMEVNT              QMODULE_SYMEVNT_16
#   define QMODULE_SYMFAT               QMODULE_SYMFAT_16
#   define QMODULE_SYMFMT               QMODULE_SYMFMT_16
#   define QMODULE_SYMGUI               QMODULE_SYMGUI_16
#   define QMODULE_SYMIPX               QMODULE_SYMIPX_16
#   define QMODULE_SYMKRNL              QMODULE_SYMKRNL_16
#   define QMODULE_SYMKRNLTHUNK         QMODULE_SYMKRNLTHUNK_16
#   define QMODULE_SYMMAIL              QMODULE_SYMMAIL_16
#   define QMODULE_SYMNDS               QMODULE_SYMNDS_16
#   define QMODULE_SYMDS                QMODULE_SYMDS_16
#   define QMODULE_SYMNET               QMODULE_SYMNET_16
#   define QMODULE_SYMPIPE              QMODULE_SYMPIPE_16
#   define QMODULE_SYMPUB               QMODULE_SYMPUB_16
#   define QMODULE_SYMRUN               QMODULE_SYMRUN_16
#   define QMODULE_SYMSERVE             QMODULE_SYMSERVE_16
#   define QMODULE_SYMSPX               QMODULE_SYMSPX_16
#   define QMODULE_SYMSYS               QMODULE_SYMSYS_16
#   define QMODULE_SYMTIME              QMODULE_SYMTIME_16
#   define QMODULE_SYMUNAS              QMODULE_SYMUNAS_16
#   define QMODULE_SYMUNZP              QMODULE_SYMUNZP_16
#   define QMODULE_SYMUTIL              QMODULE_SYMUTIL_16
#   define QMODULE_SYMVID               QMODULE_SYMVID_16
//#   define QMODULE_NEFDB                QMODULE_NEFDB_16   // No Longer in QUAKE
//#   define QMODULE_NEFSCAL              QMODULE_NEFSCAL_16 // No Longer in QUAKE
#   define QMODULE_NEFHELP              QMODULE_NEFHELP_16
#   define QMODULE_SYMVWR               QMODULE_SYMVWR_16
#   define QMODULE_SYMSS                QMODULE_SYMSS_16
#   define QMODULE_SYMDOC               QMODULE_SYMDOC_16
#   define QMODULE_SYMSCAL              QMODULE_SYMSCAL_16
#   define QMODULE_SYMWEXT              QMODULE_SYMWEXT_16
#   define QMODULE_SYMWNET              QMODULE_SYMWNET_16
#   define QMODULE_SYMXPC               QMODULE_SYMXPC_16
#   define QMODULE_SYMZIP               QMODULE_SYMZIP_16
#   define QMODULE_VINES4               QMODULE_VINES4_16
#   define QMODULE_VIRLIST              QMODULE_VIRLIST_16
#   define QMODULE_VIRSCAN              QMODULE_VIRSCAN_16
#   define QMODULE_WBTAPI               QMODULE_WBTAPI_16
#   define QMODULE_SYMNEW               QMODULE_SYMNEW_16
#   define QMODULE_SYMDEBUG             QMODULE_SYMDEBUG_16

#endif

#endif // Q_VER

#endif // _QUAKEVER_H

