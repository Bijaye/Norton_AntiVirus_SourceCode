#include <stdio.h>
#include <string.h>

typedef unsigned long DWORD;

main()
{
	DWORD       dwTotalFiles, dwIter, dwPages, dwTemp, dwTotalLines;
	char        temp[512], x[128],y[128],name[128];

	dwTotalFiles = dwIter = dwPages = dwTotalLines = 0;

	while (!feof(stdin))
	{
		gets(temp);
		if (strstr(temp,"Scan:"))
			strcpy(name,temp);
		if (!strncmp(temp,"dwNumIter",9))
		{
			dwTotalFiles++;
			sscanf(temp,"%s %s %lu",x,y,&dwTemp);
			dwIter += dwTemp;
			if (dwTemp > 50)
				printf("\nBig:%s,%s\n",temp,name);
		}
		else if (!strncmp(temp,"Pages read",10))
		{
			sscanf(temp,"%s %s %s %lu",x,y,x,&dwTemp);
			dwPages += dwTemp;
		}

		if (dwTotalFiles != 0)
            fprintf(stderr,"%-8lu: Avg iter: %-8lu, Avg pages: %-8lu\r",
					dwTotalFiles,
					dwIter/dwTotalFiles,
					dwPages/dwTotalFiles);
    }

    if (dwTotalFiles != 0)
        printf("%-8lu: Avg iter: %-8lu, Avg pages: %-8lu\r",
               dwTotalFiles,
               dwIter/dwTotalFiles,
               dwPages/dwTotalFiles);

}
