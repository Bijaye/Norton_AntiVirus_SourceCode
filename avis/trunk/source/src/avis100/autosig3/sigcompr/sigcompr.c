/************************* Start of MAIN-C.C ************************                                                            */
/*
 * This is the driver program used when testing compression algorithms.
 * In order to cut back on repetitive code, this version of main is
 * used with all of the compression routines.  It in order to turn into
 * a real program, it needs to have another module that supplies one
 * routine and two strings, namely:
 *
 *     void CompressFile( BIT_BUF *input, BIT_BUF *output,
 *                        int argc, char *argv );
 *     char *CompressionName;
 *
 * The main() routine supplied here has the job of checking for valid
 * input and output files, opening them, and then calling the
 * compression routine.  If the files are not present, or no arguments
 * are supplied, it prints out an error message, which includes the
 * All of the
 * routines and strings needed by this routine are defined in the
 * sigcompr.h header file.
 *
 * After this is built into a compression program of any sort, the
 * program can be called like this:
 *
 *   main-c infile outfile [ options ]
 *
                                                                                                                                 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "bitio.h"
#include "errhand.h"
#include "sigcompr.h"
void usage_exit(char *prog_name);

int print_ratios(BIT_BUF *input,BIT_BUF *output);
#define  BIT_BUF_SIZE 2048

/*
 * Remove junk (including tabs) from string. String is converted in
 * place. Copies through trailing nul.
 * Used only when reading ASCII signature file.
 */
char *
remove_junk(char *str)
{
   register int dst_ind;
   register int src_ind;
   char val;

   for (src_ind=0, dst_ind=0;           /* Indices start equal */
        (val = str[src_ind]) != '\0';   /* Do till end of string */
        ++src_ind)                      /* Always increment src_ind */
   {
      if (val != ' ' &&                 /* If not space and */
          val != '\t' &&                /* not tab and not ? */
          val != '?')                   /* and indices not equal */
      {
         if (val == '%')
         {
            ++src_ind;
            if (str[src_ind] == '\0') break;
         }
         else
         {
            if (dst_ind != src_ind)        /* then copy byte from source */
               str[dst_ind] = val;         /* to destination */
            ++dst_ind;                     /* and bump dst_ind regardless */
         }
      }
   }
   str[dst_ind] = (char)'\0';           /* Don't forget trailing nul */
   return str;                          /* Return pointer to the string */
}
/*
 * Interpret character value as a hex char, return numerical value.
 * Slow and portable.
 */

unsigned char
charhextobyte(unsigned char charhex)
{
   switch(toupper(charhex))
   {
      case('0'): return 0x0;
      case('1'): return 0x1;
      case('2'): return 0x2;
      case('3'): return 0x3;
      case('4'): return 0x4;
      case('5'): return 0x5;
      case('6'): return 0x6;
      case('7'): return 0x7;
      case('8'): return 0x8;
      case('9'): return 0x9;
      case('A'): return 0xA;
      case('B'): return 0xB;
      case('C'): return 0xC;
      case('D'): return 0xD;
      case('E'): return 0xE;
      case('F'): return 0xF;
      default:
         printf("Bad case in charhextobyte(), val=%02.2X. Valid values are 0-9,A-F.\n", charhex);
         exit(4);
         break;
   }
   return 0x0;  /* IBM C/2 Compiler complains without this line */
}

/*
 * Convert a string of character data representing a hex string to
 * a hex string. Return the length of the output hex string in bytes.
 * Warning - this may not be portable.
 */
int
stringtohex(unsigned char *hexstring, char *bytestring)
{
   int i;
   int len;

   len=strlen(bytestring);
   if (len%2) printf("warning - odd length signature - last digit ignored\n");
   memset(hexstring,0,len/2);
   for (i=0; i<len; ++i)
   {
      if (i%2)
         hexstring[i/2] |= charhextobyte(bytestring[i]);
      else
         hexstring[i/2] |= (unsigned char) (charhextobyte(bytestring[i]) << 4);
   }
   /*pr_hex(hexstring,len/2+len%2);*/
   return len/2+len%2;
}


int main(int argc, char *argv[])
{
   BIT_BUF *output;
   BIT_BUF *input;
   unsigned char *outbitbuf;
   unsigned char *inbitbuf;
   int len = 0;
   int rv = 0;

   outbitbuf = calloc(BIT_BUF_SIZE, 1);
   assert(outbitbuf != NULL);
   inbitbuf = calloc(BIT_BUF_SIZE, 1);
   assert(inbitbuf != NULL);

   setbuf(stdout, NULL);
   if (argc < 2)
      usage_exit(argv[0]);

   remove_junk(argv[1]);
   len += stringtohex(&inbitbuf[len], argv[1]);

   input = OpenInputBitBuf(inbitbuf, len);
   if (input == NULL)
      fatal_error("Error opening %s for input\n", argv[2]);

   output = OpenOutputBitBuf(outbitbuf, BIT_BUF_SIZE);
   if (output == NULL)
      fatal_error("Error opening %s for output\n", argv[2]);

   printf("\nCompressing %s\n", argv[1]);
   printf("Using %s\n", CompressionName);
   CompressFile(input, output, argc-3, argv+3);
   CloseOutputBitBuf(output);
   rv = print_ratios(input, output);
   return rv;
}

/*
 * This routine just wants to print out the usage message that is
 * called for when the program is run with no parameters.  The first
 * part of the Usage statement is supposed to be just the program
 * name.  argv[ 0 ] generally holds the fully qualified path name
 * of the program being run.  I make a half-hearted attempt to strip
 * out that path info and file extension before printing it.  It should
 * get the general idea across.
                                                                                                                                 */

void usage_exit(char *prog_name)
{
   char *short_name;
   char *extension;

   short_name = strrchr(prog_name, '\\');
   if (short_name == NULL)
      short_name = strrchr(prog_name, '/');
   if (short_name == NULL)
      short_name = strrchr(prog_name, ':');
   if (short_name != NULL)
      short_name++;
   else
      short_name = prog_name;
   extension = strrchr(short_name, '.');
   if (extension != NULL)
      *extension = '\0';
   printf("\nUsage:  %s %s\n", short_name, "inhexfile");
   exit(2);
}

/*
 * This routine prints out the compression ratios after the input
 * and output files have been closed.
                                                                                                                                 */

int print_ratios(BIT_BUF *input, BIT_BUF *output)
{
   long input_size;
   long output_size;
   int ratio;
   int rv = 0;

   input_size = input->buflen;
   if (input_size == 0)
      input_size = 1;
   output_size = output->bufpos;
   ratio = 100-(int)(output_size *100L/input_size);
   printf("\nInput bytes:        %ld\n", input_size);
   printf("Output bytes:       %ld\n", output_size);
   if (output_size == 0)
      output_size = 1;
   printf("Compression ratio:  %d%%\n", ratio);
   if (input_size > output_size && output_size < 14)
      { rv = 10; }
   return rv;
}

/************************** End of MAIN-C.C *************************                                                            */
