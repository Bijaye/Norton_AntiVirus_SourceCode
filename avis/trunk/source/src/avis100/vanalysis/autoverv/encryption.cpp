/*##################################################################*/
/*                                                                  */
/* Program name:  encryption.cpp                                    */
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

#include "encryption.h"



struct ENCR_BASE {
   virtual const char* name         (void) const = 0;
   virtual size_t      size         (void) const = 0;
   virtual size_t      keyLength    (void) const = 0;
   virtual uLong       mod          (void) const = 0;

   virtual uLong       encrypt      (const void* pSrc, Keyval Key) const = 0;
   virtual uLong       decrypt      (const void* pSrc, Keyval Key) const = 0;
   virtual uLong       decrypt      (const byte* pSrc, const byte* pKey) const = 0;
   virtual uLong       invariant    (const byte* pSrc) const = 0;
   virtual void        memInvariant (byte* pDest, const byte* pSrc, size_t count) const = 0;
};



template <class T> class ENCR_BASE_T: public ENCR_BASE {
   protected:
      virtual uLong encrOp (T item, T key) const = 0;
      virtual uLong decrOp (T item, T key) const = 0;
      virtual uLong inv2op (T item, T prev) const = 0;

   public:
      size_t size      (void) const {return sizeof(T);}
      size_t keyLength (void) const {return size();}
      uLong  mod       (void) const {return 1L << size()*8;}   //?? endian?

      uLong encrypt   (const void* pSrc, Keyval Kin) const {
         T Src, Key;
         memcpy(&Src, pSrc, size());
         memcpy(&Key, &Kin, size());

         return encrOp(Src,Key) % mod();
      }

      uLong decrypt   (const void* pSrc, Keyval Kin) const {
         T Src, Key;
         memcpy(&Src, pSrc, size());
         memcpy(&Key, &Kin, size());

         return decrOp(Src,Key) % mod();
      }

      uLong decrypt   (const byte* pSrc, const byte* pKey) const {
         T Src;
         Keyval Key;
         memcpy(&Src, pSrc, size());
         memcpy(&Key, pKey, size());

         return decrOp(Src,Key) % mod();
      }

      uLong invariant (const byte* pSrc) const {
         T Src, Prev;
         memcpy(&Src, pSrc, size());
         memcpy(&Prev, pSrc-size(), size());

         return inv2op(Src,Prev) % mod();
      }

      void memInvariant (byte* pDest, const byte* pSrc, size_t count) const {
         count = (count + size() - 1) / size(); 
         while (count--) {
            const uLong inv = invariant(pSrc);
            memcpy (pDest, &inv, size());
            pSrc  += size();
            pDest += size();
         }
      }

      virtual const char* name (void) const {return "*encryption type with undefined name!";}
};



template <class T> class ADD_ENCR_BASE: public ENCR_BASE_T<T> {
   protected:
      uLong encrOp (T item, T key)  const {return item + key;}
      uLong decrOp (T item, T key)  const {return item - key;}
      uLong inv2op (T item, T prev) const {return item - prev;}
};



template <class T> class XOR_ENCR_BASE: public ENCR_BASE_T<T> {
   protected:
      uLong encrOp (T item, T key)  const {return item ^ key;}
      uLong decrOp (T item, T key)  const {return item ^ key;}
      uLong inv2op (T item, T prev) const {return item ^ prev;}
};

// template instantiations ---------------------------------------------------------------

class PLAIN_ENCR: public ENCR_BASE_T<byte> {
   protected:
      uLong encrOp (byte item, byte key)  const {return item;}
      uLong decrOp (byte item, byte key)  const {return item;}
      uLong inv2op (byte item, byte prev) const {return item;}
   public:
      const char* name (void) const {return "PLAIN";}
      size_t keyLength (void) const {return 0;}
};



class ADD1_ENCR: public ADD_ENCR_BASE<byte> {
   public: const char* name (void) const {return "ADD1";}
};



class XOR1_ENCR: public XOR_ENCR_BASE<byte> {
   public: const char* name (void) const {return "XOR1";}
};



class ADD2_ENCR: public ADD_ENCR_BASE<word> {
   public: const char* name (void) const {return "ADD2";}
};



class XOR2_ENCR: public XOR_ENCR_BASE<word> {
   public: const char* name (void) const {return "XOR2";}
};



class NULL_ENCR: public ENCR_BASE_T<byte> {
   protected:
      uLong encrOp (byte item, byte key)  const {return 0;}
      uLong decrOp (byte item, byte key)  const {return 0;}
      uLong inv2op (byte item, byte prev) const {return 0;}
   public:
      const char* name (void) const {return "NULL";}
      size_t keyLength (void) const {return 0;}
};


//-------------------------------------------------------------------------------------


static const NULL_ENCR  encrNULL;
static const PLAIN_ENCR encrPLAIN;
static const ADD1_ENCR  encrADD1;
static const XOR1_ENCR  encrXOR1;
static const ADD2_ENCR  encrADD2;
static const XOR2_ENCR  encrXOR2;


static const ENCR_BASE* encryption_array[ENCR_COUNT+1] = {
   &encrNULL,
   &encrPLAIN,
   &encrADD1,
   &encrXOR1,
   &encrADD2,
   &encrXOR2
};

static const ENCR_BASE** encryption = encryption_array + 1;


//-------------------------------------------------------------------------------------


int size (ENCRYPTION e) {
   return encryption[e]->size();
}

int keyLength (ENCRYPTION e) {
   return encryption[e]->keyLength();
}

uLong mod (ENCRYPTION e) {
   return encryption[e]->mod();
}

const char* name (ENCRYPTION e) {
   return encryption[e]->name();
}


uLong encrypt (ENCRYPTION e, const void* pSrc, Keyval Key) {
   return encryption[e]->encrypt(pSrc, Key);
}

uLong decrypt (ENCRYPTION e, const void* pSrc, Keyval Key) {
   return encryption[e]->decrypt(pSrc, Key);
}

uLong decrypt (ENCRYPTION e, const byte* pSrc, const byte* pKey) {
   return encryption[e]->decrypt(pSrc, pKey);
}

uLong invariant (ENCRYPTION e, const byte* pSrc) {
   return encryption[e]->invariant(pSrc);
}

void memInvariant (ENCRYPTION e, byte* pDest, const byte* pSrc, size_t count) {
   encryption[e]->memInvariant(pDest, pSrc, count);
}


ENCRYPTION getSuperset (ENCRYPTION e1, ENCRYPTION e2) {

   if (e1 == e2) return e1;

   ENCRYPTION greater = (e1 > e2 ? e1 : e2);
   ENCRYPTION lesser  = (e1 > e2 ? e2 : e1);

   switch (lesser) {
      case PLAIN: return  greater;
      case XOR1:  return (greater == XOR2 ? greater : NULL_ENCRYPTION);
      case ADD1:  return (greater == ADD2 ? greater : NULL_ENCRYPTION);
   }

   return NULL_ENCRYPTION;
}

