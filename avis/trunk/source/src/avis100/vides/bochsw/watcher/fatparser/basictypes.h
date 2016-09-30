#ifndef BASICTYPES_H
#define BASICTYPES_H

// Just the basic types. Useless if you use windows.h

#ifndef WIN32

  #ifndef BYTE
  typedef unsigned char BYTE;
  #endif

  #ifndef WORD
  typedef unsigned short WORD;
  #endif

  #ifndef DWORD
  typedef unsigned long DWORD;
  #endif

#endif /* WIN32 */

#endif
