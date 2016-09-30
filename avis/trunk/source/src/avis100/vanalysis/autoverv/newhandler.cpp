/*##################################################################*/
/*                                                                  */
/* Program name:  newhandler.cpp                                    */
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

#include <stdio.h>
#include <process.h>
#include <new.h>
#include "assert.h"

#define K(k)   (k * 1024)
#define M(m)   K(K(m))

static _PNH pPrevNewHandler = NULL;


class ReserveMemoryForHandler {
   char* p;
public:
   ReserveMemoryForHandler(void) {p = new char [K(64)];}
  ~ReserveMemoryForHandler(void) {delete p;}
  void release (void) {delete p; p = NULL;}
};

static ReserveMemoryForHandler Reserve;

static int myNewHandler (size_t size) {
   Reserve.release();
   puts ("*** out of memory");
   exit(32);
   return 0;
}

void* setNewHandler (void) {
   _set_new_mode(1);   // malloc will also use the new handler.
   return pPrevNewHandler = _set_new_handler(myNewHandler);
}

//-------------------------------------------------------------------

#include <new.h>
#include <limits.h>

extern "C" void* _nh_malloc (size_t, int);
extern "C" void  free (void*);

static unsigned long LowestHeap  = ULONG_MAX;
static unsigned long HighestHeap = 0;
static unsigned long HeapLimit   = M(200);


unsigned long setHeapLimit (unsigned long NewLimit) {
   unsigned long OldLimit = HeapLimit;
   HeapLimit = M(NewLimit);
   return OldLimit;
}


void* operator new (unsigned int cb) {
   void* newMem = _nh_malloc (cb, 1);
   if (!HeapLimit || !newMem) return newMem;
   
   bool delta = false;

   if (unsigned long(newMem)        < LowestHeap)  {
      LowestHeap  = unsigned long(newMem);
      delta = true;
   }
   if (unsigned long(newMem)+(cb-1) > HighestHeap) {
      HighestHeap = unsigned long(newMem)+(cb-1);
      delta = true;
   }
   if (!delta) return newMem;

   //??printf("heap range %16Xh %16Xh   %16lu\n",LowestHeap, HighestHeap, HighestHeap-LowestHeap);//??
   if (HighestHeap-LowestHeap > HeapLimit) {
      printf ("*** Heap limit %luM exceeded\n", HeapLimit/M(1));
      exit (33);
   }
   return newMem;
}


void operator delete (void* p) { 
   free (p);
}


//--- for C-style allocation --------------------------------------------------


void mem_err(void) {
   puts ("allocation fail");
   fprintf(stderr,"***> allocation fail\n");
   assert(0);
   exit(34);
}
