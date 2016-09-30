// ---------------------------------------------------------------------------
// File header goes here.
// ---------------------------------------------------------------------------

#ifndef _CIRCULAR_FILE_
#define _CIRCULAR_FILE_

#define CFAPPTYPE_NAV    0x3456414E     // Application ID: 'NAV4'
#define CFBITS_RESERVED  0xF0000000     // Reserved bits in dwRecordType

#define CFILE_VERCOMPAT   0x0100
#define CFILE_VERREAL     0x0100

// CFILE_Resize() flags

#define CFFLAG_RESIZE_KEEPNEW   0x0000
#define CFFLAG_RESIZE_KEEPOLD   0x0001
#define CFFLAG_RESIZE_UNWIND    0x0002  // Record #1 is first
//#define CFFLAG_RESIZE_COMPRESS  0x0004  // Make file as small as possible
//#define CFFLAG_RESIZE_EXPAND    0x0008  // Make file "full size"

                                        // &? This won't work because this
                                        //    header doesn't know about
                                        //    CFILEHEADER -- make it a fn.
                                        //    (or drop it for the app)
#define CFILE_MINHEADER   sizeof(CFILEHEADER)

                                        // file handle
typedef LPVOID HCFILE;
typedef HCFILE FAR * LPHCFILE;
                                        // error condinitons
typedef int CFILEERR;

#define CFERR_OK                ((CFILEERR) 0)
#define CFERR_OPENCREATE        ((CFILEERR)-1)
#define CFERR_INVALIDHANDLE     ((CFILEERR)-2)
#define CFERR_WRITE             ((CFILEERR)-3)
#define CFERR_READ              ((CFILEERR)-4)
#define CFERR_FLUSH             ((CFILEERR)-5)
#define CFERR_INVALIDRECSIZE    ((CFILEERR)-6)
#define CFERR_INVALIDBUFSIZE    ((CFILEERR)-7)
#define CFERR_INVALIDRECNUM     ((CFILEERR)-8)
#define CFERR_NOMEMORY          ((CFILEERR)-9)
#define CFERR_LESSRECORDSWRITTEN ((CFILEERR)-10)
#define CFERR_INTERNAL          ((CFILEERR)-99)

                                        // initialization struct used
                                        // during create, and returned to
                                        // application at open.
typedef struct tagCIRCULARFILEINFO
{
    DWORD   dwAppID;                    // Identifies specific file type
                                        // (Header ID is generic for engine)
    WORD    wRecSize;
    DWORD   dwMaxNumRecs;               // number of records file will hold

} CIRCULARFILEINFO, FAR * LPCIRCULARFILEINFO;
                                        // function prototypes
#ifdef SYM_DOS
    #ifdef __cplusplus
        extern "C" {
    #endif
#endif

CFILEERR WINAPI CFILE_Create ( LPSTR lpFile, LPCIRCULARFILEINFO, LPHCFILE );
CFILEERR WINAPI CFILE_Open ( LPSTR lpFile, LPCIRCULARFILEINFO, LPHCFILE );
CFILEERR WINAPI CFILE_Close ( LPHCFILE );
CFILEERR WINAPI CFILE_Add ( HCFILE, LPVOID lpRecord);
CFILEERR WINAPI CFILE_Write ( HCFILE, LPVOID lpRecord, DWORD dwRecNum);
CFILEERR WINAPI CFILE_Read ( HCFILE, LPVOID lpRecord, DWORD dwRecNum);
CFILEERR WINAPI CFILE_Flush ( HCFILE );
CFILEERR WINAPI CFILE_Resize ( HCFILE, DWORD dwNumRecs, WORD wResizeFlags );
CFILEERR WINAPI CFILE_BlockWriteStart ( HCFILE, DWORD dwNumRec );
CFILEERR WINAPI CFILE_BlockWriteEnd ( HCFILE );
DWORD    WINAPI CFILE_GetCount ( HCFILE hCFile );

#ifdef SYM_DOS
    #ifdef __cplusplus
        }
    #endif
#endif

#endif  // ifdef _CIRCULAR_FILE_

