#ifdef LINUX
   #ifndef LINUX_WRAP_NTX_H_INCLUDED
      #ifdef _NTX_H
         #error "ntx.h already included"
      #endif
      #define LINUX_WRAP_NTX_H_INCLUDED
   #endif

   #define HANDLE void *
#endif

#if defined LINUX && ! defined NLM
   #define NLM
   #include "../../../../../Release/AMS/include/ntx.h"
   #undef NLM
#else
   #include "../../../../../Release/AMS/include/ntx.h"
#endif

#ifdef LINUX
   #undef TCHAR
   #undef BOOL
#endif
