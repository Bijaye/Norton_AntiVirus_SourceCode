
///////////////////////////////////////////////////////////////////////////////
//
// File: NAVInstMIF.h
//
///////////////////////////////////////////////////////////////////////////////



#ifndef NAV_INST_MIF_HEADER
#define NAV_INST_MIF_HEADER


// These are indices into the g_szMIFStatements string table
#define  IDX_MANUFACTURER  14
#define  IDX_PRODUCT       23
#define  IDX_VERSION       32
#define  IDX_USER          41
#define  IDX_COMPANY       43
#define  IDX_DATETIME      52
#define  IDX_INSTSTAT      66
#define  IDX_INSTSTATDESC  75
#define  IDX_LAST          79


// The MIF file created during an SMS Install/Uninstall
#define  SMS_MIF_FILENAME       "NAVWNT.MIF"


// array that holds the MIF file strings
extern  char    g_szMIFStatements[][SI_MAXSTR];


#endif  // NAV_INST_MIF_HEADER