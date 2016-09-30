#include <stdio.h>
#include <string.h>

#include <windows.h>

#include "killapp.h"

char szClassName[100];
char szUserName[100];
char szPassword[100];


void usage()
{
	printf ("Usage : killapp Classname [-user usrname -password passwd]\n");
	printf ("        Classname : class name of the main window of the application (ex : OpusApp for Word)\n");	
	exit(3);
}


void process_params(int argc, char * argv[])
{
int nArgs;
BOOL bUserSpecified = FALSE;
BOOL bPasswordSpecified = FALSE;

	if (argc ==1) {usage();}
	else
	{
		strcpy(szClassName, argv[1]);

		for(nArgs = 2; nArgs < argc; ++nArgs)
		{
			if ((!(stricmp(argv[nArgs], "-user")))&&(nArgs + 1 < argc)) 
				strcpy(szUserName,argv[++nArgs]);
			else if ((!(stricmp(argv[nArgs], "-password")))&&(nArgs + 1 < argc)) 
				strcpy(szPassword,argv[++nArgs]);
			else
				usage();
		}
		if (bUserSpecified && !bPasswordSpecified) usage();
	}
}


void main(int argc, char * argv[])
{
long lRes = 0;

	szUserName[0] = 0;
	szPassword[0] = 0;

	process_params(argc, argv);

	if (szUserName[0] != 0) lRes = KillAllUserAppInstances(szClassName, szUserName, szPassword);
	else lRes = KillAllAppInstances(szClassName);
		
exit (lRes); 
}
