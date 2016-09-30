/*##################################################################*/
/*                                                                  */
/* Program name:  recon.cpp                                         */
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

#include "recon.h"

Reconstruction::Reconstruction(const Key* pK,Location H,Location S):
   pKey(pK), HostLoc(H), SrcLoc(S), HostLast(H), SrcLast(S), Length(1), pIvarSection(NULL) {}


Reconstruction::Reconstruction(const Key* pK,
                               Location H,Location S,
                               Location HL,Location SL,
                               const Section* pIvar):
   pKey(pK), HostLoc(H), SrcLoc(S), HostLast(HL), SrcLast(SL),
   Length (
      HostLoc.Anchor == HostLast.Anchor ? HostLast.Offset - HostLoc.Offset + 1 :
      SrcLoc.Anchor  == SrcLast.Anchor  ? SrcLast.Offset  - SrcLoc.Offset  + 1 :
      1),
    pIvarSection(pIvar)
    {}


Reconstruction::Reconstruction (const HIsection& HI):
   pKey     (HI.pActualKey), 
   HostLoc  (HI.Host.First), SrcLoc  (HI.Infected.First),
   HostLast (HI.Host.Last),  SrcLast (HI.Infected.Last),
   Length (
      HostLoc.Anchor == HostLast.Anchor ? HostLast.Offset - HostLoc.Offset + 1 :
      SrcLoc.Anchor  == SrcLast.Anchor  ? SrcLast.Offset  - SrcLoc.Offset  + 1 :
      1),
   pIvarSection(NULL)
   {assert(pKey);}


Reconstruction::Reconstruction (const Reconstruction& R, long Len):
   pKey(R.pKey),
   HostLoc  (R.HostLoc),       SrcLoc  (R.SrcLoc),
   HostLast (R.HostLoc+Len-1L), SrcLast (R.SrcLoc+Len-1L),
   Length(Len),
   pIvarSection(R.pIvarSection)
   {}



void Reconstruction::append (const Reconstruction& other) {
   HostLast = other.HostLast;
   SrcLast = other.SrcLast;
   Length += other.Length;
   if (!other.pIvarSection) //?? assumes pIvarSection is being used as a boolean   //(other.pIvarSection != pIvarSection)
      pIvarSection = NULL;
}


      
bool Reconstruction::correspondsTo (const Reconstruction* pOther) const {
   return pKey == pOther->pKey 
       && SrcLoc.Anchor == pOther->SrcLoc.Anchor 
       && relativeOffset() == pOther->relativeOffset();
}



void Reconstruction::emitVerv (vector<PAMcode*>& PAMverv) const {
   PAMcode* pPAM = new PAMcode;
   pPAM->setAction(PAM_REPAIR);
   pPAM->setEncr(pKey->Encr);
   if (pKey->Encr != PLAIN) pPAM->setKeyLoc(pKey->location().Anchor,pKey->location().Offset);
   pPAM->setStartLoc(SrcLoc.Anchor,SrcLoc.Offset);
   pPAM->setEndLoc(SrcLast.Anchor,SrcLast.Offset);
   pPAM->setDestLoc(HostLoc.Anchor,HostLoc.Offset);   //?? not needed if recon. is built sequentially
   PAMverv.push_back(pPAM);
}

/*??
bool Reconstruction::by_Hloc_Key_Reloffset (const Reconstruction* a, const Reconstruction* b) {
   if (pHostLocLT(a,b)) return true; 
   if (pHostLocLT(b,a)) return false;
   
   if (a->pKey < b->pKey) return true;
   if (b->pKey < a->pKey) return false;

   if (a->relativeOffset() < b->relativeOffset()) return true; 
   if (b->relativeOffset() < a->relativeOffset()) return false;

   if (a->HostLoc.Anchor < b->HostLoc.Anchor) return true;
   if (b->HostLoc.Anchor < a->HostLoc.Anchor) return false;

   return false;
}
***/
   
//----------------------------------------------------------------------------------

bool ReconGroup::isWorseThan (const ReconGroup& other) const {
   if (!pKey) return true;
   if (!other.pKey) return false;
   
   // in order of decreasing significance...

   //??if (!isFromIvar    &&  other.isFromIvar)    return true;
   //??if ( isFromIvar    && !other.isFromIvar)    return false;

   // plaintext
   if (pKey->Encr != PLAIN && other.pKey->Encr == PLAIN) return true;
   if (pKey->Encr == PLAIN && other.pKey->Encr != PLAIN) return false;

   // keys from HI sections (i.e. keys known to be used)
   if (!pKey->isHIkey()    &&  other.pKey->isHIkey())    return true;
   if ( pKey->isHIkey()    && !other.pKey->isHIkey())    return false;

   // reconstruction from Ivar section
   if (IvarCount < other.IvarCount) return true;
   if (IvarCount > other.IvarCount) return false;

   // large groups
   return count < other.count;
}

//----------------------------------------------------------------------------------

int ReconRank::add (ReconGroup R) {
   if (!R.pKey) return 0;
   for (int g = 0; g < Groups.size(); g++) {
      if (Groups[g]->isSameGroupAs(R)) return (*Groups[g] += R).count;  //?? do we really want to return a bool?
   }
   Groups.push_back(new ReconGroup(R));
   return R.count;
}

const ReconGroup ReconRank::getBest (void) { //?? if we re-init each time, dont need all this
   assert(Groups.size());
   if (!Groups.size()) return ReconGroup();
   make_heap(Groups.begin(), Groups.end(),ReconGroup::pLT);
   ReconGroup best = *Groups.front();
   pop_heap(Groups.begin(), Groups.end(),ReconGroup::pLT);
   return best;
}

//----------------------------------------------------------------------------------
