/*=====================================================================================*/
/* This module builds the index file for a fixed record length database file.          */
/* Index file parameters are in triedb.h                                               */
/*=====================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <assert.h>
#include "triedb.h"
unsigned int rlen = 0;
void pr_usage(void)
{
   printf("bldindex /rlen=D fname\n");
}

int proc_fname(char *fname)
{
   FILE *fp = NULL;
   FILE *ifp = NULL;
   unsigned char *buffer = NULL;
   long fl; /* breaks for big files */
   int rv = 0;
   int rc;
   unsigned long i, j;
   unsigned int prev_key = 0x0000;
   char idx_fname[256];
   unsigned long index = 0l;

   printf("processing (%s)\n", fname);
   fp = fopen(fname, "rb");
   if (fp == NULL)
   {
      rv = 1;
      goto done_proc_fname;
   }

   setbuf(fp, NULL);
   fl = filelength(fileno(fp));
   assert(rlen != 0);
   assert(fl%rlen == 0);
   buffer = malloc(fl);
   if (buffer == NULL)
   {
      rv = 1;
      goto done_proc_fname;
   }

   rc = fread(buffer, 1, fl, fp);
   if (rc != fl)
   {
      rv = 1;
      goto done_proc_fname;
   }

   strcpy(idx_fname, fname);
   if (strrchr(idx_fname, '.'))
   {
      strcpy(strrchr(idx_fname, '.'), ".idx");
   }
   else
   {
      strcat(idx_fname, ".idx");
   }
   printf("idx_fname=(%s)\n", idx_fname);
   ifp = fopen(idx_fname, "wb");
   if (ifp == NULL)
   {
      rv = 1;
      goto done_proc_fname;
   }

   fwrite(&index, sizeof(unsigned long), 1, ifp);
   for (i = 0; i < (unsigned long)fl; i += rlen)
   {
      if (KEY(&buffer[i]) > prev_key)
      {
         index = i;
         for (j = 0; j < KEY(&buffer[i])-prev_key; ++j)
            fwrite(&index, sizeof(unsigned long), 1, ifp);
         prev_key = KEY(&buffer[i]);
      }
   }
   index = fl-rlen;
   for (j = 0; j < MAXKEY-prev_key; ++j)
      fwrite(&index, sizeof(unsigned long), 1, ifp);
done_proc_fname:
   if (buffer != NULL)
      free(buffer);
   if (fp != NULL)
      fclose(fp);
   if (ifp != NULL)
      fclose(fp);
   return  rv;
}

void proc_fname_list(char *fname_list, unsigned long *proc_cnt)
{
   FILE *lfp = NULL;
   char worknbuf[256];
   char fname[256];

   lfp = fopen(fname_list, "r");
   if (lfp == NULL)
      goto done_proc_fname_list;

   for (; ; )
   {
      if (fgets(worknbuf, 256, lfp) == NULL)
         goto done_proc_fname_list;

      sscanf(worknbuf, "%s", fname);
      if (proc_fname(fname) == 0)
         ++(*proc_cnt);
   }
done_proc_fname_list:
   if (lfp != NULL)
      fclose(lfp);
   return ;
}

int quiet_mode = 0;
int main(int argc, char **argv)
{
   unsigned long proc_cnt = 0;
   int i;

   if (argc < 3)
   {
      pr_usage();
      return 1;
   }
   for (i = 1; i < argc; ++i)
   {
      switch (argv[i][0])
      {
         case ('-') :
         case ('/') :
            if (toupper(argv[i][1]) == 'Q')
            {
               quiet_mode = 1;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "rlen=", 5))
            {
               sscanf(&argv[i][6], "%01u", &rlen);
               printf("rlen=%01u\n", rlen);
               break;
            }
            pr_usage();
            return 1;
            break;
         case ('@') :
            proc_fname_list(&argv[i][1], &proc_cnt);
            break;
         default  :
            if (proc_fname(argv[i]) == 0)
               ++proc_cnt;
            break;
      }
   }
   return 0;
}
