//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/mcrscndf.cpv   1.108   08 Jan 1997 20:22:40   DANG  $
//
// Description:
//      Contains WordDocument macro scan definitions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/mcrscndf.cpv  $
// 
//    Rev 1.108   08 Jan 1997 20:22:40   DANG
// Pulled NPad.K since NPad.J detects it.  New engine will have new def
// 
//    Rev 1.107   08 Jan 1997 18:01:10   DANG
// Changed Bandung.G to NPAD.K and fixed signature
// 
//    Rev 1.106   08 Jan 1997 16:46:16   DANG
// Added Bandung.G def 1ED1.
// 
//    Rev 1.105   03 Jan 1997 18:16:10   AOONWAL
// Added WM.Npad.J
// 
//    Rev 1.104   03 Jan 1997 12:35:50   RELNITI
// Add WM.Wazzu.X detection, VID = 0x1e16, Customer Issue #10496
// 
//    Rev 1.103   26 Dec 1996 14:59:04   AOONWAL
// Modified during JAN97 build
// 
//    Rev JAN97   19 Dec 1996 20:26:04   CFORMUL
// Added Npad.H
// 
// 
//    Rev 1.102   13 Dec 1996 17:33:30   AOONWAL
// Added detection for WM.Appder
// 
//    Rev 1.101   12 Dec 1996 17:36:02   CRENERT
// Commented out a comment.
// 
// 
//    Rev 1.100   12 Dec 1996 16:00:06   CRENERT
// Uncommented Johnny.
// 
// 
//    Rev 1.99   12 Dec 1996 13:10:02   CFORMUL
// Added WM.Wazzu.Y
// 
// 
//    Rev 1.98   10 Dec 1996 15:57:58   CFORMUL
// Added MDMA.C
// 
// 
//    Rev 1.97   09 Dec 1996 18:36:24   CFORMUL
// Added Nuclear.E
// 
// 
//    Rev 1.96   06 Dec 1996 19:35:10   RELNITI
// removing WM.Johnny temporarily
// 
// 
//    Rev 1.95   06 Dec 1996 19:20:40   RELNITI
// add detection string for WM.Johnny (0x1e0b)
// 
// 
//    Rev 1.94   05 Dec 1996 16:01:04   CRENERT
// Removed NiceDay
// 
// 
//    Rev 1.93   05 Dec 1996 15:54:12   CRENERT
// Added NiceDay Macro.
// 
// 
//    Rev 1.92   05 Dec 1996 15:19:32   JWILBER
// Added detection for Divina.C, 1e0e.
//
//    Rev 1.91   05 Dec 1996 14:10:26   CFORMUL
// Added Npad.E
//
//    Rev 1.90   02 Dec 1996 19:02:02   AOONWAL
// Added detection for WM.Sutra
//
//    Rev 1.89   02 Dec 1996 17:21:42   AOONWAL
// Changed detection for WM.Concept.T to include Normal.dot infections
//
//    Rev 1.88   02 Dec 1996 16:25:36   AOONWAL
// Added detection for 0x1e06
//
//    Rev 1.87   02 Dec 1996 13:45:26   AOONWAL
// DEC96 update
//
//    Rev 1.89   25 Nov 1996 19:46:00   AOONWAL
// Added detection for "WM.Alien.B
//
//    Rev 1.88   22 Nov 1996 15:50:18   JWILBER
// Added detection for Twno.D, 1fff.
//
//    Rev 1.87   20 Nov 1996 11:42:56   AOONWAL
// No change.
//
//    Rev 1.86   19 Nov 1996 13:08:46   CFORMUL
// Added WM.Npad.C
//
//    Rev 1.85   15 Nov 1996 23:39:24   JWILBER
// Added detection for WM.AntiConcept.
//
//    Rev 1.84   14 Nov 1996 17:44:42   JWILBER
// Added detection for Stryx in Normal.dot
//
//    Rev 1.83   14 Nov 1996 11:58:10   CFORMUL
// Added Lunch.B
//
//    Rev 1.82   13 Nov 1996 02:23:52   JWILBER
// Added detections for Look.C, Weather.B, Theatre, and Twno.C & B.
//
//    Rev 1.81   12 Nov 1996 13:40:16   JWILBER
// Added detection for Stryx, and Outlaw A & B, 1fef-f3.
//
//    Rev 1.80   08 Nov 1996 18:32:22   JWILBER
// Fixed Jakarta.A.Damaged detection.
//
//    Rev 1.79   07 Nov 1996 19:50:32   JWILBER
// Changed detection of Jakarta.A.Damaged (formerly Npad) to
// not detect undamaged Jarkarta.A samples.
//
//    Rev 1.78   07 Nov 1996 19:25:34   JWILBER
// Added detection for Nuclear.A.Damaged and Npad.
//
//    Rev 1.77   05 Nov 1996 16:31:44   JWILBER
// Added detection for BigDaddy, 1fea.
//
//    Rev 1.76   05 Nov 1996 13:00:48   JWILBER
// Added detection for Helper, 1fe8.
//
//    Rev 1.75   04 Nov 1996 19:38:02   CFORMUL
// Added WM.Switches
//
//    Rev 1.74   04 Nov 1996 14:41:52   CFORMUL
// Added WM.Rapi
//
//    Rev 1.73   31 Oct 1996 17:12:08   CFORMUL
// Added WM.Alien
//
//    Rev 1.72   31 Oct 1996 16:00:58   CFORMUL
// Added WM.Colors.C
//
//    Rev 1.71   30 Oct 1996 17:08:28   CFORMUL
// Added WM.Concept.K
//
//    Rev 1.70   29 Oct 1996 14:34:58   CFORMUL
// Added WM.Wazzu.F
//
//    Rev 1.69   29 Oct 1996 12:56:52   AOONWAL
// Modified during NOV96 freeze
//
//    Rev 1.72   25 Oct 1996 12:24:38   CFORMUL
// Added WM.Wazzu.E
//
//    Rev 1.71   24 Oct 1996 15:01:36   CFORMUL
// Added WM.Divina.B
//
//    Rev 1.70   24 Oct 1996 14:00:24   CFORMUL
// Added WM.Wazzu.D
//
//    Rev 1.69   21 Oct 1996 17:02:24   CFORMUL
// Added detection for WM.Jakarta.b
//
//    Rev 1.68   18 Oct 1996 18:16:10   AOONWAL
// Added detection for "WM.Concept.J"
//
//    Rev 1.67   17 Oct 1996 17:44:52   AOONWAL
// Added detection for "WM.Taiwan1"
//
//    Rev 1.66   16 Oct 1996 16:13:14   JWILBER
// Switched order of Reflex DOT and DOC defs in pointer table at
// end of file.  The macros in NORMAL.DOT are a superset of the
// ones in normal docs, necessitating this cheez.
//
//    Rev 1.65   16 Oct 1996 14:31:04   AOONWAL
// Added detection for "WM.Concept.I"
//
//    Rev 1.64   16 Oct 1996 13:58:12   AOONWAL
// Added detection for "WM.Concept.E (3)" and "WM.Concept.E (4)"
//
//    Rev 1.63   16 Oct 1996 13:34:28   CFORMUL
// Added Concept.b and Tedious
//
//    Rev 1.62   16 Oct 1996 13:32:02   JWILBER
// Added signatures for: NOP.B    1fd3/4
//                       Reflex   1fd5/6
//                       Satanic  1fdb
//
//    Rev 1.61   11 Oct 1996 15:22:28   AOONWAL
// Added signature for "WM.Bandung.b"
//
//    Rev 1.60   09 Oct 1996 17:52:24   JWILBER
// Added detection for Wazzu.B, 1fcf.
//
//    Rev 1.59   07 Oct 1996 13:14:58   CFORMUL
// Added detection for WM.Clock
//
//    Rev 1.58   02 Oct 1996 15:36:26   AOONWAL
// Added detection for "WM.Terbaru"
//
//    Rev 1.57   27 Sep 1996 14:25:42   CFORMUL
// Added detection for 8 new macro viruses
//
//    Rev 1.56   25 Sep 1996 16:00:04   CFORMUL
// Added 4 new macro viruses: Alliance, EasyMan, Saver, Spooky
//
//    Rev 1.55   24 Sep 1996 18:02:14   AOONWAL
// Added detection for Wazzu (3)
//
//    Rev 1.54   13 Sep 1996 15:59:06   JWILBER
// Added detection for Indonesia, 1fbe.
//
//    Rev 1.53   10 Sep 1996 14:07:06   DANG
// Added Bogus macro virus
//
//    Rev 1.52   09 Sep 1996 12:43:36   CFORMUL
// Changed VID for mac infections of Xenixos
//
//    Rev 1.51   04 Sep 1996 14:45:24   SCOURSE
// Changed event
//
//    Rev 1.50   04 Sep 1996 14:24:50   SCOURSE
// Added comma, and removed comma
//
//    Rev 1.49   04 Sep 1996 14:23:26   SCOURSE
// Added comma
//
//    Rev 1.48   04 Sep 1996 14:21:32   SCOURSE
// Added entry for Goldfish
//
//    Rev 1.47   29 Aug 1996 17:35:56   DANG
// Fixed missing comma error.
//
//    Rev 1.46   29 Aug 1996 16:07:48   DANG
// Added Concept.H
//
//    Rev 1.45   29 Aug 1996 15:20:34   CFORMUL
// Added Strings for BuroNeu German Macro virus
//
//    Rev 1.44   28 Aug 1996 17:23:16   CFORMUL
// Added string for Bosco
//
//    Rev 1.43   19 Aug 1996 19:27:10   JWILBER
// Added detection for Jakarta, 1fb3.
//
//    Rev 1.42   16 Aug 1996 12:19:04   CFORMUL
// fixed bug in wazzu variant
//
//    Rev 1.41   16 Aug 1996 12:07:54   CFORMUL
// Added string for variant of Wazzu that did not include the Payload
// sub-routine.
//
//    Rev 1.40   15 Aug 1996 20:56:28   AOONWAL
// Added detection for Microsloth
//
//    Rev 1.39   15 Aug 1996 19:14:48   AOONWAL
// Added detection for Telefonica
//
//    Rev 1.38   13 Aug 1996 12:22:36   CFORMUL
// Uncommented string for mac infections of Xenixos
//
//    Rev 1.37   29 Jul 1996 17:56:40   SCOURSE
// Added entry for Divina
//
//    Rev 1.36   29 Jul 1996 14:49:26   LNGUYEN
// Replaced "Macro" to "FileNew." for Colors.
//
//    Rev 1.35   29 Jul 1996 14:44:22   LNGUYEN
// No change.
//
//    Rev 1.34   29 Jul 1996 14:40:40   LNGUYEN
// Replaced Colors with the new String. to seperated colors and Color_B det.
//
//    Rev 1.33   29 Jul 1996 14:26:04   LNGUYEN
// Added new string for Color_B. 1fab.
//
//    Rev 1.32   25 Jul 1996 18:53:28   JWILBER
// Added def for Friendly, 1faa.
//
//    Rev 1.31   25 Jul 1996 11:49:14   LNGUYEN
// Fixed for both MadDog and Phantom. "AutoOpen"
//
//    Rev 1.30   24 Jul 1996 15:49:02   LNGUYEN
// Changed entry point for Both MadDog and Phantom
//
//    Rev 1.29   24 Jul 1996 10:50:36   LNGUYEN
// Added new def. for Phantom
//
//    Rev 1.28   23 Jul 1996 20:20:12   LNGUYEN
// Correction for MadDog. deleted a line of text.
//
//    Rev 1.27   23 Jul 1996 20:09:36   AOONWAL
// Added detection for "Nuclear.b"
//
//    Rev 1.26   23 Jul 1996 19:35:38   LNGUYEN
// Restored the Def. for MadDog.
//
//    Rev 1.25   23 Jul 1996 19:00:06   CFORMUL
// Added detection for Polite macro
//
//    Rev 1.24   17 Jul 1996 17:56:14   CRENERT
// Reverted back to July 12th version.
//
//    Rev 1.22   12 Jul 1996 17:01:02   CFORMUL
// Commented out the mac stuff for Xenixos so they can deal with it themselves
//
//    Rev 1.21   12 Jul 1996 16:54:10   CFORMUL
// Added another string for Xenixos so it will pick up infections on
// Macs.
//
//    Rev 1.20   12 Jul 1996 13:59:44   JWILBER
// Added def for Atom, 1fa4.
//
//    Rev 1.19   11 Jul 1996 16:37:22   LNGUYEN
// Added Vid for MDMADMV
//
//    Rev 1.18   11 Jul 1996 13:56:20   JWILBER
// Added defs for NOP, VIDs 1f9c/a1.
//
//    Rev 1.17   11 Jul 1996 12:53:52   AOONWAL
// Modified parasite
//
//    Rev 1.16   11 Jul 1996 12:45:46   AOONWAL
// Modified Parasite
//
//    Rev 1.15   10 Jul 1996 18:25:50   AOONWAL
// Modified Parasite
//
//    Rev 1.14   10 Jul 1996 17:23:16   AOONWAL
// Added Parasite macro virus
//
//    Rev 1.13   28 Jun 1996 14:06:14   CFORMUL
// Added detections for Xenixos and Boom
//
//    Rev 1.12   15 May 1996 16:58:40   JALLEE
// Changed order of scanning of two nuclear sigs, damaged first, so repair
// will work.
//
//    Rev 1.11   14 May 1996 17:26:18   JALLEE
// Added second sig for Nuclear.  Customer 2967 has infectious sample with
// corruption over the old sig.
//
//    Rev 1.10   09 May 1996 17:17:56   JWILBER
// Added def for Infezione/Italian Macro virus, VID 1f8a.
//
//    Rev 1.9   07 May 1996 17:16:36   JALLEE
// Added Irish macro virus
//
//    Rev 1.8   29 Apr 1996 19:50:40   JWILBER
// Added detection for Wazzu Macro virus.
//
//    Rev 1.7   10 Apr 1996 12:15:02   JWILBER
// Removed detection for French Concept in DOCs, changed #defines
// to reflect only one def now for French Concept.
//
//    Rev 1.6   09 Apr 1996 13:56:04   JALLEE
// Added Imposter macro virus
//
//    Rev 1.5   08 Apr 1996 18:20:22   JWILBER
// Fixed up repair for French Concept in NORMAL.DOT file.
//
//    Rev 1.4   08 Apr 1996 18:17:08   JWILBER
// Added def info for French Concept in NORMAL.DOT file.
//
//    Rev 1.3   08 Apr 1996 16:10:00   JWILBER
// Added def for French Concept, 1f79.
//
//    Rev 1.2   15 Feb 1996 16:18:58   JALLEE
// Added Hot macro virus detection.
//
//    Rev 1.1   06 Feb 1996 16:53:32   DCHI
// Correction of CONCEPT def to account for NORMAL.DOT.
//
//    Rev 1.0   03 Jan 1996 17:14:56   DCHI
// Initial revision.
//
//************************************************************************

#include "mcrscndf.h"

#include "navexshr.h"

//
// Notes:
//
// 1. Signatures shouldn't ever begin at offset 0xFFFF
// 2. The signature string should be an already decrypted string
//      (which means that if the macro is encrypted, you must
//      decrypt it first before you extract a string from it!)
//

//********************************************************************
// Colors
//********************************************************************

BYTE gpbySigColorsMacros[] =
{
    0x88, 0x00,                                     // Offset in macro
    0x10,                                           // Length of string
    0x73, 0x9B, 0x00, 0x0C, 0x6C, 0x00, 0x00, 0x1E,
    0x69, 0x03, 0x64, 0x6C, 0x67, 0x73, 0x9B, 0x00,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigColors[] =
{
    (BYTE LPF *)"FILENEW", gpbySigColorsMacros,      // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Concept
//********************************************************************

BYTE gpbySigConceptAAAZFS[] =
{
    0x15, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string

    0x07, 0x3A, 0x41, 0x41, 0x41, 0x5A, 0x46, 0x53, // String
    0x64, 0x67, 0xC2, 0x80, 0x6A, 0x0D, 0x47,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigConcept0[] =
{
    (BYTE LPF *)"AAAZFS", gpbySigConceptAAAZFS,     // Sub-signature
    NULL, NULL                                      // No more subsigs
};

WORD_MACRO_SIG_T gpwmsSigConcept1[] =
{
    (BYTE LPF *)"FILESAVEAS", gpbySigConceptAAAZFS, // Sub-signature
    NULL, NULL                                      // No more subsigs
};

// French Concept

// DOC file detection is the same in all languages
// NORMAL.DOT file detection varies because FileSaveAs is translated

BYTE gpbySigConceptF[] =
{
    0xa1, 0x00,                                     // Offset in macro
    0x0f,                                           // Length of string
    0x41, 0x75, 0x74, 0x6f, 0x4f, 0x70, 0x65, 0x6e,
    0x64, 0x67, 0xc2, 0x80, 0x6a, 0x0d, 0x47,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigConF[] =
{
    (BYTE LPF *)"FICHIERENREGISTRERSOUS", gpbySigConceptF,  // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// DMV
//********************************************************************

BYTE gpbySigDMVAutoClose[] =
{
    0x93, 0x06,                                     // Offset in macro
    0x0F,                                           // Length of string
    'l', 'o', 's', 'e', 'd', 0x52, 0x67, 0x2B,      // String
    0x80, 0x6A, 0x37, 0x49, 0x6E, 0x66, 0x65,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigDMV[] =
{
    (BYTE LPF *)"AUTOCLOSE", gpbySigDMVAutoClose,   // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// FormatC
//********************************************************************

BYTE gpbySigFormatCAutoOpen[] =
{
    0x1D, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string
    'f', 'o', 'r', 'm', 'a', 't', ' ', 'c',         // String
    ':', ' ', '/', 'u', 0x64, 0x52, 0x67,
    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigFormatC[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigFormatCAutoOpen, // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Nuclear
//********************************************************************

BYTE gpbySigNuclearDropSuriv[] =
{
    0x08, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x4E, 0x20, 0x50, 0x48, 0x33, 0x33, 0x52, 0x2E, // String
    0x43, 0x4F, 0x4D, 0x64, 0x36, 0x71, 0x6C,

    0xFF, 0xFF                                      // No more
};

BYTE gpbySigNuclearAuto[] =
{
    0x5e, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x80, 0x69, 0x02, 0x41, 0x24, 0x07, 0x6a, 0x09,
    0x3a, 0x41, 0x75, 0x74, 0x6f, 0x4f, 0x70,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigNuclear[] =
{
    (BYTE LPF *)"DROPSURIV", gpbySigNuclearDropSuriv,   // Sub-signature
    NULL, NULL                                      // No more subsigs
};

                                                    // Second sig added
                                                    // for enhanced check.
WORD_MACRO_SIG_T gpwmsSigNuclearB[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigNuclearAuto,     // Sub-signature
    NULL, NULL                                      // No more subsigs
};


//********************************************************************
// Hot
//********************************************************************

BYTE gpbySigHotStartOfDoc[] =
{
    0x60, 0x02,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x6e, 0x0f, 0x6a, 0x05, 0x51, 0x4c, 0x48, 0x6f,
    0x74, 0x12, 0x69, 0x08, 0x57, 0x69, 0x6e,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigHot0[] =
{
    (BYTE LPF *)"STARTOFDOC", gpbySigHotStartOfDoc, // Sub-signature
    NULL, NULL                                      // No more subsigs
};

WORD_MACRO_SIG_T gpwmsSigHot1[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigHotStartOfDoc,   // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Imposter
//********************************************************************

BYTE gpbySigImpStartOfDoc[] =
{
    0xcd, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string

    0x80, 0x6a, 0x0a, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x3a,
    0x44, 0x4d, 0x56, 0x12, 0x69,


    0xFF, 0xFF                                      // No more
};
                                                    // Doc
WORD_MACRO_SIG_T gpwmsSigImp0[] =
{
    (BYTE LPF *)"DMV", gpbySigImpStartOfDoc,        // Sub-signature
    NULL, NULL                                      // No more subsigs
};
                                                    // Normal.dot
WORD_MACRO_SIG_T gpwmsSigImp1[] =
{
    (BYTE LPF *)"FILESAVEAS", gpbySigImpStartOfDoc, // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Irish
//********************************************************************

BYTE gpbySigIrish[] =
{
    0x89, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string

    0x26, 0x69, 0x01, 0x69, 0x64, 0x52, 0x69, 0x08, 0x44, 0x6f,
    0x63, 0x4e, 0x61, 0x6d, 0x65,


    0xFF, 0xFF                                      // No more
};
                                                    // Doc
WORD_MACRO_SIG_T gpwmsSigIrish0[] =
{
    (BYTE LPF *)"FILESAVE", gpbySigIrish,        // Sub-signature
    NULL, NULL                                      // No more subsigs
};
                                                    // Normal.dot
WORD_MACRO_SIG_T gpwmsSigIrish1[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigIrish, // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Wazzu
//********************************************************************

BYTE gpbySigWazStartOfDoc[] =
{
    0xb4, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x69, 0x07, 0x52, 0x6e, 0x64, 0x57, 0x6f, 0x72,
    0x64, 0x64, 0x6f, 0x03, 0x67, 0x12, 0x80,

    0xff, 0xff                                      // No more
};

BYTE gpbySigWazStartOfDoc2[] =
{
    0x04, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x67, 0xc2, 0x80, 0x69, 0x0a, 0x66, 0x69, 0x6c,
    0x65, 0x4d, 0x61, 0x63, 0x72, 0x6f, 0x24,

    0xff, 0xff                                      // No more
};

BYTE gpbySigWazStartOfDoc3[] =
{
    0x10, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x67, 0x6c, 0x6f, 0x62, 0x4d, 0x61, 0x63, 0x72,
    0x6f, 0x24, 0x64, 0x52, 0x1a, 0x1d, 0x64,

    0xff, 0xff                                      // No more
};

BYTE gpbySigWazStartOfDocB[] =
{
    0x7e, 0x00,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x0c, 0x6a, 0x0f, 0x47, 0x6c, 0x6f, 0x62, 0x61,
    0x6c, 0x3a, 0x61, 0x75, 0x74, 0x6f, 0x4f,

    0xff, 0xff
};

BYTE gpbySigWazStartOfDocD[] =
{
    0xb3, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x69, 0x07, 0x52, 0x6e, 0x64, 0x57, 0x6f, 0x72,
    0x64, 0x64, 0x6f, 0x03, 0x67, 0x12, 0x80,

    0xff, 0xff                                      // No more
};

BYTE gpbySigWazStartOfDocE[] =
{
    0x08, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x67, 0x6c, 0x6f, 0x62, 0x4d, 0x61, 0x63, 0x72,
    0x6f, 0x24, 0x64, 0x52, 0x1a, 0x1d, 0x64,

    0xff, 0xff                                      // No more
};

BYTE gpbySigWazStartOfDocF[] =
{
    0x98, 0x00,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x67, 0x6c, 0x6f, 0x62, 0x4d, 0x61, 0x63, 0x72,
    0x6f, 0x24, 0x0c, 0x6a, 0x0f, 0x47, 0x6c,

    0xff, 0xff                                      // No more
};

BYTE gpbySigWazzuX[] =
{
    0x0e, 0x00,                                     // Offset in macro
    0x14,                                           // Length of string

    0x2a, 0x69, 0x04, 0x6a, 0x61, 0x6e, 0x65, 0x64,
    0x64, 0x52, 0x6b, 0x33, 0x54, 0x68, 0x65, 0x20,
    0x4d, 0x65, 0x61, 0x74,

    0xff, 0xff
};

BYTE gpbySigWazzuY[] =
{
    0xbf, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x69, 0x07, 0x52, 0x6e, 0x64, 0x57, 0x6f, 0x72,
    0x64, 0x64, 0x6e, 0x18, 0x67, 0x12, 0x80,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigWaz[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazStartOfDoc,  // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigWaz2[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazStartOfDoc2, // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigWaz3[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazStartOfDoc3, // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigWazB[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazStartOfDocB, // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigWazD[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazStartOfDocD, // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigWazE[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazStartOfDocE, // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigWazF[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazStartOfDocF, // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigWazzuX[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazzuX,  // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigWazzuY[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWazzuY,  // Sub-signature
    NULL, NULL
};

//********************************************************************
// Infezione
//********************************************************************

BYTE gpbySigInfezStartOfDoc[] =
{
    0x90, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string

    0x08, 0x41, 0x75, 0x74, 0x6f, 0x4f, 0x70, 0x65,
    0x6e, 0x06, 0x0d, 0x69, 0x0c, 0x66, 0x69,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigInfez[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigInfezStartOfDoc,    // Sub-signature
    NULL, NULL
};

//********************************************************************
// Xenixos
//********************************************************************


BYTE gpbySigXenixosDateiS[] =
{
    0xF0, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x50, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64, // String
    0x24, 0x0c, 0x67, 0x05, 0x80, 0x05, 0x6c,

    0xFF, 0xFF                                      // No more
};

// Here's the string that will work for Macs

BYTE gpbySigXenixosAutoExec[] =
{
    0x99, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x50, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64, // String
    0x24, 0x0c, 0x67, 0x05, 0x80, 0x05, 0x6c,

    0xFF, 0xFF                                      // No more
};


WORD_MACRO_SIG_T gpwmsSigXenixos[] =
{
    (BYTE LPF *)"DATEISPEICHERN", gpbySigXenixosDateiS,     // Sub-signature
    NULL, NULL                                              // No more subsigs
};



WORD_MACRO_SIG_T gpwmsSigXenixosMacWeenie[] =
{
    (BYTE LPF *)"AUTOEXEC", gpbySigXenixosAutoExec,         // Sub-signature
    NULL, NULL                                              // No more subsigs
};

//********************************************************************
// Boom
//********************************************************************

BYTE gpbySigBoomSystem[] =
{
    0x0B, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x61, 0x74, 0x65, 0x69, 0x12, 0x6a, 0x0e, 0x4d, // String
    0x72, 0x2e, 0x20, 0x42, 0x6f, 0x6f, 0x6d,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigBoom[] =
{
    (BYTE LPF *)"SYSTEM", gpbySigBoomSystem,        // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Parasite / Concept.E
//********************************************************************

BYTE gpbySigParasite0FILESAVEAS[] =
{
    0x79, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x47, 0x6C, 0x6F, 0x62, 0x61, 0x6C, 0x3A, 0x50,
    0x41, 0x52, 0x41, 0x12, 0x69, 0x08, 0x73,

    0xFF, 0xFF                                      // No more
};

BYTE gpbySigParasite1A678[] =
{
    0x9B, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x47, 0x6C, 0x6F, 0x62, 0x61, 0x6C, 0x3A, 0x50,
    0x41, 0x52, 0x41, 0x12, 0x69, 0x08, 0x73,

    0xFF, 0xFF                                      // No more
};

BYTE gpbySigPara4FILESAVEAS[] =
{
    0xAF, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x47, 0x6C, 0x6F, 0x62, 0x61, 0x6C, 0x3A, 0x50,
    0x41, 0x52, 0x41, 0x12, 0x69, 0x08, 0x73,

    0xFF, 0xFF                                      // No more
};

BYTE gpbySigPara3A678[] =
{
    0x5E, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x47, 0x6C, 0x6F, 0x62, 0x61, 0x6C, 0x3A, 0x50,
    0x41, 0x52, 0x41, 0x12, 0x69, 0x08, 0x73,

    0xFF, 0xFF                                      // No more
};


WORD_MACRO_SIG_T gpwmsSigParasite0[] =
{
    (BYTE LPF *)"FILESAVEAS", gpbySigParasite0FILESAVEAS, // Sub-signature
    NULL, NULL                                        // No more subsigs
};

WORD_MACRO_SIG_T gpwmsSigParasite1[] =
{
    (BYTE LPF *)"A678", gpbySigParasite1A678,         // Sub-signature
    NULL, NULL                                        // No more subsigs
};

WORD_MACRO_SIG_T gpwmsSigPara4[] =
{
    (BYTE LPF *)"FILESAVEAS", gpbySigPara4FILESAVEAS, // Sub-signature
    NULL, NULL                                        // No more subsigs
};

WORD_MACRO_SIG_T gpwmsSigPara3[] =
{
    (BYTE LPF *)"A678", gpbySigPara3A678,             // Sub-signature
    NULL, NULL                                        // No more subsigs
};

//********************************************************************
// NOP
//********************************************************************

BYTE gpbySigNOP[] =
{
    0x4a, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string
    0xc2, 0x80, 0x6a, 0x15, 0x47, 0x6c, 0x6f, 0x62,
    0x61, 0x6c, 0x3a, 0x44, 0x61, 0x74, 0x65,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigNOP1[] =                   // NORMAL.DOT
{
    (BYTE LPF *) "DATEISPEICHERN", gpbySigNOP,      // Sub-signature
    NULL, NULL                                      // No more subsigs
};

WORD_MACRO_SIG_T gpwmsSigNOP2[] =                   // Other Docs
{
    (BYTE LPF *) "NOP", gpbySigNOP,                 // Sub-signature
    NULL, NULL                                      // No more subsigs
};

// NOP.B

BYTE gpbySigNOPB[] =
{
    0x3d, 0x00,                                     // Offset in macro
    0x10,                                           // Length of string
    0x41, 0x75, 0x74, 0x6f, 0x4f, 0x70, 0x65, 0x6e,
    0x64, 0x67, 0xc2, 0x80, 0x6a, 0x0a, 0x47, 0x6c,

    0xff, 0xff                                      // No more
};

// These sigs could be combined with the other NOP sigs to save some
// time and space, but then all strains of NOP would be given the same
// name upon detection.  By splitting them up, we add a name to the virus
// list in NAV.

WORD_MACRO_SIG_T gpwmsSigNOPB0[] =                  // NORMAL.DOT
{
    (BYTE LPF *) "DATEISPEICHERN", gpbySigNOPB,     // Sub-signature
    NULL, NULL                                      // No more subsigs
};

WORD_MACRO_SIG_T gpwmsSigNOPB1[] =                  // Other Docs
{
    (BYTE LPF *) "NOP", gpbySigNOPB,                // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// MDMADMV
//********************************************************************

BYTE gpbySigMDMADMVAuto[] =
{
    0x3E, 0x02,                                     // Offset in macro
    0x10,
    0x47, 0x6C, 0x6F, 0x62, 0x61, 0x6C, 0x3A, 0x41,
    0x75, 0x74, 0x6F, 0x43, 0x6C, 0x6F, 0x73, 0x65, // Length of string

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigMDMADMV[] =
{
    (BYTE LPF *)"AUTOCLOSE", gpbySigMDMADMVAuto,    // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Atom
//********************************************************************

BYTE gpbySigAtom[] =
{
    0x40, 0x00,                                     // Offset in macro
    0x0f,                                           // Length of string
    0x2a, 0x2e, 0x2a, 0x64, 0x1a, 0x1d, 0x65, 0x09,
    0x4b, 0x69, 0x6c, 0x6c, 0x45, 0x72, 0x72,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigATOM[] =                   // NORMAL.DOT
{
    (BYTE LPF *) "ATOM", gpbySigAtom,               // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Polite
//********************************************************************

BYTE gpbySigPoliteFileSaveAs[] =
{
    0x64, 0x00,                                     // Offset in macro
    0x10,                                           // Length of string
    0x6f, 0x6e, 0x31, 0x0c, 0x67, 0x2b, 0x80, 0x05,
    0x6a, 0x19, 0x53, 0x68, 0x61, 0x6c, 0x6c, 0x20,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigPolite[] =
{
    (BYTE LPF *)"FILESAVEAS", gpbySigPoliteFileSaveAs,    // Sub-signature
    NULL, NULL                                            // No more subsigs
};
//********************************************************************
// MADDOG
//********************************************************************

BYTE gpbySigMADDOGAuto[] =
{
    0x6B, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x39, 0x35, 0x20, 0x22, 0x4d, 0x61, 0x64, 0x44,
    0x6f, 0x67, 0x22, 0x20, 0x4d, 0x61, 0x63, 0x72,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigMADDOG[] =
{
    (BYTE LPF *)"FILECLOSE", gpbySigMADDOGAuto,    // Sub-signature
    NULL, NULL                                     // No more subsigs
};

//********************************************************************
// Nuclear.b
//********************************************************************

BYTE gpbySigNuclearAutoopen[] =
{
    0x24, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string
    0x46, 0x69, 0x6c, 0x65, 0x50, 0x72, 0x69, 0x6e,
    0x74, 0x44, 0x65, 0x66, 0x61, 0x75, 0x6c,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigNuclear_b[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigNuclearAutoopen, // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// PHANTOM
//********************************************************************

BYTE gpbySigPHANTOMAuto[] =
{
    0x6A, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x61, 0x72, 0x74, 0x19, 0x64, 0x69, 0x01, 0x72,
    0x0C, 0x67, 0x02, 0x80, 0x05, 0x67, 0x0E, 0x80,                                      // Length of string

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigPHANTOM[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigPHANTOMAuto,    // Sub-signature
    NULL, NULL                                     // No more subsigs
};

//********************************************************************
// Friendly
//********************************************************************

BYTE gpbySigFriendly[] =
{
    0x31, 0x00,                                 // Offset in macro
    0x0f,                                       // # Bytes.
    0x79, 0x06, 0x64, 0x1d, 0x69, 0x02, 0x41, 0x24,
    0x0c, 0x6a, 0x02, 0x44, 0x4d, 0x1e, 0x2a,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigFriendly[] =
{
    (BYTE LPF *)"INFIZIEREN", gpbySigFriendly,      // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// COLOR_B
//********************************************************************

BYTE gpbySigCOLOR_BAuto[] =
{
    0x7E, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x6C, 0x67, 0x64, 0x6B, 0x30, 0x09, 0x49, 0x66,
    0x20, 0x64, 0x6C, 0x67, 0x2E, 0x4E, 0x65, 0x77,
 
    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigCOLOR_B[] =
{
    (BYTE LPF *)"FILENEW", gpbySigCOLOR_BAuto,    // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Divina
//********************************************************************

BYTE gpbySigDivinaAuto[] =
{
    0x20, 0x06,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x63, 0x6F, 0x6D, 0x70, 0x6C, 0x65, 0x61, 0x6E,
    0x6E, 0x6F, 0x20, 0x64, 0x69, 0x20, 0x44, 0x69,
 
    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigDivina[] =
{
    (BYTE LPF *)"AUTOCLOSE", gpbySigDivinaAuto,     // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Divina.B
//********************************************************************

BYTE gpbySigDivinaB[] =
{
    0x20, 0x06,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x63, 0x6F, 0x6D, 0x6d, 0x65, 0x6e, 0x64, 0x65,
    0x64, 0x2e, 0x07, 0x67, 0x05, 0x80, 0x05, 0x6c,
 
    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigDivinaB[] =
{
    (BYTE LPF *)"AUTOCLOSE", gpbySigDivinaB,     // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Telefonica
//********************************************************************

BYTE gpbySigTelefonicaAuto[] =
{
    0x33, 0x00,                                 // Offset in macro
    0x0F,                                       // # Bytes.
    0x6A, 0x09, 0x30, 0x78, 0x30, 0x30, 0x33, 0x30,
    0x33, 0x30, 0x33, 0x64, 0x69, 0x0C, 0x50,
 
    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigTelefonica[] =
{
    (BYTE LPF *)"AUTOEXEC", gpbySigTelefonicaAuto,  // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Microsloth
//********************************************************************

BYTE gpbySigMicroslothAuto[] =
{
    0xD4, 0x02,                                 // Offset in macro
    0x0F,                                       // # Bytes.
    0x64, 0x6F, 0x03, 0x23, 0x69, 0x01, 0x49, 0x0C,
    0x6C, 0x01, 0x00, 0x24, 0x6C, 0x14, 0x00,
 
    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigMicrosloth[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigMicroslothAuto,  // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Jakarta
//********************************************************************

BYTE gpbySigJakarta[] =
{
    0x31, 0x05,                                 // Offset in macro
    0x0f,                                       // # Bytes.
    0x06, 0x6a, 0x75, 0x6d, 0x6c, 0x61, 0x68, 0x12,
    0x69, 0x05, 0x77, 0x61, 0x6b, 0x74, 0x75,
 
    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigJakarta[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigJakarta,         // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Jakarta.b
//********************************************************************

BYTE gpbySigJakartaB[] =
{
    0x31, 0x05,                                 // Offset in macro
    0x0f,                                       // # Bytes.
    0xfa, 0xae, 0xfa, 0xef, 0xf6, 0xa7, 0xea, 0xe9,
    0xc1, 0x8e, 0xfd, 0xdc, 0x8e, 0x82, 0xe4,
 
    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigJakartaB[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigJakartaB,         // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Bosco
//********************************************************************

BYTE gpbySigBosco[] =
{
    0x7f, 0x01,                                 // Offset in macro
    0x13,                                       // # Bytes.
    0x64, 0x64, 0x6f, 0x03, 0x67, 0x2b, 0x80, 0x6a,
    0x1a, 0x54, 0x68, 0x69, 0x73, 0x20, 0x6f, 0x6e,
    0x65, 0x27, 0x73,
 
    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigBosco[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigBosco,         // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// BuroNeu
//********************************************************************

BYTE gpbySigBuroNeu0[] =
{
    0x31, 0x00,                                 // Offset in macro
    0x0f,                                       // # Bytes.
    0x4e, 0x65, 0x75, 0x64, 0x52, 0x67, 0xc2, 0x80,
    0x69, 0x02, 0x6d, 0x24, 0x12, 0x6a, 0x15,
 
    0xff, 0xff                                      // No more
};

BYTE gpbySigBuroNeu1[] =
{
    0xce, 0x01,                                 // Offset in macro
    0x0f,                                       // # Bytes.
    0x4e, 0x65, 0x75, 0x64, 0x52, 0x67, 0xc2, 0x80,
    0x6a, 0x15, 0x47, 0x6c, 0x6f, 0x62, 0x61,
 
    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigBuroNeu0[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigBuroNeu0,         // Sub-signature
    NULL, NULL                                      // No more subsigs
};

WORD_MACRO_SIG_T gpwmsSigBuroNeu1[] =
{
    (BYTE LPF *)"DATEISPEICHERN", gpbySigBuroNeu1,   // Sub-signature
    NULL, NULL                                      // No more subsigs
};


//********************************************************************
// Concept.H
//********************************************************************

BYTE gpbySigConceptH[] =
{
    0x15, 0x01,                                 // Offset in macro
    0x0F,                                       // # Bytes.
    0x24, 0x07, 0x6A, 0x08, 0x3A, 0x43, 0x49, 0x54,
    0x50, 0x59, 0x52, 0x43, 0x64, 0x67, 0xC2,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigConceptH0[] =
{
    (BYTE LPF *)"CITPYRC", gpbySigConceptH,         // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigConceptH1[] =
{
    (BYTE LPF *)"FILESAVEAS", gpbySigConceptH,      // Sub-signature
    NULL, NULL
};

//********************************************************************
// Fishfood
//********************************************************************

BYTE gpbySigFishfoodAuto[] =
{
    0xC2, 0x02,                                 // Offset in macro
    0x11,                                       // # Bytes.
    0x49, 0x20, 0x61, 0x6d, 0x20, 0x74, 0x68, 0x65,
    0x20, 0x47, 0x6f, 0x6c, 0x64, 0x46, 0x69, 0x73, 0x68,
 
    0xFF, 0xFF                                  // No more
};

WORD_MACRO_SIG_T gpwmsSigFishfood[] =
{
    (BYTE LPF *)"AUTOOPEN", gpbySigFishfoodAuto,   // Sub-signature
    NULL, NULL                                     // No more subsigs
};


//********************************************************************
// Bogus
//********************************************************************

BYTE gpbySigBogus[] =
{
    0x6A, 0x01,                                 // Offset in macro
    0x0F,                                       // # Bytes.
    0x44, 0x61, 0x74, 0x65, 0x0C, 0x67, 0xF0, 0x80,
    0x05, 0x6C, 0xCC, 0x07, 0x12, 0x6C, 0x01,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigBogus0[] =      // normal.dot
{
    (BYTE LPF *)"AUTOEXEC", gpbySigBogus,         // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigBogus1[] =      // document infections
{
    (BYTE LPF *)"MICROSOFT03", gpbySigBogus,      // Sub-signature
    NULL, NULL
};

//********************************************************************
// Indonesia
//********************************************************************

BYTE gpbySigIndo[] =
{
    0xe0, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x64, 0x69, 0x72, 0x73, 0x24, 0x05, 0x69, 0x01,
    0x69, 0x06, 0x0c, 0x67, 0xaf, 0x80, 0x05, 0x67,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigIndo[] =               // normal.dot
{
    (BYTE LPF *) "AUTOEXEC", gpbySigIndo,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Alliance
//********************************************************************

BYTE gpbySigAlliance[] =
{
    0x76, 0x00,                                 // Offset in macro
    0x12,                                       // # Bytes.
    0x41, 0x6c, 0x6c, 0x69, 0x61, 0x6e, 0x63, 0x65, 0x64,
    0x64, 0x67, 0xc2, 0x80, 0x69, 0x08, 0x4d, 0x61, 0x63,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigAlliance[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigAlliance,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// EasyMan
//********************************************************************

BYTE gpbySigEasyMan[] =
{
    0x77, 0x02,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x45, 0x61, 0x73, 0x79, 0x20, 0x4d, 0x61, 0x6e,
    0x64, 0x67, 0x13, 0x80, 0x64, 0x1d, 0x69, 0x10,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigEasyMan[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigEasyMan,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Saver
//********************************************************************

BYTE gpbySigSaver[] =
{
    0x61, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x69, 0x61, 0x20, 0x31, 0x39, 0x39, 0x36, 0x64,
    0x2f, 0x69, 0x03, 0x64, 0x6c, 0x67, 0x34, 0x67,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigSaver[] =
{
    (BYTE LPF *) "DATEISPEICHERN", gpbySigSaver,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Spooky
//********************************************************************

BYTE gpbySigSpooky[] =
{
    0x71, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x73, 0x70, 0x6f, 0x6f, 0x6b, 0x79, 0x12, 0x6c,
    0x01, 0x00, 0x64, 0x67, 0xc2, 0x80, 0x69, 0x03,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigSpooky[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigSpooky,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Atom.b
//********************************************************************

BYTE gpbySigAtomB[] =
{
    0x41, 0x00,                                     // Offset in macro
    0x0f,                                           // Length of string
    0x2a, 0x2e, 0x2a, 0x64, 0x1a, 0x1d, 0x65, 0x09,
    0x4b, 0x69, 0x6c, 0x6c, 0x45, 0x72, 0x72,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigAtomB[] =
{
    (BYTE LPF *) "ATOM", gpbySigAtomB,               // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Birthday
//********************************************************************

BYTE gpbySigBirthday[] =
{
    0x35, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string
    0x80, 0x69, 0x04, 0x6d, 0x73, 0x67, 0x24, 0x12,
    0x6a, 0x0e, 0x48, 0x61, 0x70, 0x70, 0x79,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigBirthday[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigBirthday,       // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Doggie
//********************************************************************

BYTE gpbySigDoggie[] =
{
    0xb9, 0x00,                                     // Offset in macro
    0x0f,                                           // Length of string
    0x44, 0x6f, 0x67, 0x67, 0x69, 0x65, 0x12, 0x67,
    0x25, 0x80, 0x05, 0x06, 0x07, 0x6a, 0x07,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigDoggie[] =
{
    (BYTE LPF *) "FILESAVEAS", gpbySigDoggie,       // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// KillDLL
//********************************************************************

BYTE gpbySigKillDLL[] =
{
    0x01, 0x01,                                     // Offset in macro
    0x0f,                                           // Length of string
    0x53, 0x59, 0x53, 0x54, 0x45, 0x4d, 0x5c, 0x2a,
    0x2e, 0x44, 0x3f, 0x3f, 0x06, 0x64, 0x52,

    0xff, 0xff                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigKillDLL[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigKillDLL,       // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Concept.c
//********************************************************************

BYTE gpbySigConceptC[] =
{
    0x1A, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string

    0x46, 0x32, 0x12, 0x69, 0x08, 0x73, 0x54, 0x4d, // String
    0x61, 0x63, 0x72, 0x6f, 0x24, 0x64, 0x69,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigConceptC[] =
{
    (BYTE LPF *)"F2", gpbySigConceptC,              // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Concept.d
//********************************************************************

BYTE gpbySigConceptD[] =
{
    0xD6, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string

    0x46, 0x69, 0x6c, 0x65, 0x53, 0x6f, 0x72, 0x74, // String
    0x12, 0x69, 0x06, 0x4d, 0x61, 0x63, 0x72,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigConceptD[] =
{
    (BYTE LPF *)"FILESORT", gpbySigConceptD,              // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Concept.e
//********************************************************************

BYTE gpbySigConceptE[] =
{
    0x2C, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string

    0x4c, 0x6f, 0x61, 0x64, 0x12, 0x69, 0x08, 0x73, // String
    0x74, 0x6d, 0x61, 0x63, 0x72, 0x6f, 0x24,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigConceptE[] =
{
    (BYTE LPF *)"AAAZFS", gpbySigConceptE,              // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Concept.hcr
//********************************************************************

BYTE gpbySigConceptHcr[] =
{
    0x15, 0x01,                                     // Offset in macro
    0x0F,                                           // Length of string

    0x07, 0x3A, 0x58, 0x41, 0x41, 0x5A, 0x46, 0x53,
    0x64, 0x67, 0xC2, 0x80, 0x6A, 0x0D, 0x47,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigConceptHcr[] =
{
    (BYTE LPF *)"XAAZFS", gpbySigConceptHcr,              // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// Terbaru
//********************************************************************

BYTE gpbySigTerbaru[] =
{
    0x79, 0x00,                                     // Offset in macro
    0x0F,                                           // Length of string

    0x64, 0x6F, 0x02, 0x69, 0x06, 0x53, 0x61, 0x76,
    0x65, 0x49, 0x54, 0x64, 0x52, 0x20, 0x64,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigTerbaru0[] =               // normal.dot
{
    (BYTE LPF *)"AUTOCLOSE", gpbySigTerbaru,        // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigTerbaru1[] =
{
    (BYTE LPF *)"CFXX", gpbySigTerbaru,             // Sub-signature
    NULL, NULL                                      // No more subsigs
};

//********************************************************************
// WM.CLock
//********************************************************************

BYTE gpbySigClock[] =
{
    0xcf, 0x02,                                     // Offset in macro
    0X10,                                           // lENGTH OF STRING

    0x41, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x12, 0x69,
    0x08, 0x73, 0x54, 0x4d, 0x61, 0x63, 0x72, 0x6f,

    0xFF, 0xFF                                      // No more
};

WORD_MACRO_SIG_T gpwmsSigClock[] =
{
    (BYTE LPF *)"ACTION", gpbySigClock,              // Sub-signature
    NULL, NULL                                       // No more subsigs
};

//********************************************************************
// Bandung
//********************************************************************

BYTE gpbySigBand[] =
{
    0xfd, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x69, 0x04, 0x53, 0x4d, 0x65, 0x24, 0x07, 0x6a,
    0x0f, 0x3a, 0x54, 0x6f, 0x6f, 0x6c, 0x73, 0x43,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigBand[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigBand,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Reflex
//********************************************************************

BYTE gpbySigReflex[] =
{
    0x90, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x00, 0x64, 0x67, 0xc2, 0x80, 0x6a, 0x09, 0x67,
    0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x3a, 0x46, 0x41,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigReflex0[] =
{
    (BYTE LPF *) "FCLOSE", gpbySigReflex,       // Sub-signature
    NULL, NULL                                  // DOC files
};

WORD_MACRO_SIG_T gpwmsSigReflex1[] =
{
    (BYTE LPF *) "FILECLOSE", gpbySigReflex,    // Sub-signature
    NULL, NULL                                  // NORMAL.DOT
};

//********************************************************************
// Satanic
//********************************************************************

BYTE gpbySigSatanic[] =
{
    0xec, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x7c, 0x46, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x20,
    0x63, 0x3a, 0x20, 0x2f, 0x75, 0x64, 0x52, 0x67,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigSatanic[] =
{
    (BYTE LPF *) "AUTOEXIT", gpbySigSatanic,    // Sub-signature
    NULL, NULL                                  // DOC files
};


//*******************************************************************
// Tedious
//********************************************************************

BYTE gpbySigTedious[] =
{
    0x9f, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x76, 0x41, 0x75, 0x74, 0x6f, 0x4e, 0x65, 0x77,
    0x64, 0x52, 0x67, 0xc2, 0x80, 0x69, 0x07, 0x53,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigTedious[] =
{
    (BYTE LPF *) "FILESAVEAS", gpbySigTedious,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Concept.b
//********************************************************************

BYTE gpbySigConceptB[] =
{
    0xda, 0x02,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x47, 0x6f, 0x65, 0x64, 0x31, 0x64, 0x6f, 0x02,
    0x67, 0x2b, 0x80, 0x6a, 0x07, 0x47, 0x4f, 0x54,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigConceptB[] =
{
    (BYTE LPF *) "IKWORDNIETGOED1", gpbySigConceptB,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Concept.i
//********************************************************************

BYTE gpbySigConceptI[] =
{
    0x62, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x71, 0x6c, 0x01, 0x00, 0x64, 0x3d, 0x6a, 0x0f,
    0x43, 0x3a, 0x5c, 0x61, 0x75, 0x74, 0x6f, 0x65,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigConceptI[] =
{
    (BYTE LPF *) "TOOLSSPELLING", gpbySigConceptI,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Taiwan1
//********************************************************************

BYTE gpbySigTaiwan1[] =
{
    0x22, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x01, 0x69, 0x06, 0x06, 0x07, 0x79, 0x02, 0x00,
    0x20, 0x00, 0x2a, 0x00, 0x64, 0x6f, 0x03, 0x26,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigTaiwan1[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigTaiwan1,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Concept.J
//********************************************************************

BYTE gpbySigConceptJ[] =
{
    0x30, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x24, 0x69, 0x05, 0x74, 0x6f, 0x74, 0x61, 0x6c,
    0x64, 0x6f, 0x04, 0x1d, 0x67, 0xb8, 0x80, 0x05,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigConceptJ[] =
{
    (BYTE LPF *) "AUTOCLOSE", gpbySigConceptJ,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Concept.K
//********************************************************************

BYTE gpbySigConceptK[] =
{
    0x9e, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x54, 0x69, 0x6d, 0x65, 0x21, 0x12, 0x6c, 0x30,
    0x00, 0x64, 0x54, 0x64, 0x3e, 0x69, 0x03, 0x64,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigConceptK[] =
{
    (BYTE LPF *) "NEWFS", gpbySigConceptK,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Colors.C
//********************************************************************

BYTE gpbySigColorsC[] =
{
    0x78, 0x04,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x73, 0x24, 0x05,
    0x69, 0x01, 0x69, 0x06, 0x12, 0x67, 0x07, 0x80,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigColorsC[] =
{
    (BYTE LPF *) "MACROS", gpbySigColorsC,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Alien / Alien.B
//********************************************************************

BYTE gpbySigAlien[] =
{
    0x10, 0x0d,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x64, 0x6f, 0x03, 0x67, 0x0a, 0x81, 0x05, 0x6a,
    0x0f, 0x50, 0x72, 0x6f, 0x67, 0x72, 0x61, 0x6d,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigAlien[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigAlien,       // Sub-signature
    NULL, NULL
};

BYTE gpbySigAlienb[] =
{
    0x35, 0x0d,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x41, 0x6C, 0x69, 0x65, 0x6E, 0x64, 0x6F, 0x03,
    0x67, 0x0A, 0x81, 0x05, 0x6A, 0x0F, 0x50, 0x72,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigAlienb[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigAlienb,       // Sub-signature
    NULL, NULL
};


//*******************************************************************
// Rapi
//********************************************************************

BYTE gpbySigRapi[] =
{
    0x52, 0x0d,                                 // Offset in macro
    0x18,                                       // # Bytes.
    0x54, 0x58, 0x54, 0x23, 0x3b, 0x34, 0x71, 0x6c,
    0x01, 0x00, 0x64, 0x39, 0x71, 0x6c, 0x01, 0x00,
    0x12, 0x69, 0x08, 0x62, 0x61, 0x63, 0x61, 0x6c,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigRapi[] =
{
    (BYTE LPF *) "RPAE", gpbySigRapi,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Helper
//********************************************************************

BYTE gpbySigHelper[] =
{
    0x30, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x00, 0x73, 0x60, 0x00, 0x0c, 0x6a, 0x04, 0x68,
    0x65, 0x6c, 0x70, 0x64, 0x54, 0x64, 0x64, 0x69,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigHelper[] =
{
    (BYTE LPF *) "AUTOCLOSE", gpbySigHelper,    // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Switches
//********************************************************************

BYTE gpbySigSwitches[] =
{
    0x9f, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x6c, 0x01, 0x00, 0x64, 0x36, 0x71, 0x6c, 0x01,
    0x00, 0x12, 0x6a, 0x11, 0x64, 0x65, 0x6c, 0x74,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigSwitches[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigSwitches,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// BigDaddy
//********************************************************************

BYTE gpbySigBigDaddy[] =
{
    0xc0, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x69, 0x73, 0x65, 0x12, 0x73, 0x98, 0x00, 0x12,
    0x73, 0x03, 0x00, 0x0c, 0x6c, 0x00, 0x00, 0x64,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigBigDaddy[] =
{
    (BYTE LPF *) "GANGSTERZ", gpbySigBigDaddy,       // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Nuclear.A.Damaged
//********************************************************************

BYTE gpbySigNukeDam[] =
{
    0x20, 0x02,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x6c, 0x01, 0x00, 0x64, 0x1a, 0x1d, 0x64, 0x29,
    0x69, 0x09, 0x44, 0x72, 0x6f, 0x70, 0x53, 0x75,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigNukeDam[] =
{
    (BYTE LPF *) "AUTOEXEC", gpbySigNukeDam,    // Sub-signature
    NULL, NULL
};

//*******************************************************************
// Jakarta.A.Damaged
//********************************************************************

BYTE gpbySigNpad[] =                            // Used to be called Npad
{
    0x31, 0x05,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0xef, 0xe7, 0xae, 0xea, 0xeb, 0xe0, 0xe9, 0xef,
    0xe0, 0xae, 0xde, 0xeb, 0xfc, 0xe0, 0xf7, 0xef,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigNpad[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigNpad,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Stryx
//********************************************************************

BYTE gpbySigStryx[] =
{
    0x90, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x64, 0x36, 0x71, 0x6c, 0x01, 0x00, 0x12, 0x6a,
    0x0c, 0x4e, 0x20, 0x44, 0x52, 0x41, 0x43, 0x48,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigStryx[] =              // Normal.dot detection
{
    (BYTE LPF *) "STRYXONE", gpbySigStryx,      // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigStryx2[] =             // Normal.dot detection
{
    (BYTE LPF *) "STRYX1", gpbySigStryx,        // Sub-signature
    NULL, NULL
};

//********************************************************************
// Outlaw
//********************************************************************

BYTE gpbySigOutlaw1a[] =                        // 1st Macro in Outlaw.A
{
    0x00, 0x04,                                 // Offset in macro
    0x10,                                       // # of Bytes
    0x67, 0x0e, 0x80, 0x05, 0x06, 0x0a, 0x05, 0x69,
    0x03, 0x54, 0x77, 0x6f, 0x08, 0x69, 0x03, 0x4f,

    0xff, 0xff
};

BYTE gpbySigOutlaw2[] =                         // 2nd Macro in Outlaw A & B
{
    0xd0, 0x02,                                 // Offset in macro
    0x10,                                       // # of Bytes
    0x09, 0x6c, 0x61, 0x75, 0x67, 0x68, 0x2e, 0x73,
    0x63, 0x72, 0x23, 0x3b, 0x34, 0x71, 0x6c, 0x01,

    0xff, 0xff
};

BYTE gpbySigOutlaw3a[] =                        // 3rd Macro in Outlaw A
{
    0x80, 0x05,                                 // Offset in macro
    0x10,                                       // # of Bytes
    0x64, 0x67, 0x4c, 0x81, 0x6a, 0x0a, 0x56, 0x69,
    0x72, 0x4e, 0x61, 0x6d, 0x65, 0x44, 0x6f, 0x63,

    0xff, 0xff
};

BYTE gpbySigOutlaw1b[] =                        // 1st Macro in Outlaw B
{
    0x00, 0x04,                                 // Offset in macro
    0x10,                                       // # of Bytes
    0x67, 0x02, 0x80, 0x05, 0x67, 0x0e, 0x80, 0x05,
    0x06, 0x0a, 0x05, 0x69, 0x03, 0x54, 0x77, 0x6f,

    0xff, 0xff
};

BYTE gpbySigOutlaw3b[] =                        // 3rd Macro in Outlaw B
{
    0x80, 0x05,                                 // Offset in macro
    0x10,                                       // # of Bytes
    0x12, 0x6c, 0x01, 0x00, 0x64, 0x67, 0x4c, 0x81,
    0x6a, 0x0a, 0x56, 0x69, 0x72, 0x4e, 0x61, 0x6d,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigOutlaw1A[] =
{
    (BYTE LPF *) "*", gpbySigOutlaw1a,          // Sub-signature
    NULL, NULL                                  // "*" scans all macros
};

WORD_MACRO_SIG_T gpwmsSigOutlaw2[] =
{
    (BYTE LPF *) "*", gpbySigOutlaw2,           // Sub-signature
    NULL, NULL                                  // "*" scans all macros
};

WORD_MACRO_SIG_T gpwmsSigOutlaw3A[] =
{
    (BYTE LPF *) "*", gpbySigOutlaw3a,          // Sub-signature
    NULL, NULL                                  // "*" scans all macros
};

WORD_MACRO_SIG_T gpwmsSigOutlaw1B[] =
{
    (BYTE LPF *) "*", gpbySigOutlaw1b,          // Sub-signature
    NULL, NULL                                  // "*" scans all macros
};

WORD_MACRO_SIG_T gpwmsSigOutlaw3B[] =
{
    (BYTE LPF *) "*", gpbySigOutlaw3b,          // Sub-signature
    NULL, NULL                                  // "*" scans all macros
};

//********************************************************************
// LookC
//********************************************************************

BYTE gpbySigLookC[] =
{
    0x80, 0x05,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x6e, 0x10, 0x67, 0xae, 0x00, 0x73, 0x22, 0x00,
    0x0c, 0x6c, 0x23, 0x00, 0x6e, 0x23, 0x64, 0x6e,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigLookC[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigLookC,      // Sub-signature
    NULL, NULL
};

//********************************************************************
// Weather.B
//********************************************************************

BYTE gpbySigWeatherB[] =
{
    0x80, 0x06,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x64, 0x6e, 0x04, 0x67, 0x2b, 0x80, 0x79, 0x10,
    0x00, 0x40, 0xa4, 0xec, 0xa6, 0xd3, 0xa8, 0xdb,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigWeatherB[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigWeatherB,   // Sub-signature
    NULL, NULL
};

//********************************************************************
// Theatre
//********************************************************************

BYTE gpbySigTheatre[] =
{
    0xb0, 0x0a,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x01, 0x73, 0xca, 0x02, 0x0c, 0x79, 0x08, 0x00,
    0x44, 0x00, 0x61, 0x00, 0x72, 0x00, 0x6b, 0x00,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigTheatre[] =
{
    (BYTE LPF *) "FILECLOSE", gpbySigTheatre,   // Sub-signature
    NULL, NULL
};

//********************************************************************
// Twno.C
//********************************************************************

BYTE gpbySigTwnoC[] =
{
    0x60, 0x05,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x00, 0x12, 0x69, 0x06, 0x66, 0x69, 0x6c, 0x65,
    0x6d, 0x24, 0x52, 0x64, 0x1a, 0x1d, 0x64, 0x64,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigTwnoC1[] =             // Document Infection
{
    (BYTE LPF *) "AUTOOPEN", gpbySigTwnoC,      // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigTwnoC2[] =             // Normal.dot Infection
{
    (BYTE LPF *) "AUTONEW", gpbySigTwnoC,
    NULL, NULL
};

//********************************************************************
// Twno.B
//********************************************************************

BYTE gpbySigTwnoB[] =
{
    0x50, 0x05,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x12, 0x69, 0x06, 0x66, 0x69, 0x6c, 0x65, 0x6d,
    0x24, 0x52, 0x64, 0x1a, 0x1d, 0x64, 0x64, 0x52,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigTwnoB1[] =             // Document Infection
{
    (BYTE LPF *) "AUTOEXEC", gpbySigTwnoB,      // Sub-signature
    NULL, NULL
};

WORD_MACRO_SIG_T gpwmsSigTwnoB2[] =             // Normal.dott Infection
{
    (BYTE LPF *) "AUTONEW", gpbySigTwnoB,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Lunch.B
//********************************************************************

BYTE gpbySigLunchB[] =
{
    0xbe, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x54, 0x69, 0x6d, 0x65, 0x21, 0x12, 0x6c, 0x30,
    0x00, 0x64, 0x54, 0x64, 0x67, 0x54, 0x00, 0x69,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigLunchB[] =
{
    (BYTE LPF *) "NEWFS", gpbySigLunchB,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// AntiConcept
//********************************************************************

BYTE gpbySigAntiCon[] =
{
    0x80, 0x02,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x6c, 0x3a, 0x50, 0x61, 0x79, 0x4c, 0x6f, 0x61,
    0x64, 0x64, 0x64, 0x67, 0x2b, 0x80, 0x05, 0x6a,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigAntiCon[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigAntiCon,    // Sub-signature
    NULL, NULL
};

//********************************************************************
// Npad.C (Damaged version of Npad.A)
//********************************************************************

BYTE gpbySigNpadC[] =
{
    0x4b, 0x06,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x8e, 0x8e, 0x8e, 0x8e, 0x8e, 0x62, 0x61, 0x74,
    0x61, 0x73, 0x12, 0x69, 0x05, 0x57, 0x61, 0x6b,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigNpadC[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigNpadC,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Npad.E (Damaged version of Npad.A)
//********************************************************************

BYTE gpbySigNpadE[] =
{
    0x31, 0x05,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x88, 0xea, 0xe1, 0xe0, 0xb5, 0xfa, 0x8f, 0x86,
    0x87, 0xcb, 0xc3, 0xde, 0xc2, 0xc1, 0xd7, 0xcb,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigNpadE[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigNpadE,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Npad.H (Damaged version of Npad.A)
//********************************************************************

BYTE gpbySigNpadH[] =
{
    0x67, 0x06,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x76, 0x76, 0x76, 0x00, 0x00, 0x24, 0x69, 0x05,
    0x62, 0x61, 0x74, 0x61, 0x73, 0x64, 0x52, 0x69,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigNpadH[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigNpadH,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Twno.D
//********************************************************************

BYTE gpbySigTwnoD[] =
{
    0xa0, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x0c, 0x79, 0x03, 0x00, 0xf5, 0xa7, 0x6f, 0xbf,
    0xa7, 0xbe, 0x1e, 0x2a, 0x69, 0x07, 0x6d, 0x65,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigTwnoD[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigTwnoD,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Concept.T
//********************************************************************

BYTE gpbySigConceptT[] =
{
    0x86, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x01, 0x00, 0x03, 0x69, 0x0b, 0x6d, 0x6f, 0x6e,
    0x74, 0x68, 0x6e, 0x75, 0x6d, 0x62, 0x65, 0x72,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigConceptT[] =
{
    (BYTE LPF *) "AUTOEXIT", gpbySigConceptT,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Sutra
//********************************************************************

BYTE gpbySigSutra[] =
{
    0x0c, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x64, 0x6f, 0x03, 0x69, 0x0f, 0x62, 0x54, 0x6f,
    0x6f, 0x4d, 0x75, 0x63, 0x68, 0x54, 0x72, 0x6f,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigSutra[] =
{
    (BYTE LPF *) "CTFBORNIN83", gpbySigSutra,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Divina.C
//********************************************************************

BYTE gpbySigDivinaC[] =
{
    0xa0, 0x0a,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x1d, 0x69, 0x03, 0x6d, 0x69, 0x6e, 0x0c, 0x6c,
    0x11, 0x00, 0x1e, 0x64, 0x64, 0x52, 0x23, 0x69,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigDivinaC[] =
{
    (BYTE LPF *) "AUTOCLOSE", gpbySigDivinaC,   // Sub-signature
    NULL, NULL
};

//********************************************************************
// Johnny
//********************************************************************

BYTE gpbySigJohnny[] =
{
    0x51, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x47, 0x6f, 0x6a, 0x6f, 0x68, 0x6e, 0x6e, 0x79,
    0x1e, 0x64, 0x69, 0x02, 0x42, 0x49, 0x0c, 0x08,

    0xff, 0xff
};

//PRESENTV is also copied to AUTOOPEN in doc files
WORD_MACRO_SIG_T gpwmsSigJohnny0[] =
{
    (BYTE LPF *) "PRESENTV", gpbySigJohnny,       // Sub-signature
    NULL, NULL
};

//********************************************************************
// Nuclear.E
//********************************************************************

BYTE gpbySigNuclearE[] =
{
    0x2c, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x80, 0x67, 0x3b, 0x80, 0x05, 0x06, 0x07, 0x6a,
    0x08, 0x3a, 0x4d, 0x63, 0x41, 0x66, 0x65, 0x65,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigNuclearE[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigNuclearE,   // Sub-signature
    NULL, NULL
};

//********************************************************************
// MDMA.C
//********************************************************************

BYTE gpbySigMDMAC[] =
{
    0xc9, 0x01,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x69, 0x6e, 0x66, 0x65, 0x63, 0x74, 0x64, 0x52,
    0x1d, 0x05, 0x67, 0x0c, 0x81, 0x05, 0x6c, 0x15,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigMDMAC[] =
{
    (BYTE LPF *) "AUTOCLOSE", gpbySigMDMAC,   // Sub-signature
    NULL, NULL
};

//********************************************************************
// WM.APPDER
//********************************************************************

BYTE gpbySigAppder[] =
{
    0x91, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x64, 0x65, 0x72, 0x12, 0x69, 0x08, 0x73, 0x54,
    0x4d, 0x61, 0x63, 0x72, 0x6f, 0x24, 0x64, 0x69,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigAppder[] =
{
    (BYTE LPF *) "AUTOCLOSE", gpbySigAppder,   // Sub-signature
    NULL, NULL
};

//********************************************************************
// WM.NPAD.J
//********************************************************************

BYTE gpbySigNpadJ[] =
{
    0x8e, 0x00,                                 // Offset in macro
    0x10,                                       // # Bytes.
    0x67, 0x9e, 0x80, 0x05, 0x6c, 0x01, 0x00, 0x06,
    0x64, 0x67, 0xd6, 0x80, 0x05, 0x6c, 0x00, 0x00,

    0xff, 0xff
};

WORD_MACRO_SIG_T gpwmsSigNpadJ[] =
{
    (BYTE LPF *) "AUTOOPEN", gpbySigNpadJ,   // Sub-signature
    NULL, NULL
};


//********************************************************************
//********************************************************************
// Array of Word Macro Virus signatures
//********************************************************************
//********************************************************************

WORD_SIG_T gpWordVirusSigs[] =
{
    VID_COLORS,     gpwmsSigColors,
    VID_CONCEPT0,   gpwmsSigConcept0,
    VID_CONCEPTF,   gpwmsSigConF,       // Normal.dot   (French)
    VID_CONCEPT1,   gpwmsSigConcept1,   // Normal.dot   (English)
    VID_DMV,        gpwmsSigDMV,
    VID_FORMATC,    gpwmsSigFormatC,
    VID_NUCLEAR,    gpwmsSigNuclearB,
    VID_NUCLEAR,    gpwmsSigNuclear,
    VID_HOT1,       gpwmsSigHot1,       // Document
    VID_HOT0,       gpwmsSigHot0,       // Normal.dot
    VID_IMP0,       gpwmsSigImp0,       // Document
    VID_IMP1,       gpwmsSigImp1,       // Normal.dot
    VID_IRISH0,     gpwmsSigIrish0,     // normal.dot
    VID_IRISH1,     gpwmsSigIrish1,     // document
    VID_WAZZU,      gpwmsSigWaz,
    VID_WAZZU2,     gpwmsSigWaz2,       // wazzu w/out Payload
    VID_WAZZU3,     gpwmsSigWaz3,       // wazzu w/out Payload
    VID_WAZZU4,     gpwmsSigWazD,
    VID_WAZZU5,     gpwmsSigWazE,
    VID_WAZZU6,     gpwmsSigWazF,
    VID_INFEZIONE,  gpwmsSigInfez,
    VID_XENIXOS,    gpwmsSigXenixos,
    VID_XENIXOS1,   gpwmsSigXenixosMacWeenie,  // For Macs
    VID_BOOM,       gpwmsSigBoom,
    VID_PARASITE1,  gpwmsSigParasite1,  // Document
    VID_PARASITE0,  gpwmsSigParasite0,  // Normal.dot
    VID_NOP1,       gpwmsSigNOP1,       // Normal.dot
    VID_NOP2,       gpwmsSigNOP2,       // Document
    VID_MDMADMV,    gpwmsSigMDMADMV,    // Normal.dot
    VID_ATOM,       gpwmsSigATOM,
    VID_POLITE,     gpwmsSigPolite,
    VID_MADDOG,     gpwmsSigMADDOG,     // Normal.dot AND dOC.
    VID_NUCLEARB,   gpwmsSigNuclear_b,
    VID_PHANTOM,    gpwmsSigPHANTOM,    // Normal.dot AND dOC.
    VID_FRIENDLY,   gpwmsSigFriendly,
    VID_COLOR_B,    gpwmsSigCOLOR_B,    // Normal.dot AND dOC.
    VID_DIVINA,     gpwmsSigDivina,
    VID_TELEFONICA, gpwmsSigTelefonica,
    VID_MICROSLOTH, gpwmsSigMicrosloth,
    VID_JAKARTA,    gpwmsSigJakarta,
    VID_BOSCO,      gpwmsSigBosco,
    VID_BURONEU0,   gpwmsSigBuroNeu0,
    VID_BURONEU1,   gpwmsSigBuroNeu1,
    VID_CONCEPTH0,  gpwmsSigConceptH0,  // Document
    VID_CONCEPTH1,  gpwmsSigConceptH1,  // Normal.dot
    VID_FISHFOOD,   gpwmsSigFishfood,
    VID_BOGUS0,     gpwmsSigBogus0,     // normal.dot
    VID_BOGUS1,     gpwmsSigBogus1,     // document infections
    VID_INDONESIA,  gpwmsSigIndo,
    VID_ALLIANCE,   gpwmsSigAlliance,
    VID_EASYMAN,    gpwmsSigEasyMan,
    VID_SAVER,      gpwmsSigSaver,
    VID_SPOOKY,     gpwmsSigSpooky,
    VID_ATOMB,      gpwmsSigAtomB,
    VID_BIRTHDAY,   gpwmsSigBirthday,
    VID_DOGGIE,     gpwmsSigDoggie,
    VID_KILLDLL,    gpwmsSigKillDLL,
    VID_CONCEPTC,   gpwmsSigConceptC,
    VID_CONCEPTD,   gpwmsSigConceptD,
    VID_CONCEPTE,   gpwmsSigConceptE,
    VID_CONCEPTHCR, gpwmsSigConceptHcr,
    VID_TERBARU0,   gpwmsSigTerbaru0,   // normal.dot
    VID_TERBARU1,   gpwmsSigTerbaru1,   // document infections
    VID_CLOCK,      gpwmsSigClock,
    VID_WAZZUB,     gpwmsSigWazB,       // Wazzu w/ Payload in AutoOpen
    VID_BANDUNG,    gpwmsSigBand,       // Wazzu w/ Payload in AutoOpen
    VID_NOPB0,      gpwmsSigNOPB0,      // NOP.B in docs
    VID_NOPB1,      gpwmsSigNOPB1,      // NOP.B in NORMAL.DOT
    VID_REFLEX1,    gpwmsSigReflex1,    // Reflex in NORMAL.DOT Reflex ptrs
    VID_REFLEX0,    gpwmsSigReflex0,    // Reflex in docs       *must* be in
    VID_SATANIC,    gpwmsSigSatanic,    //                      this order!!
    VID_TEDIOUS,    gpwmsSigTedious,
    VID_CONCEPTB,   gpwmsSigConceptB,
    VID_PARASITE3,  gpwmsSigPara3,       // document infections
    VID_PARASITE4,  gpwmsSigPara4,       // normal.dot infections
    VID_CONCEPTI,   gpwmsSigConceptI,
    VID_TAIWAN1,    gpwmsSigTaiwan1,
    VID_CONCEPTJ,   gpwmsSigConceptJ,
    VID_JAKARTAB,   gpwmsSigJakartaB,
    VID_DIVINAB,    gpwmsSigDivinaB,
    VID_CONCEPTK,   gpwmsSigConceptK,
    VID_COLORSC,    gpwmsSigColorsC,
    VID_ALIEN,      gpwmsSigAlien,
    VID_RAPI,       gpwmsSigRapi,
    VID_HELPER,     gpwmsSigHelper,
    VID_SWITCHES,   gpwmsSigSwitches,
    VID_BIGDADDY,   gpwmsSigBigDaddy,
    VID_NUKEDAM,    gpwmsSigNukeDam,
    VID_NPAD,       gpwmsSigNpad,
    VID_STRYX,      gpwmsSigStryx,
    VID_OUTLAW1A,   gpwmsSigOutlaw1A,
    VID_OUTLAW2,    gpwmsSigOutlaw2,
    VID_OUTLAW3A,   gpwmsSigOutlaw3A,
    VID_OUTLAW1B,   gpwmsSigOutlaw1B,
    VID_OUTLAW3B,   gpwmsSigOutlaw3B,
    VID_LOOKC,      gpwmsSigLookC,
    VID_WEATHERB,   gpwmsSigWeatherB,
    VID_THEATRE,    gpwmsSigTheatre,
    VID_TWNOC1,     gpwmsSigTwnoC1,     // Documents
    VID_TWNOC2,     gpwmsSigTwnoC2,     // Normal.dot
    VID_TWNOB1,     gpwmsSigTwnoB1,     // Documents
    VID_TWNOB2,     gpwmsSigTwnoB2,     // Normal.dot
    VID_LUNCHB,     gpwmsSigLunchB,
    VID_STRYX2,     gpwmsSigStryx2,
    VID_ANTICON,    gpwmsSigAntiCon,
    VID_NPADC,      gpwmsSigNpadC,
    VID_TWNOD,      gpwmsSigTwnoD,
    VID_ALIENB,     gpwmsSigAlienb,
    VID_CONCEPTT,   gpwmsSigConceptT,
    VID_SUTRA,      gpwmsSigSutra,
    VID_NPADE,      gpwmsSigNpadE,
    VID_DIVINAC,    gpwmsSigDivinaC,
    VID_NUCLEARE,   gpwmsSigNuclearE,
    VID_MDMAC,      gpwmsSigMDMAC,
    VID_WAZZUY,     gpwmsSigWazzuY,
    VID_JOHNNY0,    gpwmsSigJohnny0,
    VID_APPDER,     gpwmsSigAppder,
    VID_NPADH,      gpwmsSigNpadH,
    VID_WAZZUX,     gpwmsSigWazzuX,
    VID_NPADJ,      gpwmsSigNpadJ
};

WORD gwNumMacroScanDefs = (sizeof(gpWordVirusSigs)/sizeof(WORD_SIG_T));
