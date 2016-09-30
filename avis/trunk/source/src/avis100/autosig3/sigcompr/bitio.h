/************************** Start of BITIO.H *************************/

#ifndef _BITIO_H
#define _BITIO_H

#include <stdio.h>

typedef struct bit_buf {
    unsigned char *buf;
    unsigned int buflen;
    unsigned int bufpos;
    unsigned char mask;
    int rack;
    int pacifier_counter;
} BIT_BUF;

BIT_BUF     *OpenInputBitBuf( unsigned char *inbuf, unsigned int buflen);
BIT_BUF     *OpenOutputBitBuf( unsigned char *outbuf, unsigned int buflen);
void          OutputBit( BIT_BUF *bit_buf, int bit );
void          OutputBits( BIT_BUF *bit_buf,
                          unsigned long code, int count );
int           InputBit( BIT_BUF *bit_buf );
unsigned long InputBits( BIT_BUF *bit_buf, int bit_count );
void          CloseInputBitBuf( BIT_BUF *bit_buf );
void          CloseOutputBitBuf( BIT_BUF *bit_buf );
void          FilePrintBinary( FILE *file, unsigned int code, int bits );
int           getbufc(BIT_BUF *bit_buf);
int           putbufc(int c, BIT_BUF *bit_buf);

#endif  /* _BITIO_H */

/*************************** End of BITIO.H **************************/

