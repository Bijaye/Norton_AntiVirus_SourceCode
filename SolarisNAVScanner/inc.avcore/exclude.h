// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/EXCLUDE.H_v   1.3   06 Sep 1996 14:04:20   CNACHEN  $
//
// Description:
//
//  This file contains constants for the different dynamic and static
//  exclusion categories supported by the PAM exclusion module.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/EXCLUDE.H_v  $
// 
//    Rev 1.3   06 Sep 1996 14:04:20   CNACHEN
// Updated header to use proper #pragma pack() for NTK/WIN32.
// 
//    Rev 1.3   06 Sep 1996 14:01:12   CNACHEN
// Added proper #pragma pack() commands for NTK/WIN32 platforms.
// 
//    Rev 1.2   15 Mar 1996 13:27:34   CNACHEN
// Added INVALID_INDEX exclusions...
// 
//    Rev 1.1   20 Feb 1996 13:48:24   CNACHEN
// Changed bitmap type to bitmap_t for Windows NT driver...
// 
// 
//    Rev 1.0   01 Feb 1996 10:17:44   CNACHEN
// Initial revision.
// 
//    Rev 1.7   23 Oct 1995 12:51:28   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.6   19 Oct 1995 18:39:18   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************


#define MAX_EXCLUDE_CAT     487

#define NUM_OPCODE_CAT      256
#define NUM_INT_21_CAT      128

#define OPCODE_BASE         0
#define INT_21_BASE         256

#define BAD_INT_05          384
#define BAD_INT_10          385
#define BAD_INT_12          386
#define BAD_INT_13          387
#define BAD_INT_16          388
#define BAD_INT_17          389
#define BAD_INT_1A          390
#define BAD_INT_20          391
#define BAD_INT_21          392
#define BAD_INT_25          393
#define BAD_INT_26          394
#define BAD_INT_27          395
#define BAD_INT_2F          396
#define BAD_INT_33          397
#define BAD_INT_67          398

#define BAD_COM             399
#define BAD_EXE             400
#define BAD_SYS             401

#define MAX_EXE_EOF_0P1K    402
#define MAX_EXE_EOF_0P2K    403
#define MAX_EXE_EOF_0P3K    404

#define MAX_EXE_EOF_1P0K    405
#define MAX_EXE_EOF_1P1K    406
#define MAX_EXE_EOF_1P2K    407
#define MAX_EXE_EOF_1P3K    408

#define MAX_EXE_EOF_2P0K    409
#define MAX_EXE_EOF_2P1K    410
#define MAX_EXE_EOF_2P2K    411
#define MAX_EXE_EOF_2P3K    412

#define MAX_EXE_EOF_3P0K    413
#define MAX_EXE_EOF_3P1K    414
#define MAX_EXE_EOF_3P2K    415
#define MAX_EXE_EOF_3P3K    416

#define MAX_EXE_EOF_4P0K    417
#define MAX_EXE_EOF_4P1K    418
#define MAX_EXE_EOF_4P2K    419
#define MAX_EXE_EOF_4P3K    420

#define MAX_EXE_EOF_5P0K    421
#define MAX_EXE_EOF_5P1K    422
#define MAX_EXE_EOF_5P2K    423
#define MAX_EXE_EOF_5P3K    424

#define MAX_EXE_EOF_6P0K    425
#define MAX_EXE_EOF_6P1K    426
#define MAX_EXE_EOF_6P2K    427
#define MAX_EXE_EOF_6P3K    428

#define MAX_EXE_EOF_7P0K    429
#define MAX_EXE_EOF_7P1K    430
#define MAX_EXE_EOF_7P2K    431
#define MAX_EXE_EOF_7P3K    432

#define MIN_EXE_EOF_0P1K    433
#define MIN_EXE_EOF_0P2K    434
#define MIN_EXE_EOF_0P3K    435

#define MIN_EXE_EOF_1P0K    436
#define MIN_EXE_EOF_1P1K    437
#define MIN_EXE_EOF_1P2K    438
#define MIN_EXE_EOF_1P3K    439

#define MIN_EXE_EOF_2P0K    440
#define MIN_EXE_EOF_2P1K    441
#define MIN_EXE_EOF_2P2K    442
#define MIN_EXE_EOF_2P3K    443

#define MIN_EXE_EOF_3P0K    444
#define MIN_EXE_EOF_3P1K    445
#define MIN_EXE_EOF_3P2K    446
#define MIN_EXE_EOF_3P3K    447

#define MIN_EXE_EOF_4P0K    448
#define MIN_EXE_EOF_4P1K    449
#define MIN_EXE_EOF_4P2K    450
#define MIN_EXE_EOF_4P3K    451

#define MIN_EXE_EOF_5P0K    452
#define MIN_EXE_EOF_5P1K    453
#define MIN_EXE_EOF_5P2K    454
#define MIN_EXE_EOF_5P3K    455

#define MIN_EXE_EOF_6P0K    456
#define MIN_EXE_EOF_6P1K    457
#define MIN_EXE_EOF_6P2K    458
#define MIN_EXE_EOF_6P3K    459

#define MIN_EXE_EOF_7P0K    460
#define MIN_EXE_EOF_7P1K    461
#define MIN_EXE_EOF_7P2K    462
#define MIN_EXE_EOF_7P3K    463

#define MIN_SIZE_1K         466
#define MIN_SIZE_2K         467
#define MIN_SIZE_4K         468
#define MIN_SIZE_8K         469

#define COM_JUMP_START      470
#define REP_MOVS            471
#define REP_STOS            472
#define CALL128             473
#define REP_SCAS            474
#define RELOC_AFTER_CSIP    475
#define STOP_AFTER_16       476
#define STOP_AFTER_1K       477
#define STOP_AFTER_16K      478
#define STOP_AFTER_32K      479
#define STOP_AFTER_48K      480
#define STOP_AFTER_64K      481
#define STOP_AFTER_80K      482
#define STOP_AFTER_96K      483
#define STOP_AFTER_128K     484
#define RUN_32              485
#define INVALID_INDEX_WRITE 486

#define MAX_DWORD           0xFFFFFFFFUL

#define SIG_EXCLUDE_SIZE        32      // 32 bytes are matched


#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif


typedef struct
{
    WORD    *bitmap;
} bitmap_t;


typedef struct
{
    bitmap_t    emask[MAX_EXCLUDE_CAT];
    WORD        cand_words;                 /* # of viruses/16 (& remainder word) */
    WORD        last_word_mask;             /* last WORD contains partial mask */
} exclude_type;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

