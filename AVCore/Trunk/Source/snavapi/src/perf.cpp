#include "perf.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifdef SYM_LINUX_X86
static inline unsigned long long read_rdtsc(void)
{
  unsigned long long d;
  __asm__ __volatile__ ("rdtsc" : "=A" (d) );
  return d;
}
#endif


BOOL QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency)
{
#ifdef SYM_NLM
	return FALSE;
#elif SYM_SOLARIS
  lpFrequency->HighLow.LowPart = 1000000000;
  lpFrequency->HighLow.HighPart = 0;
  return TRUE;
#elif defined(SYM_LINUX_X86)
  // Check for existence of /proc/cpuinfo, and
  // "tsc" on the "flags" line.
  FILE* fp;
  char szTemp[1024];
  int i;
  double dCPUMHz;

  fp = fopen("/proc/cpuinfo", "rb");

  dCPUMHz = 0;
  while (!feof(fp))
    {
      i = 0;
      szTemp[i] == '\0';
      while (!feof(fp) && szTemp[i-1] != '\n' && i < 1023)
	{
	  szTemp[i] = getc(fp);
	  i++;
	}
      szTemp[i] == '\0';

      // Check if it's the "cpu MHz" line.
      if (strncmp(szTemp, "cpu MHz", strlen("cpu MHz")) == 0)
	{
	  for (i=0; szTemp[i] != ':'; i++)
	    {
	      if (szTemp[i] == '\0')
		{
		  fclose(fp);
		  return FALSE;
		}
	    }
	  if (szTemp[i+1] != ' ')
	    {
	      fclose(fp);
	      return FALSE;
	    }
	  sscanf(&(szTemp[i+2]), "%le", &dCPUMHz);
	}

      // Check if it's the "flags" line.
      if (strncmp(szTemp, "flags", strlen("flags")) == 0)
	{
	  // See if we can find " tsc" somewhere.
	  if (strstr(szTemp, " tsc") == '\0')
	    {
	      fclose(fp);
	      return FALSE;
	    }
	}
    }

  fclose(fp);

  lpFrequency->QuadPart = (long long) (dCPUMHz * 1000000);

  return TRUE;
#else
  return FALSE;
#endif //#ifdef SYM_NLM
}

BOOL QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount)
{
#ifdef SYM_NLM
	return FALSE;
#elif SYM_SOLARIS
  lpPerformanceCount->QuadPart = (unsigned long long) gethrtime();
  return TRUE;
#elif defined(SYM_LINUX_X86)
  lpPerformanceCount->QuadPart = read_rdtsc();
  return TRUE;
#else
  return FALSE;
#endif // #ifdef SYM_NLM
}
