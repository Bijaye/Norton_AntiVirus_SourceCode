//===========================================================================
//
//                               TESTGATE.CPP
//
// Application to do stress and intensive tests for Immune system gateway
// functionality.
//
//===========================================================================


// Suppress warnings, coming from Bill Gates's code.
//
#pragma  warning (disable : 4201)   // Suppress warning: "Nonstandard extension used..." (from MS *.H files.)
#pragma  warning (disable : 4514)   // Suppress warning: "Unreferenced inline function has been removed..."

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <conio.h>
#include <stdio.h>

#include "tg_all.h"


//---------------------------------------------------------------------------
// Local definitions.

typedef struct _PRODUCT_VERSION_INFO
{
   char     *pProjectName;    // Ex: "MYTOOL"
   DWORD    dwVersionMajor;   // Ex: 1
   DWORD    dwVersionMinor;   // Ex: 2
   DWORD    dwBuildNumber;    // Ex: 375
} PRODUCT_VERSION_INFO;

PRODUCT_VERSION_INFO   g_MyProjectVersionInfo = {
#include "version.inc"
};

#define  CRITICAL_ERROR_IDS_MAX  10

typedef struct _SAM_THREAD_RESULTS
{
   DWORD                dwUploadedFiles;           // Successfully uploaded files.
   unsigned __int64     iUploadedBytes;            // Total number of successfully uploaded bytes of file (without HTTP headers).
   DWORD                dwFileIoErrors;
   DWORD                dwTimeouts;
   DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
} SAM_THREAD_RESULTS;

typedef struct _SIG_THREAD_RESULTS
{
   DWORD                dwDownloadedSignatureSets; // Successfully downloaded signature sets.
   unsigned __int64     iDownloadedBytes;          // Total number of successfully downloaded bytes.
   DWORD                dwTimeouts;
   DWORD                dwErraticReplies;          // Any reply, that did not contain expected data.
   DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
} SIG_THREAD_RESULTS;

typedef struct _SQ_THREAD_RESULTS
{
   DWORD                dwRequestsSubmitted;
   DWORD                dwRepliesReceived;
   DWORD                dwTimeouts;
   DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
} SQ_THREAD_RESULTS;


#define  THREAD_TYPE_SAM   1
#define  THREAD_TYPE_SIG   2
#define  THREAD_TYPE_SQ    3
typedef struct _ALL_THREAD_PARAMS
{
   HANDLE               hThread;
   unsigned             uThreadID;                    // GetCurrentThreadId();
   DWORD                dwThreadType;                 // THREAD_TYPE_*

   char                 acGatewayNameOrIP [MAX_PATH]; // '/gateway='. navajo.watson.ibm.com. From command line or from .INI file.
   unsigned short       usGatewayPort;                // '/port='. 2847. From command line or from .INI file.
   BOOL                 bUseSSL;                      // '/ssl'.
   SSL_CTX              *pSslContext;                 // Used for SSL connections.
   BOOL                 bZapBits;                     // '/zapbits' - randomly change file contents before submitting file.
   BOOL                 bZapNames;                    // '/zapnames' - randomly change file name before submitting file.
   int                  iMaxFilesPerSamThread;        // -1:as many as found, else use this number.
   int                  iMaxSigsPerSigThread;         // -1:infinitely in a loop, else use this number.
   int                  iSamDelay;                    // ms. of delay between file submissions or between retries after error occured.
   int                  iSamErrorsMax;                // Allow this number of critical errors per Sam thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
   char                 ***pppCheckSumsNew;           // Array of ASCIIZ md5 crcs, saved by SAM threads to be used later by SQ threads.
   int                  *piCheckSumsNewNum;           // number of elements in array.
   int                  iMaxCheckSumsNew;             // Save no more, than this number of checksums.

   BOOL                 bLoop;                        // '/loop'. Do sample submission is a loop until key pressed.
   BOOL                 bVerbose;                     // '/verbose'. Log HTTP transaction data to screen.
   BOOL                 bManualSample;                // '/sammanual'. Set X-Sample-Reason to 'manual'
   BOOL                 bSkipDownload;                // '/sigskipdownload'. Skip signature defintion downloads

   FILE_INFO            **ppFileInfos;                // FILE_INFO   **ppFileInfos: Array of pointers to 'FILE_INFO' structure.
   int                  iFileInfosNum;                // int         iFileInfosNum: Number of elements in array.

   CONSTANT_INFO        *pConstantInfo;               // Needed for 'Allocate_HTTP_Header_For_SubmitSample()'.

   int                  iSigDelay;                    // ms. of delay between downloads of blessed signature sets or between retries after error occured.
   int                  iSigErrorsMax;                // Allow this number of critical errors per Sig thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.

   int                  iMaxSQsPerSQThread;           // -1:unlimited. Max. number of status queries for SQ thread to perform. '/maxsqs='.
   char                 **ppSQCheckSums;              // Array of ASCIIZ md5 crc: "FB40A7....8C" used by SQ thread to query status.
   int                  iSQCheckSumsNum;              // number of elements in array.
   int                  iSQDelay;
   int                  iSQErrorsMax;
   SQ_THREAD_RESULTS    SQResults;

   int                  iMinDelay;
   int                  iMaxDelay;
   int                  iSeed;

   SAM_THREAD_RESULTS   SamResults;
   SIG_THREAD_RESULTS   SigResults;

   HANDLE               hEventTerminating;            // Will be set into signalled state if all termination needed.
} ALL_THREAD_PARAMS;


typedef struct _DOWNLOAD_SIGSET_CALLBACK_DATA
{
   ALL_THREAD_PARAMS    *pTP;
   DWORD                dwBytesReceivedSoFar;
   DWORD                dwStartTimerValue;
   BOOL                 bAboutToSendVeryFirstDataChunk;

   BOOL                 bReceivedVeryFirstDataChunk;
   DWORD                dwTotalSizeOfDataToExpect;
   char                 *pDataBuffer;
   int                  iDataBufferSize;
} DOWNLOAD_SIGSET_CALLBACK_DATA;


typedef struct _UPLOAD_SAMPLE_CALLBACK_DATA
{
   ALL_THREAD_PARAMS    *pTP;
   DWORD                dwBytesSentSoFar;       // 0 at first.
   DWORD                dwBytesReceivedSoFar;   // 0 at first.
   DWORD                dwStartTimerValue;
   BOOL                 bAboutToSendVeryFirstDataChunk; // TRUE at first.
   HANDLE               hFile;
   DWORD                dwFileSize;
   DWORD                dwZapKey;
   VOID                 *pvScrambleHandle;

   char                 *pDataBuffer;           // Preallocated buffer
   int                  iDataBufferSize;        // Size of it.
   char                 *pHTTPHeader;           // HTTP header.
   int                  iHTTPHeaderSize;        // Size of it.
} UPLOAD_SAMPLE_CALLBACK_DATA;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Globals.
char           g_acCopyright [] =
"\n            +====================================================+"
///!!!"\n            |        *** LoadGate ver %d.%02d [build %03d] ***       |"
"\n            |        *** LoadGate ver %s [build %s] ***    |"
"\n            |        Immune system gateway testing utility.      |"
"\n            |         Copyright (c) 2000, IBM Corporation        |"
"\n            |               All Rights Reserved                  |"
"\n            |                                                    |"
"\n            |           Written by Gleb Esman (gesman@mensk.com) |"
"\n            +====================================================+";

char           g_acSyntax1 [] =
"\nSyntax: LOADGATE /[parameters] include_specs :exclude_specs                    ";

char           g_acSyntax2 [] =
"\n   /gateway=NAME_IP  - Name or IP address of gateway.                          "
"\n   /port=NNN         - Gateway's port number to connect to.                    "
"\n   /ssl              - Use SSL for communication with gateway. Default:No.     "
"\n   /dir=DIRNAME      - Name of directory where to search for samples to submit."
"\n                       Could be in UNC format. See examples.                   "
"\n   /s                - Include subdirectories. Default=FALSE.                  "
"\n   include_specs     - Any number of filespecs, describing samples to search.  "
"\n                       '*' and '?' allowed. Ex: FILE*.T?? *Vir*.CO* MYFILE.EXE "
"\n   :exclude_specs    - Any number of filespecs, describing files to exclude.   "
"\n                       '*' and '?' allowed. Ex: :*.BAK :*.TMP :CLEAN*.???      "
"\n   /minsize=NNN      \\ Include samples with sizes within this range.           "
"\n   /maxsize=NNN      / Default: 0...Infinite.                                  "
"\n   /:minsize=NNN     \\ Exclude samples with sizes within this range.           "
"\n   /:maxsize=NNN     / Default: Infinite...Infinite.                           "
"\n   /maxfiles=NNN     - Do not submit more, than this number of files per       "
"\n                       thread, regardless of how many files found.             "
"\n   /maxsigs=NNN      - Do not download more, than this number of signature sets"
"\n                       per signature thread. /sigthreads - must be specified.  "
"\n   /maxsqs=NNN       - Status queries per SQ thread. Default=unlimited.        "
"\n   /sqdelay=NNN      - Set delay (ms) between status queries.                  "
"\n   /sqerrorsmax=NNN  - Allows maximum this number or critical errors per       "
"\n                       status query thread. -1:infinite(default).              "
"\n   /samthreads=NNN   - Number of sample submission threads to spawn. Default=1."
"\n                       /samthreads=0 will not spawn any submission threads.    "
"\n   /samdelay=NNN     - Set delay (ms) after submitting of sample file.         "
"\n   /samerrorsmax=NNN - Allows maximum this number or critical errors per       "
"\n                       sample submission thread. -1:infinite(default).         "
"\n   /sammanual        - Set the X-Sample-Reason attribute to 'manual' in all    "
"\n                       samples submitted. (default is 'unknown')               "
"\n   /sigskipdownload  - Skip signature definition downloads, just check def     "
"\n                       availabililty (default is check and then download)      "
"\n   /zapbits          - Randomly modify file data to submit to gateway.         "
"\n   /zapnames         - Randomly modify filenames to submit to gateway.         "
"\n   /loop             - Indefinitely repeat submission of files and downloading "
"\n                       of signature sets (if /sigthreads= was specified).      "
"\n                       Pressing <ESC> will terminate loop.                     "
"\n   /verbose          - Enable display of HTTP transaction data and signature   "
"\n                       download progress report lines.                         "
"\n                       Pressing <ESC> will terminate loop.                     "
"\n   /sigthreads=NNN   - Number of additional threads, dedicated to downloading  "
"\n                       of the latest blessed signature set. Default=0          "
"\n   /sigdelay=NNN     - Set delay (ms) after every download of signature set.   "
"\n   /sigerrorsmax=NNN - Allows maximum this number or critical errors per       "
"\n                       signature downloader thread. -1:infinite(default).      "
"\n   /sqthreads=NNN    - Number of threads, dedicated to status query requests.  "
"\n                       Default=0                                               "
"\n   /mindelay=NNN     - Minimum delay (ms) before performing an action.         "
"\n                       Used by all threads. Default = 0.                       "
"\n                       Thread will randomly be delayed between 'mindelay' and  "
"\n                       'maxdelay' ms before performing an action.              "
"\n   /maxdelay=NNN     - Maximum delay (ms) before performing an action.         "
"\n                       Used by all threads. Default = 0. If mindelay=maxdelay=0"
"\n                       then thread will perform no delay.                      "
"\n                       Thread will randomly be delayed between 'mindelay' and  "
"\n                       'maxdelay' ms before performing an action.              "
"\n   /seed=NNN         - Starting value to generate random delays. Unsigned int. "
"\n                       Default:0. In that case current timer value used.       "
"\n   /noask              Do not ask user's confirmation before starting tests.   "
"\n   /profile=PrfName  - Save all current settings into the LOADGATE.INI file    "
"\n                       under the section: [PrfName]. Subsequent runs:          "
"\n                       LOADGATE /profile=PrfName   - will allow to use all the "
"\n                       same settings, without actually typing them all.        "
"\n   /getsums=filename - Read checksums for status query threads from this file. "
"\n   /setsums=filename - Write checksums for newly uploaded files to this file.  "
"\n   /?                - Print this help.                                        "
"\n   Note: LOADGATE never alters or modify any files or directories on the       "
"\n         local machine. Modifications of file data or file names is referred to"
"\n         the contents of HTTP requests, not to the actual files on the local   "
"\n         system.                         Direct questions to: gesman@mensk.com ";

char           g_acSyntax3 [] =
"\nExamples:                                                                      "
"\n   LOADGATE /dir=D:\\samples /s /samthreads=100 /zapbits                       "
"\n   LOADGATE /dir=e:\\samples\\bin /s /maxsize=1000000 /profile=TEST100         "
"\n   LOADGATE /dir=\\\\Server\\SHARE\\Dir *.v* :*.vbx *.com /s /profile=LanFiles "
"\n   LOADGATE /dir=\\\\9.21.5.22\\SHARE\\Dir *.v* :*.vbx :*.vxd /s /zapbits /loop"
"\n   LOADGATE /dir=d:\\test *.exe *.com /s /samthreads=500 /profile=ToughOne     "
"\n   LOADGATE /profile=ToughOne        - runs with previously recorded settings. "
"\n   LOADGATE /profile=ToughOne /samthreads=250    - updates and run profile.    "
"\n   LOADGATE /samthreads=0 /sigthreads=100 /profile=ToughSignatureDownloaderTest"
"\n   LOADGATE /samthreads=1 /sqthreads=10 /mincookie=1 /maxcookie=1000           ";



WORD           g_wOrigAttrs                     =  FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE;

char           g_acIniFilename            [MAX_PATH];
char           g_acLogFilename            [MAX_PATH];

char           *g_acGetSumsFrom_CMDINI []       =  {"/getsums=",  "Filename_to_retrieve_checksums_from" };
char           *g_acSetSumsTo_CMDINI []         =  {"/setsums=",  "Filename_to_store_checksums_to" };

char           g_acTestgateNameString []        =  "LoadGate";
char           g_acTestgateUndefinedString []   =  "LOADGATE_undefined";

char           *g_acSamplesRootDir_CMDINI []    =  {"/dir=",      "Samples_root_directory"   };
char           *g_acGatewayNameOrIP_CMDINI []   =  {"/gateway=",  "Gateway_name_or_IP" };
char           *g_iGatewayPort_CMDINI []        =  {"/port=",     "Gateway_port" };

char           *g_iSamThreadsNum_CMDINI []      =  {"/samthreads=",  "Submit_Sample_Threads_number"  };
char           *g_iSamDelay_CMDINI []           =  {"/samdelay=",    "File_delay(between_submits,ms)" };
char           *g_iSQDelay_CMDINI []            =  {"/sqdelay=",    "Delay_between_status_queries,ms)" };
char           *g_iSamErrorsMax_CMDINI []       =  {"/samerrorsmax=","Max_errors_per_sample_thread" };
char           *g_iSQErrorsMax_CMDINI []        =  {"/sqerrorsmax=","Max_errors_per_status_query_thread" };
char           *g_iSigThreadsNum_CMDINI []      =  {"/sigthreads=","Download_Sig_Set_Threads_number"  };
char           *g_iSQThreadsNum_CMDINI []       =  {"/sqthreads=","Status_Query_Threads_number"  };
char           *g_iSigDelay_CMDINI []           =  {"/sigdelay=", "Sig_Set_delay(between_downloads,ms)" };
char           *g_iSigErrorsMax_CMDINI []       =  {"/sigerrorsmax=","Max_errors_per_sigset_thread" };
char           *g_iMinDelay_CMDINI []           =  {"/mindelay=", "Min_delay_ms" };
char           *g_iMaxDelay_CMDINI []           =  {"/maxdelay=", "Max_delay_ms" };
char           *g_iSeed_CMDINI []               =  {"/seed=",     "Seed_for_random_generator" };
char           *g_bIncludeSubdirs_CMDINI []     =  {"/s",         "Include_subdirs"    };
char           *g_iIncludeSizeMin_CMDINI []     =  {"/minsize=",  "Include_size_min"   };
char           *g_iIncludeSizeMax_CMDINI []     =  {"/maxsize=",  "Include_size_max"   };
char           *g_iExcludeSizeMin_CMDINI []     =  {"/:minsize=", "Exclude_size_min"   };
char           *g_iExcludeSizeMax_CMDINI []     =  {"/:maxsize=", "Exclude_size_min"   };
char           *g_bZapBits_CMDINI []            =  {"/zapbits",   "Zap_bits(modify_file_contents)" };
char           *g_bZapNames_CMDINI []           =  {"/zapnames",  "Zap_names(modify_file_names)"   };
char           *g_iMaxFilesPerSamThread_CMDINI [] =  {"/maxfiles=", "Submit_max_files_per_thread" };
char           *g_iMaxSigsPerSigThread_CMDINI []  =  {"/maxsigs=",  "Download_max_signatures_per_thread" };
char           *g_iMaxSQsPerSQThread_CMDINI []        =  {"/maxsqs=", "Max_number_of_status_queries_per_thread" };
char           *g_bLoop_CMDINI []               =  {"/loop",      "Loop"   };
char           *g_bVerbose_CMDINI []            =  {"/verbose",   "Verbose"   };
char           *g_bManualSample_CMDINI []       =  {"/sammanual", "Manual_Sample"   };
char           *g_bSkipDownload_CMDINI []       =  {"/sigskipdownload", "Skip_Download"   };
char           *g_bNoAsk_CMDINI []              =  {"/noask",     "Noask"  };
char           *g_bUseSSL_CMDINI []             =  {"/ssl",       "Use_SSL"  };
//
char           *g_ppIncludeSpecs_CMDINI []      =  {"",           "Include_specs"   };
char           *g_ppExcludeSpecs_CMDINI []      =  {"",           "Exclude_specs"   };


// All this information is enterable from the command line.
///char           g_acRedirectionSearchPattern []  = "HTTP/*3*Moved *ly";  // Search for this in gateway's reply to detect redirection.
char           g_acRedirectionSearchPattern []  = "HTTP/*30";  // Search for this in gateway's reply to detect redirection.

char           g_acGetSumsFrom            [MAX_PATH] = "";
char           g_acSetSumsTo              [MAX_PATH] = "";

char           g_acCurrentProfileName [MAX_PATH]   = "";   // '/profile='.
char           g_acSamplesRootDir [MAX_PATH]       = "";    // No default dir
char           g_acGatewayNameOrIP       [MAX_PATH]= "navajo.watson.ibm.com";   // '/gateway='. navajo.watson.ibm.com. From command line or from .INI file.
char           g_acTargetDirectoryOnGateway [MAX_PATH]="/AVIS/postSuspectSample";
int            g_iGatewayPort                      = 2847;                      // '/port='. 2847. From command line or from .INI file.
int            g_iSamThreadsNum   = 0;                   // '/samthreads='.
int            g_iSamDelay        = 0;                   // ms. of delay between file submissions.
int            g_iSQDelay         = 0;                   // ms. of delay between status queries.
int            g_iSamErrorsMax    = -1;                  // -1:infinite, 0:exit after first critical error, >0:use this number.
int            g_iSQErrorsMax     = -1;                  // -1:infinite, 0:exit after first critical error, >0:use this number.
int            g_iSigThreadsNum   = 0;                   // '/sigthreads='.
int            g_iSQThreadsNum    = 0;                   // '/sqthreads='.
int            g_iSigDelay        = 0;                   // ms. of delay between downloads of signature sets.
int            g_iSigErrorsMax    = -1;                  // -1:infinite, 0:exit after first critical error, >0:use this number.
int            g_iMinDelay        = 0;                   // ms
int            g_iMaxDelay        = 0;                   // ms
int            g_iSeed            = 0;                   // unsigned int. Default = 0.
BOOL           g_bIncludeSubdirs  = FALSE;               // '/s'.
int            g_iIncludeSizeMin  = 0;                   // '/minsize='
int            g_iIncludeSizeMax  = FILESIZE_INFINITE;   // '/maxsize='
int            g_iExcludeSizeMin  = FILESIZE_INFINITE;   // '/:minsize='
int            g_iExcludeSizeMax  = FILESIZE_INFINITE;   // '/:maxsize='
BOOL           g_bZapBits         = FALSE;               // '/zapbits' - randomly change file contents before submitting file.
BOOL           g_bZapNames        = FALSE;               // '/zapnames' - randomly change file name before submitting file.
int            g_iMaxFilesPerSamThread = -1;    // -1:as many as found, else use this number.
int            g_iMaxSigsPerSigThread  = -1;    // -1:download infinitely in a loop, else use this number.
int            g_iMaxSQsPerSQThread  = -1;    // -1:unlimited number of SQs, else use this number.


BOOL           g_bLoop            = FALSE;   // '/loop'. Do sample submission is a loop until key pressed.
BOOL           g_bVerbose         = FALSE;   // '/verbose'. log HTTP transaction data to screen.
BOOL           g_bManualSample    = FALSE;   // '/sammanual'. Set X-Sample-Reason to 'manual'
BOOL           g_bSkipDownload    = FALSE;   // '/sigskipdownload'. Skip signature def download
BOOL           g_bNoAsk           = FALSE;   // '/noask'. Confirmation before beginning tests.
BOOL           g_bUseSSL          = FALSE;   // '/ssl'. Confirmation before beginning tests.
//
char           **g_ppIncludeSpecs = NULL;                // file*.*
int            g_iIncludeSpecsNum = 0;
char           **g_ppExcludeSpecs = NULL;                // :file*.*
int            g_iExcludeSpecsNum = 0;


HANDLE         g_SSL_lock_cs [CRYPTO_NUM_LOCKS];
SSL_CTX        *g_pSslContext     = NULL;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local function prototypes.

unsigned __stdcall   SubmitSample_Thread           (VOID *pvThreadParm);
BOOL  Callback_SubmitSample   (TRANSACT_INFO_STRUCTURE *pTIS);
unsigned __stdcall   DownloadSignatureSet_Thread   (VOID *pvThreadParm);
BOOL  Callback_DownloadSigSet (TRANSACT_INFO_STRUCTURE *pTIS);
unsigned __stdcall   StatusQuery_Thread (VOID *pvThreadParm);
VOID  *ScrambleBuffer (
         VOID     *pvHandle,     // NULL:first time run, else:Scramble handle, returned from prev. call.
         char     *pBuffer,
         DWORD    dwBufferSize
         );
VOID  Free_HTTP_Request       (char *pAllocatedHttpRequest);
BOOL  Testgate_Initialize     (int iArgsNum, char **ppArgs, CONSTANT_INFO *pConstantInfo, Fileset **ppFileset);
VOID  Testgate_Cleanup        (Fileset **ppFileset);
BOOL  AllThreadsTerminated (
         ALL_THREAD_PARAMS   **ppSubmitThreadParms,
         int                           iSubmitThreadParmsNum
         );
DWORD ZapBitsInBuffer (
         DWORD    dwStartingRandomNumber_Or_Handle,
         char     *pBuffer,
         int      iBufferSize
         );
VOID  ZapFilename (
         const char  *pfilename,
         char        *pAlteredFilename
         );
BOOL  GetFileCRCs (
         const char  *pFilename,
         char        *pUseBuffer,         // NULL|ptr to buffer for file I/O. NULL-allocate/free itself.
         DWORD       dwUseBufferSize,     // 0|Size of buffer. 0:allocate itself for the whole file and ignore 'pUseBuffer'.
         BOOL        bZapBits,            // TRUE - randomly zap bits of file.
         DWORD       dwZapKey,            // Random 32 bit number to be used to zap bits.
         char        *pPlainCRC,          // At least 33 bytes. Or zapped pre-scrambled CRC.
         char        *pScrambledCRC,      // At least 33 bytes. Scrambled CRC.
         DWORD       *pdwLastError,
         char        *pLastErrorText,
         DWORD       dwLastErrorTextSize
         );
VOID  Test        (VOID);
char  *ValidateParameters (char **ppArgs, int iArgsNum);
VOID  AddErrorToArray (
         DWORD    ppdwErrorArray[][2],
         DWORD    dwErrorID,
         int      iSizeOfArray   = CRITICAL_ERROR_IDS_MAX,
         DWORD    dwErrorsNumber = 1
         );
VOID     Printf                     (LPCSTR fs, ...);
SSL_CTX  *SSL_Initialize            (VOID);
VOID     SSL_Cleanup                (SSL_CTX *pSslContext);
VOID     SSL_win32_locking_callback (int mode, int type, const char *file, int line);
BOOL     ParseNewLocationData (
            const char     *pLocationStrBuffer,    // input - buffer with data, where "Location" text is.
            int            iLocationStrBufferSize, // input : int - size of above buffer.
            char           *pHostNameOrIP,      // output: must be at least MAX_PATH long.
            unsigned short *pusPortNumber,      // output:
            char           *pDirectoryName,     // output: must be at least MAX_PATH long.
            BOOL           *bUseSSL             // output.
            );
VOID  LoadCheckSumsFromFile (
         const char  *pFilename,
         char        ***pppCheckSumsArray,
         int         *piCheckSumsInArray
         );
VOID  SaveCheckSumsToFile (
         const char  *pFilename,
         char        **ppCheckSumsArray,
         int         iCheckSumsInArray,
         DWORD       dwMaxFileSize = 0xFFFFFFFF // 0xFFFFFFFF:unlimited.
         );
BOOL  ValidateCRCBuffer (const char *pCRCBuffer);
VOID  StoreChecksumToArray (
         const char  *pCheckSum,             // ASCIIZ MD5 checksum to store to array.
         int         iMaxAllowedChecksums,   // Store no more than this number of checksums.
         char        ***pppCheckSums,
         int         *piCheckSumsNum
         );
//---------------------------------------------------------------------------


//===========================================================================
//
// General syntax:
// TESTGATE  source_dir_or_files  inc_spec1 inc_specN :exc_spec1 :exc_specN /parm1=xx /parmN=yy
// first parameter is always directory name or list of files. All others are optional and
// may come in any order.

void  main (int iArgsNum, char **ppArgs)
{
//---------------------------------------------------------------------------
   char              *pWholeArg, *pRestOfArg;
   char              *pTmp;

   WSADATA           wsData;

   Fileset           *pFileset = NULL;
   CONSTANT_INFO     ciConstantInfo;
   FILE_INFO         **ppFileInfos = NULL;       // FILE_INFO   ***pppFileInfos,  // Ptr to an array of pointers to 'FILE_INFO' structure.
   int               iFileInfosNum = 0;          // int         *piFileInfosNum,  // Number of elements in array.

   HANDLE            hTerminatingEvent = INVALID_HANDLE_VALUE;

   ALL_THREAD_PARAMS **ppAllThreadParms = NULL;
   int               iAllThreadParmsNum = 0;
   int               iChar;

   DWORD             dwStartTime, dwEndTime;
   SAM_THREAD_RESULTS  samAllSamThreads;
   SIG_THREAD_RESULTS  sigAllSigThreads;
   SQ_THREAD_RESULTS   sqAllSQThreads;
   char              acDateTimeString [80];
   char              acI64String      [80];

   char              **ppCheckSumsNew = NULL;   // New checksums, coming from SAM threads.
   int               iCheckSumsNewNum = 0;      // New checksums, coming from SAM threads.

   char              **ppSQCheckSums = NULL;
   int               iSQCheckSumsNum = 0;

   BOOL              bPrintingLastHalf;
   char              *pLastErrorText = NULL;
   BOOL              bPrintfHelp = FALSE;
   BOOL              bRetCode;
   int               iRetCode;
   int               i, j;
//---------------------------------------------------------------------------


   memset (g_SSL_lock_cs, 0, sizeof (g_SSL_lock_cs));

   bRetCode =
      IsArgument ( /* BOOL: TRUE:exists, FALSE:not.   */
      ppArgs,           /* char  **ppArgs:      input:  As passed to 'main()'.   */
      iArgsNum,         /* int   iArgsNum:       input:  As passed to 'main()'.  */
      "/nocls",         /* char  *pArg:         input:  "value="        Cannot be NULL.   */
      &pWholeArg,       /* char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.   */
      &pRestOfArg,      /* char  **ppRestArg:   output: "1024".         Cannot be NULL.   */
      TRUE              /* BOOL  bStrict= FALSE: TRUE - exact copy required (no partial). */
      );
   if (!bRetCode)
      clrscr();


   textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY);
   printf (g_acCopyright, AVIS_VERSION, AVIS_BUILD);

///      g_MyProjectVersionInfo.dwVersionMajor,
///      g_MyProjectVersionInfo.dwVersionMinor,
///      g_MyProjectVersionInfo.dwBuildNumber
///      );
   textattr (g_wOrigAttrs);

   if (iArgsNum < 2)
      bPrintfHelp = TRUE;
   bRetCode =
      IsArgument ( /* BOOL: TRUE:exists, FALSE:not.   */
      ppArgs,           /* char  **ppArgs:      input:  As passed to 'main()'.   */
      iArgsNum,         /* int   iArgsNum:       input:  As passed to 'main()'.  */
      "/?",             /* char  *pArg:         input:  "value="        Cannot be NULL.   */
      &pWholeArg,       /* char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.   */
      &pRestOfArg,      /* char  **ppRestArg:   output: "1024".         Cannot be NULL.   */
      FALSE             /* BOOL  bStrict= FALSE: TRUE - exact copy required (no partial). */
      );
   if (bRetCode)
      bPrintfHelp = TRUE;
   bRetCode =
      IsArgument ( /* BOOL: TRUE:exists, FALSE:not.   */
      ppArgs,           /* char  **ppArgs:      input:  As passed to 'main()'.   */
      iArgsNum,         /* int   iArgsNum:       input:  As passed to 'main()'.  */
      "-?",             /* char  *pArg:         input:  "value="        Cannot be NULL.   */
      &pWholeArg,       /* char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.   */
      &pRestOfArg,      /* char  **ppRestArg:   output: "1024".         Cannot be NULL.   */
      FALSE             /* BOOL  bStrict= FALSE: TRUE - exact copy required (no partial). */
      );
   if (bRetCode)
      bPrintfHelp = TRUE;
   bRetCode =
      IsArgument ( /* BOOL: TRUE:exists, FALSE:not.   */
      ppArgs,           /* char  **ppArgs:      input:  As passed to 'main()'.   */
      iArgsNum,         /* int   iArgsNum:       input:  As passed to 'main()'.  */
      "?",             /* char  *pArg:         input:  "value="        Cannot be NULL.   */
      &pWholeArg,       /* char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.   */
      &pRestOfArg,      /* char  **ppRestArg:   output: "1024".         Cannot be NULL.   */
      FALSE             /* BOOL  bStrict= FALSE: TRUE - exact copy required (no partial). */
      );
   if (bRetCode)
      bPrintfHelp = TRUE;

   if (bPrintfHelp)
      {
      textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY);
      printf (g_acSyntax1);
      textattr (g_wOrigAttrs);
      printf (g_acSyntax2);
      textattr (FOREGROUND_GREEN + FOREGROUND_INTENSITY);
      printf (g_acSyntax3);
      textattr (g_wOrigAttrs);
      return;
      }


   pTmp = ValidateParameters (ppArgs, iArgsNum);
   if (pTmp)
      {
      textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
      printf ("\n\nError: invalid parameter: ");
      textattr (BACKGROUND_RED + BACKGROUND_GREEN + BACKGROUND_INTENSITY +
                FOREGROUND_RED + FOREGROUND_INTENSITY);
      printf ("  %s  ", pTmp);

      textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);
      printf ("\n\nUse: LOADGATE /?   for help");
      textattr (g_wOrigAttrs);
      return;
      }


   if (iArgsNum < 2)
      {
      textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);
      printf ("\nUse:   LOADGATE /?   for help");
      textattr (g_wOrigAttrs);
      }

   // Initialize socket library forst.
   //
   iRetCode =
      WSAStartup (   // int. 0-success, error code otherwise.
         MAKEWORD (1, 1),  // WORD wVersionRequested
         &wsData           // LPWSADATA lpWSAData
         );
   if (iRetCode != 0)
      {
      iRetCode = WSAGetLastError();
      textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
      printf ("\nLOADGATE: Cannot initialize socket library.");
      printf ("\nLOADGATE: WSAGetLastError() = %d. Aborting...", iRetCode);
      textattr (g_wOrigAttrs);
      goto  ABORT;
      }


   // Read all CMD line, INI file, initialize all structures and globals.
   bRetCode = Testgate_Initialize (iArgsNum, ppArgs, &ciConstantInfo, &pFileset);
   if (!bRetCode)
      goto  ABORT;   // All error printf-ing already done.


   // Loading checksums from file.
   if (g_acGetSumsFrom[0])
      LoadCheckSumsFromFile (
         g_acGetSumsFrom,  // const char  *pFilename,
         &ppSQCheckSums,   // char        ***pppCheckSumsArray,
         &iSQCheckSumsNum  // int         *piCheckSumsInArray
         );


   if (g_iSamThreadsNum && g_acSamplesRootDir[0])
      {
      printf ("\nLOADGATE: Searching files in: %s. Wait ...", g_acSamplesRootDir);
      bRetCode =
         AllocateFileInfoList (
            g_acSamplesRootDir,  // const char  *pDirName,        // Starting directory name.
            g_iMaxFilesPerSamThread,   // int         iMaxFileToSearch, // -1(unlimited):max number of files to return.
            pFileset,            // Fileset     *pFileset,        // Initialized fileset.
            g_bIncludeSubdirs,   // BOOL        bIncludeSubdirs,  // TRUE:=>Include subdirs in search.
            &ppFileInfos,        // FILE_INFO   ***pppFileInfos,  // Ptr to an array of pointers to 'FILE_INFO' structure.
            &iFileInfosNum,      // int         *piFileInfosNum,  // Number of elements in array.
            &pLastErrorText      // char        **ppLastErrorText /*=NULL*/ // Ptr to the "reason for error" text description.
            );
      if (!bRetCode)
         {
         printf ("\nLOADGATE: %s", pLastErrorText);
         goto  ABORT;
         }
      }

   if (!g_iSamThreadsNum && !g_iSigThreadsNum && !g_iSQThreadsNum)
      {
      textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
      printf ("\nLOADGATE: Nothing to do! (use /samthreads= /sigthreads= or /sqthreads=)");
      textattr (g_wOrigAttrs);
      goto  ABORT;
      }

   if (!iFileInfosNum)
      {
      // Setting number of sample submission threads to 0.
      g_iSamThreadsNum        = 0;
      g_iMaxFilesPerSamThread = 0;

      textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
      if (g_acSamplesRootDir[0])
         printf ("\nLOADGATE: No files found under the tree: %s", g_acSamplesRootDir);
      else
         printf ("\nLOADGATE: No samples directory specified. No samples will be submitted.");
      textattr (g_wOrigAttrs);

      if (!g_iSigThreadsNum && !g_iSQThreadsNum)
         {
         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\nLOADGATE: No Sig- or SQ- threads requested. Nothing to do!");
         textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);
         printf ("\n          Use:  LOADGATE /?   for help.");
         textattr (g_wOrigAttrs);
         goto  ABORT;
         }

      textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
      printf ("\nLOADGATE: No sample submission threads will be running.");
      textattr (g_wOrigAttrs);
      }

   if (iFileInfosNum && g_iSamThreadsNum)
      {
      printf ("\nLOADGATE: Found %d files under the tree: %s.", iFileInfosNum, g_acSamplesRootDir);
      for (bPrintingLastHalf = FALSE, i=0; i<iFileInfosNum; i++)
         {
         printf ("\n   %4d: ", i+1);
         textattr (FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);
         printf ("%s (%d bytes)", ppFileInfos[i]->pFullFilename, ppFileInfos[i]->nFileSizeLow);
         textattr (g_wOrigAttrs);
         if (i>=10 && !bPrintingLastHalf && iFileInfosNum>20)
            {
            printf ("\n ...etc...");
            i = iFileInfosNum - 11;
            bPrintingLastHalf = TRUE;
            }
         }
      }

   if (g_iMinDelay > g_iMaxDelay)
      {
      int   iTmp = g_iMinDelay;
      g_iMinDelay = g_iMaxDelay;
      g_iMaxDelay = iTmp;

      textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
      printf ("\nWarning: 'mindelay' value is greater, than 'maxdelay' value. Switching...");
      textattr (g_wOrigAttrs);
      }

   printf ("\n\nAbout to initiate gateway tests with the following parameters:\n");

   textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);

   printf ("\nGateway : port: ------------------------------- %s : %d",  g_acGatewayNameOrIP, g_iGatewayPort);
   printf ("\nUse SSL? -------------------------------------- %s", g_bUseSSL?"Yes":"No");
   printf ("\nNumber of sample submission threads: ---------- %d", g_iSamThreadsNum);
   printf ("\nNumber of signature set downloader threads: --- %d", g_iSigThreadsNum);
   printf ("\nNumber of status query threads: --------------- %d", g_iSQThreadsNum);
   printf ("\nRead checksums from file: --------------------- %s", g_acGetSumsFrom[0]?g_acGetSumsFrom:"Not specified");
   printf ("\nWrite checksums to file: ---------------------- %s", g_acSetSumsTo[0]?g_acSetSumsTo:"Not specified");

   printf ("\nSamples directory: ---------------------------- %s",  g_acSamplesRootDir[0]?g_acSamplesRootDir:"Not specified");
   printf ("\nInclude subdirectories? ----------------------- %s", g_bIncludeSubdirs?"Yes":"No");
   printf ("\nInclude filespecs: ----------------------------");
   for (i=0; i<pFileset->GetSpecsNum(TRUE); i++)
      printf (" %s", pFileset->GetSpecElement (TRUE, i));
   printf ("\nExclude filespecs: ----------------------------%s", pFileset->GetSpecsNum(FALSE)?"":" None");
   for (i=0; i<pFileset->GetSpecsNum(FALSE); i++)
      printf (" %s", pFileset->GetSpecElement (FALSE, i));
   printf ("\nInclude size MIN: ----------------------------- %d", g_iIncludeSizeMin);
   printf ("\nInclude size MAX: ----------------------------- %d", g_iIncludeSizeMax);
   printf ("\nExclude size MIN: ----------------------------- %d", g_iExcludeSizeMin);
   printf ("\nExclude size MAX: ----------------------------- %d", g_iExcludeSizeMax);
   printf ("\nDelay between submitting of samples(ms): ------ %d", g_iSamDelay);
   printf ("\nDelay between status queries(ms): ------------- %d", g_iSQDelay);
   printf ("\nMAX number of critical errors per sam. thread - %d%s", g_iSamErrorsMax, (g_iSamErrorsMax==-1)?"(unlimited)":"");
   printf ("\nMAX number of critical errors per SQ thread --- %d%s", g_iSQErrorsMax, (g_iSQErrorsMax==-1)?"(unlimited)":"");

   printf ("\nMAX number of files per submission thread ----- %s", (g_iMaxFilesPerSamThread==-1)?"Unlimited":"");
   if (g_iMaxFilesPerSamThread >= 0) printf ("%d", g_iMaxFilesPerSamThread);

   printf ("\nNumber of times to download signature set ----- %s", (g_iMaxSigsPerSigThread==-1)?"Unlimited":"");
   if (g_iMaxSigsPerSigThread >= 0) printf ("%d", g_iMaxSigsPerSigThread);

   printf ("\nMax number of status queries per SQ thread ---- %s", (g_iMaxSQsPerSQThread==-1)?"Unlimited":"");
   if (g_iMaxSQsPerSQThread >= 0) printf ("%d", g_iMaxSQsPerSQThread);

   printf ("\nModify data buffer? --------------------------- %s", g_bZapBits?"Yes":"No");
   printf ("\nSubmit modified file names? ------------------- %s", g_bZapNames?"Yes":"No");
   printf ("\nSet X-Sample-Reason to 'manual'?--------------- %s", g_bManualSample?"Yes":"No");
   printf ("\nSkip download of signature definitions?-------- %s", g_bSkipDownload?"Yes":"No");
   printf ("\nDelay after every downloaded set(ms): --------- %d", g_iSigDelay);
   printf ("\nMAX number of critical errors per sig. thread - %d%s", g_iSigErrorsMax, (g_iSigErrorsMax==-1)?"(unlimited)":"");
   printf ("\nMIN random delay (ms) ------------------------- %d", g_iMinDelay);
   printf ("\nMAX random delay (ms) ------------------------- %d", g_iMaxDelay);
   printf ("\nRandom generator's seed value: ---------------- %d", g_iSeed);
   printf ("\nSubmit samples infinitely (until <ESC>)? ------ %s", g_bLoop?"Yes":"No");
   printf ("\nVerbose messages? ----------------------------- %s", g_bVerbose?"Yes":"No");
   printf ("\nSettings saved in profile: -------------------- %s", g_acCurrentProfileName[0]?g_acCurrentProfileName:"Not specified");
   textattr (g_wOrigAttrs);

   if (!g_bNoAsk)
      {
      textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
      printf ("\n\nProceed with tests (Y/N)? ");
      textattr (g_wOrigAttrs);
      Beep (200, 50);
      if (toupper(getch()) != 'Y')
         {
         printf ("N");
         printf ("\nLOADGATE: Aborted per user's request");
         textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);
         printf ("\n\nUse:   LOADGATE /?   for help");
         textattr (g_wOrigAttrs);
         goto  ABORT;
         }

      printf ("Y");
      }


// typedef struct _FILE_INFO
// {
//    char        *pFullFilename;   // Dynamically allocated.
//
//    // These are copied from 'WIN32_FIND_DATA' structure.
//    DWORD       dwFileAttributes;
//    FILETIME    ftCreationTime;
//    FILETIME    ftLastAccessTime;
//    FILETIME    ftLastWriteTime;
//    DWORD       nFileSizeHigh;
//    DWORD       nFileSizeLow;
// } FILE_INFO;


   CurrentDateTimeToString2 (acDateTimeString);
   Printf ("\n\n\nLOADGATE Started at: %s", acDateTimeString);
   dwStartTime = StartTimer();

   // Initialize SSL library.
   if (g_bUseSSL)
      g_pSslContext = SSL_Initialize ();


   hTerminatingEvent =
      CreateEvent ( // HANDLE: NULL:fail, else:handle of event object.
         NULL,    // LPSECURITY_ATTRIBUTES lpEventAttributes: pointer to security attributes
         TRUE,    // BOOL bManualReset:  TRUE:manual-reset event.
         FALSE,   // BOOL bInitialState: TRUE:initially signalled, FALSE:non-signalled.
         NULL     // LPCTSTR lpName:  pointer to event-object name
         );

// typedef struct _ALL_THREAD_PARAMS
// {
//    HANDLE               hThread;
//    unsigned             uThreadID;                    // GetCurrentThreadId();
//    DWORD                dwThreadType;                 // THREAD_TYPE_*
//
//    char                 acGatewayNameOrIP [MAX_PATH]; // '/gateway='. navajo.watson.ibm.com. From command line or from .INI file.
//    unsigned short       usGatewayPort;                // '/port='. 2847. From command line or from .INI file.
//    BOOL                 bUseSSL;                      // '/ssl'.
//    SSL_CTX              *pSslContext;                 // Used for SSL connections.
//    BOOL                 bZapBits;                     // '/zapbits' - randomly change file contents before submitting file.
//    BOOL                 bZapNames;                    // '/zapnames' - randomly change file name before submitting file.
//    int                  iMaxFilesPerSamThread;        // -1:as many as found, else use this number.
//    int                  iMaxSigsPerSigThread;         // -1:infinitely in a loop, else use this number.
//    int                  iSamDelay;                    // ms. of delay between file submissions or between retries after error occured.
//    int                  iSamErrorsMax;                // Allow this number of critical errors per Sam thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
//    BOOL                 bLoop;                        // '/loop'. Do sample submission is a loop until key pressed.
//    BOOL                 bVerbose;                     // '/verbose'. Log HTTP transaction data to screen.
//    BOOL                 bManualSample;                // '/sammanual'. Set X-Sample-Reason to 'manual'
//    BOOL                 bSkipDownload;                // '/sigskipdownload'. Skip signature def download
//    int                  iFileInfosNum;                // int         iFileInfosNum: Number of elements in array.
//
//    CONSTANT_INFO        *pConstantInfo;               // Needed for 'Allocate_HTTP_Header_For_SubmitSample()'.
//
//    int                  iSigDelay;                    // ms. of delay between downloads of blessed signature sets or between retries after error occured.
//    int                  iSigErrorsMax;                // Allow this number of critical errors per Sig thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
//
//    int                  iMinDelay;
//    int                  iMaxDelay;
//    int                  iSeed;
//
//    SAM_THREAD_RESULTS   SamResults;
//    SIG_THREAD_RESULTS   SigResults;
//
//    HANDLE               hEventTerminating;            // Will be set into signalled state if all termination needed.
// } ALL_THREAD_PARAMS;

// typedef struct _SAM_THREAD_RESULTS
// {
//    DWORD                dwUploadedFiles;           // Successfully uploaded files.
//    unsigned __int64     iUploadedBytes;            // Total number of successfully uploaded bytes.
//    DWORD                dwFileIoErrors;
//    DWORD                dwTimeouts;
//    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
// } SAM_THREAD_RESULTS;

   // Spawn submission samples worker threads...
   for (i=0; i<g_iSamThreadsNum; i++)
      {
      ALL_THREAD_PARAMS   *pTP;
      pTP = new ALL_THREAD_PARAMS;
      memset (pTP, 0, sizeof (*pTP));
      pTP->hThread =
         (HANDLE) _beginthreadex (     // unsigned long - must be typecasted to HANDLE. Error=>0|-1
            NULL,                      // void *security: Security descriptor for new thread | NULL.
            0,                         // unsigned stack_size: Stack size for new thread or 0(same size as main).
            SubmitSample_Thread,       // unsigned (* start_address )(void *): Start address of thread routine.
            (void *)pTP,               // void *arglist: Argument list to be passed to new thread or NULL.
            CREATE_SUSPENDED,          // unsigned initflag: Initial state of new thread (0-running or CREATE_SUSPENDED(?)-suspended).
            &pTP->uThreadID            // unsigned *thrdaddr: Address of new thread ID. ID is assigned by system.
            );
      pTP->dwThreadType       = THREAD_TYPE_SAM;
      strcpy (pTP->acGatewayNameOrIP, g_acGatewayNameOrIP);
      pTP->usGatewayPort      = (unsigned short)g_iGatewayPort;
      pTP->bUseSSL            = g_bUseSSL;
      pTP->pSslContext        = g_pSslContext;
      pTP->bZapBits           = g_bZapBits;
      pTP->bZapNames          = g_bZapNames;
      pTP->iMaxFilesPerSamThread = g_iMaxFilesPerSamThread;
      pTP->iMaxSigsPerSigThread  = g_iMaxSigsPerSigThread;
      pTP->iSamDelay          = g_iSamDelay;
      pTP->iSamErrorsMax      = g_iSamErrorsMax;

      pTP->pppCheckSumsNew    = &ppCheckSumsNew  ;
      pTP->piCheckSumsNewNum  = &iCheckSumsNewNum;
      pTP->iMaxCheckSumsNew   = 1000;

      pTP->bLoop              = g_bLoop;
      pTP->bVerbose           = g_bVerbose;
      pTP->bManualSample      = g_bManualSample;
      pTP->bSkipDownload      = g_bSkipDownload;
      pTP->ppFileInfos        = ppFileInfos;
      pTP->iFileInfosNum      = iFileInfosNum;
      pTP->pConstantInfo      = &ciConstantInfo;
      pTP->iSigDelay          = g_iSigDelay;
      pTP->iSigErrorsMax      = g_iSigErrorsMax;

      pTP->iMaxSQsPerSQThread = g_iMaxSQsPerSQThread;
      pTP->ppSQCheckSums      = NULL;
      pTP->iSQCheckSumsNum    = 0;
      pTP->iSQDelay           = g_iSQDelay;
      pTP->iSQErrorsMax       = g_iSQErrorsMax;

      pTP->iMinDelay          = g_iMinDelay;
      pTP->iMaxDelay          = g_iMaxDelay;
      pTP->iSeed              = g_iSeed + i*71;

      pTP->SamResults.dwCriticalErrors [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.
      pTP->SigResults.dwCriticalErrors [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.
      pTP->SQResults.dwCriticalErrors  [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.

      pTP->hEventTerminating  = hTerminatingEvent;

      AddPtrToArray ((void ***)&ppAllThreadParms, &iAllThreadParmsNum, (const void *)pTP);

      // Resume thread now.
      ResumeThread (pTP->hThread);
      }


   // Spawn download signature worker threads...
   for (i=0; i<g_iSigThreadsNum; i++)
      {
      ALL_THREAD_PARAMS   *pTP;
      pTP = new ALL_THREAD_PARAMS;
      memset (pTP, 0, sizeof (*pTP));
      pTP->hThread =
         (HANDLE) _beginthreadex (     // unsigned long - must be typecasted to HANDLE. Error=>0|-1
            NULL,                      // void *security: Security descriptor for new thread | NULL.
            0,                         // unsigned stack_size: Stack size for new thread or 0(same size as main).
            DownloadSignatureSet_Thread, // unsigned (* start_address )(void *): Start address of thread routine.
            (void *)pTP,               // void *arglist: Argument list to be passed to new thread or NULL.
            CREATE_SUSPENDED,          // unsigned initflag: Initial state of new thread (0-running or CREATE_SUSPENDED(?)-suspended).
            &pTP->uThreadID            // unsigned *thrdaddr: Address of new thread ID. ID is assigned by system.
            );
      pTP->dwThreadType       = THREAD_TYPE_SIG;
      strcpy (pTP->acGatewayNameOrIP, g_acGatewayNameOrIP);
      pTP->usGatewayPort      = (unsigned short)g_iGatewayPort;
      pTP->bUseSSL            = g_bUseSSL;
      pTP->pSslContext        = g_pSslContext;
      pTP->bZapBits           = g_bZapBits;
      pTP->bZapNames          = g_bZapNames;
      pTP->iMaxFilesPerSamThread = g_iMaxFilesPerSamThread;
      pTP->iMaxSigsPerSigThread  = g_iMaxSigsPerSigThread;
      pTP->iSamDelay          = g_iSamDelay;
      pTP->iSamErrorsMax      = g_iSamErrorsMax;
      pTP->bLoop              = g_bLoop;
      pTP->bVerbose           = g_bVerbose;
      pTP->bManualSample      = g_bManualSample;
      pTP->bSkipDownload      = g_bSkipDownload;
      pTP->ppFileInfos        = ppFileInfos;
      pTP->iFileInfosNum      = iFileInfosNum;
      pTP->pConstantInfo      = &ciConstantInfo;
      pTP->iSigDelay          = g_iSigDelay;
      pTP->iSigErrorsMax      = g_iSigErrorsMax;

      pTP->pppCheckSumsNew    = &ppCheckSumsNew  ;
      pTP->piCheckSumsNewNum  = &iCheckSumsNewNum;
      pTP->iMaxCheckSumsNew   = 1000;

      pTP->iMaxSQsPerSQThread = g_iMaxSQsPerSQThread;
      pTP->ppSQCheckSums      = NULL;
      pTP->iSQCheckSumsNum    = 0;
      pTP->iSQDelay           = g_iSQDelay;
      pTP->iSQErrorsMax       = g_iSQErrorsMax;


      pTP->iMinDelay          = g_iMinDelay;
      pTP->iMaxDelay          = g_iMaxDelay;
      pTP->iSeed              = g_iSeed + i*71;

      pTP->SamResults.dwCriticalErrors [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.
      pTP->SigResults.dwCriticalErrors [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.
      pTP->SQResults.dwCriticalErrors  [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.

      pTP->hEventTerminating  = hTerminatingEvent;

      AddPtrToArray ((void ***)&ppAllThreadParms, &iAllThreadParmsNum, (const void *)pTP);

      // Resume thread now.
      ResumeThread (pTP->hThread);
      }


   // Spawn status query worker threads...
   for (i=0; i<g_iSQThreadsNum; i++)
      {
      ALL_THREAD_PARAMS   *pTP;
      pTP = new ALL_THREAD_PARAMS;
      memset (pTP, 0, sizeof (*pTP));
      pTP->hThread =
         (HANDLE) _beginthreadex (     // unsigned long - must be typecasted to HANDLE. Error=>0|-1
            NULL,                      // void *security: Security descriptor for new thread | NULL.
            0,                         // unsigned stack_size: Stack size for new thread or 0(same size as main).
            StatusQuery_Thread,        // unsigned (* start_address )(void *): Start address of thread routine.
            (void *)pTP,               // void *arglist: Argument list to be passed to new thread or NULL.
            CREATE_SUSPENDED,          // unsigned initflag: Initial state of new thread (0-running or CREATE_SUSPENDED(?)-suspended).
            &pTP->uThreadID            // unsigned *thrdaddr: Address of new thread ID. ID is assigned by system.
            );
      pTP->dwThreadType       = THREAD_TYPE_SQ;
      strcpy (pTP->acGatewayNameOrIP, g_acGatewayNameOrIP);
      pTP->usGatewayPort      = (unsigned short)g_iGatewayPort;
      pTP->bUseSSL            = g_bUseSSL;
      pTP->pSslContext        = g_pSslContext;
      pTP->bZapBits           = g_bZapBits;
      pTP->bZapNames          = g_bZapNames;
      pTP->iMaxFilesPerSamThread = g_iMaxFilesPerSamThread;
      pTP->iMaxSigsPerSigThread  = g_iMaxSigsPerSigThread;
      pTP->iSamDelay          = g_iSamDelay;
      pTP->iSamErrorsMax      = g_iSamErrorsMax;
      pTP->bLoop              = g_bLoop;
      pTP->bVerbose           = g_bVerbose;
      pTP->bManualSample      = g_bManualSample;
      pTP->bSkipDownload      = g_bSkipDownload;
      pTP->ppFileInfos        = ppFileInfos;
      pTP->iFileInfosNum      = iFileInfosNum;
      pTP->pConstantInfo      = &ciConstantInfo;
      pTP->iSigDelay          = g_iSigDelay;
      pTP->iSigErrorsMax      = g_iSigErrorsMax;

      pTP->pppCheckSumsNew    = &ppCheckSumsNew  ;
      pTP->piCheckSumsNewNum  = &iCheckSumsNewNum;
      pTP->iMaxCheckSumsNew   = 1000;

      pTP->iMaxSQsPerSQThread = g_iMaxSQsPerSQThread;
      pTP->iSQDelay           = g_iSQDelay;
      pTP->iSQErrorsMax       = g_iSQErrorsMax;
      pTP->ppSQCheckSums      = ppSQCheckSums  ;
      pTP->iSQCheckSumsNum    = iSQCheckSumsNum;

      pTP->iMinDelay          = g_iMinDelay;
      pTP->iMaxDelay          = g_iMaxDelay;
      pTP->iSeed              = g_iSeed + i*71;

      pTP->SamResults.dwCriticalErrors [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.
      pTP->SigResults.dwCriticalErrors [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.
      pTP->SQResults.dwCriticalErrors  [CRITICAL_ERROR_IDS_MAX - 1][0] = (DWORD)(-1);  // All other errors has ID = -1.

      pTP->hEventTerminating  = hTerminatingEvent;

      AddPtrToArray ((void ***)&ppAllThreadParms, &iAllThreadParmsNum, (const void *)pTP);

      // Resume thread now.
      ResumeThread (pTP->hThread);
      }


   textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
   printf ("\nPress <ESC> to exit... ");
   textattr (g_wOrigAttrs);

   // Wait for user input to terminate.
   iChar = 0;
   do
      {
      if (AllThreadsTerminated (ppAllThreadParms, iAllThreadParmsNum))
         break;
      if (kbhit())
         {
         iChar = getch();
         if (iChar == 0x1B)
            break;
         else
            {
            textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
            printf ("\nPress <ESC> to exit... ");
            textattr (g_wOrigAttrs);
            Beep (200, 50);
            }
         }
      // Check termination condition once a second.
      Sleep (1000);
      }
#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while (1);
#pragma  warning (default : 4127)

   // Signal everyone to terminate.
   textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
   if (iChar == 0x1B)
      printf ("\n*****  <ESC> Pressed! Stopping LOADGATE. Please wait...  *****");
   else
      printf ("\n*****  All threads terminated. Stopping LOADGATE. Please wait...  *****");
   textattr (g_wOrigAttrs);

   SetEvent (hTerminatingEvent);

   // Give all threads 30 seconds to terminate. After that start killing them.
   for (i=0; i<60; i++)
      {
      Sleep (500);
      if (AllThreadsTerminated (ppAllThreadParms, iAllThreadParmsNum))
         break;
      }

   for (i=0; i<iAllThreadParmsNum; i++)
      {
      if (WaitForSingleObject (ppAllThreadParms[i]->hThread, 0) == WAIT_TIMEOUT)
         {
         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\nLOADGATE: *** Warning *** Thread [%04X] refused to terminate. Killing...", ppAllThreadParms[i]->uThreadID);
         textattr (g_wOrigAttrs);
         Beep (200, 50);
         TerminateThread (ppAllThreadParms[i]->hThread, 0);
         }
      CloseHandle (ppAllThreadParms[i]->hThread);
      }


// Saving new checksums in the file, if requested.
   if (g_acSetSumsTo [0])
      SaveCheckSumsToFile (
         g_acSetSumsTo,    // const char  *pFilename,
         ppCheckSumsNew,   // char        **ppCheckSumsArray,
         iCheckSumsNewNum, // int         iCheckSumsInArray,
         50000             // DWORD       dwMaxFileSize /* = 0xFFFFFFFF */ // 0xFFFFFFFF:unlimited.
         );

// Report results.
   memset (&samAllSamThreads, 0, sizeof (samAllSamThreads));
   memset (&sigAllSigThreads, 0, sizeof (sigAllSigThreads));
   memset (&sqAllSQThreads,   0, sizeof (sqAllSQThreads));

// typedef struct _SQ_THREAD_RESULTS
// {
//    DWORD                dwRequestsSubmitted;
//    DWORD                dwRepliesReceived;
//    DWORD                dwTimeouts;
//    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
// } SQ_THREAD_RESULTS;

   for (i=0; i<iAllThreadParmsNum; i++)
      {
      if (ppAllThreadParms[i]->dwThreadType == THREAD_TYPE_SAM)
         {
         // SAM thread
         samAllSamThreads.dwUploadedFiles += ppAllThreadParms[i]->SamResults.dwUploadedFiles;
         samAllSamThreads.iUploadedBytes  += ppAllThreadParms[i]->SamResults.iUploadedBytes;
         samAllSamThreads.dwFileIoErrors  += ppAllThreadParms[i]->SamResults.dwFileIoErrors;
         samAllSamThreads.dwTimeouts      += ppAllThreadParms[i]->SamResults.dwTimeouts;
         for (j=0; j<CRITICAL_ERROR_IDS_MAX; j++)
            AddErrorToArray (
               samAllSamThreads.dwCriticalErrors,  // DWORD    ppdwErrorArray[][2],
               ppAllThreadParms[i]->SamResults.dwCriticalErrors[j][0],  // DWORD    dwErrorID,
               CRITICAL_ERROR_IDS_MAX,                                  // int      iSizeOfArray   /* = CRITICAL_ERROR_IDS_MAX */,
               ppAllThreadParms[i]->SamResults.dwCriticalErrors[j][1]   // DWORD    dwErrorsNumber /* = 1 */
               );
         }
      else if (ppAllThreadParms[i]->dwThreadType == THREAD_TYPE_SIG)
         {
         // SIG thread.
         sigAllSigThreads.dwDownloadedSignatureSets += ppAllThreadParms[i]->SigResults.dwDownloadedSignatureSets;
         sigAllSigThreads.iDownloadedBytes          += ppAllThreadParms[i]->SigResults.iDownloadedBytes;
         sigAllSigThreads.dwTimeouts                += ppAllThreadParms[i]->SigResults.dwTimeouts;
         sigAllSigThreads.dwErraticReplies          += ppAllThreadParms[i]->SigResults.dwErraticReplies;
         for (j=0; j<CRITICAL_ERROR_IDS_MAX; j++)
            AddErrorToArray (
               sigAllSigThreads.dwCriticalErrors,  // DWORD    ppdwErrorArray[][2],
               ppAllThreadParms[i]->SigResults.dwCriticalErrors[j][0],  // DWORD    dwErrorID,
               CRITICAL_ERROR_IDS_MAX,                                  // int      iSizeOfArray   /* = CRITICAL_ERROR_IDS_MAX */,
               ppAllThreadParms[i]->SigResults.dwCriticalErrors[j][1]   // DWORD    dwErrorsNumber /* = 1 */
               );
         }
      else
         {
         // SQ thread.
         sqAllSQThreads.dwRequestsSubmitted         += ppAllThreadParms[i]->SQResults.dwRequestsSubmitted;
         sqAllSQThreads.dwRepliesReceived           += ppAllThreadParms[i]->SQResults.dwRepliesReceived  ;
         sqAllSQThreads.dwTimeouts                  += ppAllThreadParms[i]->SQResults.dwTimeouts         ;
         for (j=0; j<CRITICAL_ERROR_IDS_MAX; j++)
            AddErrorToArray (
               sqAllSQThreads.dwCriticalErrors,  // DWORD    ppdwErrorArray[][2],
               ppAllThreadParms[i]->SQResults.dwCriticalErrors[j][0],  // DWORD    dwErrorID,
               CRITICAL_ERROR_IDS_MAX,                                  // int      iSizeOfArray   /* = CRITICAL_ERROR_IDS_MAX */,
               ppAllThreadParms[i]->SQResults.dwCriticalErrors[j][1]   // DWORD    dwErrorsNumber /* = 1 */
               );
         }
      }


   textattr (FOREGROUND_GREEN + FOREGROUND_RED + FOREGROUND_INTENSITY);
   dwEndTime = QueryTimer (dwStartTime);
   CurrentDateTimeToString2 (acDateTimeString);
   Printf ("\n\n*****************************************************************************");
   Printf ("\nLOADGATE Ended at: %s (was running for %02d:%02d:%02d.%03d)",
               acDateTimeString,
               dwEndTime/3600000,
               (dwEndTime%3600000)/60000,
               ((dwEndTime%3600000)%60000)/1000,
               ((dwEndTime%3600000)%60000)%1000
               );


   textattr (FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);
   if (g_iSamThreadsNum)
      {
      Printf ("\n\n*** Combined results for all sample uploading threads ***");
      Printf ("\n    Total number of sample uploading threads spawned: -------------- %d", g_iSamThreadsNum);
      Printf ("\n    Total number of files uploaded: -------------------------------- %d", samAllSamThreads.dwUploadedFiles);
///!!!      Printf ("\n    Total number of bytes of file data uploaded: ------------------- %I64d", samAllSamThreads.iUploadedBytes);
      Printf ("\n    Total number of bytes of file data uploaded: ------------------- %s", _i64toa (samAllSamThreads.iUploadedBytes, acI64String, 10));
      Printf ("\n    Total number of file I/O errors occured: ----------------------- %d", samAllSamThreads.dwFileIoErrors);
      Printf ("\n    Total number of timeouts waiting for a gateway's reply: -------- %d", samAllSamThreads.dwTimeouts);
      for (i=0; i<CRITICAL_ERROR_IDS_MAX; i++)
         if ((samAllSamThreads.dwCriticalErrors [i][0] > 0) && (samAllSamThreads.dwCriticalErrors [i][0] != 0xFFFFFFFF))
            Printf ("\n    Total number of critical errors (%d): -------------------------- %d",
                     samAllSamThreads.dwCriticalErrors [i][0],
                     samAllSamThreads.dwCriticalErrors [i][1]
                     );
         else if (i == (CRITICAL_ERROR_IDS_MAX - 1))
            Printf ("\n    Total number of all other critical errors: --------------------- %d", samAllSamThreads.dwCriticalErrors [i][1]);
      }

   if (g_iSigThreadsNum)
      {
      Printf ("\n\n*** Combined results for all signature set downloader threads ***");
      Printf ("\n    Total number of signature downloader threads spawned: ---------- %d",    g_iSigThreadsNum);
      Printf ("\n    Total number of signatures downloaded: ------------------------- %d",    sigAllSigThreads.dwDownloadedSignatureSets);
///!!!      Printf ("\n    Total number of bytes of signature sets downloaded: ------------ %I64d", sigAllSigThreads.iDownloadedBytes);
      Printf ("\n    Total number of bytes of signature sets downloaded: ------------ %s", _i64toa (sigAllSigThreads.iDownloadedBytes, acI64String, 10));
      Printf ("\n    Total number of timeouts waiting for a gateway's reply: -------- %d", sigAllSigThreads.dwTimeouts);
      Printf ("\n    Total number of erratic or unrecognizable gateway's replies: --- %d", sigAllSigThreads.dwErraticReplies);
      for (i=0; i<CRITICAL_ERROR_IDS_MAX; i++)
         if ((sigAllSigThreads.dwCriticalErrors [i][0] > 0) && (sigAllSigThreads.dwCriticalErrors [i][0] != 0xFFFFFFFF))
            Printf ("\n    Total number of critical errors (%d): -------------------------- %d",
                     sigAllSigThreads.dwCriticalErrors [i][0],
                     sigAllSigThreads.dwCriticalErrors [i][1]
                     );
         else if (i == (CRITICAL_ERROR_IDS_MAX - 1))
            Printf ("\n    Total number of all other critical errors: --------------------- %d", sigAllSigThreads.dwCriticalErrors [i][1]);
      }

   if (g_iSQThreadsNum)
      {
      Printf ("\n\n*** Combined results for all status query threads ***");
      Printf ("\n    Total number of status query threads spawned: ------------------ %d",    g_iSQThreadsNum);
      Printf ("\n    Total number of status query requests submitted: --------------- %d",    sqAllSQThreads.dwRequestsSubmitted);
      Printf ("\n    Total number of status query replies received: ----------------- %d",    sqAllSQThreads.dwRepliesReceived);
      Printf ("\n    Total number of timeouts waiting for a gateway's reply: -------- %d",    sqAllSQThreads.dwTimeouts);
      Printf ("\n    Total number of erratic or unrecognizable gateway's replies: --- %d", sigAllSigThreads.dwErraticReplies);
      for (i=0; i<CRITICAL_ERROR_IDS_MAX; i++)
         if ((sqAllSQThreads.dwCriticalErrors [i][0] > 0) && (sqAllSQThreads.dwCriticalErrors [i][0] != 0xFFFFFFFF))
            Printf ("\n    Total number of critical errors (%d): -------------------------- %d",
                     sqAllSQThreads.dwCriticalErrors [i][0],
                     sqAllSQThreads.dwCriticalErrors [i][1]
                     );
         else if (i == (CRITICAL_ERROR_IDS_MAX - 1))
            Printf ("\n    Total number of all other critical errors: --------------------- %d", sqAllSQThreads.dwCriticalErrors [i][1]);
      }


   textattr (FOREGROUND_GREEN + FOREGROUND_RED + FOREGROUND_INTENSITY);
   Printf ("\n*****************************************************************************");
   textattr (g_wOrigAttrs);

// typedef struct _SAM_THREAD_RESULTS
// {
//    DWORD                dwUploadedFiles;           // Successfully uploaded files.
//    unsigned __int64     iUploadedBytes;            // Total number of successfully uploaded bytes of file (without HTTP headers).
//    DWORD                dwFileIoErrors;
//    DWORD                dwTimeouts;
//    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
// } SAM_THREAD_RESULTS;
//
// typedef struct _SIG_THREAD_RESULTS
// {
//    DWORD                dwDownloadedSignatureSets; // Successfully downloaded signature sets.
//    unsigned __int64     iDownloadedBytes;          // Total number of successfully downloaded bytes.
//    DWORD                dwTimeouts;
//    DWORD                dwErraticReplies;          // Any reply, that did not contain expected data.
//    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
// } SIG_THREAD_RESULTS;
//
// typedef struct _SQ_THREAD_RESULTS
// {
//    DWORD                dwRequestsSubmitted;
//    DWORD                dwRepliesReceived;
//    DWORD                dwTimeouts;
//    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
// } SQ_THREAD_RESULTS;




EXIT:
   if (g_bUseSSL)
      {
      SSL_Cleanup (g_pSslContext);
      g_pSslContext = NULL;
      }

   for (i=0; i<iSQCheckSumsNum; i++)
      delete [] ppSQCheckSums[i];
   delete [] ppSQCheckSums;
   ppSQCheckSums   = NULL;
   iSQCheckSumsNum = 0;

   // Deallcoate new checksums array.
   for (i=0; i<iCheckSumsNewNum; i++)
      delete [] ppCheckSumsNew[i];
   delete [] ppCheckSumsNew;
   ppCheckSumsNew   = NULL;
   iCheckSumsNewNum = 0;

   for (i=0; i<iAllThreadParmsNum; i++)
      delete ppAllThreadParms [i];
   FreeArrayOfPtrs ((void ***)&ppAllThreadParms, &iAllThreadParmsNum);

   CloseHandle (hTerminatingEvent);

   FreeFileInfoList (
      &ppFileInfos,     // FILE_INFO   ***pppFileInfos,  // Ptr to an array of pointers to 'FILE_INFO' structure.
      &iFileInfosNum    // int         *piFileInfosNum,  // Number of elements in array.
      );

   WSACleanup();

   Testgate_Cleanup (&pFileset);

   textattr (FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY);
   printf ("\n\n                              LOADGATE: Done!");
   textattr (g_wOrigAttrs);
   return;


ABORT:
   goto  EXIT;

}
//===========================================================================

//===========================================================================
//
// Thread will:
// -  Get next files.
// -  Prepares file's data and CRC-s.
// -  Create ClientSocket and initiate connection to gateway.
// -  Assemble HTTP request. Sent it to gateway.
// -  Wait for the reply and print it.

#define  FILE_IO_ERRORS_LIMIT    10
#define  GATEWAY_ACK_TIMEOUT     1000  // Poll every 1 second.
#define  GATEWAY_TOTAL_TIMEOUT   (15*60*1000) // Wait 15 minutes for gateway to process request.

#define  FILE_BUFFER_SIZE        (32*1024)   // 32K per file buffer.

unsigned __stdcall   SubmitSample_Thread (VOID *pvThreadParm)
{
//---------------------------------------------------------------------------
   ALL_THREAD_PARAMS   *pTP = (ALL_THREAD_PARAMS *)pvThreadParm;
   int                  iFilesNum;

   TRANSACT_INFO_STRUCTURE  tisStructure;

   int                  i;
   HANDLE               hFile          = INVALID_HANDLE_VALUE;
   DWORD                dwFileSize;
   DWORD                dwLastError;
   char                 acFilenameToSubmit [MAX_PATH];
   DWORD                dwTimerValue;
   char                 acErrorText [256];

   WIN32_FIND_DATA      w32FindData;

   char                 acPlainCRC   [40];
   char                 acCryptedCRC [40];

   char                 *pHttpRequest  = NULL;
   int                  iHttpRequestSize;
   int                  iFileIoErrors = 0;
   UPLOAD_SAMPLE_CALLBACK_DATA   usCallbackData;
   DWORD                dwZapKey;

   // We will use these values as working values, to accomodate cases where redirection
   // happens.
   BOOL                 bRedirected;

   char                 acTargetNameOrIP [MAX_PATH];           // Machine name or IP.
   char                 acTargetDirectoryOnGateway [MAX_PATH]; // "/AVIS/postSuspectSample" or "AVIS/postSuspectSample"(will be prepended with '/' then).
   unsigned short       usTargetPort;
   BOOL                 bUseSSL;
   char                 *pTmpPtr;


   DWORD                dwCriticalErrors = 0;
   BOOL                 bBreakCondition = FALSE;

   DWORD                dwRetCode;
   BOOL                 bRetCode;
//---------------------------------------------------------------------------

   // Calculate number of files for the cycle.
   if (pTP->iMaxFilesPerSamThread == -1)
      iFilesNum = pTP->iFileInfosNum;
   else
      {
      if (pTP->iMaxFilesPerSamThread < pTP->iFileInfosNum)
         iFilesNum = pTP->iMaxFilesPerSamThread;
      else
         iFilesNum = pTP->iFileInfosNum;
      }

// // Structure keeps information about file.
// typedef struct _FILE_INFO
// {
//    char        *pFullFilename;   // Dynamically allocated.
//
//    // These are copied from 'WIN32_FIND_DATA' structure.
//    DWORD       dwFileAttributes;
//    FILETIME    ftCreationTime;
//    FILETIME    ftLastAccessTime;
//    FILETIME    ftLastWriteTime;
//    DWORD       nFileSizeHigh;
//    DWORD       nFileSizeLow;
// } FILE_INFO;

   dwCriticalErrors = 0;

   // Initiate random number generator with the requested seed value.
   GetRandomNumber   (0, 1, pTP->iSeed);

   // Start file loop.
   for (i=0; i<iFilesNum; i++)
      {
      // Check for terminating conditions at the beginning of each loop.
      if (WaitForSingleObject (pTP->hEventTerminating, 0) != WAIT_TIMEOUT)
         {
         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\n[%04X]U: Terminating per user's request", pTP->uThreadID);
         textattr (g_wOrigAttrs);
         break;
         }

      if (pTP->ppFileInfos[i]->nFileSizeHigh)
         {
         // Skip this file. Too large!
         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\n[%04X]U: Skipping file: %s. Reason: too large!", pTP->uThreadID, pTP->ppFileInfos[i]->pFullFilename);
         textattr (g_wOrigAttrs);
         continue;
         }

      // Execute random delay, if requested.
      if (pTP->iMinDelay || pTP->iMaxDelay)
         {
         int   iDelay = GetRandomNumber (pTP->iMinDelay, pTP->iMaxDelay);
         printf ("\n[%04X]U: Sleeping for %d ms...", pTP->uThreadID, iDelay);
         Sleep (iDelay);
         }

      // Calculate zap key once per file.
      dwZapKey = GetRandomDword ();

      printf ("\n[%04X]U: Calculating file checksums. (%s [%d bytes]). Wait ...",
                  pTP->uThreadID,
                  pTP->ppFileInfos[i]->pFullFilename,
                  pTP->ppFileInfos[i]->nFileSizeLow
                  );

// typedef struct _SAM_THREAD_RESULTS
// {
//    DWORD                dwUploadedFiles;           // Successfully uploaded files.
//    unsigned __int64     iUploadedBytes;            // Total number of successfully uploaded bytes.
//    DWORD                dwFileIoErrors;
//    DWORD                dwTimeouts;
//    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
// } SAM_THREAD_RESULTS;

      bRetCode =
         GetFileCRCs (
            pTP->ppFileInfos[i]->pFullFilename, // const char  *pFilename,
            NULL,             // char        *pUseBuffer,         // NULL|ptr to buffer for file I/O. NULL-allocate/free itself.
            FILE_BUFFER_SIZE, // DWORD       dwUseBufferSize,     // 0|Size of buffer. 0:allocate itself for the whole file and ignore 'pUseBuffer'.
            pTP->bZapBits,    // BOOL        bZapBits,            // TRUE - randomly zap bits of file.
            dwZapKey,         // DWORD       dwZapKey,            // Random 32 bit number to be used to zap bits.
            acPlainCRC,       // char        *pPlainCRC,          // At least 33 bytes. Or zapped pre-scrambled CRC.
            acCryptedCRC,     // char        *pScrambledCRC,      // At least 33 bytes. Scrambled CRC.
            &dwLastError,     // DWORD       *pdwLastError,
            acErrorText,      // char        *pLastErrorText,
            sizeof (acErrorText) // DWORD       dwLastErrorTextSize
            );
      if (!bRetCode)
         {
         pTP->SamResults.dwFileIoErrors ++;   // Record error.

         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\n[%04X]U: File I/O error: %s. \n%s", pTP->uThreadID, pTP->ppFileInfos[i]->pFullFilename, acErrorText);
         textattr (g_wOrigAttrs);
         iFileIoErrors++;
         if (iFileIoErrors > FILE_IO_ERRORS_LIMIT)
            {
            // Too many consecutive file i/o errors for the given thread.
            textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
            printf ("\n[%04X]U: ERROR: Too many consecutive file I/O errors. Aborting thread...", pTP->uThreadID);
            textattr (g_wOrigAttrs);
            break;
            }
         }
      iFileIoErrors = 0;


      if (pTP->bZapNames)
         ZapFilename (pTP->ppFileInfos[i]->pFullFilename, acFilenameToSubmit);
      else
         strcpy (acFilenameToSubmit, pTP->ppFileInfos[i]->pFullFilename);


      strcpy (acTargetNameOrIP, pTP->acGatewayNameOrIP);
      strcpy (acTargetDirectoryOnGateway, g_acTargetDirectoryOnGateway);
      usTargetPort = pTP->usGatewayPort;
      bUseSSL      = pTP->bUseSSL;

      bRedirected = FALSE;

      do
         {
         // Allocate HTTP request...
         Allocate_HTTP_Header_For_SubmitSample (
            acTargetNameOrIP,    // const char           *pTargetNameOrIP,       // Machine name or IP.
            acTargetDirectoryOnGateway,   // const char           *pTargetDirectoryOnGateway,   // "/AVIS/postSuspectSample" or "AVIS/postSuspectSample"(will be prepended with '/' then).
            usTargetPort,        // unsigned short       usTargetPort,
            bUseSSL,             // BOOL                 bUseSSL,

            pTP->pConstantInfo,  // const CONSTANT_INFO  *pConstantInfo,
            pTP->ppFileInfos[i], // const FILE_INFO      *pFileInfo,
            acFilenameToSubmit,  // const char *pUseAsFilename
            acPlainCRC,          // const char           *pPlainCRCString,
            acCryptedCRC,        // const char           *pCryptedCRCString
            &pHttpRequest,       // char                 **ppAllocatedRequest:   - output - ptr to allocated request.
            &iHttpRequestSize    // int                  *piAllocatedBufferSize: - output - ptr to size of allocated request buffer.
            );

         textattr (FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);
         printf ("\n[%04X]U: Submitting: %s (%d bytes)... ", pTP->uThreadID, acFilenameToSubmit, pTP->ppFileInfos[i]->nFileSizeLow);
         textattr (g_wOrigAttrs);

         if (pTP->bVerbose) PrintfStreamingData (pHttpRequest, iHttpRequestSize, FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY);

   // typedef struct _ALL_THREAD_PARAMS
   // {
   //    HANDLE               hThread;
   //    unsigned             uThreadID;                    // GetCurrentThreadId();
   //    DWORD                dwThreadType;                 // THREAD_TYPE_*
   //
   //    char                 acGatewayNameOrIP [MAX_PATH]; // '/gateway='. navajo.watson.ibm.com. From command line or from .INI file.
   //    unsigned short       usGatewayPort;                // '/port='. 2847. From command line or from .INI file.
   //    BOOL                 bUseSSL;                      // '/ssl'.
   //    SSL_CTX              *pSslContext;                 // Used for SSL connections.
   //    BOOL                 bZapBits;                     // '/zapbits' - randomly change file contents before submitting file.
   //    BOOL                 bZapNames;                    // '/zapnames' - randomly change file name before submitting file.
   //    int                  iMaxFilesPerSamThread;        // -1:as many as found, else use this number.
   //    int                  iMaxSigsPerSigThread;         // -1:infinitely in a loop, else use this number.
   //    int                  iSamDelay;                    // ms. of delay between file submissions or between retries after error occured.
   //    int                  iSamErrorsMax;                // Allow this number of critical errors per Sam thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
   //    BOOL                 bLoop;                        // '/loop'. Do sample submission is a loop until key pressed.
   //    BOOL                 bVerbose;                     // '/verbose'. Log HTTP transaction data to screen.
   //    BOOL                 bManualSample;                // '/sammanual'. Set X-Sample-Reason to 'manual'
   //    BOOL                 bSkipDownload;                // '/sigskipdownload'. Skip signature def download
   //
   //    FILE_INFO            **ppFileInfos;                // FILE_INFO   **ppFileInfos: Array of pointers to 'FILE_INFO' structure.
   //    int                  iFileInfosNum;                // int         iFileInfosNum: Number of elements in array.
   //
   //    CONSTANT_INFO        *pConstantInfo;               // Needed for 'Allocate_HTTP_Header_For_SubmitSample()'.
   //
   //    int                  iSigDelay;                    // ms. of delay between downloads of blessed signature sets or between retries after error occured.
   //    int                  iSigErrorsMax;                // Allow this number of critical errors per Sig thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
   //
   //    int                  iMinDelay;
   //    int                  iMaxDelay;
   //    int                  iSeed;
   //
   //    SAM_THREAD_RESULTS   SamResults;
   //    SIG_THREAD_RESULTS   SigResults;
   //
   //    HANDLE               hEventTerminating;            // Will be set into signalled state if all termination needed.
   // } ALL_THREAD_PARAMS;

   // #define  TG_RETCODE_SUCCESS      0           //  - Server disconnected. Possibly some data retrieved. No errors.
   // #define  TG_RETCODE_USER_ABORT   1           //  - Callback returned FALSE.
   // #define  TG_RETCODE_TIMEOUT      2           //  - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
   // #define  TG_RETCODE_ERROR        0xFFFFFFFF  //  - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->acErrorText' has error text.
   //
   //
   // #define  TG_CALLBACK_EVENT_CONNECTED            1
   // #define  TG_CALLBACK_EVENT_REQUEST_DATA        2
   // #define  TG_CALLBACK_EVENT_DATA_SENT            3
   // #define  TG_CALLBACK_EVENT_DATA_RECEIVED        4
   // #define  TG_CALLBACK_EVENT_NO_DATA_READY        5
   //
   // typedef struct _TRANSACT_INFO_STRUCTURE
   // {
   //    char           acTargetNameOrIP [MAX_PATH];
   //    unsigned short usTargetPort;
   //    BOOL           bUseSSL;
   //    SSL_CTX        *pSslContext;                 // Used for SSL connections.
   //    DWORD          dwTotalTimeout;               // INFINITE|(ms) Stay in this function max. this time.
   //    DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
   //    BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.
   //
   //    BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
   //    DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
   //    char           *pTransactData;               // Multipurpose I/O buffer.
   //    int            iTransactDataSize;            // I/O buffer size.
   //    char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
   //    int            iWSALastErrorCode;            // last WSA error.
   //
   //    VOID           *pvUserValue;                 // Caller's-defined value;
   //    DWORD          dwUserValue;                  // Caller's-defined value;
   // } TRANSACT_INFO_STRUCTURE;


         HLP_GetFileAttributes (pTP->ppFileInfos[i]->pFullFilename, &w32FindData);
         dwFileSize = w32FindData.nFileSizeLow;

         // Open destination file.
         //
         hFile =
            CreateFile ( // HANDLE: INVALID_HANDLE_VALUE:error, else:handle.
               pTP->ppFileInfos[i]->pFullFilename, // LPCTSTR  lpFileName:   address of name of the file
               GENERIC_READ,     // DWORD  dwDesiredAccess:   access (read-write) mode
               FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,  // DWORD  dwShareMode: share mode.
               NULL,             // LPSECURITY_ATTRIBUTES  lpSecurityAttributes:   address of security descriptor
               OPEN_EXISTING,    // DWORD  dwCreationDistribution:  how to create
               0,                // DWORD  dwFlagsAndAttributes: file attributes
               NULL              // HANDLE  hTemplateFile: handle of file with attributes to copy
               );
         if (hFile == INVALID_HANDLE_VALUE)
            {
            dwLastError = GetLastError();

            pTP->SamResults.dwFileIoErrors ++;   // Record error.

            textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
            printf ("\n[%04X]U: Error: cannot open file: %s. Error %d.", pTP->uThreadID, pTP->ppFileInfos[i]->pFullFilename, dwLastError);
            textattr (g_wOrigAttrs);
            iFileIoErrors++;
            if (iFileIoErrors > FILE_IO_ERRORS_LIMIT)
               {
               // Too many consecutive file i/o errors for the given thread.
               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               printf ("\n[%04X]U: ERROR: Too many consecutive file I/O errors. Aborting thread...", pTP->uThreadID);
               textattr (g_wOrigAttrs);
               break;
               }
            }
         else
            {
            iFileIoErrors = 0;

            memset (&tisStructure, 0, sizeof (tisStructure));
            strcpy (tisStructure.acTargetNameOrIP, acTargetNameOrIP);
            tisStructure.usTargetPort      = usTargetPort;
            tisStructure.bUseSSL           = bUseSSL;
            tisStructure.pSslContext       = pTP->pSslContext; // Used for SSL connections.
            tisStructure.pTransactData     = NULL;    // Callback will initialize.
            tisStructure.iTransactDataSize = 0;       // Callback will initialize.
            tisStructure.pfn_EventCallback = Callback_SubmitSample;
            tisStructure.dwTotalTimeout    = GATEWAY_TOTAL_TIMEOUT;  // Give gateway 5 minutes to send us reply.
            tisStructure.dwCallbackTimeout = GATEWAY_ACK_TIMEOUT;    // Callback time value - 1 sec.
            tisStructure.bDumpData         = FALSE;         // Save data

            // Initialize data, used by the callback.
            memset (&usCallbackData, 0, sizeof (usCallbackData));
            usCallbackData.pTP                              =  pTP;
            usCallbackData.dwBytesSentSoFar                 =  0;
            usCallbackData.dwBytesReceivedSoFar             =  0;
            usCallbackData.dwStartTimerValue                =  StartTimer();
            usCallbackData.bAboutToSendVeryFirstDataChunk   =  TRUE;
            usCallbackData.hFile                            =  hFile;
            usCallbackData.dwFileSize                       =  dwFileSize;
            usCallbackData.dwZapKey                         =  dwZapKey;
            usCallbackData.pvScrambleHandle                 =  NULL;
            usCallbackData.pDataBuffer                      =  new char [FILE_BUFFER_SIZE];
            usCallbackData.iDataBufferSize                  =  FILE_BUFFER_SIZE;
            usCallbackData.pHTTPHeader                      =  pHttpRequest;
            usCallbackData.iHTTPHeaderSize                  =  iHttpRequestSize;

            tisStructure.pvUserValue = (VOID *)&usCallbackData;

      // typedef struct _UPLOAD_SAMPLE_CALLBACK_DATA
      // {
      //    ALL_THREAD_PARAMS    *pTP;
      //    DWORD                dwBytesSentSoFar;       // 0 at first.
      //    DWORD                dwBytesReceivedSoFar;   // 0 at first.
      //    DWORD                dwStartTimerValue;
      //    BOOL                 bAboutToSendVeryFirstDataChunk; // TRUE at first.
      //    HANDLE               hFile;
      //    DWORD                dwFileSize;
      //    DWORD                dwZapKey;
      //    VOID                 *pvScrambleHandle;
      //
      //    char                 *pDataBuffer;           // Preallocated buffer
      //    int                  iDataBufferSize;        // Size of it.
      //    char                 *pHTTPHeader;           // HTTP header.
      //    int                  iHTTPHeaderSize;        // Size of it.
      // } UPLOAD_SAMPLE_CALLBACK_DATA;

            // Returns: TG_RETCODE_[SUCCESS|USER_ABORT|TIMEOUT|ERROR].
            dwRetCode = Allocate_TransactWithServer (&tisStructure);

            delete [] (void *)usCallbackData.pDataBuffer;

            CloseHandle (hFile);

   // typedef struct _SAM_THREAD_RESULTS
   // {
   //    DWORD                dwUploadedFiles;           // Successfully uploaded files.
   //    unsigned __int64     iUploadedBytes;            // Total number of successfully uploaded bytes.
   //    DWORD                dwFileIoErrors;
   //    DWORD                dwTimeouts;
   //    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
   // } SAM_THREAD_RESULTS;


            switch (dwRetCode)
               {
               case  TG_RETCODE_SUCCESS   :
                  dwTimerValue = QueryTimer (usCallbackData.dwStartTimerValue);


                  textattr (FOREGROUND_GREEN + FOREGROUND_INTENSITY);
                  printf ("\n[%04X]U: Reply received. %d.%d sec passed since submitting file: '%s' (%d bytes)",
                     pTP->uThreadID,
                     dwTimerValue / 1000,
                     dwTimerValue % 1000,
                     acFilenameToSubmit,
                     pTP->ppFileInfos[i]->nFileSizeLow
                     );
                  textattr (g_wOrigAttrs);
                  if (pTP->bVerbose) PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_GREEN + FOREGROUND_INTENSITY);
                  printf ("\n[%04X]U: Server: %s:%d disconnected. Request completed...", pTP->uThreadID, pTP->acGatewayNameOrIP, (DWORD)pTP->usGatewayPort);

                  // Checking for redirection here. Init 'bRedirected' if TRUE and all vars are initialized.
                  pTmpPtr =
                     StrStr  (
                        tisStructure.pTransactData,      // const char  *pBuffer,
                        g_acRedirectionSearchPattern,    // const char  *pString,
                        tisStructure.iTransactDataSize,  // int         iBufferSize,
                        FALSE,                           // BOOL        bCaseSensitive,
                        TRUE,                            // BOOL        bSpecSearch /*=FALSE*/,   // TRUE:'?' and '*' considered as spec, not as 'pString' part.
                        NULL                             // int         *piFoundLen /*=NULL */
                        );
                  if (pTmpPtr)
                     {
                     // Redirection directive in HTTP reply detected. Parse it to try to figure out where is redirection is to.

                     // Possible format of 'Location' reply:
                     // 'Location: https://another-one.watson.ibm.com:2847/AVIS/postSuspectSample'
                     // 'Location: http://another-one.watson.ibm.com/AVIS/postSuspectSample'

                     // If function returns TRUE - redirection data is initialized.
                     bRedirected =
                        ParseNewLocationData (
                           tisStructure.pTransactData,      // const char     *pLocationStrBuffer,    // input - buffer with data, where "Location" text is.
                           tisStructure.iTransactDataSize,  // int            iLocationStrBufferSize, // input : int - size of above buffer.
                           acTargetNameOrIP,                // char           *pHostNameOrIP,      // output: must be at least MAX_PATH long.
                           &usTargetPort,                   // unsigned short *pusPortNumber,      // output:
                           acTargetDirectoryOnGateway,      // char           *pDirectoryName,     // output: must be at least MAX_PATH long.
                           &bUseSSL                         // BOOL           *bUseSSL             // output.
                           );
                     if (bRedirected)
                        {
                        textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                        printf ("\n[%04X]U: Request will be redirected to location: http%s://%s:%d%s",
                              pTP->uThreadID,
                              bUseSSL?"s":"",
                              acTargetNameOrIP,
                              (int)usTargetPort,
                              acTargetDirectoryOnGateway
                              );
                        textattr (g_wOrigAttrs);
                        }
                     }
                  else
                     {
                     bRedirected = FALSE;
                  if (!bRedirected)
                     {
                     pTP->SamResults.dwUploadedFiles ++;   // Record event.
                     pTP->SamResults.iUploadedBytes += pTP->ppFileInfos[i]->nFileSizeLow;
                     }

                     // After completely successful non-redirected upload transaction
                     // store checksum to array.
                     StoreChecksumToArray (
                        acPlainCRC,                // const char  *pCheckSum,             // ASCIIZ MD5 checksum to store to array.
                        pTP->iMaxCheckSumsNew,     // int         iMaxAllowedChecksums,   // Store no more than this number of checksums.
                        pTP->pppCheckSumsNew,       // char        ***pppCheckSums,
                        pTP->piCheckSumsNewNum      // int         *piCheckSumsNum
                        );
                     }

                  break;


               case  TG_RETCODE_USER_ABORT:
                  // Count only file bytes.
                  if (usCallbackData.dwBytesSentSoFar > (DWORD)usCallbackData.iHTTPHeaderSize)
                     pTP->SamResults.iUploadedBytes += (usCallbackData.dwBytesSentSoFar - usCallbackData.iHTTPHeaderSize);

                  textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                  printf ("\n[%04X]U: Terminating sample uploader thread per user's request...", pTP->uThreadID);
                  textattr (g_wOrigAttrs);
                  bBreakCondition = TRUE;
                  break;


               case  TG_RETCODE_TIMEOUT   :
                  dwTimerValue = QueryTimer (usCallbackData.dwStartTimerValue);

                  pTP->SamResults.dwTimeouts ++;
                  // Count only file bytes.
                  if (usCallbackData.dwBytesSentSoFar > (DWORD)usCallbackData.iHTTPHeaderSize)
                     pTP->SamResults.iUploadedBytes += (usCallbackData.dwBytesSentSoFar - usCallbackData.iHTTPHeaderSize);

                  textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                  printf (
                     "\n[%04X]U: Warning: Timeout! %d.%d sec passed since submitting file: '%s' (%d bytes)",
                     pTP->uThreadID,
                     dwTimerValue / 1000,
                     dwTimerValue % 1000,
                     acFilenameToSubmit,
                     pTP->ppFileInfos[i]->nFileSizeLow
                     );
                  textattr (g_wOrigAttrs);
                  if (tisStructure.iTransactDataSize)
                     {
                     textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                     printf ("\n[%04X]U: Warning: Disconnecting from server after timeout... Received %d bytes:",
                              pTP->uThreadID, tisStructure.iTransactDataSize);
                     textattr (g_wOrigAttrs);
                     if (pTP->bVerbose) PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_GREEN + FOREGROUND_INTENSITY);
                     }
                  else
                     {
                     textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                     printf ("\n[%04X]U: Warning: Disconnecting from server after timeout... Nothing received.",
                              pTP->uThreadID);
                     textattr (g_wOrigAttrs);
                     }
                  break;


               case  TG_RETCODE_ERROR     :
               default:
                  AddErrorToArray (
                     pTP->SamResults.dwCriticalErrors,   // DWORD    ppdwErrorArray[][2],
                     tisStructure.iWSALastErrorCode      // DWORD    dwErrorID,
                     // int      iSizeOfArray   /* = CRITICAL_ERROR_IDS_MAX */,
                     // DWORD    dwErrorsNumber /* = 1 */
                     );

                  textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                  printf ("\n[%04X]U: *** Critical Error ***: %s (%d).",
                        pTP->uThreadID,
                        tisStructure.acErrorText[0]?tisStructure.acErrorText:"Unknown",
                        tisStructure.iWSALastErrorCode
                        );
                  if (tisStructure.iWSALastErrorCode == WSAECONNREFUSED)
                     printf ("\n[%04X]U: %d = Connection refused.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
                  if (tisStructure.iWSALastErrorCode == WSAETIMEDOUT)
                     printf ("\n[%04X]U: %d = Timeout waiting for connection.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
                  if (tisStructure.iWSALastErrorCode == WSAECONNRESET)
                     printf ("\n[%04X]U: %d = Connection reset by peer.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
                  if (tisStructure.iWSALastErrorCode == WSAHOST_NOT_FOUND)
                     printf ("\n[%04X]U: %d = Host not found.", pTP->uThreadID, tisStructure.iWSALastErrorCode);


                  dwCriticalErrors ++;

                  // if 'pTP->iSamErrorsMax' < 0 - then unlimited number of errors allowed.
                  if (pTP->iSamErrorsMax >= 0 && (dwCriticalErrors > (DWORD)pTP->iSamErrorsMax))
                     {
                     printf ("\n[%04X]U: Too many critical errors (%d). Aborting thread...", pTP->uThreadID, dwCriticalErrors);
                     bBreakCondition = TRUE;
                     }
                  else
                     printf ("\n[%04X]U: So far encountered %d critical errors. Continuing...", pTP->uThreadID, dwCriticalErrors);
                  textattr (g_wOrigAttrs);
                  break;
               }

            // Release allocated resources.
            Free_TransactWithServer (tisStructure.pTransactData);
            }

   // typedef struct _SAM_THREAD_RESULTS
   // {
   //    DWORD                dwUploadedFiles;           // Successfully uploaded files.
   //    unsigned __int64     iUploadedBytes;            // Total number of successfully uploaded bytes.
   //    DWORD                dwFileIoErrors;
   //    DWORD                dwTimeouts;
   //    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
   // } SAM_THREAD_RESULTS;

         Free_HTTP_Request (pHttpRequest);
         pHttpRequest  = NULL;
         }
      while (!bBreakCondition && bRedirected);


      if (bBreakCondition)
         break;

      // In case of loop - reset counter to the beginning of the file heap.
      if (pTP->bLoop && (i == (iFilesNum-1)))
         i = -1;  // Next iteration will make it 0.

      // Execute delay between submissions.
      if (pTP->iSamDelay)
         Sleep (pTP->iSamDelay);
      }


   // Deallocate resources, in case we were breaken out.
   if (pHttpRequest)
      Free_HTTP_Request (pHttpRequest);
   pHttpRequest  = NULL;

   textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
   printf ("\n[%04X]U: Sample Uploader thread terminating...", pTP->uThreadID);
   textattr (g_wOrigAttrs);

   _endthreadex (0);
   return 0;   // To avoid warning.
}
//===========================================================================

//===========================================================================
//
// Returns: TRUE:  continue processing.
//          FALSE: abort requested.

BOOL  Callback_SubmitSample   (TRANSACT_INFO_STRUCTURE *pTIS)
{
//---------------------------------------------------------------------------
   UPLOAD_SAMPLE_CALLBACK_DATA   *pusCallbackData = (UPLOAD_SAMPLE_CALLBACK_DATA *)pTIS->pvUserValue;
   DWORD                dwBytes;

   DWORD                dwLastError;
   BOOL                 bRetCode = TRUE;
   DWORD                dwCurrentTime;
//---------------------------------------------------------------------------

   // typedef struct _UPLOAD_SAMPLE_CALLBACK_DATA
   // {
   //    ALL_THREAD_PARAMS    *pTP;
   //    DWORD                dwBytesSentSoFar;       // 0 at first.
   //    DWORD                dwBytesReceivedSoFar;   // 0 at first.
   //    DWORD                dwStartTimerValue;
   //    BOOL                 bAboutToSendVeryFirstDataChunk; // TRUE at first.
   //    HANDLE               hFile;
   //    DWORD                dwFileSize;
   //    DWORD                dwZapKey;
   //    VOID                 *pvScrambleHandle;
   //
   //    char                 *pDataBuffer;           // Preallocated buffer
   //    int                  iDataBufferSize;        // Size of it.
   //    char                 *pHTTPHeader;           // HTTP header.
   //    int                  iHTTPHeaderSize;        // Size of it.
   // } UPLOAD_SAMPLE_CALLBACK_DATA;


   dwCurrentTime = QueryTimer (pusCallbackData->dwStartTimerValue);

   switch (pTIS->dwEventID)
      {
      case  TG_CALLBACK_EVENT_CONNECTED      :
         printf ("\n[%04X]U: Connected to: %s : %d",
            pusCallbackData->pTP->uThreadID,
            pTIS->acTargetNameOrIP,
            pTIS->usTargetPort
            );
         bRetCode = TRUE;
         break;


      case  TG_CALLBACK_EVENT_REQUEST_DATA  :
         if (pusCallbackData->bAboutToSendVeryFirstDataChunk)
            {
            // Reset flag.
            pusCallbackData->bAboutToSendVeryFirstDataChunk = FALSE;

            // Send HTTP header as the very first data chunk.
            pTIS->pTransactData     = pusCallbackData->pHTTPHeader;
            pTIS->iTransactDataSize = pusCallbackData->iHTTPHeaderSize;
            }
         else
            {
            bRetCode =
               ReadFile ( // BOOL: fail=>0, else != 0.
                  pusCallbackData->hFile,             // HANDLE hFile: handle of file to read
                  pusCallbackData->pDataBuffer,       // LPVOID lpBuffer: address of buffer that receives data
                  pusCallbackData->iDataBufferSize,   // DWORD nNumberOfBytesToRead:  number of bytes to read
                  &dwBytes,         // LPDWORD lpNumberOfBytesRead: address of number of bytes read
                  NULL              // LPOVERLAPPED lpOverlapped:   address of structure for data
                  );
            dwLastError = GetLastError();

            if (!bRetCode)
               {
               dwLastError = GetLastError();
               GetLastErrorText (pTIS->acErrorText, sizeof (pTIS->acErrorText), dwLastError);

               pTIS->pTransactData     = NULL;
               pTIS->iTransactDataSize = 0;

               bRetCode = FALSE;
               }
            else
               {
               if (dwBytes)
                  {
                  // Next data was read.
                  if (pusCallbackData->pTP->bZapBits)
                     pusCallbackData->dwZapKey =
                        ZapBitsInBuffer (
                           pusCallbackData->dwZapKey,       // DWORD    dwStartingRandomNumber_Or_Handle
                           pusCallbackData->pDataBuffer,    // char     *pBuffer,
                           pusCallbackData->iDataBufferSize // int      iBufferSize
                           );

                  // Scramble buffer.
                  pusCallbackData->pvScrambleHandle =
                     ScrambleBuffer (
                        pusCallbackData->pvScrambleHandle,  // VOID     *pvHandle,     // NULL:first time run, else:Scramble handle, returned from prev. call.
                        pusCallbackData->pDataBuffer,       // char     *pBuffer,
                        pusCallbackData->iDataBufferSize    // DWORD    dwBufferSize,
                        );

                  pTIS->pTransactData     = pusCallbackData->pDataBuffer;
                  pTIS->iTransactDataSize = dwBytes;
                  bRetCode = TRUE;
                  }
               else
                  {
                  // No more data condition.
                  pTIS->pTransactData     = NULL;
                  pTIS->iTransactDataSize = 0;

                  bRetCode = TRUE;
                  }
               }

            }
         break;


      case  TG_CALLBACK_EVENT_DATA_SENT      :
         pusCallbackData->dwBytesSentSoFar += pTIS->iTransactDataSize;
	 if (pusCallbackData->pTP->bVerbose)
	   printf ("\n[%04X]U: Sent %d of %d bytes ...",
		   pusCallbackData->pTP->uThreadID,
		   pusCallbackData->dwBytesSentSoFar,
		   pusCallbackData->dwFileSize + pusCallbackData->iHTTPHeaderSize
		   );
         bRetCode = TRUE;
         break;


      case  TG_CALLBACK_EVENT_DATA_RECEIVED  :
         pusCallbackData->dwBytesReceivedSoFar += pTIS->iTransactDataSize;
         break;


      case  TG_CALLBACK_EVENT_NO_DATA_READY  :
         // Printf in-progress
         printf ("\n[%04X]U: Waiting for more data. So far %d bytes received. %d.%d sec passed (%d bytes/sec) ...",
               pusCallbackData->pTP->uThreadID,
               pusCallbackData->dwBytesReceivedSoFar,
               dwCurrentTime/1000,
               dwCurrentTime%1000,
               (pusCallbackData->dwBytesReceivedSoFar * 1000) / dwCurrentTime
               );
         bRetCode = TRUE;
         break;


      default                                :
         bRetCode = FALSE;
         break; // Unknown event ID - debug it!
      }

   // Check for user abort condition.
   if (bRetCode && (WaitForSingleObject (pusCallbackData->pTP->hEventTerminating, 0) != WAIT_TIMEOUT))
      {
      strcpy (pTIS->acErrorText, "Timeout");
      bRetCode = FALSE;
      }


   return (bRetCode);


// #define  TG_RETCODE_SUCCESS      0           //  - Server disconnected. Possibly some data retrieved. No errors.
// #define  TG_RETCODE_USER_ABORT   1           //  - Callback returned FALSE.
// #define  TG_RETCODE_TIMEOUT      2           //  - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
// #define  TG_RETCODE_ERROR        0xFFFFFFFF  //  - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->acErrorText' has error text.
//
//
// #define  TG_CALLBACK_EVENT_CONNECTED            1
// #define  TG_CALLBACK_EVENT_REQUEST_DATA        2
// #define  TG_CALLBACK_EVENT_DATA_SENT            3
// #define  TG_CALLBACK_EVENT_DATA_RECEIVED        4
// #define  TG_CALLBACK_EVENT_NO_DATA_READY        5
//
// typedef struct _TRANSACT_INFO_STRUCTURE
// {
//    char           acTargetNameOrIP [MAX_PATH];
//    unsigned short usTargetPort;
//    BOOL           bUseSSL;
//    SSL_CTX        *pSslContext;                 // Used for SSL connections.
//    DWORD          dwTotalTimeout;               // INFINITE|(ms) Stay in this function max. this time.
//    DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
//    BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.
//
//    BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
//    DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
//    char           *pTransactData;               // Multipurpose I/O buffer.
//    int            iTransactDataSize;            // I/O buffer size.
//    char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
//    int            iWSALastErrorCode;            // last WSA error.
//
//    VOID           *pvUserValue;                 // Caller's-defined value;
//    DWORD          dwUserValue;                  // Caller's-defined value;
// } TRANSACT_INFO_STRUCTURE;

}
//===========================================================================

//===========================================================================
//
// Thread will download blessed signature set from the gateway.

unsigned __stdcall   DownloadSignatureSet_Thread (VOID *pvThreadParm)
{
//---------------------------------------------------------------------------
   ALL_THREAD_PARAMS   *pTP = (ALL_THREAD_PARAMS *)pvThreadParm;

   char                 acRequest1 [] =
                           "HEAD /AVIS/headSignatureSet HTTP/1.0" // new
                           "\nX-Signatures-Sequence: blessed"     // new
                           "\n\n";                                // new

   char                 acRequest2Format [] =
                           "GET %s HTTP/1.0"  // "/AVIS/getSignatureSet" // new
                           "\nX-Signatures-Sequence: %s"                 // new
                           "\nAccept: application/x-NAV-VDB"             // new
                           "\n\n";                                       // new

   char                 acRequest2Data [sizeof(acRequest2Format) * 2];

   char                 acSearchString [] = "X_Signatures_Sequence";
   char                 acSignatureSequenceStr [20];  // "0008274"

   char                 acErrorMsg [512]; // Non-CRLF-ed error description.
   int                  iWSALastError = 0;

   TRANSACT_INFO_STRUCTURE  tisStructure;
   DOWNLOAD_SIGSET_CALLBACK_DATA cdCallbackData;

   // We will use these values as working values, to accomodate cases where redirection
   // happens.
   BOOL                 bRedirected;

   char                 acTargetNameOrIP [MAX_PATH];           // Machine name or IP.
   char                 acTargetDirectoryOnGateway [MAX_PATH]; // "/AVIS/postSuspectSample" or "AVIS/postSuspectSample"(will be prepended with '/' then).
   unsigned short       usTargetPort;
   BOOL                 bUseSSL;
   char                 *pTmpPtr;


   DWORD                dwCriticalErrors = 0;
   DWORD                dwStartTimer;
   DWORD                dwTimerValue;
   int                  iCounter;
   BOOL                 bProblemDownloadingSigSet; // Decrement 'iCounter' only if 'bProblemDownloadingSigSet' is FALSE.

   DWORD                dwRetCode;
   BOOL                 bRetCode;
//---------------------------------------------------------------------------


   iCounter = pTP->iMaxSigsPerSigThread;
   bProblemDownloadingSigSet = FALSE;

   // Initiate random number generator with the requested seed value.
   GetRandomNumber   (0, 1, pTP->iSeed);

   do
      {
      // Check for terminating conditions at the beginning of each loop.
      if (WaitForSingleObject (pTP->hEventTerminating, 0) != WAIT_TIMEOUT)
         {
         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\n[%04X]D: Terminating per user's request", pTP->uThreadID);
         textattr (g_wOrigAttrs);
         break;
         }

      if (pTP->iMaxSigsPerSigThread == -1)
         ;   // Do infinitely.
      else
         {
         if (!bProblemDownloadingSigSet)
            iCounter --;

         if (iCounter < 0)
            break;
         }

      // Execute random delay, if requested.
      if (pTP->iMinDelay || pTP->iMaxDelay)
         {
         int   iDelay = GetRandomNumber (pTP->iMinDelay, pTP->iMaxDelay);
         printf ("\n[%04X]D: Sleeping for %d ms...", pTP->uThreadID, iDelay);
         Sleep (iDelay);
         }

      bProblemDownloadingSigSet = FALSE;


      strcpy (acTargetNameOrIP, pTP->acGatewayNameOrIP);
      strcpy (acTargetDirectoryOnGateway, "/AVIS/getSignatureSet");
      usTargetPort = pTP->usGatewayPort;
      bUseSSL      = pTP->bUseSSL;

      bRedirected = FALSE;

      do
         {
         memset (&tisStructure, 0, sizeof (tisStructure));
         strcpy (tisStructure.acTargetNameOrIP, acTargetNameOrIP);
         tisStructure.usTargetPort      = usTargetPort;
         tisStructure.bUseSSL           = bUseSSL;
         tisStructure.pSslContext       = pTP->pSslContext; // Used for SSL connections.
         tisStructure.pTransactData     = acRequest1;          // No callback. Data to send already initialized.
         tisStructure.iTransactDataSize = strlen (acRequest1);
         tisStructure.pfn_EventCallback = NULL;
         tisStructure.dwTotalTimeout    = 240*1000; // 4 minutes to get reply about the latest blesses signature set ID.
         tisStructure.dwCallbackTimeout = tisStructure.dwTotalTimeout;  // Callback is NULL - no reason to have other callback timeout.
         tisStructure.bDumpData         = FALSE;
         // Remaining values we don't need to initialize.

         printf ("\n[%04X]D: Sending request to gateway for the latest signature set ID. Wait...", pTP->uThreadID);
         if (pTP->bVerbose) PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY);

         dwStartTimer = StartTimer();

         // Returns: TG_RETCODE_[SUCCESS|USER_ABORT|TIMEOUT|ERROR].
         dwRetCode = Allocate_TransactWithServer (&tisStructure);

   // typedef struct _SIG_THREAD_RESULTS
   // {
   //    DWORD                dwDownloadedSignatureSets; // Successfully downloaded signature sets.
   //    unsigned __int64     iDownloadedBytes;         // Total number of successfully downloaded bytes.
   //    DWORD                dwTimeouts;
   //    DWORD                dwErraticReplies;          // Any reply, that did not contain expected data.
   //    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
   // } SIG_THREAD_RESULTS;

         switch (dwRetCode)
            {
            case  TG_RETCODE_SUCCESS   :
               if (pTP->bVerbose) PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_GREEN + FOREGROUND_INTENSITY);

               // Checking for redirection here. Init 'bRedirected' if TRUE and all vars are initialized.
               pTmpPtr =
                  StrStr  (
                     tisStructure.pTransactData,      // const char  *pBuffer,
                     g_acRedirectionSearchPattern,    // const char  *pString,
                     tisStructure.iTransactDataSize,  // int         iBufferSize,
                     FALSE,                           // BOOL        bCaseSensitive,
                     TRUE,                            // BOOL        bSpecSearch /*=FALSE*/,   // TRUE:'?' and '*' considered as spec, not as 'pString' part.
                     NULL                             // int         *piFoundLen /*=NULL */
                     );
               if (pTmpPtr)
                  {
                  // Redirection directive in HTTP reply detected. Parse it to try to figure out where is redirection is to.

                  // Possible format of 'Location' reply:
                  // 'Location: https://another-one.watson.ibm.com:2847/AVIS/postSuspectSample'
                  // 'Location: http://another-one.watson.ibm.com/AVIS/postSuspectSample'

                  // If function returns TRUE - redirection data is initialized.
                  bRedirected =
                     ParseNewLocationData (
                        tisStructure.pTransactData,      // const char     *pLocationStrBuffer,    // input - buffer with data, where "Location" text is.
                        tisStructure.iTransactDataSize,  // int            iLocationStrBufferSize, // input : int - size of above buffer.
                        acTargetNameOrIP,                // char           *pHostNameOrIP,      // output: must be at least MAX_PATH long.
                        &usTargetPort,                   // unsigned short *pusPortNumber,      // output:
                        acTargetDirectoryOnGateway,      // char           *pDirectoryName,     // output: must be at least MAX_PATH long.
                        &bUseSSL                         // BOOL           *bUseSSL             // output.
                        );
                  if (bRedirected)
                     {
                     textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                     printf ("\n[%04X]D: Request will be redirected to location: http%s://%s:%d%s",
                           pTP->uThreadID,
                           bUseSSL?"s":"",
                           acTargetNameOrIP,
                           (int)usTargetPort,
                           acTargetDirectoryOnGateway
                           );
                     textattr (g_wOrigAttrs);
                     }
                  }
               else
                  bRedirected = FALSE;

               break;


            case  TG_RETCODE_USER_ABORT:  // Cannot be here actually at that stage.
               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               printf ("\n[%04X]D: Terminating signature downloader thread per user's request.", pTP->uThreadID);
               textattr (g_wOrigAttrs);
               Free_TransactWithServer (tisStructure.pTransactData);
               break;

            case  TG_RETCODE_TIMEOUT   :
               // We have to report
               // this as a timeout condition - incomplete data received from server - no
               // disconnect within specified timeout.
               dwTimerValue = QueryTimer(dwStartTimer);

               pTP->SigResults.dwTimeouts++; // Report event.

               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               printf ("\n[%04X]D: Warning: timeouted after %d.%d sec of waiting for server to complete reply. Let's see what we have...",
                        pTP->uThreadID,
                        dwTimerValue / 1000,
                        dwTimerValue % 1000
                        );
               textattr (g_wOrigAttrs);
               if (pTP->bVerbose) PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_GREEN);
               break;

            case  TG_RETCODE_ERROR     :
            default:
               bProblemDownloadingSigSet = TRUE;

               iWSALastError = tisStructure.iWSALastErrorCode;
               strcpy (acErrorMsg, tisStructure.acErrorText);

               AddErrorToArray (
                  pTP->SigResults.dwCriticalErrors,   // DWORD    ppdwErrorArray[][2],
                  tisStructure.iWSALastErrorCode      // DWORD    dwErrorID,
                  // int      iSizeOfArray   /* = CRITICAL_ERROR_IDS_MAX */,
                  // DWORD    dwErrorsNumber /* = 1 */
                  );

               // Release allocated resources.
               Free_TransactWithServer (tisStructure.pTransactData);

               if (tisStructure.iWSALastErrorCode == WSAECONNREFUSED)
                  printf ("\n[%04X]D: *** Critical error *** %d = Connection refused.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAETIMEDOUT)
                  printf ("\n[%04X]D: *** Critical error *** %d = Timeout waiting for connection.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAECONNRESET)
                  printf ("\n[%04X]D: *** Critical error *** %d = Connection reset by peer.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAHOST_NOT_FOUND)
                  printf ("\n[%04X]D: *** Critical error *** %d = Host not found.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else
                  printf ("\n[%04X]D: *** Critical error *** %d.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               break;
            }


         if (dwRetCode == TG_RETCODE_SUCCESS && bRedirected)
            // Deallocate resources before next redirection loop.
            Free_TransactWithServer  (
               tisStructure.pTransactData      // char     **ppData,           // Ptr to allocated buffer with data, as it was passed to 'Allocate_TransactWithServer()'.
               );
         }
      while (bRedirected);


      if (dwRetCode == TG_RETCODE_ERROR)
         {
         dwCriticalErrors ++;

         // if 'pTP->iSigErrorsMax' < 0 - then unlimited number of errors allowed.
         if (pTP->iSigErrorsMax >= 0 && (dwCriticalErrors > (DWORD)pTP->iSigErrorsMax))
            {
            printf ("\n[%04X]D: Too many critical errors (%d). Aborting thread...", pTP->uThreadID, dwCriticalErrors);
            goto  ERROR_EXIT;
            }

         printf ("\n[%04X]D: So far encountered %d critical errors. Continuing...", pTP->uThreadID, dwCriticalErrors);
         // Execute optional delay.
         if (pTP->iSigDelay)
            Sleep (pTP->iSigDelay);

         continue;
         }


      if (dwRetCode == TG_RETCODE_USER_ABORT)
         break;


      // We are interested in line: 'X_Signatures_Sequence: 00001870'
      //
      bRetCode =
         FindKey (
            tisStructure.pTransactData,     // const char  *pBuffer,         // Where to search for.
            tisStructure.iTransactDataSize, // int         iBufferSize,      // Size of buffer.
            acSearchString,   // const char  *pKeyName,        // ASCIIZ name of key.
            ':',              // char        cSeparator,
            FALSE,            // BOOL        bCaseSensitive,   // TRUE:case sensitive
            acSignatureSequenceStr, // char        *pKeyValueStr,    // Where to return key value.
            sizeof (acSignatureSequenceStr) // int         iKeyValueStrSize  // Size of destination buffer
            );

      // Deallocate resources now.
      Free_TransactWithServer  (
         tisStructure.pTransactData      // char     **ppData,           // Ptr to allocated buffer with data, as it was passed to 'Allocate_TransactWithServer()'.
         );

      if (!bRetCode)
         {
         pTP->SigResults.dwErraticReplies++; // Report event.

         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\n[%04X]D: Warning: couldn't find valid '%s' value in gateway's reply.", pTP->uThreadID, acSearchString);
         printf ("\n[%04X]D: Thread will repeat request from start.", pTP->uThreadID);
         textattr (g_wOrigAttrs);

         // Execute optional delay.
         if (pTP->iSigDelay)
            Sleep (pTP->iSigDelay);

         bProblemDownloadingSigSet = TRUE;

         // Repeat loop.
         continue;
         }


      // Ok, acSignatureSequenceStr is ready now.
      textattr (FOREGROUND_GREEN + FOREGROUND_INTENSITY);
      printf ("\n[%04X]D: Gateway replied: \"blessed signature sequence number = %s\". Initiating download ...",
            pTP->uThreadID, acSignatureSequenceStr);
      textattr (g_wOrigAttrs);


      // If we are skipping definition package downlaods, then repeat
      // main loop from the top now, skipping the "GET
      // /AVIS/getSignatureSet" transaction below, starting with the
      // next "HEAD /AVIS/headSignatureSet" transaction above (EJP
      // 6/29/00).
      
      if (pTP->bSkipDownload)
	{
         // Execute optional delay.
         if (pTP->iSigDelay) Sleep (pTP->iSigDelay);
	 // Start the next iteration of the main loop now.
         continue;
	}

      // Local values of:
      // 'acTargetNameOrIP'
      // 'acTargetDirectoryOnGateway'
      // 'usTargetPort'
      // 'bUseSSL' - are already initialized by the previous part of request.

      bRedirected = FALSE;

      do
         {
         // Preparing download request in 'acRequest2Data'.
///         sprintf (acRequest2Data, acRequest2Format, acSignatureSequenceStr, acSignatureSequenceStr);

         sprintf (acRequest2Data, acRequest2Format, acTargetDirectoryOnGateway, acSignatureSequenceStr); // new
// old         sprintf (acRequest2Data, acRequest2Format, acTargetDirectoryOnGateway, acSignatureSequenceStr, acSignatureSequenceStr);

         memset (&tisStructure, 0, sizeof (tisStructure));
         strcpy (tisStructure.acTargetNameOrIP, acTargetNameOrIP);
         tisStructure.usTargetPort      = usTargetPort;
         tisStructure.bUseSSL           = bUseSSL;
         tisStructure.pSslContext       = pTP->pSslContext; // Used for SSL connections.
         tisStructure.pTransactData     = NULL; //
         tisStructure.iTransactDataSize = 0;    // / These will be initialized during callback : 'TG_CALLBACK_EVENT_REQUEST_DATA'.
         tisStructure.pfn_EventCallback = Callback_DownloadSigSet;
         tisStructure.dwTotalTimeout    = 15*60*1000; // 15 minutes to download signature set.
         tisStructure.dwCallbackTimeout = 1000;       // Callback approx. every 1 second.
         tisStructure.bDumpData         = FALSE;      // Don't need this huge binary data, but need first block. WCA.

         // Initialize data, used by the callback.
         memset (&cdCallbackData, 0, sizeof (cdCallbackData));
         cdCallbackData.pTP                              =  pTP;
         cdCallbackData.dwBytesReceivedSoFar             =  0;
         cdCallbackData.dwStartTimerValue                =  StartTimer();
         cdCallbackData.bAboutToSendVeryFirstDataChunk   =  TRUE;
         cdCallbackData.bReceivedVeryFirstDataChunk      =  TRUE;
         cdCallbackData.dwTotalSizeOfDataToExpect        =  0;
         cdCallbackData.pDataBuffer                      =  acRequest2Data;
         cdCallbackData.iDataBufferSize                  =  strlen (acRequest2Data);

         tisStructure.pvUserValue = (VOID *)&cdCallbackData;

   // typedef struct _DOWNLOAD_SIGSET_CALLBACK_DATA
   // {
   //    ALL_THREAD_PARAMS    *pTP;
   //    DWORD                dwBytesReceivedSoFar;
   //    DWORD                dwStartTimerValue;
   //    BOOL                 bAboutToSendVeryFirstDataChunk;
   //    BOOL                 bReceivedVeryFirstDataChunk;
   //    DWORD                dwTotalSizeOfDataToExpect;
   //    char                 *pDataBuffer;
   //    int                  iDataBufferSize;
   // } DOWNLOAD_SIGSET_CALLBACK_DATA;

   // typedef struct _SIG_THREAD_RESULTS
   // {
   //    DWORD                dwDownloadedSignatureSets; // Successfully downloaded signature sets.
   //    unsigned __int64     iDownloadedBytes;         // Total number of successfully downloaded bytes.
   //    DWORD                dwTimeouts;
   //    DWORD                dwErraticReplies;          // Any reply, that did not contain expected data.
   //    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
   // } SIG_THREAD_RESULTS;

         // Remaining values we don't need to initialize.
         printf ("\n[%04X]D: Sending request to the gateway to retrieve blessed signature set. Wait...", pTP->uThreadID);
///         PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY);
         if (pTP->bVerbose) PrintfStreamingData (cdCallbackData.pDataBuffer, cdCallbackData.iDataBufferSize, FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY);

         // Returns: TG_RETCODE_[SUCCESS|USER_ABORT|TIMEOUT|ERROR].
         dwRetCode = Allocate_TransactWithServer (&tisStructure);

         switch (dwRetCode)
            {
            case  TG_RETCODE_SUCCESS   :

               textattr (FOREGROUND_GREEN + FOREGROUND_INTENSITY);
               printf ("\n[%04X]D: Received %d bytes of blessed signature set reply.",
                     pTP->uThreadID, cdCallbackData.dwBytesReceivedSoFar);
               textattr (g_wOrigAttrs);

               // Checking for redirection here. Init 'bRedirected' if TRUE and all vars are initialized.
               pTmpPtr =
                  StrStr  (
                     tisStructure.pTransactData,      // const char  *pBuffer,
                     g_acRedirectionSearchPattern,    // const char  *pString,
                     tisStructure.iTransactDataSize,  // int         iBufferSize,
                     FALSE,                           // BOOL        bCaseSensitive,
                     TRUE,                            // BOOL        bSpecSearch /*=FALSE*/,   // TRUE:'?' and '*' considered as spec, not as 'pString' part.
                     NULL                             // int         *piFoundLen /*=NULL */
                     );
               if (pTmpPtr)
                  {
                  // Redirection directive in HTTP reply detected. Parse it to try to figure out where is redirection is to.

                  // Possible format of 'Location' reply:
                  // 'Location: https://another-one.watson.ibm.com:2847/AVIS/postSuspectSample'
                  // 'Location: http://another-one.watson.ibm.com/AVIS/postSuspectSample'

                  // If function returns TRUE - redirection data is initialized.
                  bRedirected =
                     ParseNewLocationData (
                        tisStructure.pTransactData,      // const char     *pLocationStrBuffer,    // input - buffer with data, where "Location" text is.
                        tisStructure.iTransactDataSize,  // int            iLocationStrBufferSize, // input : int - size of above buffer.
                        acTargetNameOrIP,                // char           *pHostNameOrIP,      // output: must be at least MAX_PATH long.
                        &usTargetPort,                   // unsigned short *pusPortNumber,      // output:
                        acTargetDirectoryOnGateway,      // char           *pDirectoryName,     // output: must be at least MAX_PATH long.
                        &bUseSSL                         // BOOL           *bUseSSL             // output.
                        );
                  if (bRedirected)
                     {
                     textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                     printf ("\n[%04X]D: Request will be redirected to location: http%s://%s:%d%s",
                           pTP->uThreadID,
                           bUseSSL?"s":"",
                           acTargetNameOrIP,
                           (int)usTargetPort,
                           acTargetDirectoryOnGateway
                           );
                     textattr (g_wOrigAttrs);
                     }
                  }
               else
                  bRedirected = FALSE;
               if (!bRedirected)
                  {
                  pTP->SigResults.dwDownloadedSignatureSets++;    // Report event.
                  pTP->SigResults.iDownloadedBytes += cdCallbackData.dwBytesReceivedSoFar;
                  }

               break;


            case  TG_RETCODE_USER_ABORT:
               pTP->SigResults.iDownloadedBytes += cdCallbackData.dwBytesReceivedSoFar;

               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               printf ("\n[%04X]D: Terminating signature downloader thread per user's request.", pTP->uThreadID);
               textattr (g_wOrigAttrs);
               break;


            case  TG_RETCODE_TIMEOUT   :
               // We have to report
               // this as a timeout condition - incomplete data received from server - no
               // disconnect within specified timeout.
               pTP->SigResults.dwTimeouts++; // Report event.

               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               printf ("\n[%04X]D: Warning: timeouted after %dms of waiting for server to complete reply.",
                        pTP->uThreadID, QueryTimer (cdCallbackData.dwStartTimerValue));
               textattr (g_wOrigAttrs);
               break;


            case  TG_RETCODE_ERROR     :
            default:
               bProblemDownloadingSigSet = TRUE;
               iWSALastError = tisStructure.iWSALastErrorCode;
               strcpy (acErrorMsg, tisStructure.acErrorText);

               AddErrorToArray (
                  pTP->SigResults.dwCriticalErrors,   // DWORD    ppdwErrorArray[][2],
                  tisStructure.iWSALastErrorCode      // DWORD    dwErrorID,
                  // int      iSizeOfArray   /* = CRITICAL_ERROR_IDS_MAX */,
                  // DWORD    dwErrorsNumber /* = 1 */
                  );

               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               if (tisStructure.iWSALastErrorCode == WSAECONNREFUSED)
                  printf ("\n[%04X]D: *** Critical error *** %d = Connection refused.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAETIMEDOUT)
                  printf ("\n[%04X]D: *** Critical error *** %d = Timeout waiting for connection.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAECONNRESET)
                  printf ("\n[%04X]D: *** Critical error *** %d = Connection reset by peer.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAHOST_NOT_FOUND)
                  printf ("\n[%04X]D: *** Critical error *** %d = Host not found.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else
                  printf ("\n[%04X]D: *** Critical error *** %d.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               textattr (g_wOrigAttrs);
               break;
            }

         // Release allocated resources.
         Free_TransactWithServer (tisStructure.pTransactData);
         }
      while (bRedirected);


      if (dwRetCode == TG_RETCODE_ERROR)
         {
         dwCriticalErrors ++;

         // if 'pTP->iSigErrorsMax' < 0 - then unlimited number of errors allowed.
         if (pTP->iSigErrorsMax >= 0 && (dwCriticalErrors > (DWORD)pTP->iSigErrorsMax))
            goto  ERROR_EXIT;

         // Execute optional delay.
         if (pTP->iSigDelay)
            Sleep (pTP->iSigDelay);

         continue;
         }


      if (dwRetCode == TG_RETCODE_USER_ABORT)
         break;

      // Execute optional delay.
      if (pTP->iSigDelay)
         Sleep (pTP->iSigDelay);
      }
#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while (1);
#pragma  warning (default : 4127)



// #define  TG_RETCODE_SUCCESS      0           //  - Server disconnected. Possibly some data retrieved. No errors.
// #define  TG_RETCODE_USER_ABORT   1           //  - Callback returned FALSE.
// #define  TG_RETCODE_TIMEOUT      2           //  - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
// #define  TG_RETCODE_ERROR        0xFFFFFFFF  //  - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->acErrorText' hass error text.
//
//
// #define  TG_CALLBACK_EVENT_CONNECTED            1
// #define  TG_CALLBACK_EVENT_REQUEST_DATA        2
// #define  TG_CALLBACK_EVENT_DATA_SENT            3
// #define  TG_CALLBACK_EVENT_DATA_RECEIVED        4
// #define  TG_CALLBACK_EVENT_NO_DATA_READY        5
//
// typedef struct _TRANSACT_INFO_STRUCTURE
// {
//    char           acTargetNameOrIP [MAX_PATH];
//    unsigned short usTargetPort;
//    BOOL           bUseSSL;
//    SSL_CTX        *pSslContext;                 // Used for SSL connections.
//    DWORD          dwTotalTimeout;               // INFINITE|(ms) Stay in this function max. this time.
//    DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
//    BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.
//
//    BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
//    DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
//    char           *pTransactData;               // Multipurpose I/O buffer.
//    int            iTransactDataSize;            // I/O buffer size.
//    char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
//    int            iWSALastErrorCode;            // last WSA error.
//
//    VOID           *pvUserValue;                 // Caller's-defined value;
//    DWORD          dwUserValue;                  // Caller's-defined value;
// } TRANSACT_INFO_STRUCTURE;

// typedef struct _ALL_THREAD_PARAMS
// {
//    HANDLE               hThread;
//    unsigned             uThreadID;                    // GetCurrentThreadId();
//    DWORD                dwThreadType;                 // THREAD_TYPE_*
//
//    char                 acGatewayNameOrIP [MAX_PATH]; // '/gateway='. navajo.watson.ibm.com. From command line or from .INI file.
//    unsigned short       usGatewayPort;                // '/port='. 2847. From command line or from .INI file.
//    BOOL                 bUseSSL;                      // '/ssl'.
//    SSL_CTX              *pSslContext;                 // Used for SSL connections.
//    BOOL                 bZapBits;                     // '/zapbits' - randomly change file contents before submitting file.
//    BOOL                 bZapNames;                    // '/zapnames' - randomly change file name before submitting file.
//    int                  iMaxFilesPerSamThread;        // -1:as many as found, else use this number.
//    int                  iMaxSigsPerSigThread;         // -1:infinitely in a loop, else use this number.
//    int                  iSamDelay;                    // ms. of delay between file submissions or between retries after error occured.
//    int                  iSamErrorsMax;                // Allow this number of critical errors per Sam thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
//    BOOL                 bLoop;                        // '/loop'. Do sample submission is a loop until key pressed.
//    BOOL                 bVerbose;                     // '/verbose'. Log HTTP transaction data to screen.
//    BOOL                 bManualSample;                // '/sammanual'. Set X-Sample-Reason to 'manual'
//    BOOL                 bSkipDownload;                // '/sigskipdownload'. Skip signature def download
//
//    FILE_INFO            **ppFileInfos;                // FILE_INFO   **ppFileInfos: Array of pointers to 'FILE_INFO' structure.
//    int                  iFileInfosNum;                // int         iFileInfosNum: Number of elements in array.
//
//    CONSTANT_INFO        *pConstantInfo;               // Needed for 'Allocate_HTTP_Header_For_SubmitSample()'.
//
//    int                  iSigDelay;                    // ms. of delay between downloads of blessed signature sets or between retries after error occured.
//    int                  iSigErrorsMax;                // Allow this number of critical errors per Sig thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
//
//    int                  iMinDelay;
//    int                  iMaxDelay;
//    int                  iSeed;
//
//    SAM_THREAD_RESULTS   SamResults;
//    SIG_THREAD_RESULTS   SigResults;
//
//    HANDLE               hEventTerminating;            // Will be set into signalled state if all termination needed.
// } ALL_THREAD_PARAMS;



EXIT:
   textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
   printf ("\n[%04X]D: Signature Downloader thread terminating...", pTP->uThreadID);
   textattr (g_wOrigAttrs);

   _endthreadex (0);
   return 0;   // To avoid warning.


ERROR_EXIT:
   // Here 'iWSALastError' and 'acErrorMsg' must already be initialized.
   textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
   printf ("\n[%04X]D: %s. Error: %d. Aborting thread...", pTP->uThreadID, acErrorMsg, iWSALastError);
   textattr (g_wOrigAttrs);
   goto  EXIT;
}
//===========================================================================

//===========================================================================
//
// Returns: TRUE:  continue processing.
//          FALSE: abort requested.

BOOL  Callback_DownloadSigSet (TRANSACT_INFO_STRUCTURE *pTIS)
{
//---------------------------------------------------------------------------
   DOWNLOAD_SIGSET_CALLBACK_DATA    *pCallbackData = (DOWNLOAD_SIGSET_CALLBACK_DATA *)pTIS->pvUserValue;

   BOOL                             bRetCode = TRUE;
   DWORD                            dwCurrentTime;
//---------------------------------------------------------------------------

// typedef struct _DOWNLOAD_SIGSET_CALLBACK_DATA
// {
//    ALL_THREAD_PARAMS    *pTP;
//    DWORD                dwBytesReceivedSoFar;
//    DWORD                dwStartTimerValue;
//    BOOL                 bAboutToSendVeryFirstDataChunk;
//    BOOL                 bReceivedVeryFirstDataChunk;
//    DWORD                dwTotalSizeOfDataToExpect;
//    char                 *pDataBuffer;
//    int                  iDataBufferSize;
// } DOWNLOAD_SIGSET_CALLBACK_DATA;

   dwCurrentTime = QueryTimer (pCallbackData->dwStartTimerValue);

   switch (pTIS->dwEventID)
      {
      case  TG_CALLBACK_EVENT_CONNECTED      :
         printf ("\n[%04X]D: Connected to: %s : %d", pCallbackData->pTP->uThreadID, pTIS->acTargetNameOrIP, pTIS->usTargetPort);
         bRetCode = TRUE;
         break;


      case  TG_CALLBACK_EVENT_REQUEST_DATA  :
         if (pCallbackData->bAboutToSendVeryFirstDataChunk)
            {
            pCallbackData->bAboutToSendVeryFirstDataChunk = FALSE;
            pTIS->pTransactData      = pCallbackData->pDataBuffer;
            pTIS->iTransactDataSize  = pCallbackData->iDataBufferSize;
            }
         else
            {
            pTIS->pTransactData      = NULL;
            pTIS->iTransactDataSize  = 0;
            }
         bRetCode = TRUE;
         break;


      case  TG_CALLBACK_EVENT_DATA_SENT      :
         printf ("\n[%04X]D: Sent request for blessed signature set. Waiting for reply ...", pCallbackData->pTP->uThreadID);
         bRetCode = TRUE;
         break;


      case  TG_CALLBACK_EVENT_DATA_RECEIVED  :
         pCallbackData->dwBytesReceivedSoFar += pTIS->iTransactDataSize;

         if (pCallbackData->bReceivedVeryFirstDataChunk)
            {
            // Here we want to printf very first chunk of data.
            pCallbackData->bReceivedVeryFirstDataChunk = FALSE;
            if (pCallbackData->pTP->bVerbose) PrintfStreamingData (pTIS->pTransactData, pTIS->iTransactDataSize, FOREGROUND_GREEN + FOREGROUND_INTENSITY);

            // Determine the total size of binary signature data to follow...

            BOOL     bFound;
            char     acKeyName [] = "Content-Length";
            char     acKeyData [20];
            bFound =
               FindKey (
                  pTIS->pTransactData,     // const char  *pBuffer:         Where to search for.
                  pTIS->iTransactDataSize, // int         iBufferSize:      Size of buffer.
                  acKeyName,              // const char  *pKeyName:        ASCIIZ name of key.
                  ':',                    // char        cSeparator
                  FALSE,                  // BOOL        bCaseSensitive:   TRUE:case sensitive
                  acKeyData,              // char        *pKeyValueStr:    Where to return key value.
                  sizeof (acKeyData)      // int         iKeyValueStrSize: Size of destination buffer
                  );
            if (bFound)
               // Found key!
               pCallbackData->dwTotalSizeOfDataToExpect = atoi (acKeyData);
            else
               {
               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               printf ("\n[%04X]D: Warning: Cannot find valid '%s' key value in incoming stream of data",
                  pCallbackData->pTP->uThreadID,
                  acKeyName
                  );
               }
            pTIS->bDumpData = FALSE;
            }
         else
            {
            pTIS->bDumpData = TRUE;
            }

         // Printf in-progress
         if (pCallbackData->pTP->bVerbose) // WCA
         {
         __int64  i64 =  pCallbackData->dwBytesReceivedSoFar;
         i64 *= 1000;
         i64 /= dwCurrentTime;

         printf ("\n[%04X]D: Received %d of %d bytes of signature set for %3d.%03d sec (%d bytes/sec) ...",
               pCallbackData->pTP->uThreadID,
               pCallbackData->dwBytesReceivedSoFar,
               pCallbackData->dwTotalSizeOfDataToExpect,
               dwCurrentTime/1000,
               dwCurrentTime%1000,
               (DWORD) i64
               );
         }

         break;


      case  TG_CALLBACK_EVENT_NO_DATA_READY  :
         // Printf in-progress
         if (pCallbackData->pTP->bVerbose) // WCA
         {
         __int64  i64 =  pCallbackData->dwBytesReceivedSoFar;
         i64 *= 1000;
         i64 /= dwCurrentTime;
         printf ("\n[%04X]D: Waiting for more data. So far %d bytes received. %3d.%03d sec passed (%d bytes/sec) ...",
               pCallbackData->pTP->uThreadID,
               pCallbackData->dwBytesReceivedSoFar,
               dwCurrentTime/1000,
               dwCurrentTime%1000,
               (DWORD) i64
               );
         }
         bRetCode = TRUE;
         break;


      default                                :
         bRetCode = FALSE;
         break; // Unknown event ID - debug it!
      }

   // Check for user abort condition.
   if (WaitForSingleObject (pCallbackData->pTP->hEventTerminating, 0) != WAIT_TIMEOUT)
      bRetCode = FALSE;


   return (bRetCode);

// #define  TG_RETCODE_SUCCESS      0           //  - Server disconnected. Possibly some data retrieved. No errors.
// #define  TG_RETCODE_USER_ABORT   1           //  - Callback returned FALSE.
// #define  TG_RETCODE_TIMEOUT      2           //  - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
// #define  TG_RETCODE_ERROR        0xFFFFFFFF  //  - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->acErrorText' has error text.
//
//
// #define  TG_CALLBACK_EVENT_CONNECTED            1
// #define  TG_CALLBACK_EVENT_REQUEST_DATA        2
// #define  TG_CALLBACK_EVENT_DATA_SENT            3
// #define  TG_CALLBACK_EVENT_DATA_RECEIVED        4
// #define  TG_CALLBACK_EVENT_NO_DATA_READY        5
//
// typedef struct _TRANSACT_INFO_STRUCTURE
// {
//    char           acTargetNameOrIP [MAX_PATH];
//    unsigned short usTargetPort;
//    BOOL           bUseSSL;
//    SSL_CTX        *pSslContext;                 // Used for SSL connections.
//    DWORD          dwTotalTimeout;               // INFINITE|(ms) Stay in this function max. this time.
//    DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
//    BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.
//
//    BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
//    DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
//    char           *pTransactData;               // Multipurpose I/O buffer.
//    int            iTransactDataSize;            // I/O buffer size.
//    char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
//    int            iWSALastErrorCode;            // last WSA error.
//
//    VOID           *pvUserValue;                 // Caller's-defined value;
//    DWORD          dwUserValue;                  // Caller's-defined value;
// } TRANSACT_INFO_STRUCTURE;

}
//===========================================================================

//===========================================================================
//
// Thread will query gateway for status of the given cookie number.
/***
Parameters, used by this thread:
--------------------------------
/sqthreads=
/mincookie= (default 1)
/maxcookie= (default 0x7FFFFFFF)
/maxsqs=  - maximal number of status queries per SQ thread before thread terminates.
/sqerrorsmax=
/sqdelay=

***/

unsigned __stdcall   StatusQuery_Thread (VOID *pvThreadParm)
{
//---------------------------------------------------------------------------
   ALL_THREAD_PARAMS   *pTP = (ALL_THREAD_PARAMS *)pvThreadParm;

   char                 acRequestFormat [] =
                           "GET /AVIS/getSampleStatus HTTP/1.0"
                           "\nX-Sample-Checksum: %s"   // 00000123
                           "\n\n";
   char                 acRequestData [sizeof(acRequestFormat) * 2];

   char                 acErrorMsg [512]; // Non-CRLF-ed error description.
   int                  iWSALastError = 0;

   TRANSACT_INFO_STRUCTURE  tisStructure;

   int                  iCurrentIndex;

   // We will use these values as working values, to accomodate cases where redirection
   // happens.
   BOOL                 bRedirected;

   char                 acTargetNameOrIP [MAX_PATH];           // Machine name or IP.
   char                 acTargetDirectoryOnGateway [MAX_PATH];
   unsigned short       usTargetPort;
   BOOL                 bUseSSL;
   char                 *pTmpPtr;


   DWORD                dwCriticalErrors = 0;
   DWORD                dwStartTimer;
   DWORD                dwTimerValue;
   int                  iCounter;

   DWORD                dwRetCode;
//---------------------------------------------------------------------------


   iCounter = pTP->iMaxSQsPerSQThread;

   // Initiate random number generator with the requested seed value.
   GetRandomNumber   (0, 1, pTP->iSeed);

   if (!pTP->iSQCheckSumsNum)
      {
      textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
      printf ("\n[%04X]S: Array of checksums to query is empty. Nothing to do. Leaving...", pTP->uThreadID);
      textattr (g_wOrigAttrs);
      goto  EXIT;
      }

   do
      {
      // Check for terminating conditions at the beginning of each loop.
      if (WaitForSingleObject (pTP->hEventTerminating, 0) != WAIT_TIMEOUT)
         {
         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\n[%04X]S: Terminating per user's request", pTP->uThreadID);
         textattr (g_wOrigAttrs);
         break;
         }

      if (pTP->iMaxSQsPerSQThread == -1)
         ;   // Do infinitely.
      else
         {
         iCounter --;

         if (iCounter < 0)
            break;
         }

      // Execute random delay, if requested.
      if (pTP->iMinDelay || pTP->iMaxDelay)
         {
         int   iDelay = GetRandomNumber (pTP->iMinDelay, pTP->iMaxDelay);
         printf ("\n[%04X]S: Sleeping for %d ms...", pTP->uThreadID, iDelay);
         Sleep (iDelay);
         }

//*  char                 **ppSQCheckSums;              // Array of ASCIIZ md5 crc: "FB40A7....8C" used by SQ thread to query status.
//*  int                  iSQCheckSumsNum;              // number of elements in array.

      // Randomly choose the next CRC.
      iCurrentIndex = GetRandomNumber (0, pTP->iSQCheckSumsNum - 1);
      sprintf (acRequestData, acRequestFormat, pTP->ppSQCheckSums [iCurrentIndex]);

      strcpy (acTargetNameOrIP, pTP->acGatewayNameOrIP);
      strcpy (acTargetDirectoryOnGateway, "/AVIS/getSampleStatus");
      usTargetPort = pTP->usGatewayPort;
      bUseSSL      = pTP->bUseSSL;

      bRedirected = FALSE;

      do
         {
         memset (&tisStructure, 0, sizeof (tisStructure));
         strcpy (tisStructure.acTargetNameOrIP, acTargetNameOrIP);
         tisStructure.usTargetPort      = usTargetPort;
         tisStructure.bUseSSL           = bUseSSL;
         tisStructure.pSslContext       = pTP->pSslContext; // Used for SSL connections.
         tisStructure.pTransactData     = acRequestData;          // No callback. Data to send already initialized.
         tisStructure.iTransactDataSize = strlen (acRequestData);
         tisStructure.pfn_EventCallback = NULL;
         tisStructure.dwTotalTimeout    = 240*1000; // 4 minutes to get reply about the latest blesses signature set ID.
         tisStructure.dwCallbackTimeout = tisStructure.dwTotalTimeout;  // Callback is NULL - no reason to have other callback timeout.
         tisStructure.bDumpData         = FALSE;
         // Remaining values we don't need to initialize.

         printf ("\n[%04X]S: Sending request to gateway to retrieve status for checksum: %s. Wait...", pTP->uThreadID, pTP->ppSQCheckSums [iCurrentIndex]);
         if (pTP->bVerbose) PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_RED + FOREGROUND_GREEN + FOREGROUND_INTENSITY);

         dwStartTimer = StartTimer();

         // Returns: TG_RETCODE_[SUCCESS|USER_ABORT|TIMEOUT|ERROR].
         dwRetCode = Allocate_TransactWithServer (&tisStructure);

         pTP->SQResults.dwRequestsSubmitted ++;   // Record event.

// typedef struct _SQ_THREAD_RESULTS
// {
//    DWORD                dwRequestsSubmitted;
//    DWORD                dwRepliesReceived;
//    DWORD                dwTimeouts;
//    DWORD                dwCriticalErrors [CRITICAL_ERROR_IDS_MAX][2];  // [i][0] - error code, [i][1] - number of errors for that code. Last element - other errors.
// } SQ_THREAD_RESULTS;

         switch (dwRetCode)
            {
            case  TG_RETCODE_SUCCESS   :
               if (pTP->bVerbose) PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_GREEN + FOREGROUND_INTENSITY);

               // Checking for redirection here. Init 'bRedirected' if TRUE and all vars are initialized.
               pTmpPtr =
                  StrStr  (
                     tisStructure.pTransactData,      // const char  *pBuffer,
                     g_acRedirectionSearchPattern,    // const char  *pString,
                     tisStructure.iTransactDataSize,  // int         iBufferSize,
                     FALSE,                           // BOOL        bCaseSensitive,
                     TRUE,                            // BOOL        bSpecSearch /*=FALSE*/,   // TRUE:'?' and '*' considered as spec, not as 'pString' part.
                     NULL                             // int         *piFoundLen /*=NULL */
                     );
               if (pTmpPtr)
                  {
                  // Redirection directive in HTTP reply detected. Parse it to try to figure out where is redirection is to.

                  // Possible format of 'Location' reply:
                  // 'Location: https://another-one.watson.ibm.com:2847/AVIS/postSuspectSample'
                  // 'Location: http://another-one.watson.ibm.com/AVIS/postSuspectSample'

                  // If function returns TRUE - redirection data is initialized.
                  bRedirected =
                     ParseNewLocationData (
                        tisStructure.pTransactData,      // const char     *pLocationStrBuffer,    // input - buffer with data, where "Location" text is.
                        tisStructure.iTransactDataSize,  // int            iLocationStrBufferSize, // input : int - size of above buffer.
                        acTargetNameOrIP,                // char           *pHostNameOrIP,      // output: must be at least MAX_PATH long.
                        &usTargetPort,                   // unsigned short *pusPortNumber,      // output:
                        acTargetDirectoryOnGateway,      // char           *pDirectoryName,     // output: must be at least MAX_PATH long.
                        &bUseSSL                         // BOOL           *bUseSSL             // output.
                        );
                  if (bRedirected)
                     {
                     textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
                     printf ("\n[%04X]S: Request will be redirected to location: http%s://%s:%d%s",
                           pTP->uThreadID,
                           bUseSSL?"s":"",
                           acTargetNameOrIP,
                           (int)usTargetPort,
                           acTargetDirectoryOnGateway
                           );
                     textattr (g_wOrigAttrs);
                     }
                  }
               else
                  {
                  pTP->SQResults.dwRepliesReceived ++;   // Record event.
                  bRedirected = FALSE;
                  }

               break;


            case  TG_RETCODE_USER_ABORT:  // Cannot be here actually at that stage.
               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               printf ("\n[%04X]S: Terminating status query thread per user's request.", pTP->uThreadID);
               textattr (g_wOrigAttrs);
               Free_TransactWithServer (tisStructure.pTransactData);
               break;

            case  TG_RETCODE_TIMEOUT   :
               // We have to report
               // this as a timeout condition - incomplete data received from server - no
               // disconnect within specified timeout.
               dwTimerValue = QueryTimer(dwStartTimer);

               pTP->SQResults.dwTimeouts++; // Report event.

               textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
               printf ("\n[%04X]S: Warning: timeouted after %d.%d sec of waiting for server to complete reply. Let's see what we have...",
                        pTP->uThreadID,
                        dwTimerValue / 1000,
                        dwTimerValue % 1000
                        );
               textattr (g_wOrigAttrs);
               if (pTP->bVerbose) PrintfStreamingData (tisStructure.pTransactData, tisStructure.iTransactDataSize, FOREGROUND_GREEN);
               break;

            case  TG_RETCODE_ERROR     :
            default:
               iWSALastError = tisStructure.iWSALastErrorCode;
               strcpy (acErrorMsg, tisStructure.acErrorText);

               AddErrorToArray (
                  pTP->SQResults.dwCriticalErrors,   // DWORD    ppdwErrorArray[][2],
                  tisStructure.iWSALastErrorCode      // DWORD    dwErrorID,
                  // int      iSizeOfArray   /* = CRITICAL_ERROR_IDS_MAX */,
                  // DWORD    dwErrorsNumber /* = 1 */
                  );

               // Release allocated resources.
               Free_TransactWithServer (tisStructure.pTransactData);

               if (tisStructure.iWSALastErrorCode == WSAECONNREFUSED)
                  printf ("\n[%04X]S: *** Critical error *** %d = Connection refused.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAETIMEDOUT)
                  printf ("\n[%04X]S: *** Critical error *** %d = Timeout waiting for connection.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAECONNRESET)
                  printf ("\n[%04X]S: *** Critical error *** %d = Connection reset by peer.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else if (tisStructure.iWSALastErrorCode == WSAHOST_NOT_FOUND)
                  printf ("\n[%04X]S: *** Critical error *** %d = Host not found.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               else
                  printf ("\n[%04X]S: *** Critical error *** %d.", pTP->uThreadID, tisStructure.iWSALastErrorCode);
               break;
            }


         if (dwRetCode == TG_RETCODE_SUCCESS && bRedirected)
            // Deallocate resources before next redirection loop.
            Free_TransactWithServer  (
               tisStructure.pTransactData      // char     **ppData,           // Ptr to allocated buffer with data, as it was passed to 'Allocate_TransactWithServer()'.
               );
         }
      while (bRedirected);


      if (dwRetCode == TG_RETCODE_ERROR)
         {
         dwCriticalErrors ++;

         // if 'pTP->iSQErrorsMax' < 0 - then unlimited number of errors allowed.
         if (pTP->iSQErrorsMax >= 0 && (dwCriticalErrors > (DWORD)pTP->iSQErrorsMax))
            {
            printf ("\n[%04X]S: Too many critical errors (%d). Aborting thread...", pTP->uThreadID, dwCriticalErrors);
            goto  ERROR_EXIT;
            }

         printf ("\n[%04X]S: So far encountered %d critical errors. Continuing...", pTP->uThreadID, dwCriticalErrors);
         // Execute optional delay.
         if (pTP->iSQDelay)
            Sleep (pTP->iSQDelay);

         continue;
         }


      if (dwRetCode == TG_RETCODE_USER_ABORT)
         break;


      // Deallocate resources now.
      Free_TransactWithServer  (
         tisStructure.pTransactData      // char     **ppData,           // Ptr to allocated buffer with data, as it was passed to 'Allocate_TransactWithServer()'.
         );
      }
#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while (1);
#pragma  warning (default : 4127)



// #define  TG_RETCODE_SUCCESS      0           //  - Server disconnected. Possibly some data retrieved. No errors.
// #define  TG_RETCODE_USER_ABORT   1           //  - Callback returned FALSE.
// #define  TG_RETCODE_TIMEOUT      2           //  - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
// #define  TG_RETCODE_ERROR        0xFFFFFFFF  //  - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->acErrorText' hass error text.
//
//
// #define  TG_CALLBACK_EVENT_CONNECTED            1
// #define  TG_CALLBACK_EVENT_REQUEST_DATA        2
// #define  TG_CALLBACK_EVENT_DATA_SENT            3
// #define  TG_CALLBACK_EVENT_DATA_RECEIVED        4
// #define  TG_CALLBACK_EVENT_NO_DATA_READY        5
//
// typedef struct _TRANSACT_INFO_STRUCTURE
// {
//    char           acTargetNameOrIP [MAX_PATH];
//    unsigned short usTargetPort;
//    BOOL           bUseSSL;
//    SSL_CTX        *pSslContext;                 // Used for SSL connections.
//    DWORD          dwTotalTimeout;               // INFINITE|(ms) Stay in this function max. this time.
//    DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
//    BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.
//
//    BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
//    DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
//    char           *pTransactData;               // Multipurpose I/O buffer.
//    int            iTransactDataSize;            // I/O buffer size.
//    char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
//    int            iWSALastErrorCode;            // last WSA error.
//
//    VOID           *pvUserValue;                 // Caller's-defined value;
//    DWORD          dwUserValue;                  // Caller's-defined value;
// } TRANSACT_INFO_STRUCTURE;

// typedef struct _ALL_THREAD_PARAMS
// {
//    HANDLE               hThread;
//    unsigned             uThreadID;                    // GetCurrentThreadId();
//    DWORD                dwThreadType;                 // THREAD_TYPE_*
//
//    char                 acGatewayNameOrIP [MAX_PATH]; // '/gateway='. navajo.watson.ibm.com. From command line or from .INI file.
//    unsigned short       usGatewayPort;                // '/port='. 2847. From command line or from .INI file.
//    BOOL                 bUseSSL;                      // '/ssl'.
//    SSL_CTX              *pSslContext;                 // Used for SSL connections.
//    BOOL                 bZapBits;                     // '/zapbits' - randomly change file contents before submitting file.
//    BOOL                 bZapNames;                    // '/zapnames' - randomly change file name before submitting file.
//    int                  iMaxFilesPerSamThread;        // -1:as many as found, else use this number.
//    int                  iMaxSigsPerSigThread;         // -1:infinitely in a loop, else use this number.
//    int                  iSamDelay;                    // ms. of delay between file submissions or between retries after error occured.
//    int                  iSamErrorsMax;                // Allow this number of critical errors per Sam thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
//    BOOL                 bLoop;                        // '/loop'. Do sample submission is a loop until key pressed.
//    BOOL                 bVerbose;                     // '/verbose'. Log HTTP transaction data to screen.
//    BOOL                 bManualSample;                // '/sammanual'. Set X-Sample-Reason to 'manual'
//    BOOL                 bSkipDownload;                // '/sigskipdownload'. Skip signature def download
//
//    FILE_INFO            **ppFileInfos;                // FILE_INFO   **ppFileInfos: Array of pointers to 'FILE_INFO' structure.
//    int                  iFileInfosNum;                // int         iFileInfosNum: Number of elements in array.
//
//    CONSTANT_INFO        *pConstantInfo;               // Needed for 'Allocate_HTTP_Header_For_SubmitSample()'.
//
//    int                  iSigDelay;                    // ms. of delay between downloads of blessed signature sets or between retries after error occured.
//    int                  iSigErrorsMax;                // Allow this number of critical errors per Sig thread, exit after that. -1:infinite. 0:exit upon first critical error. >0:use this number.
//
//    int                  iMinDelay;
//    int                  iMaxDelay;
//    int                  iSeed;
//
//    SAM_THREAD_RESULTS   SamResults;
//    SIG_THREAD_RESULTS   SigResults;
//
//    HANDLE               hEventTerminating;            // Will be set into signalled state if all termination needed.
// } ALL_THREAD_PARAMS;



EXIT:
   textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
   printf ("\n[%04X]S: Status querier thread terminating...", pTP->uThreadID);
   textattr (g_wOrigAttrs);

   _endthreadex (0);
   return 0;   // To avoid warning.


ERROR_EXIT:
   // Here 'iWSALastError' and 'acErrorMsg' must already be initialized.
   textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
   printf ("\n[%04X]S: %s. Error: %d. Aborting thread...", pTP->uThreadID, acErrorMsg, iWSALastError);
   textattr (g_wOrigAttrs);
   goto  EXIT;
}
//===========================================================================

//===========================================================================
//
// 'pvHandle' is the handle, used to scramble buffer in multiple chunks.
// 'pvHandle' contains information, required for continuation of scrambling.
// For the first time (or single) call - pass NULL.
//
// Scrambles buffer with "xor-vampish" algorithm.
// (c) Alla Segal, All Rights Reserved. :)
//
// Derived from SampleUnScrambler.cpp

const char g_scrambleMask [] =
   { 'Y', 'A', 'K', 'V', 'A', 'M', 'P', 'I',
     'S', 'H', 'U', 'C', 'H', 'E', 'G', 'O',
     'Z', 'H', 'E', 'B', 'O', 'L', 'E', '.',
     '.'
   };

VOID  *ScrambleBuffer (
         VOID     *pvHandle,     // NULL:first time run, else:Scramble handle, returned from prev. call.
         char     *pBuffer,
         DWORD    dwBufferSize
         )
{
//---------------------------------------------------------------------------
   int   i, scrambleMaskSize;
   int   iMaskIndex = (int)pvHandle;
//---------------------------------------------------------------------------

   scrambleMaskSize = sizeof (g_scrambleMask) / sizeof (g_scrambleMask[0]);

   for (i=0; i<(int)dwBufferSize; i++, iMaskIndex++)
      {
      if (iMaskIndex == scrambleMaskSize)
         iMaskIndex = 0;
      pBuffer[i] ^= g_scrambleMask [iMaskIndex];
      }

   return (VOID *)iMaskIndex;
}
//===========================================================================

//===========================================================================
//
// Allocate complete request, uncluding file data.
// User 'Free_HTTP_Request()' to free memory, allocated by request.

#define APPEND_TO(x) ((x)+strlen(x))

VOID  Allocate_HTTP_Header_For_SubmitSample (
         const char           *pTargetNameOrIP,       // Machine name or IP.
         const char           *pTargetDirectoryOnGateway,   // "/AVIS/postSuspectSample" or "AVIS/postSuspectSample"(will be prepended with '/' then).
         unsigned short       usTargetPort,
         BOOL                 bUseSSL,

         const CONSTANT_INFO  *pConstantInfo,
         const FILE_INFO      *pFileInfo,
         char                 *pUseAsFilename,        // Use this as a filename in case user mangling filenames.
         const char           *pPlainCRCString,
         const char           *pCryptedCRCString,
         char                 **ppAllocatedRequest,   // - output - ptr to allocated request.
         int                  *piAllocatedBufferSize  // - output - ptr to size of allocated request buffer.
         )
{
//---------------------------------------------------------------------------
   char           *pRequest;
   char           *pFullRequest;
   char           acCurrentDateTimeString [100];
   char           acDateTimeString [100];
   int            iFullRequestSize;
//---------------------------------------------------------------------------

   bUseSSL;

   // Getting current date/time...
   CurrentDateTimeToString3 (acCurrentDateTimeString);

   // Allocate 10K for the request.
   pRequest = new char [10*1024];
   pRequest[0] = 0;

// // Structure keeps information about file.
// typedef struct _FILE_INFO
// {
//    char        *pFullFilename;   // Dynamically allocated.
//
//    // These are copied from 'WIN32_FIND_DATA' structure.
//    DWORD       dwFileAttributes;
//    FILETIME    ftCreationTime;
//    FILETIME    ftLastAccessTime;
//    FILETIME    ftLastWriteTime;
//    DWORD       nFileSizeHigh;
//    DWORD       nFileSizeLow;
// } FILE_INFO;



// // typedef struct _CONSTANT_INFO
// {
//    char        acLocalMachineName      [MAX_COMPUTERNAME_LENGTH + 1];// GetComputerName()
//    char        acLocalUserName         [MAX_PATH];                   // GetUserName(): Currently logged on user.
//    char        acGatewayNameOrIP       [MAX_PATH];                   // navajo.watson.ibm.com. From command line or from .INI file.
//    int         iGatewayPort;                                         // 2847. From command line or from .INI file.
//    char        acRegisteredSystemOwner [MAX_PATH];                   // "HKLM/Software/Microsoft/WindowsNT/CurrentVersion"
//    char        acProcessorName         [200];                        // "HKLM/HARDWARE/DESCRIPTION/System/CentralProcessor/0/ *
//    char        acPlatformScanner       [200];                        //  Norton anti-virus or TESTGATE.
//    char        acOperatingSystemName   [200];                        // GetVersionEx();
//    char        acCustomerContactName   [200];
//    char        acCustomerContactEmail  [200];
//    char        acCustomerContactPhone  [200];
//    char        acCustomerIdentifier    [200];
//
//    char        acPlatformGUID          [40];
//    char        acSampleUUID            [40];
//
//    char        acConnectionType        [80];
//    char        acContentType           [80];
//    char        acAnalysisState         [80];
//    char        acChecksumMethod        [80];
//    char        acContentCompression    [80];
//    char        acContentScrambling     [80];
//    char        acIcePackInitializedState [80];
//    char        acPlatformLanguage      [80];
//    char        acScanResult            [80];
//    int         iSampleChanges;
//    int         iSampleFileID;
//    int         iSamplePriority;
//    char        acSampleReason          [80];
//    int         iSampleStatus;
//    char        acSampleStatusReadable  [80];
//    int         iSampleSubmissionRoute;
//    char        acSampleType            [80];
//    int         iScanSignaturesSequence;
//    char        acScanSignaturesVersion [80];
// } CONSTANT_INFO;


   sprintf (APPEND_TO(pRequest), "POST %s%s HTTP/1.0\n", pTargetDirectoryOnGateway[0]=='/'?"":"/", pTargetDirectoryOnGateway);
//C   sprintf (APPEND_TO(pRequest), "POST /AVIS/postSuspectSample HTTP/1.0\n");
   sprintf (APPEND_TO(pRequest), "Connection: %s\n",                 pConstantInfo->acConnectionType);
   sprintf (APPEND_TO(pRequest), "Content-Type: %s\n",               pConstantInfo->acContentType);
   sprintf (APPEND_TO(pRequest), "Content-Length: %d\n",             pFileInfo->nFileSizeLow);
   sprintf (APPEND_TO(pRequest), "X-Analysis-State: %s\n",           pConstantInfo->acAnalysisState);
   sprintf (APPEND_TO(pRequest), "X-Checksum-Method: %s\n",          pConstantInfo->acChecksumMethod);
   sprintf (APPEND_TO(pRequest), "X-Content-Checksum: %s\n",         pCryptedCRCString);
// sprintf (APPEND_TO(pRequest), "X-Content-Compression: %s\n",      pConstantInfo->acContentCompression);
   sprintf (APPEND_TO(pRequest), "X-Content-Scrambling: %s\n",       pConstantInfo->acContentScrambling);
   sprintf (APPEND_TO(pRequest), "X-Customer-Contact-Email: %s\n",   pConstantInfo->acCustomerContactEmail);
   sprintf (APPEND_TO(pRequest), "X-Customer-Contact-Name: %s\n",    pConstantInfo->acCustomerContactName);
   sprintf (APPEND_TO(pRequest), "X-Customer-Contact-Telephone: %s\n",pConstantInfo->acCustomerContactPhone);
   sprintf (APPEND_TO(pRequest), "X-Customer-Identifier: %s\n",      pConstantInfo->acCustomerIdentifier);

   FileTimeToString3 (&pFileInfo->ftLastAccessTime, acDateTimeString);
   sprintf (APPEND_TO(pRequest), "X-Date-Accessed: %s\n",            acDateTimeString);

   FileTimeToString3 (&pFileInfo->ftCreationTime, acDateTimeString);
   sprintf (APPEND_TO(pRequest), "X-Date-Created: %s\n",             acDateTimeString);

   FileTimeToString3 (&pFileInfo->ftLastWriteTime, acDateTimeString);
   sprintf (APPEND_TO(pRequest), "X-Date-Modified: %s\n",            acDateTimeString);

   sprintf (APPEND_TO(pRequest), "X-Date-Quarantined: %s\n",         acCurrentDateTimeString);
   sprintf (APPEND_TO(pRequest), "X-Date-Submitted: %s\n",           acCurrentDateTimeString);
   sprintf (APPEND_TO(pRequest), "X-IcePack-Initialized: %s\n",      pConstantInfo->acIcePackInitializedState);
   sprintf (APPEND_TO(pRequest), "X-Platform-Computer: %s\n",        pConstantInfo->acLocalMachineName);
   sprintf (APPEND_TO(pRequest), "X-Platform-Distributor: %s\n",     g_acTestgateUndefinedString);
   sprintf (APPEND_TO(pRequest), "X-Platform-Domain: %s\n",          g_acTestgateUndefinedString);
   sprintf (APPEND_TO(pRequest), "X-Platform-GUID: %s\n",            pConstantInfo->acPlatformGUID);
   sprintf (APPEND_TO(pRequest), "X-Platform-Host: %s\n",            pConstantInfo->acLocalMachineName);
   sprintf (APPEND_TO(pRequest), "X-Platform-Language: %s\n",        pConstantInfo->acPlatformLanguage);
   sprintf (APPEND_TO(pRequest), "X-Platform-Owner: %s\n",           pConstantInfo->acRegisteredSystemOwner);
   sprintf (APPEND_TO(pRequest), "X-Platform-Processor: %s\n",       pConstantInfo->acProcessorName);
   sprintf (APPEND_TO(pRequest), "X-Platform-Scanner: %s\n",         pConstantInfo->acPlatformScanner);
   sprintf (APPEND_TO(pRequest), "X-Platform-System: %s\n",          pConstantInfo->acOperatingSystemName);

   sprintf (APPEND_TO(pRequest), "X-Platform-User: %s\n",            pConstantInfo->acLocalUserName);
   sprintf (APPEND_TO(pRequest), "X-Scan-Result: %s\n",              pConstantInfo->acScanResult);
   sprintf (APPEND_TO(pRequest), "X-Sample-Changes: %d\n",           pConstantInfo->iSampleChanges);
   sprintf (APPEND_TO(pRequest), "X-Sample-Checksum: %s\n",          pPlainCRCString);

   char  *pExtension = FileExtension (pUseAsFilename);
   sprintf (APPEND_TO(pRequest), "X-Sample-Extension: %s\n",         pExtension[0]?pExtension:"COM");
   sprintf (APPEND_TO(pRequest), "X-Sample-File: %s\n",              pUseAsFilename);
   sprintf (APPEND_TO(pRequest), "X-Sample-FileID: %d\n",            pConstantInfo->iSampleFileID);   // ??
   sprintf (APPEND_TO(pRequest), "X-Sample-Priority: %d\n",          pConstantInfo->iSamplePriority);
   sprintf (APPEND_TO(pRequest), "X-Sample-Reason: %s\n",            pConstantInfo->acSampleReason);
   sprintf (APPEND_TO(pRequest), "X-Sample-Size: %d\n",              pFileInfo->nFileSizeLow);
   sprintf (APPEND_TO(pRequest), "X-Sample-Status: %d\n",            pConstantInfo->iSampleStatus);   // 9 = customer's intervention required.
   sprintf (APPEND_TO(pRequest), "X-Sample-Status-Readable: %s\n",   pConstantInfo->acSampleStatusReadable);
   sprintf (APPEND_TO(pRequest), "X-Sample-Submission-Route: %d\n",  pConstantInfo->iSampleSubmissionRoute);
   sprintf (APPEND_TO(pRequest), "X-Sample-Type: %s\n",              pConstantInfo->acSampleType);
   sprintf (APPEND_TO(pRequest), "X-Sample-UUID: %s\n",              pConstantInfo->acSampleUUID);
   sprintf (APPEND_TO(pRequest), "X-Scan-Signatures-Sequence: %d\n", pConstantInfo->iScanSignaturesSequence);
   sprintf (APPEND_TO(pRequest), "X-Scan-Signatures-Version: %s\n",  pConstantInfo->acScanSignaturesVersion);
//   sprintf (APPEND_TO(pRequest), "X-Scan-Virus-Identifier: %d\n",    88888);
//   sprintf (APPEND_TO(pRequest), "X-Scan-Virus-Name: %s\n",          g_acTestgateUndefinedString);
   sprintf (APPEND_TO(pRequest), "User-Agent: %s\n",                 g_acTestgateNameString);
//C   sprintf (APPEND_TO(pRequest), "Host: %s:%d\n",                    pConstantInfo->acGatewayNameOrIP, pConstantInfo->iGatewayPort);
   sprintf (APPEND_TO(pRequest), "Host: %s:%d\n",                    pTargetNameOrIP, (int)usTargetPort);
   sprintf (APPEND_TO(pRequest), "\n");


// // typedef struct _CONSTANT_INFO
// {
//    char        acLocalMachineName      [MAX_COMPUTERNAME_LENGTH + 1];// GetComputerName()
//    char        acLocalUserName         [MAX_PATH];                   // GetUserName(): Currently logged on user.
//    char        acGatewayNameOrIP       [MAX_PATH];                   // navajo.watson.ibm.com. From command line or from .INI file.
//    int         iGatewayPort;                                         // 2847. From command line or from .INI file.
//    char        acRegisteredSystemOwner [MAX_PATH];                   // "HKLM/Software/Microsoft/WindowsNT/CurrentVersion"
//    char        acProcessorName         [200];                        // "HKLM/HARDWARE/DESCRIPTION/System/CentralProcessor/0/ *
//    char        acPlatformScanner       [200];                        //  Norton anti-virus or TESTGATE.
//    char        acOperatingSystemName   [200];                        // GetVersionEx();
//    char        acCustomerContactName   [200];
//    char        acCustomerContactEmail  [200];
//    char        acCustomerContactPhone  [200];
//    char        acCustomerIdentifier    [200];
//
//    char        acPlatformGUID          [40];
//    char        acSampleUUID            [40];
//
//    char        acConnectionType        [80];
//    char        acContentType           [80];
//    char        acAnalysisState         [80];
//    char        acChecksumMethod        [80];
//    char        acContentCompression    [80];
//    char        acContentScrambling     [80];
//    char        acIcePackInitializedState [80];
//    char        acPlatformLanguage      [80];
//    char        acScanResult            [80];
//    int         iSampleChanges;
//    int         iSampleFileID;
//    int         iSamplePriority;
//    char        acSampleReason          [80];
//    int         iSampleStatus;
//    char        acSampleStatusReadable  [80];
//    int         iSampleSubmissionRoute;
//    char        acSampleType            [80];
//    int         iScanSignaturesSequence;
//    char        acScanSignaturesVersion [80];
// } CONSTANT_INFO;


   iFullRequestSize = strlen(pRequest);
   pFullRequest = new char [iFullRequestSize+1];
   strcpy (pFullRequest, pRequest);
   delete [] pRequest;

   *ppAllocatedRequest     =  pFullRequest;
   *piAllocatedBufferSize  =  iFullRequestSize;
}
//===========================================================================

//===========================================================================
//

VOID  Free_HTTP_Request (char *pAllocatedHttpRequest)
{
   delete [] pAllocatedHttpRequest;
}
//===========================================================================


//===========================================================================
//
// Initialize global variables, static structures. Reads command lines.
//
//   Testgate_Initialize():
//   ======================
// - Global hardcoded default values already initialized.
// - Get profile name (/profile=)
// - If profile name were specified:
//    - Read profile from INI file onto the global variables.
// - Create fileset and copy specs and sizes from globals to fileset.
// - Read command line arguments onto the global variables - effectively overwriting
//   whatever was read from INI file (if /profile was specified).
// - If profile name were specified:
//    - Write global variables onto the INI file under this profile.
// - Initialize CONSTANT_INFO structure.

#define  INIT_STRING_VALUE(val)                                                                                                                                        \
   {                                                                                                                                                                   \
   /* Read data from INI file first */                                                                                                                                 \
   if (g_acCurrentProfileName [0])                                                                                                                                     \
      LoadValue_String       (                                                                                                                                         \
         0,                /* HKEY        hBaseKeyHandle:   HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS]. */              \
         g_acCurrentProfileName,    /* const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file. */        \
         val##_CMDINI[1],  /* const char  *pValueName,   */                                                                                                            \
         val,              /* char        *pDestString,  */                                                                                                            \
         sizeof (val),     /* int         iDestStrSize,  */                                                                                                            \
         val,              /* char        *pStrDefault,  */                                                                                                            \
         g_acIniFilename   /* const char  *pIniFilename=NULL:  Name of INI file, if INI file must be used. */                                                          \
         );                                                                                                                                                            \
   /* Overwrite INI data with command line data (if any). */                                                                                                           \
   bRetCode =                                                                                                                                                          \
      IsArgument ( /* BOOL: TRUE:exists, FALSE:not.   */                                                                                                               \
      ppArgs,           /* char  **ppArgs:      input:  As passed to 'main()'.   */                                                                                    \
      iArgsNum,         /* int   iArgsNum:       input:  As passed to 'main()'.  */                                                                                    \
      val##_CMDINI[0],  /* char  *pArg:         input:  "value="        Cannot be NULL.   */                                                                           \
      &pWholeArg,       /* char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.   */                                                                           \
      &pRestOfArg,      /* char  **ppRestArg:   output: "1024".         Cannot be NULL.   */                                                                           \
      FALSE             /* BOOL  bStrict= FALSE: TRUE - exact copy required (no partial). */                                                                           \
      );                                                                                                                                                               \
   if (bRetCode)                                                                                                                                                       \
      strcpy (val, pRestOfArg);                                                                                                                                        \
                                                                                                                                                                       \
   /* Save new value to INI file */                                                                                                                                    \
   if (g_acCurrentProfileName [0])                                                                                                                                     \
      SaveValue_String       (                                                                                                                                         \
         0,                /* HKEY        hBaseKeyHandle:   HKEY hKey|NULL(if INI): handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS]. */ \
         g_acCurrentProfileName,    /* const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file. */        \
         val##_CMDINI[1],  /* const char  *pValueName */                                                                                                               \
         val,              /* char        *pString    */                                                                                                               \
         g_acIniFilename   /* const char  *pIniFilename=NULL: Name of INI file, if INI file must be used. */                                                           \
         );                                                                                                                                                            \
   }

#define  INIT_INT_VALUE(val)                                                                                                                                           \
   {                                                                                                                                                                   \
   /* Read data from INI file first */                                                                                                                                 \
   if (g_acCurrentProfileName [0])                                                                                                                                     \
      LoadValue_Int    (                                                                                                                                               \
         0,                /* HKEY        hBaseKeyHandle:   HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS]. */              \
         g_acCurrentProfileName,    /* const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file. */        \
         val##_CMDINI[1],  /* const char  *pValueName,                                 */                                                                              \
         &val,             /* int         *piValue:   Where to read the value.                     */                                                                  \
         val,              /* int         iDefault:   Default value for the value, if not found.   */                                                                  \
         g_acIniFilename   /* const char  *pIniFilename=NULL:  Name of INI file, if INI file must be used. */                                                          \
         );                                                                                                                                                            \
   /* Overwrite INI data with command line data (if any). */                                                                                                           \
   bRetCode =                                                                                                                                                          \
      IsArgument ( /* BOOL: TRUE:exists, FALSE:not.   */                                                                                                               \
      ppArgs,           /* char  **ppArgs:      input:  As passed to 'main()'.   */                                                                                    \
      iArgsNum,         /* int   iArgsNum:       input:  As passed to 'main()'.  */                                                                                    \
      val##_CMDINI[0],  /* char  *pArg:         input:  "value="        Cannot be NULL.   */                                                                           \
      &pWholeArg,       /* char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.   */                                                                           \
      &pRestOfArg,      /* char  **ppRestArg:   output: "1024".         Cannot be NULL.   */                                                                           \
      FALSE             /* BOOL  bStrict= FALSE: TRUE - exact copy required (no partial). */                                                                           \
      );                                                                                                                                                               \
   if (bRetCode)                                                                                                                                                       \
      val = atoi (pRestOfArg);                                                                                                                                         \
                                                                                                                                                                       \
   /* Save new value to INI file */                                                                                                                                    \
   if (g_acCurrentProfileName [0])                                                                                                                                     \
      SaveValue_Int    (                                                                                                                                               \
         0,                /* HKEY        hBaseKeyHandle:   HKEY hKey|NULL(if INI): handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS]. */ \
         g_acCurrentProfileName,    /* const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file. */        \
         val##_CMDINI[1],  /* const char  *pValueName */                                                                                                               \
         val,              /* int         iValue:  Value.   */                                                                                                         \
         g_acIniFilename   /* const char  *pIniFilename=NULL: Name of INI file, if INI file must be used. */                                                           \
         );                                                                                                                                                            \
   }

#define  INIT_BOOL_VALUE(val)                                                                                                                                          \
   {                                                                                                                                                                   \
   /* Read data from INI file first */                                                                                                                                 \
   if (g_acCurrentProfileName [0])                                                                                                                                     \
      LoadValue_Bool    (                                                                                                                                              \
         0,                /* HKEY        hBaseKeyHandle:   HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS]. */              \
         g_acCurrentProfileName,    /* const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file. */        \
         val##_CMDINI[1],  /* const char  *pValueName,                                 */                                                                              \
         &val,             /* BOOL        *pbValue:   Where to read the value.   */                                                                                    \
         val,              /* BOOL        bDefault:   Default value for the value, if not found.   */                                                                  \
         g_acIniFilename   /* const char  *pIniFilename=NULL:  Name of INI file, if INI file must be used. */                                                          \
         );                                                                                                                                                            \
   /* Overwrite INI data with command line data (if any). */                                                                                                           \
   bRetCode =                                                                                                                                                          \
      IsArgument ( /* BOOL: TRUE:exists, FALSE:not.   */                                                                                                               \
      ppArgs,           /* char  **ppArgs:      input:  As passed to 'main()'.   */                                                                                    \
      iArgsNum,         /* int   iArgsNum:       input:  As passed to 'main()'.  */                                                                                    \
      val##_CMDINI[0],  /* char  *pArg:         input:  "value="        Cannot be NULL.   */                                                                           \
      &pWholeArg,       /* char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.   */                                                                           \
      &pRestOfArg,      /* char  **ppRestArg:   output: "1024".         Cannot be NULL.   */                                                                           \
      TRUE              /* BOOL  bStrict= FALSE: TRUE - exact copy required (no partial). */                                                                           \
      );                                                                                                                                                               \
   if (bRetCode)                                                                                                                                                       \
      val = 1;                                                                                                                                                         \
                                                                                                                                                                       \
   /* Save new value to INI file */                                                                                                                                    \
   if (g_acCurrentProfileName [0])                                                                                                                                     \
      SaveValue_Bool    (                                                                                                                                              \
         0,                /* HKEY        hBaseKeyHandle:   HKEY hKey|NULL(if INI): handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS]. */ \
         g_acCurrentProfileName,    /* const char  *pFullKeyOrSectionName: Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file. */        \
         val##_CMDINI[1],  /* const char  *pValueName */                                                                                                               \
         val,              /* BOOL        bValue:  Value.  */                                                                                                          \
         g_acIniFilename   /* const char  *pIniFilename=NULL: Name of INI file, if INI file must be used. */                                                           \
         );                                                                                                                                                            \
   }

BOOL  Testgate_Initialize  (
         int            iArgsNum,
         char           **ppArgs,
         CONSTANT_INFO  *pConstantInfo,
         Fileset        **ppFileset
         )
{
//---------------------------------------------------------------------------
   Fileset        *pFileset = NULL;
   char           *pWholeArg, *pRestOfArg;
   DWORD          dwRegistryValue1;
   char           acRegistryValue1 [MAX_PATH];
   char           acRegistryValue2 [MAX_PATH];
   OSVERSIONINFO  osVerInfo;
   DWORD          dwAttrs;
   BOOL           bCmdLineFilespecPresent;
   int            i;
   DWORD          dwSize;

   BOOL           bRetCode = FALSE;
//---------------------------------------------------------------------------


   // This will initalize it.
   GetRandomNumber (1, 10);

   memset (pConstantInfo, 0, sizeof (*pConstantInfo));

   g_acGetSumsFrom [0] = 0;
   g_acSetSumsTo   [0] = 0;

   // Assemble INI filename. Which would be our name, but .INI extension.
   GetModuleFileName (
      NULL,                      // HMODULE hModule: handle to module to find filename for
      g_acIniFilename,           // LPTSTR lpFilename:  pointer to buffer to receive module path
      sizeof (g_acIniFilename)   // DWORD nSize:  size of buffer, in characters
      );
   int   iLen = strlen (g_acIniFilename);
   // Change extension to .INI
   g_acIniFilename [iLen - 3] = 'I';
   g_acIniFilename [iLen - 2] = 'N';
   g_acIniFilename [iLen - 1] = 'I';

   // Assemble LOG filename. for Printf() calls.
   GetModuleFileName (
      NULL,                      // HMODULE hModule: handle to module to find filename for
      g_acLogFilename,           // LPTSTR lpFilename:  pointer to buffer to receive module path
      sizeof (g_acLogFilename)   // DWORD nSize:  size of buffer, in characters
      );
   iLen = strlen (g_acLogFilename);
   // Change extension to .INI
   g_acLogFilename [iLen - 3] = 'L';
   g_acLogFilename [iLen - 2] = 'O';
   g_acLogFilename [iLen - 1] = 'G';


   // Assembling name of profile, if it was specified on the command line.
   // Checking '/profile='
   bRetCode =
      IsArgument ( // BOOL: TRUE:exists, FALSE:not.
      ppArgs,        // char  **ppArgs:      input:  As passed to 'main()'.
      iArgsNum,      // int   iArgsNum:       input:  As passed to 'main()'.
      "/profile=",   // char  *pArg:         input:  "value="        Cannot be NULL.
      &pWholeArg,    // char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.
      &pRestOfArg,   // char  **ppRestArg:   output: "1024".         Cannot be NULL.
      FALSE          // BOOL  bStrict /*= FALSE*/: TRUE - exact copy required (no partial).
      );
   if (bRetCode)
      strcpy (g_acCurrentProfileName, pRestOfArg);
   else
      g_acCurrentProfileName [0] = 0;

   // Initializing all variables now.
   // Read INI file first (if /profile= was specified), overwrite data with
   // the one from command line (if was present).
   // Save updated data to INI file (if /profile= was specified).

   // Normalize dirname.
   ConvertWeirdNameToFullPathName (
      g_acSamplesRootDir,   // const char  *pWeirdName,         // "..\..\.\.\dir1\..\..\dir2\file3
      g_acSamplesRootDir,   // char        *pFullPathName,      // "c:\dir1\dir2\file3"
      sizeof (g_acSamplesRootDir)  // DWORD       dwFullPathNameSize,
      // char        **ppFilenamePart     /* = NULL */
      );
   INIT_STRING_VALUE    (g_acSamplesRootDir)

   if (g_acSamplesRootDir[0])
      {
      dwAttrs = HLP_GetFileAttributes (g_acSamplesRootDir);

      if (dwAttrs == 0xFFFFFFFF || (dwAttrs != 0xFFFFFFFF && !(dwAttrs & FILE_ATTRIBUTE_DIRECTORY)))
         {
         textattr (FOREGROUND_RED + FOREGROUND_INTENSITY);
         printf ("\nLOADGATE: Error: Cannot access directory: '%s'. Aborting", g_acSamplesRootDir);
         textattr (g_wOrigAttrs);
         goto  ABORT;
         }
      }

   INIT_STRING_VALUE (g_acGetSumsFrom)
   // Normalize name.
   ConvertWeirdNameToFullPathName (
      g_acGetSumsFrom,   // const char  *pWeirdName,         // "..\..\.\.\dir1\..\..\dir2\file3
      g_acGetSumsFrom,   // char        *pFullPathName,      // "c:\dir1\dir2\file3"
      sizeof (g_acGetSumsFrom)  // DWORD       dwFullPathNameSize,
      // char        **ppFilenamePart     /* = NULL */
      );

   INIT_STRING_VALUE (g_acSetSumsTo)
   // Normalize name.
   ConvertWeirdNameToFullPathName (
      g_acSetSumsTo,   // const char  *pWeirdName,         // "..\..\.\.\dir1\..\..\dir2\file3
      g_acSetSumsTo,   // char        *pFullPathName,      // "c:\dir1\dir2\file3"
      sizeof (g_acSetSumsTo)  // DWORD       dwFullPathNameSize,
      // char        **ppFilenamePart     /* = NULL */
      );

   INIT_STRING_VALUE (g_acGatewayNameOrIP)
   if (!strcmpi (g_acGatewayNameOrIP, "s"))
      //  /gateway=s  - means use Symantec's.
      strcpy (g_acGatewayNameOrIP, "pilot.gateways.dis.symantec.com");

   INIT_BOOL_VALUE   (g_bUseSSL)
   if (g_bUseSSL) g_iGatewayPort = 2848;  // For SSL set default port to 2848.

   INIT_INT_VALUE    (g_iGatewayPort)
   INIT_INT_VALUE    (g_iSamThreadsNum)
   INIT_INT_VALUE    (g_iSamDelay)
   INIT_INT_VALUE    (g_iSamErrorsMax)
   INIT_INT_VALUE    (g_iSigThreadsNum)
   INIT_INT_VALUE    (g_iSQThreadsNum)
   INIT_INT_VALUE    (g_iSigDelay)
   INIT_INT_VALUE    (g_iSigErrorsMax)
   INIT_INT_VALUE    (g_iMinDelay)
   INIT_INT_VALUE    (g_iMaxDelay)
   INIT_INT_VALUE    (g_iSeed)
   INIT_BOOL_VALUE   (g_bIncludeSubdirs)
   INIT_INT_VALUE    (g_iIncludeSizeMin)
   INIT_INT_VALUE    (g_iIncludeSizeMax)
   INIT_INT_VALUE    (g_iExcludeSizeMin)
   INIT_INT_VALUE    (g_iExcludeSizeMax)
   INIT_BOOL_VALUE   (g_bZapBits)
   INIT_BOOL_VALUE   (g_bZapNames)
   INIT_INT_VALUE    (g_iMaxFilesPerSamThread)
   INIT_INT_VALUE    (g_iMaxSigsPerSigThread)
   INIT_INT_VALUE    (g_iMaxSQsPerSQThread)
   INIT_INT_VALUE    (g_iSQDelay)
   INIT_INT_VALUE    (g_iSQErrorsMax)

   INIT_BOOL_VALUE   (g_bLoop)
   INIT_BOOL_VALUE   (g_bVerbose)
   INIT_BOOL_VALUE   (g_bManualSample)
   INIT_BOOL_VALUE   (g_bSkipDownload)

   // /noask only process in command line. Do not save/read it from INI
   bRetCode =
      IsArgument ( /* BOOL: TRUE:exists, FALSE:not.   */
      ppArgs,           /* char  **ppArgs:      input:  As passed to 'main()'.   */
      iArgsNum,         /* int   iArgsNum:       input:  As passed to 'main()'.  */
      g_bNoAsk_CMDINI[0],  /* char  *pArg:         input:  "value="        Cannot be NULL.   */
      &pWholeArg,       /* char  **ppWholeArg:  output: "value=1024"    Cannot be NULL.   */
      &pRestOfArg,      /* char  **ppRestArg:   output: "1024".         Cannot be NULL.   */
      FALSE             /* BOOL  bStrict= FALSE: TRUE - exact copy required (no partial). */
      );
   if (bRetCode)
      g_bNoAsk = 1;

   // Now taking care of include/exclude specs.

   // - Create fileset.
   pFileset =
      new Fileset (
         "",   // const char *_pszFilesetName,
         0,    // HKEY        _hBaseKeyHandle,
         NULL, // const char *_pFullRegistryKeyName,
         g_acCurrentProfileName,    // const char *_pIniSectionName,
         g_acCurrentProfileName[0]?g_acIniFilename:NULL // const char *_pIniFilename
         );

   // Check if any filespec present on the command line.
   bCmdLineFilespecPresent = FALSE;
   for (i=1; i<iArgsNum; i++)
      {
      if (ppArgs[i][0] != '/')
         {
         bCmdLineFilespecPresent = TRUE;
         break;
         }
      }

   if (bCmdLineFilespecPresent)
      {
      // Read specs from command line.
      for (i=1; i<iArgsNum; i++)
         {
         if (ppArgs[i][0] != '/' && ppArgs[i][0] != EXCLUDE_CHAR)
            AddStringToArray (&g_ppIncludeSpecs, &g_iIncludeSpecsNum, ppArgs[i]);   // Adding include spec.
         else if (ppArgs[i][0] == EXCLUDE_CHAR)
            AddStringToArray (&g_ppExcludeSpecs, &g_iExcludeSpecsNum, ppArgs[i]+1); // Adding exclude spec.
         }
      }
   else
      {
      if (g_acCurrentProfileName [0])
         {
         // Load specs from INI file only if no specs present on
         // the command line and /profile parameter was specified.
         LoadValue_StringArray  (
            0,                      // HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            g_acCurrentProfileName, // const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            g_ppIncludeSpecs_CMDINI [1], // const char  *pValueName,
            &g_ppIncludeSpecs,      // char        ***pppStrArray,
            &g_iIncludeSpecsNum,    // int         *piStrArraySize,
            g_acIniFilename         // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );

         LoadValue_StringArray  (
            0,                      // HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            g_acCurrentProfileName, // const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
            g_ppExcludeSpecs_CMDINI [1], // const char  *pValueName,
            &g_ppExcludeSpecs,      // char        ***pppStrArray,
            &g_iExcludeSpecsNum,    // int         *piStrArraySize,
            g_acIniFilename         // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
            );
         }
      else
         {
         // Initialize specs with default val: include all: "*".
         AddStringToArray (&g_ppIncludeSpecs, &g_iIncludeSpecsNum, "*");
         }
      }

   // If profile name was specified - update INI file with filespecs info.
   if (g_acCurrentProfileName [0])
      {
      SaveValue_StringArray  (
         0, // HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         g_acCurrentProfileName, // const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         g_ppIncludeSpecs_CMDINI [1], // const char  *pValueName,
         g_ppIncludeSpecs,       // char        **ppString,
         g_iIncludeSpecsNum,     // int         iStringsNum,
         g_acIniFilename         // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         );

      SaveValue_StringArray  (
         0, // HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         g_acCurrentProfileName, // const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         g_ppExcludeSpecs_CMDINI [1], // const char  *pValueName,
         g_ppExcludeSpecs,       // char        **ppString,
         g_iExcludeSpecsNum,     // int         iStringsNum,
         g_acIniFilename         // const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         );
      }

   // Add to fileset filespecs and filesize values.
   pFileset->AddSpecs          (TRUE,  (const char **)g_ppIncludeSpecs, g_iIncludeSpecsNum);
   pFileset->AddSpecs          (FALSE, (const char **)g_ppExcludeSpecs, g_iExcludeSpecsNum);
   // If at this point there are no "include filespecs" whatsoever - then add
   // '*'
   if (!pFileset->GetSpecsNum (TRUE))
      pFileset->AddSpec (TRUE, "*");   // Include all files in that case.
   pFileset->SetIncludeSizes   (g_iIncludeSizeMin, g_iIncludeSizeMax);
   pFileset->SetExcludeSizes   (g_iExcludeSizeMin, g_iExcludeSizeMax);

   // Here we have complete fileset created and initialized.

   // Initialize CONSTANT_INFO structure.
   //
   dwSize = sizeof (pConstantInfo->acLocalMachineName);
   GetComputerName (
      pConstantInfo->acLocalMachineName,   // LPTSTR lpBuffer: address of name buffer
      &dwSize  // LPDWORD nSize:   address of size of name buffer
      );

   dwSize = sizeof (pConstantInfo->acLocalUserName);
   GetUserName (
      pConstantInfo->acLocalUserName,   // LPTSTR lpBuffer: address of name buffer
      &dwSize  // LPDWORD nSize:   address of size of name buffer
      );

   strcpy (pConstantInfo->acGatewayNameOrIP, g_acGatewayNameOrIP);
   pConstantInfo->iGatewayPort = g_iGatewayPort;

   dwSize = sizeof (pConstantInfo->acRegisteredSystemOwner);
   HLP_RegLoadValue (
      HKEY_LOCAL_MACHINE,  // HKEY        hBaseKeyHandle,// HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
      "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",  // const char  *pKeyName,     // "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
      "RegisteredOwner",   // const char  *pValueName,   // "Include Subdirectories"
      pConstantInfo->acRegisteredSystemOwner,     //char        *pData,        // Where to load data or NULL if data size query.
      &dwSize,             // DWORD       *pdwDataSize,  // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
      g_acTestgateUndefinedString,     // char        *pDefaultData     /*=NULL */,  // Default data buffer if key is not in registry.
      sizeof (g_acTestgateUndefinedString)   // DWORD       dwDefaultDataSize /*=0    */   // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
      );

   dwSize = sizeof (acRegistryValue1);
   HLP_RegLoadValue (
      HKEY_LOCAL_MACHINE,  // HKEY        hBaseKeyHandle,// HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",  // const char  *pKeyName,     // "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
      "VendorIdentifier",  // const char  *pValueName,   // "Include Subdirectories"
      acRegistryValue1,    //char        *pData,        // Where to load data or NULL if data size query.
      &dwSize,             // DWORD       *pdwDataSize,  // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
      g_acTestgateUndefinedString,     // char        *pDefaultData     /*=NULL */,  // Default data buffer if key is not in registry.
      sizeof (g_acTestgateUndefinedString)   // DWORD       dwDefaultDataSize /*=0    */   // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
      );
   dwSize = sizeof (dwRegistryValue1);
   HLP_RegLoadValue (
      HKEY_LOCAL_MACHINE,  // HKEY        hBaseKeyHandle,// HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",  // const char  *pKeyName,     // "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
      "~MHZ",              // const char  *pValueName,   // "Include Subdirectories"
      (char *)&dwRegistryValue1,   //char        *pData,        // Where to load data or NULL if data size query.
      &dwSize,             // DWORD       *pdwDataSize,  // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
      g_acTestgateUndefinedString,     // char        *pDefaultData     /*=NULL */,  // Default data buffer if key is not in registry.
      sizeof (g_acTestgateUndefinedString)   // DWORD       dwDefaultDataSize /*=0    */   // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
      );
   dwSize = sizeof (acRegistryValue2);
   HLP_RegLoadValue (
      HKEY_LOCAL_MACHINE,  // HKEY        hBaseKeyHandle,// HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",  // const char  *pKeyName,     // "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
      "Identifier",        // const char  *pValueName,   // "Include Subdirectories"
      acRegistryValue2,    //char        *pData,        // Where to load data or NULL if data size query.
      &dwSize,             // DWORD       *pdwDataSize,  // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
      g_acTestgateUndefinedString,     // char        *pDefaultData     /*=NULL */,  // Default data buffer if key is not in registry.
      sizeof (g_acTestgateUndefinedString)   // DWORD       dwDefaultDataSize /*=0    */   // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
      );
   sprintf (pConstantInfo->acProcessorName, "%s(%dMHz) %s", acRegistryValue1, dwRegistryValue1, acRegistryValue2);

   strcpy (pConstantInfo->acPlatformScanner, g_acTestgateUndefinedString);


   osVerInfo.dwOSVersionInfoSize = sizeof (osVerInfo);
   GetVersionEx (&osVerInfo); // pointer to version information structure

   sprintf (pConstantInfo->acOperatingSystemName, "Windows %s %d.%02d (Build %d) %s",
            osVerInfo.dwPlatformId==VER_PLATFORM_WIN32_NT ? "NT" : (osVerInfo.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS?"9x":"???"),
            osVerInfo.dwMajorVersion,
            osVerInfo.dwMinorVersion,
            osVerInfo.dwBuildNumber,
            osVerInfo.szCSDVersion
            );

   strcpy (pConstantInfo->acCustomerContactName , "LoadGate");
   strcpy (pConstantInfo->acCustomerContactEmail, "pavuluri@us.ibm.com");
   strcpy (pConstantInfo->acCustomerContactPhone, "914-784-7254");
   strcpy (pConstantInfo->acCustomerIdentifier  , g_acTestgateNameString);

   strcpy (pConstantInfo->acPlatformGUID, "{00000000-0000-8829-0000-000000000000}");
   strcpy (pConstantInfo->acSampleUUID  , "{00000000-0000-8829-0000-000000000000}");

   strcpy (pConstantInfo->acConnectionType,           "close");
   strcpy (pConstantInfo->acContentType,              "application/x-avis-sample");
   strcpy (pConstantInfo->acAnalysisState,            "submitting");
   strcpy (pConstantInfo->acChecksumMethod,           "md5");
   strcpy (pConstantInfo->acContentCompression,       "deflate");

   strcpy (pConstantInfo->acContentScrambling,        "xor-vampish");
   strcpy (pConstantInfo->acIcePackInitializedState,  "true");
   strcpy (pConstantInfo->acPlatformLanguage,         "English");
   strcpy (pConstantInfo->acScanResult,               "unrepairable");
   pConstantInfo->iSampleChanges          =           1;
   pConstantInfo->iSampleFileID           =           8829;
   pConstantInfo->iSamplePriority         =           42;
   strcpy (pConstantInfo->acSampleReason,             g_bManualSample ? "manual" : "unknown");
   pConstantInfo->iSampleStatus           =           9;
   strcpy (pConstantInfo->acSampleStatusReadable,     "customer intervention required");
   pConstantInfo->iSampleSubmissionRoute  =           1;
   strcpy (pConstantInfo->acSampleType,               "file");
   pConstantInfo->iScanSignaturesSequence =           4183;
   strcpy (pConstantInfo->acScanSignaturesVersion,    "19991221.024");


   // Done!

   bRetCode = TRUE;



EXIT:
   *ppFileset = pFileset;
   return (bRetCode);

ABORT:
   bRetCode = FALSE;
   goto  EXIT;
}
//===========================================================================

//===========================================================================
//
//

VOID  Testgate_Cleanup (Fileset **ppFileset)
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

   delete  (*ppFileset);
   *ppFileset = NULL;

   DeleteStringArray   (&g_ppIncludeSpecs, &g_iIncludeSpecsNum);
   DeleteStringArray   (&g_ppExcludeSpecs, &g_iExcludeSpecsNum);
}
//===========================================================================


//===========================================================================
//
//

BOOL  AllThreadsTerminated (
         ALL_THREAD_PARAMS   **ppAllThreadParms,
         int                           iAllThreadParmsNum
         )
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------


   for (i=0; i<iAllThreadParmsNum; i++)
      {
      if (WaitForSingleObject (ppAllThreadParms[i]->hThread, 0) == WAIT_TIMEOUT)
         return (FALSE);
      }

   return (TRUE);
}
//===========================================================================

//===========================================================================
//
// Initially pass the random number.
// For subsequent calls on the same buffer chunks pass here the value,
// returned by the prev. calls.
//
// Input DWORD:
//
// bits:
// 24-32 IIII IIII   XOR[0]
// 16-23 IIII IIII   XOR[1]
// 8-15  IIII IIII   XOR[2]
// 0-7   IIII IIII
//         |    |
//         |    ----- starting index for XOT-ing.
//         ---------- increments to the next byte to XOR

DWORD ZapBitsInBuffer (
         DWORD    dwStartingRandomNumber_Or_Handle,
         char     *pBuffer,
         int      iBufferSize
         )
{
//---------------------------------------------------------------------------
   int            i;

   int            iStartingIndex;   // low 4 bits.
   int            iStepSize;        // high 4 bits.
   unsigned char  acXOR[3];
   int            iXORIndex;

   int            iNextStartingIndex;   // low 4 bits.
   unsigned char  acNextXOR[3];
   DWORD          dwNextHandle;
//---------------------------------------------------------------------------

   // WCA 2000/05/23 use all bits of random number, not just 24
   if (iBufferSize >= sizeof(dwStartingRandomNumber_Or_Handle)) // WCA
      *((DWORD *)pBuffer) ^= dwStartingRandomNumber_Or_Handle;  // WCA

   // WCA 2000/05/23 feed through a known-good linear congruential generator.
   dwStartingRandomNumber_Or_Handle = dwStartingRandomNumber_Or_Handle * 69069L + 7;

   iStartingIndex = dwStartingRandomNumber_Or_Handle & 0x0F;
   iStepSize      = (dwStartingRandomNumber_Or_Handle >> 4) & 0x0F;
   if (!iStepSize) iStepSize = 7;

   acXOR[0]       = (char)((dwStartingRandomNumber_Or_Handle >> 24));
   acXOR[1]       = (char)((dwStartingRandomNumber_Or_Handle >> 16) & 0xFF);
   acXOR[2]       = (char)((dwStartingRandomNumber_Or_Handle >> 8)  & 0xFF);

   for (iXORIndex = 0, i=iStartingIndex; i<iBufferSize; i+=iStepSize, iXORIndex++)
      {
      if (iXORIndex>2)
         iXORIndex = 0;
      pBuffer[i] ^= acXOR [iXORIndex];
      }

   // Calculate next starting index.
   iNextStartingIndex = i - iBufferSize;

   // Fill next XORs.
   for (i=0; i<3; i++, iXORIndex++)
      {
      if (iXORIndex>2)
         iXORIndex = 0;
      acNextXOR[i] = acXOR [iXORIndex];
      }

   dwNextHandle =
      (((DWORD)acNextXOR [0]) << 24)      +
      (((DWORD)acNextXOR [1]) << 16)      +
      (((DWORD)acNextXOR [2]) << 8)       +
      (iStepSize << 4)                    +
      (iNextStartingIndex & 0x0F);


   return (dwNextHandle);
}
//===========================================================================

//===========================================================================
//
//

VOID  ZapFilename (
         const char  *pFilename,
         char        *pAlteredFilename
         )
{
//---------------------------------------------------------------------------
   char           acJustFilename [MAX_PATH];
   char           acRandomChars [20];
   char           *pRandomChars;
   DWORD          dwRandomDword;
//---------------------------------------------------------------------------

   strcpy (pAlteredFilename, pFilename);
   strcpy (acJustFilename, JustFilename (pFilename));

   // Cut filename.
   *JustFilename(pAlteredFilename) = 0;

   // 0 ... 2**24
   dwRandomDword = GetRandomDword();

   sprintf (acRandomChars, "%08d", dwRandomDword);
   pRandomChars = acRandomChars + 2;

   sprintf (pAlteredFilename + strlen(pAlteredFilename), "%04X_%s_%s",
         GetCurrentThreadId(), pRandomChars, acJustFilename);
}
//===========================================================================

//===========================================================================
//
// Function will calculate plain and scrambled CRCs of file.
// Returns:
//    TRUE  - success. no errors.
//    FALSE - error. CRC buffers are invalid. Error code present.

BOOL  GetFileCRCs (
         const char  *pFilename,
         char        *pUseBuffer,         // NULL|ptr to buffer for file I/O. NULL-allocate/free itself.
         DWORD       dwUseBufferSize,     // 0|Size of buffer. 0:allocate itself for the whole file and ignore 'pUseBuffer'.
         BOOL        bZapBits,            // TRUE - randomly zap bits of file.
         DWORD       dwZapKey,            // Random 32 bit number to be used to zap bits.
         char        *pPlainCRC,          // At least 33 bytes. Or zapped pre-scrambled CRC.
         char        *pScrambledCRC,      // At least 33 bytes. Scrambled CRC.
         DWORD       *pdwLastError,
         char        *pLastErrorText,
         DWORD       dwLastErrorTextSize
         )
{
//---------------------------------------------------------------------------
   WIN32_FIND_DATA   w32FindData;
   HANDLE            hFile                = INVALID_HANDLE_VALUE;
   DWORD             dwFileSize;
   char              *pDataBuffer         = NULL;
   DWORD             dwDataBufferSize     = 0;
   BOOL              bFreeBufferAtTheEnd  = TRUE;
   DWORD             dwZapHandle;
   VOID              *pvCRCContext_Clean        = NULL;
   VOID              *pvCRCContext_Scrambled    = NULL;
   VOID              *pvScrambleHandle          = NULL;

   DWORD             dwBytes              = 0;

   DWORD             dwLastError          = 0;
   BOOL              bRetCode;
//---------------------------------------------------------------------------


   // Reset outputs first.
   pPlainCRC [0]     = 0;
   pScrambledCRC [0] = 0;
   if (pdwLastError)
      *pdwLastError = 0;
   if (pLastErrorText)
      *pLastErrorText = 0;


   HLP_GetFileAttributes (pFilename, &w32FindData);
   dwFileSize = w32FindData.nFileSizeLow;

   // Open destination file.
   //
   hFile =
      CreateFile ( // HANDLE: INVALID_HANDLE_VALUE:error, else:handle.
         pFilename,        // LPCTSTR  lpFileName:   address of name of the file
         GENERIC_READ,     // DWORD  dwDesiredAccess:   access (read-write) mode
         FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,  // DWORD  dwShareMode: share mode.
         NULL,             // LPSECURITY_ATTRIBUTES  lpSecurityAttributes:   address of security descriptor
         OPEN_EXISTING,    // DWORD  dwCreationDistribution:  how to create
         0,                // DWORD  dwFlagsAndAttributes: file attributes
         NULL              // HANDLE  hTemplateFile: handle of file with attributes to copy
         );
   if (hFile == INVALID_HANDLE_VALUE)
      {
      dwLastError = GetLastError();
      if (pLastErrorText)
         GetLastErrorText (pLastErrorText, dwLastErrorTextSize, dwLastError);

      goto  ABORT;   // Init errors and return FALSE.
      }


   if (pUseBuffer)
      {
      if (dwUseBufferSize)
         {
         dwDataBufferSize     =  dwUseBufferSize;
         pDataBuffer          =  pUseBuffer;
         bFreeBufferAtTheEnd  =  FALSE;
         }
      else
         {
         // Ignore non-NULL 'pUseBuffer' here.
         dwDataBufferSize     =  dwFileSize;
         pDataBuffer          =  new char [dwFileSize];
         bFreeBufferAtTheEnd  =  TRUE;
         }
      }
   else
      {
      if (dwUseBufferSize)
         {
         dwDataBufferSize     =  dwUseBufferSize;
         pDataBuffer          =  new char [dwUseBufferSize];
         bFreeBufferAtTheEnd  =  TRUE;
         }
      else
         {
         dwDataBufferSize     =  dwFileSize;
         pDataBuffer          =  new char [dwFileSize];
         bFreeBufferAtTheEnd  =  TRUE;
         }
      }


   // Reset values.
   dwZapHandle             = dwZapKey; // 0 ... 2**24
   pvCRCContext_Clean      = NULL;
   pvCRCContext_Scrambled  = NULL;
   pvScrambleHandle        = NULL;

   do
      {
      bRetCode =
         ReadFile ( // BOOL: fail=>0, else != 0.
            hFile,            // HANDLE hFile: handle of file to read
            pDataBuffer,      // LPVOID lpBuffer: address of buffer that receives data
            dwDataBufferSize, // DWORD nNumberOfBytesToRead:  number of bytes to read
            &dwBytes,         // LPDWORD lpNumberOfBytesRead: address of number of bytes read
            NULL              // LPOVERLAPPED lpOverlapped:   address of structure for data
            );
      dwLastError = GetLastError();

      if (!bRetCode)
         {
         dwLastError = GetLastError();
         if (pLastErrorText)
            GetLastErrorText (pLastErrorText, dwLastErrorTextSize, dwLastError);

         goto  ABORT;   // Init errors and return FALSE.
         }

      if (dwBytes == 0)
         {
         // EOF reached!
         break;
         }

      if (bZapBits)
         {
         dwZapHandle =
            ZapBitsInBuffer (
               dwZapHandle,   // DWORD    dwStartingRandomNumber_Or_Handle
               pDataBuffer,   // char     *pBuffer,
               dwBytes        // int      iBufferSize
               );
         }


      // Calculate clean CRC.
      pvCRCContext_Clean =
         CalculateBufferCRCString (
            pvCRCContext_Clean,     // VOID        *pvContext,       //   NULL-first run | return code from previous call.
            pDataBuffer,      // const char  *pBuffer,         //   NULL|Buffer with binary data for CRC calcs.
            dwBytes           // DWORD       dwBufferSize,     //   0|Size of buffer.
            // BOOL        bLastBuffer /*=FALSE*/,   //   TRUE:last buffer in the sequence of buffers.
            // char        *pCrcString   /*=NULL*/   // NULL|ptr to where to return CRC string. Must not be NULL, if 'bLastBuffer' is TRUE.
            );

      // Scramble buffer.
      pvScrambleHandle =
         ScrambleBuffer (
            pvScrambleHandle, // VOID     *pvHandle,     // NULL:first time run, else:Scramble handle, returned from prev. call.
            pDataBuffer,      // char     *pBuffer,
            dwBytes           // DWORD    dwBufferSize,
            );

      // Calculate scrambled CRC.
      pvCRCContext_Scrambled =
         CalculateBufferCRCString (
            pvCRCContext_Scrambled, // VOID        *pvContext,       //   NULL-first run | return code from previous call.
            pDataBuffer,      // const char  *pBuffer,         //   NULL|Buffer with binary data for CRC calcs.
            dwBytes           // DWORD       dwBufferSize,     //   0|Size of buffer.
            // BOOL        bLastBuffer /*=FALSE*/,   //   TRUE:last buffer in the sequence of buffers.
            // char        *pCrcString   /*=NULL*/   // NULL|ptr to where to return CRC string. Must not be NULL, if 'bLastBuffer' is TRUE.
            );
      }
#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while (1);
#pragma  warning (default : 4127)


   bRetCode = TRUE;


EXIT:
   // Finish CRC calculations / out data.
   // Even if we're here because of error 'CalculateBufferCRCString()' must be called
   // with 'bLastBuffer'==TRUE to cleanup resources.

   // Calculate clean CRC.
   pvCRCContext_Clean =
      CalculateBufferCRCString (
         pvCRCContext_Clean,     // VOID        *pvContext,       //   NULL-first run | return code from previous call.
         NULL,             // const char  *pBuffer,         //   NULL|Buffer with binary data for CRC calcs.
         0,                // DWORD       dwBufferSize,     //   0|Size of buffer.
         TRUE,             // BOOL        bLastBuffer /*=FALSE*/,   //   TRUE:last buffer in the sequence of buffers.
         bRetCode?pPlainCRC:NULL       // char        *pCrcString   /*=NULL*/   // NULL|ptr to where to return CRC string. Must not be NULL, if 'bLastBuffer' is TRUE.
         );

   // Calculate scrambled CRC.
   pvCRCContext_Scrambled =
      CalculateBufferCRCString (
         pvCRCContext_Scrambled, // VOID        *pvContext,       //   NULL-first run | return code from previous call.
         NULL,             // const char  *pBuffer,         //   NULL|Buffer with binary data for CRC calcs.
         0,                // DWORD       dwBufferSize,     //   0|Size of buffer.
         TRUE,             // BOOL        bLastBuffer /*=FALSE*/,   //   TRUE:last buffer in the sequence of buffers.
         bRetCode?pScrambledCRC:NULL   // char        *pCrcString   /*=NULL*/   // NULL|ptr to where to return CRC string. Must not be NULL, if 'bLastBuffer' is TRUE.
         );

   // Close file now.
   if (hFile != INVALID_HANDLE_VALUE)
      CloseHandle (hFile);

   if (bFreeBufferAtTheEnd)
      delete [] pDataBuffer;

   if (pdwLastError)
      *pdwLastError = dwLastError;

   return (bRetCode);


ABORT:
   bRetCode = FALSE;
   goto  EXIT;
}
//===========================================================================

//===========================================================================
//
// Function performs single talking session to the specified server in
// a fashion:
// Send request(s), receive reply(s), close connection.
//
// -  Function establishes connection to the specified server
// -  Send data, possibly in a number of send() calls.
// -  Receves data possibly in a number of recv() calls.
// -  Assembles received data in a single zero-terminated buffer.
// -  Returns to the caller.
//
// Function allows safely read blocking sockets in non-blocking fashion
// with user-defined timeout.
//
// Function returns:
//
// TG_RETCODE_SUCCESS          - Server disconnected. Possibly some data retrieved. No errors.
// TG_RETCODE_USER_ABORT       - Callback returned FALSE. No errors.
// TG_RETCODE_TIMEOUT          - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
// TG_RETCODE_ERROR            - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->acErrorText' has error text
//
// Note: Free_TransactWithServer() must be called to release resources,
//       allocated by this function. See notes below.
//
//Function called with a single parameter - ptr to 'TRANSACT_INFO_STRUCTURE', which has
// all the data required for function to work.
//Explanations of syntax and parameters:
//
//    char           acTargetNameOrIP [MAX_PATH];  - Input.
//       Name or IP address of server to connect/transact with.
//
//    unsigned short usTargetPort;
//       Port number of server to connect to.
//
//   BOOL           bUseSSL;                       // Use SSL connection to gateway.
//
//   BOOL           pSslContext;                   // Used for SSL connections.
//
//   DWORD          dwTotalTimeout;                // INFINITE|(ms) Stay in this function max. this time.
//       Function must spend no more, that this time (ms) before closing socket and return.
//
//    DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
//       'pfn_EventCallback' must be called every 'dwCallbackTimeout'ms. Caller will have a chance
//       to do housekeeping things, like premature returns.
//
//    BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.
//       If TRUE - function will discard data immediately upon return from recv() call.
//       If FALSE- all received data will be returned in 'pTransactData','iTransactDataSize'
//      parameters. In latter case call to 'Free_TransactWithServer (pTransactData)' will be
//       required to release memory, allocated by 'Allocate_TransactWithServer()' call.
//
//    BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
//       NULL|callback function, that will be called every 'dwCallbackTimeout'ms with appropriate
//       TG_CALLBACK_EVENT_* ID.
//       Caller may defined custom callback function and do from within of it progress notifications,
//       request premature exit and other operations.
//       Following IDs are defined:
//          TG_CALLBACK_EVENT_CONNECTED      -  Successful connection to server is accomplished.
//         TG_CALLBACK_EVENT_REQUEST_DATA   -  About to issue send(). Callback must initialize
//                                              'pTransactData','iTransactDataSize' values. If upon
//                                             return from callback either of these values is NULL|0
//                                              function will not send any data to server and will
//                                              go to the stage receiving of data from server.
//          TG_CALLBACK_EVENT_DATA_SENT      -  Issued directly after data was successfully sent to server.
//                                              'pTransactData','iTransactDataSize' will point to the buffer
//                                              with data, that has been sent and number of bytes, successfully
//                                              sent.
//          TG_CALLBACK_EVENT_DATA_RECEIVED  -  Chunk of data was just been received from server.
//                                             'pTransactData','iTransactDataSize' will points to the data received.
//                                              If 'bDumpData' is TRUE - this callback notification is the only chance
//                                             for caller to know what kind of data has been received from server.
//                                              Please note, that    pTransactData [iTransactDataSize] = 0  in this case.
//                                              (Which means returned buffer is always 1 byte longer, than 'iTransactDataSize'
//                                              says and always zero-terminated for caller's convenient printf-ing purposes.
//          TG_CALLBACK_EVENT_NO_DATA_READY  -  Function timeouted, waiting for data to be received from
//                                              the server. 'pTransactData','iTransactDataSize' are NULL,0.
//
//       If during the communication with server network-related error occurs, function
//      will not call callback function, but will exit with appropriate error code and will most
//       likely have error text message in 'acErrorText' and also 'iWSALastErrorCode' will
//      probably be initialized.
//
//    DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
//       This value will be initialized to the appropriate TG_CALLBACK_EVENT_* value during the callback call.
//       See above for description.
//
//
//    char           *pTransactData;               // Multipurpose I/O buffer.
//   int            iTransactDataSize;            // I/O buffer size.
//       'pTransactData','iTransactDataSize' are initialized to the appropriate values during the lifetime
//      of the function call.
//       Function startup:
//       -----------------
//          -  if 'pfn_EventCallback' is NULL, then 'pTransactData','iTransactDataSize'
//             must have a valid values, unless caller does not want to send any data
//             to server. Because function does not have a way to call callback -
//             it will send data only once and obviously only if these values are initialized
//             to non-zeros.
//            'pfn_EventCallback' is *NOT* NULL, then 'pTransactData','iTransactDataSize'
//             are ignored on startup and callback (TG_CALLBACK_EVENT_REQUEST_DATA) will
//            be used to retrieve these values.
//
//       Function exit:
//       --------------
//          -  'pTransactData','iTransactDataSize' will point to the function-allocated
//             data buffer, containing total data received from the server, unless
//             'bDumpData' was set to TRUE by the caller.
//             'Free_TransactWithServer (pTransactData)' must be called to release this
//            pointer, if it is not NULL upon return.
//             If 'bDumpData' is NULL 'pTransactData','iTransactDataSize' will always be NULL,0,
//            upon return regardless of whether anything was received from the server or not.
//
//       Function in progress:
//       ---------------------
//          -  When function is executing it may be in a number of states, like
//             requesting data, sending data, receiving data, etc...
//             Every such state has an appropriate callback ID and see the desriptions
//             of TG_CALLBACK_EVENT_* for appropriate usage for
//            'pTransactData','iTransactDataSize' for every event ID.
//
//
//    char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
//       If upon termination function return somethng other, than 'TG_RETCODE_SUCCESS'
//       'acErrorText' might contain the text description of error code.
//       Check 'acErrorText[0]' for presence of error message.
//
//    int            iWSALastErrorCode;            // last WSA error.
//       'iWSALastErrorCode' will contain sockets-specific error code , in case
//      there is TCP/IP-related network error occured.
//
//
//    VOID           *pvUserValue;                 // Caller's-defined value;
//    DWORD          dwUserValue;                  // Caller's-defined value;
//       Function will not use these values. Caller is free to use these values for
//       his own purposes, such as to store context information, pointers to
//       his own structures etc...

typedef struct _DATA_BUFFER
{
   char  *pBuffer;
   int   iSize;
} DATA_BUFFER;

DWORD Allocate_TransactWithServer (TRANSACT_INFO_STRUCTURE *pTIS)
{
//---------------------------------------------------------------------------
   SOCKET               sSocket;
   struct sockaddr_in   sSockAddrIn_Local;
   struct sockaddr_in   sSockAddrIn_Remote;

   // --- SSL specific stuff ---
   SSL                  *pSsl        = NULL;

   struct timeval       tvTimeVal;
   struct fd_set        FDSet;
   unsigned int         uiBytesToRead;
   int                  i;

   DATA_BUFFER          *pDataBuffer       = NULL;
   DATA_BUFFER          **ppArrayOfBuffers = NULL;
   int                  iArrayOfBuffersNum = 0;
   char                 *pFinalBuffer;
   DWORD                dwFinalBufferSize;
   DWORD                dwBytesCopiedAlready;

   DWORD                dwStartTimer, dwTimerValue;

   int                  iWSALastErrorCode;
   BOOL                 bRetCode;
   int                  iRetCode;
   int                  iTGRetValue;
//---------------------------------------------------------------------------


// struct timeval
// {
//    long    tv_sec;         /* seconds */
//    long    tv_usec;        /* and microseconds */
// };

// typedef struct fd_set
// {
//    u_int    fd_count;               // How many are SET? (The number of sockets that are set).
//    SOCKET   fd_array [FD_SETSIZE];  // An array of SOCKETs, that are set.
// } fd_set;

   dwStartTimer = StartTimer();

   pTIS->acErrorText[0] = 0;
   iWSALastErrorCode     = 0;
   iTGRetValue           = 0;

   memset (&tvTimeVal, 0, sizeof (tvTimeVal));

   sSocket =
      socket ( // SOCKET: INVALID_SOCKET:fail, else:success.
         AF_INET,       // int af:        AF_*.
         SOCK_STREAM,   // int type,      SOCK_[STREAM|DGRAM].
         0              // int protocol   *PROTO_*|0:use default for given type of socket.
         );
   if (sSocket == INVALID_SOCKET)
      {
      iWSALastErrorCode = WSAGetLastError ();
      sprintf (pTIS->acErrorText, "socket() failed");
      iTGRetValue = TG_RETCODE_ERROR;
      goto  DONE;
      }

// TG_RETCODE_SUCCESS          - Server disconnected. Possibly some data retrieved. No errors.
// TG_RETCODE_USER_ABORT       - Callback returned FALSE.
// TG_RETCODE_TIMEOUT          - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
// TG_RETCODE_ERROR            - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->acErrorText' has error text

// WINSOCK.H:
// Structure used by kernel to store most addresses.
//
// struct sockaddr {
//       u_short sa_family;   // address family.
//       char    sa_data[14]; // up to 14 bytes of direct address.
//       };
//
// struct in_addr
//    {
//    union
//       {
//       struct { u_char  s_b1,s_b2,s_b3,s_b4; }   S_un_b;
//       struct { u_short s_w1,s_w2; }             S_un_w;
//       u_long S_addr;
//       } S_un;
//    };
//
// Socket address, internet style.
// For AF_INET 'sockaddr' will be the following 'sockaddr_in':
// struct sockaddr_in      // == sockaddr for TCP/IP:
//    {
//    short             sin_family;    // AF_[INET|*].
//    u_short           sin_port;      // 0:service provider assigned value: 1024-5000(use getsockname() to get assigned port number).
//    struct  in_addr   sin_addr;      // 4-byte internet addr. '.S_un.S_addr' = htonl(INADDR_ANY) - host machine.
//    char              sin_zero[8];   // Padding - must be 0 for 'bind()' call.
//    };
//

   memset (&sSockAddrIn_Local, 0, sizeof(sSockAddrIn_Local));
   sSockAddrIn_Local.sin_family           = AF_INET;
   sSockAddrIn_Local.sin_port             = htons (0); // if 0 => use getsockname() to get port after bind().
   sSockAddrIn_Local.sin_addr.s_addr      = htonl (INADDR_ANY);

   // bind() associates a local address with a socket.
   iRetCode =
      bind ( // int: 0:success, SOCKET_ERROR:error.
         sSocket,       // SOCKET s,
         (struct sockaddr *)&sSockAddrIn_Local, // const struct sockaddr *name: use 'sockaddr_in *' for AF_INET
         sizeof(sSockAddrIn_Local)  // int namelen
         );
   if (iRetCode == SOCKET_ERROR)
      {
      iWSALastErrorCode = WSAGetLastError ();
      sprintf (pTIS->acErrorText, "bind() failed");
      iTGRetValue = TG_RETCODE_ERROR;
      goto  DONE;
      }


   // Fill the structure for target connection.
   bRetCode =
      Fill_sockaddr_in_Structure (
         pTIS->acTargetNameOrIP,  // char           *pHostNameOrIPAddress,
         pTIS->usTargetPort,     // unsigned short usPortNumber,
         &sSockAddrIn_Remote,    // struct sockaddr_in    *pSockAddr,    // Caller' allocated space for structure.
         &iRetCode               // int            *piLastError   /* = NULL */ // WinSock's last error or -1.
         );
   if (!bRetCode)
      {
      iWSALastErrorCode = iRetCode;
      sprintf (pTIS->acErrorText, "Gateway unreachable or unresolvable");
      iTGRetValue = TG_RETCODE_ERROR;
      goto  DONE;
      }


   // Check timeout condition.
   if (pTIS->dwTotalTimeout <= QueryTimer(dwStartTimer))
      {
      iTGRetValue = TG_RETCODE_TIMEOUT;   // Timeout!
      goto  DONE;
      }


   iRetCode =
      connect ( // int: 0:success, SOCKET_ERROR:error.
         sSocket,          // SOCKET s,
         (struct sockaddr *)&sSockAddrIn_Remote,   // const struct sockaddr *name
         sizeof (sSockAddrIn_Remote)   // int namelen
         );
   if (iRetCode == SOCKET_ERROR)
      {
      iWSALastErrorCode = WSAGetLastError ();
      sprintf (pTIS->acErrorText, "Cannot connect to the server");
      iTGRetValue = TG_RETCODE_ERROR;
      goto  DONE;
      }

   // Check timeout condition.
   if (pTIS->dwTotalTimeout <= QueryTimer(dwStartTimer))
      {
      iTGRetValue = TG_RETCODE_TIMEOUT;   // Timeout!
      goto  DONE;
      }

   if (pTIS->pfn_EventCallback)
      {
      pTIS->dwEventID         = TG_CALLBACK_EVENT_CONNECTED;
      pTIS->pTransactData     = NULL;
      pTIS->iTransactDataSize = 0;
      bRetCode = pTIS->pfn_EventCallback (pTIS);
      if (!bRetCode)
         {
         iTGRetValue = TG_RETCODE_USER_ABORT;   // User's callback returned FALSE. Aborting...
         goto  DONE;
         }
      }


   if (pTIS->bUseSSL)
      {
      // TCP/IP connection established. Do SSL negotiation.

      // Create an SSL structure.
      pSsl = SSL_new (pTIS->pSslContext);

      // Give it a file/socket description to use.
      SSL_set_fd (pSsl, sSocket);

      // Do connection.
      iRetCode = SSL_connect (pSsl);   // -1:error
      if (iRetCode == -1)
         {
         iWSALastErrorCode = -1;
         sprintf (pTIS->acErrorText, "SSL_connect() failed.");
         iTGRetValue = TG_RETCODE_ERROR;
         goto  DONE;
         }

#if defined (DONT_WASTE_TIME_ON_THAT)
      // Printf SSL connection parameters.
      //
      X509  *pServerCertif;
      char  *pStr;
      textattr (FOREGROUND_GREEN + FOREGROUND_BLUE + FOREGROUND_INTENSITY);

      printf ("\nSSL connection using: --------> %s", SSL_get_cipher (pSsl));

      // Get server's certificate (note: beware of dynamic allocation) - opt
      pServerCertif = SSL_get_peer_certificate (pSsl);
      if (pServerCertif)
         {
         pStr = X509_NAME_oneline (X509_get_subject_name (pServerCertif), 0, 0);
         printf ("\nServer certificate subject: --> %s", pStr?pStr:"???");
         Free (pStr);

         pStr = X509_NAME_oneline (X509_get_issuer_name  (pServerCertif), 0, 0);
         printf ("\nServer certificate issuer: ---> %s", pStr?pStr:"???");
         Free (pStr);

         // We could do all sorts of certificate verification stuff here before
         // deallocating the certificate.

         X509_free (pServerCertif);
         }

      textattr (g_wOrigAttrs);
#endif
      }


   // do in a loop.
   // -  Get data to send.
   // -  Send data

   do
      {
      // Check timeout condition.
      if (pTIS->dwTotalTimeout <= QueryTimer(dwStartTimer))
         {
         iTGRetValue = TG_RETCODE_TIMEOUT;   // Timeout!
         goto  DONE;
         }

      if (pTIS->pfn_EventCallback)
         {
         // Retrieve data to send.
         pTIS->dwEventID         = TG_CALLBACK_EVENT_REQUEST_DATA;
         // Reset buffer. User must initialize it from callback.
         pTIS->pTransactData      = NULL;
         pTIS->iTransactDataSize  = 0;
         bRetCode = pTIS->pfn_EventCallback (pTIS);
         if (!bRetCode)
            {
            iTGRetValue = TG_RETCODE_USER_ABORT;   // User's callback returned FALSE. Aborting...
            goto  DONE;
            }
         }

      if (!pTIS->pTransactData || !pTIS->iTransactDataSize)
         break;   // No more (or could be that never was) any data to send.

      if (pTIS->bUseSSL)
         {
         iRetCode =
            SSL_write (pSsl, pTIS->pTransactData, pTIS->iTransactDataSize);
         }
      else
         {
         iRetCode =
            send ( // int: SOCKET_ERROR:error, else:bytes sent: <= 'len'.
               sSocket,             // SOCKET      s,
               pTIS->pTransactData,     // const char  *buf,
               pTIS->iTransactDataSize, // int         len,
               0                    // int         flags: MSG_[DONTROUTE|OOB].
               );
         }
      if (iRetCode == SOCKET_ERROR)
         {
         iWSALastErrorCode = WSAGetLastError ();
         sprintf (pTIS->acErrorText, "Error sending data to the server");
         iTGRetValue = TG_RETCODE_ERROR;
         goto  DONE;
         }


      if (pTIS->pfn_EventCallback)
         {
         pTIS->dwEventID         = TG_CALLBACK_EVENT_DATA_SENT;
         // pTIS->pTransactData - already initialized.
         pTIS->iTransactDataSize  = iRetCode;   // Number of bytes actually sent
         bRetCode = pTIS->pfn_EventCallback (pTIS);
         if (!bRetCode)
            {
            iTGRetValue = TG_RETCODE_USER_ABORT;   // User's callback returned FALSE. Aborting...
            goto  DONE;
            }
         }
      else
         break;   // No callback - sent only once!
      }
#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while (1);
#pragma  warning (default : 4127)


// #define  TG_CALLBACK_EVENT_CONNECTED            1
// #define  TG_CALLBACK_EVENT_REQUEST_DATA         2
// #define  TG_CALLBACK_EVENT_DATA_SENT            3
// #define  TG_CALLBACK_EVENT_DATA_RECEIVED        4
// #define  TG_CALLBACK_EVENT_NO_DATA_READY        5
//
// typedef struct _TRANSACT_INFO_STRUCTURE
// {
//    char           acTargetNameOrIP [MAX_PATH];
//    unsigned short usTargetPort;
//    BOOL           bUseSSL;
//    SSL_CTX        *pSslContext;                 // Used for SSL connections.
//    DWORD          dwTotalTimeout;               // INFINITE|(ms) Stay in this function max. this time.
//    DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
//    BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.
//
//    BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
//    DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
//    char           *pTransactData;               // Multipurpose I/O buffer.
//    int            iTransactDataSize;            // I/O buffer size.
//    char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
//    int            iWSALastErrorCode;            // last WSA error.
//
//    VOID           *pvUserValue;                 // Caller's-defined value;
//    DWORD          dwUserValue;                  // Caller's-defined value;
// } TRANSACT_INFO_STRUCTURE;



   // Receive loop.

// TG_RETCODE_SUCCESS          - Server disconnected. Possibly some data retrieved. No errors.
// TG_RETCODE_USER_ABORT       - Callback returned FALSE.
// TG_RETCODE_TIMEOUT          - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
// TG_RETCODE_ERROR            - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->acErrorText' has error text

   do
      {
      // Check timeout condition.
      dwTimerValue = QueryTimer(dwStartTimer);
      if (pTIS->dwTotalTimeout <= dwTimerValue)
         {
         iTGRetValue = TG_RETCODE_TIMEOUT;
         break;   // Timeout!
         }

//      tvTimeVal.tv_sec  = (dwTotalTimeout - dwTimerValue)/1000;
//      tvTimeVal.tv_usec = ((dwTotalTimeout - dwTimerValue)%1000)*1000; // Convert remainder to microseconds.

      // Wait in 0.5s chunks to provide capability to check for user's
      // event once in a little while.
      memset (&FDSet, 0, sizeof (FDSet));
      FDSet.fd_count     = 1;
      FDSet.fd_array [0] = sSocket;

      if (pTIS->dwTotalTimeout == INFINITE || ((pTIS->dwTotalTimeout - dwTimerValue) > pTIS->dwCallbackTimeout))
         {
         tvTimeVal.tv_sec  = pTIS->dwCallbackTimeout / 1000;
         tvTimeVal.tv_usec = (pTIS->dwCallbackTimeout % 1000)*1000;
         }
      else
         {
         DWORD    dwRemainingTime = pTIS->dwTotalTimeout - dwTimerValue;
         tvTimeVal.tv_sec  = dwRemainingTime / 1000;
         tvTimeVal.tv_usec = (dwRemainingTime % 1000)*1000;
         }

      iRetCode =
         select ( // int: 0:timeout and no sockets ready, >0:number of ready sockets, else:SOCKET_ERROR(WSAGetLastError()).
            0,          // int nfds:   ignored.
            &FDSet,     // ifd_set FAR * readfds:  NULL|pointer to a set of sockets to be checked for readability. Refilled by call.
            NULL,       // ifd_set FAR * writefds: NULL|pointer to a set of sockets to be checked for writability. Refilled by call.
            NULL,       // ifd_set FAR * exceptfds:   NULL|pointer to a set of sockets to be checked for errors.   Refilled by call.
            &tvTimeVal  // iconst struct timeval FAR * timeout: struct timeval {long tv_sec;/*sec*/ long tv_usec;/*microsec*/ };
            );
      if (iRetCode == SOCKET_ERROR)
         {
         iWSALastErrorCode = WSAGetLastError ();
         sprintf (pTIS->acErrorText, "select() failed.");
         iTGRetValue = TG_RETCODE_ERROR;
         break;
         }


      // No data ready yet.
      if (iRetCode == 0)
         {
         if (pTIS->pfn_EventCallback)
            {
            pTIS->dwEventID         = TG_CALLBACK_EVENT_NO_DATA_READY;
            pTIS->pTransactData      = NULL;
            pTIS->iTransactDataSize  = 0;
            bRetCode = pTIS->pfn_EventCallback (pTIS);
            if (!bRetCode)
               {
               iTGRetValue = TG_RETCODE_USER_ABORT;   // User's callback returned FALSE. Aborting...
               break;
               }
            }
         continue;
         }


      // Data is ready!
      // Determine how much data is avl. for reading. We need to know that to avoid blocking.
      uiBytesToRead = 0;
      iRetCode =
         ioctlsocket (  // int: 0:success, else:SOCKET_ERROR(WSAGetLastError()).
            sSocket,       // SOCKET   s:   socket.
            FIONREAD,      // long     cmd: FION[BIO|READ],SIOCATMARK
            (u_long FAR*)&uiBytesToRead // u_long FAR* argp: ptr to cmd'specific param: 'unsigned long *' for FIONREAD.
            );
      if (iRetCode == SOCKET_ERROR)
         {
         iWSALastErrorCode = WSAGetLastError ();
         sprintf (pTIS->acErrorText, "ioctlsocket() failed.");
         iTGRetValue = TG_RETCODE_ERROR;
         break;
         }

      if (!uiBytesToRead)
         {
         // Connection closed (socket is "ready", but 0 bytes to read).
         iTGRetValue = TG_RETCODE_SUCCESS;
         break;
         }

// typedef _DATA_BUFFER
// {
//    char  *pBuffer;
//    int   iSize;
// } DATA_BUFFER

      pDataBuffer = new DATA_BUFFER;
      pDataBuffer->pBuffer = new char [uiBytesToRead+1];
      pDataBuffer->iSize   = uiBytesToRead;

      // Receive response from server.
      if (pTIS->bUseSSL)
         {
         iRetCode =
            SSL_read (pSsl, pDataBuffer->pBuffer, pDataBuffer->iSize);
         }
      else
         {
         iRetCode =
            recv ( // int: 0:connection closed, SOCKET_ERROR:error occured, else:number of bytes read.
               sSocket,       // SOCKET   s,
               pDataBuffer->pBuffer,   // char     *buf: buffer.
               pDataBuffer->iSize,     // int      len:  buffer size.
               0              // int      flags: MSG_[PEEK|OOB].
               );
         }
      if (iRetCode == SOCKET_ERROR)
         {
         iWSALastErrorCode = WSAGetLastError ();
         if (pTIS->bUseSSL)
            sprintf (pTIS->acErrorText, "SSL_read() failed.");
         else
            sprintf (pTIS->acErrorText, "recv() failed.");
         iTGRetValue = TG_RETCODE_ERROR;
         delete [] pDataBuffer->pBuffer;
         delete pDataBuffer;
         break;
         }

      if (pTIS->pfn_EventCallback && iRetCode)
         {
         pDataBuffer->pBuffer [iRetCode] = 0;   // Zero-terminate buffer.
         pTIS->dwEventID         = TG_CALLBACK_EVENT_DATA_RECEIVED;
         pTIS->pTransactData     = pDataBuffer->pBuffer;
         pTIS->iTransactDataSize = iRetCode; // Number of bytes actually received.
         bRetCode = pTIS->pfn_EventCallback (pTIS);
         if (!bRetCode)
            {
            delete [] pDataBuffer->pBuffer;
            delete pDataBuffer;
            iTGRetValue = TG_RETCODE_USER_ABORT;   // User's callback returned FALSE. Aborting...
            break;
            }
         }

      if (pTIS->bDumpData)
         {
         delete [] pDataBuffer->pBuffer;
         pDataBuffer->pBuffer = NULL;
         }

      if (iRetCode)
         AddPtrToArray ((void ***)&ppArrayOfBuffers, &iArrayOfBuffersNum, (const void *)pDataBuffer);
      else
         {
         iTGRetValue = TG_RETCODE_SUCCESS;
         break;   // 0 => Connection closed.
         }
      }
#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while (1);
#pragma  warning (default : 4127)


DONE:

   // Send SSL/TLS close_notify.
   if (pTIS->bUseSSL)
      {
      if (pSsl)
         {
         // So that SSL lib will not get creative and won't close sockets by itself.
         BIO_set_close (SSL_get_rbio (pSsl), BIO_NOCLOSE);
         BIO_set_close (SSL_get_wbio (pSsl), BIO_NOCLOSE);
         SSL_shutdown (pSsl);
         }
      }

   if (sSocket != INVALID_SOCKET && sSocket != SOCKET_ERROR)
      {
      // To ensure graceful disconnect.
      iRetCode =
         shutdown (
            sSocket, // SOCKET   s,
            SD_BOTH  // int      how
            );
      iRetCode = closesocket (sSocket);
      sSocket = INVALID_SOCKET;
      }

   if (pTIS->bUseSSL)
      {
      if (pSsl)
         SSL_free (pSsl);
      }

// typedef _DATA_BUFFER
// {
//    char  *pBuffer;
//    int   iSize;
// } DATA_BUFFER

   // Calculate total buffer size.
   //
   for (dwFinalBufferSize=0, i=0; i<iArrayOfBuffersNum; i++)
      dwFinalBufferSize += ppArrayOfBuffers[i]->iSize;

   if (!pTIS->bDumpData && dwFinalBufferSize)
      {
      // Reserve extra byte to make easier to printf resulted buffer.
      pFinalBuffer = new char [dwFinalBufferSize + 1];

      memset (pFinalBuffer, 0, dwFinalBufferSize + 1);

      dwBytesCopiedAlready = 0;
      for (i=0; i<iArrayOfBuffersNum; i++)
         {
         memcpy (pFinalBuffer+dwBytesCopiedAlready, ppArrayOfBuffers[i]->pBuffer, ppArrayOfBuffers[i]->iSize);
         dwBytesCopiedAlready += ppArrayOfBuffers[i]->iSize;
         }
      }
   else
      pFinalBuffer = NULL;

   // Now free array of ptrs.
   for (i=0; i<iArrayOfBuffersNum; i++)
      {
      delete [] ppArrayOfBuffers[i]->pBuffer;
      delete ppArrayOfBuffers[i];
      }
   FreeArrayOfPtrs   ((void ***)&ppArrayOfBuffers, &iArrayOfBuffersNum);


// #define  TG_CALLBACK_EVENT_CONNECTED            1
// #define  TG_CALLBACK_EVENT_REQUEST_DATA        2
// #define  TG_CALLBACK_EVENT_DATA_SENT            3
// #define  TG_CALLBACK_EVENT_DATA_RECEIVED        4
// #define  TG_CALLBACK_EVENT_NO_DATA_READY        5
//
// typedef struct _TRANSACT_INFO_STRUCTURE
// {
//    char           acTargetNameOrIP [MAX_PATH];
//    unsigned short usTargetPort;
//    BOOL           bUseSSL;
//    SSL_CTX        *pSslContext;                 // Used for SSL connections.
//    DWORD          dwTotalTimeout;               // INFINITE|(ms) Stay in this function max. this time.
//    DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
//    BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.
//
//    BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
//    DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
//    char           *pTransactData;               // Multipurpose I/O buffer.
//    int            iTransactDataSize;            // I/O buffer size.
//    char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
//    int            iWSALastErrorCode;            // last WSA error.
//
//    VOID           *pvUserValue;                 // Caller's-defined value;
//    DWORD          dwUserValue;                  // Caller's-defined value;
// } TRANSACT_INFO_STRUCTURE;


   // Return values.
   pTIS->pTransactData      = pFinalBuffer;
   pTIS->iTransactDataSize  = dwFinalBufferSize;

   pTIS->iWSALastErrorCode = iWSALastErrorCode;

   return (iTGRetValue);
}
//===========================================================================

//===========================================================================
//
//

VOID  Free_TransactWithServer (
         char     *pData            // Ptr to allocated buffer with data, as it was passed to 'Allocate_ReadDataFromBlockedSocket()'.
         )
{
   delete [] pData;
}
//===========================================================================

BOOL  CompareParms (char *pParm1, char *pParm2);

//===========================================================================
//
// Returns:
//    NULL:    all parameters are ok.
//    else:    ptr to invalid paramter.

char  *ValidateParameters (char **ppArgs, int iArgsNum)
{
//---------------------------------------------------------------------------
    int             i;
//---------------------------------------------------------------------------


   for (i=1; i<iArgsNum; i++)
      {
      if (ppArgs[i][1] == ':')
         // Invalid parameter found! Probably C:\DIR specified, instead of dir=c:\dir
         return (ppArgs[i]);

      if (ppArgs[i][0] != '/')
         continue;   // It is probably filespec.

      if (CompareParms (ppArgs[i], "/dir="            ))  continue;
      if (CompareParms (ppArgs[i], "/gateway="        ))  continue;
      if (CompareParms (ppArgs[i], "/port="           ))  continue;
      if (CompareParms (ppArgs[i], "/ssl"             ))  continue;
      if (CompareParms (ppArgs[i], "/samthreads="     ))  continue;
      if (CompareParms (ppArgs[i], "/samdelay="       ))  continue;
      if (CompareParms (ppArgs[i], "/samerrorsmax="   ))  continue;
      if (CompareParms (ppArgs[i], "/sigthreads="     ))  continue;
      if (CompareParms (ppArgs[i], "/sigdelay="       ))  continue;
      if (CompareParms (ppArgs[i], "/sigerrorsmax="   ))  continue;
      if (CompareParms (ppArgs[i], "/mindelay="       ))  continue;
      if (CompareParms (ppArgs[i], "/maxdelay="       ))  continue;
      if (CompareParms (ppArgs[i], "/seed="           ))  continue;
      if (CompareParms (ppArgs[i], "/s"               ))  continue;
      if (CompareParms (ppArgs[i], "/minsize="        ))  continue;
      if (CompareParms (ppArgs[i], "/maxsize="        ))  continue;
      if (CompareParms (ppArgs[i], "/:minsize="       ))  continue;
      if (CompareParms (ppArgs[i], "/:maxsize="       ))  continue;
      if (CompareParms (ppArgs[i], "/zapbits"         ))  continue;
      if (CompareParms (ppArgs[i], "/zapnames"        ))  continue;
      if (CompareParms (ppArgs[i], "/maxfiles="       ))  continue;
      if (CompareParms (ppArgs[i], "/maxsigs="        ))  continue;
      if (CompareParms (ppArgs[i], "/maxsqs="         ))  continue;
      if (CompareParms (ppArgs[i], "/sqdelay="        ))  continue;
      if (CompareParms (ppArgs[i], "/sqerrorsmax="    ))  continue;
      if (CompareParms (ppArgs[i], "/sqthreads="      ))  continue;
      if (CompareParms (ppArgs[i], "/getsums="        ))  continue;
      if (CompareParms (ppArgs[i], "/setsums="        ))  continue;

      if (CompareParms (ppArgs[i], "/loop"            ))  continue;
      if (CompareParms (ppArgs[i], "/verbose"         ))  continue;
      if (CompareParms (ppArgs[i], "/sammanual"       ))  continue;
      if (CompareParms (ppArgs[i], "/sigskipdownload" ))  continue;
      if (CompareParms (ppArgs[i], "/noask"           ))  continue;
      if (CompareParms (ppArgs[i], "/profile="        ))  continue;

      // Invalid parameter found!
      return (ppArgs[i]);
      }

   // All parameters are valid.
   return (NULL);
}
//===========================================================================

//===========================================================================
//
// Compare parameters until '=' or \0.

BOOL  CompareParms (char *pParm1, char *pParm2)
{
//---------------------------------------------------------------------------
    int             i;
//---------------------------------------------------------------------------

   for (i=0;;i++)
      {
      if (!pParm1[i] && !pParm2[i])
         return (TRUE);    // Both finished - Equal.

      if (!pParm1[i] || !pParm2[i])
         return (FALSE);   // One finished - other still. - Not equal.

      if (pParm1[i] == '=' && pParm2[i] == '=')
         return (TRUE);    // Both at '=' sign. Equal.

      if (toupper(pParm1[i]) != toupper(pParm2[i]))
         return (FALSE);
      }
}
//===========================================================================

//===========================================================================
//
//

VOID  AddErrorToArray (
         DWORD    ppdwErrorArray[][2],
         DWORD    dwErrorID,
         int      iSizeOfArray   /* = CRITICAL_ERROR_IDS_MAX */,
         DWORD    dwErrorsNumber /* = 1 */
         )
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------


   // Search for existing entry.
   for (i=0; i<iSizeOfArray; i++)
      {
      if (ppdwErrorArray[i][0] == dwErrorID)
         {
         ppdwErrorArray[i][1] += dwErrorsNumber;
         return;
         }
      }

   // Existing not found. Search for zero entry.
   for (i=0; i<(iSizeOfArray-1); i++)
      {
      if (!ppdwErrorArray[i][0])
         {
         ppdwErrorArray[i][0] = dwErrorID;
         ppdwErrorArray[i][1] = dwErrorsNumber; // Set number of errors.
         return;
         }
      }

   // Zero entry (free) not found too. Add as a last element: "all other errors".
   ppdwErrorArray[i][1] += dwErrorsNumber;
   return;
}
//===========================================================================


//===========================================================================
//
//

VOID Printf (LPCSTR fs, ...)
{
//---------------------------------------------------------------------------
   FILE           *fp;
   char           strBuf [512];
   va_list        argList;
//---------------------------------------------------------------------------

   va_start  (argList, fs);
   wvsprintf (strBuf,  fs, argList);
   va_end    (argList);

   printf (strBuf);

   // Creates file if not exists, appends data at the end of file.
   fp = fopen (g_acLogFilename, "a");

   if(fp)
      {
      fprintf(fp, "%s", strBuf);

      fclose (fp);
      }
}
//===========================================================================

//===========================================================================
//
// Derived from SSL\LATEST\mt\mttest.c

SSL_CTX  *SSL_Initialize (VOID)
{
//---------------------------------------------------------------------------
   int            i;
   SSL_METHOD     *pSslMethod  = NULL;
   SSL_CTX        *pSslContext = NULL;
//---------------------------------------------------------------------------

   for (i=0; i<CRYPTO_NUM_LOCKS; i++)
      g_SSL_lock_cs [i] = CreateMutex (NULL, FALSE, NULL);

   CRYPTO_set_locking_callback (SSL_win32_locking_callback);
   /* id callback defined */

   SSLeay_add_ssl_algorithms  ();
   pSslMethod = SSLv2_client_method ();
   SSL_load_error_strings     ();
   pSslContext = SSL_CTX_new  (pSslMethod);  // NULL:error

   return (pSslContext);
}
//===========================================================================

//===========================================================================
//
// Derived from SSL\LATEST\mt\mttest.c

VOID     SSL_Cleanup    (SSL_CTX *pSslContext)
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------

   CRYPTO_set_locking_callback (NULL);

   for (i=0; i<CRYPTO_NUM_LOCKS; i++)
      {
      if (g_SSL_lock_cs [i] && (g_SSL_lock_cs [i] != INVALID_HANDLE_VALUE))
         CloseHandle (g_SSL_lock_cs [i]);
      g_SSL_lock_cs [i] = INVALID_HANDLE_VALUE;
      }

   if (pSslContext)
      SSL_CTX_free (pSslContext);
}
//===========================================================================

//===========================================================================
//
// Derived from SSL\LATEST\mt\mttest.c

VOID  SSL_win32_locking_callback (int mode, int type, const char *file, int line)
{
   file;
   line;

   if (mode & CRYPTO_LOCK)
      WaitForSingleObject (g_SSL_lock_cs[type], INFINITE);
   else
      ReleaseMutex        (g_SSL_lock_cs[type]);
}
//===========================================================================

//===========================================================================
//
// Possible format of 'Location' reply:
// 'Location: https://another-one.watson.ibm.com:2847/AVIS/postSuspectSample'
// 'Location: http://another-one.watson.ibm.com/AVIS/postSuspectSample'
//
// Returns:
//    TRUE: All data (probably without port number) was successfully retrieved.
//    FALSE: Failed to retrieve required data elements.
//
// Note: If function will fail to retrieve a certain element, it will not
//       reset or re-initialize it. So effectively old value will be returned.

BOOL  ParseNewLocationData (
         const char     *pLocationStrBuffer,    // input - buffer with data, where "Location" text is.
         int            iLocationStrBufferSize, // input : int - size of above buffer.
         char           *pHostNameOrIP,      // output: must be at least MAX_PATH long.
         unsigned short *pusPortNumber,      // output:
         char           *pDirectoryName,     // output: must be at least MAX_PATH long.
         BOOL           *bUseSSL             // output.
         )
{
//---------------------------------------------------------------------------
   int            i;
   char           acLocationSpec [] = "Location:*http";

   char           *pFoundString;
   int            iFoundStringSize;
   char           acPortNumber [40];
   int            iPortNumber;
//---------------------------------------------------------------------------

   // Make sure string actually starts with "Location: http"...

   // Possible format of 'Location' reply:
   // 'Location: https://another-one.watson.ibm.com:2847/AVIS/postSuspectSample'
   // 'Location: http://another-one.watson.ibm.com/AVIS/postSuspectSample'


   // Checking for redirection here. Init 'bRedirected' if TRUE and all vars are initialized.
   pFoundString =
      StrStr  (
         pLocationStrBuffer,              // const char  *pBuffer,
         acLocationSpec,                  // const char  *pString,
         iLocationStrBufferSize,          // int         iBufferSize,
         FALSE,                           // BOOL        bCaseSensitive,
         TRUE,                            // BOOL        bSpecSearch /*=FALSE*/,   // TRUE:'?' and '*' considered as spec, not as 'pString' part.
         &iFoundStringSize                // int         *piFoundLen /*=NULL */
         );
   if (!pFoundString)
      return (FALSE);

   if (toupper(pFoundString [iFoundStringSize]) == 'S')
      {
      *bUseSSL = TRUE;
      iFoundStringSize ++;
      }
   else
      *bUseSSL = FALSE;

   iFoundStringSize += 3;

   // Initialize 'pHostNameOrIP'.
   for (i=0;
         pFoundString [iFoundStringSize + i] != ':' &&
         pFoundString [iFoundStringSize + i] != '/' &&
         pFoundString [iFoundStringSize + i] >  0x0D;
        i++
        )
      pHostNameOrIP [i] = pFoundString [iFoundStringSize + i];
   pHostNameOrIP [i] = 0;

   pFoundString += (iFoundStringSize + i);
   // Here '*pFoundString' points either on ':' or on '/'.
   //
   if (pFoundString [0] == ':')
      {
      pFoundString++;

      // Initialize port number...
      for (i=0; pFoundString[i] >= '0' && pFoundString[i] <= '9'; i++)
         acPortNumber [i] = pFoundString[i];

      acPortNumber [i] = 0;
      iPortNumber = atoi (acPortNumber);

      if (!iPortNumber)
         return (FALSE);

      *pusPortNumber = (unsigned short) iPortNumber;

      pFoundString += i;
      }

   // Here '*pFoundString' must point to '/'.
   // Initialize 'pDirectoryName'.
   for (i=0; pFoundString [i] > 0x0D; i++)
      pDirectoryName [i] = pFoundString [i];
   pDirectoryName [i] = 0;


   return (TRUE);
}
//===========================================================================

//===========================================================================
//
// Assembles array of ASCIIZ MD5 checksums, stored in text file.
// d61b78b659c389f5d74ff7d5e3bf6ccf = 32 chars.

VOID  LoadCheckSumsFromFile (
         const char  *pFilename,
         char        ***pppCheckSumsArray,
         int         *piCheckSumsInArray
         )
{
//---------------------------------------------------------------------------
   HANDLE            hFile       = INVALID_HANDLE_VALUE;
   char              *pBuffer    = NULL;
   DWORD             dwFileSize, dwBytesRead;
   BOOL              bRetCode;
   char              acNextCRC [33];
   int               i, j;
//---------------------------------------------------------------------------

   *pppCheckSumsArray  = NULL;
   *piCheckSumsInArray = 0;

   hFile =
      CreateFile ( // HANDLE: INVALID_HANDLE_VALUE:error, else:handle.
         pFilename,        // LPCTSTR  lpFileName:   address of name of the file
         GENERIC_READ,     // DWORD  dwDesiredAccess:   access (read-write) mode
         FILE_SHARE_READ | FILE_SHARE_WRITE,  // DWORD  dwShareMode: share mode
         NULL,             // LPSECURITY_ATTRIBUTES  lpSecurityAttributes:   address of security descriptor
         OPEN_EXISTING,    // DWORD  dwCreationDistribution:  how to create
         FILE_ATTRIBUTE_NORMAL, // DWORD  dwFlagsAndAttributes: file attributes
         NULL              // HANDLE  hTemplateFile: handle of file with attributes to copy
         );

   if (hFile == INVALID_HANDLE_VALUE)
      goto  EXIT;

   dwFileSize = GetFileSize (hFile, NULL);

   pBuffer = new char [dwFileSize];
   if (!pBuffer)
      goto  EXIT;

   // Read file data.
   bRetCode =
      ReadFile ( // BOOL: fail=>0, else != 0.
         hFile,    // HANDLE hFile: handle of file to read
         (VOID *)pBuffer,  // LPVOID lpBuffer: address of buffer that receives data
         dwFileSize,       // DWORD nNumberOfBytesToRead:  number of bytes to read
         &dwBytesRead,     // LPDWORD lpNumberOfBytesRead: address of number of bytes read
         NULL              // LPOVERLAPPED lpOverlapped:   address of structure for data
         );

   if (!bRetCode || dwFileSize != dwBytesRead)
      goto  EXIT;

   for (i=0; i<(int)dwFileSize; i++)
      {
      // Skip non-alnum
      for (j=0; ((j+i) < (int)dwFileSize) && (pBuffer[j+i] < '0'); j++)
         ;

      if ((j+i) >= (int)dwFileSize)
         break;   // End of file.

      // Alnum begins here.
      i+=j;

      for (j=0; ((j+i) < (int)dwFileSize) && (pBuffer[j+i] >= '0') && (j<32); j++)
         acNextCRC [j] = pBuffer [j+i];

      if (j==32)  // Break because 'acNextCRC' is full.
         {
         acNextCRC [j] = 0;
         if (ValidateCRCBuffer (acNextCRC))  // Make sure it contains only 0-9, A-F chars.
            {
            char  *pTmp = new char [33];
            strcpy (pTmp, acNextCRC);
            AddStringToArray (pppCheckSumsArray, piCheckSumsInArray, pTmp);
            }
         }

      if ((j+i) >= (int)dwFileSize)
         break;   // End of file.

      i+=j;
      i--;
      }

EXIT:
   if (hFile != INVALID_HANDLE_VALUE)
      CloseHandle (hFile);
   delete [] pBuffer;
   return;
}
//===========================================================================

//===========================================================================
//
//

VOID  SaveCheckSumsToFile (
         const char  *pFilename,
         char        **ppCheckSumsArray,
         int         iCheckSumsInArray,
         DWORD       dwMaxFileSize /* = 0xFFFFFFFF */ // 0xFFFFFFFF:unlimited.
         )
{
//---------------------------------------------------------------------------
   HANDLE            hFile       = INVALID_HANDLE_VALUE;
   DWORD             dwFileSize;
   DWORD             dwBytes;
   BOOL              bRetCode;
   char              acBuffer [32 + 2];   // + 2 - for 0x0D, 0x0A
   int               i, j;
//---------------------------------------------------------------------------

   hFile =
      CreateFile ( // HANDLE: INVALID_HANDLE_VALUE:error, else:handle.
         pFilename,        // LPCTSTR  lpFileName:   address of name of the file
         GENERIC_READ | GENERIC_WRITE,     // DWORD  dwDesiredAccess:   access (read-write) mode
         FILE_SHARE_READ | FILE_SHARE_WRITE,  // DWORD  dwShareMode: share mode
         NULL,             // LPSECURITY_ATTRIBUTES  lpSecurityAttributes:   address of security descriptor
         OPEN_ALWAYS,      // DWORD  dwCreationDistribution:  how to create
         FILE_ATTRIBUTE_NORMAL, // DWORD  dwFlagsAndAttributes: file attributes
         NULL              // HANDLE  hTemplateFile: handle of file with attributes to copy
         );

   if (hFile == INVALID_HANDLE_VALUE)
      goto  EXIT;

   dwFileSize = GetFileSize (hFile, NULL);

   // Make sure file size does not exceed limit.
   if (dwMaxFileSize != 0xFFFFFFFF && (dwFileSize >= dwMaxFileSize))
      goto  EXIT;

   // Make sure to append to file.
   SetFilePointer ( // DWORD low order DWORD of new ptr. 0xFFFFFFFF:error.
      hFile,               // HANDLE hFile:  handle of file
      0,                   // LONG lDistanceToMove:   number of bytes to move file pointer
      NULL,                // PLONG lpDistanceToMoveHigh:   address of high-order word of distance to move
      FILE_END             // DWORD dwMoveMethod:  how to move
      );

   for (i=0; i<iCheckSumsInArray; i++)
      {
      acBuffer [0] = 0x0D;
      acBuffer [1] = 0x0A;
      for (j=2; j<34; j++)
         acBuffer [j] = ppCheckSumsArray[i][j-2];

      bRetCode =
         WriteFile ( // BOOL: fail=>0, else != 0.
            hFile,        // HANDLE hFile: handle to file to write to
            acBuffer,      // LPCVOID lpBuffer:   pointer to data to write to file
            sizeof (acBuffer),          // DWORD nNumberOfBytesToWrite: number of bytes to write
            &dwBytes,  // LPDWORD lpNumberOfBytesWritten: pointer to number of bytes written
            NULL              // LPOVERLAPPED lpOverlapped:   pointer to structure needed for overlapped I/O
            );
      }


EXIT:
   if (hFile != INVALID_HANDLE_VALUE)
      CloseHandle (hFile);
   return;
}
//===========================================================================

//===========================================================================
//
// Makes sure, that buffer contains only 0-9, A-F.

BOOL  ValidateCRCBuffer (const char *pCRCBuffer)
{
//---------------------------------------------------------------------------
   int            cChar;
   int            i;
//---------------------------------------------------------------------------

   for (i=0; pCRCBuffer [i] && i<32; i++)
      {
      cChar = toupper (pCRCBuffer [i]);
      if (
         (cChar >='0' && cChar <= '9') ||
         (cChar >='A' && cChar <= 'F')
         )
         continue;
      else
         break;
      }

   if (i<32)
      return (FALSE);   // Buffer too small.

   return (TRUE);
}
//===========================================================================

//===========================================================================
//
// This call will be used collectively by all the sample uploading threads.

CriticalSection      g_CheckSumAccess;

VOID  StoreChecksumToArray (
         const char  *pCheckSum,             // ASCIIZ MD5 checksum to store to array.
         int         iMaxAllowedChecksums,   // Store no more than this number of checksums.
         char        ***pppCheckSums,
         int         *piCheckSumsNum
         )
{
//---------------------------------------------------------------------------
   CriticalSection   cs (&g_CheckSumAccess);
   char              *pTmp;
   int               i;
//---------------------------------------------------------------------------

   if (*piCheckSumsNum >= iMaxAllowedChecksums)
      return;

   for (i=0; i<*piCheckSumsNum; i++)
      if (!strcmpi (pCheckSum, (*pppCheckSums)[i]))
         return;  // This checksum already exists in array.

   pTmp = new char [32 + 1];
   strcpy (pTmp, pCheckSum);

   AddStringToArray (pppCheckSums, piCheckSumsNum, pTmp);

   return;
}
//===========================================================================








