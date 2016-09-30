/************************** Start of BITIO.C *************************
 *
 * This utility file contains all of the routines needed to impement
 * bit oriented routines under either ANSI or K&R C.  It needs to be
 * linked with every program used in the entire book.
 *
                                                                                                                                 */

#include <stdio.h>
#include <stdlib.h>
#include "bitio.h"
#include "errhand.h"
#define  PACIFIER_COUNT 2047

int
getbufc(BIT_BUF *bit_buf)
{
   if (bit_buf->bufpos >= bit_buf->buflen)
      return EOF;
   else
      return (int) (bit_buf->buf[bit_buf->bufpos++]);
}

int
putbufc(int c, BIT_BUF *bit_buf)
{
   if (bit_buf->bufpos >= bit_buf->buflen)
      return EOF;
   else
   {
      bit_buf->buf[bit_buf->bufpos++] = (unsigned char) c;
      return c;
   }
}

BIT_BUF *OpenOutputBitBuf(unsigned char *outbuf, unsigned int buflen)
{
   BIT_BUF *bit_buf;

   bit_buf = (BIT_BUF *)calloc(1, sizeof(BIT_BUF));
   if (bit_buf == NULL)
      return (bit_buf);
   bit_buf->buf = outbuf;
   bit_buf->buflen = buflen;
   bit_buf->bufpos = 0l;
   bit_buf->rack = 0;
   bit_buf->mask = 0x80;
   bit_buf->pacifier_counter = 0;
   return (bit_buf);
}

BIT_BUF *OpenInputBitBuf(unsigned char *inbuf, unsigned int buflen)
{
   BIT_BUF *bit_buf;

   bit_buf = (BIT_BUF *)calloc(1, sizeof(BIT_BUF));
   if (bit_buf == NULL)
      return (bit_buf);
   bit_buf->buf = inbuf;
   bit_buf->buflen = buflen;
   bit_buf->bufpos = 0l;
   bit_buf->rack = 0;
   bit_buf->mask = 0x80;
   bit_buf->pacifier_counter = 0;
   return (bit_buf);
}

void CloseOutputBitBuf(BIT_BUF *bit_buf)
{
   if (bit_buf->mask != 0x80)
      if (putbufc(bit_buf->rack, bit_buf) != bit_buf->rack)
         fatal_error("Fatal error in CloseBitBuf!\n");
   free((char *)bit_buf);
}

void CloseInputBitBuf(BIT_BUF *bit_buf)
{
   free((char *)bit_buf);
}

void OutputBit(BIT_BUF *bit_buf, int bit)
{
   if (bit)
      bit_buf->rack |= bit_buf->mask;
   bit_buf->mask >>= 1;
   if (bit_buf->mask == 0)
   {
      if (putbufc(bit_buf->rack, bit_buf) != bit_buf->rack)
         fatal_error("Fatal error in OutputBit!\n");
      else
         if ((bit_buf->pacifier_counter++&PACIFIER_COUNT) == 0)
            putc('.', stdout);
      bit_buf->rack = 0;
      bit_buf->mask = 0x80;
   }
}

void OutputBits(BIT_BUF *bit_buf, unsigned long code, int count)
{
   unsigned long mask;

   mask = 1L << (count-1);
   while (mask != 0)
   {
      if (mask&code)
         bit_buf->rack |= bit_buf->mask;
      bit_buf->mask >>= 1;
      if (bit_buf->mask == 0)
      {
         if (putbufc(bit_buf->rack, bit_buf) != bit_buf->rack)
            fatal_error("Fatal error in OutputBit!\n");
         else
            if ((bit_buf->pacifier_counter++&PACIFIER_COUNT) == 0)

               putc('.', stdout);
         bit_buf->rack = 0;
         bit_buf->mask = 0x80;
      }
      mask >>= 1;
   }
}

int InputBit(BIT_BUF *bit_buf)
{
   int value;

   if (bit_buf->mask == 0x80)
   {
      bit_buf->rack = bit_buf->buf[bit_buf->bufpos++];
      if (bit_buf->rack == EOF)
         fatal_error("Fatal error in InputBit!\n");
      if ((bit_buf->pacifier_counter++&PACIFIER_COUNT) == 0)
         putc('.', stdout);
   }
   value = bit_buf->rack&bit_buf->mask;
   bit_buf->mask >>= 1;
   if (bit_buf->mask == 0)
      bit_buf->mask = 0x80;
   return (value?1:0);
}

unsigned long InputBits(BIT_BUF *bit_buf, int bit_count)
{
   unsigned long mask;
   unsigned long return_value;

   mask = 1L << (bit_count-1);
   return_value = 0;
   while (mask != 0)
   {
      if (bit_buf->mask == 0x80)
      {
         bit_buf->rack = getbufc(bit_buf);
         if (bit_buf->rack == EOF)
            fatal_error("Fatal error in InputBit!\n");
         if ((bit_buf->pacifier_counter++&PACIFIER_COUNT) == 0)
            putc('.', stdout);
      }
      if (bit_buf->rack&bit_buf->mask)
         return_value |= mask;
      mask >>= 1;
      bit_buf->mask >>= 1;
      if (bit_buf->mask == 0)
         bit_buf->mask = 0x80;
   }
   return (return_value);
}

void FilePrintBinary(FILE *file, unsigned int code, int bits)
{
   unsigned int mask;

   mask = 1 << (bits-1);
   while (mask != 0)
   {
      if (code&mask)
         fputc('1', file);
      else
         fputc('0', file);
      mask >>= 1;
   }
}

/*************************** End of BITIO.C *************************                                                            */
