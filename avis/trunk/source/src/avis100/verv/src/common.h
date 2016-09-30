#ifndef __COMMON_H
#define __COMMON_H
typedef unsigned long   dword;
typedef unsigned char   byte;
typedef unsigned long   polyn;
#ifdef BCOS2
/*#error "polymath !"*/
   typedef short twobytes;
   typedef unsigned short utwobytes;
#else
/*#error "others !"*/
   typedef int twobytes;
   typedef unsigned int utwobytes;
#endif
#endif
