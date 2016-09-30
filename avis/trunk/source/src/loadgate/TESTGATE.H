//===========================================================================
// 
//                               TESTGATE.H
// 
// 
// 
//===========================================================================



#ifndef  _TESTGATE_H_
#define  _TESTGATE_H_


//---------------------------------------------------------------------------
// Forward declarations.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Definitions.

// 'Allocate_TransactWithServer()' return codes.
#define  TG_RETCODE_SUCCESS                     0           //  - Server disconnected. Possibly some data retrieved. No errors.
#define  TG_RETCODE_USER_ABORT                  1           //  - Callback returned FALSE.
#define  TG_RETCODE_TIMEOUT                     2           //  - Server was still connected, when timeout occured. Possibly some data retrieved. No errors.
#define  TG_RETCODE_ERROR                       0xFFFFFFFF  //  - One of socket's APIs returned error. Possibly some data retrieved. 'pTIS->pErrorText' initialized.

// 'Allocate_TransactWithServer()' Callback's event codes.
#define  TG_CALLBACK_EVENT_CONNECTED            1           // Connection to target established. About to send data.
#define  TG_CALLBACK_EVENT_REQUEST_DATA         2           // Request for data buffer to send().
#define  TG_CALLBACK_EVENT_DATA_SENT            3           // Notification aobut completed send().
#define  TG_CALLBACK_EVENT_DATA_RECEIVED        4           // Non-zero buffer received fro client.
#define  TG_CALLBACK_EVENT_NO_DATA_READY        5           // No data ready yet from the client.


typedef struct _TRANSACT_INFO_STRUCTURE TRANSACT_INFO_STRUCTURE;

typedef struct _TRANSACT_INFO_STRUCTURE
{
   char           acTargetNameOrIP [MAX_PATH];
   unsigned short usTargetPort;
   BOOL           bUseSSL;
   SSL_CTX        *pSslContext;                 // Used for SSL connections.
   DWORD          dwTotalTimeout;               // INFINITE|(ms) Stay in this function max. this time.
   DWORD          dwCallbackTimeout;            // (ms) Call callback function approximately every 'dwCallbackTimeout' ms.
   BOOL           bDumpData;                    // TRUE:discard all data, although 'iDataSize' will still have a valid value.

   BOOL           (*pfn_EventCallback)(TRANSACT_INFO_STRUCTURE *pTIS); // Returns: TRUE:continue, FALSE:abort, cleanup and exit.
   DWORD          dwEventID;                    // TG_CALLBACK_EVENT_[CONNECTED|REQUEST_DATA|DATA_SENT|DATA_RECEIVED|NO_DATA_READY].
   char           *pTransactData;               // Multipurpose I/O buffer.
   int            iTransactDataSize;            // I/O buffer size.
   char           acErrorText [256];            // Buffer with error text. acErrorText[0]=0 | Non-CRLF/non-dotted "error message".
   int            iWSALastErrorCode;            // last WSA error.

   VOID           *pvUserValue;                 // Caller's-defined value;
   DWORD          dwUserValue;                  // Caller's-defined value;
} TRANSACT_INFO_STRUCTURE;

#define  EXCLUDE_CHAR            ':'

typedef struct _CONSTANT_INFO
{
   char        acLocalMachineName      [MAX_COMPUTERNAME_LENGTH + 1];// GetComputerName()
   char        acLocalUserName         [MAX_PATH];                   // GetUserName(): Currently logged on user.
   char        acGatewayNameOrIP       [MAX_PATH];                   // navajo.watson.ibm.com. From command line or from .INI file.
   int         iGatewayPort;                                         // 2847. From command line or from .INI file.
   char        acRegisteredSystemOwner [MAX_PATH];                   // "HKLM/Software/Microsoft/WindowsNT/CurrentVersion"
   char        acProcessorName         [200];                        // "HKLM/HARDWARE/DESCRIPTION/System/CentralProcessor/0/ *
   char        acPlatformScanner       [200];                        //  Norton anti-virus or TESTGATE.
   char        acOperatingSystemName   [200];                        // GetVersionEx();
   char        acCustomerContactName   [200];   
   char        acCustomerContactEmail  [200];
   char        acCustomerContactPhone  [200];
   char        acCustomerIdentifier    [200];

   char        acPlatformGUID          [40];
   char        acSampleUUID            [40];

   char        acConnectionType        [80];
   char        acContentType           [80];
   char        acAnalysisState         [80];
   char        acChecksumMethod        [80];
   char        acContentCompression    [80];
   char        acContentScrambling     [80];
   char        acIcePackInitializedState [80];
   char        acPlatformLanguage      [80];
   char        acScanResult            [80];
   int         iSampleChanges;
   int         iSampleFileID;
   int         iSamplePriority;
   char        acSampleReason          [80];
   int         iSampleStatus;
   char        acSampleStatusReadable  [80];
   int         iSampleSubmissionRoute;
   char        acSampleType            [80];
   int         iScanSignaturesSequence;
   char        acScanSignaturesVersion [80];
} CONSTANT_INFO;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Classes.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Function prototypes.

extern VOID  Allocate_HTTP_Header_For_SubmitSample (
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
         );
extern DWORD Allocate_TransactWithServer (TRANSACT_INFO_STRUCTURE *pTIS);
extern VOID  Free_TransactWithServer (
         char     *pData            // Ptr to allocated buffer with data, as it was passed to 'Allocate_ReadDataFromBlockedSocket()'.
         );
//---------------------------------------------------------------------------


#endif   // _TESTGATE_H_
