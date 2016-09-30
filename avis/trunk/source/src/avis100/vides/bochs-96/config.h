/* config.h.  Generated automatically by configure.  */
/*
Copyright Notice
================
BOCHS is Copyright 1994,1995,1996 by Kevin P. Lawton.

BOCHS is commercial software.

For more information, read the file 'LICENSE' included in the bochs
distribution.  If you don't have access to this file, or have questions
regarding the licensing policy, the author may be contacted via:

    US Mail:  Kevin Lawton
              528 Lexington St.
              Waltham, MA 02154

    EMail:    bochs@world.std.com
*/


#ifdef _BX_CONFIG_H_
#else
#define _BX_CONFIG_H_ 1

#define BX_CPU 4 // try 4

#define WORDS_BIGENDIAN 0

#define SIZEOF_UNSIGNED_CHAR      1
#define SIZEOF_UNSIGNED_SHORT     2
#define SIZEOF_UNSIGNED_INT       4
#define SIZEOF_UNSIGNED_LONG      4
#define SIZEOF_UNSIGNED_LONG_LONG 8

#if SIZEOF_UNSIGNED_CHAR != 1
#  error "sizeof (unsigned char) != 1"
#else
  typedef unsigned char Bit8u;
  typedef   signed char Bit8s;
#endif

#if SIZEOF_UNSIGNED_SHORT != 2
#  error "sizeof (unsigned short) != 2"
#else
  typedef unsigned short Bit16u;
  typedef   signed short Bit16s;
#endif

#if SIZEOF_UNSIGNED_INT == 4
  typedef unsigned int Bit32u;
  typedef   signed int Bit32s;
#elif SIZEOF_UNSIGNED_LONG == 4
  typedef unsigned long Bit32u;
  typedef   signed long Bit32s;
#else
#  error "can't find sizeof(type) of 4 bytes!"
#endif

#if SIZEOF_UNSIGNED_LONG == 8
  typedef unsigned long Bit64u;
  typedef   signed long Bit64s;
#elif SIZEOF_UNSIGNED_LONG_LONG == 8
   #ifdef WIN32
     typedef unsigned __int64 Bit64u;
     typedef signed   __int64 Bit64s;
   #else
     typedef unsigned long long Bit64u;
     typedef   signed long long Bit64s;
   #endif
#else
#  define BX_NO_64BIT_TYPE
#endif

#if WORDS_BIGENDIAN
#  ifndef BIG_ENDIAN
#    define BIG_ENDIAN
#  endif
#else
#  ifndef LITTLE_ENDIAN
#    define LITTLE_ENDIAN
#  endif
#endif

#define HAVE_SIGACTION 1

#define USE_INLINE 0

#define inline inline

#define BX_USE_NATIVE_FLOPPY 0

#define USE_DEBUGGER 0

#define BX_PHY_MEM_MEGS 2
#define BX_PHY_MEM_SIZE  (BX_PHY_MEM_MEGS * 1024 * 1024)

#define BX_EXTERNAL_ENVIRONMENT 0

#define BX_EMULATE_HGA_DUMPS 0

#define BX_DMA_FLOPPY_IO 0

#define BX_SUPPORT_V8086_MODE 0
#define BX_SUPPORT_PAGING     0
#define BX_SUPPORT_TASKING    0

#define BX_SUPPORT_VGA        0

#endif /* _BX_CONFIG_H */
