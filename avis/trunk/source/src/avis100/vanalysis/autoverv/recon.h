/*##################################################################*/
/*                                                                  */
/* Program name:  recon.h                                           */
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

#ifndef RECON_H
#define RECON_H

#include "section.h"
#include "verification.h"

//==========================================================================================

struct Reconstruction {
   const Key*     pKey;
   Location       HostLoc;
   Location       SrcLoc;
   Location       HostLast;
   Location       SrcLast;
   long           Length;   // 1 if the length is not fixed (different start & end anchors)(??)
   const Section* pIvarSection;  // tells us if this comes from an Ivar section (desirable)

   Reconstruction (const Key* pK, Location H, Location S);
   Reconstruction (const Key* pK, Location H, Location S, Location HL, Location SL, const Section* pIvar = NULL);
   Reconstruction (const HIsection& HI);
   Reconstruction (const Reconstruction& R, long Len);

   void append (const Reconstruction& other);



   long relativeOffset (void) const {return HostLoc.Offset - SrcLoc.Offset;}

   bool isInSameGroupAs (const Reconstruction& other) const {
      return pKey->hasSameValuesAs(*other.pKey) && relativeOffset() == other.relativeOffset();
   }

   bool immediatelyFollows (const Reconstruction& other) const {
      return HostLoc.immediatelyFollows(other.HostLast);
   }

   bool precedes (const Reconstruction& other) const {
      return HostLoc.precedes(other.HostLast);
   }

   bool isContinuationOf (const Reconstruction& other) const {
      return HostLoc.immediatelyFollows(other.HostLast)
          && SrcLoc.immediatelyFollows(other.SrcLast)
          && pKey == other.pKey;
   }

   bool correspondsTo (const Reconstruction* pOther) const;

   void emitVerv (vector<PAMcode*>& PAMverv) const;

//?? needed?
   static bool hostLocLT (const Reconstruction& a, const Reconstruction& b) {
      return a.HostLoc < b.HostLoc;
   }

   static bool pHostLocLT (const Reconstruction* a, const Reconstruction* b) {
      return hostLocLT(*a,*b);
   }

//??   static bool by_Hloc_Key_Reloffset (const Reconstruction* a, const Reconstruction* b);
};

dclVector(Reconstruction)

//==========================================================================================

struct ReconGroup {
   const Key*        pKey;
   const long        RelOffset;
   long              count;      // cumulative Length of all member Reconstructions
   long              IvarCount;  // tells us if this comes from an Ivar section (desirable)


   ReconGroup (const Reconstruction& R): pKey(R.pKey), RelOffset(R.relativeOffset()),
      count(R.Length), IvarCount(R.pIvarSection ? 1 : 0) {}

   ReconGroup (void): pKey(NULL), RelOffset(0), count(0), IvarCount(0) {}

   const ReconGroup& operator+= (const ReconGroup& G) {
      count += G.count; IvarCount += G.IvarCount; return *this;
   }


   bool isSameGroupAs (ReconGroup other) const {
      return pKey->hasSameValuesAs(*other.pKey) && RelOffset == other.RelOffset;
   }

   bool isWorseThan (const ReconGroup& other) const;

   bool operator< (const ReconGroup& other) const {return isWorseThan(other);}

   static bool pLT (const ReconGroup* p1, const ReconGroup* p2) {return *p1 < *p2;}
};

dclVector(ReconGroup)

//-------------------------------------------------------------------------------------

struct ReconRank {
   ReconGroup_vec Groups;

   int add (ReconGroup R);

   const ReconGroup getBest (void);

   void ReInit (void) {recycle(Groups);}

  ~ReconRank (void) {recycle(Groups);} 
};

//==========================================================================================

#endif
