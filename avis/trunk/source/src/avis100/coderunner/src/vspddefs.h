#ifndef __VSPDDEFS_H__
#define __VSPDDEFS_H__
#define VS_ERROR_BASE 0

/* VIRSCAN interface record types */
#define RID_VS_OK                  (VS_ERROR_BASE + 310)
#define RID_VS_ERROR               (VS_ERROR_BASE + 311)
#define RID_VS_VIRUS_FOUND         (VS_ERROR_BASE + 312)
#define RID_VS_ERROR_VIRUS_FOUND   (VS_ERROR_BASE + 313)

/* VIRSCAN bucket types */
/* When a reportable search pattern is found, these */
/*   buckets are added to the returned interface record */
/*   in this order */
#define BID_VIRUS_FOUND_B1         (VS_ERROR_BASE + 400)
#define BID_VIRUS_FOUND_B2         (VS_ERROR_BASE + 401)
#define BID_VIRUS_FOUND_B3         (VS_ERROR_BASE + 402)
#define BID_VIRUS_FOUND_B4         (VS_ERROR_BASE + 403)

/* VIRSCAN errors. */
#define FILE_OPEN_VSERROR          (VS_ERROR_BASE + 500)
#define FILE_READ_VSERROR          (VS_ERROR_BASE + 501)
#define SYSTEM_DATASET_VSERROR     (VS_ERROR_BASE + 502)
#define SBS_READ_VSERROR           (VS_ERROR_BASE + 520)
#define SBS_HPFS_READ_VSERROR      (VS_ERROR_BASE + 521)
#define MBR_READ_VSERROR           (VS_ERROR_BASE + 540)
#define BMBS_READ_VSERROR          (VS_ERROR_BASE + 560)
#define HEX2BYTE_CASE_VSERROR      (VS_ERROR_BASE + 580)
#define ODD_LEN_SIG_VSERROR        (VS_ERROR_BASE + 581)
#define BSIG_TOO_LONG_VSERROR      (VS_ERROR_BASE + 583)
#define ERR_READ_BSIG_VSERROR      (VS_ERROR_BASE + 584)
#define ERR_READ_LBSIG_VSERROR     (VS_ERROR_BASE + 585)
#define BMSG_TOO_LONG_VSERROR      (VS_ERROR_BASE + 586)
#define ERR_READ_BMSG_VSERROR      (VS_ERROR_BASE + 587)
#define ERROR_OPEN_SF_VSERROR      (VS_ERROR_BASE + 588)
#define ERROR_PARSE_LINE_VSERROR   (VS_ERROR_BASE + 590)
#define NO_WILDCARDS_VSERROR       (VS_ERROR_BASE + 591)
#define SIG_OVERFLOW_VSERROR       (VS_ERROR_BASE + 592)
#define CSIG_OVERFLOW_VSERROR      (VS_ERROR_BASE + 593)
#define SIG_TOO_SHORT_VSERROR      (VS_ERROR_BASE + 594)
#define ERR_READING_SML_VSERROR    (VS_ERROR_BASE + 595)
#define ERR_READING_STF_VSERROR    (VS_ERROR_BASE + 596)
#define INCOMPAT_BSIGF_VSERROR     (VS_ERROR_BASE + 597)
#define MTEDETU_VSERROR            (VS_ERROR_BASE + 600)
#define DPMI_VSERROR               (VS_ERROR_BASE + 610)   /* Windows only */
#define DPMI_VSERROR2              (VS_ERROR_BASE + 611)   /* Windows only */
#define DPMI_VSERROR3              (VS_ERROR_BASE + 612)   /* Windows only */
#define MEM_NOT_SCANNED_VSERROR    (VS_ERROR_BASE + 620)   /* Windows only */
#define MEM_WINOS2_OK_VSERROR      (VS_ERROR_BASE + 621)   /* Windows only */
#define INVALID_BOOTOBJECT_VSERROR (VS_ERROR_BASE + 630)
#define EXTERNAL_SCAN_TERM_VSERROR (VS_ERROR_BASE + 640)
#define WRITING_TMPFILE_VSERROR    (VS_ERROR_BASE + 700)
#define LZEXE_HEAD_NFOUND_VSERROR  (VS_ERROR_BASE + 710)
#define COMP_FILE_READ_VSERROR     (VS_ERROR_BASE + 720)
#define CORRUPT_COMP_FILE_VSERROR  (VS_ERROR_BASE + 730)
#define DIET_HEAD_NFOUND_VSERROR   (VS_ERROR_BASE + 740)
#define MISSING_VXD_VSERROR        (VS_ERROR_BASE + 750)

/* Object types. Returned in a field in the virus-found bucket number 1 */
#define T_FILE                 1
#define T_MASTER_BOOT_RECORD   2
#define T_SYSTEM_BOOT_SECTOR   3
#define T_BOOTMAN_BOOT_SECTOR  4
#define T_SYSTEM_MEMORY        5

/* Generally-agreed-upon program return codes.
 */
#define NO_ERROR                  0    /* No error that caused program */
                                       /* termination occurred, and no virus */
                                       /* was found. */

#define OUT_OF_MEM                5    /* Program ran out of memory */
#define INVALID_PARMS             10   /* Invalid command line parameter(s) */
#define INIT_ERROR                20   /* Error during initialization */
#define USER_ABORT                30   /* User stopped the program (?) */
#define CHECKING_ERROR            40   /* Severe error during checking */
#define INTERNAL_ERROR            60   /* Internal error (impossible?) */
#define PROGRAM_MODIFIED          70   /* Program was modified, perhaps by a */
                                       /* virus. */

/* The following 4 codes are in a hierarchy. The highest possible code */
/* should be returned. */
#define VIRUS_FOUND_ALL_FIXED     100  /* Viruses were found, but every */
                                       /* infected object was fixed. (Virus */
                                       /* was not found in memory). */
#define VIRUS_REMAINS_ON_DISKETTE 105  /* Virus was found on diskette, but */
                                       /* not on fixed disk or in memory. */
                                       /* The virus was not removed from one */
                                       /* or more diskettes. */
#define VIRUS_FOUND_NOT_ALL_FIXED 110  /* Virus was found on a fixed disk, */
                                       /* but not in memory. */
                                       /* The virus was not removed from one */
                                       /* or more objects on the fixed disk */
                                       /* Note that viruses may also be on */
                                       /* diskettes in this case */
#define VIRUS_FOUND_IN_MEMORY     120  /* Virus was found in memory. */

/* The following are compression types returned by file_may_be_compressed */
#define CT_NOT_COMPRESSED 0
#define CT_LZEXE          1
#define CT_PKLITE         2
#define CT_ZIP            3
#define CT_DIET           4
#define CT_DIET100        5

#endif
