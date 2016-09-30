// Copyright 1996 Symantec Corporation
// ----------------------------------------------------------------------------
//
// $Header:   S:/AVENGE/VCS/BOOTDEFS.H_v   1.0   18 Apr 1997 13:39:18   MKEATIN  $
//
// Description:
//  This file contains common Boot Sector and MBR macros
//
// Contains:
//
// See Also:
//
// ----------------------------------------------------------------------------
// $Log:   S:/AVENGE/VCS/BOOTDEFS.H_v  $
// 
//    Rev 1.0   18 Apr 1997 13:39:18   MKEATIN
// Initial revision.
// 
//    Rev 1.1   19 Jul 1996 18:22:00   RAY
// Added support for 1k and 2k cluster DMF generic boot repair.
// 
//    Rev 1.0   04 Jun 1996 13:45:44   RAY
// Initial revision.
//
// ----------------------------------------------------------------------------

#if !defined ( BOOTDEFS_H )
#   define BOOTDEFS_H
#pragma pack (1)

                                        // --------------
                                        // JMP 0040 & NOP
                                        // --------------
#define JMP_AND_NOP { 0xEB, 0x3E, 0x90 }
#define sJMP_AND_NOP 0x03

                                        // -------------------
                                        // OEM ID = "SYMANTEC"
                                        // -------------------
#define OEM_ID { "SYMANTEC" }
#define sOEM_ID 0x08

                                        // ----------------------------
                                        // Volume Label = "GENERICBOOT"
                                        // ----------------------------
#define VOLUME_LABEL { "GENERICBOOT" }
#define sVOLUME_LABEL 0x0B



#define NON_BOOTABLE_CODE { 0xFA, 0x33, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C, \
                            0x8E, 0xD8, 0x8E, 0xC0, 0xFB, 0xBE, 0x80, 0x7C, \
                            0xE8, 0x06, 0x00, 0x30, 0xE4, 0xCD, 0x16, 0xCD, \
                            0x19, 0xAC, 0x08, 0xC0, 0x74, 0x09, 0xB4, 0x0E, \
                            0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xF2, 0xC3 }

#define sNON_BOOTABLE_CODE 0x28

                                        // ----------------------------------
                                        // 0140 FA            CLI
                                        // 0141 33C0          XOR     AX,AX
                                        // 0143 8ED0          MOV     SS,AX
                                        // 0145 BC007C        MOV     SP,7C00
                                        // 0148 8ED8          MOV     DS,AX
                                        // 014A 8EC0          MOV     ES,AX
                                        // 014C FB            STI
                                        // 014D BE807C        MOV     SI,7C80
                                        // 0150 E80600        CALL    0159
                                        // 0153 30E4          XOR     AH,AH
                                        // 0155 CD16          INT     16
                                        // 0157 CD19          INT     19
                                        // 0159 AC            LODSB
                                        // 015A 08C0          OR      AL,AL
                                        // 015C 7409          JZ      0167
                                        // 015E B40E          MOV     AH,0E
                                        // 0160 BB0700        MOV     BX,0007
                                        // 0163 CD10          INT     10
                                        // 0165 EBF2          JMP     0159
                                        // 0167 C3            RET
                                        // ----------------------------------


#define NON_BOOTABLE_TEXT { "\0x13\0x10This is not a bootable diskette.\0x13\0x10Remove it and press a key to restart...\0x13\0x10" }
#define sNON_BOOTABLE_TEXT 0x4D

#define MBR_CODE {  0xFA, 0x33, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C, \
                    0x8B, 0xF4, 0x8E, 0xC0, 0x8E, 0xD8, 0xFB, 0xFC, \
                    0xBF, 0x00, 0x06, 0xB9, 0x00, 0x01, 0xF2, 0xA5, \
                    0xEA, 0x1D, 0x06, 0x00, 0x00, 0xBE, 0xBE, 0x07, \
                    0xB3, 0x04, 0x80, 0x3C, 0x80, 0x74, 0x0E, 0x80, \
                    0x3C, 0x00, 0x75, 0x1C, 0x83, 0xC6, 0x10, 0xFE, \
                    0xCB, 0x75, 0xEF, 0xCD, 0x18, 0x8B, 0xEE, 0x8B, \
                    0x14, 0x8B, 0x4C, 0x02, 0x83, 0xC6, 0x10, 0xFE, \
                    0xCB, 0x74, 0x0A, 0x80, 0x3C, 0x00, 0x74, 0xF4, \
                    0xBE, 0xC7, 0x06, 0xEB, 0x30, 0xBF, 0x04, 0x00, \
                    0xB8, 0x01, 0x02, 0xBB, 0x00, 0x7C, 0x57, 0xCD, \
                    0x13, 0x5F, 0x73, 0x0C, 0x33, 0xC0, 0xCD, 0x13, \
                    0x4F, 0x75, 0xED, 0xBE, 0xA8, 0x06, 0xEB, 0x15, \
                    0xBF, 0xFE, 0x7D, 0x81, 0x3D, 0x55, 0xAA, 0x74, \
                    0x05, 0xBE, 0x8F, 0x06, 0xEB, 0x07, 0x8B, 0xF5, \
                    0xEA, 0x00, 0x7C, 0x00, 0x00, 0xAC, 0x0A, 0xC0, \
                    0x74, 0x0B, 0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00, \
                    0xCD, 0x10, 0x5E, 0xEB, 0xF0, 0xEB, 0xFE, 0x4D, \
                    0x69, 0x73, 0x73, 0x69, 0x6E, 0x67, 0x20, 0x6F, \
                    0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6E, 0x67, \
                    0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x00, \
                    0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x6C, 0x6F, \
                    0x61, 0x64, 0x69, 0x6E, 0x67, 0x20, 0x6F, 0x70, \
                    0x65, 0x72, 0x61, 0x74, 0x69, 0x6E, 0x67, 0x20, \
                    0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x00, 0x49, \
                    0x6E, 0x76, 0x61, 0x6C, 0x69, 0x64, 0x20, 0x70, \
                    0x61, 0x72, 0x74, 0x69, 0x74, 0x69, 0x6F, 0x6E, \
                    0x20, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x00, 0x42, \
                    0x56, 0x41, 0x4E }

#define sMBR_CODE 0xE3

    // ------------------------------------------------------------------------
    // 0100 FA            CLI
    // 0101 33C0          XOR AX,AX
    // 0103 8ED0          MOV SS,AX
    // 0105 BC007C        MOV SP,7C00
    // 0108 8BF4          MOV SI,SP
    // 010A 8EC0          MOV ES,AX
    // 010C 8ED8          MOV DS,AX
    // 010E FB            STI
    // 010F FC            CLD
    // 0110 BF0006        MOV DI,0600
    // 0113 B90001        MOV CX,0100
    // 0116 F2            REPNZ
    // 0117 A5            MOVSW
    // 0118 EA1D060000    JMP 0000:061D
    // 011D BEBE07        MOV SI,07BE
    // 0120 B304          MOV BL,04
    // 0122 803C80        CMP BYTE PTR [SI],80
    // 0125 740E          JZ  0135
    // 0127 803C00        CMP BYTE PTR [SI],00
    // 012A 751C          JNZ 0148
    // 012C 83C610        ADD SI,+10
    // 012F FECB          DEC BL
    // 0131 75EF          JNZ 0122
    // 0133 CD18          INT 18
    // 0135 8BEE          MOV BP,SI
    // 0137 8B14          MOV DX,[SI]
    // 0139 8B4C02        MOV CX,[SI+02]
    // 013C 83C610        ADD SI,+10
    // 013F FECB          DEC BL
    // 0141 740A          JZ  014D
    // 0143 803C00        CMP BYTE PTR [SI],00
    // 0146 74F4          JZ  013C
    // 0148 BEC706        MOV SI,06C7
    // 014B EB30          JMP 017D
    // 014D BF0400        MOV DI,0004
    // 0150 B80102        MOV AX,0201
    // 0153 BB007C        MOV BX,7C00
    // 0156 57            PUSH    DI
    // 0157 CD13          INT 13
    // 0159 5F            POP DI
    // 015A 730C          JNB 0168
    // 015C 33C0          XOR AX,AX
    // 015E CD13          INT 13
    // 0160 4F            DEC DI
    // 0161 75ED          JNZ 0150
    // 0163 BEA806        MOV SI,06A8
    // 0166 EB15          JMP 017D
    // 0168 BFFE7D        MOV DI,7DFE
    // 016B 813D55AA      CMP WORD PTR [DI],AA55
    // 016F 7405          JZ  0176
    // 0171 BE8F06        MOV SI,068F
    // 0174 EB07          JMP 017D
    // 0176 8BF5          MOV SI,BP
    // 0178 EA007C0000    JMP 0000:7C00
    // 017D AC            LODSB
    // 017E 0AC0          OR  AL,AL
    // 0180 740B          JZ  018D
    // 0182 56            PUSH    SI
    // 0183 B40E          MOV AH,0E
    // 0185 BB0700        MOV BX,0007
    // 0188 CD10          INT 10
    // 018A 5E            POP SI
    // 018B EBF0          JMP 017D
    // 018D EBFE          JMP 018D
    // ------------------------------------------------------------------------
    // 018F                                               4D                  M
    // 0190  69 73 73 69 6E 67 20 6F-70 65 72 61 74 69 6E 67   issing operating
    // 01A0  20 73 79 73 74 65 6D 00-45 72 72 6F 72 20 6C 6F    system.Error lo
    // 01B0  61 64 69 6E 67 20 6F 70-65 72 61 74 69 6E 67 20   ading operating
    // 01C0  73 79 73 74 65 6D 00 49-6E 76 61 6C 69 64 20 70   system.Invalid p
    // 01D0  61 72 74 69 74 69 6F 6E-20 74 61 62 6C 65 00      artition table.
    // ------------------------------------------------------------------------


                                        // -------------
                                        // 360k Floppies
                                        // -------------
#define BPB_360K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
                      0xD0, 0x02, 0xFD, 0x02, 0x00, 0x09, 0x00, 0x02 }

                                        // -------------
                                        // 720k Floppies
                                        // -------------
#define BPB_720K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
                      0xA0, 0x05, 0xF9, 0x03, 0x00, 0x09, 0x00, 0x02 }

                                        // -------------
                                        // 1.2M Floppies
                                        // -------------
#define BPB_12M     { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0xE0, 0x00, \
                      0x60, 0x09, 0xF9, 0x07, 0x00, 0x0F, 0x00, 0x02 }

                                        // --------------
                                        // 1.44M Floppies
                                        // --------------
#define BPB_144M    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0xE0, 0x00, \
                      0x40, 0x0B, 0xF0, 0x09, 0x00, 0x12, 0x00, 0x02 }

                                        // --------------
                                        // 2.88M Floppies
                                        // --------------
#define BPB_288M    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0xF0, 0x00, \
                      0x80, 0x16, 0xF0, 0x09, 0x00, 0x24, 0x00, 0x02 }

                                        // -------------
                                        // 160k Floppies
                                        // -------------
#define BPB_160K    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0x40, 0x00, \
                      0x40, 0x01, 0xFE, 0x01, 0x00, 0x08, 0x00, 0x01 }

                                        // -------------
                                        // 180k Floppies
                                        // -------------
#define BPB_180K    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0x40, 0x00, \
                      0x68, 0x01, 0xFC, 0x02, 0x00, 0x09, 0x00, 0x01 }

                                        // -------------
                                        // 320k Floppies
                                        // -------------
#define BPB_320K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
                      0x80, 0x02, 0xFF, 0x01, 0x00, 0x08, 0x00, 0x02 }

                                        // -------------
                                        // 640k Floppies
                                        // -------------
#define BPB_640K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
                      0x00, 0x05, 0xFB, 0x02, 0x00, 0x08, 0x00, 0x02 }

                                        // --------------------------
                                        // 1.2M Floppies (1k sectors)
                                        // --------------------------
#define BPB_12M_2   { 0x00, 0x04, 0x01, 0x01, 0x00, 0x02, 0xC0, 0x00, \
                      0xD0, 0x04, 0xFE, 0x02, 0x00, 0x08, 0x00, 0x02 }

                                        // --------------------------------
                                        // 1.68M DMF Floppies (1k clusters)
                                        // --------------------------------
#define BPB_1KDMF   { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x10, 0x00, \
                      0x20, 0x0D, 0xF0, 0x05, 0x00, 0x15, 0x00, 0x02 }

                                        // --------------------------------
                                        // 1.68M DMF Floppies (2k clusters)
                                        // --------------------------------
#define BPB_2KDMF   { 0x00, 0x02, 0x04, 0x01, 0x00, 0x02, 0x10, 0x00, \
                      0x20, 0x0D, 0xF0, 0x03, 0x00, 0x15, 0x00, 0x02 }


#define BPB_INDEX_360K      0x00
#define BPB_INDEX_720K      0x01
#define BPB_INDEX_12M       0x02
#define BPB_INDEX_144M      0x03
#define BPB_INDEX_288M      0x04
#define BPB_INDEX_160K      0x05
#define BPB_INDEX_180K      0x06
#define BPB_INDEX_320K      0x07
#define BPB_INDEX_640K      0x08
#define BPB_INDEX_12M_2     0x09
#define BPB_INDEX_1KDMF     0x0A
#define BPB_INDEX_2KDMF     0x0B

#define NUM_BPBs            0x0C

#define sBPB            0x10
#define BPB_OFFSET      0x0B
                                        // -----------------------------
#define BPB_MATCH_THRESHOLD     7       // out of 10 fields causes match
                                        // -----------------------------

#pragma pack ()
                                        // ----------
#endif                                  // BOOTDEFS_H
                                        // ----------
