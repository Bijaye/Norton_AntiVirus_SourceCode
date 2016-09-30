#ifndef INCL_TKEXTRNS
#define INCL_TKEXTRNS

/*----------------------------------------------------------------------*/
/*  These next variables are located in the other modules.              */
/*----------------------------------------------------------------------*/

extern  Byte   *TK_HELP_SCREEN [ ];
extern  Byte    TK_LOADED[];
extern  Byte    TK_LOW[];
extern  Byte    TK_HIGH[];
extern  Byte    TK_NO_HIGH_LOAD[];

extern  Byte    TK_NOT_INSTALLED[];
extern  Byte    TK_INSTALLED[];

extern  Byte    TK_ENABLED[];
extern  Byte    TK_DISABLED[];

extern  Byte    TK_CANT_REMOVE[];
extern  Byte    TK_REMOVED_OK[];

extern  Byte    DRIVE_STATUS_OFF_STR[];
extern  Byte    DRIVE_STATUS_ON_STR[];  
extern  Byte    STATUS_STR[];          
extern  Byte   *DRIVE_STRS[];          
extern  Byte    MONITORED_STR[];       
extern  Byte   *CHG_STRS[];
extern  Byte    PENDING_STR[];
extern  Byte    LOCKED_STR[];

#ifndef INCL_MAIN
#include    <main.h>
#endif

extern  SwitchRec switchRemove;
extern  SwitchRec switchOff;
extern  SwitchRec switchStatus;
extern  SwitchRec switchOn;
extern  SwitchRec SW_LOADLOW;           /*      This one's in NORTON.LIB                */

#ifdef  MAIN
extern  MenuBarRec  menuBar;
extern  Byte far TKEnvironment;
extern  FP       ResidentEnd;        /*  A Byte in the Segment after
                                        the end of res. code (really
                                        just a label)                   */
extern 	IntChain far NewInt21;
extern  IntChain far NewInt2F;
extern 	TSRCtrl  far resCTRL;
#endif

extern  char            mainHelp[];
extern  Word            helpTopics[];

#ifdef  EVENT
extern  DialogRec       mainMenuDlg;
extern  ButtonsRec      mainMenuButtons;
extern  OutputFieldRec  mainDriveStatus;
extern  Word            helpTopics[];

extern  OutputFieldRec  mainDriveStatus;
extern  DialogRec       driveDlg;
extern  DialogRec       noDrivesSelectedDlg;
extern  ButtonsRec      onOffCancelButtons;
#endif

/*----------------------------------------------------------------------*/
/*  These next variables are provided in the Norton Library.            */

extern  Word      dosVersion;
extern  Word      PSPSegment;
extern  Byte      TSR_ID_TREEMON[];     /*     = "DM";                  */
extern  Byte      _UMBServerType;

#endif
