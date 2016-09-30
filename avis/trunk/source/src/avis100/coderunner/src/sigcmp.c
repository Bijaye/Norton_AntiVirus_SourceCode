/*---------------------------------------------------------------------------*/
/* Search pattern comparators.                                               */
/*                                                                           */
/* One is used for matching complex signatures with variable-length          */
/* wildcards.                                                                */
/*                                                                           */
/* The other is the main search pattern comparator, that can call the        */
/* one for matching search patterns with variable length wildcards.          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
/*
 used to replace all occurrences with malloc(), strdup(), argv(),
            hmalloc(), cmalloc(), free()
*/
#ifdef S_H
#include "mymalloc.h"
#endif

#include "vsdefs.h"
#include "vstypes.h"

#include "sigcmp.pro"

#if 0
extern bOOlean tolerance_allowed;
#endif

/*
 * Match a very complex signature, i.e. one with n-byte don't cares.
 * This routine is recursive.
 */
bOOlean
match_vcomplex_sig(byte *masked_signature,
                   byte *test_addr,
                   int len)
{
   int i;
   register int j;
   register int N;
#if WATC
   static   unsigned long TS_counter=0l;
#endif


#if WATC
      if ((TS_counter++ % 1024)==0)
         IBMAVthreadSwitch();
#endif

   if (*masked_signature == (byte)(0xFE XOR 0xFF))
   {
      N = masked_signature[1] XOR 0xFF;
      for (j=0; j<=N; ++j)
      {
         if (match_vcomplex_sig(&masked_signature[2],
                                &test_addr[j],
                                len-(2)))
         {
            return TRUE;
         }
      }
      return FALSE;
   }
   else
   {
      if (((*masked_signature XOR (byte) 0xFF) != *test_addr) &&
          (*masked_signature != (byte)(0xFF XOR 0xFF))
         )
      {
         return FALSE;
      }
   }
   for (i=1; i<len; ++i)
   {
      if (masked_signature[i] == (byte)(0xFE XOR 0xFF))
      {
         N = masked_signature[i+1] XOR 0xFF;
         for (j=0; j<=N; ++j)
         {
            if (match_vcomplex_sig(&masked_signature[i+2],
                                   &test_addr[i+j],
                                   len-(i+2)))
            {
               return TRUE;
            }
         }
         return FALSE;
      }
      else
      {
         if (((masked_signature[i] XOR (byte) 0xFF) != test_addr[i]) &&
             (masked_signature[i] != (byte)(0xFF XOR 0xFF))
            )
         {
            return FALSE;
         }
      }
   }
   return TRUE;
}

/*
 * Compare masked signature with a test range of bytes.
 * A variable number of mismatched bytes are allowed, depending on the
 * length of the signature. The *mismatched_bytes_count value is only
 * set if the function returns TRUE;
 */
#if 0
int
mycmp(byte *masked_signature,
      byte *test_addr,
      register int len,
      int *mismatched_bytes_count,
      struct sigdats *sig)
{
   register int i;
   int tolerance;
   int mismatch_count;

   if (sig->is_vcomplex_sig)
   {
      if (!match_vcomplex_sig(&masked_signature[2],&test_addr[2],len-2))
         return 1;
      *mismatched_bytes_count = 0;
      return 0;
   }
   else
   if (sig->is_complex_sig)
   {    /* In this case, either we've already matched 3 bytes, or */
        /* the third byte is a FF metacharacter */
        /* If we using cim sigs, then we need to compare sig[i] with */
        /* test[i+3] with i starting at 0, else we need to compare */
        /* sig[i] to test[i] with i starting at 3. */
        /* To do this, shift starts of signatures. */
        /* Don't forget to shrink length if we're working */
        /* with a full search pattern and so mush shift */
        /* start of search pattern. */
      if (sig->cim_sig)
      {
         test_addr = &test_addr[3];
      }
      else
      {
         masked_signature = &masked_signature[3];
         test_addr = &test_addr[3];
         len -= 3;
      }
      if (tolerance_allowed)
      {
         mismatch_count = 0;
         tolerance = sig->max_mismatches_allowed;
         for (i=0; i<len; ++i)
            if ((masked_signature[i] XOR (byte) 0xFF) != test_addr[i] &&
                (masked_signature[i] != 0x00)) /* 0xFF XOR 0xFF==0x00 */
               if (++mismatch_count > tolerance)
                  return 1;
         *mismatched_bytes_count = mismatch_count;
      }
      else
      {
         for (i=0; i<len; ++i)
            if ((masked_signature[i] XOR (byte) 0xFF) != test_addr[i] &&
                (masked_signature[i] != 0x00)) /* 0xFF XOR 0xFF==0x00 */
               return 1;
         *mismatched_bytes_count = 0;
      }
      return 0;
   }
   else /* Signature is plain hex, neither very complex nor complex */
   {    /* In this case, we've already matched 3 bytes */
      if (sig->cim_sig)
      {
         test_addr = &test_addr[3];
      }
      else
      {
         masked_signature = &masked_signature[3];
         test_addr = &test_addr[3];
         len -= 3;
      }
      if (tolerance_allowed)
      {
         mismatch_count = 0;
         tolerance = sig->max_mismatches_allowed;
         for (i=0; i<len; ++i)
            if ((masked_signature[i] XOR (byte) 0xFF) != test_addr[i])
               if (++mismatch_count > tolerance)
                  return 1;
         *mismatched_bytes_count = mismatch_count;
      }
      else
      {
         for (i=0; i<len; ++i)
            if ((masked_signature[i] XOR (byte) 0xFF) != test_addr[i])
               return 1;
         *mismatched_bytes_count = 0;
      }
      return 0;
   }
}
#endif
