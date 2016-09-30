/*##################################################################*/
/*                                                                  */
/* Program name:  autoverv.h                                        */
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

#ifndef AUTOVERV_H
#define AUTOVERV_H

#include <algorithm>
#include <vector>
#include <string>
#include <set>
using namespace std;

#include "polymath.h"

#pragma warning(disable: 4786) // name truncation in debug info.


// typedefs of built-in types ---------------------------------------------------------


typedef unsigned int   uInt;
typedef unsigned long  uLong;
typedef unsigned char  byte;
typedef unsigned short word;    //?? platform-dependent


// constants --------------------------------------------------------------------------


#define MAXINT   (0x7FFFFFFFL)
#define MAXUINT  (0xFFFFFFFFL)
#define MAXLONG   MAXINT
#define MAXULONG  MAXUINT

#define DIR_DELIMITER ('\\')
#define DRV_DELIMITER (':')
#define PAD    (16)           // the length of an area of zero bytes placed around
                              // file text and invariant buffers, to avoid end-effects 
                              // when encrypting, decrypting and invariant-transforming.

extern int MIN_REPLICANTS;

extern int MIN_HI;
#define HI_MAP ('H')

extern int MIN_I;
#define I_MAP  ('I')

#define MAX_PADDING MAXULONG  //??64 // The largest amount of variability that we can possibly 
#define P_MAP ('P')           // attribute to padding by the virus, e.g. for alignment

#define CODE_IN_THIS_FILE    1
#define CODE_IN_OTHER_FILE   2
#define IS_CODE(F) (F == CODE_IN_THIS_FILE || F == CODE_IN_OTHER_FILE)

extern polyn* CRC_TABLE;

// macros -----------------------------------------------------------------------------

#define max _cpp_max 
#define min _cpp_min

#define sign(N) (N < 0 ? '-' : ' ')
#define Sign(N) (N < 0 ? '-' : N > 0 ? '+' : ' ')

#define arraySize(A) (sizeof(A)/sizeof(*A))

char* strDup (const char* original);

void FailExit(void);
#define FailMsg(S) {printf("***Fail at %d in %s\n\t%s",__LINE__,__FILE__,S); FailExit();}
#define Fail FailMsg("")


#define dclVec(C) \
   struct C##_vec: public vector<C*> {}

#define dclVecIter(C) \
   typedef C##_vec::iterator C##_vec_iter; \
   typedef C##_vec::const_iterator const_##C##_vec_iter

#define dclVector(C) \
   dclVec(C); \
   dclVecIter(C);


#define dclSet(C) \
   struct C##_set: public vector<C*> {}; \
   typedef C##_set::iterator C##_set_iter; \
   typedef C##_set::const_iterator const_##C##_set_iter


// These define a consistent set of comparison operators, all based on <
//?? see STL p 57
//template <class T1, class T2>
//inline bool operator == (const T1& a, const T2& b) {return !(a < b || b < a);}

#define dclComparisons(T) \
   bool operator >  (const T& other) const { return other < *this;} \
   bool operator != (const T& other) const { return other < *this || other > *this;} \
   bool operator == (const T& other) const { return ! (other != *this);} \
   bool operator >= (const T& other) const { return ! (*this < other);} \
   bool operator <= (const T& other) const { return ! (*this > other);}

#define recycle(V) {for(int i = 0; i < V.size(); i++) delete V[i]; V.clear();}

// predicates -------------------------------------------------------------------------

bool isNull(void* p);

// class name declarations ------------------------------------------------------------

struct Section;
struct Section_vec;

struct HIsection;
struct HIsection_vec;

struct File;
struct File_vec;

//?? and the rest

// templates --------------------------------------------------------------------------

template <class T> bool pLT (const T* A, const T* B) {return *A < *B;}

//-------------------------------------------------------------------------------------

string str (const char* fmt,  long n);
string str (const char* fmt, uLong n);

#endif
