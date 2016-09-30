/* dbutil.h
 * Convert compressed count (log) into count.
 */

unsigned long cnt_from_logcnt(unsigned int logcnt);

/* Convert count into compressed (log) count.
 */

unsigned int logcnt(unsigned long n);
