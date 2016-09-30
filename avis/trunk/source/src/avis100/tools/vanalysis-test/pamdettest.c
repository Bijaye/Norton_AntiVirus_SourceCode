#include <stdlib.h>
#include <stdio.h>

typedef unsigned char uChar;

uChar p[1024 * 32];

int main (int argc, char** argv) {
   int jmpLocn, sigLocn;
   uChar byte;
   unsigned short word;

   char*  zName;
   FILE*  hFile;

   uChar  pgm[]   = "\xBC\x78\x01\xB1\x01\xFE\xC9\x75\xFC\xE9\xF4\x0D\xB1\x01\xFE\xC9\x75\xFC\xCD\x20";
   uChar* pNloop1 = pgm + 0x4;
   uChar* pAway   = pgm + 0xA;
   uChar* pNloop2 = pgm + 0xD;

   if (argc < 4) {
      puts ("PAMdetTest pgmName jmpLocn sigOffset");
      return 8;
   }
   zName = argv[1];
   jmpLocn = atoi(argv[2]);
   sigLocn = jmpLocn + atoi(argv[3]);
   if (sigLocn < jmpLocn+3 && sigLocn+3 >= jmpLocn) {
      puts ("signature overlaps code");
      return 4;
   }
   word = jmpLocn - 0xC;
   memcpy (pAway, &word, sizeof(word));

   memcpy (p, pgm, sizeof(pgm));

   p[jmpLocn] = '\xE9';
   word = 0xC - (jmpLocn+3);
   memcpy (p+jmpLocn+1, &word, sizeof(word));

   memcpy (p+sigLocn, "PPPP", 4);

   hFile = fopen(zName, "wb");
   if (!hFile) {
      perror("open");
      return 9;
   }
   fwrite (p, 1, 1024 * 32, hFile);
   fclose(hFile);
   return 0;
}
