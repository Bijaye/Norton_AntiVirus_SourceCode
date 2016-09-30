/************************** Start of MAIN.H **************************/

#ifndef _SIGCOMPR_H
#define _SIGCOMPR_H

void CompressFile( BIT_BUF *input, BIT_BUF *output, int argc, char *argv[] );
void ExpandFile( BIT_BUF *input, BIT_BUF *output, int argc, char *argv[] );

extern char *Usage;
extern char *CompressionName;

#endif  /* _SIGCOMPR_H */

/*************************** End of SIGCOMPR.H ***************************/

