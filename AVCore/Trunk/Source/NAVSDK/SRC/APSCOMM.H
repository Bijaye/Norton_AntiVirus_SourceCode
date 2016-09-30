/////////////////////////////////////////////////////////////////////
// APSCOMM.H
//
// This header file defines the control codes used to talk to
// the NAV Auto-Protect Service.
//

#if !defined (__APSCOMM_H)
#define __APSCOMM_H

#define PAD_SIZE (512)

/////////////////////////////////////////////////////////////////////
// Custom control codes to tell the service what to do...
// These are sent via the ServiceControlManager

#define  NAVAPSVC_RELOADCONFIG   (128)  // Range of commands is 128-255 only!
#define  NAVAPSVC_RELOADOPTIONS  (129)


/////////////////////////////////////////////////////////////////////
// Here are the control codes that will be written into the
// pipe to request NAVAPSVC to request action from the AP

#define  NAVAPSVC_GET_STATUS      (1)
#define  NAVAPSVC_AP_ENABLE       (2)
#define  NAVAPSVC_AP_DISABLE      (3)

#define  NAVAPSVC_AP_SCANFLOPPYA  (4)

#define  NAVAPSVC_EXEC_PROGRAM    (5)

/////////////////////////////////////////////////////////////////////
// Return codes for NAVAPSVC requests

#define  NAVAPSVC_ENABLED         (1)
#define  NAVAPSVC_DISABLED        (2)

#define  NAVAPSVC_ERROR           (3)

#define  NAVAPSVC_FLOPPYCLEAN     (4)
#define  NAVAPSVC_FLOPPYINFECTED  (5)

#define  NAVAPSVC_SUCCESS         (6)

/////////////////////////////////////////////////////////////////////
// Structure for our NAVAPSVC_EXEC_PROGRAM packet

typedef struct tagEXEDATA
{
   char szCmdLine [SYM_MAX_PATH];       // Program to run // NO UNICODE HERE!
   int  nCmdShow;                       // startup style
} EXEDATA, * PEXEDATA;

typedef struct tagCMDPACKET
{
   DWORD dwSize;                        // struct size
   DWORD dwCommand;                     // Command

   union
   {
      EXEDATA rExeData;                 // Program to run information
      BYTE fixed_union_size [PAD_SIZE]; // to keep our packets a contant size
   } rData;

} CMDPACKET, * PCMDPACKET;


#endif // if !defined (__APSCOMM_H)

