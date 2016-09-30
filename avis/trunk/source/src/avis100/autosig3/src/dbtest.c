#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dblookup.h"

#include "bcounts.h"

void
pr_usage(void)
{
   printf("dbtest [/dbr=dbrootname\\]\n");
}
            
#define N 24
int main(int argc, char **argv)
{
   unsigned char buf[N];
   int i;
   char *dbr = "e:\\corpusdb\\";

   for (i=1; i<argc; ++i)
   {
      switch(argv[i][0])
      {
         case('-'):
         case('/'):
            if (!strnicmp(&argv[i][1], "dbr=", 4))
            {
               dbr = strdup(&argv[i][5]);
            }
            break;
         default:
            pr_usage();
            return;
            break;
      }
   }


   //for (i=0; i<N; ++i) buf[i] = (unsigned char)i;
   //for (i=0; i<N; ++i) buf[i] = (unsigned char)rand();
   //for (i=0; i<N; i+=2) buf[i] = (unsigned char)rand();
   fill_buf_bytefreq(buf, N);

   printf("scorebuf(buf) = %f\n",  scorebuf(buf, N, dbr));
}

