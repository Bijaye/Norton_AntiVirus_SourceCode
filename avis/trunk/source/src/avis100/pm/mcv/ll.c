// FILE: \DOCFILE\LL.C

/* ll.c -- Low-level routines for dealing with the Real World (tm). In
 * this version, int file handles are used. */

/* Define RANDOM_MEM_FAIL while building to a value to get random *
 * malloc failures. This is for testing purposes only, of
 * course. Might make a good feature, though. */

/* To do: change file access to use buffered I/O by converting the
 * file handle to a stream. */

#ifdef T_VXD
 #include "iavvxd1.h"
 #include "basict.h"
#else
 /* for the real (non vxd) world: */
   #include <stdio.h>
   #ifdef MCV
      #include <string.h>
   #endif
//   #ifdef RANDOM_MEM_FAIL
//      #include <stdlib.h>
//      #include <time.h>
//   #endif
   #include <share.h>
   #include <io.h>
   #include <fcntl.h>
   #include <assert.h>
   #include <string.h>
   #ifndef __DEBUG_ALLOC__
      #ifdef S_H
         #include "mymalloc.h"
         #include <process.h>
      #else
         #include <malloc.h>
      #endif
   #else
      #include <malloc.h>
   #endif
#endif

#include "common.h"
#include "df.h"

#ifdef S_H
  #undef malloc
#endif

#if defined(C_MSC6)||defined(C_BC45)||defined(C_BCOS2)||defined(WATC)||defined(C_BC31)
#define _open open
#define _close close
#define _lseek lseek
#define _read read
#define _write write
#endif

#include "ll.h"

// debug code
#ifdef DEBUG_CODE
unsigned char  tempbuffer[FILEBUFFER_BUFFER];
unsigned char  *ptempbuffer;

unsigned char  tempbuffer1[FILEBUFFER_BUFFER];
unsigned char  *ptempbuffer1;

int buffcompare = 0;
int tempBytesRead;
int tempbytecounter;
int tempdiffcounter = 0;
#endif
// debug code

//Added on OCT/14/96 for redefining SOPEN call.
#ifdef S_H
   #include "nwsopen.h"
#endif

#if defined(T_WINNT) && !defined(MCV)
#include "platfuns.h" //WinNT special open/sopen/fopen with backup semantics
                      //Added by Bill and Robert K. on 12/26/96
#endif

#ifndef T_VXD

 #ifndef S_H
   #undef malloc
   #undef free
 #else
   #undef malloc
   #define malloc(x) (((x)>262144)?(NULL):mymalloc(x))
 #endif
#endif

#ifdef MCV
int iDebugLevel;
#endif

// debug variables

#ifdef DEBUG_CODE
 DWORD dwReadSize;
 DWORD dwFileOffset;
 DWORD dwCurrentFilePosition;
 DWORD dwBufferStartPosition;
 DWORD dwBufferEndPosition;
 DWORD dwBufferSize;
 DWORD dwBuffertoFlush;
 DWORD dwNumBuffers;
 DWORD dwFileSize;
 DWORD dwBytesReallyAllocated;
 DWORD dwpFileData;
#endif

 DWORD testcodeenabled = 0;

// end debug variables

// windows 95 VxD
#ifdef WIN40

// external assembler functions (in VXDTONLM.ASM) for NIOS.
;
extern int NIOSDOSSeek( int file, long fpos, int len );
;
extern int NIOSDOSRead( int file, long fpos, void *buf, int len );

#endif
;

//----------------------------------------------------------------------------
//
// Module Name: ou_fgetfilesize
//
// Description:
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
long ou_fgetfilesize( unsigned long filehandle )
{

 // *****************************
 // Local Data
 // *****************************

    long iFileSize;
    int iFileHandle;

    WORD wError ;

 //********************
 // Start of the Code
 //********************

// OS SPECIFIC OPERATIONS
#ifdef T_VXD

  // windows 3.1x VxD
  #ifdef WIN31

      iFileHandle = ((pwdmac_handles)filehandle)->handle;

      // determine file size
      iFileSize   = R0_GetFileSize( (HANDLE)iFileHandle, &wError );

  #endif

  // windows 95 VxD
  #ifdef WIN40

        pFileIOTable pFIOT;

        pFIOT = ((pwdmac_handles)filehandle)->pFIOT;

        // get the file size
        iFileSize = FSD_lseek( pFIOT, 0, FILE_END );

        // set back to the beginning
        FSD_lseek( pFIOT, 0, FILE_BEGIN );

  #endif

#else

      iFileHandle = ((pwdmac_handles)filehandle)->handle;

    // no-vxd calls
    #if defined(T_DOS) || defined(T_WIN3X) || defined(T_OS2) || defined(T_WINNT) || defined(WATC) || defined(T_WIN95)
      iFileSize  = filelength( iFileHandle );
    #else
      iFileSize  = filelength(fileno(iFileHandle));
    #endif

#endif

    return iFileSize;

}

//----------------------------------------------------------------------------
//
// Module Name: ou_MemAlloc
//
// Description:
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
void *ou_MemAlloc(size_t size)
{

 // *****************************
 // Local Data
 // *****************************

  void *p;

 //********************
 // Start of the Code
 //********************

#ifdef RANDOM_MEM_FAIL
  if (rand() % RANDOM_MEM_FAIL == 3)
  {
    return NULL;
  }
  else
  {
#endif
    p = malloc(size);
    if (p == NULL)
    {
      df_errno = ELL_MALLOC_FAILED;
      return NULL;
    }
    else
    {
      return p;
    } /* endif */

#ifdef RANDOM_MEM_FAIL
  } /* endif */
#endif

//    return malloc(size);

}

//----------------------------------------------------------------------------
//
// Module Name: ou_SafeFree
//
// Description: performs a free only if the object was really allocated
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
int ou_SafeFree(void *p)
{

 // *****************************
 // Local Data
 // *****************************


 //********************
 // Start of the Code
 //********************

    assert(p != NULL); /* for debugging, never let this happen */

    if (p != NULL)
    {
        free( p );
        return TRUE;
    }

    return FALSE;
}

//----------------------------------------------------------------------------
//
// Module Name: ou_fopen
//
// Description:
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
int ou_fopen(char *filename)
{

 // *****************************
 // Local Data
 // *****************************

  int fhandle = -1;

  PFILEDATA pFileData ;

 //********************
 // Start of the Code
 //********************

#ifndef T_VXD

  // Non VxDs

  assert( filename != NULL );

  /* try opening for read/write */
  fhandle = sopen(filename, O_RDWR | O_BINARY, SH_DENYNO);

  /* if opening for read/write  failed */
  if ( fhandle == -1 )
  { //  open for readonly

    /* otherwise resort to read only */
    fhandle = sopen(filename, O_RDONLY | O_BINARY, SH_DENYNO);

  }  // end opened for readonly

  return fhandle;

#else

  return 0;

#endif

}

//----------------------------------------------------------------------------
//
// Module Name: ou_fBufferOpen
//
// Description:
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
PFILEDATA ou_fBufferOpen(unsigned long filehandle )
{ // ou_fBufferOpen

 // *****************************
 // Local Data
 // *****************************

    // Buffer management variables
    int iNumBuffers        = 0;
    int iNumResidualBuffer = 0;
    int iBufferCounter;
    int iFreeBufferCounter;
    int bytesRead;
    int iFileHandle;
    long iFileSize;
    int iBufferSize;

    PFILEDATA pFileData = NULL;

    unsigned char *pFileBuffer = NULL;

    // end Buffer management variables

#ifdef T_VXD

  // windows 95
  #ifdef WIN40

        pFileIOTable pFIOT;

  #endif

#endif

 //********************
 // Start of the Code
 //********************

    // get file read info structure
    pFileData = (FILEDATA *)malloc( sizeof(FILEDATA) ) ;

   #ifdef DEBUG_CODE
     dwpFileData     = (DWORD)pFileData;
     dwBuffertoFlush = pFileData->buffer_to_flush;
     dwNumBuffers    = pFileData->NumBuffers ;
   #endif

    // Check for valid pointer
    if ( pFileData == NULL )
      return 0;
    else
    { // got the buffer, fill in the info.

     // Initialize the File Info. data structure
     memset( pFileData , 0, sizeof(FILEDATA) );

     iFileSize = ou_fgetfilesize( filehandle );

     // OS SPECIFIC OPERATIONS
     #ifdef T_VXD

       // For VxDs

       // windows 3.1x
       #ifdef WIN31

        // get file handle
        iFileHandle = ((pwdmac_handles)filehandle)->handle;

       #endif

       // windows 95
       #ifdef WIN40

        pFIOT = (pFileIOTable)((pwdmac_handles)filehandle)->pFIOT;

        // get file handle
        iFileHandle = pFIOT->IOReq.ir_fh;

       #endif
     #else

      // For Non-VxDs

      // get file handle
      iFileHandle = ((pwdmac_handles)filehandle)->handle;

     #endif

      // save info for file reads
      pFileData->file_handle = (HANDLE)iFileHandle ;
      pFileData->file_size   = iFileSize;

      // clear file written to indicator
      pFileData->file_written_to = 0;


      // See if file size is less than or equal to buffer size
      if ( iFileSize <= FILEBUFFER_BUFFER )
      { // file size less than or equal buffer

         // set the buffer size to file size and number buffers to 1
         iBufferSize = (int) iFileSize;
         iNumBuffers = 1;

      }  // end file size less than or equal buffer
      else
      { // file size is larger than one buffer

         // calculate the number of buffers needed
         iNumBuffers        = (int)(iFileSize / FILEBUFFER_BUFFER);
         iBufferSize        = FILEBUFFER_BUFFER;
         iNumResidualBuffer = (int)(iFileSize % FILEBUFFER_BUFFER);

         // check for residual buffer
         if ( iNumResidualBuffer != 0 )
           iNumBuffers ++;

         // check for number of buffers vs. limit
         if ( iNumBuffers > MAX_FILE_BUFFERS )
           iNumBuffers = MAX_FILE_BUFFERS;

      } // end file size is larger than one buffer

      // store away the number of buffers
      pFileData->NumBuffers = iNumBuffers;

      // set buffer 1 to be first buffer to flush, i.e. reread into
       if (pFileData->NumBuffers > 1 )
         pFileData->buffer_to_flush = 1;
       else
         pFileData->buffer_to_flush = 0;

      // -------------------------
      // loop to set up buffers
      // -------------------------
      for ( iBufferCounter = 0; iBufferCounter < iNumBuffers; iBufferCounter++ )
      { // loop to set up buffers

          // get file buffer
           pFileBuffer = malloc( iBufferSize );

          // check for malloc fail
           if ( pFileBuffer == NULL )
           {// error, bail out with NULL
             goto ErrorFreeBuffer;
           }

          // init. buffer information
           pFileData->FileBufferArray[iBufferCounter].buffer_size         = iBufferSize;
           pFileData->FileBufferArray[iBufferCounter].pfilebuffer         = pFileBuffer ;
           pFileData->FileBufferArray[iBufferCounter].bufferstartposition = (iBufferCounter * iBufferSize) ;
           pFileData->FileBufferArray[iBufferCounter].bufferendposition   = (iBufferCounter * iBufferSize) + iBufferSize - 1;

           bytesRead = os_ReadFile( filehandle,                                                      // handle to file
                                    pFileData->FileBufferArray[iBufferCounter].bufferstartposition,  // file offset to start reading from
                                    pFileBuffer,                                                     // buffer to read into
                                    iBufferSize );                                                   // number bytes to read

      } // end loop to set up buffers

      // return pointer to file structure
      return pFileData ;

    } // end, got the buffer, fill in the info.

ErrorFreeBuffer:
      // Clean up the buffers
      for ( iFreeBufferCounter = 0; iFreeBufferCounter < iBufferCounter; iFreeBufferCounter++ )
      {
           if ( pFileData->FileBufferArray[iFreeBufferCounter].pfilebuffer != NULL )
             free( pFileData->FileBufferArray[iFreeBufferCounter].pfilebuffer );

      }

      if ( pFileData != NULL)
       free( pFileData );

      return NULL;

} // end ou_fBufferOpen

//----------------------------------------------------------------------------
//
// Module Name: ou_fclose
//
// Description:
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
void ou_fclose(int hnd)
{

 // *****************************
 // Local Data
 // *****************************

 //********************
 // Start of the Code
 //********************

#ifndef T_VXD

  _close( hnd );

#else

    return;

#endif

}

//----------------------------------------------------------------------------
//
// Module Name: ou_fBufferClose
//
// Description:
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
void ou_fBufferClose( unsigned long filehandle, PFILEDATA pFileData )
{

 // *****************************
 // Local Data
 // *****************************
    int iBufferCounter;


 //********************
 // Start of the Code
 //********************

    // -------------------------
    // loop to free buffers
    // -------------------------
    for ( iBufferCounter = 0; iBufferCounter < pFileData->NumBuffers; iBufferCounter++ )
    { // loop to set up buffers

         // free all the file buffers
          free( pFileData->FileBufferArray[iBufferCounter].pfilebuffer);

    } // end loop to free buffers

    // free the file information buffer
    free( pFileData );

    return;

}

//----------------------------------------------------------------------------
//
// Module Name: ou_ReadFile
//
// Description: simplified interface to fread and fseek
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
int ou_ReadFile( unsigned long file, long FileOffset, void *buffer, int  Count)
{

   // ------------
   // LOCAL DATA
   // ------------

   int   BytesRead;
   DWORD Actual_R0_BytesRead = 0;

   WORD error;

  // Buffer management variables
   int iNumBuffers;
   int iBufferCounter;
   int iBuffertoFlush;
   int bufferhit;

   PFILEDATA pFileData;

   // -----------------
   // START OF THE CODE
   // -----------------

     assert(buffer != NULL);
     assert(FileOffset >= 0);
     assert(Count >= 0);

   #ifdef S_H
   /*
   For NetWare scanner code, simulate the process yield by calling
   ThreadSwitch. This is done, because of the buffered IO code may not
   get an oppurtunity to do any blocking file IO.
   */
   ThreadSwitch ();
   #endif

   #ifdef T_VXD

     #ifdef WIN40

     // check if doing a NIOS read
     if ((int)((pwdmac_handles)file)->pFIOT == 0x534F494E )
     { // NIOS Stuff

         // Call the NIOS DOSSeek and DOSRead assembly functions
          if ( NIOSDOSSeek(((pwdmac_handles)file)->handle,  FileOffset, Count ) != -1)
             return ( NIOSDOSRead(((pwdmac_handles)file)->handle,  FileOffset, buffer, Count ) == Count );
          else
             return FALSE;

     } // end NIOS stuff

     #endif

   #endif

   // get pointer to file data buffer
     pFileData = (PFILEDATA)((pwdmac_handles)file)->pFileData;

   // set debug variables to use as watch points for soft/ice
   #ifdef DEBUG_CODE
     dwReadSize      = Count;
     dwFileOffset    = FileOffset;
     dwpFileData     = (DWORD)pFileData;
     dwBuffertoFlush = pFileData->buffer_to_flush;
     dwNumBuffers    = pFileData->NumBuffers ;
   #endif

   // get buffer to be first buffer to flush, i.e. reread into
     iBuffertoFlush = pFileData->buffer_to_flush;
     iNumBuffers    = pFileData->NumBuffers;

   // init buffer counter and buffer hit boolean
     iBufferCounter = 0;
     bufferhit      = FALSE;

   #ifdef DEBUG_CODE
   //***********************
   // test code
   //***********************
   if ( testcodeenabled == 1 )
   {

     ptempbuffer = tempbuffer;

    // clear buffer before read
     memset ( tempbuffer, 0, FILEBUFFER_BUFFER );

    // read file into current buffer
     tempBytesRead = os_ReadFile( file,                                                      // handle to file
                                  FileOffset,                                               // file offset to start reading from
                                  tempbuffer,      // buffer to read into
                                  Count );         // number bytes to read

   }
   //***********************
   // test code
   //***********************
   #endif

   // test to see if the file has been written to
    if ( pFileData->file_written_to == 1 )
    { // read from the file since it has changed

      // read file into current buffer
      BytesRead = os_ReadFile( file,        // handle to file
                               FileOffset,  // file offset to start reading from
                               buffer,      // buffer to read into
                               Count );     // number bytes to read
      BytesRead = Count;

      return (BytesRead == Count );

    } // end read from the file since it has changed


   // -------------------------
   // loop to read buffer info.
   // -------------------------
     while ( (iBufferCounter < iNumBuffers) && (bufferhit == FALSE) )
     { // loop to read buffer info.

       // Check if current file offset is in range of current buffer
       if ( ( (DWORD)FileOffset >= (DWORD)pFileData->FileBufferArray[iBufferCounter].bufferstartposition) &&
              (DWORD)(FileOffset <= (DWORD)pFileData->FileBufferArray[iBufferCounter].bufferendposition) )
       { // within current buffer

         // buffer hit variable
          bufferhit = TRUE;

         // get out of loop
          break;

       } // end within current buffer
       else
         // debug variable
          bufferhit = FALSE;

       // increment buffer counter
       iBufferCounter ++;

     } // end loop to read buffer info.

     // check if we got a buffer hit
     if ( bufferhit == TRUE )
     { // the file offset was within a current buffer

       // check if full read is within the current buffer
       if ( (FileOffset + Count - 1) <= pFileData->FileBufferArray[iBufferCounter].bufferendposition )
       { // yes, get the buffer

          // Check new buffer end position vs. file offset
           if ( (FileOffset > pFileData->file_size) ||
                ( FileOffset > pFileData->FileBufferArray[iBufferCounter].bufferendposition))
           {

              #ifdef DEBUG_CODE
                  dwReadSize             = Count;
                  dwFileOffset           = FileOffset ;
                  dwBufferStartPosition  = pFileData->FileBufferArray[iBufferCounter].bufferstartposition;
                  dwBufferEndPosition    = pFileData->FileBufferArray[iBufferCounter].bufferendposition;
                  dwBufferSize           = pFileData->FileBufferArray[iBufferCounter].buffer_size;
                  dwBuffertoFlush        = iBuffertoFlush;
                  dwNumBuffers           = pFileData->NumBuffers;
                  dwFileSize             = pFileData->file_size;
                #endif

              return ( FALSE );
           }


           // Copy buffer contents into user supplied buffer
           memcpy ( (void *)buffer,                                                                           // destination buffer
                    (void *)(pFileData->FileBufferArray[iBufferCounter].pfilebuffer +                         // Source buffer
                            (FileOffset - pFileData->FileBufferArray[iBufferCounter].bufferstartposition) ),  // Source buffer
                            Count );                                                                          // Number to copy


           // set return values
           BytesRead = Count;
           error     = 0;

         #ifdef DEBUG_CODE
           if ( testcodeenabled == 1 )
           {

             ptempbuffer1 = tempbuffer1;

             // clear buffer before read
             memset ( tempbuffer1, 0, FILEBUFFER_BUFFER );

             // Copy buffer contents into user supplied buffer
             memcpy ( (void *)tempbuffer1,                                                                           // destination buffer
                      (void *)(pFileData->FileBufferArray[iBufferCounter].pfilebuffer +                         // Source buffer
                              (FileOffset - pFileData->FileBufferArray[iBufferCounter].bufferstartposition) ),  // Source buffer
                              Count );                                                                          // Number to copy


             buffcompare = memcmp( tempbuffer1, tempbuffer, Count );

             if ( buffcompare != 0)
             {

               tempdiffcounter = 0;

               buffcompare = -1;

               for ( tempbytecounter = 0; tempbytecounter < Count;  tempbytecounter++ )
               {

                 if ( tempbuffer1[tempbytecounter] != tempbuffer[tempbytecounter] )
                   tempdiffcounter ++;

               }
             }

           }
         #endif

       } // end yes, get the buffer
       else
       { // get a new buffer

          // set debug variables to use as watch points for soft/ice
        #ifdef DEBUG_CODE
           dwpFileData           = (DWORD)pFileData;
           dwBuffertoFlush       = pFileData->buffer_to_flush;
           dwNumBuffers          = pFileData->NumBuffers ;
           dwBufferStartPosition = pFileData->FileBufferArray[iBuffertoFlush].bufferstartposition;
           dwBufferEndPosition   = pFileData->FileBufferArray[iBuffertoFlush].bufferendposition;
           dwBufferSize          = pFileData->FileBufferArray[iBuffertoFlush].buffer_size;
        #endif
          // end set debug variables

          // Check new buffer end position vs. file offset
           if ( FileOffset > pFileData->file_size )
           {

              #ifdef DEBUG_CODE
                  dwReadSize             = Count;
                  dwFileOffset           = FileOffset ;
                  dwBufferStartPosition  = pFileData->FileBufferArray[iBuffertoFlush].bufferstartposition;
                  dwBufferEndPosition    = pFileData->FileBufferArray[iBuffertoFlush].bufferendposition;
                  dwBufferSize           = pFileData->FileBufferArray[iBuffertoFlush].buffer_size;
                  dwBuffertoFlush        = iBuffertoFlush;
                  dwNumBuffers           = pFileData->NumBuffers;
                  dwFileSize             = pFileData->file_size;
                #endif

              return ( FALSE );
           }

          // reset start positions
           pFileData->FileBufferArray[iBuffertoFlush].bufferstartposition = FileOffset;

          // check for end of file
           if ( (FileOffset + pFileData->FileBufferArray[iBuffertoFlush].buffer_size) > (DWORD)pFileData->file_size)
             pFileData->FileBufferArray[iBuffertoFlush].buffer_size = pFileData->file_size - FileOffset;

          // reset end positions
           pFileData->FileBufferArray[iBuffertoFlush].bufferendposition = FileOffset + pFileData->FileBufferArray[iBuffertoFlush].buffer_size;

          // Check new buffer end position
           if ( FileOffset > pFileData->FileBufferArray[iBuffertoFlush].bufferendposition)
              return ( FALSE );

          // clear buffer before read
           memset ( pFileData->FileBufferArray[iBuffertoFlush].pfilebuffer,
                    0,
                    pFileData->FileBufferArray[iBuffertoFlush].buffer_size );

          // read file into current buffer
           BytesRead = os_ReadFile( file,                                                      // handle to file
                                    FileOffset,                                                // file offset to start reading from
                                    pFileData->FileBufferArray[iBuffertoFlush].pfilebuffer,    // buffer to read into
                                    pFileData->FileBufferArray[iBuffertoFlush].buffer_size );  // number bytes to read

           // Copy buffer contents into user supplied buffer
           memcpy ( (void *)buffer,                                                                           // destination buffer
                    (void *)pFileData->FileBufferArray[iBuffertoFlush].pfilebuffer,                          // Source buffer
                            Count );                                                                          // Number to copy

           // set return values
           BytesRead = Count;

           // reset buffer to be first buffer to flush, i.e. reread into
           iBuffertoFlush++;

           // check for wrap
           if ( iBuffertoFlush >= iNumBuffers)
              iBuffertoFlush = 0;

           //store it away
           pFileData->buffer_to_flush = iBuffertoFlush ;

       } // end get a new buffer

     } // end the file offset was within a current buffer
     else
     { // no bufferhit, get a new buffer

       // set debug variables to use as watch points for soft/ice
      #ifdef DEBUG_CODE
        dwpFileData           = (DWORD)pFileData;
        dwBuffertoFlush       = pFileData->buffer_to_flush;
        dwNumBuffers          = pFileData->NumBuffers ;
        dwBufferStartPosition = pFileData->FileBufferArray[iBuffertoFlush].bufferstartposition;
        dwBufferEndPosition   = pFileData->FileBufferArray[iBuffertoFlush].bufferendposition;
        dwBufferSize          = pFileData->FileBufferArray[iBuffertoFlush].buffer_size;
      #endif
       // end set debug variables

       // Check new buffer end position vs. file offset
       if ( FileOffset > pFileData->file_size )
       {

          #ifdef DEBUG_CODE
              dwReadSize             = Count;
              dwFileOffset           = FileOffset ;
              dwBufferStartPosition  = pFileData->FileBufferArray[iBuffertoFlush].bufferstartposition;
              dwBufferEndPosition    = pFileData->FileBufferArray[iBuffertoFlush].bufferendposition;
              dwBufferSize           = pFileData->FileBufferArray[iBuffertoFlush].buffer_size;
              dwBuffertoFlush        = iBuffertoFlush;
              dwNumBuffers           = pFileData->NumBuffers;
              dwFileSize             = pFileData->file_size;
         #endif

         return ( FALSE );
       }

       // reset start positions
       pFileData->FileBufferArray[iBuffertoFlush].bufferstartposition = FileOffset;

       // check for end of file
       if ( (FileOffset + pFileData->FileBufferArray[iBuffertoFlush].buffer_size) > (DWORD)pFileData->file_size)
            pFileData->FileBufferArray[iBuffertoFlush].buffer_size = pFileData->file_size - FileOffset;
       else
        if ( pFileData->FileBufferArray[iBuffertoFlush].buffer_size < Count)
              pFileData->FileBufferArray[iBuffertoFlush].buffer_size = Count;

       // reset end positions
       pFileData->FileBufferArray[iBuffertoFlush].bufferendposition = FileOffset + pFileData->FileBufferArray[iBuffertoFlush].buffer_size;

       // Check new buffer end position
       if ( FileOffset > pFileData->FileBufferArray[iBuffertoFlush].bufferendposition)
          return ( FALSE );

       // clear buffer before read
       memset ( pFileData->FileBufferArray[iBuffertoFlush].pfilebuffer,
                0,
                pFileData->FileBufferArray[iBuffertoFlush].buffer_size );

       // read file into current buffer
       BytesRead = os_ReadFile( file,                                                      // handle to file
                                FileOffset,                                               // file offset to start reading from
                                pFileData->FileBufferArray[iBuffertoFlush].pfilebuffer,   // buffer to read into
                                pFileData->FileBufferArray[iBuffertoFlush].buffer_size ); // number bytes to read

       // Copy buffer contents into user supplied buffer
       memcpy ( (void *)buffer,                                                                           // destination buffer
                (void *)pFileData->FileBufferArray[iBuffertoFlush].pfilebuffer,                          // Source buffer
                        Count );                                                                          // Number to copy

       // set return value
       BytesRead = Count;

       // reset buffer to be first buffer to flush, i.e. reread into
       iBuffertoFlush++;

       // check for wrap
       if ( iBuffertoFlush >= iNumBuffers)
          iBuffertoFlush = 0;

       //store it away
       pFileData->buffer_to_flush = iBuffertoFlush ;

     } // end no bufferhit, get a new buffer

     return( BytesRead == Count );

} // end ou_ReadFile

//----------------------------------------------------------------------------
//
// Module Name: os_ReadFile
//
// Description: simplified interface to fread and fseek
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
int os_ReadFile( unsigned long filehandle, long FileOffset, void *buffer, int  Count )
{

   // ------------
   // LOCAL DATA
   // ------------

   int BytesRead = 0;
   int iFileHandle;
   int file;

   DWORD Actual_R0_BytesRead = 0;

   WORD error;

   // -----------------
   // START OF THE CODE
   // -----------------


#ifndef T_VXD

  // get the file handle
   file = ((pwdmac_handles)filehandle)->handle;

  #ifdef LL_DEBUG
   printf("fread %i bytes at %li\n", Count, FileOffset);
  #endif

  if (_lseek(file, FileOffset, SEEK_SET) == FileOffset)
  {

    BytesRead = _read(file, buffer, Count);

    return( Count );

  }
  else
  {
    return FALSE;
  }

#else

  // windows 3.1x VxD
  #ifdef WIN31

      iFileHandle = ((pwdmac_handles)filehandle)->handle;

  #endif

  // windows 95
  #ifdef WIN40

        pFileIOTable pFIOT;

        pFIOT = (pFileIOTable)((pwdmac_handles)filehandle)->pFIOT;


  #endif


   // set debug variables to use as watch points for soft/ice
   #ifdef DEBUG_CODE
     dwReadSize   = Count;
     dwFileOffset = FileOffset;
   #endif
   // end set debug variables

// OS SPECIFIC OPERATIONS
  #ifdef WIN31

     BytesRead = R0_ReadFile( TRUE,
                              (HANDLE)iFileHandle ,
                              buffer,
                              (DWORD) Count,
                              (DWORD) FileOffset,
                              &error);

     if (error)
       return 0;
     else
       return BytesRead;

  #endif

  #ifdef WIN40


    // seek to position in the file
    if( FSD_lseek( pFIOT, FileOffset, FILE_BEGIN ) != -1 )
    { // if

      // Use FSD to do the read
      BytesRead = FSD_read( pFIOT, buffer, Count );

      // check for error
      if ( BytesRead < 0 )
        return 0;
      else
        return ( BytesRead );

    } // end if
    else
      return 0;

  #endif

#endif

}

//----------------------------------------------------------------------------
//
// Module Name: ou_WriteFile
//
// Description: simplified interface to fwrite and fseek
//
// Input Parameters:
//
//
// Returns:
//
//
//----------------------------------------------------------------------------
int ou_WriteFile(unsigned long file, long fpos, void *buf, int len)
{

   // ------------
   // LOCAL DATA
   // ------------

   PFILEDATA pFileData;

   int iFileHandle;

   // -----------------
   // START OF THE CODE
   // -----------------

    assert(buf != NULL);
    assert(fpos >= 0);
    assert(len >= 0);

  // get pointer to file data buffer
  pFileData = (PFILEDATA)((pwdmac_handles)file)->pFileData;

  // get the file handle
  iFileHandle = ((pwdmac_handles)file)->handle;

  // indicate file has been written to
  pFileData->file_written_to = 1;

#ifndef T_VXD

  if ( _lseek( iFileHandle, fpos, SEEK_SET ) != fpos )
  {

 #ifdef LL_DEBUG
    printf("!lseek(%i, %li)\n", iFileHandle, fpos);
    perror("ou_WriteFile failed");
 #endif

    return FALSE;
  }

 #ifdef LL_DEBUG
  if ( _write( iFileHandle, buf, len ) != len )
  {
    printf("!write(%i, %li, %i)\n", iFileHandle, fpos, len);
    perror("ou_WriteFile failed");
    return FALSE;
  }
  else
  {
    printf("write(%i, %li, %i)\n", iFileHandle, fpos, len);
    return TRUE;
  }
  #else
  return ( _write(iFileHandle, buf, len) == len );
  #endif

#else

// OS SPECIFIC OPERATIONS

 #ifdef ENABLE_VXD_WRITES

   // windows 95 FSD stuff
   #ifdef WIN40
     if (((pwdmac_handles)file)->pFIOT ==NULL)
      return FALSE;

     if( FSD_lseek( ((pwdmac_handles)file)->pFIOT, fpos, FILE_BEGIN ) != -1  )
         return ( FSD_write( ((pwdmac_handles)file)->pFIOT, buf, len ) == len );
     else
         return FALSE;
   #endif

   // windows 3.1xx stuff
   #ifdef WIN31
        return TRUE;
   #endif

 #else
  return FALSE;
 #endif


#endif

}

#ifdef MCV
/* from printlin.c */

void printline(dword label, byte *line, int length)
{
   int i, j, len;
   char oline[80];
   char str[33];

   for (j=0; j < length; j+=16) {
      sprintf(str, "%08X: ", label+j);
      strcpy(oline, str);

      for (i=0; i< min(length-j, 16); i++)
      {
         sprintf(str, "%02X", (int)line[i+j]);
         strcat(oline, str);
         if ((i+1)%2 == 0) strcat(oline, " ");
         if ((i+1)%4 == 0) strcat(oline, " ");
      }
      for (; i<16; i++)
      {
         strcat(oline, "  ");
         if ((i+1)%2 == 0) strcat(oline, " ");
         if ((i+1)%4 == 0) strcat(oline, " ");
      }

      strcat(oline, ": ");
      len = strlen(oline);
      for (i=0; i<min(length-j, 16); i++) {
         oline[len+i] = (line[i+j]>=32)?((line[i+j]<=126)?line[i+j]:'.'):'.';
      }
      oline[len+i] = 0;

      printf("%s\n", oline);
   }
}
#endif

// END FILE: \DOCFILE\LL.C
