/*##################################################################*/
/*                                                                  */
/* Program name:  FileClass.h                                       */
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

#ifndef FILECLASS_H
#define FILECLASS_H

#include "file.h"
#include "section.h"
#include "sample.h"
#include "recon.h"
#include "key.h"
#include "verification.h"

//-------------------------------------------------------------------------------------

struct LengthBasedFileSubClass;
struct TypeBasedFileSubClass;

//-------------------------------------------------------------------------------------

struct FileClass {
   Sample_vec          Samples;
   File_vec            HostFiles;
   File_vec            InfectedGoats;
   File_vec            ViralFiles;   // infected goats + original samples

   FileClass*          pParent;
   vector<FileClass*>  SubClasses;

   Match_vec           IconstMatches;

   HIsection_vec       HIsections;
   Section_vec         HIIsections;
   Section_vec         HIHsections;
   Section_vec         Hsections;
   Section_vec         Isections;   //list of sections in which to search for actual keys (Iconst + Ivar sections)
   Section_vec         IconstSections;
   Section_vec         IvarSections;

   ActualKeySet        ActualKeys;
   AliasedKeySet       AliasedKeys;
   
   Reconstruction_vec  PossibleRecons;
   ReconRank           PossibleReconRank;
   Reconstruction_vec  ChosenReconList;
   vector<VerSec*>     Verifications;
   long                MinVirusLength;
   long                MaxVirusLength;
   long                Threshold;
   
   FileClass(void): pParent(NULL), MinVirusLength(LONG_MAX), MaxVirusLength(0L), Threshold(-1L) {}
  ~FileClass(void);

   void subClass (FileClass& SubClass);
   TypeBasedFileSubClass* makeTypeBasedFileSubClass (FILE_TYPE F);
   LengthBasedFileSubClass* makeLengthBasedFileSubClass(FILE_TYPE F, FILELENGTH_TYPE L);
   virtual bool accept (const Sample* pSample) const {return true;}   // does sample belong in this class?
   virtual const char* classname (void) const        {return "FILE";}
   long getCriticalLength (void); //The length that divides short from long files, if it matters (0 if not)
   void emitCriticalLength(FILE* hFile) const;
   void getParentsIconstMatches (void);

   bool add (Sample* pSample);
   void addSample (File* pFile);
   void matchIconst (void);
   void makeHIsections (void);
   void makeIsections (void);
   void makeHsections (void);
   void findHIkeys (void);
   void findIconstKeys (void);
   void findPossibleHostRecons (void);
   void makePossibleFieldRecons (void);
   void chooseReconstructions (void);
   void addHIreconstructions(void);
   void consolidateReconstructions(void);
   void makeVerifications(void);

   bool reconsAreCompleteAndConsistent(void) const;
   bool needsMZlengthCalc (void) const;
   bool isAllOfType(FILE_TYPE Type) const;
   bool isWellMatchedToHost (int minMatchPercent = 50, unsigned long maxUnmatchedLength = 50) const;

   void getVerification(vector<PAMcode*>& PAMverv) const;
   void emitVerification(FILE* hPAM) const;
   void getReconstruction(vector<PAMcode*>& PAMverv) const;
   void emitReconstruction(FILE* hPAM) const;
   void emitMap(vector<char*>& MapTxt) const;
   void emitMap(FILE* hFile, const char* zVirusName, const char* zDir) const;
   void emitPAMmap(vector<char*>& MapTxt) const;
   void emitPAMmap(FILE* hFile) const;
void emitPAMmap(FILE* hFile, const char* zVirusName, const char* zDir) const; //!!??old, after extractsig replaced

private:
   HIsection* makeNthHIsections (int n);
   void makeIconstSections (void);
   void makeIvarSections (void);
   void makeIsectionList (void);
   int  findInterSections (const Section_vec& Src, Section_vec& Gaps);
   void mergeSections (const Section_vec& Src1, const Section_vec& Src2, Section_vec& Dest);
   void addSectionToList (Section* pNewSec, Section_vec& Dest);
   int  findPossibleLocnRecons (Location Hloc, Location Iloc);
   int  findPossibleEncrLocnRecons (Location Hloc, Location Iloc, const KeySet& Keys);
   int  makePossibleByteRecon (const Key* pKey, Location Hloc, Location Iloc);
   bool isFieldStartInAllFiles(Location Loc);
   void findFullFieldRecons (Location Hbegin, Location Hend);
   void invalidateConflictingRecons (Reconstruction P);   // need to do copy as item will be deleted
   bool getMatchingSections(const Match* pMatch, Section_vec& MatchingSections);
   bool getMatchingSections (Section* pGlobalSection, Section_vec& MatchingSections);
   bool sectionsAgreeOnCodeDataMapping(const Match* pMatch,const Section_vec& MatchingSections) const;
   void emitPadding(Location Prev,Location Next,vector<PAMcode*>& PAMverv) const;
//??   void removeIncompleteFieldRecons (void);

   void groupPossibleRecons (void);


   void deleteSections (void);
};

dclVector(FileClass)

//- subclasses ----------------------------------------------------------------

struct TypeBasedFileSubClass: FileClass {
   FILE_TYPE           FileType;

   TypeBasedFileSubClass (FILE_TYPE F): FileType(F) {}

   bool accept (const Sample* pSample) const {
      return pSample->Inf.file_type == FileType;
   }

   const char* classname (void) const {return Filetype_txt[FileType];}

};

dclVector(TypeBasedFileSubClass)



struct LengthBasedFileSubClass: FileClass {
   FILE_TYPE            FileType;
   FILELENGTH_TYPE      LengthType;
   char                 Name[8];

   LengthBasedFileSubClass (FILE_TYPE F, FILELENGTH_TYPE L): 
      FileType(F), LengthType(L) {
         strcpy (Name, Filetype_txt[FileType]);
         strcat (Name, Lengthtype_txt[LengthType]);
      }

   bool accept (const Sample* pSample) const {
      if (pSample->Inf.file_type != FileType) return false;
      if (LengthType == LONG) return pSample->Inf.length >  Threshold;
      else                    return pSample->Inf.length <= Threshold;
   }

   const char* classname (void) const {return Name;}

};

dclVector(LengthBasedFileSubClass)



#endif
