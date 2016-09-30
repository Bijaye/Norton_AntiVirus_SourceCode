#include <stdio.h>         /* definition of I/O constants        */
#ifdef VMS
#include <file.h>
#else
#include <fcntl.h>     /* definition of system I/O constants */
#endif

/********************************************************************/
/*   Begin updates by Abdelaziz Mounji (FUNDP)                      */
/********************************************************************/

#ifdef BS2000
#include <sys.time.h>
#endif
#ifdef VMS
#include <time.h>
#endif
#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif


/********************************************************************/
/*                         End updates                              */
/********************************************************************/


/* definitions of system constants */
#define MAXPATHLEN   1024
 
/* definition of macros */
#define MIN(a,b) (((a)<(b))?(a):(b))    /* minimum between a and b */
#define MAX(a,b) (((a)>(b))?(a):(b))    /* maximum between a and b */
 
 
