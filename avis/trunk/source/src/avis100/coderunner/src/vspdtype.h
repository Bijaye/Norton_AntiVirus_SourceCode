#ifndef __VSPDTYPE_H__
#define __VSPDTYPE_H__

#include "common.h"

/* Interface record head */
 typedef  struct  {
   twobytes recordLength;
   twobytes recordType;
} irec_head;

/* General bucket structure head */
typedef struct  {
   twobytes bucketLength;
   twobytes bucketType;
}  bucket_head;

/*
 * "object_type" is one of
 *
 * T_FILE                1
 * T_MASTER_BOOT_RECORD  2
 * T_SYSTEM_BOOT_SECTOR  3
 * T_BOOTMAN_BOOT_SECTOR 4
 * T_SYSTEM_MEMORY       5
 *
 * These are defined in vspddefs.h
 *
 * "is_precise_match" is set TRUE if all of the signature was found. If
 * it is set FALSE, then the signature was not completely matched or
 * the hit was on a signature fragment. If the match was not precise,
 * then we already know that the VERV code will not verify the virus.
 *
 * "pause_if_found", "PIF_but_VSTOP_stops_it" and "scan_memory" are
 * used by the function "unVSTOPped_virus_present" to determine whether
 * it is safe to continue the scan.
 *
 * "suspicious_boot" is set if all we found was suspicious code in a boot
 * record.
 *
 * "inside_compressed" is set if a virus was found inside of a de-compressed
 * object.
 *
 * "family signature" is set if the signature was a family signature.
 * ("family signatures" are only reported if there wasn't a precise match
 * on some other signature.)
 */
typedef struct
{
  twobytes object_type;
  unsigned int is_precise_match : 1;       /* If all of the signature matched.*/

  unsigned int pause_if_found : 1;         /* Ignore these three bits.*/
  unsigned int PIF_but_VSTOP_stops_it : 1; /* Post-processed by routine*/
  unsigned int scan_memory : 1;            /* "unVSTOPped_virus_present()"*/
  unsigned int suspicious_boot : 1;        /* Suspicious boot record.*/
  unsigned int inside_compressed : 1;      /* Virus inside of compressed file.*/
  unsigned int family_signature : 1;       /* Found with family signature. */
} vfoundRecord;

/*
 * The first bucket returned when a virus is found contains the
 * fixed-length informational fields.
 */
typedef struct
{
   twobytes vfrBucketLength;
   twobytes vfrBucketType;            /* BID_VIRUS_FOUND_B1*/
   vfoundRecord vfrBucketData;   /* Defined above*/
} vfoundFirstBucket;

/*
 * An ASCIIZ virus name. There may be more than one name; if
 * so, the various names will be separated by commas
 * and/or "or"s. The virus name is extracted algorithmically by
 * virscan from the signature line.
 */
typedef struct
{
   twobytes vfmBucketLength;
   twobytes vfmBucketType;            /* BID_VIRUS_FOUND_B2*/
   char vfmBucketData[1];        /* The virus-found message (ASCIIZ)*/
} vfoundSecondBucket;

/*
 * ASCIIZ VERV code(s). There may be more than one VERV code, since
 * VIRSCAN doesn't identify viruses uniquely.
 * Multiple VERV codes are separated by blanks.
 * If the virus doesn't have a VERV code, this field will contain
 * the string "NoVervCode".
 */
typedef struct
{
   twobytes vcBucketLength;
   twobytes vcBucketType;             /* BID_VIRUS_FOUND_B3*/
   char vcBucketData[1];         /* VERV code(s) (ASCIIZ)*/
} vfoundThirdBucket;

/*
 * ASCIIZ object name, usable by VERV. The format will depend on the
 * object type (see above):
 * T_FILE                  : ASCIIZ pathname
 * T_MASTER_BOOT_RECORD    : ASCIIZ string, containing a two digit hex
 *                           string. Format is "%02X" (2 hex digit
 *                           hard drive address. 0x80 is first hard drive.
 * T_SYSTEM_BOOT_SECTOR    : ASCIIZ drivename. Of form "A:", etc.
 * T_BOOTMAN_BOOT_SECTOR   : ASCIIZ string, containing a two digit hex
 *                           string and a decimal partition number.
 *                           Format is "%02X %d"
 * T_SYSTEM_MEMORY         : ASCIIZ string, format "%04X0", which is the
 *                           segment (16K boundaries) in which the virus
 *                           signature was found. This is basically
 *                           useless information at the moment.
 */
typedef struct
{
   twobytes vnBucketLength;
   twobytes vnBucketType;             /* BID_VIRUS_FOUND_B4*/
   char vnBucketData[1];         /* Object name usable by VERV. (ASCIIZ)*/
} vfoundFourthBucket;
#endif
