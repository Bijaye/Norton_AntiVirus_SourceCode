// DecIO.h : Decomposer I/O Interface
// Copyright 1999 by Symantec Corporation.  All rights reserved.


#if !defined(DECIO_H)
#define DECIO_H

#include <tchar.h>
#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

// Generic 32-bit handle (or 32-bit pointer) used for
// accessing Decomposer data streams (or files).
// It is the responsibility of the Decomposer client to manage these
// "handles".  Internally, the Decomposer has no knowledge of what these
// "handles" actually are.
typedef unsigned long HDECDATA;


/////////////////////////////////////////////////////////////////////////////
// Interface IDecIO - interface definition for Decomposer file I/O callbacks
//

class IDecIO
{
public:
    // IDecIO methods:
    // Note that the client can be in complete control of what the
    // pszDataSourceName string below is simply by implementing DecOpen
    // and providing the necessary translation.  Also note that the
    // client will thus be able to pass any arbitrary name to the Decomposer.
    // The caveat is that because some of the Decomposer engines sometimes
    // need to examine the extension of a file, it would be advisable
    // (but not required) for the client to append a meaningful extension
    // to the name.
    // The default implementation assumes pszDataSourceName to be a filename
    // in the currently operating file system.

    // DecAccess returns true if the file has the given mode, false otherwise.
    virtual bool DecAccess (
                LPCTSTR path,
                int mode) = 0;

    // DecChangeMode returns true if the file's mode is successfully changed,
    // false otherwise.
    virtual bool DecChangeMode (
                LPCTSTR filename,
                int pmode) = 0;

    virtual void DecClearError (
                HDECDATA hDecData) = 0;

    // DecClose returns true if handle is closed successfully, false otherwise.
    virtual bool DecClose (
                HDECDATA hDecData) = 0;

    // DecEOF returns true if at end of stream, false otherwise.
    virtual bool DecEOF (
                HDECDATA hDecData) = 0;

    // DecError returns a standard C library IO error code (from errno.h).
    virtual int DecError (
                HDECDATA hDecData) = 0;

    // DecFlush returns true if stream buffer was successfully flushed, false otherwise.
    virtual bool DecFlush (
                HDECDATA hDecData) = 0;

    // DecGetChar returns true if character was successfully read, false otherwise.
    // Use DecEOF to differentiate between a read error and an EOF condition.
    virtual bool DecGetChar (
                HDECDATA hDecData,
                int *pChar) = 0;

    // DecGetPos returns true if position information is successfully retrieved,
    // false otherwise.
    virtual bool DecGetPos (
                HDECDATA hDecData,
                fpos_t *pos) = 0;

    // DecOpen returns 0 to indicate an error and a non-zero handle otherwise.
    virtual HDECDATA DecOpen (
                LPCTSTR pszDataSourceName,
                LPCTSTR mode) = 0;

    // DecPutChar returns true if character was successfully written, false otherwise.
    virtual bool DecPutChar (
                HDECDATA hDecData,
                int iChar) = 0;

    virtual unsigned int DecRead (
                HDECDATA hDecData,
                void *pBuffer,
                unsigned int uSize) = 0;

    // DecRemove returns true if file is deleted, false otherwise.
    virtual bool DecRemove (
                LPCTSTR path) = 0;

    // DecRename returns true if file is renamed, false otherwise.
    virtual bool DecRename (
                LPCTSTR oldname,
                LPCTSTR newname) = 0;

    virtual void DecRewind (
                HDECDATA hDecData) = 0;

    // DecSeek returns true if seek operation was successful, false otherwise.
    virtual bool DecSeek (
                HDECDATA hDecData,
                long lOffset,
                int nOrigin) = 0;

    // DecSetPos returns true if position information is successfully set,
    // false otherwise.
    virtual bool DecSetPos (
                HDECDATA hDecData,
                const fpos_t *pos) = 0;

    // DecSetVBuf returns true if buffer is set successfully, false otherwise.
    virtual bool DecSetVBuf (
                HDECDATA hDecData,
                char *buffer,
                int mode,
                unsigned int size) = 0;

    // DecStat returns true if status information is retrieved, false otherwise.
    virtual bool DecStat (
                LPCTSTR path,
                struct _stat *buffer) = 0;

    virtual long DecTell (
                HDECDATA hDecData) = 0;

    virtual unsigned int DecWrite (
                HDECDATA hDecData,
                void *pBuffer,
                unsigned int uSize) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Class CDecIO - Standard/default implementation of Decomposer file I/O
//                callbacks.

class CDecIO : public IDecIO
{
public:
    // DecAccess returns true if the file has the given mode, false otherwise.
    virtual bool DecAccess (
                LPCTSTR path,
                int mode)
    {
        if (!_access (path, mode))
            return (true);
        return (false);
    }

    // DecChangeMode returns true if the file's mode is successfully changed,
    // false otherwise.
    virtual bool DecChangeMode (
                LPCTSTR filename,
                int pmode)
    {
        if (!_chmod (filename, pmode))
            return (true);
        return (false);
    }

    virtual void DecClearError (
                HDECDATA hDecData)
    {
        clearerr ((FILE *)hDecData);
    }

    // DecClose returns true if handle is closed successfully, false otherwise.
    virtual bool DecClose (
                HDECDATA hDecData)
    {
        if (!fclose ((FILE *)hDecData))
            return (true);
        return (false);
    }

    // DecEOF returns true if at end of stream, false otherwise.
    virtual bool DecEOF (
                HDECDATA hDecData)
    {
        if (feof ((FILE *)hDecData))
            return (true);
        return (false);
    }

    // DecError returns a standard C library IO error code (from errno.h).
    virtual int DecError (
                HDECDATA hDecData)
    {
        return (ferror ((FILE *)hDecData));
    }

    // DecFlush returns true if stream buffer was successfully flushed, false otherwise.
    virtual bool DecFlush (
                HDECDATA hDecData)
    {
        if (!fflush ((FILE *)hDecData))
            return (true);
        return (false);
    }

    // DecGetChar returns true if character was successfully read, false otherwise.
    // Use DecEOF to differentiate between a read error and an EOF condition.
    virtual bool DecGetChar (
                HDECDATA hDecData,
                int *pChar)
    {
        int c;

        c = fgetc ((FILE *)hDecData);
        if (c == EOF)
            return (false);
        *pChar = c;
        return (true);
    }

    // DecGetPos returns true if position information is successfully retrieved,
    // false otherwise.
    virtual bool DecGetPos (
                HDECDATA hDecData,
                fpos_t *pos)
    {
        if (!fgetpos ((FILE *)hDecData, pos))
            return (true);
        return (false);
    }

    // DecOpen returns 0 to indicate an error and a non-zero handle otherwise.
    virtual HDECDATA DecOpen (
                LPCTSTR pszDataSourceName,
                LPCTSTR mode)
    {
        FILE *fp;

        fp = fopen (pszDataSourceName, mode);
        return ((HDECDATA)fp);
    }

    // DecPutChar returns true if character was successfully written, false otherwise.
    virtual bool DecPutChar (
                HDECDATA hDecData,
                int iChar)
    {
        if (fputc (iChar, (FILE *)hDecData) == EOF)
            return (false);
        return (true);
    }

    virtual unsigned int DecRead (
                HDECDATA hDecData,
                void *pBuffer,
                unsigned int uSize)
    {
        unsigned int uiBytesRead;

        uiBytesRead = fread (pBuffer, 1, uSize, (FILE *)hDecData);
        return (uiBytesRead);
    }

    // DecRemove returns true if file is deleted, false otherwise.
    virtual bool DecRemove (
                LPCTSTR path)
    {
        if (!remove (path))
            return (true);
        return (false);
    }

    // DecRename returns true if file is renamed, false otherwise.
    virtual bool DecRename (
                LPCTSTR oldname,
                LPCTSTR newname)
    {
        if (!rename (oldname, newname))
            return (true);
        return (false);
    }

    virtual void DecRewind (
                HDECDATA hDecData)
    {
        rewind ((FILE *)hDecData);
    }

    // DecSeek returns true if seek operation was successful, false otherwise.
    virtual bool DecSeek (
                HDECDATA hDecData,
                long lOffset,
                int nOrigin)
    {
        if (!fseek ((FILE *)hDecData, lOffset, nOrigin))
            return (true);
        return (false);
    }

    // DecSetPos returns true if position information is successfully set,
    // false otherwise.
    virtual bool DecSetPos (
                HDECDATA hDecData,
                const fpos_t *pos)
    {
        if (!fsetpos ((FILE *)hDecData, pos))
            return (true);
        return (false);
    }

    // DecSetVBuf returns true if buffer is set successfully, false otherwise.
    virtual bool DecSetVBuf (
                HDECDATA hDecData,
                char *buffer,
                int mode,
                unsigned int size)
    {
        if (!setvbuf ((FILE *)hDecData, buffer, mode, size))
            return (true);
        return (false);
    }

    // DecStat returns true if status information is retrieved, false otherwise.
    virtual bool DecStat (
                LPCTSTR path,
                struct _stat *buffer)
    {
        if (!_stat (path, buffer))
            return (true);
        return (false);
    }

    virtual long DecTell (
                HDECDATA hDecData)
    {
        return (ftell ((FILE *)hDecData));
    }

    virtual unsigned int DecWrite (
                HDECDATA hDecData,
                void *pBuffer,
                unsigned int uSize)
    {
        unsigned int uiBytesWritten;

        uiBytesWritten = fwrite (pBuffer, 1, uSize, (FILE *)hDecData);
        return (uiBytesWritten);
    }
};


#endif  // DECIO_H

