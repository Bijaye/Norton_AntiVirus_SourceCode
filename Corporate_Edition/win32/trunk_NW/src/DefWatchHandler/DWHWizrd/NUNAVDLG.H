// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVWORK/VCS/nunavdlg.h_v   1.13   17 Jun 1998 23:50:10   tcashin  $
//
// Main scan window class.
//
//************************************************************************
// $Log:   S:/NAVWORK/VCS/nunavdlg.h_v  $
// 
//    Rev 1.13   17 Jun 1998 23:50:10   tcashin
// Added an ID for the quarantine progress bar.
// 
//    Rev 1.12   07 Jun 1998 15:11:32   tcashin
// Added more control IDs.
// 
//    Rev 1.11   03 Jun 1998 18:38:30   tcashin
// Added a new define for the Forward Files dialog.
// 
//    Rev 1.10   19 May 1998 00:13:06   tcashin
// Added more control IDs.
// 
//    Rev 1.9   12 May 1998 13:38:20   tcashin
// Added defines for new controls for quarantined & deleted files.
// 
//    Rev 1.8   06 Apr 1998 15:24:10   DBuches
// Added resource IDs for animation sequences.
//
//    Rev 1.7   03 Apr 1998 12:02:18   tcashin
// Added new #defines for the quarantiine panel in the repair wizard.
//
//    Rev 1.6   09 Mar 1998 23:46:34   DALLEE
// Defines, etc. for new Repair Wizard.
//
//    Rev 1.5   18 Feb 1998 16:22:10   mdunn
// Ported NEC code from the ROSNEC area.
//
//    Rev 1.4   18 Jul 1997 19:46:50   GWESTER
// Added 256-color animations.
//
//    Rev 1.3   22 May 1997 13:42:32   TIVANOV
// new quarantine button ID
//
//    Rev 1.2   02 May 1997 12:13:06   TIVANOV
// new quarantine define
//
//    Rev 1.1   24 Apr 1997 11:01:00   JBRENNA
// Remove IDD_MEMSCANDLG_MEMONLY. NEC now scans floppy BRs.
//
//    Rev 1.0   06 Feb 1997 21:02:34   RFULLER
// Initial revision
//
//    Rev 1.7   27 Jan 1997 11:24:34   JBRENNA
// Add IDD_MEMSCANDLG_MEMONLY.
//
//    Rev 1.6   01 Nov 1996 12:27:00   JBRENNA
// Add ID_SR_STATUS.
//
//    Rev 1.5   30 Aug 1996 16:06:58   DALLEE
// Removed #ifdef NAVSCANW from internet download dialogs.
//
//    Rev 1.4   19 Aug 1996 14:43:14   GWESTER
// Removed #ifdef around ID_ICON_ADVOTEXT .
//
//    Rev 1.3   28 Jun 1996 10:58:32   GWESTER
// Ported in fixes / changes from Luigi Plus.
//
//    Rev 1.2   09 May 1996 17:59:48   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.1   05 Feb 1996 22:20:04   YVANG
// Removed #ifdef NAVSCAW around the block where IDI_MSG_VIRUSINFO is defined.
//
//    Rev 1.0   30 Jan 1996 15:55:54   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:58   BARRY
// Initial revision.
//
//    Rev 1.27   25 Jan 1996 19:05:12   GWESTER
// Added dialog components for download mode.
//
//    Rev 1.26   23 Jan 1996 11:33:02   YVANG
// For NAVSCANW: Added three new IDs, which are IDD_SCANSUMDLG_NT,
// IDD_SCANDLG_NT and IDD_MEMSCANDLG_NT.
//
//    Rev 1.25   21 Jan 1996 18:04:36   GWESTER
// Added dialog component IDs for Netscape downloader running under NT.
//
//    Rev 1.24   21 Jan 1996 13:01:58   GWESTER
// Added dialog IDs for the Netscape downloading dialogs.
//
//    Rev 1.23   14 Jan 1996 16:50:04   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.22   28 Nov 1995 10:23:36   BARRY
// Don't ifdef ID_DOALL_FILLBAR_BOX
//************************************************************************

#define NAV_SCANWINDOW_CLASS "ScanWindow"


/////////////////////////////////////////////////////////////////////////////
//
// Bitmap
//

#define IDB_CHECKMARK       100
#define IDB_CHECKMARK_MASK  101
#define IDB_NOWAYJOSE       110
#define IDB_NOWAYJOSE_MASK  111
#define IDB_NETSCAPELOGO         120
#define IDB_NETSCAPELOGO_MASK    121

#define IDB_WZ256_0             131
#define IDB_WZ256_1             132
#define IDB_WZ256_2             133
#define IDB_WZ256_3             134
#define IDB_WZ256_4             135
#define IDB_WZ256_5             136
#define IDB_WZ256_6             137
#define IDB_WZ256_7             138

#define IDB_WZ_16_0             140
#define IDB_WZ_16_1             141
#define IDB_WZ_16_2             142
#define IDB_WZ_16_3             143
#define IDB_WZ_16_4             144
#define IDB_WZ_16_5             145
#define IDB_WZ_16_6             146
#define IDB_WZ_16_7             147

#define IDB_SIRENS              150

// --------------------------------------------------------------------
//
// NOTE: This is the place to define NAVWORK Animation BITMAPS!
//
#define IDC_ANIMATION             9910  // proj level reserved animation control
#define IDC_ANIMATION_ALERT       9911

    // ID's 9912 through 9924 are available

                                        // True for both the 16- and 256-color version:
    #define SCAN_ANIMATION_NUM_FRAMES           3

    #define ID_NAVVIRFOUND_ANIM_BMP_1           9941
    #define ID_NAVVIRFOUND_ANIM_BMP_2           9942
    #define ID_NAVVIRFOUND_ANIM_BMP_3           9943
    #define ID_NAVVIRFOUND_ANIM_BMP_4           9944
    #define ID_NAVVIRFOUND_ANIM_BMP_5           9945
    #define ID_NAVVIRFOUND_ANIM_BMP_6           9946
    #define ID_NAVVIRFOUND_ANIM_BMP_7           9947
    #define ID_NAVVIRFOUND_ANIM_BMP_8           9948
    #define ID_NAVVIRFOUND_ANIM_BMP_9           9949
    #define ID_NAVVIRFOUND_ANIM_BMP_10          9950
    #define ID_NAVVIRFOUND_ANIM_BMP_11          9951
    #define ID_NAVVIRFOUND_ANIM_BMP_12          9952
    #define ID_NAVVIRFOUND_ANIM_BMP_13          9953
    #define ID_NAVVIRFOUND_ANIM_BMP_14          9954
    #define ID_NAVVIRFOUND_ANIM_BMP_15          9955
    #define ID_NAVVIRFOUND_ANIM_BMP_16          9956
    #define ID_NAVVIRFOUND_ANIM_BMP_17          9957
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_1      9958
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_2      9959
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_3      9960
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_4      9961
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_5      9962
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_6      9963
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_7      9964
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_8      9965
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_9      9966
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_10     9967
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_11     9968
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_12     9969
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_13     9970
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_14     9971
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_15     9972
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_16     9973
    #define ID_NAVVIRFOUND_ANIM_MASK_BMP_17     9974


    // New animation definitions
    #define ID_SCAN_ANIMATION256                9975
    #define ID_SCAN_ANIMATION16                 9976

/////////////////////////////////////////////////////////////////////////////
//
// Dialog boxes
//

// ******************************* //
// Memory and Boot Scan Dialog Box //
// ******************************* //

#define IDD_MEMSCANDLG              100
#define IDD_MEMSCANDLG_NT           101
#define IDD_MEMSCANDLG_NT_NEC       102
#define IDD_MEMSCANDLG_MEMANDBR     103

#define IDI_MS_FILLMEM      1
#define IDI_MS_FILLMBOOT    2
#define IDI_MS_FILLBOOT     3
#define IDI_MS_BOX          4
#define IDI_MS_BOX1         5
#define IDI_MS_BOX2         6

// ************************* //
// Critical error Dialog Box //
// ************************* //

#define IDD_CRITERRDLG    200
#define IDI_CR_MSG          102
#define IDI_CR_ALERT        103
#define IDI_CR_BOX          104


// **************************************** //
//          Virus Found Dialog Box          //
// **************************************** //

#define IDD_VIRUSDLG      300
#define IDI_V_MSG          11
#define IDI_V_ALERT        12
#define IDI_V_REPAIR       13
#define IDI_V_DELETE       14
#define IDI_V_EXCLUDE      15
#define IDI_V_INFO         16
#define IDI_V_STOP         17
#define IDI_V_CONTINUE     18
#define IDI_V_MSG_EX       19
#define IDI_V_BOX          20
#define IDI_V_QUARANTINE   21

// ********************************************************** //
// Virus Information Dialog box. This box is displayed at the //
// users request at the time a virus is found.                //
// The control IDs are the same as for IDD_SCANINFODLG        //
// ********************************************************** //

#define IDD_VIRUSINFODLG      400

// ********************************** //
// While Scan in Progress, Dialog Box //
// ********************************** //

#define IDD_SCANDLG         500
#define IDD_SCANDLG_NT      501

#define ID_SC_MEMSCAN       101
#define ID_SC_MEMINF        102
#define ID_SC_MEMCLN        103
#define ID_SC_MBSCAN        104
#define ID_SC_MBINF         105
#define ID_SC_MBCLN         106
#define ID_SC_BRSCAN        107
#define ID_SC_BRINF         108
#define ID_SC_BRCLN         109
#define ID_SC_FILSCAN       110
#define ID_SC_FILINF        111
#define ID_SC_FILCLN        112
#define ID_SC_WORKINGON     113
#define ID_SC_BOX1          114
#define ID_SC_BOX2          115
#define ID_SC_BOX3          116
#define ID_SC_FILQUAR       117
#define ID_SC_FILDEL        118

#define ID_SC_ICON           20
#define ID_SC_FILLBAR        21
#define ID_SC_DIRTXT         22
#define ID_SC_DIR            23
#define ID_SC_ITEMTXT        24
#define ID_SC_SCANNEDTXT     25
#define ID_SC_INFTXT         26
#define ID_SC_CLEANTXT       27
#define ID_SC_MEMTXT         28
#define ID_SC_MBRECTXT       29
#define ID_SC_BRTXT          30
#define ID_SC_FILESTXT       31
#define ID_SC_STOP           32
#define ID_SC_MBDEL          33
#define ID_SC_MBQUAR         34
#define ID_SC_BRDEL          35
#define ID_SC_BRQUAR         36


// **************************** //
// Show Scan Results Dialog Box //
// **************************** //

#define IDD_SCANRESDLG      600
#define ID_SR_ALERTFRAME     90         // Required so we can hide it
#define ID_SR_ALERT          91
#define ID_SR_LISTBOX       101
#define ID_SR_RAP           102
#define ID_SR_RAP2          103
#define ID_SR_DONE          104
#define ID_SR_REPAIR        105
#define ID_SR_DELETE        106
#define ID_SR_INOC          107
#define ID_SR_EXCL          108
#define ID_SR_INFO          109
#define ID_SR_REGISTER      110
#define ID_SR_BOX1          111
#define ID_SR_BOX2          112
#define ID_SR_QUARANTINE    113
#define ID_SR_VIRUSLISTBOX  114
#define ID_SR_STATUS        115
#define ID_SR_LISTVIRUSNAME 116


// ********************************************************************** //
// Details Dialog box. The control IDs are the same as for IDD_SCANRESDLG //
// ********************************************************************** //

#define IDD_DETAILS         700

// ************************************* //
// Information after scanning Dialog Box //
// ************************************* //

#define IDD_SCANINFODLG      800
#define IDD_VIRUSONLYINFODLG 801
#define IDI_VI_VNAMETXT      101
#define IDI_VI_VNAME         102
#define IDI_VI_ALIASTXT      103
#define IDI_VI_ALIAS         104
#define IDI_VI_INFECTSTXT    105
#define IDI_VI_INFECTS       106
#define IDI_VI_LIKETXT       107
#define IDI_VI_LIKE          108
#define IDI_VI_LENGTHTXT     109
#define IDI_VI_LENGTH        110
#define IDI_VI_TYPEGRP       111
#define IDI_VI_MEMRESTXT     112
#define IDI_VI_MEMRES        113
#define IDI_VI_STEALTHTXT    114
#define IDI_VI_SIZESTLTH     115
#define IDI_VI_SLFENCRTXT    116
#define IDI_VI_FULLSTLTH     117
#define IDI_VI_OVERWRTXT     118
#define IDI_VI_TRIGGER       119
#define IDI_VI_COMPTXT       210
#define IDI_VI_ENCRYPT       211
#define IDI_VI_PARASTXT      212
#define IDI_VI_POLYMORHP     213
#define IDI_VI_COMMGRP       214
#define IDI_VI_COMMENT       215
#define IDI_VI_PREV          216
#define IDI_VI_NEXT          217
#define IDI_VI_PRINT         310
#define IDI_VI_FNAMETXT      311
#define IDI_VI_FNAME         312
#define IDI_VI_STATUSTXT     313
#define IDI_VI_STATUS        314
#define IDI_VI_SHOWNEXT      (WM_USER + 1)


// *************************** //
// Scanning Summary Dialog Box //
// *************************** //

#define IDD_SCANSUMDLG        900
#define IDD_SCANSUMDLG_NT     901
#define ID_SS_SUMM_GRPBOX     101
#define ID_SS_ITEM_GRPBOX     201
#define ID_SS_SCANSUMTXT       10
#define ID_SS_SCANSUM          11
#define ID_SS_ITSCANTXT        12
#define ID_SS_ITSCAN           13
#define ID_SS_SCANTYPTXT       14
#define ID_SS_SCANTYP          15
#define ID_SS_INOCTXT          16
#define ID_SS_INOC             17
#define ID_SS_OTHERTXT         18
#define ID_SS_OTHER            19
#define ID_SS_SCANTIMTXT       20
#define ID_SS_SCANTIM          21
#define ID_SS_ITEMTXT          22
#define ID_SS_SCANNEDTXT       23
#define ID_SS_INFTXT           24
#define ID_SS_REPAIRTXT        25
#define ID_SS_MEMTXT           26
#define ID_SS_MEMSCAN          27
#define ID_SS_MEMINF           28
#define ID_SS_MEMCLN           29
#define ID_SS_MBRECTXT         30
#define ID_SS_MBSCAN           31
#define ID_SS_MBINF            32
#define ID_SS_MBCLN            33
#define ID_SS_BRTXT            34
#define ID_SS_BRSCAN           35
#define ID_SS_BRINF            36
#define ID_SS_BRCLN            37
#define ID_SS_FILESTXT         38
#define ID_SS_FILSCAN          39
#define ID_SS_FILINF           40
#define ID_SS_FILCLN           41
#define ID_SS_PRINT            42
#define ID_SS_DETAILS          43
#define ID_SS_BOX1             44
#define ID_SS_BOX2             45
#define ID_SS_QUARTXT          46
#define ID_SS_DELETETXT	       47
#define ID_SS_FILQUAR          48
#define ID_SS_FILDEL           49
#define ID_SS_CLEANTXT         50
#define ID_SS_MBDEL            51
#define ID_SS_MBQUAR           52
#define ID_SS_BRDEL            53
#define ID_SS_BRQUAR           54



// ************************ //
// Ask Questions Dialog Box //
// ************************ //

#define IDD_ASKQUESTION     1000
#define IDI_ASK_RAPFLD         102
#define IDI_ASK_ONLYONE        103
#define IDI_ASK_ALL            104
#define IDI_ASK_BOX            105


#define IDD_SYSINTEG          1100
#define IDI_SYSINTEG_TEXT      100


#define IDD_SYSINTEG_PROMPT   1200
#define IDI_SYSINTEG_ALERT     101
#define IDI_SYSINTEG_REPAIR    102
#define IDI_SYSINTEG_INOC      103
#define IDI_SYSINTEG_CONT      104
#define IDI_SYSINTEG_STOP      105
#define IDI_SYSINTEG_HELP      106
#define IDI_SYSINTEG_TEXT2     107
#define IDI_SYSINTEG_BOX       108


#define IDD_INOCFILECHANGED   1400
#define IDI_INOCHANGE_MSG      100
#define IDI_INOCHANGE_ALERT    101
#define IDI_INOCHANGE_REPAIR   102
#define IDI_INOCHANGE_DELETE   103
#define IDI_INOCHANGE_INOC     104
#define IDI_INOCHANGE_EXCLUDE  105
#define IDI_INOCHANGE_CONTINUE 106
#define IDI_INOCHANGE_STOP     107
#define IDI_INOCHANGE_HELP     108
#define IDI_INOCHANGE_INFO     109
#define IDI_INOCHANGE_BOX      110

// ********************* //
// Inoc Files Dialog Box //
// ********************* //

#define IDD_INOCFILE          1500
#define IDI_INOCFILE_MSG       100
#define IDI_INOCFILE_ALERT     101
#define IDI_INOCFILE_INOC      102
#define IDI_INOCFILE_EXCLUDE   103
#define IDI_INOCFILE_DELETE    104
#define IDI_INOCFILE_CONTINUE  105
#define IDI_INOCFILE_STOP      106
#define IDI_INOCFILE_HELP      107
#define IDI_INOCFILE_INFO      108
#define IDI_INOCFILE_BOX       109

#define IDD_DOALL               1600
#define ID_DOALL_ACTION          100
#define ID_DOALL_FILLBAR         101
#define ID_DOALL_FILE            102
#define ID_DOALL_BOX             103
#define ID_DOALL_FILLBAR_BOX     104

#define IDD_WRITELOG            1800
#define ID_WRITELOG_FILLBAR      100
#define ID_WRITELOG_BOX          101

#define IDD_FORWARDFILES        1900
#define ID_FORWARDFILES_FILLBAR  100

// ##################################################################### //


// ********************* //
// Icon Resource Defines //
// ********************* //
#define ID_ICON_NAV0                  1
#define ID_ICON_NAV1                  2
#define ID_ICON_NAV2                  3
#define ID_ICON_NAV3                  4
#define ID_ICON_NAV4                  5
#define ID_ICON_NAV5                  6
#define ID_ICON_NAV6                  7
#define ID_ICON_NAV7                  8
#define ID_ICON_NAV8                  9

#define ID_ICON_ADVOTEXT              50


// **************************************** //
// Netscape Downloading Process + Dialogs :
// **************************************** //

#define IDD_DOWNLOADVIRUSDLG      301
#define IDI_MSG_VIRUSREPAIR       200
#define IDI_MSG_VIRUSDELETE       201
#define IDI_MSG_VIRUSCONTINUE     202
#define IDI_MSG_VIRUSINFO         203

#define IDD_DOWNLOADVIRUSZIPDLG   2200
#define IDI_MSG_VIRUSZIPDELETE    200
#define IDI_MSG_VIRUSZIPCONTINUE  201
#define IDI_MSG_VIRUSZIPSTOP      202

#define IDD_DOWNLOADSAVEAS_95       2300
#define IDD_DOWNLOADSAVEINFECTED_95 2325
#define IDD_DOWNLOADSAVEAS_NT       2350
#define IDD_DOWNLOADSAVEINFECTED_NT 2375
#define IDI_CS_MSG_AREWEINFECTED  200
#define IDI_CS_MSG_ADVOTEXT       201
#define IDI_CS_MSG_AREWEINFECTED2 202
#define IDI_CS_MSG_AREWEINFECTED3 203
#define IDI_CS_MSG_PRODUCT          204
#define IDI_CS_MSG_CUSTOMVERSION    205
#define IDI_CS_STATIC_SEPARATOR   206
#define IDI_CS_NETSCAPELOGO       207
#define IDI_CS_STATIC_3DSEPARATOR   208
#define IDI_CS_FILENAME           1152
#define IDI_CS_DIRECTORY          1088
#define IDI_CS_FILEBOX            1120
#define IDI_CS_DIRECTORYBOX       1121
#define IDI_CS_FILETYPES          1136
#define IDI_CS_DRIVES             1137
#define IDI_CS_DIRECTORYLABEL     1138

#define ID_DLG_ICON1              210
#define ID_DLG_ICON2              211



// **************************************** //
// Repair Wizard Controls :
// **************************************** //

#define IDD_WIZPAGE1                    2400
#define IDD_WIZPAGE2                    2401
#define IDD_WIZPAGE3                    2402
#define IDD_WIZPAGE4                    2403
#define IDD_WIZPAGE5                    2404
#define IDD_WIZPAGE6                    2405
#define IDD_WIZPAGE7                    2406

#define IDC_ANIMATE                     1000
#define IDC_PICTURE                     1001
#define IDC_DOIT                        1002
#define IDC_LIST1                       1003
#define IDC_LIST_VIRUSES                1003
#define IDC_VIRUS_INFO_BUTTON           1004
#define IDC_BUTTON_VIRUS_INFO           1004
#define IDC_RADIO1                      1005
#define IDC_RADIO2                      1006
#define IDC_LISTVIEW                    1007
#define IDC_ADVICE_BUTTON               1010
#define IDC_MORE_INFO_BUTTON            1012
#define IDC_ITEMS_SCANNED_STATIC        1013
#define IDC_ITEMS_INFECTED_STATIC       1014
#define IDC_ITEMS_REPIRED_STATIC        1015
#define IDC_RADIO_AUTOMATIC             1017
#define IDC_RADIO_MANUAL                1018
#define IDC_ITEMS_REPAIRED_STATIC       1019
#define IDC_ITEMS_QUARANTINED_STATIC    1020
#define IDC_ITEMS_DELETED_STATIC        1021
#define IDC_FINISH_TEXT                 1022


// ***************************
// Confirm Delete (All) Dialog
// ***************************

#define IDD_CONFIRM_DELETE          2500
#define IDC_CD_ALL                   100
#define IDC_CD_NO                    101
#define IDC_CD_FILE                  102


