/*##################################################################*/
/*                                                                  */
/* Program name:  key.cpp                                           */
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

#include "assert.h"
#include "key.h"
#include "file.h"
#include "section.h"

//==========================================================================================

// static class member
const Key Key::PlaintextKey(PLAIN,0);


// aliased key
Key::Key (const Section& Sec, const File_vec& Ifiles)
:  Encr(Sec.encr), HI(false)
{
   for (int f = 0; f < Ifiles.size(); f++) {
      Keys.push_back (Ifiles[f]->KeyvalAt(Sec.First, Encr));
   }
}


// actual key from HI section
Key::Key (const HIsection& HI, const File_vec& Hfiles, const File_vec& Ifiles)
:  Encr(HI.encr()), chosenLoc(0), HI(true)  //NB: assumes is HI key iff this constructor used
{
   assert (Hfiles.size() == Ifiles.size());
//?? file vectors as HI section members?
   for (int f = 0; f < Hfiles.size(); f++) {
      const byte* pHostByte = Hfiles[f]->pPlaintext(HI.Host.First);
      const byte* pInfByte  = Ifiles[f]->pPlaintext(HI.Infected.First);
      const Keyval key = decrypt(Encr, pInfByte, pHostByte); //??? general rule? not for SUB?
      Keys.push_back(key);
   }
}

// actual key from aliased key
Key::Key (const Key& K, uLong Correction)
:  Encr(K.Encr), chosenLoc(0), HI(false)
{
   for (int k = 0; k < K.Keys.size(); k++) {
      Keys.push_back (decrypt(Encr, &K.Keys[k], Correction)); //??? OK for other encr types?
   }
}

//-------------------------------------------------------------------------------------------

bool Key::hasSameValuesAs (const Key& other) const {
   if (Encr != other.Encr) return false;
   if (HI != other.HI) return false;
   if (Keys.size() != other.Keys.size()) return false;
   for (int k = 0; k < Keys.size(); k++) {
      if (Keys[k] != other.Keys[k]) return false;
   }
   return true;
}


bool Key::isAliasOf (const Key& other) const {
   if (Encr != other.Encr) return false;
   if (size() != other.size()) return false;

   uLong delta = decrypt(Encr, &Keys[0], other.Keys[0]); //??? OK for other encr types? 
   for (int k = 1; k < size(); k++) {                    // ?? create a key diff function?
      if (decrypt(Encr, &Keys[k], other.Keys[k]) != delta) return false;
   }
   return true;
}

//-------------------------------------------------------------------------------------------

int Key::findKeyLocations (const Section_vec& Isections) {
   int n = 0;

   const int F = size();          // number of files in the class this group belongs to

   Loc.clear();

   // for each of the sections
   for (int s = 0; s < Isections.size(); s++) {
      assert (Isections[s]->Segments.size() == F);

      // for each offset within the section
      for (long o = 0; o < Isections[s]->minLength; o++) { //?? and the other way?

         // for each file in the class
         for (int f = 0; f < F; f++) {
            const File* pFile = Isections[s]->Segments[f]->pFile;
            // is the actual key at offset o within section s ?
            if (memcmp(pFile->pPlaintext(Isections[s]->First + o),&Keys[f],keyLength(Encr))) break;
         }
         if (f != F) continue;  // not so, for at least one file

         // actual key was found at relative offset o of section s in all files 
         Loc.push_back (Isections[s]->First + o);
         n++;
      }
   }
   chooseBestLocation();

   //if (n == 0) printf ("--+ key not found in infected file set\n");   //??
   //if (n > 1)  printf ("--+ key found more than once in infected file set\n");   //??

//?? dbg
//if(n) {
//printf("key %s\n", asString().c_str());
//for (int l = 0; l < n; l++) printf("%s %s",l ? "":"   locs:", Loc[l].asString().c_str());
//printf("\n");
//}

   return n;
}



string Key::asString (void) const {
   char bfr[8];
   string txt;

   txt += name(Encr);
   if (Loc.size()) {txt += " @ ";txt += location().asString();}
   else            {txt += "       ";}
   txt += ":\t";

   for (int v = 0; v < size(); v++) {
      for (int b = 0; b < keyLength(Encr); b++) {
         sprintf(bfr,"%c%02X", b ? '.' : ' ', ((byte*)&Keys[v])[b]);
         txt += bfr;
      }
   }
   return txt;
}


//-------------------------------------------------------------------------------------------

const Key* AliasedKeySet::add (Key K) {
   assert(K.Encr != PLAIN);
   for (int k = 0; k < vec.size(); k++) {
      if (vec[k]->isAliasOf(K)) return vec[k];
   }
//??printf ("new aliased key %s\n",K.asString().c_str()); //??
   return push (new Key(K));
}


const Key* ActualKeySet::add (Key K, const Section_vec& Isections) {
   assert(K.Encr != PLAIN);
   for (int k = 0; k < vec.size(); k++) {
      if (vec[k]->hasSameValuesAs(K))  return vec[k];
   }
   if (K.findKeyLocations(Isections) == 0) return NULL;
//??printf ("new actual key %s\n",K.asString().c_str()); //??
   return push(new Key(K));
}

//-------------------------------------------------------------------------------------------
/***??
const Key* AliasedKeySet::add (Key K) {
   const Key& item = *insert(K).first;
   return &item;
}


const Key* ActualKeySet::add (Key K, const Section_vec& Isections) {
   if (K.Encr != PLAIN && K.findKeyLocations(Isections) == 0) return NULL; //?? fix to allow plaintext pseudo-key - initialize with plaintext in set constr?
   const Key& item = *insert(K).first;
   return &item;
}
***/
