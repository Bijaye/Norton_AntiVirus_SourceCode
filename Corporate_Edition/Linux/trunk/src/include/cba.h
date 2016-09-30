#ifdef LINUX
   #ifndef LINUX_WRAP_CBA_H_INCLUDED
      #ifdef _CBA_H
         #error "cba.h already included"
      #endif
      #define LINUX_WRAP_CBA_H_INCLUDED
   #endif

   #define HANDLE void *
#endif

#if defined LINUX && ! defined NLM
   #define NLM
   #include "../../../../../Release/AMS/include/cba.h"
   #undef NLM
#else
   #include "../../../../../Release/AMS/include/cba.h"
#endif

#ifdef LINUX
   #undef TCHAR
   #undef BOOL
#endif
