#if defined (__cplusplus)
extern "C" {
#endif

/* Self-priming; no need to initialize.                                                */
/* "the_database_root" needs a trailing backslash.                                     */
unsigned long lookup_ngram_count(unsigned char *ngram,
                                 unsigned int len,
                                 const char *the_database_root);

/* Default noisy. Set to one to turn off messages during lookup.                       */

extern unsigned int quiet_mode;

/* Set to 1 to use indexed binary search. Default is off, i.e. by default will do
 * a full binary search of database files.
                                                                                       */

extern unsigned int use_index;

/* Set to 1 to load indexes from single combined index file. By default, if
 * indexes are used they in multiple index files.
                                                                                       */

extern unsigned int use_single_index;

/* Call after completion. Not necessary, just available for people who like to wrap    */
/* everything up.                                                                      */
void close_all_db_files(void);

/* Score buffer for exact matches, no wildcards and no mismatches.                     */
/* Will be a negative number; larger absolute value is better.                         */
/* "dbr" needs a trailing backslash.                                                   */
double
scorebuf(const unsigned char *buf,
         unsigned int len,
         const char *dbr);/* database root */

#if defined (__cplusplus)
}
#endif
