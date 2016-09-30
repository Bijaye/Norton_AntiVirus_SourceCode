//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wdsigcmd.h_v   1.3   11 May 1998 18:02:44   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wdsigcmd.h_v  $
// 
//    Rev 1.3   11 May 1998 18:02:44   DCHI
// Changes for menu repair.
// 
//    Rev 1.2   09 Jul 1997 16:23:06   DCHI
// Added #define VNIB1_HEURISTIC_REPAIR      0x07
// 
//    Rev 1.1   08 Apr 1997 12:40:30   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.0   17 Jan 1997 11:25:06   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDSIGCMD_H_

#define _WDSIGCMD_H_

#define MAX_SCAN_BYTES              5
#define MAX_SCAN_RANGES             2

#define MAX_RAW_STRING_LEN          255

//////////////////////////////////////////////////////////////////////
// Name and macro text signature commands
//////////////////////////////////////////////////////////////////////

#define NIB0_SEEK                   0x00

    #define NIB1_SEEK_SET_0             0x00
    #define NIB1_SEEK_SET_1             0x01
    #define NIB1_SEEK_SET_2             0x02
    #define NIB1_SEEK_SET_3             0x03
    #define NIB1_SEEK_SET_4             0x04
    #define NIB1_SEEK_SET_5             0x05
    #define NIB1_SEEK_SET_6             0x06
    #define NIB1_SEEK_SET_BYTE          0x07
    #define NIB1_SEEK_SET_WORD          0x08
    #define NIB1_SEEK_SET_DWORD         0x09
    #define NIB1_SEEK_CUR_BYTE          0x0A
    #define NIB1_SEEK_CUR_WORD          0x0B
    #define NIB1_SEEK_CUR_DWORD         0x0C
    #define NIB1_SEEK_END_BYTE          0x0D
    #define NIB1_SEEK_END_WORD          0x0E
    #define NIB1_SEEK_END_DWORD         0x0F

#define NIB0_SEEK_CUR_POS           0x01

    #define NIB1_SEEK_CUR_1             0x00
    #define NIB1_SEEK_CUR_2             0x01
    #define NIB1_SEEK_CUR_3             0x02
    #define NIB1_SEEK_CUR_4             0x03
    #define NIB1_SEEK_CUR_5             0x04
    #define NIB1_SEEK_CUR_6             0x05
    #define NIB1_SEEK_CUR_7             0x06
    #define NIB1_SEEK_CUR_8             0x07
    #define NIB1_SEEK_CUR_9             0x08
    #define NIB1_SEEK_CUR_10            0x09
    #define NIB1_SEEK_CUR_11            0x0A
    #define NIB1_SEEK_CUR_12            0x0B
    #define NIB1_SEEK_CUR_13            0x0C
    #define NIB1_SEEK_CUR_14            0x0D
    #define NIB1_SEEK_CUR_15            0x0E
    #define NIB1_SEEK_CUR_16            0x0F

#define NIB0_SEEK_CUR_NEG           0x02

    #define NIB1_SEEK_CUR_N1            0x00
    #define NIB1_SEEK_CUR_N2            0x01
    #define NIB1_SEEK_CUR_N3            0x02
    #define NIB1_SEEK_CUR_N4            0x03
    #define NIB1_SEEK_CUR_N5            0x04
    #define NIB1_SEEK_CUR_N6            0x05
    #define NIB1_SEEK_CUR_N7            0x06
    #define NIB1_SEEK_CUR_N8            0x07
    #define NIB1_SEEK_CUR_N9            0x08
    #define NIB1_SEEK_CUR_M10           0x09
    #define NIB1_SEEK_CUR_N11           0x0A
    #define NIB1_SEEK_CUR_N12           0x0B
    #define NIB1_SEEK_CUR_N13           0x0C
    #define NIB1_SEEK_CUR_N14           0x0D
    #define NIB1_SEEK_CUR_N15           0x0E
    #define NIB1_SEEK_CUR_N16           0x0F

#define NIB0_SCAN                   0x03

    #define NIB1_SCAN_SLIDE_BYTE_SINGLE 0x00
    #define NIB1_SCAN_SLIDE_WORD_SINGLE 0x01
    #define NIB1_SCAN_SLIDE_BYTE_COMB   0x02
    #define NIB1_SCAN_SLIDE_WORD_COMB   0x03
    #define NIB1_SCAN_MASK_BYTE_SINGLE  0x04
    #define NIB1_SCAN_MASK_WORD_SINGLE  0x05
    #define NIB1_SCAN_MASK_BYTE_COMB    0x06
    #define NIB1_SCAN_MASK_WORD_COMB    0x07
    #define NIB1_SCAN_STRING_BYTE_S     0x08
    #define NIB1_SCAN_STRING_WORD_S     0x09
    #define NIB1_SCAN_STRING_BYTE_L     0x0A
    #define NIB1_SCAN_STRING_WORD_L     0x0B
    #define NIB1_SCAN_STRINGI_BYTE_S    0x0C
    #define NIB1_SCAN_STRINGI_WORD_S    0x0D
    #define NIB1_SCAN_STRINGI_BYTE_L    0x0E
    #define NIB1_SCAN_STRINGI_WORD_L    0x0F

        #define NIB2_COMB_R0_B2             0x00
        #define NIB2_COMB_R0_B3             0x01
        #define NIB2_COMB_R0_B4             0x02
        #define NIB2_COMB_R0_B5             0x03
        #define NIB2_COMB_R1_B0             0x04
        #define NIB2_COMB_R1_B1             0x05
        #define NIB2_COMB_R1_B2             0x06
        #define NIB2_COMB_R1_B3             0x07
        #define NIB2_COMB_R1_B4             0x08
        #define NIB2_COMB_R1_B5             0x09
        #define NIB2_COMB_R2_B0             0x0A
        #define NIB2_COMB_R2_B1             0x0B
        #define NIB2_COMB_R2_B2             0x0C
        #define NIB2_COMB_R2_B3             0x0D
        #define NIB2_COMB_R2_B4             0x0E
        #define NIB2_COMB_R2_B5             0x0F

        #define NIB2_SHORT_STR_2            0x00
        #define NIB2_SHORT_STR_3            0x01
        #define NIB2_SHORT_STR_4            0x02
        #define NIB2_SHORT_STR_5            0x03
        #define NIB2_SHORT_STR_6            0x04
        #define NIB2_SHORT_STR_7            0x05
        #define NIB2_SHORT_STR_8            0x06
        #define NIB2_SHORT_STR_9            0x07
        #define NIB2_SHORT_STR_10           0x08
        #define NIB2_SHORT_STR_11           0x09
        #define NIB2_SHORT_STR_12           0x0A
        #define NIB2_SHORT_STR_13           0x0B
        #define NIB2_SHORT_STR_14           0x0C
        #define NIB2_SHORT_STR_15           0x0D
        #define NIB2_SHORT_STR_16           0x0E
        #define NIB2_SHORT_STR_17           0x0F

#define NIB0_SIZE                   0x04

    #define NIB1_SIZE_BOUND_BYTE        0x00
    #define NIB1_SIZE_BOUND_WORD        0x01
    #define NIB1_SIZE_EQUAL_BYTE        0x02
    #define NIB1_SIZE_EQUAL_WORD        0x03
    #define NIB1_SIZE_LESS_THAN_BYTE    0x04
    #define NIB1_SIZE_LESS_THAN_WORD    0x05
    #define NIB1_SIZE_GREATER_THAN_BYTE 0x06
    #define NIB1_SIZE_GREATER_THAN_WORD 0x07

#define NIB0_MISC                   0x05

    #define NIB1_CUSTOM_SCAN_BYTE       0x00
    #define NIB1_CUSTOM_SCAN_WORD       0x01

    #define NIB1_NOT                    0x02
    #define NIB1_END_NOT                0x03

    #define NIB1_NAME_SIG_BYTE          0x04
    #define NIB1_NAME_SIG_WORD          0x05

    #define NIB1_HOT_BYTE_SKIP          0x08
    #define NIB1_AT_END                 0x09

    #define NIB1_BEG_OR                 0x0A
    #define NIB1_OR                     0x0B
    #define NIB1_END_OR                 0x0C

#define NIB0_RESERVED               0x06

#define NIB0_END                    0x07

#define NIB0_MATCH_1                0x08
#define NIB0_MATCH_2                0x09
#define NIB0_MATCH_3                0x0A
#define NIB0_MATCH_4                0x0B
#define NIB0_MATCH_5                0x0C
#define NIB0_MATCH_6                0x0D
#define NIB0_MATCH_7                0x0E
#define NIB0_MATCH_N                0x0F

//////////////////////////////////////////////////////////////////////
// Virus signature commands
//////////////////////////////////////////////////////////////////////

#define VNIB0_NAME_SIG_BYTE         0x00
#define VNIB0_NAME_SIG_WORD         0x01
#define VNIB0_NAMED_CRC_SIG_BYTE    0x02
#define VNIB0_NAMED_CRC_SIG_WORD    0x03
#define VNIB0_MACRO_SIG_BYTE        0x04
#define VNIB0_MACRO_SIG_WORD        0x05
#define VNIB0_CRC_BYTE              0x06
#define VNIB0_CRC_WORD              0x07

#define VNIB0_AND                   0x08
#define VNIB0_OR                    0x09
#define VNIB0_NOT                   0x0A

#define VNIB0_USE_SIGNATURE         0x0B
#define VNIB0_USE_OTHER_REPAIR_BYTE 0x0C
#define VNIB0_USE_OTHER_REPAIR_WORD 0x0D

#define VNIB0_MISC                  0x0E

    #define VNIB1_CUSTOM_REPAIR_BYTE    0x00
    #define VNIB1_CUSTOM_REPAIR_WORD    0x01
    #define VNIB1_FULL_SET_REPAIR       0x02
    #define VNIB1_MACRO_COUNT_EQ        0x03
    #define VNIB1_MACRO_COUNT_LT        0x04
    #define VNIB1_MACRO_COUNT_GT        0x05
    #define VNIB1_FULL_SET              0x06
    #define VNIB1_HEURISTIC_REPAIR      0x07
    #define VNIB1_MENU_REPAIR           0x08

#define VNIB0_END                   0x0F

#endif // #ifndef _WDSIGCMD_H_
