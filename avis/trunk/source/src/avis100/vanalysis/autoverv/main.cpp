/*##################################################################*/
/*                                                                  */
/* Program name:  main.cpp                                          */
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
//
// 15/3/99: Create H sections before finding I matches, so that H 
//          sections can be adjusted, if necessary, without 
//          conflicting with I matches (which occur coincidentally,
//          e.g. in alignment padding between the end-of-host
//          and a viral section).
//
//          This necessitated creating subclasses before the I
//          sections are known, so the I matches have to be 
//          propagated to the subclasses later.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

#include "newhandler.h"
#include "fileclass.h"
#include "loadsamples.h"
#include "match.h"
#include "test.h"
#include "lcs.h"
#include "dotProduct.h"
#include "buildtime.h"
//?? NB no need for reserve on stack vectors - they'll be temporary and freed up in LIFO order
//   - could cause fragmentation, though


static polyn* crc_setup(void);


static FileClass          GlobalClass;
static vector<FileClass*> Class;


static char DIR[512] = "";


static const char* LST = ".lst";
static const char* LCS = ".lcs";
static const char* VDS = ".vds";
static const char* MAP = ".bytes2";
static const char* SIG = ".PAMbytes";
static const char* PAM = ".ers";
static const char* XCL = ".xcl";



static void fileNameSubst (char* dest, const char* src, const char* ext) {
   strcpy(dest,src);
   if (strrchr(dest,'.')) *strrchr(dest,'.') = '\0';
   strcat(dest, ext);
}


static void getSampleDir(char* zDir, const char* zListFile) {
   FILE* h = fopen (zListFile, "r");
   if (!h) return;
   fgets(zDir,512,h);
   if (strrchr(zDir,'\\')) *strrchr(zDir,'\\') = '\0';
   fclose(h);
}


//-------------------------------------------------------------------------------------------


int main (int argc, char* argv[]) {
   Sample_vec Replicants;
   Sample_vec_iter r;
   File_vec   Samples;
   File_vec_iter s;
   int nReplicants;

   char* zWD = NULL; 
   char zListFile[512] = "*";
   char LCSname[512] = "";

   FILE* hImap = NULL;
   char ImapName[512] = "*";

   FILE* hSig = NULL;
   char IsigName[512] = "*";

   FILE* hPAM = NULL;
   char PAMname[512] = "*";

   FILE* hExcludeLog = NULL;
   char ExcludeLogName[512] = "*";

   char zDir[512] = "";

   printf ("AutoVerv 3.0 (%s)\n\n",buildTime());

   // initialize

   setNewHandler();
   //??setHeapLimit(800000000);
   CRC_TABLE = crc_setup();

   // parse arguments

   if (argc < 2) {
      puts ("insufficient arguments: (virus name, host dir) or list file");
      return 4;
   }
   const char* zVirusName = argv[1];
   const char* zHostDir   = (argc < 3 || argv[2][0] != '-') ? argv[2] : NULL;
   bool hexMode           = false;
   bool useCodeDataSeg    = false;
   bool infectionMapOnly  = false;

   for (int a = 2; a < argc; a++) {
      if (!stricmp(argv[a],"-Imap"))     strcpy(ImapName,argv[++a]);
      if (!stricmp(argv[a],"-sig"))      strcpy(IsigName,argv[++a]);
      if (!stricmp(argv[a],"-PAM"))      strcpy(PAMname,argv[++a]);
      if (!stricmp(argv[a],"-List"))     strcpy(zListFile,argv[++a]);
      if (!stricmp(argv[a],"-Codo"))     useCodeDataSeg  = true;
      if (!stricmp(argv[a],"-HImin"))    MIN_HI          = atoi(argv[++a]);
      if (!stricmp(argv[a],"-Imin"))     MIN_I           = atoi(argv[++a]);
      if (!stricmp(argv[a],"-MinRep"))   MIN_REPLICANTS  = atoi(argv[++a]);
      if (!stricmp(argv[a],"-WD"))       zWD = argv[++a];
      if (!stricmp(argv[a],"-Hex"))      hexMode = true;
      if (!stricmp(argv[a],"-LCSin"))   {LCS_IOmode = "r"; strcpy(LCSname,argv[++a]);}
      if (!stricmp(argv[a],"-LCSout"))  {LCS_IOmode = "w"; strcpy(LCSname,argv[++a]);}
      if (!stricmp(argv[a],"-ImapOnly")) infectionMapOnly = true;
      if (!stricmp(argv[a],"-HeapMax")) {setHeapLimit(atol(argv[++a]));}
   }

   // working directory
   if (zWD) {
      _getcwd(DIR,arraySize(DIR));
      if (_chdir(zWD)) {
         fprintf(stderr,"unable to go to %s\n",zWD);
         return 8;
      }
   }

   // list file
   if (zListFile[0] == '*') fileNameSubst(zListFile,zVirusName,LST);  // use the virus name with .lst extension

   // LCS file in/out
   if (LCS_IOmode) {
      if (LCSname[0] == '*') fileNameSubst(LCSname,zVirusName,LCS);  // use the virus name with .lcs extension
      hLCS_IO = fopen(LCSname,LCS_IOmode);
      if (!hLCS_IO) {
         perror("LCS file open");
         return 8;
      }
      pLCS = fileLCS;
   }else {
      pLCS = lcs;
   }

   // open results files
   if (IsigName[0] == '*') fileNameSubst(IsigName,zVirusName,SIG);  // use the virus name with .PAMbytes extension
   hSig = fopen(IsigName, "w");
   if (!hSig) {
      perror("Sig file open");
      return 8;
   }
   if (ImapName[0] == '*') fileNameSubst(ImapName,zVirusName,MAP);  // use the virus name with .bytes2 extension
   hImap = fopen(ImapName, "w");
   if (!hImap) {
      perror("Imap file open");
      return 8;
   }
   if (PAMname[0] == '*') fileNameSubst(PAMname,zVirusName,PAM);  // use the virus name with .ers extension
   hPAM = fopen(PAMname, "w");
   if (!hPAM) {
      perror("PAM/ERS file open");
      return 8;
   }
   if (ExcludeLogName[0] == '*') fileNameSubst(ExcludeLogName,zVirusName,XCL);  // use the virus name with .ers extension
   hExcludeLog = fopen(ExcludeLogName, "w");
   if (!hExcludeLog) {
      perror("Sample exclusion log file open");
      return 8;
   }

   // samples directory
   getSampleDir(zDir,zListFile);


   // begin

   if (!zHostDir) {   // preferred 
      nReplicants = loadSampleList(zListFile,Replicants,Samples,hexMode,useCodeDataSeg);
   }else {            // deprecated
      nReplicants = loadSampleList(zListFile,zHostDir,Replicants,Samples,hexMode,useCodeDataSeg);
   }



   for (r = Replicants.begin(); r != Replicants.end(); r++) {
      (*r)->matchHI();
   }



   nReplicants = DotProductExclusion (Replicants, hExcludeLog);

   if (nReplicants < MIN_REPLICANTS) {
      printf ("ERR: Replicants count %d < %d\n",nReplicants, MIN_REPLICANTS);
      return 8;
   }



   for (r = Replicants.begin(); r != Replicants.end(); r++) {
      GlobalClass.add(*r);
   }

   for (s = Samples.begin(); s != Samples.end(); s++) {
      GlobalClass.addSample(*s);
   }
   prtHImatches (GlobalClass);

   if (!infectionMapOnly && !GlobalClass.isWellMatchedToHost()) {
      puts ("WARN:- Unsatisfactory host match - signature extraction only\n");  //?? fail (FailMsg)?
      infectionMapOnly = true;
   }

   if (GlobalClass.getCriticalLength() == 0) {
      printf("COM subclass...\n"); //??
      Class.push_back (GlobalClass.makeTypeBasedFileSubClass(S_COM));

      printf("EXE subclass...\n"); //??
      Class.push_back (GlobalClass.makeTypeBasedFileSubClass(S_EXE));
   }else {
      printf("COM short subclass...\n"); //??
      Class.push_back (GlobalClass.makeLengthBasedFileSubClass(S_COM,SHORT));

      printf("COM long subclass...\n"); //??
      Class.push_back (GlobalClass.makeLengthBasedFileSubClass(S_COM,LONG));

      printf("EXE short subclass...\n"); //??
      Class.push_back (GlobalClass.makeLengthBasedFileSubClass(S_EXE,SHORT));

      printf("EXE long subclass...\n"); //??
      Class.push_back (GlobalClass.makeLengthBasedFileSubClass(S_EXE,LONG));
   }

   for (int c = 0; c < Class.size(); c++) {
      if (!Class[c]->Samples.size()) continue; //?? safe to process empty set?
      printf("\n=== class %s =================================================================\n\n",Class[c]->classname());

      if (Class[c]->Samples.size() < MIN_REPLICANTS) {
         printf ("ERR: Replicants count %d < %d in class %d\n",Class[c]->Samples.size(),MIN_REPLICANTS,c);
         return 8;
      }
      Class[c]->makeHIsections();

      prtHIsections (*Class[c]);
   }
   printf("\n=== class global ===============================================================\n\n");
   
   GlobalClass.matchIconst();
   prtIconstMatches (GlobalClass);

   GlobalClass.emitMap(hImap, zVirusName, zDir);
   GlobalClass.emitCriticalLength(hPAM);

   for (c = 0; c < Class.size(); c++) {
      if (!Class[c]->Samples.size()) continue; //?? safe to process empty set?
      printf("\n=== class %s =================================================================\n\n",Class[c]->classname());
      
      Class[c]->makeIsections();
       prtIconstSections (*Class[c]);
       prtIvarSections (*Class[c]);
       prtIsections (*Class[c]);

      Class[c]->emitPAMmap(hSig); //!!??, zVirusName, zDir);
   if (infectionMapOnly) continue;

      Class[c]->makeHsections();
       prtHsections (*Class[c]);

      Class[c]->findHIkeys();
       prtActualKeys (*Class[c]);

      Class[c]->findIconstKeys();
       prtAliasedKeys (*Class[c]);
      
      Class[c]->findPossibleHostRecons();
      //prtPossRecons (*Class[c],"PR");

      Class[c]->makePossibleFieldRecons();
       prtPossRecons (*Class[c],"PRF");

      Class[c]->chooseReconstructions();
      Class[c]->addHIreconstructions();
      Class[c]->consolidateReconstructions();

       prtActualKeys (*Class[c]);
       prtAliasedKeys (*Class[c]);
       prtChosenRecons (*Class[c]);
       
      Class[c]->makeVerifications();
       prtVerifications (*Class[c]);
      Class[c]->emitVerification(hPAM);

      Class[c]->emitReconstruction (hPAM);
       prtReconstruction (*Class[c]);
   }
   
   bool isRepairable = true;   // initial assumption
   for (c = 0; c < Class.size(); c++) {
      if (!Class[c]->Samples.size()) continue;
      printf("\n=== class %s =================================================================\n\n",Class[c]->classname());

      if (!Class[c]->reconsAreCompleteAndConsistent()) {
         isRepairable = false;
         break;
      }
   }
   puts (isRepairable ? "INFO: reconstruction OK" : "WARN: bad reconstruction");

   for (r = Replicants.begin(); r != Replicants.end(); r++) delete *r;
   for (s = Samples.begin(); s != Samples.end(); s++) delete *s;
   if (hSig) fclose(hSig);
   if (hImap) fclose(hImap);
   if (hLCS_IO) fclose(hLCS_IO);
   if (hPAM) fclose(hPAM);
   if (hExcludeLog) fclose (hExcludeLog);
   if (strlen(DIR)) _chdir(DIR);
   delete[] CRC_TABLE;
   return isRepairable ? 1 : 0;
}



void FailExit(void) {
   puts ("=== FailExit ===============================================================");
   for (int c = 0; c < 2; c++) { 
      if (!Class[c]->Samples.size()) continue;
      prtClass(*Class[c]);
   }
   if (strlen(DIR)) _chdir(DIR);
   exit(8);
}



static polyn* crc_setup(void)
{
   polyn our_poly;
   polyn *our_table;
   PWB keyphrase;

   memset(keyphrase.db, '\0', PWB_SIZE);
   strncpy((char*)keyphrase.db, "Verifier", sizeof(keyphrase.db));

   our_poly = new_poly(&keyphrase);

   our_table = crc_init(our_poly);          /* build the crc lookup table */
   if (!our_table) {
      printf("Not enough memory for CRC table.\n");
   }

   return our_table;
}  /* end crc_setup */



/***??
static void prtTable (const polyn *our_table) {
   for (int i = 0; i < 256; i++) {
      if (i % 8 == 0)  printf("\tDD "); 
      printf("0%08Xh",our_table[i]);
      if (i % 8 == 7) printf("\n");
      else            printf(", ");
   }
}
***/

