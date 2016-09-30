/**

    ZZZZZ   IIIII   PPPP     222    X  X      H   H
       Z      I     P   P   2 2    X X       H   H
      Z       I     PPPP       2      X        HHHHH
     Z         I     P        2      X X       H   H
    ZZZZZ   IIIII   P       22222   X  X   *  H   H

    Description: Header file for ZIP 2.x compression library (public)

    Product: Central Point 'PC TOOLS'

    CONFIDENTIAL and PROPRIETARY
    (c) Copyright 1992,93,94 Quintessence Corporation
    (c) Copyright 1993,94 Central Point Software
    All rights reserved.

    Revision History:

    Version Date      Author  Comments
    ------- --------    ------   --------------------------------
    2.50 07-05-93    SJP   created this file
    2.75 06-06-94    SJP   split into public/private
               16/32 bit type portable

**/

#ifndef ZIP2X_H
#define ZIP2X_H

/*
* compression methods
*/

#define ZIP2X_DEFLATN 0
#define ZIP2X_DEFLATX 1
#define ZIP2X_DEFLATF 2
#define ZIP2X_DEFLATS 3

/*
* model dependencies
*/

#ifdef __cplusplus
    #define EXTERN extern "C"
#else
    #define EXTERN extern
#endif

#if _M_IX86
    #ifndef FAR
      #define FAR /**/
    #endif
    #ifndef PASCAL
      #define PASCAL /**/
    #endif
#else
    #define FAR far
    #define PASCAL pascal
#endif

/*
* function prototypes
*/

#if 0
EXTERN unsigned FAR PASCAL Zip2x_CompressWorkSize(void);
EXTERN int FAR PASCAL Zip2x_CompressInit(void FAR *, long, int);
EXTERN void FAR PASCAL Zip2x_CompressFree(void FAR *);

EXTERN int FAR PASCAL Zip2x_Compress(
    void FAR *, unsigned FAR *,
    void FAR *, unsigned FAR *,
    void FAR *,
    int);

EXTERN unsigned FAR PASCAL Zip2x_ExpandWorkSize(void);
EXTERN int FAR PASCAL Zip2x_ExpandInit(void FAR *);
EXTERN void FAR PASCAL Zip2x_ExpandFree(void FAR *);

EXTERN int FAR PASCAL Zip2x_Expand(
    void FAR *, unsigned FAR *,
    void FAR *, unsigned FAR *,
    void FAR *,
    int);
#endif

#endif

