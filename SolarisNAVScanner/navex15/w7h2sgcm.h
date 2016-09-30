// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/w7h2sgcm.h_v   1.0   10 Jun 1998 13:05:56   DCHI  $
//
// Description:
//  Heuristic signature control stream definitions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/w7h2sgcm.h_v  $
// 
//    Rev 1.0   10 Jun 1998 13:05:56   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _W7H2SGCM_H_

#define _W7H2SGCM_H_

////////////////////////////////////////////////////////////
// Str sigs
////////////////////////////////////////////////////////////

#define W7H2_SNIB0_MATCH_N              0x00
#define W7H2_SNIB0_MISC                 0x0E
#define W7H2_SNIB0_END                  0x0F

////////////////////////////////////////////////////////////
// Line, Func, and Set sigs
////////////////////////////////////////////////////////////

#define W7H2_HNIB0_CMD_SIG_BYTE         0x00
#define W7H2_HNIB0_CMD_SIG_WORD         0x01
#define W7H2_HNIB0_DLG_SIG_BYTE         0x02
#define W7H2_HNIB0_DLG_SIG_WORD         0x03
#define W7H2_HNIB0_LINE_SIG_BYTE        0x04
#define W7H2_HNIB0_LINE_SIG_WORD        0x05
#define W7H2_HNIB0_FUNC_SIG_BYTE        0x06
#define W7H2_HNIB0_FUNC_SIG_WORD        0x07

#define W7H2_HNIB0_STR_SIG_BYTE         0x08
#define W7H2_HNIB0_STR_SIG_WORD         0x09

#define W7H2_HNIB0_OP                   0x0A

#define W7H2_HNIB0_AND                  0x0B
#define W7H2_HNIB0_OR                   0x0C
#define W7H2_HNIB0_NOT                  0x0D

#define W7H2_HNIB0_MISC                 0x0E

    #define W7H2_HNIB1_DLG_NOT_0            0x00
    #define W7H2_HNIB1_CAT_GT               0x01
    #define W7H2_HNIB1_CAT_LT               0x02
    #define W7H2_HNIB1_CAT_EQ               0x03
    #define W7H2_HNIB1_ENCRYPTED_STR        0x04

#define W7H2_HNIB0_END                  0x0F

#endif // #ifndef _W7H2SGCM_H_

