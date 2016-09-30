/*=====================================================================================*/
/* This module combines all the first byte index files into a single                   */
/* index file.                                                                         */
/*=====================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <assert.h>
#include <process.h>
#include "triedb.h"
void pr_usage(void)
{
   printf("combindx [ofname] [-lurpath]\n");
}

index indexes[MAX_NGRAM_LEN-2][256];
char *database_root = "c:\\triedb\\";  /* Default database root                        */
int main(int argc,char **argv)
{
   int i;
   char *combindx_name = NULL;

   for (i = 1; i < argc; ++i)
   {
      switch (argv[i][0])
      {
         case ('-') :
         case ('/') :
            if (!strnicmp(&argv[i][1], "lur", 3))
            {
               database_root = &argv[i][4];
               break;
            }
            else
               pr_usage();
            return 1;
            break;
         default  :
            combindx_name = argv[i];
            break;
      }
   }

   /************************************************************************************/
   /* Read in the individual index files                                               */
   /************************************************************************************/

   {
      char idx_name[256];
      FILE *ifp;
      unsigned long ifl;
      unsigned int i;
      unsigned int rc;
      unsigned int ngram_length;

      memset(indexes, 0, sizeof(indexes));
      for (ngram_length = 3; ngram_length <= MAX_NGRAM_LEN; ++ngram_length)
      {
         for (i = 0; i < 256; ++i)
         {
            sprintf(idx_name, "%strie%01u_%02X.idx", database_root, ngram_length, i);

            /***************************************************************************/
            /* printf("idx_name=(%s)\n", idx_name);                                    */
            /***************************************************************************/

            ifp = fopen(idx_name, "rb");
            if (ifp == NULL)
            {
               printf("Failed open of index file (%s)\n", idx_name);
               exit(1);
            }
            setbuf(ifp, NULL);
            ifl = filelength(fileno(ifp));
            assert(ifl == IDXLEN *sizeof(unsigned long));
            rc = (unsigned int)fread(&indexes[ngram_length-3][i], 1, ifl, ifp);
            if (rc != ifl)
            {
               printf("Failed read of index file (%s)\n", idx_name);
               exit(1);
            }
            fclose(ifp);
         }
      }
   }

   /************************************************************************************/
   /* Write out the combined index file.                                               */
   /************************************************************************************/

   {
      char cidx_name[256];
      FILE *ofp;

      if (combindx_name == NULL)
         sprintf(cidx_name, "%s%s", database_root, "trie.idx");
      else
         strcpy(cidx_name, combindx_name);
      ofp = fopen(cidx_name, "wb");
      if (ofp == NULL)
      {
         printf("Failed open of output index file (%s)\n", combindx_name);
         exit(1);
      }

      /*********************************************************************************/
      /* setvbuf(ofp, NULL, 32768, _IOFBF);                                            */
      /*********************************************************************************/

      setvbuf(ofp, NULL, _IOFBF, 32768);
      fwrite(indexes, sizeof(indexes), 1, ofp);
      fclose(ofp);
   }
   return 0;
}
