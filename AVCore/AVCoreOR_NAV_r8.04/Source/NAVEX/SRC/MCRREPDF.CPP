//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/mcrrepdf.cpv   1.102   08 Jan 1997 20:22:56   DANG  $
//
// Description:
//      Contains WordDocument macro repair definitions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/mcrrepdf.cpv  $
// 
//    Rev 1.102   08 Jan 1997 20:22:56   DANG
// Pulled NPad.K since NPad.J detects it.  New engine will have new def
// 
//    Rev 1.101   08 Jan 1997 18:03:48   DANG
// Changed Bandung.G to NPad.K
// 
//    Rev 1.100   08 Jan 1997 16:47:06   DANG
// Added Bandung.G 0x1ED1.  Repair is just to remove AUTOOPEN.
// 
//    Rev 1.99   03 Jan 1997 18:18:20   AOONWAL
// Added repair for WM.Npad.J
// 
//    Rev 1.98   03 Jan 1997 12:38:38   RELNITI
// Add WM.Wazzu.X repair, VID = 0x1e16, Customer #10496
// 
//    Rev 1.97   26 Dec 1996 14:57:22   AOONWAL
// Modified during JAN97 build
// 
//    Rev JAN97  19 Dec 1996 20:26:34   CFORMUL
// Added Npad.H
// 
// 
//    Rev 1.96   13 Dec 1996 17:34:52   AOONWAL
// Added repair for WM.Appder
// 
//    Rev 1.95   12 Dec 1996 17:36:16   CRENERT
// Commented in a repair name (or 3).
// 
// 
//    Rev 1.94   12 Dec 1996 16:01:32   CRENERT
// Uncommented Johnny.
// 
// 
//    Rev 1.93   12 Dec 1996 13:10:16   CFORMUL
// Added WM.Wazzu.Y
// 
// 
//    Rev 1.92   10 Dec 1996 15:58:10   CFORMUL
// Added MDMA.C
// 
// 
//    Rev 1.91   09 Dec 1996 18:37:02   CFORMUL
// Added Nuclear.E
// 
// 
//    Rev 1.90   06 Dec 1996 19:37:14   RELNITI
// removing WM.Johnny temporarily
// 
// 
//    Rev 1.89   06 Dec 1996 19:24:12   RELNITI
// add repair for WM.Johnny (0x1e0b)
// 
//    Rev 1.88   05 Dec 1996 15:19:06   JWILBER
// Added repair for WM.Divina.C, 1e0e.
//
//    Rev 1.87   05 Dec 1996 14:10:46   CFORMUL
// Added Npad.E
//
//    Rev 1.86   02 Dec 1996 19:10:36   AOONWAL
// Added repair for WM.Sutra
//
//    Rev 1.85   02 Dec 1996 17:30:06   AOONWAL
// Modified repair to include WM.Concept.T infections in norml.dot
//
//    Rev 1.84   02 Dec 1996 17:23:38   AOONWAL
// Modified repair for WM.Concept.T to include Normal.dot infection
//
//    Rev 1.83   02 Dec 1996 16:41:56   AOONWAL
// modified to remove syntax error
//
//    Rev 1.82   02 Dec 1996 16:29:32   AOONWAL
// Added repair for 0x1e06
//
//    Rev 1.81   02 Dec 1996 13:45:06   AOONWAL
// DEC96 update
//
//    Rev 1.83   25 Nov 1996 19:48:44   AOONWAL
// Added repair for "WM.ALien.B"
//
//    Rev 1.82   22 Nov 1996 15:49:58   JWILBER
// Added repair for Twno.C, 1fff.
//
//    Rev 1.81   20 Nov 1996 11:43:02   AOONWAL
// No change.
//
//    Rev 1.80   19 Nov 1996 13:09:00   CFORMUL
// Added WM.Npad.C
//
//    Rev 1.79   15 Nov 1996 23:39:04   JWILBER
// Added repair for WM.AntiConcept.
//
//    Rev 1.78   14 Nov 1996 18:50:14   AOONWAL
// Added repair for Wm.Twno.A:Tw
//
//    Rev 1.77   14 Nov 1996 17:44:18   JWILBER
// Added repairs for Stryx in Normal.dot
//
//    Rev 1.76   14 Nov 1996 11:58:28   CFORMUL
// Added Lunch.B
//
//    Rev 1.75   13 Nov 1996 02:23:00   JWILBER
// Added repair for Look.C, Weather.B, Theatre, and Twno.C & B.
//
//    Rev 1.74   12 Nov 1996 13:40:58   JWILBER
// Added repair for Stryx, 1fef.
//
//    Rev 1.73   07 Nov 1996 19:25:04   JWILBER
// Added repairs for Nuclear.A.Damaged and Npad.
//
//    Rev 1.72   05 Nov 1996 16:32:08   JWILBER
// Added repair for BigDaddy, 1fea.
//
//    Rev 1.71   05 Nov 1996 13:01:18   JWILBER
// Added repair for Helper, 1fe8.
//
//    Rev 1.70   04 Nov 1996 19:38:22   CFORMUL
// Added WM.Switches
//
//    Rev 1.69   04 Nov 1996 14:42:06   CFORMUL
// Added WM.Rapi
//
//    Rev 1.68   31 Oct 1996 17:12:26   CFORMUL
// Added WM.Alien
//
//    Rev 1.67   31 Oct 1996 15:10:30   CFORMUL
// Added WM.Colors.C
//
//    Rev 1.66   30 Oct 1996 17:08:44   CFORMUL
// Added WM.Concept.K
//
//    Rev 1.65   29 Oct 1996 14:35:32   CFORMUL
// Added WM.Wazzu.F
//
//    Rev 1.64   29 Oct 1996 12:55:48   AOONWAL
// Modified during NOV96 freeze
//
//    Rev 1.67   25 Oct 1996 12:24:54   CFORMUL
// Added WM.Wazzu.E
//
//    Rev 1.66   24 Oct 1996 15:01:50   CFORMUL
// Added WM.Divina.B
//
//    Rev 1.65   24 Oct 1996 14:00:48   CFORMUL
// Added WM.Wazzu.D
//
//    Rev 1.64   21 Oct 1996 17:02:44   CFORMUL
// Added repair for WM.Jakarta.b
//
//    Rev 1.63   18 Oct 1996 18:18:04   AOONWAL
// Added repair for "WM.Concept.J"
//
//    Rev 1.62   17 Oct 1996 17:55:56   AOONWAL
// Removed "WM.Taiwan1" repair
//
//    Rev 1.61   17 Oct 1996 17:46:54   AOONWAL
// Added repair for "WM.Taiwan1"
//
//    Rev 1.60   16 Oct 1996 16:58:08   JWILBER
// Added repairs for: NOP.B    1fd3/4
//                    Reflex   1fd5/6
//                    Satanic  1fdb
//
//    Rev 1.59   16 Oct 1996 16:08:48   AOONWAL
// Modified and commented out repair for "WM.Concept.I"
//
//    Rev 1.58   16 Oct 1996 14:59:48   AOONWAL
// commented out repair for "WM.Concept.I"
//
//    Rev 1.57   16 Oct 1996 14:43:44   AOONWAL
// Added repair for "WM.Concept.I"
//
//    Rev 1.56   16 Oct 1996 14:00:04   AOONWAL
// Added repair for "WM.Concept.E (3)" and "WM.Concept.E (4)"
//
//    Rev 1.55   16 Oct 1996 13:14:58   CFORMUL
// Added Repair for WM.Concept.b and WM.Tedious
//
//    Rev 1.54   11 Oct 1996 15:25:36   AOONWAL
// Added repair for "WM.Bandung.b"
//
//    Rev 1.53   09 Oct 1996 17:52:04   JWILBER
// Added repair def for Wazzu.B, 1fcf.
//
//    Rev 1.52   07 Oct 1996 17:27:06   CFORMUL
// Added two more to the list of macros to delete for WM.Clock
//
//    Rev 1.51   07 Oct 1996 13:15:42   CFORMUL
// Added repair for WM.Clock
//
//    Rev 1.50   04 Oct 1996 16:32:40   AOONWAL
// Changed macro name to be deleted in Terbaru repair from 0FXX to OFXX
//
//    Rev 1.49   04 Oct 1996 14:27:28   JWILBER
// Eliminated duplicate repair for ColorB - it's the same as Colors.
//
//    Rev 1.48   02 Oct 1996 15:42:16   AOONWAL
// Added repair for "WM.Terbaru"
//
//    Rev 1.47   27 Sep 1996 14:26:04   CFORMUL
// Added repair for 8 new macro viruses
//
//    Rev 1.46   26 Sep 1996 13:52:06   CFORMUL
// Added AUTONEW string for macro repair
//
//    Rev 1.45   25 Sep 1996 16:03:18   CFORMUL
// Added repairs for 4 macro viruses: Alliance, EasyMan, Saver, Spooky
//
//    Rev 1.44   24 Sep 1996 18:04:30   AOONWAL
// Added repair for Wazzu (3)
//
//    Rev 1.43   17 Sep 1996 16:08:52   DCHI
// Looks like the umlauted O in DATEI÷FFNEN was modified somehow?!
// Changed it back.
//
//    Rev 1.42   17 Sep 1996 15:48:34   DCHI
// Added XENIXOS1 repair.
//
//    Rev 1.41   13 Sep 1996 15:59:36   JWILBER
// Added repair for Indonesia, 1fbe.
//
//    Rev 1.40   10 Sep 1996 14:07:20   DANG
// Added Bogus macro virus
//
//    Rev 1.39   05 Sep 1996 18:03:12   SCOURSE
// Added repair specific for Fishfood.  Apparently one macro was left over.
//
//    Rev 1.38   05 Sep 1996 15:11:18   SCOURSE
// Modified repair for Fishfood
//
//    Rev 1.37   05 Sep 1996 14:38:52   SCOURSE
// Added entry for Goldfish
//
//    Rev 1.36   04 Sep 1996 13:38:30   CFORMUL
// Fixed a typo in one of the German macro names
//
//    Rev 1.35   29 Aug 1996 17:43:12   DANG
// Another small typo fixed.
//
//    Rev 1.34   29 Aug 1996 16:12:14   DANG
// Fixed missing comma when I made changes.
//
//    Rev 1.33   29 Aug 1996 16:07:30   DANG
// Added Concept.H
//
//    Rev 1.32   29 Aug 1996 15:21:10   CFORMUL
// Added repair for BuroNeu
//
//    Rev 1.31   28 Aug 1996 17:23:38   CFORMUL
// Added repair for Bosco
//
//    Rev 1.30   19 Aug 1996 19:27:38   JWILBER
// Added repair pointer for Jakarta, 1fb3.  Uses same repair as
// FormatC.  Eliminated duplicate repairs.  FormatC, Wazzu, Wazzu2,
// Infezione, Jakarta, Phantom, and Microsloth use the same repair.
// DMV, MDMADMV, and Divina use the same repair also.
//
//    Rev 1.29   16 Aug 1996 12:08:20   CFORMUL
// Added repair for variant of wazzu.
//
//    Rev 1.28   15 Aug 1996 21:00:56   AOONWAL
// Added repair for Microsloth
//
//    Rev 1.27   15 Aug 1996 19:41:22   AOONWAL
// Added repair for Telefonica
//
//    Rev 1.26   29 Jul 1996 17:56:22   SCOURSE
// Added entry for Divina
//
//    Rev 1.25   29 Jul 1996 14:56:04   LNGUYEN
// Deleted an empty line.
//
//    Rev 1.24   25 Jul 1996 19:27:24   JWILBER
// Added repair info for Friendly, 1faa.
//
//    Rev 1.23   24 Jul 1996 10:52:28   LNGUYEN
// Added new repaire info. for Phantom
//
//    Rev 1.22   23 Jul 1996 20:41:18   AOONWAL
// Added repair for Nuclear.b
//
//    Rev 1.21   23 Jul 1996 20:03:02   LNGUYEN
// Correction for MadDog Mac. define.
//
//    Rev 1.20   23 Jul 1996 19:56:18   LNGUYEN
// Delared new macro for MadDog.
//
//    Rev 1.19   23 Jul 1996 19:38:22   LNGUYEN
// Restored repaire info. to MadDog.
//
//    Rev 1.18   23 Jul 1996 19:04:04   CFORMUL
// Added Repair for Polite macro
//
//    Rev 1.17   17 Jul 1996 18:03:56   LNGUYEN
// Regressed changes to July 12th.
//
//    Rev 1.15   12 Jul 1996 14:10:40   JWILBER
// Added repair for ATOM, 1fa4.
//
//    Rev 1.14   11 Jul 1996 16:41:52   LNGUYEN
// Added Repair info. for MdmaDMV
//
//    Rev 1.13   11 Jul 1996 15:44:56   AOONWAL
// Added repair for Parasite
//
//    Rev 1.12   11 Jul 1996 14:29:10   JWILBER
// Added repairs for NOP macro viruses 1f9c/1a.
//
//    Rev 1.11   28 Jun 1996 14:05:26   CFORMUL
// Added repairs for Xenixos and Boom
//
//    Rev 1.10   10 May 1996 16:43:42   JWILBER
// Added repair for Infezione.  It's just a pointer to the Wazzu
// repair, which will fix Infezione, too.
//
//    Rev 1.9   07 May 1996 17:16:52   JALLEE
// Added Irish macro virus
//
//    Rev 1.8   29 Apr 1996 19:48:20   JWILBER
// Added repair for Wazzu.
//
//    Rev 1.7   10 Apr 1996 12:16:20   JWILBER
// Removed French Concept DOC file fix.
//
//    Rev 1.6   09 Apr 1996 13:56:22   JALLEE
// Added Imposter macro virus.
//
//    Rev 1.5   08 Apr 1996 18:22:52   JWILBER
// Fixed typos w/ ConceptF* repairs.
//
//    Rev 1.4   08 Apr 1996 17:21:44   JWILBER
// Added repairs for French Concept 0x1f79 and 0x1f80 in *.doc and
// normal.dot files.
//
//    Rev 1.3   15 Feb 1996 16:23:24   JALLEE
// Added repair for Hot macro virus.
//
//    Rev 1.2   06 Feb 1996 16:53:48   DCHI
// Correction of CONCEPT def to account for NORMAL.DOT.
//
//    Rev 1.1   30 Jan 1996 15:43:32   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.0   03 Jan 1996 17:14:56   DCHI
// Initial revision.
//
//************************************************************************

#ifndef SYM_NLM

#include "mcrrepdf.h"

#include "navexshr.h"

BYTE gpszRepairName[] =                 "";
BYTE gpszRepairNameOFXX[] =             "OFXX";
BYTE gpszRepairNameA678[] =             "A678";
BYTE gpszRepairNameAAAZAO[] =           "AAAZAO";
BYTE gpszRepairNameAAAZFS[] =           "AAAZFS";
BYTE gpszRepairNameAAACBCAD0[] =        "AAAÀ –";
BYTE gpszRepairNameAAAD9DBD2[] =        "AAAŸ€“";
BYTE gpszRepairNameABBRECHEN[] =        "ABBRECHEN";
BYTE gpszRepairNameACTION[] =           "ACTION";
BYTE gpszRepairNameANTIVIRUS[] =        "ANTIVIRUS";
BYTE gpszRepairNameAOPNFINISH[] =       "AOPNFINISH";
BYTE gpszRepairNameAPPDER[] =           "APPDER";
BYTE gpszRepairNameATOM[] =             "ATOM";
BYTE gpszRepairNameAUTOCLOSE[] =        "AUTOCLOSE";
BYTE gpszRepairNameAUTOEXEC[] =         "AUTOEXEC";
BYTE gpszRepairNameAUTOEXIT[] =         "AUTOEXIT";
BYTE gpszRepairNameAUTOOPEN[] =         "AUTOOPEN";
BYTE gpszRepairNameAUTONEW[] =          "AUTONEW";
BYTE gpszRepairNameBESTANDOPSLAANALS[]= "BESTANDOPSLAANALS";
BYTE gpszRepairNameBOOM[] =             "BOOM";
BYTE gpszRepairNameBURONEU[] =          "B‹RONEU";
BYTE gpszRepairNameCANCEL[] =           "CANCEL";
BYTE gpszRepairNameCEPAYLOAD[] =        "ŒPAYLOAD";
BYTE gpszRepairNameCFXX[] =             "CFXX";
BYTE gpszRepairNameCITPYRC[] =          "CITPYRC";
BYTE gpszRepairNameCK[] =               "CK";
BYTE gpszRepairNameCK1[] =              "CK1";
BYTE gpszRepairNameCRYPTIC[] =          "CRYPTIC";
BYTE gpszRepairNameCTFISTCCLLESS11[] =  "CTFISTCCLLESS11";
BYTE gpszRepairNameCTFBORNIN83[] =      "CTFBORNIN83";
BYTE gpszRepairNameDATEIALLES[] =       "DATEIALLESSPEICHERN";
BYTE gpszRepairNameDATEIBEENDEN[] =     "DATEIBEENDEN";
BYTE gpszRepairNameDATEIDOKVORLAGEN[] = "DATEIDOKVORLAGEN";
BYTE gpszRepairNameDATEIDRUCKEN[] =     "DATEIDRUCKEN";
BYTE gpszRepairNameDATEIDRUCKENSTD[] =  "DATEIDRUCKENSTANDARD";
BYTE gpszRepairNameDATEINEU[] =         "DATEINEU";
BYTE gpszRepairNameDATEIOFFNEN[] =      "DATEI÷FFNEN";
BYTE gpszRepairNameDATEISCHLIEBEN[] =   "DATEISCHLIEﬂEN";
BYTE gpszRepairNameDATEISCHLIESSEN[] =  "DATEISCHLIE_EN";
BYTE gpszRepairNameDATEISPEICHERN[] =   "DATEISPEICHERN";
BYTE gpszRepairNameDATEISPEICHERNU[] =  "DATEISPEICHERNUNTER";
BYTE gpszRepairNameDATUMUNDUHRZEIT[] =  "DATUMUNDUHRZEIT";
BYTE gpszRepairNameDIAMONDSUTRA[] =     "DIAMONDSUTRA";
BYTE gpszRepairNameDOCCLOSE[] =         "DOCCLOSE";
BYTE gpszRepairNameDOGGIE[] =           "DOGGIE";
BYTE gpszRepairNameDOKSCHLIEBEN[] =     "DOKUMENTSCHLIEﬂEN";
BYTE gpszRepairNameDMV[] =              "DMV";
BYTE gpszRepairNameDRAWBRINGINFROUT[] = "DRAWBRINGINFROUT";
BYTE gpszRepairNameDROP[] =             "DROP";
BYTE gpszRepairNameDROPSURIV[] =        "DROPSURIV";
BYTE gpszRepairNameDUMMY[] =            "DUMMY";
BYTE gpszRepairNameEDITSIZE[] =         "EDITSIZE";
BYTE gpszRepairNameEXTRASMACRO[] =      "EXTRASMACRO";
BYTE gpszRepairNameEXTRASMAKRO[] =      "EXTRASMAKRO";
BYTE gpszRepairNameF1[] =               "F1";
BYTE gpszRepairNameF2[] =               "F2";
BYTE gpszRepairNameFA[] =               "FA";
BYTE gpszRepairNameFAST[] =             "FAST";
BYTE gpszRepairNameFCFINISH[] =         "FCFINISH";
BYTE gpszRepairNameFCLOSE[] =           "FCLOSE";
BYTE gpszRepairNameFICHIER[] =          "FICHIERENREGISTRERSOUS";
BYTE gpszRepairNameFILECLOSE[] =        "FILECLOSE";
BYTE gpszRepairNameFILEEXIT[] =         "FILEEXIT";
BYTE gpszRepairNameFILENEW[] =          "FILENEW";
BYTE gpszRepairNameFILEOPEN[] =         "FILEOPEN";
BYTE gpszRepairNameFILEPRINT[] =        "FILEPRINT";
BYTE gpszRepairNameFILEPRINTDEFAULT[] = "FILEPRINTDEFAULT";
BYTE gpszRepairNameFILESAVE[] =         "FILESAVE";
BYTE gpszRepairNameFILESAVEAS[] =       "FILESAVEAS";
BYTE gpszRepairNameFILESORT[] =         "FILESORT";
BYTE gpszRepairNameGANGSTERZ[] =        "GANGSTERZ";
BYTE gpszRepairNameHAHA[] =             "HAHA";
BYTE gpszRepairNameI8U9Y13[] =          "I8U9Y13";
BYTE gpszRepairNameIKWORDNIETGOED1[] =  "IKWORDNIETGOED1";
BYTE gpszRepairNameIKWORDNIETGOED2[] =  "IKWORDNIETGOED2";
BYTE gpszRepairNameINFIZIEREN[] =       "INFIZIEREN";
BYTE gpszRepairNameINSERTPAGEBREAK[] =  "INSERTPAGEBREAK";
BYTE gpszRepairNameINSERTPAYLOAD[] =    "INSERTPAYLOAD";
BYTE gpszRepairNameINSERTPBREAK[] =     "INSERTPBREAK";
BYTE gpszRepairNameK[] =                "K";
BYTE gpszRepairNameLADING[] =           "LADING";
BYTE gpszRepairNameLOAD[] =             "LOAD";
BYTE gpszRepairNameMACROS[] =           "MACROS";
BYTE gpszRepairNameMCAFEE1[] =          "MCAFEE1";
BYTE gpszRepairNameMICROSOFT01[] =      "MICROSOFT01";
BYTE gpszRepairNameMICROSOFT02[] =      "MICROSOFT02";
BYTE gpszRepairNameMICROSOFT03[] =      "MICROSOFT03";
BYTE gpszRepairNameMICROSOFT04[] =      "MICROSOFT04";
BYTE gpszRepairNameMICROSOFT05[] =      "MICROSOFT05";
BYTE gpszRepairNameNEWAO[] =            "NEWAO";
BYTE gpszRepairNameNEWFS[] =            "NEWFS";
BYTE gpszRepairNameNOP[] =              "NOP";
BYTE gpszRepairNameNOWRUN[] =           "NOWRUN";
BYTE gpszRepairNameOFFNEN[] =           "÷FFNEN";
BYTE gpszRepairNamePARA[] =             "PARA";
BYTE gpszRepairNamePARADISE[] =         "PARADISE";
BYTE gpszRepairNamePAYLOAD[] =          "PAYLOAD";
BYTE gpszRepairNamePRESENTV[] =         "PRESENTV";
BYTE gpszRepairNamePRESENTW[] =         "PRESENTW";
BYTE gpszRepairNamePRESENTZ[] =         "PRESENTZ";
BYTE gpszRepairNameRPAO[] =             "RPAO";
BYTE gpszRepairNameRPAE[] =             "RPAE";
BYTE gpszRepairNameRPFO[] =             "RPFO";
BYTE gpszRepairNameRPFS[] =             "RPFS";
BYTE gpszRepairNameRPTC[] =             "RPTC";
BYTE gpszRepairNameRPTM[] =             "RPTM";
BYTE gpszRepairNameRPFSA[] =            "RPFSA";
BYTE gpszRepairNameSHOW[] =             "SHOW";
BYTE gpszRepairNameSITE[] =             "SITE";
BYTE gpszRepairNameSPEICHERN[] =        "SPEICHERN";
BYTE gpszRepairNameSPOOKY[] =           "SPOOKY";
BYTE gpszRepairNameSTARTOFDOC[] =       "STARTOFDOC";
BYTE gpszRepairNameSTRYX1[] =           "STRYX1";
BYTE gpszRepairNameSTRYX2[] =           "STRYX2";
BYTE gpszRepairNameSTRYXONE[] =         "STRYXONE";
BYTE gpszRepairNameSTRYXTWO[] =         "STRYXTWO";
BYTE gpszRepairNameSYSTEM[] =           "SYSTEM";
BYTE gpszRepairNameTALK[] =             "TALK";
BYTE gpszRepairNameTELEFONICA[] =       "TELEFONICA";
BYTE gpszRepairNameTOOLSCUSTOMIZE[] =   "TOOLSCUSTOMIZE";
BYTE gpszRepairNameTOOLSMACRO[] =       "TOOLSMACRO";
BYTE gpszRepairNameTOOLSREPAGINAT[] =   "TOOLSREPAGINAT";
BYTE gpszRepairNameTOOLSSPELLING[] =    "TOOLSSPELLING";
BYTE gpszRepairNameVAUTONEW[] =         "VAUTONEW";
BYTE gpszRepairNameVCLOSE[] =           "VCLOSE";
BYTE gpszRepairNameVFILESAVEAS[] =      "VFILESAVEAS";
BYTE gpszRepairNameVGOJOHNNY[] =        "VGOJOHNNY";
BYTE gpszRepairNameVOPEN[] =            "VOPEN";
BYTE gpszRepairNameWORDHELP[] =         "WORDHELP";
BYTE gpszRepairNameWORDHELPNT[] =       "WORDHELPNT";
BYTE gpszRepairNameXAAZAO[] =           "XAAZAO";
BYTE gpszRepairNameXAAZFS[] =           "XAAZFS";
BYTE gpszRepairNameXUTOOPEN[] =         "XUTOOPEN";
BYTE gpszRepairNameXAYLOAD[] =          "XAYLOAD";

//********************************************************************
// Colors / ColorB
//********************************************************************

BYTE LPF *gpszMacroRepairColors[] =
{
    gpszRepairNameAUTOCLOSE,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILEEXIT,
    gpszRepairNameFILENEW,
    gpszRepairNameFILESAVE,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameMACROS,
    gpszRepairNameTOOLSMACRO,
    NULL
};

//********************************************************************
// Concept
//********************************************************************

BYTE LPF *gpszMacroRepairConcept0[] =
{
    gpszRepairNameAAAZAO,
    gpszRepairNameAAAZFS,
    gpszRepairNameAUTOOPEN,
    gpszRepairNamePAYLOAD,
    NULL
};

BYTE LPF *gpszMacroRepairConcept1[] =
{
    gpszRepairNameAAAZAO,
    gpszRepairNameAAAZFS,
    gpszRepairNameFILESAVEAS,
    gpszRepairNamePAYLOAD,
    NULL
};

// French Concept

// Detection/Repair of DOCs is the same as English version
// Detection/Repair of NORMAL.DOT is only different because of the
//  translation of "FileSaveAs"

BYTE LPF *gpszMacroRepairConceptF[] =
{
    gpszRepairNameAAAZAO,
    gpszRepairNameAAAZFS,
    gpszRepairNameFICHIER,
    gpszRepairNamePAYLOAD,
    NULL
};

//********************************************************************
// DMV / MDMADMV / Divina / Divina.B / Concept.J / Helper / Divina.C
// MDMA.C
//********************************************************************

BYTE LPF *gpszMacroRepairDMV[] =
{
    gpszRepairNameAUTOCLOSE,
    NULL
};

//********************************************************************
// FormatC / Wazzu / Wazzu2 / Wazzu3 / Infezione / Jakarta / Phantom
// Microsloth / Bosco / KillDLL / WazzuB / Jakarta.b / Wazzu.D / Wazzu.E
// Wazzu.F / Jakarta.Damaged / Twno.C (1) / Npad.C / Npad.E / Npad.H / Npad.J
//********************************************************************

BYTE LPF *gpszMacroRepairFormatC[] =
{
    gpszRepairNameAUTOOPEN,
    NULL
};

//********************************************************************
// Nuclear
//********************************************************************

BYTE LPF *gpszMacroRepairNuclear[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameDROPSURIV,
    gpszRepairNameFILEEXIT,
    gpszRepairNameFILEPRINT,
    gpszRepairNameFILEPRINTDEFAULT,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameINSERTPAYLOAD,
    gpszRepairNamePAYLOAD,
    NULL
};

//********************************************************************
// Nuclear.A.Damaged
//********************************************************************

BYTE LPF *gpszMacroRepairNukeDam[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameDROPSURIV,
    gpszRepairNameFILEEXIT,
    gpszRepairNameFILEPRINT,
    gpszRepairNameFILEPRINTDEFAULT,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameINSERTPAYLOAD,
    gpszRepairNamePAYLOAD,
    NULL
};

//********************************************************************
// Hot
//********************************************************************

BYTE LPF *gpszMacroRepairHot0[] =
{

    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILESAVE,
    gpszRepairNameSTARTOFDOC,
    gpszRepairNameINSERTPAGEBREAK,
    NULL
};

BYTE LPF *gpszMacroRepairHot1[] =
{

    gpszRepairNameAUTOOPEN,
    gpszRepairNameDRAWBRINGINFROUT,
    gpszRepairNameINSERTPBREAK,
    gpszRepairNameTOOLSREPAGINAT,
    NULL

};

//********************************************************************
// Imposter
//********************************************************************

BYTE LPF *gpszMacroRepairImp0[] =
{

    gpszRepairNameAUTOCLOSE,
    gpszRepairNameDMV,
    NULL

};

BYTE LPF *gpszMacroRepairImp1[] =
{

    gpszRepairNameFILESAVEAS,
    gpszRepairNameDMV,
    NULL

};

//********************************************************************
// Irish
//********************************************************************

BYTE LPF *gpszMacroRepairIrish0[] =
{
    gpszRepairNameFILESAVE,
    gpszRepairNameWORDHELP,
    gpszRepairNameWORDHELPNT,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameANTIVIRUS,
    NULL

};

BYTE LPF *gpszMacroRepairIrish1[] =
{

    gpszRepairNameANTIVIRUS,
    gpszRepairNameWORDHELP,
    gpszRepairNameWORDHELPNT,
    gpszRepairNameAUTOOPEN,
    NULL
};

//********************************************************************
// Xenixos
//********************************************************************

BYTE LPF *gpszMacroRepairXenixos[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameDATEIBEENDEN,
    gpszRepairNameDATEIDRUCKEN,
    gpszRepairNameDATEIDRUCKENSTD,
    gpszRepairNameDATEIOFFNEN,
    gpszRepairNameDATEISPEICHERN,
    gpszRepairNameDATEISPEICHERNU,
    gpszRepairNameEXTRASMAKRO,
    gpszRepairNameDROP,
    gpszRepairNameDUMMY,
    NULL
};

//********************************************************************
// Xenixos1 / Switches
//********************************************************************

BYTE LPF *gpszMacroRepairXenixos1[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    NULL
};

//********************************************************************
// Boom
//********************************************************************

BYTE LPF *gpszMacroRepairBoom[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameDATEISPEICHERNU,
    gpszRepairNameSYSTEM,
    NULL
};

//********************************************************************
// NOP / NOP.B
//********************************************************************

BYTE LPF *gpszMacroRepairNOP1[] =               // Normal.dot
{
    gpszRepairNameNOP,
    gpszRepairNameDATEISPEICHERN,
    NULL
};

BYTE LPF *gpszMacroRepairNOP2[] =               // Documents
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameNOP,
    NULL
};

//********************************************************************
// Parasite
//********************************************************************

BYTE LPF *gpszMacroRepairParasite0[] =
{

    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOEXIT,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILESAVEAS,
    gpszRepairNamePAYLOAD,
    gpszRepairNamePARA,
    gpszRepairNameSITE,
    NULL
};

BYTE LPF *gpszMacroRepairParasite1[] =
{

    gpszRepairNameAUTOOPEN,
    gpszRepairNamePAYLOAD,
    gpszRepairNamePARA,
    gpszRepairNameSITE,
    gpszRepairNameK,
    gpszRepairNameA678,
    gpszRepairNameI8U9Y13,
    NULL
};

//********************************************************************
// ATOM, ATOM.b
//********************************************************************

BYTE LPF *gpszMacroRepairATOM[] =
{
    gpszRepairNameATOM,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILEOPEN,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// Polite
//********************************************************************

BYTE LPF *gpszMacroRepairPolite[] =
{
    gpszRepairNameFILECLOSE,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// MADDOG
//********************************************************************

BYTE LPF *gpszMacroRepairMADDOG[] =
{
    gpszRepairNameAUTOCLOSE,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILECLOSE,
    gpszRepairNameAOPNFINISH,
    gpszRepairNameFCFINISH,
    NULL
};

//********************************************************************
// Nuclear.b
//********************************************************************

BYTE LPF *gpszMacroRepairNuclear_b[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILEPRINT,
    gpszRepairNameFILEPRINTDEFAULT,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameINSERTPAYLOAD,
    gpszRepairNamePAYLOAD,
    NULL
};

//********************************************************************
// Friendly
//********************************************************************

BYTE LPF *gpszMacroRepairFriendly[] =
{
    gpszRepairNameABBRECHEN,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameCANCEL,
    gpszRepairNameDATEIBEENDEN,
    gpszRepairNameDATEINEU,
    gpszRepairNameDATEISCHLIESSEN,
    gpszRepairNameDATEISPEICHERN,
    gpszRepairNameDATEISPEICHERNU,
    gpszRepairNameDATEIOFFNEN,
    gpszRepairNameEXTRASMACRO,
    gpszRepairNameEXTRASMAKRO,
    gpszRepairNameFAST,
    gpszRepairNameFILEEXIT,
    gpszRepairNameFILENEW,
    gpszRepairNameFILEOPEN,
    gpszRepairNameFILESAVE,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameINFIZIEREN,
    gpszRepairNameTALK,
    NULL
};

//********************************************************************
// telefonica
//********************************************************************

BYTE LPF *gpszMacroRepairTelefonica[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameTELEFONICA,
    gpszRepairNameDATEIBEENDEN,
    gpszRepairNameDATEIDRUCKEN,
    gpszRepairNameDATEINEU,
    gpszRepairNameDATEIOFFNEN,
    NULL
};

//********************************************************************
// BuroNeu
//********************************************************************

BYTE LPF *gpszMacroRepairBuroNeu0[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameBURONEU,
    NULL
};

BYTE LPF *gpszMacroRepairBuroNeu1[] =
{
    gpszRepairNameDATEISPEICHERN,
    gpszRepairNameBURONEU,
    NULL
};

//********************************************************************
// Concept.H
//********************************************************************

BYTE LPF *gpszMacroRepairConceptH0[] =
{
    gpszRepairNameCITPYRC,
    gpszRepairNameCRYPTIC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNamePAYLOAD,
    NULL
};

BYTE LPF *gpszMacroRepairConceptH1[] =
{
    gpszRepairNameCITPYRC,
    gpszRepairNameCRYPTIC,
    gpszRepairNameFILESAVEAS,
    gpszRepairNamePAYLOAD,
    NULL
};

//********************************************************************
// Fishfood / LookC
//********************************************************************

BYTE LPF *gpszMacroRepairFishfood[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameAUTOCLOSE,
    NULL
};

//********************************************************************
// Bogus
//********************************************************************

BYTE LPF *gpszMacroRepairBogus0[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameTOOLSMACRO,
    gpszRepairNameMICROSOFT01,
    gpszRepairNameMICROSOFT02,
    gpszRepairNameMICROSOFT04,
    gpszRepairNameMICROSOFT05,
    NULL
};

BYTE LPF *gpszMacroRepairBogus1[] =
{
    gpszRepairNameAUTOCLOSE,
    gpszRepairNameTOOLSMACRO,
    gpszRepairNameMICROSOFT01,
    gpszRepairNameMICROSOFT02,
    gpszRepairNameMICROSOFT03,
    gpszRepairNameMICROSOFT04,
    gpszRepairNameMICROSOFT05,
    NULL
};

//********************************************************************
// Bandung (AKA Indonesia) / Bandung.b
//********************************************************************

BYTE LPF *gpszMacroRepairIndo[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILESAVE,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameTOOLSCUSTOMIZE,
    gpszRepairNameTOOLSMACRO,
    NULL
};

//********************************************************************
// Alliance
//********************************************************************

BYTE LPF *gpszMacroRepairAlliance[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameAUTONEW,
    NULL
};

//********************************************************************
// Saver
//********************************************************************

BYTE LPF *gpszMacroRepairSaver[] =
{
    gpszRepairNameDATEISPEICHERN,
    NULL
};

//********************************************************************
// Spooky
//********************************************************************

BYTE LPF *gpszMacroRepairSpooky[] =
{
    gpszRepairNameSPOOKY,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameDATEIDRUCKEN,
    gpszRepairNameDATEIDRUCKENSTD,
    gpszRepairNameDATEIOFFNEN,
    gpszRepairNameDATEISPEICHERNU,
    gpszRepairNameEXTRASMAKRO,
    gpszRepairNameDATEIDOKVORLAGEN,
    NULL
};

//********************************************************************
// Birthday
//********************************************************************

BYTE LPF *gpszMacroRepairBirthday[] =
{
    gpszRepairNameDATEISPEICHERNU,
    gpszRepairNameAUTOOPEN,
    NULL
};

//********************************************************************
// Doggie
//********************************************************************

BYTE LPF *gpszMacroRepairDoggie[] =
{
    gpszRepairNameDOGGIE,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// Concept.c
//********************************************************************

BYTE LPF *gpszMacroRepairConceptC[] =
{
    gpszRepairNameF1,
    gpszRepairNameF2,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameBOOM,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// Concept.d
//********************************************************************

BYTE LPF *gpszMacroRepairConceptD[] =
{
    gpszRepairNameEDITSIZE,
    gpszRepairNameFILESORT,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameHAHA,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// Concept.e
//********************************************************************

BYTE LPF *gpszMacroRepairConceptE[] =
{
    gpszRepairNameAAAZAO,
    gpszRepairNameAAAZFS,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameLOAD,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// Concept.hcr
//********************************************************************

BYTE LPF *gpszMacroRepairConceptHcr[] =
{
    gpszRepairNameXAAZAO,
    gpszRepairNameXAAZFS,
    gpszRepairNameXUTOOPEN,
    gpszRepairNameXAYLOAD,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// Terbaru
//********************************************************************

BYTE LPF *gpszMacroRepairTerbaru0[] =
{
    gpszRepairNameOFXX,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOCLOSE,
    NULL
};

BYTE LPF *gpszMacroRepairTerbaru1[] =
{
    gpszRepairNameCFXX,
    gpszRepairNameSHOW,
    gpszRepairNameAUTOOPEN,
    NULL
};

//********************************************************************
// Clock
//********************************************************************

BYTE LPF *gpszMacroRepairClock[] =
{
    gpszRepairNameACTION,
    gpszRepairNameOFFNEN,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameSPEICHERN,
    gpszRepairNameEXTRASMAKRO,
    gpszRepairNameDATEISCHLIEBEN,
    gpszRepairNameDATUMUNDUHRZEIT,
    gpszRepairNameDATEIDOKVORLAGEN,
    gpszRepairNameDATEIALLES,
    gpszRepairNameDATEISPEICHERNU,
    gpszRepairNameDATEISPEICHERN,
    gpszRepairNameDATEIOFFNEN,
    NULL
};

//********************************************************************
// Tedious
//********************************************************************

BYTE LPF *gpszMacroRepairTedious[] =
{
    gpszRepairNameAUTONEW,
    gpszRepairNameVAUTONEW,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameVFILESAVEAS,
    NULL
};

//********************************************************************
// Concept.b
//********************************************************************

BYTE LPF *gpszMacroRepairConceptB[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameLADING,
    gpszRepairNameIKWORDNIETGOED1,
    gpszRepairNameIKWORDNIETGOED2,
    gpszRepairNameBESTANDOPSLAANALS,
    NULL
};

//********************************************************************
// Concept.i
//********************************************************************

BYTE LPF *gpszMacroRepairConceptI[] =
{
    gpszRepairNameAAACBCAD0,
    gpszRepairNameAAAD9DBD2,
    gpszRepairNameDOCCLOSE,
    gpszRepairNameCEPAYLOAD,
    gpszRepairNameTOOLSSPELLING,
    NULL
};

//********************************************************************
// Reflex
//********************************************************************

BYTE LPF *gpszMacroRepairReflex0[] =                // Documents
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFA,
    gpszRepairNameFCLOSE,
    gpszRepairNameNOWRUN,       // May or may not be present
    NULL
};

BYTE LPF *gpszMacroRepairReflex1[] =                // Normal.dot
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFA,           // May or may not be present
    gpszRepairNameFCLOSE,
    gpszRepairNameFILECLOSE,
    NULL
};

//********************************************************************
// Satanic
//********************************************************************

BYTE LPF *gpszMacroRepairSatanic[] =
{
    gpszRepairNameAUTOCLOSE,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOEXIT,
    gpszRepairNameAUTONEW,
    gpszRepairNameAUTOOPEN,
    NULL
};

//********************************************************************
// Concept.K
//********************************************************************

BYTE LPF *gpszMacroRepairConceptK[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILESAVE,
    gpszRepairNameNEWFS,
    gpszRepairNameNEWAO,
    NULL
};

//********************************************************************
// Alien
//********************************************************************

BYTE LPF *gpszMacroRepairAlien[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameAUTOCLOSE,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// Rapi
//********************************************************************

BYTE LPF *gpszMacroRepairRapi[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameFILEOPEN,
    gpszRepairNameFILESAVE,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameTOOLSMACRO,
    gpszRepairNameTOOLSCUSTOMIZE,
    gpszRepairNameRPAO,
    gpszRepairNameRPAE,
    gpszRepairNameRPFO,
    gpszRepairNameRPFS,
    gpszRepairNameRPTC,
    gpszRepairNameRPTM,
    gpszRepairNameRPFSA,
    NULL
};

//********************************************************************
// Gangsterz (formerly BigDaddy)
//********************************************************************

BYTE LPF *gpszMacroRepairBigDaddy[] =
{
    gpszRepairNameGANGSTERZ,
    gpszRepairNamePARADISE,
    NULL
};

//********************************************************************
// Stryx
//********************************************************************

BYTE LPF *gpszMacroRepairStryx[] =
{
    gpszRepairNameDATEISCHLIEBEN,
    gpszRepairNameDOKSCHLIEBEN,
    gpszRepairNameSTRYXONE,
    gpszRepairNameSTRYXTWO,
    NULL
};

BYTE LPF *gpszMacroRepairStryx2[] =
{
    gpszRepairNameDATEISCHLIEBEN,
    gpszRepairNameDOKSCHLIEBEN,
    gpszRepairNameSTRYX1,
    gpszRepairNameSTRYX2,
    NULL
};

//********************************************************************
// Weather.B
//********************************************************************

BYTE LPF *gpszMacroRepairWeatherB[] =
{
    gpszRepairNameAUTONEW,
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameTOOLSMACRO,
    NULL
};

//********************************************************************
// Theatre
//********************************************************************

BYTE LPF *gpszMacroRepairTheatre[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameCK,
    gpszRepairNameCK1,
    gpszRepairNameDOCCLOSE,
    gpszRepairNameFILECLOSE,
    gpszRepairNameTOOLSMACRO,
    NULL
};

//********************************************************************
// Twno.C (2) / Taiwan1 / Twno.D
//********************************************************************

BYTE LPF *gpszMacroRepairTwnoC2[] =
{
    gpszRepairNameAUTOCLOSE,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameAUTONEW,
    NULL
};

//********************************************************************
// TwnoB1
//********************************************************************

BYTE LPF *gpszMacroRepairTwnoB1[] =
{
    gpszRepairNameAUTOEXEC,
    NULL
};

//********************************************************************
// TwnoB2
//********************************************************************

BYTE LPF *gpszMacroRepairTwnoB2[] =
{
    gpszRepairNameAUTOEXEC,
    gpszRepairNameAUTOCLOSE,
    gpszRepairNameAUTONEW,
    NULL
};

//********************************************************************
// AntiConcept
//********************************************************************

BYTE LPF *gpszMacroRepairAntiCon[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILENEW,
    gpszRepairNameFILESAVE,
    gpszRepairNameFILESAVEAS,
    NULL
};

//********************************************************************
// Concept.T
//********************************************************************

BYTE LPF *gpszMacroRepairConceptT[] =
{
    gpszRepairNameVCLOSE,
    gpszRepairNameVOPEN,
    gpszRepairNamePAYLOAD,
    gpszRepairNameAUTOEXIT,
    gpszRepairNameAUTOOPEN,
    gpszRepairNameAUTOCLOSE,
    NULL
};

//********************************************************************
// Sutra
//********************************************************************

BYTE LPF *gpszMacroRepairSutra[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILESAVEAS,
    gpszRepairNameCTFBORNIN83,
    gpszRepairNameDIAMONDSUTRA,
    gpszRepairNameCTFISTCCLLESS11,
    NULL
};

//********************************************************************
// Nuclear.E
//********************************************************************

BYTE LPF *gpszMacroRepairNuclearE[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameMCAFEE1,
    gpszRepairNameFILEEXIT,
    gpszRepairNameFILEPRINT,
    gpszRepairNameFILEPRINTDEFAULT,
    gpszRepairNameFILESAVEAS,
    NULL
};


//********************************************************************
// JOHNNY
//********************************************************************

//normal.dot has no AUTOOPEN
//if saved by FILESAVEAS, docs have no FILESAVEAS
//if saved by FILESAVE, docs have no FILESAVEAS, FILESAVE

BYTE LPF *gpszMacroRepairJohnny0[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameFILESAVE,
    gpszRepairNameFILESAVEAS,
    gpszRepairNamePRESENTV,
    gpszRepairNamePRESENTW,
    gpszRepairNamePRESENTZ,
    gpszRepairNameVGOJOHNNY,
    NULL
};

//********************************************************************
// WM.APPDER
//********************************************************************

//normal.dot has no AUTOOPEN

BYTE LPF *gpszMacroRepairAppder[] =
{
    gpszRepairNameAUTOOPEN,
    gpszRepairNameAUTOCLOSE,
    gpszRepairNameAPPDER,
    NULL
};


//********************************************************************
//********************************************************************
// Array of Word Macro Virus Repair Information
//********************************************************************
//********************************************************************

MACRO_REPAIR_T gMacroVirusRepairInfo[] =
{
    VID_COLORS,     gpszMacroRepairColors,
    VID_CONCEPT0,   gpszMacroRepairConcept0,
    VID_CONCEPT1,   gpszMacroRepairConcept1,
    VID_DMV,        gpszMacroRepairDMV,
    VID_FORMATC,    gpszMacroRepairFormatC,
    VID_NUCLEAR,    gpszMacroRepairNuclear,
    VID_HOT0,       gpszMacroRepairHot0,
    VID_HOT1,       gpszMacroRepairHot1,
    VID_IMP0,       gpszMacroRepairImp0,                // Doc
    VID_IMP1,       gpszMacroRepairImp1,                // Normal.dot
    VID_IRISH0,     gpszMacroRepairIrish0,              // normal.dot
    VID_IRISH1,     gpszMacroRepairIrish1,              // doc
    VID_CONCEPTF,   gpszMacroRepairConceptF,
    VID_WAZZU,      gpszMacroRepairFormatC,             // Same as FormatC
    VID_WAZZU2,     gpszMacroRepairFormatC,
    VID_WAZZU3,     gpszMacroRepairFormatC,
    VID_WAZZU4,     gpszMacroRepairFormatC,
    VID_WAZZU5,     gpszMacroRepairFormatC,
    VID_WAZZU6,     gpszMacroRepairFormatC,
    VID_INFEZIONE,  gpszMacroRepairFormatC,             // Same as Wazzu - jjw
    VID_XENIXOS,    gpszMacroRepairXenixos,
    VID_XENIXOS1,   gpszMacroRepairXenixos1,
    VID_BOOM,       gpszMacroRepairBoom,
    VID_NOP1,       gpszMacroRepairNOP1,                // Normal.dot
    VID_NOP2,       gpszMacroRepairNOP2,                // Docs
    VID_PARASITE1,  gpszMacroRepairParasite1,           // Normal.dot
    VID_PARASITE0,  gpszMacroRepairParasite0,           // Docs
    VID_MDMADMV,    gpszMacroRepairDMV,                 // Normal.dot
    VID_ATOM,       gpszMacroRepairATOM,
    VID_ATOMB,      gpszMacroRepairATOM,
    VID_POLITE,     gpszMacroRepairPolite,
    VID_MADDOG,     gpszMacroRepairMADDOG,              // Normal.dot & Doc.
    VID_NUCLEARB,   gpszMacroRepairNuclear_b,
    VID_PHANTOM,    gpszMacroRepairFormatC,             // Normal.dot & Doc.
    VID_FRIENDLY,   gpszMacroRepairFriendly,
    VID_COLOR_B,    gpszMacroRepairColors,              // Same as Colors
    VID_DIVINA,     gpszMacroRepairDMV,
    VID_DIVINAB,    gpszMacroRepairDMV,
    VID_TELEFONICA, gpszMacroRepairTelefonica,
    VID_MICROSLOTH, gpszMacroRepairFormatC,
    VID_JAKARTA,    gpszMacroRepairFormatC,
    VID_JAKARTAB,   gpszMacroRepairFormatC,
    VID_BOSCO,      gpszMacroRepairFormatC,
    VID_BURONEU0,   gpszMacroRepairBuroNeu0,
    VID_BURONEU1,   gpszMacroRepairBuroNeu1,
    VID_CONCEPTH0,  gpszMacroRepairConceptH0,           // doc
    VID_CONCEPTH1,  gpszMacroRepairConceptH1,           // normal.dot
    VID_FISHFOOD,   gpszMacroRepairFishfood,
    VID_BOGUS0,     gpszMacroRepairBogus0,              // normal.dot
    VID_BOGUS1,     gpszMacroRepairBogus1,              // documents
    VID_INDONESIA,  gpszMacroRepairIndo,
    VID_ALLIANCE,   gpszMacroRepairAlliance,
    VID_EASYMAN,    gpszMacroRepairFormatC,
    VID_SAVER,      gpszMacroRepairSaver,
    VID_SPOOKY,     gpszMacroRepairSpooky,
    VID_BIRTHDAY,   gpszMacroRepairBirthday,
    VID_DOGGIE,     gpszMacroRepairDoggie,
    VID_KILLDLL,    gpszMacroRepairFormatC,
    VID_CONCEPTC,   gpszMacroRepairConceptC,
    VID_CONCEPTD,   gpszMacroRepairConceptD,
    VID_CONCEPTE,   gpszMacroRepairConceptE,
    VID_CONCEPTHCR, gpszMacroRepairConceptHcr,
    VID_TERBARU0,   gpszMacroRepairTerbaru0,            // normal.dot
    VID_TERBARU1,   gpszMacroRepairTerbaru1,            // documents
    VID_CLOCK,      gpszMacroRepairClock,
    VID_WAZZUB,     gpszMacroRepairFormatC,
    VID_BANDUNG,    gpszMacroRepairIndo,
    VID_TEDIOUS,    gpszMacroRepairTedious,
    VID_CONCEPTB,   gpszMacroRepairConceptB,
    VID_PARASITE4,  gpszMacroRepairParasite0,           // Normal.dot
    VID_PARASITE3,  gpszMacroRepairParasite1,           // Docs
    VID_CONCEPTI,   gpszMacroRepairConceptI,
    VID_NOPB0,      gpszMacroRepairNOP1,                // Docs
    VID_NOPB1,      gpszMacroRepairNOP2,                // Normal.dot
    VID_REFLEX0,    gpszMacroRepairReflex0,             // Docs
    VID_REFLEX1,    gpszMacroRepairReflex1,             // Normal.dot
    VID_SATANIC,    gpszMacroRepairSatanic,
    VID_CONCEPTJ,   gpszMacroRepairDMV,
    VID_CONCEPTK,   gpszMacroRepairConceptK,
    VID_LUNCHB,     gpszMacroRepairConceptK,
    VID_COLORSC,    gpszMacroRepairColors,
    VID_ALIEN,      gpszMacroRepairAlien,
    VID_RAPI,       gpszMacroRepairRapi,
    VID_HELPER,     gpszMacroRepairDMV,
    VID_SWITCHES,   gpszMacroRepairXenixos1,
    VID_BIGDADDY,   gpszMacroRepairBigDaddy,
    VID_NUKEDAM,    gpszMacroRepairNukeDam,
    VID_NPAD,       gpszMacroRepairFormatC,
    VID_STRYX,      gpszMacroRepairStryx,               // Docs
    VID_LOOKC,      gpszMacroRepairFishfood,
    VID_WEATHERB,   gpszMacroRepairWeatherB,
    VID_THEATRE,    gpszMacroRepairTheatre,
    VID_TWNOC1,     gpszMacroRepairFormatC,             // Docs
    VID_TWNOC2,     gpszMacroRepairTwnoC2,              // Normal.dot
    VID_TWNOB1,     gpszMacroRepairTwnoB1,              // Docs
    VID_TWNOB2,     gpszMacroRepairTwnoB2,              // Normal.dot
    VID_STRYX2,     gpszMacroRepairStryx2,              // Normal.dot
    VID_TAIWAN1,    gpszMacroRepairTwnoC2,
    VID_ANTICON,    gpszMacroRepairAntiCon,
    VID_NPADC,      gpszMacroRepairFormatC,
    VID_TWNOD,      gpszMacroRepairTwnoC2,
    VID_ALIENB,     gpszMacroRepairAlien,
    VID_CONCEPTT,   gpszMacroRepairConceptT,
    VID_SUTRA,      gpszMacroRepairSutra,
    VID_NPADE,      gpszMacroRepairFormatC,
    VID_DIVINAC,    gpszMacroRepairDMV,
    VID_NUCLEARE,   gpszMacroRepairNuclearE,
    VID_MDMAC,      gpszMacroRepairDMV,
    VID_WAZZUY,     gpszMacroRepairFormatC,
    VID_JOHNNY0,    gpszMacroRepairJohnny0,
    VID_APPDER,     gpszMacroRepairAppder,
    VID_NPADH,      gpszMacroRepairFormatC,
    VID_WAZZUX,     gpszMacroRepairFormatC,
    VID_NPADJ,      gpszMacroRepairFormatC
};

WORD gwNumMacroRepairDefs =
    (sizeof(gMacroVirusRepairInfo)/sizeof(MACRO_REPAIR_T));

#endif  // #ifndef SYM_NLM
