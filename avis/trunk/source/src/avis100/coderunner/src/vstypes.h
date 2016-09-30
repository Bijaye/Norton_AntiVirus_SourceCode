#ifndef __VSTYPES_H

#define __VSTYPES_H
/*
 * Useful types.
 */
#include "common.h"
typedef unsigned int bOOlean;
typedef unsigned short int word;

/*
 * Signature data block. Pointers to these go into hash tables.
 */
typedef struct sigdats
{
  /* First byte (and a half) is tested with another table */
  byte second_byte;               /* 0 */
  byte third_byte;                /* 1 */
  byte *signature;                /* 2 */
  struct sigdats *next_sibling;   /* 6 */
  bOOlean com_files : 1;          /* 10 */
  bOOlean exe_files : 1;
  bOOlean pause_if_found : 1;
  bOOlean scan_memory : 1;
  bOOlean cim_sig : 1;
  bOOlean is_sig_frag : 1;
  bOOlean is_complex_sig : 1;
  bOOlean OLE2_executable : 1;
  bOOlean boot_records : 1;
  bOOlean is_vcomplex_sig : 1;
  bOOlean family_signature : 1;
  bOOlean disable_fragment_scan : 1;
  bOOlean epoint_only : 1;
  bOOlean has_vervcodes : 1;
  bOOlean compressor : 1;
  bOOlean PIF_but_VSTOP_stops_it : 1;

  byte max_mismatches_allowed;    /* 12 : Assumes bit fields are 16 bits */
  byte len_signature;             /* 13 */

  long offset;                    /* 14 */

  char *message;                  /* 18 */
} sigdat;

/*
 * Compact in-memory signature data block.
 * Pointers to these go into hash tables.
 * This structure type is used if the signature has been read from a
 * binary signature file, and is can be used in compact form, and
 * the program has been instructed to do so using the global variable
 * "compact_sigs_in_memory" prior to hash table build time. (i.e.
 * scanner code initialization.)
 */
typedef struct cim_sigdats
{
  /* First byte (and a half) is tested with another table */
  byte second_byte;               /* 0 */
  byte third_byte;                /* 1 */
  byte *signature;                /* 2 */
  struct sigdats *next_sibling;   /* 6 */
  bOOlean com_files : 1;          /* 10 */
  bOOlean exe_files : 1;
  bOOlean pause_if_found : 1;
  bOOlean scan_memory : 1;
  bOOlean cim_sig : 1;
  bOOlean is_sig_frag : 1;
  bOOlean is_complex_sig : 1;
  bOOlean OLE2_executable : 1;
  bOOlean boot_records : 1;
  bOOlean is_vcomplex_sig : 1;
  bOOlean family_signature : 1;
  bOOlean disable_fragment_scan : 1;
  bOOlean epoint_only : 1;
  bOOlean has_vervcodes : 1;
  bOOlean compressor : 1;
  bOOlean PIF_but_VSTOP_stops_it : 1;

  byte max_mismatches_allowed;    /* 12 */
  byte len_signature;             /* 13 */

  utwobytes sigfile_offset;       /* 14 */
  byte sigfile_offset_high;       /* 16 */
/*byte sigfile_number;*/          /* 17 */
} cim_sigdat;

/*
 * This structure combines the first_sig_chars array and the hash_table
 * array into a single object, so that the assembler version of the scanner
 * can know that they're in the same segment.
 */
typedef struct s_lookup_tables
{
  byte first_sig_chars[SIZE_FSC];   /* This table enables a quick test */
                                    /*   of whether or not a byte is the */
                                    /*   first byte of a signature. It */
                                    /*   is a performance hack that is only */
                                    /*   particularly significant if the */
                                    /*   pointers in 'hash_table' are 4 byte */
                                    /*   pointers and the machine can compare */
                                    /*   only 2 bytes at a time */
  sigdat *hash_table[SIZE_HT];      /* 'Hash value' is the first byte of a */
                                    /*   signature. Test for null may be */
                                    /*   more expensive than test of a */
                                    /*   byte, hence other table */
} lookup_tables;

/*
 * This information is use to pass information through the "SCANLOOP"
 * routine, so that parameters can be more easily added without changing
 * an assembler source file.
 */
typedef struct
{
   char *filename;
   bOOlean is_exe_type;
   long base;
   int object_type;
   char *vp_name;
   bOOlean inside_compressed;
   byte *buffer;
   unsigned long filelen;
   unsigned long saved_epoint;
   char *true_filename;
} auxiliary_scaninfo;

/*
 * This is what a DOS EXE header looks like. (Also the DOS stub header at
 * the front of an OS/2-only or Windows-only program.)
 */
typedef struct s_EXE_header
{
   word signature;                       /* 5a4d */
   word length_mod_512;                  /* just so */
   word file_size;                       /* ... in 512 byte paragraphs */
   word num_relocation_table_items;      /* just so */
   word size_of_header;                  /* ... in 16 byte paragraphs */
   word min_num_paragraphs;              /* ... needed above program */
   word max_num_paragraphs;              /* ... desired above program */
   word stack_segment_displacement;      /* ... 16 byte paragraphs */
   word stack_pointer_upon_entry;        /* just so */
   word word_checksum;                   /* negative sum of all words */
   word instruction_pointer_upon_entry;  /* just so */
   word code_segment_displacement;       /* ... 16 byte paragraphs */
   word offset_relocation_table;         /* offset of first relocation item */
   word overlay_number;                  /* 0 for resident part of program */
} EXE_header;

#endif
