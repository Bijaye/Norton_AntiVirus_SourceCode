#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include "dbutil.h"
#include "triedb.h"

/* This is a cheap log2 computation.                                                   */

unsigned long log2(unsigned long n)
{
   unsigned int log = 31;

   while (n)
   {
      if (n&0x80000000l)
         return  log;
      --log;

      n <<= 1;
   }
   return 0;
}

/* This is a cheap 2**N function                                                       */

unsigned long cnt_from_log2(unsigned int logcnt)
{
   return ((unsigned long)1) << logcnt;
}

static double maxlog = -1.0;

/* This routine expands a log count scaled to fit in a full byte.
                                                                                       */

unsigned long cnt_from_scaled_log(unsigned int logcnt)
{
   if (maxlog == -1.0)
      maxlog = log((double)ULONG_MAX);
   return (unsigned long)(exp((((double)logcnt-1.0)*maxlog)/((double)254)));
}

/* Convert compressed count (log) into count.
 */

unsigned long cnt_from_logcnt(unsigned int logcnt)
{
#if      0
   return  cnt_from_log2(logcnt);
#endif
#if      1
   return  cnt_from_scaled_log(logcnt);
#endif
}

/* We want to use the full resolution of a byte to store our
 * log of a count. Zero is already taken. Counts in file are
 * actually 0 or scaled_log(count)+1. Note we're rounding
 * up on N.5. This is probably not correct for our application, but
 * seems to work fine, and allows us to distinquish between 1 and 2
 * which might be important.
 */

unsigned int scaled_log(unsigned long n)
{
   double worklog;
   double wrv;
   unsigned int rv;

   if (maxlog == -1.0)
      maxlog = log((double)ULONG_MAX);
   worklog = log((double)n);
   wrv = (((double)254)*(worklog/maxlog));
   rv = ((unsigned int)(wrv+1.5));
#if      0
   printf("n=%09lu, wrv=%06.3f, rv=%03u, log2=%02u, l2c=%09lu, lsc=%09lu\n", n, wrv, rv,
      log2(n), cnt_from_log2(log2(n)), cnt_from_scaled_log(rv));
#endif
   return  rv;
}

/* Convert count into compressed (log) count.
 */

unsigned int logcnt(unsigned long n)
{
#if      0
   return  log2(n);
#endif
#if      1
   return  scaled_log(n);
#endif
}
