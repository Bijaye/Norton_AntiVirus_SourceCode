// Additional functions & variables for virus analysis

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "platform.h"
#include "pamapi.h"
#include "analysis.h"
#include "assert.h"

#define MIN_SIG_LEN    4
#define MAX_SIG_LEN   24

extern BOOL gbVerbose = FALSE;
extern BOOL gbTestSig = FALSE;

//-----------------------------------------------------------------------------

static FILE* LOG = NULL;
static long self_mod_run_length = 0; // # of modified instructions executed since last non-modified
static char zLastFetch[512] = "";    // description of last opcode fetched

static const char* AddrFormat (long imageOffset, const PAMLHANDLE hLocal) {
    static char text[16];
    if (imageOffset >= 0)
        sprintf (text," %8lx",imageOffset);
    else
        sprintf (text,"A%8lx",ImageToAbsOffset(imageOffset,hLocal));
    return text;
}

//-----------------------------------------------------------------------------

void logInterpretStart (const PAMLHANDLE hLocal) {
    long imageLoadAddr = -hLocal->CPU.offset_modifier;
    long imageOffsetInFile = hLocal->CPU.image_start_offset;
    int  lPrefetch = prefetch_len[hLocal->CPU.prefetch.wCurrentRequest];

    if (LOG) fprintf (LOG,"GO  prefetch=%d, image offset=%x, load addr=%x\n",lPrefetch,imageOffsetInFile,imageLoadAddr);

    clearModified();
    self_mod_run_length = 0;
    memset (hLocal->opcodes, 0x00, sizeof hLocal->opcodes);
}

// instruction counts are 1-based (all values are recorded after the counter has been updated)

void logPutByte (long image_offset, 
                 BYTE newByte, 
                 BYTE prevByte, 
                 const PAMLHANDLE hLocal)
{
   if (gbTestSig || !gbVerbose || !LOG) return;    
   fprintf (LOG,"PUT f%8ld,i%8ld,%s,%02x,%02x\n",
                      hLocal->CPU.instructionFetchCount,
                            hLocal->CPU.iteration,
                                 AddrFormat(image_offset,hLocal),
                                    newByte,
                                         prevByte);
}


long logInstr (BYTE opcode, const PAMLHANDLE hLocal) {
    long abs_off = getAbsOffset(hLocal->CPU.CS,hLocal->CPU.IP-1);   //?? IP was incremented on fetch??
    long image_off = getImageOffset(abs_off,hLocal);
    long whenModified = lastModified(abs_off);

    if (gbTestSig || !LOG) return 0;

    sprintf (zLastFetch,"OP%c %8ldf,%8ldi,%s,%02x",
                           whenModified > 0 ? '*' : ' ',
                               hLocal->CPU.instructionFetchCount,
                                     hLocal->CPU.iteration, 
                                          AddrFormat(image_off,hLocal),
                                             (int)opcode);
    if (whenModified) sprintf (zLastFetch+strlen(zLastFetch),"%8ld",whenModified);
    sprintf(zLastFetch+strlen(zLastFetch),"\n");

    if (gbVerbose || hLocal->CPU.instructionFetchCount == 1) fprintf (LOG,zLastFetch); // 1 = EP 

    // for finding signatures
    if (whenModified) {
        // do it before each sig?? if (!self_mod_run_length) logOpcodeSet(hLocal);     //save opcode set if starting a run
        self_mod_run_length++;
        //?? do it when the engine looks for signatures   logCandidateSignature(opcode, hLocal); 
    }else {
        if (image_off >= 0) self_mod_run_length = 0; //reset, unless in interrupt code
    }

    return self_mod_run_length;
}


void logScanStart (const char* zFlags) {
    if (LOG) fprintf (LOG, "SCAN %s\n", zFlags);
}


void logScan (WORD nPage, const BYTE* pPages, WORD length) {
    int b;
    if (gbTestSig || !LOG) return;

    fprintf(LOG,"PAGE %6ld,%6ld\n",(int)nPage,(int)length);
    if (gbVerbose) for (b = 0; b < length; b++) fprintf(LOG,"%02X",(int)pPages[b]);
    fprintf(LOG,"\n");
}

//-----------------------------------------------------------------------------

void updateBuildSet (const BOOL* hits) {
    int i;
    BOOL isNewFile = FALSE;
    FILE* stream = fopen("build.set","rb+");
    if (!stream) {
       isNewFile = TRUE;
       stream = fopen("build.set","wb");
       if (!stream) {
           printf("Unable to open BUILD.SET file.\n");
           exit(4);
       }
    }
    
    for (i=0;i<256;i++) {
        if (hits[i] || isNewFile) {
            if (!isNewFile) fseek(stream,i,SEEK_SET);
            fputc(hits[i] ? '1' : '0',stream);
        }
    }
        
    fclose(stream);
}

//-----------------------------------------------------------------------------

long getAbsOffset (WORD seg, WORD offset) { 
    long global_offset = (DWORD)seg * PARAGRAPH;
    global_offset += offset;
    global_offset &= EFFECTIVE_ADDRESS_MASK;
    return global_offset;
}

long getImageOffset(long global_offset, const PAMLHANDLE hLocal) {
    return global_offset + hLocal->CPU.offset_modifier;
}

long ImageToAbsOffset(long image_offset, const PAMLHANDLE hLocal) {
    return image_offset - hLocal->CPU.offset_modifier;
}

//-----------------------------------------------------------------------------

BOOL initAnalysis (const char* zHome) {
    char zLog[MAXDIR];
    assert (!LOG);

    sprintf (zLog,"%s\\emulate.log",zHome);
    LOG = fopen (zLog,"wb");

    if (!LOG) {
        printf ("FAIL %d opening LOG %s\n",errno,zLog);
        return FALSE;
    }

    return TRUE;
}


void endAnalysis (void) {
    if (LOG) fclose (LOG);
    LOG = NULL;
}


int loadSig (PAMLHANDLE hLocal, const char* zSigText) {
    WORD i;
    hLocal->stop_sig.length = 0;
    if (!zSigText) return 0;
    for (i = 0; i < strlen(zSigText); i+=2) {
        int n;
        while (zSigText[i] && !isxdigit(zSigText[i])) i++; // sig may contain separators
        assert(isxdigit(zSigText[i]) && isxdigit(zSigText[i+1]) && hLocal->stop_sig.length < SIG_LEN);
        sscanf(zSigText+i,"%02x", &n);
        *(hLocal->stop_sig.sig + hLocal->stop_sig.length++) = (BYTE)n;
    }

    printf("Stop sig: %s\n",zSigText);
    return hLocal->stop_sig.length;
}

//-----------------------------------------------------------------------------

// For each byte in the address space, the instruction fetch count at the last 
// time it was modified.
static long modified [0x100000];


long markModified (unsigned long addr,const PAMLHANDLE hLocal) {
    long prior = modified[addr];
    assert(addr >=0 && addr < ArraySize(modified));
    modified[addr] = hLocal->CPU.iteration;
    return prior;
}

long lastModified (unsigned long addr) {
    return modified[addr];
}

BOOL isModified (unsigned long addr) {
    return lastModified(addr) > 0;
}

void clearModified (void) {
   memset (modified, 0x00, sizeof modified);
}



static const char* OpcodeSetStr(const PAMLHANDLE hLocal) {
    static char str[ArraySize(hLocal->opcodes)+1]; 
    int i;
    for (i=0; i < ArraySize(hLocal->opcodes); i++) {
        sprintf (str+i,"%c", hLocal->opcodes[i] ? '1' : '0');
    }
    return str;
}

void logOpcodeSet(const PAMLHANDLE hLocal) {
    if (LOG) fprintf (LOG, "INST %s\n",OpcodeSetStr(hLocal));
}


// if we are executing self-modified code, extract candidate signatures
BOOL logCandidateSignature (BYTE opcode, const PAMLHANDLE hLocal) {
    static const BYTE SigStartByte[]   = {0x2E, 0x3D, 0x50, 0x74, 0x75, 0x8B, 0xB4, 0xB8,
                                          0xB9, 0xBA, 0xBB, 0xBE, 0xBF, 0xCD, 0xE8, 0xE9};
    BYTE Sig[MAX_SIG_LEN];
    WORD CS = hLocal->CPU.CS;
    WORD IP = hLocal->CPU.IP - 1;   //appears to have been incremented after fetch
    long abs_off = getAbsOffset (CS, IP);
    long whenModified = lastModified(abs_off);
    long InstrCount = 0;
    WORD lSig;
    WORD i;

    if (get_byte (hLocal, CS, IP) != opcode) {
       printf ("apparent prefetch mismatch @ %ld (%04X:%04X): opcode %02X != memory %02X\n",abs_off,CS,IP,opcode,get_byte (hLocal, CS, IP));
       return FALSE;   // skip signatures affected by prefetch non-determinism
    }

    for (i = 0; i < ArraySize(SigStartByte) && opcode != SigStartByte[i]; i++);
    if (i == ArraySize(SigStartByte)) return FALSE;   // opcode is not a valid signature start

    for (i = 0; i < MAX_SIG_LEN; i++) {
       long whenModified = lastModified(getAbsOffset(CS,IP));
       if (!whenModified) break;
       if (whenModified > InstrCount) InstrCount = whenModified;
       Sig[i] = get_byte (hLocal, CS, IP);
       if (++IP == 0) break;   // IP wrap   CS += 0x1000; ?? 
    }

    if (i < MIN_SIG_LEN) return FALSE;

    lSig = i;
    if (!gbVerbose) fprintf (LOG,zLastFetch);   // if in verbose mode, it was already printed 
    logOpcodeSet(hLocal);
    fprintf (LOG,"SIG %8ld %8ld ",InstrCount,hLocal->CPU.iteration);
    for (i = 0; i < lSig; i++) {
        fprintf(LOG,"%c%02X",i ? ',' : ' ',Sig[i]);
    }
    fprintf (LOG,"\n");

    //?? install the sig to see if it will be detected
    memcpy (hLocal->stop_sig.sig, Sig, lSig);
    hLocal->stop_sig.length = lSig;
    return TRUE;
}

//-----------------------------------------------------------------------------

WORD findTestSig (PAMLHANDLE hLocal, BYTE *buffer, WORD buff_len) {
	WORD i, j;

   if (!hLocal->stop_sig.length) return NOT_FOUND;

	/* signature must be within 512-SIG_LEN bytes in buffer */
	buff_len -= SIG_LEN;

	for (i=0; i < buff_len; i++,buffer++) {
      if (*buffer = hLocal->stop_sig.sig[0]) {
         for (j = 0; j < hLocal->stop_sig.length; j++) {
            if (buffer[j] != hLocal->stop_sig.sig[j]) break;
         }
         if (j == hLocal->stop_sig.length) {
            logOpcodeSet(hLocal);
            return 1;
         }
      }
   }
   return NOT_FOUND;
}

//-----------------------------------------------------------------------------

long gnSigs = 0;
char gzSigList[_MAX_PATH+1] = "";
BOOL gbSigSorted = FALSE;

static BYTE* sig;
static int*  sigLen;
static BOOL* sigFound;

static int*  lRemaining;


void loadSigs (void) {
   char line[2*SIG_LEN + 2];
   int nSigs = 0;
   int c;

   FILE* hSig = fopen(gzSigList, "r");
   if (!hSig) {fputs(gzSigList,stderr);perror(" - sig list open error");exit(4);}

   sig = calloc(gnSigs,sizeof(*sig)*(SIG_LEN+1));
   if (!sig) {fputs("out of sigArray memory\n",stderr);exit(5);}

   sigLen = calloc(gnSigs,sizeof(*sigLen));
   if (!sigLen) {fputs("out of sigLen memory\n",stderr);exit(6);}

   sigFound = calloc(gnSigs,sizeof(*sigFound));
   if (!sigFound) {fputs("out of sigFound memory\n",stderr);exit(7);}

   lRemaining = calloc(gnSigs,sizeof(*lRemaining));
   if (!lRemaining) {fputs("out of lRemaining memory\n",stderr);exit(8);}

   while (fgets (line, sizeof(line), hSig) && nSigs < gnSigs) {
       char* ps = line;
       BYTE* pd = sig + nSigs;
       int n = 0;
       while (isxdigit(*ps)) {
           if (n >= SIG_LEN) {fprintf(stderr,"signature %d longer than %d\n",nSigs,SIG_LEN);exit(9);}
           sscanf(ps, "%02X", &c);
           *pd = (BYTE)c;
           n++;
           ps+=2;
           pd += gnSigs;
       }
       sigLen[nSigs] = n;
       nSigs++;
   }
   fclose(hSig);
   if (nSigs != gnSigs) {fputs("didn't read all the sigs\n",stderr);exit(7);}
}


static const char* sigStr (int s) {
    static char str[SIG_LEN*2+1];
    BYTE* p = sig + s;
    int n;
    for (n = 0; n < sigLen[s]; n++, p += gnSigs) {
        sprintf(str + (2*n), "%02X", *p);
    }
    return str;
}


typedef struct {int offset, startSig, endSig;} SigSearchPosn;
static const SigSearchPosn initSigSearch = {0,0,0};


static BOOL findSigMatchesAtPosn (BYTE b, SigSearchPosn* pPosn, int* pHits, PAMLHANDLE hLocal) {
    int s, nextStartSig = -1, nextEndSig = -1;
    BYTE* p;

    if (pPosn->offset == 0) {   // begin new search
        // ?? if sorted, binary-search to set startSig, endSig?
        pPosn->startSig = 0;
        pPosn->endSig   = gnSigs;
        memcpy(lRemaining,sigLen,gnSigs * sizeof(*sigLen));
    }

    p = sig + (pPosn->offset * gnSigs) + pPosn->startSig;
    for (s = pPosn->startSig; s < pPosn->endSig; (s++,p++)) {
        if (sigFound[s]) continue;
        if (*p == b) {
            if (--lRemaining[s] == 0) {
                if (gbVerbose) printf("Hit signature %d after %ld\n",s,hLocal->CPU.iteration);
                fprintf(LOG,"SIGFOUND %d %ld %s %s\n",s,hLocal->CPU.iteration,OpcodeSetStr(hLocal),sigStr(s));
                sigFound[s] = TRUE;
                (*pHits)++;
            }else {
                if (nextStartSig < 0) nextStartSig = s;
                nextEndSig = s + 1;
            }
        }else {
            lRemaining[s] = -1;
        }
    }
    pPosn->startSig = nextStartSig;   // doesn't matter if the sigs were sorted,
    pPosn->endSig   = nextEndSig;     // but if they were, these will converge, esp. after the first pass
    pPosn->offset++;
    return nextEndSig > nextStartSig;
}



int findSigsAtIP (BYTE opcode, PAMLHANDLE hLocal) {
    WORD CS = hLocal->CPU.CS;
    WORD IP = hLocal->CPU.IP - 1;  //NB: CPU.IP was incremented on instruction fetch
    int hits = 0;
    SigSearchPosn posn = initSigSearch;
    BYTE b = opcode;

    while (findSigMatchesAtPosn(b, &posn, &hits, hLocal)) {
        b = get_byte(hLocal, CS, ++IP);
        if (IP == 0) {
            printf ("\nIP wrap at offset %d\n", posn.offset);
            break;
        }
    }
    return hits;
}


WORD scanForSigs (PAMLHANDLE hLocal, BYTE *buffer, WORD buff_len) {
   int hits = 0;
	WORD i;

	/* signature must be within 512-SIG_LEN bytes in buffer */
	buff_len -= SIG_LEN;

	for (i=0; i < buff_len; i++,buffer++) {
        SigSearchPosn posn = initSigSearch;
        while (findSigMatchesAtPosn(buffer[posn.offset], &posn, &hits, hLocal));
   }
   return hits ? 1 : NOT_FOUND;
}

//-----------------------------------------------------------------------------

