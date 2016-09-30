/*##################################################################*/
/*                                                                  */
/* Program name:  key.h                                             */
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

#ifndef KEY_H
#define KEY_H

#include "encryption.h"
#include "location.h"
#include "file.h"

// ----------------------------------------------------------------------------

struct Key {
   const ENCRYPTION Encr;
   vector<Keyval>   Keys;
   vector<Location> Loc;
   int              chosenLoc;
   const bool       HI; //?? - used in ReconGroup::isWorseThan

   static const Key  PlaintextKey;
   
   Key (const Section& Sec, const File_vec& Ifiles);

   Key (ENCRYPTION E): Encr(E), HI(false) {}


   Key (const HIsection& HI, const File_vec& Hfiles, const File_vec& Ifiles);

   Key (const Key& K, uLong Correction);


   void chooseBestLocation (void) {chosenLoc = 0;} //??

   int findKeyLocations (const Section_vec& Isections);


   size_t size (void) const {return Keys.size();}

   bool isAliasOf (const Key& other) const;

   bool hasSameValuesAs (const Key& other) const;

   Location location (void) const {return Loc.size() ? Loc[chosenLoc] : Location::null;}

   size_t locationCount (void) const {return Loc.size();}

   bool isHIkey (void) const {return HI;}

   string asString (void) const;
};

dclVector(Key)

//===========================================================================================

// base class for all key sets, aliased or actual.
class KeySet {
   public:
      size_t size (void) const {return vec.size();}
      const Key* operator[] (size_t i) const {return vec[i];}
      void clear (void) {vec.clear();}

   protected:
      vector<const Key*> vec;
      const Key* push (const Key* Key) {vec.push_back(Key); return Key;}
};



// The STL 'set' requires you to define a total ordering of its elements (so it's a subset of sets!)
//?? make a template out of this?
class AliasedKeySet: public KeySet {
   public:
      const Key* add (Key Key);

      const Key* make (const Section& Sec, const File_vec& Files) {
         return add (Key(Sec, Files));
      }
};




class ActualKeySet: public KeySet {
   public:
      const Key* add (Key Key, const Section_vec& Isections);

      const Key* make (const HIsection& HI, const File_vec& Hfiles, const File_vec& Ifiles, const Section_vec& Isections) {
         return add (Key(HI, Hfiles, Ifiles), Isections);
      }

      const Key* make (const Key& K, uLong Correction, const Section_vec& Isections) {
         return add (Key(K,Correction),Isections);
      }
};


//??
//class PlaintextKeySet: public KeySet {
//   public: PlaintextKeySet(void) {push (&Key::PlaintextKey);}
//};

// ----------------------------------------------------------------------------
/***??
struct KeySet {
   virtual const Key& operator[] (const int i) const = 0; 
   virtual size_t size (void) const = 0;
};


struct AliasedKeyCompare {
   bool operator () (const Key& K1, const Key& K2) const {
      return !K1.isAliasOf(K2) && memcmp(&K1,&K2,sizeof(K1) < 0);
   }
};


struct AliasedKeySet: public KeySet, public set<Key,AliasedKeyCompare> {
   const Key* add (Key Key);

   const Key* make (const Section& Sec, const File_vec& Files) {
      return add (Key(Sec, Files));
   }

   const Key& operator[] (const int i) {return *this[i];} 
   size_t size (void) const {return this->size();}
};


struct ActualKeyCompare {
   bool operator () (const Key& K1, const Key& K2) const {
      return !K1.hasSameValuesAs(K2) && memcmp(&K1,&K2,sizeof(K1) < 0);
   }
};


struct ActualKeySet:  public KeySet, public set<Key,ActualKeyCompare>  {

   const Key* add (Key Key, const Section_vec& Isections);

   const Key* make (const HIsection& HI, const File_vec& Hfiles, const File_vec& Ifiles, const Section_vec& Isections) {
      return add (Key(HI, Hfiles, Ifiles), Isections);
   }

   const Key* make (const Key& K, byte C, const Section_vec& Isections) {
      return add (Key(K,C),Isections);
   }

   const Key& operator[] (const int i) const {return *this[i];} 
   size_t size (void) const const {return this->size();}
};
***/

#endif
