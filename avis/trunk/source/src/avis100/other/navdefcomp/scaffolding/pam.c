#include <stdio.h>
#include <io.h>
#include <fcntl.h>

typedef unsigned long uLong;

long PAM (int HANDLE);


int main (int argc, char** argv) {
   int   HANDLE;
   uLong RC = 0;

   if (argc < 2) return 8;

   HANDLE = open(argv[1], O_RDWR | O_BINARY);   //O_RDWR
   if (HANDLE < 0) {
      fprintf(stderr,"%s   ",argv[1]);
      perror ("open");
      return 16;
   }

   RC = PAM(HANDLE);
   printf (">>> %ld %s\n", RC, !RC ? "" :"********************");

   close(HANDLE);
   return RC;
}
