/* ------------------------------------------------------------------- */
/*  COMPRESS.H / Distributed with release 2.51 of the PAK Toolkit      */
/*                                                                     */
/*       This file contains header information for use with C.         */
/*                                                                     */
/*       Please read the terms of the license in TOOLKIT.DOC for the   */
/*  limitations on commercial programs using the Toolkit.              */
/*                                                                     */
/*  Copywrite 1988-90 NoGate Consulting                                */
/* ------------------------------------------------------------------- */

#ifndef USE_ARCPAK
#define USE_ARCPAK 0
#endif

#if USE_ARCPAK

unsigned far pascal distillWorkSize(void);
void     far pascal initDistill(void far *variables);
int      far pascal preDistill(char far *inbuffer, unsigned short far *input_size,
                                char far *outbuffer, unsigned short far *output_size,
                                void far *variables, char end_of_input);
int      far pascal distill(char far *inbuffer, unsigned short far *input_size,
                            char far *outbuffer, unsigned short far *output_size,
                            void far *variables, char end_of_input);

unsigned far pascal diluteWorkSize(void);
void     far pascal initDilute(void far *variables);
int      far pascal dilute(char far *inbuffer, unsigned short far *input_size,
                           char far *outbuffer, unsigned short far *output_size,
                           void far *variables, char end_of_input);

#endif

void     far pascal initRLE(void);
int      far pascal RLE(char far *inbuffer, unsigned short far *input_size,
                        char far *outbuffer, unsigned short far *output_size,
                        char end_of_input);

unsigned far pascal LZWworkSize(void);
unsigned far pascal initLZW(void far *variables, __int16 bits, char use_rle);
int      far pascal LZW(char far *inbuffer, unsigned short far *input_size,
                        char far *outbuffer, unsigned short far *output_size,
                        void far *variables, char end_of_input);

unsigned far pascal shrinkSize(void);
void     far pascal initShrink(void far *variables);
int      far pascal shrink(char far *inbuffer, unsigned short far *input_size,
                           char far *outbuffer, unsigned short far *output_size,
                           void far *variables, char end_of_input);

unsigned far pascal CrushWorkSize(void);
void     far pascal initCrush(void far *variables);
int      far pascal Crush(char far *inbuffer, unsigned short far *input_size,
                          char far *outbuffer, unsigned short far *output_size,
                          void far *variables, char end_of_input);

unsigned far pascal implodeWorkSize(void);
void     far pascal initImplode(void far *variables, char zip_101);
int      far pascal preImplode(char far *inbuffer, unsigned short far *input_size,
                               char far *outbuffer, unsigned short far *output_size,
                               void far *variables, char end_of_input);
int      far pascal implode(char far *inbuffer, unsigned short far *input_size,
                            char far *outbuffer, unsigned short far *output_size,
                            void far *variables, char end_of_input);

void     far pascal initExpandRLE(void);
int      far pascal expandRLE(char far *inbuffer, unsigned short far *input_size,
                               char far *outbuffer, unsigned short far *output_size,
                               char end_of_input);

unsigned far pascal expandLZWsize(void);
void     far pascal initExpandLZW(void far *variables, __int16 bits,
                                    char use_rle);
int      far pascal expandLZW(char far *inbuffer, unsigned short far *input_size,
                               char far *outbuffer, unsigned short far *output_size,
                               void far *variables, char end_of_input);

unsigned far pascal unshrinkSize(void);
void     far pascal initUnshrink(void far *variables);
int      far pascal unshrink(char far *inbuffer, unsigned short far *input_size,
                             char far *outbuffer, unsigned short far *output_size,
                             void far *variables, char end_of_input);

unsigned far pascal expandCrushSize(void);
void     far pascal initExpandCrush(void far *variables);
int      far pascal expandCrush(char far *inbuffer, unsigned short far *input_size,
                                 char far *outbuffer, unsigned short far *output_size,
                                 void far *variables, char end_of_input);

unsigned far pascal explodeWorkSize(void);
void     far pascal initExplode(void far *variables, char chars_encoded,
                                 char eight_k_window);
int      far pascal explode(char far *inbuffer, unsigned short far *input_size,
                            char far *outbuffer, unsigned short far *output_size,
                            void far *variables, char end_of_input);


unsigned far pascal crc(char far *inbuffer, unsigned __int16 length,
                        unsigned __int16 crcval);
long     far pascal crc32(char far *inbuffer, unsigned __int16 length, long crcval);
void     far pascal xorBlock(char far *block, unsigned short size,
                              char far *password, __int16 far *position);
