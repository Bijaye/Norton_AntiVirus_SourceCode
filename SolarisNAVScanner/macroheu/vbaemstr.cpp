//***********************************************************************
//
// $Header:   S:/MACROHEU/VCS/VBAEMSTR.CPv   1.0   19 Aug 1997 17:33:24   DCHI  $
//
// Description:
//  Contains code to generate string lists for VBA 5 emulator.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/VBAEMSTR.CPv  $
// 
//    Rev 1.0   19 Aug 1997 17:33:24   DCHI
// Initial revision.
// 
//************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRINGS     512
#define MAX_STR_LEN     32

char gapszStrs[MAX_STRINGS][MAX_STR_LEN];

int sort_function( const void *a, const void *b)
{
   return( strcmp((char *)a,(char *)b) );
}

void main(int argc, char *argv[])
{
    FILE *  pf;
    int     i, j, nNumStrs,nLen;
    int     nOffset;
    char    szStr[256];
    char *  pszStr;
    char *  pszDollar;

    if (argc < 4)
    {
        printf("Usage: vbaemstr wordlist output prefix\n");
        printf("         (i.e., vbaemstr o97str.lst o97str O97\n");
        printf("         (i.e., vbaemstr w97str.lst w97str W97\n");
        return;
    }

    pf = fopen(argv[1],"rt");
    if (pf == NULL)
    {
        printf("Failed to open %s for reading!\n",argv[1]);
        return;
    }

    // Read the strings

    nNumStrs = 0;
    while (1)
    {
        if (fscanf(pf,"%s",szStr) != 1)
            break;

        if (strlen(szStr) > 31)
        {
            printf("String '%s' length is greater than 31!\n",
                szStr);

            break;
        }

        strcpy(gapszStrs[nNumStrs++],szStr);
    }

    printf("%d strings read\n",nNumStrs);
    fclose(pf);

    // Sort the strings

    qsort(gapszStrs,nNumStrs,MAX_STR_LEN,sort_function);

    // Generate the header file

    sprintf(szStr,"%s.h",argv[2]);
    pf = fopen(szStr,"wt");
    if (pf == NULL)
    {
        printf("Failed to open %s for writing!\n",szStr);
        return;
    }

    fprintf(pf,"typedef enum tagE%s_STR\n{\n",argv[3]);

    for (i=0;i<nNumStrs;i++)
    {
        pszStr = gapszStrs[i];
        pszDollar = strchr(pszStr,'$');
        if (pszDollar != NULL)
        {
            strcpy(szStr,pszStr);
            szStr[pszDollar-pszStr] = 'S';
            pszStr = szStr;
        }
        fprintf(pf,"    e%s_STR_%-36s= %d,\n",argv[3],pszStr,i);
    }

    fprintf(pf,"} E%s_STR_T, FAR *LPE%s_STR;\n\n",argv[3],argv[3]);

    fprintf(pf,"extern LPBYTE FAR galpby%s_STR[];\n\n",argv[3]);

    fprintf(pf,"#define NUM_%s_STR             %d\n",argv[3],nNumStrs);

    fclose(pf);

    // Generate the source file

    sprintf(szStr,"%s.cpp",argv[2]);
    pf = fopen(szStr,"wt");
    if (pf == NULL)
    {
        printf("Failed to open %s for writing!\n",szStr);
        return;
    }

    fprintf(pf,"#include \"platform.h\"\n\n");

    fprintf(pf,"BYTE FAR gaby%s_STR[] =\n{\n",argv[3]);

    for (i=0;i<nNumStrs;i++)
    {
        nLen = strlen(gapszStrs[i]);
        fprintf(pf,"    %d,",nLen);
        for (j=0;j<nLen;j++)
            fprintf(pf,"'%c',",gapszStrs[i][j]);
        fprintf(pf,"\n");
    }

    fprintf(pf,"};\n\n");

    fprintf(pf,"LPBYTE FAR galpby%s_STR[] =\n{\n",argv[3]);

    nOffset = 0;
    for (i=0;i<nNumStrs;i++)
    {
        fprintf(pf,"    gaby%s_STR + %4d,    // %3d:%s\n",
            argv[3],nOffset,i,gapszStrs[i]);
        nOffset += strlen(gapszStrs[i]) + 1;
    }

    fprintf(pf,"};\n\n");

    fclose(pf);
}

