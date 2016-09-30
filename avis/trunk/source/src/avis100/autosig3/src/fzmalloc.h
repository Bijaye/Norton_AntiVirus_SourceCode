#if 0
  A small malloc-replacement that reduces overhead greatly in cases where
  many malloc()s are done with few or no intervening free()s, and then everything
  is free()d at once.   Overhead will generally be zero bytes per fzmalloc().
  On the other hand, fzfree()s generally do not recover any storage; storage
  is not recovered until fzmalloc_done().

  For instance:

     for (i=0;i<lots_and_lots;i++)
     {
        \* ... complex stuff of various kinds ... *\
        foffle[qox] = fzmalloc(booglewasher,SLOT);
        \* ... more complex stuff ... *\
     }

     \* All done *\
     for (i=0;i<qoxen;i++)
        fzfree(foffle[i],SLOT);
     fzmalloc_done(SLOT);
     \* Blithely ignore return code. *\

  Each function takes a slot number as the last argument.  Operations on
  different slots are independant; you can fzmalloc_done() one slot even
  while you (or someone else) may still be using pointers that were
  obtained from an fzmalloc() with a different slot number.
#endif

/* Use this instead of malloc().  All rules of malloc() apply; any pointers            */
/* returned by fzmalloc() should be passed to fzfree() before doing the                */
/* fzmalloc_done().  This is because some of them were really made by                  */
/* malloc(), and only fzfree() can tell which.  NOTE: pointers returned by             */
/* fzmalloc() *cannot* be realloc()d.                                                  */

void *fzmalloc(size_t size,unsigned int slot);

/* Use this instead of free().   It will generally not recover any storage,            */
/* but it will sometimes, so you still have to call it.  See comments for              */
/* fzmalloc().                                                                         */

void fzfree(void *p,unsigned int slot);

/* Use this to shut the whole system down.  Recovers any storage used by               */
/* the system, assuming that you've been good about fzfree()ing things.                */
/* Returns 0 if all well, non-zero if for instance you never used fzmalloc.            */

int fzmalloc_done(unsigned int slot);

/** Replacement for strdup; see comments on fzmalloc().  NOTE that these    *          */
/** pointers cannot be realloc()d.                                          *          */

char *fzstrdup(char *srcstr,unsigned int slot);
