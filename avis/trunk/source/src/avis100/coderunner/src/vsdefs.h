#ifndef __VSDEFS_H

#define __VSDEFS_H
#include "common.h"
/*
 * Virscan program's return values. Not used by IBMAV.
 */
#define NO_ERR    0    /* No error. Normal case. */
#define NORM_ERR  1    /* Error. Not virus related. */
#define SIG_FOUND 2    /* A viral signature was found. */


#define SIZE_TMP_LINE_BUF   512          /* Length of working buffer used */
                                         /*   reading signature file and */
                                         /*   filelist file */
#define MAX_SIZE_SIGN       (SIZE_TMP_LINE_BUF/2)
#define MAX_SIZE_MSG        (SIZE_TMP_LINE_BUF/2)
#ifndef TRUE                             /* Make sure TRUE and FALSE defined */
#define TRUE                (1)
#endif
#ifndef FALSE
#define FALSE               (!TRUE)
#endif
#ifndef SUCCESS
#define SUCCESS             0
#endif
#define XOR                 ^            /* The XOR operator */
#define MIN(A,B)            ((A) < (B) ? (A) : (B))
#define MAX(A,B)            ((A) > (B) ? (A) : (B))
#define NUMFRAGS 4          /* Not used by IBMAV */
#define SIZEFRAG 11         /* Not used by IBMAV */

#if defined(T_DOS)&&!defined(T_OS2)
#define BCDOS TRUE
#endif
#ifndef BCDOS
#define BCDOS FALSE
#endif

#ifndef BCOS2
#if defined(T_OS2)
#define BCOS2 TRUE
#else
#define BCOS2 FALSE
#endif
#endif

#ifndef __NETWARE_386__
#define __NETWARE_386__ FALSE
#endif

#if defined(T_DOS)
#define SIZE_WORK_BUF       0x5000       /* The buffer into which executable */
                                         /*   file data is read */
#else
#define SIZE_WORK_BUF       0xC000       /* The buffer into which executable */
                                         /*   file data is read */
#endif
#define DEBUGGERY           0            /* Turn on for debugging messages */
#define ULTRA_VERBOSE       FALSE        /* Extra messages when viruses found */
#define AIX3                FALSE        /* Set TRUE for AIX 3 version */
#define AS400               FALSE        /* Change to TRUE for AS/400 */
#define WATERLOO            FALSE        /* Change to TRUE for WATC VM/CMS ver */
#define IBMC_370            FALSE        /* Change to TRUE for IBM C/370 */
#define C_SET               FALSE        /* Change to TRUE for C/SET 2 */

#if !WATC
#if __NETWARE_386__
#define WATC                TRUE         /* Change to TRUE for WATCOM */
#else
#define WATC                FALSE        /* Change to TRUE for WATCOM */
#endif
#endif

#if AS400
#define SELF_TEST           FALSE        /* This is for the AS400 port */
#define SEARCH_DRIVE_ENABLE TRUE         /* For the AS400 port */
#define BOOTS               FALSE        /* For the AS400 port */
#define ASM_SCANLOOP        FALSE        /* Turn on for (fast) 80X86 assembler */
#define MEM_SCAN            FALSE
#endif
                                         /*   version of the scan loop */


#if     WATERLOO || IBMC_370 || AIX3
#define strerror(str) ""
#define SELF_TEST           FALSE
#define SEARCH_DRIVE_ENABLE FALSE
#define BOOTS               FALSE
#define ASM_SCANLOOP        FALSE
#define MEM_SCAN            FALSE
#define SEARCH_ALL          FALSE        /* Don't Search all local drives */
#define WHITESMITHS         FALSE
#define ESDC                FALSE
#endif

#if C_SET || WATC
#define strerror(str) ""
#define SELF_TEST           FALSE
#define SEARCH_DRIVE_ENABLE FALSE
#define BOOTS               FALSE
#define ASM_SCANLOOP        FALSE
#define MEM_SCAN            FALSE
#define SEARCH_ALL          FALSE
#define WHITESMITHS         FALSE
#define RealMode()          FALSE
#endif

#if defined(T_OS2)&&!defined(T_DOS)&&defined(WS_32)
#define SELF_TEST           FALSE
#define SEARCH_DRIVE_ENABLE FALSE
#define BOOTS               TRUE
#define ASM_SCANLOOP        FALSE
#define MEM_SCAN            FALSE
#define SEARCH_ALL          FALSE
#define WHITESMITHS         FALSE
#endif

#if defined(T_WINNT)
#define SELF_TEST           FALSE
#define SEARCH_DRIVE_ENABLE FALSE
#define BOOTS               TRUE
#define ASM_SCANLOOP        FALSE
#define MEM_SCAN            FALSE
#define SEARCH_ALL          FALSE
#define WHITESMITHS         FALSE
#endif

#if defined(T_WIN95)
#define SELF_TEST           FALSE
#define SEARCH_DRIVE_ENABLE FALSE
#define BOOTS               TRUE
#define ASM_SCANLOOP        FALSE
#define MEM_SCAN            TRUE
#define SEARCH_ALL          FALSE
#define WHITESMITHS         FALSE
#endif

#if defined(C_MSC6)
#define ASM_SCANLOOP        TRUE         /* Turn on for (fast) 80X86 assembler */
                                         /*   version of the scan loop */
#define BOOTS               TRUE         /* If this is TRUE, enable boot */
                                         /*   sector scan code. Some of this */
                                         /*   code is Family API, some will */
                                         /*   only function under DOS or the */
                                         /*   DOS box. */
                                         /*   Could be a define on compiler */
                                         /*   command line */
#define MEM_SCAN            TRUE
#if !ESDC
#define SELF_TEST           TRUE         /* If this is TRUE, code to perform */
                                         /*   an integrity test on VIRSCAN */
                                         /*   is included. This has only been */
                                         /*   tested for the PC versions and */
                                         /*   so the macro is defined in the */
                                         /*   PC make file */

#define SEARCH_DRIVE_ENABLE TRUE         /* If this is TRUE, OS/2 */
                                         /*   Family API support for */
                                         /*   searching an entire specified */
                                         /*   drive will be included. */
                                         /*   Could be a define on compiler */
                                         /*   command line */
#define SEARCH_ALL          TRUE         /* Search all local drives */
#else
#define SELF_TEST           FALSE
#define SEARCH_DRIVE_ENABLE FALSE
#define SEARCH_ALL          FALSE
#endif
#endif

#if defined (__BORLANDC__)&&(defined(T_DOS)||defined(T_WIN3X))
#define SELF_TEST           FALSE
#define SEARCH_DRIVE_ENABLE FALSE
#define BOOTS               TRUE
#define ASM_SCANLOOP        TRUE
#define MEM_SCAN            TRUE
#define SEARCH_ALL          FALSE
#define WHITESMITHS         FALSE
#endif

#define FREQ_COUNTS         0            /* Used in development only. If this */
                                         /*   is set to true, virscan will */
                                         /*   count characters, and display */
                                         /*   counts at program termination. */


#ifndef NULL
#define NULL                ((void *)0)  /* Make sure NULL is defined */
#endif

/* Standard object types used globally by various modules.
 */
#define T_FILE                 1
#define T_MASTER_BOOT_RECORD   2
#define T_SYSTEM_BOOT_SECTOR   3
#define T_BOOTMAN_BOOT_SECTOR  4
#define T_SYSTEM_MEMORY        5

/* Standard drive types used globally by various modules.
 */
#define DT_LOCAL_FIXED         1
#define DT_LOCAL_REMOVABLE     2
#define DT_REMOVABLE_NOT_READY 3
#define DT_NETWORK             4
#define DT_UNKNOWN             5
#define DT_INVALID_LETTER      6
#define DT_LOCKED              7
#define DT_LOCAL               8 /* Either local-removable or local-fixed */
#define DT_REMOVABLE_LOGICAL   9
#define DT_FATAL_ERROR        10
#define DT_IN_USE             11
#define DT_BAD_UNIT           12

/*
 * If PC version, we can use a larger "first_sig_chars" table.
 * (80X8X stores words low-order in low memory byte)
 * Otherwise, use a "first_sig_chars" table the same size as the hash
 * table.
 */
#if defined(T_DOS) || defined(T_WIN3X) || defined(T_OS2) || defined(T_WINNT) || defined(WATC) || defined(T_WIN95)
#define SIZE_FSC ((unsigned short int) 8192)
#define FSC_MASK ((unsigned short int) 0x1FFF)
#if defined(WS_32)
#define SIZE_HT  ((unsigned short int) 8192)
#define HT_MASK  ((unsigned short int) 0x1FFF)
#else
#define SIZE_HT  ((unsigned short int) 1024)
#define HT_MASK  ((unsigned short int) 0x03FF)
#endif
#else
#define SIZE_FSC ((unsigned short int) 256)
#define FSC_MASK ((unsigned short int) 0xFF)
#define SIZE_HT  ((unsigned short int) 256)
#define HT_MASK  ((unsigned short int) 0xFF)
#endif

/*
 * Make sure we retrieve correct the value of "product_version" from the message
 * file before using it.
 */
#define Product_Version (message(VERSION_FLAG), product_version)

/*
 * Make sure we retrieve correct the value of "os2_prod_version" from the message
 * file before using it.
 */
#define OS2_Prod_Version (message(VERSION_FLAG), os2_prod_version)

/*#include "dbmalloc.h"*/
/*#include "ldmalloc.h"*/

#if !(defined(T_DOS) || defined(T_WIN3X) || defined(T_OS2) || defined(T_WINNT) || defined(WATC)) || defined(T_WIN95)
#define CR_WORKS_DEF FALSE
#define getch getchar
#else
#define CR_WORKS_DEF TRUE
#endif

#define DEBUG_FUNCTIONS 0

/* Report in the interface record data structure at
 * most this many virus signatures per object.
 */
#define VIRUS_FOUND_BUCKET_MAX 64

#define MAIN_SIGFILENAME "virsig.lst"

/* If a version doesn't need or want compressed file scanning, turn it off
 * at compile time with /DSCAN_COMPRESSED_FILES=0
 */
#ifndef SCAN_COMPRESSED_FILES
#define SCAN_COMPRESSED_FILES 1
#endif

/*
 * Use these to turn on/off algorithmic detectors replaced by CodeRunner
 * plus search patterns.
 */
#ifndef INC_TREMDETU
#define INC_TREMDETU   0
#endif
#ifndef INC_SBGDETU
#define INC_SBGDETU    0
#endif
#ifndef INC_NATDETU
#define INC_NATDETU    1
#endif
#ifndef INC_SMGDETU
#define INC_SMGDETU    0
#endif

#endif
