// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/heurtok.h_v   1.8   10 Jul 1997 17:06:10   CNACHEN  $
//
// Description:
//      Heuristic behavior IDs for compilation/interpretation/analysis.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/heurtok.h_v  $
// 
//    Rev 1.8   10 Jul 1997 17:06:10   CNACHEN
// Added a new IF16 test.
// 
//    Rev 1.7   13 Jun 1997 11:42:50   CNACHEN
// Added keyboard input behavior and revector 0 behavior
// 
//    Rev 1.6   10 Jun 1997 11:52:26   CNACHEN
// Added prepending virus behaviors
// 
//    Rev 1.5   02 Jun 1997 17:19:48   CNACHEN
// Added bait-write check after e9
// 
// 
//    Rev 1.4   29 May 1997 13:30:48   CNACHEN
// Added support for heuristic sensitivity level
// 
//    Rev 1.3   27 May 1997 13:51:50   CNACHEN
// Added support for FILE_FIND_EXEC
// 
//    Rev 1.2   23 May 1997 15:28:28   CNACHEN
// Added header behavior
// 
//    Rev 1.1   22 May 1997 15:07:16   CNACHEN
// 
//    Rev 1.0   14 May 1997 13:27:28   CNACHEN
// Initial revision.
// 
//************************************************************************


#ifndef _HEURTOK_H
#define _HEURTOK_H

// ID values for behaviors

#define IF1_FILE_OPEN_READ_WRITE                0x0010
#define IF1_FILE_OPEN_READ_ONLY                 0x0011
#define IF1_FILE_OPEN_WRITE_ONLY                0x0012
#define IF1_FILE_OPEN_UNKNOWN_MODE              0x0013

#define IF2_FILE_READ_LESS_8                    0x0020
#define IF2_FILE_READ_1C_TO_1E                  0x0021
#define IF2_FILE_READ_128_PLUS                  0x0022
#define IF2_FILE_READ_GENERAL                   0x0023
#define IF2_FILE_READ_3_OR_4                    0x0024
#define IF2_FILE_READ_FFFF                      0x0025
#define IF2_FILE_READ_BAIT                      0x0026

#define IF3_FILE_WRITE_LESS_8                   0x0030
#define IF3_FILE_WRITE_1C_TO_1E                 0x0031
#define IF3_FILE_WRITE_128_PLUS                 0x0032
#define IF3_FILE_WRITE_GENERAL                  0x0033
#define IF3_FILE_WRITE_3_OR_4                   0x0034
#define IF3_FILE_WRITE_BAIT                     0x0035

#define IF4_FILE_SEEK_TOF                       0x0040
#define IF4_FILE_SEEK_EOF                       0x0041
#define IF4_FILE_SEEK_GENERAL                   0x0042

#define IF5_FILE_CLOSE                          0x0050

#define IF6_FILE_GET_ATTR                       0x0060
#define IF6_FILE_SET_ATTR                       0x0061

#define IF7_FILE_GET_FILE_DATE                  0x0070
#define IF7_FILE_SET_FILE_DATE                  0x0071

#define IF8_SYSTEM_GET_TIME                     0x0080
#define IF8_SYSTEM_GET_DATE                     0x0081

#define IF9_FILE_FIND_FIRST                     0x0090
#define IF9_FILE_FIND_NEXT                      0x0091

#define IF10_FILE_DELETE                        0x00a0
#define IF10_FILE_CREATE                        0x00a1
#define IF10_FILE_RENAME                        0x00a2
#define IF10_FILE_CHDIR                         0x00a3
#define IF10_FILE_GET_CUR_DIR                   0x00a4

#define IF11_FILE_GET_DTA                       0x00b0
#define IF11_FILE_SET_DTA                       0x00b1

#define IF12_SYSTEM_GET_VECTOR_21               0x00c0
#define IF12_SYSTEM_SET_VECTOR_21               0x00c1
#define IF12_SYSTEM_GET_VECTOR_13               0x00c2
#define IF12_SYSTEM_SET_VECTOR_13               0x00c3
#define IF12_SYSTEM_GET_VECTOR_24               0x00c4
#define IF12_SYSTEM_SET_VECTOR_24               0x00c5
#define IF12_SYSTEM_GET_VECTOR_LOW              0x00c6
#define IF12_SYSTEM_SET_VECTOR_LOW              0x00c7
#define IF12_SYSTEM_GET_VECTOR_HIGH             0x00c8
#define IF12_SYSTEM_SET_VECTOR_HIGH             0x00c9
#define IF12_SYSTEM_SET_VECTOR_DIRECT           0x00ca
#define IF12_SYSTEM_SET_VECTOR_0                0x00cb

#define IF13_SYSTEM_MEM_ALLOC                   0x00d0
#define IF13_SYSTEM_PROG_EXEC                   0x00d1

#define IF14_SYSTEM_AM_I_THERE                  0x00e0

#define IF15_INT_13_READ_MBR                    0x00f0
#define IF15_INT_13_WRITE_MBR                   0x00f1
#define IF15_INT_13_READ_GENERAL                0x00f2
#define IF15_INT_13_WRITE_GENERAL               0x00f3
#define IF15_INT_13_FORMAT                      0x00f4
#define IF15_INT_13_GENERAL                     0x00f5

#define IF16_FOUND_PRINT_STRING                 0x0100
#define IF16_FOUND_TERMINATE                    0x0101
#define IF16_FOUND_KEYBOARD_INPUT               0x0102
#define IF16_FOUND_PRINT_CHAR                   0x0103

// flags for other behavior

#define BF1_WRITE_TO_CS100                      0x0210
#define BF1_READ_MCB                            0x0211
#define BF1_WRITE_MCB                           0x0212
#define BF1_FILE_WRITE_CS100                    0x0213
#define BF1_REPMOVS_CS100                       0x0214

#define BF2_JUMP_TO_CS100                       0x0220
#define BF2_FAR_JUMP_BEFORE_ENTRY_EXE           0x0221
#define BF2_FIXUP_AFTER_EP                      0x0222
#define BF2_PUSH_RET                            0x0223
#define BF2_PUSH_RET_100                        0x0224

#define BF3_FOUND_CMP_AH_4B                     0x0230
#define BF3_FOUND_CMP_AH_3D                     0x0231
#define BF3_FOUND_CMP_ANY_E9                    0x0232
#define BF3_FOUND_CMP_ANY_MZ                    0x0233
#define BF3_FOUND_CMP_AM_I_THERE                0x0234
#define BF3_FOUND_CMP_AH_11                     0x0235
#define BF3_FOUND_CMP_AH_12                     0x0236
#define BF3_FOUND_CMP_AH_4E                     0x0237
#define BF3_FOUND_CMP_AH_4F                     0x0238


#define BF4_FOUND_FILE_READ_E9                  0x0240
#define BF4_FOUND_FILE_READ_MZ                  0x0241
#define BF4_FOUND_FILE_WRITE_E9                 0x0242
#define BF4_FOUND_FILE_WRITE_MZ                 0x0243
#define BF4_FOUND_MEM_WRITE_AFTER_E9            0x0244
#define BF4_FOUND_MEM_WRITE_AFTER_MZ            0x0245
#define BF4_FOUND_BAIT_WRITE_AFTER_E9           0x0246

#define BF5_FOUND_SUSPICIOUS_OPCODES            0x0250
#define BF5_FOUND_PUSHF_CALL_FAR                0x0251
#define BF5_FOUND_CALL_POP                      0x0252
#define BF5_FOUND_REPMOVS                       0x0253
#define BF5_FOUND_REPMOVS_CODE                  0x0254
#define BF5_FOUND_E9_AT_TOF                     0x0255
#define BF5_FOUND_SUSPICIOUS_INT                0x0256
#define BF5_FOUND_E80000                        0x0257
#define BF5_FOUND_IRET                          0x0258
#define BF5_FOUND_PUSHF_CALL_FAR_21             0x0259
#define BF5_FOUND_SUSPICIOUS_INT_21             0x025a

#define BF6_FOUND_SELF_MOD_CODE                 0x0260
#define BF6_CLOSE_MOVS_WRITE_MATCH              0x0261
#define BF6_EXACT_MOVS_WRITE_MATCH              0x0262
#define BF6_CLOSE_EP_TO_EOF_WRITE_MATCH         0x0263
#define BF6_EXACT_EP_TO_EOF_WRITE_MATCH         0x0264

// attributes of the file

#define BF7_ENTRY_AFTER_FIXUPS                  0x0270
#define BF7_WINDOWS_EXE                         0x0271
#define BF7_STOP_SCANNING                       0x0272
#define BF7_COM_FILE                            0x0273
#define BF7_EXE_FILE                            0x0274
#define BF7_FOUND_HEADER_IN_BODY                0x0275
#define BF7_FIND_FIRST_EXEC                     0x0276

#define BF8_STRING_CUSS_WORD                    0x0280
#define BF8_STRING_STAR_COM                     0x0281
#define BF8_STRING_STAR_EXE                     0x0282
#define BF8_STRING_COM                          0x0283
#define BF8_STRING_EXE                          0x0284
#define BF8_STRING_COPYRIGHT                    0x0285
#define BF8_STRING_COMMAND_COM                  0x0286
#define BF8_STRING_VIRUS                        0x0287
#define BF8_STRING_ALTERNATING_UP_LOW           0x0288
#define BF8_STRING_BY                           0x0289

#define MAX_USER_DEFINED_STRINGS                16

#define BF8_STRING_SLOT_0                       0x028a
#define BF8_STRING_SLOT_1                       0x028b
#define BF8_STRING_SLOT_2                       0x028c
#define BF8_STRING_SLOT_3                       0x028d
#define BF8_STRING_SLOT_4                       0x028e
#define BF8_STRING_SLOT_5                       0x028f
#define BF8_STRING_SLOT_6                       0x0290
#define BF8_STRING_SLOT_7                       0x0291
#define BF8_STRING_SLOT_8                       0x0292
#define BF8_STRING_SLOT_9                       0x0293
#define BF8_STRING_SLOT_10                      0x0294
#define BF8_STRING_SLOT_11                      0x0295
#define BF8_STRING_SLOT_12                      0x0296
#define BF8_STRING_SLOT_13                      0x0297
#define BF8_STRING_SLOT_14                      0x0298
#define BF8_STRING_SLOT_15                      0x0299

#define BF9_JUMP_AT_TOF_TO_MIDDLE               0x02b0
#define BF9_CALL_WITH_LARGE_DISP                0x02b1

#define HEURISTIC_LEVEL                         0x02d0

#define MAX_LITERAL_INDEX                       0x0300


// need to add support for other strings


// now #define all of our operators

#define OP_ADD                                  0x02e0
#define OP_SUB                                  0x02e1
#define OP_NEG                                  0x02e2
#define OP_MULT                                 0x02e3
#define OP_DIVIDE                               0x02e4
#define OP_SET                                  0x02e5
#define OP_EQ                                   0x02e6
#define OP_GREATER                              0x02e7
#define OP_GREATER_EQ                           0x02e8
#define OP_LESS                                 0x02e9
#define OP_LESS_EQ                              0x02ea
#define OP_LOGICAL_NOT                          0x02eb
#define OP_LOGICAL_XOR                          0x02ec
#define OP_LOGICAL_XOR3                         0x02ed
#define OP_LOGICAL_XOR4                         0x02ee
#define OP_LOGICAL_AND                          0x02ef
#define OP_LOGICAL_AND3                         0x02f0
#define OP_LOGICAL_AND4                         0x02f1
#define OP_LOGICAL_OR                           0x02f2
#define OP_LOGICAL_OR3                          0x02f3
#define OP_LOGICAL_OR4                          0x02f4
#define OP_TRUE                                 0x02f5
#define OP_FALSE                                0x02f6
#define OP_GOTO                                 0x02f7
#define OP_REPORT_VIRUS                         0x02f8
#define OP_IF_IS_FALSE_GOTO                     0x02f9
#define OP_END                                  0x02fa
#define OP_END_OF_IF_EXPRESSION                 0x02fb
#define OP_END_OF_DO_EXPRESSION                 0x02fc
#define OP_PRINT                                0x02fd
#define OP_PLUS_EQUAL                           0x02fe
#define OP_MINUS_EQUAL                          0x02ff
#define OP_FOUND_STRING                         0x0300

#endif
