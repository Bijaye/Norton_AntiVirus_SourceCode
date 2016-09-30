#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "dblookup.h"

#define BFRLEN (8192)

void
pr_usage(void)
{
   fprintf(stderr,"DBscore [-dbr=DBrootname\\] [[-hex=]xxxx...]* [@response-file]*\n"
                  "\tOne parameter per line in a response file, thanks!\n");
}
            
void
hex2bin(unsigned char *dstbuf, 
        const char *srcbuf, 
        unsigned int dstbuflen, 
        unsigned int srcbuflen)
{
   unsigned int i;
   
   assert(srcbuflen % 2 == 0);
   for (i=0; i<srcbuflen; i+=2)
   {
      char wbuf[3];
      assert(isxdigit(srcbuf[i]));
      wbuf[0] = srcbuf[i];
      assert(isxdigit(srcbuf[i+1]));
      wbuf[1] = srcbuf[i+1];
      wbuf[2] = '\0';
      sscanf(wbuf, "%02X", &dstbuf[i/2]);
   }
}
      
void pr_hex(unsigned char *buf, unsigned int len)
{
   unsigned int i;

   for (i = 0; i < len; ++i)
   {
      printf("%02X", buf[i]);
   }
   printf("\n");
}



char* chomp (char* z) {
    char* Z = z + strlen(z)-1;
    if (*Z == '\n') *Z = '\0';
    return z;
}


// (re)start by passing main's (argc,argv), else pass (0,NULL)
const char* nextArg (int C, const char** V) {
    static int argc;
    static int argn;
    static const char** argv;
    static FILE* hResp = NULL;
    static char  zResp [BFRLEN*2+1];

    if (V) {   //(re)start
        argc = C;
        argv = V;
        argn = 0;
        if (hResp) fclose (hResp);
        hResp = NULL;
    }

    for (;;) {
       if (hResp) {  // currently reading a response file
           if (fgets(zResp,BFRLEN*2+1,hResp)) return chomp(zResp);
           if (ferror(hResp)) break;
           fclose(hResp);
           hResp = NULL;
       }

       if (++argn >= argc) return NULL;
       if (argv[argn][0] != '@') return argv[argn];

       hResp = fopen(argv[argn]+1,"r");
       if (!hResp) break;
    }

    fputs(argv[argn]+1,stderr);
    perror(" - response file error");
    exit (2);
}
            

void main(int argc, const char **argv)
{
   unsigned char buf[BFRLEN];
   const char*   arg;
   const char*   dbr = ".\\";
   unsigned int  hexlen;
   double        score;

   for (arg = nextArg(argc, argv); arg; arg = nextArg(0,NULL)) {

       if (strspn(arg,"-/") && !strnicmp(arg+1,"hex=",4)) arg += 5;
       if (isxdigit(arg[0])) continue;

       if (strspn(arg,"-/") && !strnicmp(arg+1,"dbr=",4)) {
          dbr = strdup(arg + 5);
          continue;
       }
       pr_usage();
       exit(1);
   }

   for (arg = nextArg(argc, argv); arg; arg = nextArg(0,NULL)) {
       if (strspn(arg,"-/") && !strnicmp(arg+1,"hex=",4)) arg += 5;
       if (!isxdigit(arg[0])) continue;

       hexlen = strlen(arg);
       hex2bin(buf, arg, BFRLEN, hexlen);
       score = scorebuf(buf, hexlen/2, dbr);
       printf ("%f %s\n", score, arg);
   }
   exit(0);
}

