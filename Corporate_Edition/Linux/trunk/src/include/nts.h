
#ifdef LINUX
   #ifndef LINUX_WRAP_NTS_H_INCLUDED
      #ifdef _NTS_H
         #error "nts.h already included"
      #endif
      #define LINUX_WRAP_NTS_H_INCLUDED
   #endif

   #ifdef WAIT_OBJECT_0
      #undef WAIT_OBJECT_0
   #endif

   #define HANDLE void *
#endif

#if defined LINUX && ! defined NLM
   #define NLM
   #include "../../../../../Release/AMS/include/nts.h"
   #undef NLM
#else
   #include "../../../../../Release/AMS/include/nts.h"
#endif

#ifdef LINUX
   #ifdef WAIT_OBJECT_0
      #undef WAIT_OBJECT_0
   #endif
   #define WAIT_OBJECT_0 0x00000000UL
   #undef TCHAR
   #undef BOOL
#endif
