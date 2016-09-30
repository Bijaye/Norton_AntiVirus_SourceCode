/*##################################################################*/
/*                                                                  */
/* Program name:  encryption.h                                      */
/*                                                                  */
/* Package name:  autoverv.exe                                      */
/*                                                                  */
/* Description:                                                     */
/*                                                                  */
/*                                                                  */
/* Statement:     Licensed Materials - Property of IBM              */
/*                (c) Copyright IBM Corp. 1999                      */
/*                                                                  */
/* Author:        Andy Raybould                                     */
/*                                                                  */
/*                U.S. Government Users Restricted Rights - use,    */
/*                duplication or disclosure restricted by GSA ADP   */
/*                Schedule Contract with IBM Corp.                  */
/*                                                                  */
/*                                                                  */
/*##################################################################*/

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include "autoverv.h"


typedef uLong Keyval;

// encryption methods supported
typedef enum {
   NULL_ENCRYPTION = -1,
   PLAIN = 0,
   ADD1,
   XOR1,
   ADD2,
   XOR2,
   // add new values above here
   ENCR_COUNT
} ENCRYPTION;

//-------------------------------------------------------------------------------------

int         size     (ENCRYPTION e);
int         keyLength(ENCRYPTION e);
uLong       mod      (ENCRYPTION e);
const char* name     (ENCRYPTION e);

uLong encrypt    (ENCRYPTION e, const void* pSrc, Keyval Key);
uLong decrypt    (ENCRYPTION e, const void* pSrc, Keyval Key);
uLong decrypt    (ENCRYPTION e, const byte* pSrc, const byte* pKey);
uLong invariant  (ENCRYPTION e, const byte* pSrc);
void memInvariant(ENCRYPTION e, byte* pDest, const byte* pSrc, size_t count);

// Compatibility of encryption methods:
//   The set of all XOR1 encryptions is a subset of all XOR2 encryptions, 
//   comprised of those members of the latter whose key's bytes are all 
//   equal. Ditto for ADD1 and ADD2. Plaintext is a subset of them all
//   (those whose keys are zero).
//
//   We may have a set of samples where some host-infected matches are 
//   made with XOR1 invariant and others with the XOR2 invariant. We
//   can make a section to cover these if the XOR1 matches are 'promoted'
//   to XOR2, and similarly whenever one is a subset of the other. The
//   following function returns whichever of its arguments is a superset
//   (not necessarily a proper superset; it's OK if they are the same),
//   or NULL_ENCRYPTION if they are not comparable.

ENCRYPTION getSuperset (ENCRYPTION e1, ENCRYPTION e2);

/***??
//-------------------------------------------------------------------------------------

extern const char* EncrName[];


// pointer to function that encrypts a byte
typedef byte (*pfENCRYPTION) (const byte* pByte, const byte* pKey);

// array of such functions, one for each encryption type 
extern const pfENCRYPTION pfEncrypt []; 



// pointer to function that encrypts a string of bytes
typedef void (*pfMEM_ENCRYPTION) (byte* pDest, const byte* pSrc, const byte* pKey, int length);

// array of such functions, one for each encryption type 
extern const pfMEM_ENCRYPTION pfMemEncrypt [];

//-------------------------------------------------------------------------------------

// pointer to function that decrypts a byte
typedef byte (*pfDECRYPTION) (const byte* pByte, const byte* pKey);

// array of such functions, one for each encryption type 
extern const pfDECRYPTION pfDecrypt []; 



// pointer to function that decrypts a string of bytes
typedef void (*pfMEM_DECRYPTION) (byte* pDest, const byte* pSrc, const byte* pKey, int length);

// array of such functions, one for each encryption type 
extern const pfMEM_DECRYPTION pfMemDecrypt [];

//-------------------------------------------------------------------------------------

// pointer to function that calculates an invariant of a byte
typedef byte (*pfINVARIANT) (const byte* pByte);

// array of such functions, one for each encryption type 
extern const pfINVARIANT pfInvar []; 



// pointer to function that calculates an invariant of a string of bytes
typedef void (*pfMEM_INVARIANT) (byte* pDest, const byte* pSrc, int length);

// array of such functions, one for each encryption type 
extern const pfMEM_INVARIANT pfMemInvar [];


// construction macros ----------------------------------------------------------------

// creates a string transformation function from the equivalent single-byte function
#define MakeEncrMemFn1(BASE_FN) \
   static void Mem##BASE_FN (byte* pDest, const byte* pSrc, int length) { \
      for (int i = 0; i < length; i++) {*pDest++ = BASE_FN (pSrc++);}}

// ditto for functions taking a key
#define MakeEncrMemFn2(BASE_FN) \
   static void Mem##BASE_FN (byte* pDest, const byte* pSrc, const byte* pKey, int length) { \
      for (int i = 0; i < length; i++) {*pDest++ = BASE_FN (pSrc++, pKey);}}
***/

#endif
