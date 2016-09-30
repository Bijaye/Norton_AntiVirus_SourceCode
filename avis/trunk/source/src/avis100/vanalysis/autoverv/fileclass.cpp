/*##################################################################*/
/*                                                                  */
/* Program name:  FileClass.cpp                                     */
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

#include "fileclass.h"
#include "match.h"
#include "test.h"

//-----------------------------------------------------------------------------------------------

// For an assert in makeHIsections and makeHsections: 
// if a host section begins or ends in the MZ header, they must all do so, at the same offset
// NB: consider EXE hosts only, for viruses that convert all infected files to one type.
#if !defined(NDEBUG)

static bool partialMZheaderOverlap(const File_vec& Hfiles, const Section_vec& Hsections) {
   for (int s = 0; s < Hsections.size(); s++) {
      bool startsInMZ, endsInMZ;
      long start, end;

      // find first EXE host
      for (int f = 0; f < Hfiles.size() && Hfiles[f]->file_type != S_EXE; f++);
      if (f == Hfiles.size()) return false;   // none are EXE

      startsInMZ = Hfiles[f]->isInMZheader(Hsections[s]->First);
      if (startsInMZ) start = Hfiles[f]->offset(Hsections[s]->First);

      endsInMZ = Hfiles[f]->isInMZheader(Hsections[s]->Last);
      if (endsInMZ) end = Hfiles[f]->offset(Hsections[s]->Last);

      for (f++; f < Hfiles.size(); f++) {
         if (Hfiles[f]->file_type != S_EXE) continue;   // skip non-EXE 

         if (Hfiles[f]->isInMZheader(Hsections[s]->First)) {
            if (!startsInMZ || start != Hfiles[f]->offset(Hsections[s]->First)) 
               return true;
         }else {
            if (startsInMZ) 
               return true;
         }
         if (Hfiles[f]->isInMZheader(Hsections[s]->Last)) {
            if (!endsInMZ || end != Hfiles[f]->offset(Hsections[s]->Last)) 
               return true;
         }else {
            if (endsInMZ) 
               return true;
         }
      }
   }
   return false;
}

#endif

//-----------------------------------------------------------------------------------------------

FileClass::~FileClass (void) {
   deleteSections();
}

bool FileClass::add (Sample* pSample) {
   if (!accept(pSample)) return false;

   Samples.push_back(pSample);

   InfectedGoats.push_back (&pSample->Inf);
   ViralFiles.push_back (&pSample->Inf);
   HostFiles.push_back (&pSample->Host);
   MinVirusLength = min (MinVirusLength, pSample->VirusLength());
   MaxVirusLength = max (MaxVirusLength, pSample->VirusLength());
   return true;
}

void FileClass::addSample (File* pSample) {
   ViralFiles.push_back (pSample);
}

//-----------------------------------------------------------------------------------------------

void FileClass::makeHIsections (void) {
   Sample* pRefSample = Samples[0];

   for (int n = 0; n < pRefSample->HImatches.size(); n++) {
      HIsection* pSection = makeNthHIsections (n);
      if (pSection && pSection->isGoodSection(false,0)) {
         HIsections.push_back(pSection);
      }else {
         // split class?
         printf("--+ bad "); prtHIsect(pSection);   //--+ bad HI section:...
         delete pSection;
         //??Fail;
      }
   }
   sort (HIsections.begin(), HIsections.end(), HIbeforeInHost());
   for (int s = 0; s < HIsections.size(); s++) {
      HIIsections.push_back(&HIsections[s]->Infected);
      HIHsections.push_back(&HIsections[s]->Host);
   }
   //??assert (!partialMZheaderOverlap(HostFiles, HIsections));
}


HIsection* FileClass::makeNthHIsections (int n) {
   Segment_vec HostSegments, InfSegments;
   // 'reserve' is just an optimization here
   HostSegments.reserve(Samples.size());
   InfSegments.reserve(Samples.size());

   // get the host and infected segments (one of each per sample) of the class' nth HIsection
   for (Sample_vec_iter s = Samples.begin(); s != Samples.end(); s++) {
      if (n >= (*s)->HImatches.size()) return NULL;
      HostSegments.push_back((*s)->HImatches[n]->seg(HOST));
      InfSegments.push_back ((*s)->HImatches[n]->seg(INFECTED));
   }
   // convert the set of segments into a section, with sample-independent locations
   return new HIsection (HostSegments, InfSegments);
}

//-----------------------------------------------------------------------------------------------

void FileClass::makeIsections (void) {
   makeIconstSections();
   makeIvarSections();
   makeIsectionList();
}


void FileClass::makeIconstSections (void) {
   assert(IconstSections.size() == 0);

   // for each Iconst match, get the section best describing its position in each infected file
   for (Match_vec_iter m = IconstMatches.begin(); m != IconstMatches.end(); m++) {
      Section* pSection = new Section (**m);
      if (pSection->isGoodSection()) {
         IconstSections.push_back (pSection);
      }else {   // ??split class? is there a way to make some use of a nonzero-range Iconst section?
                // - will have to modify mergeSection, findintersection, makeIsectionList 
         printf("+++ bad Iconst section %s\n",pSection->asString().c_str());
         delete pSection;
      }
   }
}


   // find the matches between the class' infected sample's non-host segments
void FileClass::matchIconst (void) {
   assert(IconstMatches.size() == 0);

   match (ViralFiles, IconstMatches, MIN_I, I_MAP);

   // add the matches to this class' subclasses
   for (int c = 0; c < SubClasses.size(); c++) {
      SubClasses[c]->getParentsIconstMatches();
   }
}


//-----------------------------------------------------------------------------------------------

// Because with HI matches we have both plaintext and encrypted text, we can find the actual keys.
// If we know the actual keys, we can reject any possible reconstruction that doesn't work exactly.  

void FileClass::findHIkeys (void) {
   for (int s = 0; s < HIsections.size(); s++) {
      if (HIsections[s]->encr() != PLAIN) {

         HIsections[s]->pActualKey = ActualKeys.make(*HIsections[s],HostFiles,InfectedGoats,Isections);
         if (!HIsections[s]->pActualKey) {FailMsg("cannot find HI section key");} //?? more?

         // an actual key could also be used as an aliased key in repairing H sections
         // NB: adding this here will prevent equivalent Iconst aliased keys being added
         AliasedKeys.add (*HIsections[s]->pActualKey);
      } else {
         HIsections[s]->pActualKey = &Key::PlaintextKey;
      }
   }
}



void FileClass::findIconstKeys (void) {
   for (int s = 0; s < IconstSections.size(); s++) {
      if (IconstSections[s]->encr != PLAIN) {
         AliasedKeys.make (*IconstSections[s], InfectedGoats);
      }
   }
}

//-----------------------------------------------------------------------------------------------

// find reconstructions for all H section locations
void FileClass::findPossibleHostRecons (void) {
   assert (PossibleRecons.size() == 0);

   //?? variable length H and I(var) sections could match if they vary in sync - ie same start anchors and same end anchors

   // within each host section...
   for (int h = 0; h < Hsections.size(); h++) {
      const Section* pHsec = Hsections[h];

      // test every byte...
      //?? create a byteIterator class for a File vector 
      for (long ho = 0; ho < pHsec->minLength; ho++) {   // offset into host section
         int nH = 0;                                     // number of recons. found for this host byte
         // against every I section...
         for (int v = 0; v < Isections.size(); v++) {   //?? was IvarSections - don't need them any more?
            const Section* pIsec = Isections[v];

            // byte by byte, looking for a possible reconstruction...
            for (long vo = 0; vo < pIsec->minLength; vo++) {   // offset into Ivar section
               nH += findPossibleLocnRecons (pHsec->First+ho, pIsec->First+vo);
            }
         }
         if (nH == 0) printf("*** unable to reconstruct host byte %d%s.%d from any key\n",(pHsec->First+ho).Offset,AnchorName[pHsec->First.Anchor],pHsec->First.Range); 
      }
   }
}



// finds possible reconstructions of a H location from a given Ivar location.
int FileClass::findPossibleLocnRecons (Location Hloc, Location Iloc) {
   int n = 0;

   // is it a plaintext match?
   for (int f = 0; f<HostFiles.size(); f++) {
      //??RECON FROM CODE if (InfectedGoats[f]->isCode(Iloc)) break;
      if (*HostFiles[f]->pPlaintext(Hloc) != *InfectedGoats[f]->pPlaintext(Iloc)) break;
   }
   if (f == HostFiles.size()) {
      makePossibleByteRecon (&Key::PlaintextKey,Hloc,Iloc); //!!??,pIvar??);
      return 1;
   }

   // does it match using an actual key?
   if (ActualKeys.size()) {
      n+= findPossibleEncrLocnRecons (Hloc, Iloc, ActualKeys);
      //??if (!n) puts("--+ unable to reconstruct a host byte from any HI key");
   }
   //?? if (n) return 0?

   // does it match using an aliased key?   ?? does each HI section also have an aliased Key?
   if (AliasedKeys.size()) {
      n+= findPossibleEncrLocnRecons (Hloc, Iloc, AliasedKeys);
      //??if (!n) puts("--+ unable to reconstruct a host byte from any Iconst key");
   }
   return n;
}



// finds possible reconstructions of a H location from a given Ivar location, using decryption.
int FileClass::findPossibleEncrLocnRecons (Location Hloc, Location Iloc, const KeySet& Keys) {
   assert (HostFiles.size() == InfectedGoats.size());
   int n = 0;

   for (int k = 0; k < Keys.size(); k++) {
      const Key* pKey = Keys[k];

      uLong Correction;
      for (int f = 0; f < HostFiles.size(); f++) {
         //??RECON FROM CODE if (InfectedGoats[f]->isCode(Iloc)) break;
         const byte* pHbyte = HostFiles[f]->pPlaintext(Hloc);
         const byte* pIbyte = InfectedGoats[f]->pPlaintext(Iloc);
         const uLong diff = decrypt(pKey->Encr,pHbyte,decrypt(pKey->Encr,pIbyte,pKey->Keys[f])); //??? generally valid?

         if (f == 0) Correction = diff;
         else
         if (diff != Correction) break;

         if(Correction && pKey->locationCount() > 0) break; // actual keys require exact decryption 
      }
      if (f < HostFiles.size()) continue;

      // found a match - find an actual key for it
      pKey = ActualKeys.make(*pKey, Correction, Isections); //?? check that this prevents duplicates

      if (pKey) n += makePossibleByteRecon(pKey,Hloc,Iloc);
   }
   return n;
}



// if this reconstruction is OK, add it to the list
int FileClass::makePossibleByteRecon (const Key* pKey, Location Hloc, Location Iloc) {
   long Size = size(pKey->Encr);   // the number of bytes this reconstructs

   // if a reconstruction is from an Iconst section, the reconstruction key must be an alias
   // of the aliased key associated with that section.

//?? no - a byte on an Iconst boundary may be accidentally be included, e.g. if it is a constant
//        byte in a varying field (maybe check the initial byte of the recon, as accidental 
//        inclusion is most likely to happen with trailing bytes: 1) more likely to be all 0;
//        2) if 0, may be seen as the 1st byte of a word-key encrypted region.
 
//   for (long o = 0; o < Size; o++) { 
//      for (File_vec_iter f = InfectedGoats.begin(); f < InfectedGoats.end(); f++) {
//         const Section* const pIsect = (*f)->pSectionContainingLocation(Iloc+o,IconstSections);
//         if (!pIsect || (pKey->Encr == PLAIN && pIsect->encr == PLAIN)) continue;
//         if (!pIsect->aliasedKey(InfectedGoats).isAliasOf(*pKey)) {
//            printf("++ recon rejected due to Iconst key mismatch: %s %s %s\n", Hloc.asString().c_str(), Iloc.asString().c_str(), pKey ? pKey->asString().c_str():"(none)");//??
//            return 0;
//         }
//      }
//   }
//?? this is the start-only version
   for (File_vec_iter f = InfectedGoats.begin(); f < InfectedGoats.end(); f++) {
      const Section* const pIsect = (*f)->pSectionContainingLocation(Iloc,IconstSections);
      if (!pIsect || (pKey->Encr == PLAIN && pIsect->encr == PLAIN)) continue;
      if (!pIsect->aliasedKey(InfectedGoats).isAliasOf(*pKey)) {
         //??printf("++ recon has been rejected due to Iconst key mismatch: %s %s %s\n", Hloc.asString().c_str(), Iloc.asString().c_str(), pKey ? pKey->asString().c_str():"(none)");//??
         return 0;
      }
   }

   PossibleRecons.push_back(new Reconstruction(pKey,Hloc,Iloc,Hloc+Size-1L,Iloc+Size-1L));//??pIvar
   return 1;
}

//-----------------------------------------------------------------------------------------------

void FileClass::makePossibleFieldRecons (void) {
   Location lf;   // location of start of current field

   //?? variable length H and I(var) sections could match if they vary in sync - see above

   sort (PossibleRecons.begin(), PossibleRecons.end(), Reconstruction::pHostLocLT);

   for (int h = 0; h < Hsections.size(); h++) {
      const Section* pHsec = Hsections[h];
      for (long ho = 0; ho <= pHsec->minLength; ho++) {   // offset into host section
         const Location l = pHsec->First+ho;

         // are we at a field boundary?
         if (ho == pHsec->minLength || isFieldStartInAllFiles(l)) {   
            if (ho > 0) findFullFieldRecons (lf, l); 
            lf = l;
         }else assert (ho > 0); //??FailMsg("Host section must begin on field boundary");   //?? may be able to adjust adjacent boundary - e.g. coincidental HI match (but HI sections should be properly adjusted?)
      }
   }
}



bool FileClass::isFieldStartInAllFiles(Location Loc) {
   for (int f = 0; f < HostFiles.size(); f++) if (!HostFiles[f]->isFieldStart(Loc)) return false;
   return true;
}



// assumes the possible recons are sorted by host location, ascending
void FileClass::findFullFieldRecons (Location Hbegin, Location Hend) {
   if (Hbegin+1L == Hend) return;   // single-byte field
   assert (Hbegin != Hend);

   // the boundaries, in PossibleRecons, of this field's reconstructions
   Reconstruction_vec_iter Rbegin, Rend;
   // set them   ?? assumes all relevant recons start at the field boundary
   // ?? use find_if
   for (Rbegin = PossibleRecons.begin(); Rbegin != PossibleRecons.end() && (*Rbegin)->HostLoc < Hbegin; Rbegin++);
   for (Rend = Rbegin; Rend != PossibleRecons.end() && (*Rend)->HostLoc < Hend; Rend++);

   // for each reconstruction of the first location in the field...
   Reconstruction_vec_iter R, r;
   for (R = Rbegin; R < Rend && (*R)->HostLoc == Hbegin; R++) {
      if (!*R) continue;

      // ...check there's a corresponding reconstruction for every other location in the field
      // NB: Two reconstructions correspond if they use the same key, and have the same source 
      // anchor & relative offset (these last two conditions mean the sources are in the same
      // order and the same distance apart as the bytes they reconstruct)
      for (Location l = Hbegin; l < Hend; l = (*r)->HostLast + 1L) {
         for (r = Rbegin; r < Rend; r++) {
            if (!*r) continue;

            if ((*r)->HostLoc == l && (*r)->correspondsTo(*R)) break;   // r corresponds to R at o
         }
         if (r == Rend) break;  // didn't find an r corresponding to R at o 
      }
      if (l == Hend) {   // found a reconstruction corresponding to R at all o...
         // ...so convert it into a reconstruction ranging over the field
         Reconstruction* pR = *R;
         *R = new Reconstruction (*pR, Hend.Offset-Hbegin.Offset);
         delete pR;
      }else {   // get rid of it
         delete *R;
         *R = NULL;
      }
   }

   // now delete the reconstructions for the other bytes of the field
   // - they have either been incorporated into one of the first location's reconstructions
   //   or they don't correspond to any of them
   for (; R < Rend; R++) {
      delete *R;
      *R = NULL;
   }

   // now remove the deleted recons  ?? erase in place OK?
   PossibleRecons.erase(remove_if(PossibleRecons.begin(),PossibleRecons.end(),isNull),PossibleRecons.end());
}

//-----------------------------------------------------------------------------------------------

void FileClass::chooseReconstructions (void) {
   ChosenReconList.clear();   //?? mem leakage?

   // make it easy to get all the possible recons. for a particular host location
   //??sort (PossibleRecons.begin(), PossibleRecons.end(), Reconstruction::hostLocLT);

   // repeatedly remove the best-ranked group and all others using the same key
   while (PossibleRecons.size()) { //?? guaranteed to go to zero?

      // rank the possible recons
      PossibleReconRank.ReInit();
      for (int p = 0; p < PossibleRecons.size(); p++) {
         PossibleReconRank.add (*PossibleRecons[p]);
      }
      const ReconGroup BestReconGroup = PossibleReconRank.getBest();

      // first find all the recons in the chosen group...
      for (p = 0; p < PossibleRecons.size(); p++) {
         if (!PossibleRecons[p]) continue;

         if (BestReconGroup.isSameGroupAs(*PossibleRecons[p])) {
            ChosenReconList.push_back(new Reconstruction(*PossibleRecons[p]));
            invalidateConflictingRecons(*PossibleRecons[p]); // flag any with same host or source (or src same as key loc?)
         }
      }

      // ...then any others using the same key
      if (BestReconGroup.pKey->Encr != PLAIN) {
         for (p = 0; p < PossibleRecons.size(); p++) {
            if (!PossibleRecons[p]) continue;

            if (PossibleRecons[p]->pKey->hasSameValuesAs(*BestReconGroup.pKey)) {
               ChosenReconList.push_back(new Reconstruction(*PossibleRecons[p]));
               invalidateConflictingRecons(*PossibleRecons[p]); // flag any with same host or source (or src same as key loc?)
            }
         }
      }

      // now remove the selected recons
      PossibleRecons.erase(remove_if(PossibleRecons.begin(),PossibleRecons.end(),isNull),PossibleRecons.end());
   }
}


// Removes from further consideration any possible reconstruction that would conflict
// with the one just chosen:
// Need to copy the argument (i.e. pass it by value) as the original will be deleted
// during this process.
// Items are set null rather than removed from the vector so that iterators remain valid.
// They are removed after the iteration has completed. 
void FileClass::invalidateConflictingRecons (Reconstruction P) {
   assert (P.HostLoc.Anchor == P.HostLast.Anchor && P.SrcLoc.Anchor == P.SrcLast.Anchor);
   
   for (long o = 0; o < P.Length; o++) {
      for (Reconstruction_vec_iter p = PossibleRecons.begin(); p != PossibleRecons.end(); p++) {
         if (!*p) continue;

         // deletes the following possible reconsructions as being in conflict with the chosen one:
         if ((*p)->HostLoc+o == P.HostLoc+o        // reconstructs the same host byte 
         ||  (*p)->SrcLoc+o  == P.SrcLoc+o         // uses the same source byte in the infected file
         ||  (*p)->SrcLoc+o  == P.pKey->location() // uses the chosen reconsrtuction's key as the source//??
         ){
            delete *p;
            *p = NULL;
         }
      }
   }
}


//-----------------------------------------------------------------------------------------------

void FileClass::addHIreconstructions(void) {
   for (int h = 0; h < HIsections.size(); h++) {
      ChosenReconList.push_back(new Reconstruction(*HIsections[h]));
   }
}



// If a reconstruction is a continuation of its predecessor, combine the two into one.
// If reconstructions overlap, shorten the first of the pair until they do not.
// NB afterwards, the list is in order of host location, ascending.
void FileClass::consolidateReconstructions (void) {
   if (ChosenReconList.size() <= 0) return;

   sort(ChosenReconList.begin(), ChosenReconList.end(), Reconstruction::pHostLocLT);

   Reconstruction_vec_iter after, before;

   before = after = ChosenReconList.begin();
   while (++after != ChosenReconList.end()) {
      if ((*after)->isContinuationOf(**before)) {
         (*before)->append(**after);
         delete *after;
         ChosenReconList.erase(after);
         after = before;
      }else {
         before = after;
      }
   }

   before = ChosenReconList.begin();
   while (*before != ChosenReconList.back()) {
      after = before + 1;

      if ((*before)->HostLast.Anchor == (*after)->HostLoc.Anchor) {
         const long overlap = (*before)->HostLast.Offset - (*after)->HostLoc.Offset + 1;

         if (overlap > 0) {                          // shorten 'before'
            (*before)->HostLast.Offset -= overlap;
            (*before)->SrcLast.Offset  -= overlap;

            if ((*before)->HostLoc.Anchor == (*before)->HostLast.Anchor) { // fixed-length
               assert ((*before)->SrcLoc.Anchor == (*before)->SrcLast.Anchor);
               (*before)->Length -= overlap;

               if ((*before)->Length <= 0) {          // 'before' disappears 
                  delete *before;
                  ChosenReconList.erase(before);
                  before = ChosenReconList.begin();   // start over - may overlap 'before's' predecessor 
                  continue;
               }
            }
         }
      }
      before++;
   }
}


bool FileClass::reconsAreCompleteAndConsistent (void) const {
   bool OK = true, inexact = false, consistent = true;

   if (ChosenReconList.size() <= 0) {
      printf("WARN: fatal reconstruction error: no reconstruction\n");
      return false;
   }

   // Do the reconstructions follow one after the other with no gaps?
   Location PrevEnd = Location(START,-1,0);
   for (int r = 0; r < ChosenReconList.size(); r++) {
      const Location Loc = ChosenReconList[r]->HostLoc;
      const Location End = ChosenReconList[r]->HostLast;

      if (Loc.Range != 0 || End.Range != 0) {
         printf ("WARN: fatal reconstruction error: recon %d has non-zero range\n",r);
         OK = false;
      }

      // if it fails either of the following, test based on the offsets in the actual sample files?  (??)
      // no - who is to say that it will always be OK?
      if (Loc.precedes(PrevEnd)) {
         printf("WARN: reconstruction inconsistency (?): %d overlaps its predecessor\n", r);
         consistent = false;
      }

      if (!Loc.immediatelyFollows(PrevEnd)) {
         if (isAllOfType(S_EXE) && Loc.Anchor == START) {
            if (Loc.Offset <= 6) {
               printf ("INFO: reconstruction %d missing (part of) 1st 6 bytes\n",r);
               inexact = true;
            }else if (PrevEnd.Offset >= 11 && Loc.Offset <= 16) {
               if (PrevEnd.Offset < 13) printf ("INFO: reconstruction missing Min @ %d\n",r);
               if (Loc.Offset     > 14) printf ("INFO: reconstruction missing Max @ %d\n",r);
               inexact = true;
            }else if (PrevEnd.Offset >= 17 && Loc.Offset <= 20) {
               printf("INFO: reconstruction missing Checksum @ %d\n",r);
               inexact = true;
            }else if (PrevEnd.Offset >= 25 && Loc.Offset <= 28) {
               printf("INFO: reconstruction missing Overlay # @ %d\n",r);
               inexact = true;
            }else {
               printf("WARN: fatal reconstruction error: gap before %d\n",r);
               OK = false;
            }
         }else {
            printf("WARN: fatal reconstruction error: gap before %d\n",r);
            OK = false;
         }
      }
      PrevEnd = End;
   }

   if (ChosenReconList.back()->HostLast != Location(END,0,0)) {
      printf("WARN: fatal reconstruction error: last item is not EOF\n");
      OK = false;
   }

   return OK && consistent;
}



bool FileClass::isAllOfType(FILE_TYPE Type) const {
   for (int s = 0; s < Samples.size(); s++) {
      if (Samples[s]->Host.file_type != Type) return false;
   }
   return true;
}



// true if there's no reconstruction for one of the first 6 bytes (EXE files only).
// NB: true doesn't mean the reconstruction will work after caculating the header length.
bool FileClass::needsMZlengthCalc (void) const {
   long LengthSeen = 0;

   if (ChosenReconList.size() <= 0) return false;   // don't need length if can't reconstruct!
   if (!isAllOfType(S_EXE)) return false;

   // Is the beginning of the file reconstructed?
   if (ChosenReconList[0]->HostLoc != Location(START,0,0)) return true;

   // Do the remaining header reconstructions follow one after the other with no gaps?
   for (int r = 1; r < ChosenReconList.size(); r++) {
      LengthSeen += ChosenReconList[r-1]->Length;
      if (LengthSeen >= 6) return false;
      if (!ChosenReconList[r]->immediatelyFollows(*ChosenReconList[r-1])) return true;
   }
   return ChosenReconList.back()->HostLast != Location(END,0,0);
}


// EACH sample in the class must EITHER match at least minMatchPercent of its host,
//    OR leave no more than maxUnmatchedLength bytes of its host unmatched.
bool FileClass::isWellMatchedToHost (int minMatchPercent, unsigned long maxUnmatchedLength) const {
   for (int s = 0; s < Samples.size(); s++) {
      if (Samples[s]->percentMatched()  < minMatchPercent
      &&  Samples[s]->lengthUnMatched() > maxUnmatchedLength) {
         return false;
      }
   }
   return true;
}

//-----------------------------------------------------------------------------------------------

// Make a list of the class' host sections, such that the nth section covers each of the
// class' host files' nth host segment. 
// A host segment is a contiguous sequence of host-file bytes not matched to its infected file.
// A host section is a single sample-independent location describing a set of segments, each 
// from a different file.
void FileClass::makeHsections (void) {
   assert (Hsections.size() == 0);
   findInterSections(HIHsections, Hsections);
   assert (!partialMZheaderOverlap(HostFiles, Hsections));
}



void FileClass::makeIvarSections (void) {
   assert (IvarSections.size() == 0);
   Section_vec IfileMatches;
   mergeSections (IconstSections, HIIsections, IfileMatches);
   findInterSections(IfileMatches, IvarSections);
   for (int s = 0; s < IfileMatches.size(); s++) delete IfileMatches[s]; 
}


//!!!SECTION_VEC UNION (Iconst,HIH) - a bit like findIntersections except the first and last lists are different?
void FileClass::makeIsectionList (void) {
   mergeSections(IconstSections, IvarSections, Isections);
}


//-----------------------------------------------------------------------------------------------

// section_vec method?
int FileClass::findInterSections (const Section_vec& Src, Section_vec& Gaps) {
   Section* pSection;
   int n = 0;

   if (Src.size() <= 0) return 0;

   if (Src[0]->First != Location(START,0,0)) {
      pSection = new Section (NULL,Src[0]);
      if (pSection->minLength > 0) {Gaps.push_back (pSection); n++;} else delete pSection;
   }
   for (int s = 0; s < Src.size()-1; s++) {
      pSection = new Section(Src[s],Src[s+1]);
      if (pSection->minLength > 0) {Gaps.push_back (pSection); n++;} else delete pSection;
   }
   if (Src[s]->precedesInAllFiles(Location(END,0,0))) {
      pSection = new Section(Src[s],NULL);
      if (pSection->minLength > 0) {Gaps.push_back (pSection); n++;} else delete pSection;
   }
   return n;
}



void FileClass::mergeSections (const Section_vec& Src1, const Section_vec& Src2, Section_vec& Dest) {
   //NB: doesn't consider the possibility of overlap as that won't happen as used in this program
   Section_vec CombinedSections;
   int s;

   for (s = 0; s < Src1.size(); s++) CombinedSections.push_back(new Section(*Src1[s]));
   for (s = 0; s < Src2.size(); s++) CombinedSections.push_back(new Section(*Src2[s]));
   sort (CombinedSections.begin(), CombinedSections.end(), beforeInFiles());
   for (s = 0; s < CombinedSections.size(); s++) addSectionToList(CombinedSections[s],Dest);
}



// if the new section immediately follows the last in the list and has the same encryption,
// append it to the last-in-list; otherwise, add it to the end of the list
//?? Section_vec method?
void FileClass::addSectionToList (Section* pNewSec, Section_vec& Dest) {
   if (Dest.size() > 0 && Dest.back()->appendSection(*pNewSec))
      delete pNewSec;
   else
      Dest.push_back(pNewSec);
}


//-----------------------------------------------------------------------------------------------

TypeBasedFileSubClass* FileClass::makeTypeBasedFileSubClass(FILE_TYPE F) {
   TypeBasedFileSubClass* pSubClass = new TypeBasedFileSubClass (F);
   if (!pSubClass) return NULL;
   subClass(*pSubClass);
   return pSubClass;
}

LengthBasedFileSubClass* FileClass::makeLengthBasedFileSubClass(FILE_TYPE F, FILELENGTH_TYPE L) {
   if (Threshold < 0) getCriticalLength();
   LengthBasedFileSubClass* pSubClass = new LengthBasedFileSubClass (F,L);
   if (!pSubClass) return NULL;
   subClass(*pSubClass);
   return pSubClass;
}


void FileClass::subClass (FileClass& SubClass) {
   SubClass.pParent = this;
   SubClass.MinVirusLength = MinVirusLength;
   SubClass.MaxVirusLength = MaxVirusLength;
   SubClass.Threshold      = Threshold;
   for (int s = 0; s < Samples.size(); s++) {
      bool wasAdded = SubClass.add(Samples[s]);   // won't be accepted if of the wrong type
      if (wasAdded) printf("   %s\n",Samples[s]->Host.filename);   //??
   }
   SubClass.getParentsIconstMatches(); //?? don't expect anything to be copied
   SubClasses.push_back(&SubClass);
}


// The length that divides short from long files, if it matters (0 if not)
long FileClass::getCriticalLength (void) {
   if (Threshold >= 0) return Threshold;   // memoize

   if (MaxVirusLength - MinVirusLength > 32) {
      printf ("WARN: - large variability in viral length: %d to %d\n", MinVirusLength, MaxVirusLength);
      return (Threshold = 0);
   }
   // just try 1 possibility: critical length is when host length == virus length
   long lCrit = MinVirusLength * 2;
   int  COMshortHIcount = 0, COMlongHIcount = 0, EXEshortHIcount = 0, EXElongHIcount = 0;
   int  COMshortCount = 0, COMlongCount = 0, EXEshortCount = 0, EXElongCount = 0;

   for (int s = 0; s < Samples.size(); s++) {
      const File& Inf = Samples[s]->Inf;
      const int HIcount = Samples[s]->HImatches.size();
      if (Inf.file_type == S_COM) {
         if (Inf.length <= lCrit) {
            if (COMshortCount == 0)              COMshortHIcount = HIcount;
            else if (COMshortHIcount != HIcount) COMshortHIcount = -1;
            COMshortCount++;
         }else {
            if (COMlongCount == 0)               COMlongHIcount = HIcount;
            else if (COMlongHIcount != HIcount)  COMlongHIcount = -1;
            COMlongCount++;
         }
      }else {
         if (Inf.length <= lCrit) {
            if (EXEshortCount == 0)              EXEshortHIcount = HIcount;
            else if (EXEshortHIcount != HIcount) EXEshortHIcount = -1;
            EXEshortCount++;
         }else {
            if (EXElongCount == 0)               EXElongHIcount = HIcount;
            else if (EXElongHIcount != HIcount)  EXElongHIcount = -1;
            EXElongCount++;
         }
      }
   }
   printf ("lCrit %ld: \tCs %d %d   \tCl %d %d   \tXs %d %d   \tXl %d %d\n", lCrit, COMshortCount, COMshortHIcount, COMlongCount, COMlongHIcount, EXEshortCount, EXEshortHIcount, EXElongCount, EXElongHIcount); //??
   if (COMshortHIcount < 0 || COMlongHIcount < 0 || EXEshortHIcount < 0 || EXElongHIcount < 0) {
      printf ("WARN: unable to find consistent HI pairing: Cs %d Cl %d Xs %d Xl %d\n", COMshortHIcount, COMlongHIcount, EXEshortHIcount, EXElongHIcount);
      return (Threshold = 0);
   }

   if ((COMshortCount && COMlongCount && COMshortHIcount != COMlongHIcount) 
   ||  (EXEshortCount && EXElongCount && EXEshortHIcount != EXElongHIcount)) {
      return (Threshold = lCrit);
   }
   return (Threshold = 0);
}



void FileClass::emitCriticalLength(FILE* hFile) const {
   assert (Threshold >= 0);
   if (Threshold > 0) fprintf (hFile, "CriticalLength %ld\n", Threshold);
}



void FileClass::getParentsIconstMatches (void) {
   assert (pParent);
   assert (!IconstMatches.size()); //?? assume only done once - or look for duplicates before adding to list?

   if (Samples.size() > 0) {
      for (int m = 0; m < pParent->IconstMatches.size(); m++) {
         IconstMatches.push_back(new Match(*pParent->IconstMatches[m], InfectedGoats));
      }
   }
}


// create a verification entry for each Iconst match 
void FileClass::makeVerifications(void) {
   for (int s = 0; s < IconstSections.size(); s++) {
      // find the code sections within the Iconst section

      // first, point to the section's start in all the files' code-data maps
      const Segment* pRefSeg = IconstSections[s]->Segments[0];
      const char*    pCDset  = pRefSeg->pFile->CodeMap + pRefSeg->first;

      // now walk the map to the end of the section, looking for changes in type
      char currentCodeStatus = pCDset[0];
      long first = 0;
      for (long o = 0; o < pRefSeg->length(); o++) {

         if (IS_CODE(pCDset[o]) != IS_CODE(currentCodeStatus)) {
            Verifications.push_back(new VerSec(IconstSections[s],first,o-1,IS_CODE(currentCodeStatus),classname()));
            first = o;
            currentCodeStatus = pCDset[o];
         }
      }
      Verifications.push_back(new VerSec(IconstSections[s],first,o-1,IS_CODE(currentCodeStatus),classname()));
   }
   sort(Verifications.begin(),Verifications.end(),VerSec::order);
}

/*?? the complicated version that tries to unify different subclasses
void FileClass::makeVerifications(void) {
   for (int m = 0; m < IconstMatches.size(); m++) {
      // If it's possible to find a globally-good section to cover this match, then use
      // that as the basis for verification; otherwise, the section will be verified at
      // different locations depending on the type of the file.
      Section GlobalSection(*IconstMatches[m]);

      // get, for each subclass, a section corresponding to this match.
      Section_vec CommonSections;
      if (GlobalSection.isGoodSection()) { 
         if (!getMatchingSections(&GlobalSection, CommonSections)) continue;
      }else {
         if (!getMatchingSections(IconstMatches[m], CommonSections)) continue;
      }

      // now find the code sections within the Iconst section

      // first, point to the section's start in all the files' code-data maps
      const Segment* pRefSeg = NULL;
      for (int c = 0; c < CommonSections.size() && !pRefSeg; c++) {
         if (CommonSections[c]) pRefSeg = CommonSections[c]->Segments[0];
      }
      const char* pCDset = pRefSeg->pFile->CodeMap + pRefSeg->first;
      if (!sectionsAgreeOnCodeDataMapping(IconstMatches[m],CommonSections)) {
         printf("WARN: cannot use Iconst match %d for verification\n",m);
         continue;
      }

      // now walk the map to the end of the section, looking for changes in type
      char currentCodeStatus = pCDset[0];
      long first = 0;
      for (long o = 0; o < IconstMatches[m]->length; o++) {

         if (IS_CODE(pCDset[o]) != IS_CODE(currentCodeStatus)) {
            Verifications.push_back(new VerSec(CommonSections,first,o-1,IS_CODE(currentCodeStatus)));
            first = o;
            currentCodeStatus = pCDset[o];
         }
      }
      Verifications.push_back(new VerSec(CommonSections,first,o-1,IS_CODE(currentCodeStatus)));
   }
   sort(Verifications.begin(),Verifications.end(),VerSec::order);
}
??*/

// get, from each subclass, the Iconst section corresponding to the given match.
bool FileClass::getMatchingSections(const Match* pMatch, Section_vec& MatchingSections) {
   int nFound = 0, nNotFound = 0;
   for (int c = 0; c < SubClasses.size(); c++) {
      const Section_vec& SubClassIconst = SubClasses[c]->IconstSections;

      Section* pSection = NULL;
      for (int i = 0; i < SubClassIconst.size(); i++) {
         if (SubClassIconst[i]->pMatch->pParent == pMatch) {
            assert(SubClassIconst[i]->minLength == pMatch->length);
            pSection = SubClassIconst[i];
            nFound++;
            break;
         }
      }
      MatchingSections.push_back(pSection);
      if (i && !pSection) nNotFound++;
   }
   return nFound > 0 && nNotFound == 0;
}



// use pGlobalSection in place of the individual subclass' sections.
bool FileClass::getMatchingSections (Section* pGlobalSection, Section_vec& MatchingSections) {
   for (int c = 0; c < SubClasses.size(); c++) {
      MatchingSections.push_back(SubClasses[c]->IconstSections.size() ? pGlobalSection : NULL);
   }
   return true;
}



bool FileClass::sectionsAgreeOnCodeDataMapping(const Match* pMatch,
                                               const Section_vec& MatchingSections) const
{
   // first, point to the section's start in all the files' text and code-data maps
   vector<const char*> pCDset;
   vector<const byte*> pText;
   for (int c = 0; c < MatchingSections.size(); c++) {
      if (!MatchingSections[c]) continue;
      if (c && MatchingSections[c] == MatchingSections[0]) continue;   // global section

      for (int s = 0; s < MatchingSections[c]->Segments.size(); s++) {
         const Segment* pSeg = MatchingSections[c]->Segments[s];
         if (pSeg->length() != pMatch->length)
            {assert(0);return false;}
         pCDset.push_back(pSeg->pFile->CodeMap + pSeg->first);
         pText.push_back (pSeg->pFile->Invar[pMatch->encr]  + pSeg->first);
      }
   }
//?? including original samples seems to have upset this   if (pCDset.size() != pMatch->segments.size()) 
//??      return false;

   // now compare them at every position in the section.
   for (long o = 0; o < pMatch->length; o++) {
      for (int s = 1; s < pCDset.size(); s++) {
         if (IS_CODE(pCDset[s][o]) != IS_CODE(pCDset[0][o]))
            {assert(0);return false;}
         if (pText[s][o] != pText[0][o] && o >= keyLength(pMatch->encr))   // skip end-effect 
            {assert(0);return false;}
      }
   }

   return true;
}


//-----------------------------------------------------------------------------------------------

void FileClass::getVerification(vector<PAMcode*>& PAMverv) const {
   for (int v = 0; v < Verifications.size(); v++) {
      Verifications[v]->emitVerv(PAMverv);
   }
}



void FileClass::emitVerification(FILE* hPAM) const {
   int v;

   vector<PAMcode*> PAMverv;

   getVerification (PAMverv);
   
   for (v = 0; v < PAMverv.size(); v++) {
      PAMverv[v]->emit(hPAM);
      printf ("PAM %s\n", PAMverv[v]->asString().c_str());   //??
      delete PAMverv[v];
   }
}


//-----------------------------------------------------------------------------------------------

void FileClass::getReconstruction(vector<PAMcode*>& PAMverv) const {
   Location Next(START,0,0);
   for (int r = 0; r < ChosenReconList.size(); r++) {
      if (ChosenReconList[r]->HostLoc != Next)
         emitPadding(Next,ChosenReconList[r]->HostLoc-1L,PAMverv);
      ChosenReconList[r]->emitVerv(PAMverv);
      Next = ChosenReconList[r]->HostLast+1L;
   }
   if (needsMZlengthCalc()) {
      PAMcode* pPAM = new PAMcode;
      pPAM->setAction(PAM_SETLEN);
      PAMverv.push_back(pPAM);
   }
}



void FileClass::emitReconstruction(FILE* hPAM) const {
   vector<PAMcode*> PAMverv;
   int v;

   getReconstruction(PAMverv);

   for (v = 0; v < PAMverv.size(); v++) {
      PAMverv[v]->setType(classname());
      PAMverv[v]->emit(hPAM);
      printf ("PAM %s\n", PAMverv[v]->asString().c_str());   //??
      delete PAMverv[v];
   }
}



void FileClass::emitPadding(Location First,Location Last, vector<PAMcode*>& PAMverv) const {
   const long Length = Last.Offset - First.Offset + 1;

   if (First.Anchor != Last.Anchor || Length < 0) {
      printf ("** unable to pad from %s to %s\n", First.asString().c_str(), Last.asString().c_str());
      return;
   }
   // copy from same location in infected file
   PAMcode* pPAM = new PAMcode;   // NB: default encryption is PLAIN
   pPAM->setAction(PAM_REPAIR);
   pPAM->setStartLoc(First.Anchor,First.Offset);
   pPAM->setEndLoc(Last.Anchor,Last.Offset);
   pPAM->setDestLoc(First.Anchor,First.Offset);   //?? not needed if recon. is built sequentially
   PAMverv.push_back(pPAM);
}


//-----------------------------------------------------------------------------------------------

void FileClass::emitMap(vector<char*>& MapTxt) const {
   // NB: can either emit the map from the verification list or the Iconst matches - see 
   // VerSec::emitMap() in verifivation.h.
   // Chose the latter because it may allow detection where verification can't be done;
   // the generality of the map isn't really an issue because the map as generated from 
   // the verifications isn't really general: not when COM and EXE have different anchors
   // for an Iconst section, or when the section is encrypted, in which case the
   // bytes come from the first file.
   for (int i = 0; i < IconstMatches.size(); i++) {
      IconstMatches[i]->emitMap(MapTxt);
   }
//??   for (int v = 0; v < Verifications.size(); v++) {
//??      Verifications[v]->emitMap(MapTxt);
//??   }
}



void FileClass::emitMap(FILE* hFile, const char* zVirusName, const char* zDir) const {
   fprintf(hFile,"<VIRUS> %s\n",zVirusName);
   fprintf(hFile,"<DIRECTORY> %s\n",zDir);

   vector<char*> VervTxt;
   emitMap (VervTxt);
   for (int v = 0; v < VervTxt.size(); v++) {
      if (!memcmp(VervTxt[v],"<MARKER>",sizeof("<MARKER>")-1)) fprintf(hFile,"\n");
      fprintf(hFile,"%s\n", VervTxt[v]);
      delete[] VervTxt[v];
   }
   fprintf(hFile,"\n");
}


//-----------------------------------------------------------------------------------------------

void FileClass::emitPAMmap(vector<char*>& MapTxt) const {
   for (int i = 0; i < IconstSections.size(); i++) {
      IconstSections[i]->emitPAMmap(MapTxt,classname());
   }
}



void FileClass::emitPAMmap(FILE* hFile) const {
   vector<char*> VervTxt;
   emitPAMmap (VervTxt);
   for (int v = 0; v < VervTxt.size(); v++) {
      fprintf(hFile,"%s\n", VervTxt[v]);
      delete[] VervTxt[v];
   }
}

void FileClass::emitPAMmap(FILE* hFile, const char* zVirusName, const char* zDir) const {
   if (!ftell(hFile)) { //!!
      fprintf(hFile,"<VIRUS> %s\n",zVirusName);
      fprintf(hFile,"<DIRECTORY> %s\n",zDir);
   }

   vector<char*> VervTxt;
   emitPAMmap (VervTxt);
   //!!int MAXFRAGS = 60; //!! extractsig (to be replaced) can only handle 120 sigs - 60 per class
   for (int v = 0; v < VervTxt.size() /*!!&& MAXFRAGS*/; v++) {
      if (!memcmp(VervTxt[v],"<MARKER>",sizeof("<MARKER>")-1)) {
         fprintf(hFile,"\n");
         /*!!MAXFRAGS--;*/
      }
      fprintf(hFile,"%s\n", VervTxt[v]);
      delete[] VervTxt[v];
   }
   fprintf(hFile,"\n");
}

//-----------------------------------------------------------------------------------------------

void FileClass::deleteSections (void) {
   recycle(HIsections);
   recycle(IconstSections);
   recycle(Hsections);
   recycle(IvarSections);
   recycle(Isections);
   recycle(IconstMatches);
   recycle(PossibleRecons);
   recycle(ActualKeys);
   recycle(AliasedKeys);
   recycle(ChosenReconList);
   recycle(Verifications);
}
