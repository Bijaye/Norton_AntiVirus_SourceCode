// Additional functions & variables for virus analysis

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <assert.h>
#include "pamapi.h"

extern BOOL gbVerbose;
extern BOOL gbTestSig;

void logPutByte (long image_offset, 
                 BYTE newByte, BYTE prevByte, 
                 const PAMLHANDLE hLocal);
long logInstr (BYTE opcode, const PAMLHANDLE hLocal);
void logScanStart (const char* zFlags);
void logScan (WORD nPage, const BYTE* pPages, WORD length);

void updateBuildSet (const BOOL* hits);

long getAbsOffset (WORD seg, WORD offset);
long getImageOffset(long global_offset, const PAMLHANDLE hLocal);
long ImageToAbsOffset(long image_offset, const PAMLHANDLE hLocal);

BOOL initAnalysis (const char* zHome);
void logInterpretStart (const PAMLHANDLE hLocal);
void endAnalysis (void);
int  loadSig (PAMLHANDLE hLocal, const char* zSigText);

long markModified (unsigned long addr,const PAMLHANDLE hLocal);
long lastModified (unsigned long addr);
BOOL isModified (unsigned long addr);
void clearModified (void);


void logOpcodeSet (const PAMLHANDLE hLocal);
BOOL logCandidateSignature (BYTE opcode, const PAMLHANDLE hLocal);

WORD findTestSig (PAMLHANDLE hLocal, BYTE *buffer, WORD buff_len);


extern long gnSigs;
extern char gzSigList[_MAX_PATH+1];
extern BOOL gbSigSorted;

void loadSigs (void);
int  findSigsAtIP (BYTE opcode, PAMLHANDLE hLocal);
WORD scanForSigs (PAMLHANDLE hLocal, BYTE *buffer, WORD buff_len);

#endif
