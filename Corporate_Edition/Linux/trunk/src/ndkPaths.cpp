// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK File Path Handling functions

// ================== ndkPaths ==========================================================

#include <system/dirent.h>
#include "symDirent.h"

// Need to add the extra padding for buggy implementations of readdir_r
// One would guess to subtract sizeof(dirent.d_name) from this, but then you could
// trip over bugs in Linux implementations
#define DIRENT_TOTAL ((int)(sizeof(dirent) + NAME_MAX + 1))

#include <system/sys/stat.h>
#ifdef st_atime                              // stat.h version >= glibc-2.3.3
    #define stAtime st_atim.tv_sec
    #define stMtime st_mtim.tv_sec
    #define stCtime st_ctim.tv_sec
#else                                        // stat.h version <= glibc-2.2.4
    #define stAtime st_atime
    #define stMtime st_mtime
    #define stCtime st_ctime
#endif

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <syslog.h>

#include "SymSaferStrings.h"

#include "ndkIntegers.h"
#include <stdint.h>
#include "ndkTime.h"

#include "ndkPaths.h"
#include "ndkDebugPrint.h"

// ================== Utilities =========================================================

// Copied here temporarily:
#ifndef FILE_ATTRIBUTE_READONLY
#define FILE_ATTRIBUTE_READONLY             0x00000001
#define FILE_ATTRIBUTE_HIDDEN               0x00000002
#define FILE_ATTRIBUTE_SYSTEM               0x00000004
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020
#define FILE_ATTRIBUTE_ENCRYPTED            0x00000040
#define FILE_ATTRIBUTE_NORMAL               0x00000080
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800
#define FILE_ATTRIBUTE_OFFLINE              0x00001000

#define FILE_ATTRIBUTE_DEVICE               0x01000000
#define FILE_ATTRIBUTE_FIFO                 0x02000000
#define FILE_ATTRIBUTE_LINK                 0x04000000
#define FILE_ATTRIBUTE_SOCKET               0x08000000
#endif

// As long as RTVScan is running as `root', this function is trivial. If not, you have to
// really think through how to handle file repair and quarantine with respect to the
// file's and its directory's `owner', `group', and `other' permissions. This is not the
// place to handle, just a little part of it.


static DWORD mkAttribute(struct stat* pStat) {
    DWORD val = 0;

    mode_t mode = pStat->st_mode;

    if ( S_ISCHR(mode) || S_ISBLK(mode) ) {
        val |= FILE_ATTRIBUTE_DEVICE;
    }
    if ( S_ISDIR(mode) ) {
        val |= FILE_ATTRIBUTE_DIRECTORY;
    }
    if ( S_ISFIFO(mode) ) {
        val |= FILE_ATTRIBUTE_FIFO;
    }
    if ( S_ISLNK(mode) ) {
        val |= FILE_ATTRIBUTE_LINK;
    }
    if ( S_ISSOCK(mode) ) {
        val |= FILE_ATTRIBUTE_SOCKET;
    }


    if ( val == 0 ) {
        // TODO finish READONLY, etc. support
        val = FILE_ATTRIBUTE_NORMAL;
    }

    return val;
}

// ================== Make Path =========================================================

// The `_makepath' function creates a single path and stores it in `path'. The path may
// include a drive letter, directory path, filename, and filename extension. `_wmakepath'
// is a wide-character version of `_makepath';  the arguments to `_wmakepath' are
// wide-character strings. `_wmakepath' and `_makepath' behave identically otherwise.
//
// All strings are null-terminated.
//
// The following arguments point to buffers containing the path elements:
//
//    `path'      Will contain the full path afterwards.
//
//    `drive'     Disregarded on Linux.
//
//    `dir'       Contains the path of directories, not including the drive designator or
//                the actual filename. The trailing slash is optional, and either a
//                forward slash "/" or a backslash "\" or both may be used in a single
//                `dir' argument. If a trailing slash ("/" or "\") is not specified, it
//                is inserted automatically. If `dir' is NULL or an empty string, no
//                slash is inserted in the composite `path' string. Backslashes will be
//                replaced by forward slashes.
//
//    `fname'     Contains the base filename without any extensions. If `fname' is NULL
//                or points to an empty string, no filename is inserted in the composite
//                `path' string.
//
//    `ext'       Contains the actual filename extension, with or without a leading
//                period ".". `_makepath' inserts the period automatically if it does not
//                appear in `ext'. If `ext' is a NULL character or an empty string, no
//                period is inserted in the composite `path' string.
//
// The `path' argument must point to an empty buffer large enough to hold the complete
// path.

void _wmakepath(wchar_t* path,const wchar_t* drive,
                const wchar_t* dir,const wchar_t* fname,const wchar_t* ext) {
   wchar_t* p = path; *path = L'\0';            // clear output

   if (dir && wcscmp(dir,L"")) {
      for (wchar_t* d = (wchar_t*) dir; *d; d++,p++) {
         wchar_t c = *d; *p = c == L'\\' ? L'/' : c;
      }
      if (*(p - 1) != L'/') *p++ = L'/';
   }
   if (fname && wcscmp(fname,L""))
      for (wchar_t* f = (wchar_t*) fname; *f; f++,p++) *p = *f;

   if (ext && wcscmp(ext,L"")) {
      if (*ext != L'.') *p++ = L'.';
      for (wchar_t* e = (wchar_t*) ext; *e; e++,p++) *p = *e;
   }
   *p = L'\0';
}
void _makepath(char* path,const char* drive,
               const char* dir,const char* fname,const char* ext) {
   char* p = path; *path = '\0';                // clear output

   if (dir && strcmp(dir,"")) {
      for (char* d = (char*) dir; *d; d++,p++) {
         char c = *d; *p = c == '\\' ? '/' : c;
      }
      if (*(p - 1) != '/') *p++ = '/';
   }
   if (fname && strcmp(fname,""))
      for (char* f = (char*) fname; *f; f++,p++) *p = *f;

   if (ext && strcmp(ext,"")) {
      if (*ext != '.') *p++ = '.';
      for (char* e = (char*) ext; *e; e++,p++) *p = *e;
   }
   *p = '\0';
}

// ================== Split Path ========================================================

// The `_splitpath' function breaks a path into its four components. `_splitpath'
// automatically handles multibyte-character string arguments as appropriate, recognizing
// multibyte-character sequences according to the multibyte code page currently in
// use. `_wsplitpath' is a wide-character version of `_splitpath';  the arguments to
// `_wsplitpath' are wide-character strings. These functions behave identically
// otherwise.
//
// All strings are null-terminated.
//
// The following arguments point to buffers containing the path elements:
//
//    `path'      Full path.
//
//    `drive'     Optional drive letter, followed by a colon ":". Disregarded in Linux.
//
//    `dir'       Optional directory path, including trailing slash. Forward slashes "/",
//                backslashes "\", or both may be used in the `path'. Backslashes will be
//                replaced with forward slashes.
//
//    `fname'     Base filename (no extension).
//
//    `ext'       Optional filename extension, including leading period ".".
//
// The other arguments point to buffers used to store the path elements. After a call to
// `_splitpath' is executed, these arguments contain empty strings for components not
// found in path. You can pass a NULL pointer to `_splitpath' for any component you don't
// need.

void _wsplitpath(const wchar_t* path,wchar_t* drive,
                 wchar_t* dir,wchar_t* fname,wchar_t* ext) {
   wchar_t *p = (wchar_t*) path,*q,c;

   if (drive) *drive = L'\0';                   // clear output
   if (dir)   *dir   = L'\0';
   if (fname) *fname = L'\0';
   if (ext)   *ext   = L'\0';

   p = wcschr(p,L':'); p = p ? p + 1 : (wchar_t*) path;

   c = *p;
   if (c == L'/' || c == L'\\') {
      q = max(wcsrchr(p,L'/'),wcsrchr(p,L'\\'));
      if (dir) {
         wchar_t* d;
         for (d = dir; p <= q; p++,d++) {c = *p; *d = c == L'\\' ? L'/' : c;}
         *d = L'\0';
      } else p = q + 1;
   }

   q = wcsrchr(p,L'.');
   if (fname) {
      wchar_t* f;
      for (f = fname; q ? p < q : *p; p++,f++) *f = *p;
      *f = L'\0';
   } else p = q;

   if (ext) {
      wchar_t* e;
      for (e = ext; *p; p++,e++) *e = *p;
      *e = L'\0';
   }
}
void _splitpath(const char* path,char* drive,
                char* dir,char* fname,char* ext) {
   char *p = (char*) path,*q,c;

   if (drive) *drive = '\0';                    // clear output
   if (dir)   *dir   = '\0';
   if (fname) *fname = '\0';
   if (ext)   *ext   = '\0';

   p = strchr(p,':'); p = p ? p + 1 : (char*) path;

   c = *p;
   if (c == '/' || c == '\\') {
      q = max(strrchr(p,'/'),strrchr(p,'\\'));
      if (dir) {
         char* d;
         for (d = dir; p <= q; p++,d++) {c = *p; *d = c == '\\' ? '/' : c;}
         *d = '\0';
      } else p = q + 1;
   }

   q = strrchr(p,'.');
   if (fname) {
      char* f;
      for (f = fname; q ? p < q : *p; p++,f++) *f = *p;
      *f = '\0';
   } else p = q;

   if (ext) {
      char* e;
      for (e = ext; *p; p++,e++) *e = *p;
      *e = '\0';
   }
}

// ================== End Of File =======================================================

// The `eof' function determines whether the end of the file associated with `fd' has
// been reached.
//
// Parameter
//
//    `fd'        File descriptor referring to open file.
//
// `eof' returns 1 if the current position is end of file, or 0 if it is not. A return
// value of -1 indicates an error;  in this case, `errno' is set to EBADF, which
// indicates an invalid file descriptor.

int eof(int fd) {
   char byte[2]; int n = read(fd,byte,1); if (n == 1) lseek(fd,-1,SEEK_CUR);

   return n == 0 ? 1 : n == 1 ? 0 : n;
}

// ================== File Length =======================================================

// Get the length of a file.
//
// Parameter
//
//    `fd'        Target file descriptor.
//
// `filelength' returns the file length, in bytes, of the target file associated with fd.
// Returns a value of -1L to indicate an error, and an invalid file descriptor sets
// `errno' to EBADF.

long filelength(int fd) {
   struct stat info; int rc = fstat(fd,&info);
   return rc == -1 ? rc : info.st_size;
}

// ================== Detect Keyboard Hit ===============================================

// Checks the console for keyboard input. Returns a nonzero value if a key has been
// pressed. Otherwise, it returns 0.
//
// The `kbhit' function checks the console for a recent keystroke. If the function
// returns a nonzero value, a keystroke is waiting in the buffer. The program can then
// call `getch' or `getwch' to get the keystroke.

class Poll {
public:
   Poll();
   ~Poll();

private:
   int  _flags;
   bool _isBlocking;
};

Poll::Poll() :
   _flags(fcntl(STDIN_FILENO,F_GETFL)),_isBlocking(! (_flags & O_NONBLOCK))
{
   if (_isBlocking) fcntl(STDIN_FILENO,F_SETFL,_flags | O_NONBLOCK);
}
Poll::~Poll() {
   if (_isBlocking) fcntl(STDIN_FILENO,F_SETFL,_flags);
}

// The `Poll' object ensure that the `read' is done in polling mode, i.e. it will return
// immediately to inform on whether the keyboard has been hit or not. If hit, it puts the
// byte read back to the input stream so that the next function that fetches input from
// the stream will start to read the character that was the hit.
//
// Note that the automatic invocation of the `Poll' object's desctructor as we leave the
// context of the `kbhit' function ensure that the stream is returned to its blocking
// state before the call, even when `kbhit' was interrupted.
// TODO: currently only works if user hits Enter, not just any key

int kbhit() {
   Poll poll;
   uint8_t byte = 0;
   int r;

   r = read(STDIN_FILENO, &byte, 1);
   if (r == 1) {
// TODO: need to fix this or drop it. From 'man ungetc':
//  It  is  not  advisable  to  mix calls to input functions from the stdio
//  library with low - level calls to read() for the file descriptor  asso-
//  ciated  with  the  input stream; the results will be undefined and very
//  probably not what you want.
      ungetc(byte, stdin);
      return 1;
   } else
     return 0;
}

// ================== dirent ============================================================

// Linux is using a modern verions of `dirent', while RTVScan on NetWare is using a much
// older version. Even though the names of the functions and data structures are
// identical, the data structures are radically different. Linux has switched over to use
// `streams' for file I/O, while the RTVScan is using a version of NDK that still is
// using integer `file descriptors'. The `dirent' structure in NDK contains various file
// related information, while the Linux version only contains directory oriented
// information and then relies on the `stat' function to provide the file information.
//
// To be able to implement the old functionality in the presence of newer functions with
// the same names, we use "sym" prefixed names for the old functionality in our
// implementation, while our users still use the regular names in their code -- we get
// around this by letting macros translate those names to our "sym" prefixed names.
//
// In the descriptions below, we will refer to the functions with the names that the
// users are using, while the implementation, of course, uses the prefixed names for the
// user functionality and non-prefixed names for the Linux underlying functions.

// --------------------- opendir --------------------------------------------------------

// Opens a directory for reading and the next matching file found by calling `readdir'
// functions.
//
// Parameters
//
//    `sPathName' Can be either relative to the current working directory or it can be an
//                absolute path name (must include file specification -- accepts wild
//                cards).
//
// Return Values
//
//    Returns a pointer to the `DIR' structure (required for subsequent calls to the
//    `readdir' function) containing the file names matching the pattern specified by the
//    pathname parameter.
//
//    Returns NULL if the path name is not valid or if there are no files matching the
//    path name. If an error occurs, `errno' are set.
//
// The last part of the path name can contain the characters "?" and "*" for matching
// multiple files. Wildcard searches are intended to be compatible with Win32 FindFirstFile
// and FindNextFile behavior. This includes taking a '.' as a separator between a file's
// name and its extension, and matching it to the last occurance of '.' in the directory
// being searched.
//
// More than one directory can be read at the same time by calling the
// `opendir', `readdir', and `closedir' functions.
//
// `opendir' allocates memory for a `DIR' structure. The `closedir' function frees the
// memory.
//
// Information about the first file or directory matching the specified path name is not
// placed in the `DIR' structure until after the first call to the `readdir' function.

symDIR* symOpenDir(const char* sPathName) {
   char *lastSlash = strrchr(sPathName, '/');
   char *star = strchr(sPathName, '*');
   char *question = strchr(sPathName, '?');
   if ( (star && star < lastSlash)
        || (question && question < lastSlash)
        || (!lastSlash && (star || question) ) ) {
      // Wildcards in directory parts.
      errno = ENOENT;
      return NULL;
   }

   char *basePath = NULL;
   size_t baseLen = 0;
   char *matchPart = NULL;
   size_t matchLen = 0;
   if ( star || question ) {
      baseLen = lastSlash - sPathName + 1;
      basePath = new char[baseLen + 1];
      ssStrnCpyn( basePath, sPathName, baseLen + 1, baseLen );

      matchLen = strlen(lastSlash + 1);
      matchPart = new char[matchLen + 1];
      ssStrnCpy( matchPart, lastSlash + 1, matchLen + 1 );
   } else {
      baseLen = strlen(sPathName);
      bool endSlash = sPathName[baseLen - 1] == '/';
      basePath = new char[baseLen + (endSlash ? 1 : 2)];
      ssStrnCpyn( basePath, sPathName, baseLen + (endSlash ? 1 : 2), baseLen + (endSlash ? 0 : 1) );
      if (!endSlash) {
         strcat(basePath, "/");
         baseLen++;
      }
   }


   DIR* pDIR = opendir(basePath);
   if (! pDIR)
   {
      delete[] basePath;
      if ( matchPart )
         delete[] matchPart;
      // leave errno to whatever opendir set it to
      return NULL;
   }

   symDIR* pSymDIR    = new symDIR;
   pSymDIR->__pDIR    = static_cast<void*>(pDIR);
   dirent* pDirent    = (dirent*)(new char[DIRENT_TOTAL]);
   pSymDIR->__pDirent = static_cast<void*>(pDirent);


   pSymDIR->__sDirName = basePath;
   pSymDIR->__nDirLength = baseLen;
   pSymDIR->__sMatch = matchPart;
   pSymDIR->__nMatchLength = matchLen;

   return pSymDIR;
}

// --------------------- closedir -------------------------------------------------------

// Closes a specified directory.
//
// Parameters
//
//    `pDIR'      Specifies the directory to be closed.
//
// Return Values
//
//    0x00        ESUCCESS
//    0x04        EBADF
//
// `closedir' closes the directory specified by the `pDIR' parameter and frees the memory
// allocated by the `opendir' function. All open directories are automatically closed
// when an application is terminated.

int symCloseDir(symDIR* pSymDIR) {
   DIR* pDIR = (DIR*) pSymDIR->__pDIR;
   int rc = closedir(pDIR);

   delete[] pSymDIR->__sDirName;
   delete[] (char*)pSymDIR->__pDirent;
   if ( pSymDIR->__sMatch ) {
      delete[] pSymDIR->__sMatch;
      pSymDIR->__sMatch = 0;
   }
   delete pSymDIR;

   return rc;
}

/**
 * Checks a string for matches to a pattern.
 *
 * The last part of the path name can contain the characters "?" and "*" for matching
 * multiple files. Wildcard searches are intended to be compatible with Win32 FindFirstFile
 * and FindNextFile behavior. This includes taking a '.' as a separator between a file's
 * name and its extension, and matching it to the last occurance of '.' in the string
 * being checked.
 *
 * @param pattern The pattern to match against.
 * @param str The string being checked for matching.
 *
 * @return true if the string matches the pattern, false otherwise.
 */
bool matches( const char* pattern, const char* str )
{
   bool match = false;
   if ( pattern ) {
      match = true;

      int src = 0;
      int dst = 0;
      while ( match && pattern[src] && str[dst] ) {
         switch( pattern[src] ) {
            case '*':
            {
               src++;
               if ( pattern[src] == '.' ) {
                  const char* loc = strrchr( &str[dst], '.' );
                  if ( loc ) {
                     dst += loc - &str[dst];
                  } else {
                     match = false;
                  }
               } else {
                  // eat chars
                 while ( str[dst] && str[dst] != pattern[src] ) {
                    dst++;
                 }
               }
            }
            break;
            case '?':
            {
               src++;
               dst++;
            }
            break;
            default:
               match &= (pattern[src++] == str[dst++]);
         }
      }
      match &= pattern[src] == 0 && str[dst] == 0;

   } else {
      match = true;
   }
   return match;
}

// --------------------- readdir --------------------------------------------------------

// Obtains information about the next matching file.
//
// Parameters
//
//    `pDIR'      Specifies the structure to receive information about the next matching
//                file.
//
// Return Values
//
//    Returns a pointer to an object of the `DIR' structure type containing information
//    about the next matching file or directory.
//
//    If an error occurs, such as when there are no more matching file names, NULL is
//    returned and `errno' is set.
//
// `readdir' can be called repeatedly to obtain the list of file and directory names
// contained in the directory specified by the path name given to the `opendir' function.
// The Unix specific directory members "." and ".." are not returned.
//
// The `closedir' function must be called to close the directory and free the memory
// allocated by the `opendir' function.

symDIR* symReadDir(symDIR* pSymDIR) {
    symDIR* retVal = pSymDIR;
    dirent *pDirent = static_cast<dirent*>(pSymDIR->__pDirent);
    char* name = 0;
    struct stat info = {0};
    struct _DOSDate date = {0};
    struct _DOSTime time = {0};
    bool isLink = false;

    bool valid = false;
    while ( retVal && !valid ) {
        do {
            dirent *pResult = 0;
            int rc = readdir_r( static_cast<DIR*>(pSymDIR->__pDIR), pDirent, &pResult);
            if ( !pResult || rc )
            {
                // pResult == NULL at eod
                retVal = NULL;
                errno = rc ? rc : ENOENT;
                break;
            }
            name = pDirent->d_name;
        } while ( (strcmp(name, ".") == 0)
                  || (strcmp(name, "..") == 0)
                  || !matches(pSymDIR->__sMatch, name) );

        if ( retVal ) {
            char* sFileName = new char[pSymDIR->__nDirLength + strlen(name) + 1];
            strcpy(sFileName, pSymDIR->__sDirName);
            strcat(sFileName, name);
            lstat(sFileName, &info);

            // Only allow links, regular files and directories.
            // Skip over everything else for now.
            valid = S_ISREG(info.st_mode) || S_ISDIR(info.st_mode) || S_ISLNK(info.st_mode);

            if ( valid && S_ISLNK(info.st_mode) ) {
                // Get the info for the link target instead of the link itself
                stat(sFileName, &info);
                isLink = true;
            }

            delete[] sFileName;
        }
    }

    if ( retVal ) {
        if (S_ISREG(info.st_mode)) {
            pSymDIR->d_size = info.st_size;
            _ConvertTimeToDOS(info.stAtime, &date, &time);
            pSymDIR->d_adatetime = date.packed_date << 16 | time.packed_time;
        } else {
            pSymDIR->d_size      = 0;
            pSymDIR->d_adatetime = 0;
        }

        pSymDIR->d_attr = mkAttribute(&info);
        if ( isLink ) {
            pSymDIR->d_attr |= FILE_ATTRIBUTE_LINK;
        }

        _ConvertTimeToDOS(info.stMtime, &date, &time);
        pSymDIR->d_date = date.packed_date;
        pSymDIR->d_time = time.packed_time;

        _ConvertTimeToDOS(info.stCtime, &date, &time);
        pSymDIR->d_cdatetime = date.packed_date << 16 | time.packed_time;

        ssStrnCpy( pSymDIR->d_name, name, sizeof(pSymDIR->d_name) );
    }

    return retVal;
}

// ================== Facilities ========================================================

char* mkUnixPath(char* path) {
   for (char* p = path; *p; p++) if (*p == '\\') *p = '/';
   return path;
}
