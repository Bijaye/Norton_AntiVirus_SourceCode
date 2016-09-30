#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "avtypes.h"
#include "boodata.h"
#include "token.h"

long                            gnLines = 1;

int VerifyToken
(
    FILE *                      stream,
    LPSTR                       lpszRestToken,
    int                         nReturnOnMatch
)
{
    int                         i, nLen, ch;

    nLen = strlen(lpszRestToken);

    for (i=0;i<nLen;i++)
    {
        ch = toupper(fgetc(stream));
        if (ch == EOF)
        {
            printf("error: EOF encountered in function name on line %ld\n",gnLines);
            return(TOKEN_TYPE_ERROR);
        }
        if (ch != lpszRestToken[i])
            return(TOKEN_TYPE_ERROR);
    }

    return(nReturnOnMatch);
}

int GetNextToken
(
    FILE *                          stream,
    LPLONG                          lplValue,
    LPBYTE                          lpbyMask,
    LPSTR                           lpszString
)
{
	int                             ch, nShift, i,
                                    nResult;

    while (!feof(stream))
    {
        ch = fgetc(stream);

        if (ch == EOF)
        {
            break;
        }

        switch (toupper(ch))
        {
            case ' ':
            case ',':
            case '\t':
            case '\r':
            case '\n':
                if (ch == '\r' || ch == '\n')
                    gnLines++;
                continue;
            case ';':
                while (!feof(stream) && ch != '\r' && ch != '\n')
                    ch = fgetc(stream);
                if (ch == EOF)
                    return(TOKEN_TYPE_EOF);
                gnLines++;
                break;
            case '(':
                return(TOKEN_TYPE_OPEN_PAREN);
            case ')':
                return(TOKEN_TYPE_CLOSE_PAREN);
            case '#':
                *lpbyMask = 0xFF;
				fscanf(stream,"%ld",lplValue);
                return(TOKEN_TYPE_VALUE);
            case '0':
                ch = fgetc(stream);
                if (ch == EOF)
                {
                    printf("error: EOF encountered in hex string on line %ld\n",gnLines);
                    return(TOKEN_TYPE_ERROR);
                }

                if (toupper(ch) == 'X')
                {
                    *lpbyMask = 0xFF;
                    fscanf(stream,"%lx",lplValue);
                    return(TOKEN_TYPE_VALUE);
                }
                return(TOKEN_TYPE_VALUE);
            case '@':
				nShift = 7;
				*lpbyMask = 0;
				*lplValue = 0;
				for (i=0;i<8;i++, nShift--)
				{
					ch = fgetc(stream);
					if (ch == EOF)
					{
						printf("error: EOF encountered in binary string on line %ld\n",gnLines);
						return(TOKEN_TYPE_ERROR);
					}
					if (ch == '1')
					{
						*lplValue |= (1 << nShift);
						*lpbyMask |= (1 << nShift);
					}
					else if (ch == '0')
					{
						*lpbyMask |= (1 << nShift);
					}
					else if (ch == '?')
					{
						//
					}
					else
                    {
                        printf("error: invalid character %02X (%c) in binary string on line %ld\n",ch,ch,gnLines);
                        return(TOKEN_TYPE_ERROR);
                    }
				}

				return(TOKEN_TYPE_VALUE);

            case 'S':   // scan!

                nResult = VerifyToken(stream,"CAN",TOKEN_TYPE_SCAN);

                if (nResult == TOKEN_TYPE_ERROR)
                {
                    printf("error: invalid function on line %ld\n",gnLines);
                }

                return(nResult);

            case 'B':   // bingo

                nResult = VerifyToken(stream,"INGO",TOKEN_TYPE_BINGO);

                if (nResult == TOKEN_TYPE_ERROR)
                {
                    printf("error: invalid function on line %ld\n",gnLines);
                }

                return(nResult);

            case 'T':   // Thresholds

                nResult = VerifyToken(stream,"HRESHOLDS",TOKEN_TYPE_THRESHOLDS);

                if (nResult == TOKEN_TYPE_ERROR)
                {
                    printf("error: invalid function on line %ld\n",gnLines);
                }

                return(nResult);

            case 'N':   // New()

                nResult = VerifyToken(stream,"EW",TOKEN_TYPE_BEGIN);

                if (nResult == TOKEN_TYPE_ERROR)
                {
                    printf("error: invalid function on line %ld\n",gnLines);
                }

                return(nResult);

            case '\"':
                i = 0;

                do
                {
                    ch = fgetc(stream);

                    if (ch == EOF)
                    {
                        printf("error: EOF encountered in string on line %ld\n",gnLines);
                        return(TOKEN_TYPE_ERROR);
                    }

					if (ch != '\"')
						lpszString[i++] = ch;
                }
                while (ch != '\"');
                lpszString[i] = 0;

                return(TOKEN_TYPE_STRING);

            default:
                printf("error: invalid character %02X (%c) on line %ld\n",ch,ch,gnLines);
        }
    }

    return(TOKEN_TYPE_EOF);
}

BOOL GetOpenParen
(
    FILE *                          in
)
{
    int                             nResult;
    long                            lValue;
    BYTE                            byMask;
    char                            szString[128];

    nResult = GetNextToken(in, &lValue, &byMask, szString);

    if (nResult != TOKEN_TYPE_OPEN_PAREN)
        return(FALSE);

    return(TRUE);
}

BOOL GetCloseParen
(
    FILE *                          in
)
{
    int                             nResult;
    long                            lValue;
    BYTE                            byMask;
    char                            szString[128];

    nResult = GetNextToken(in, &lValue, &byMask, szString);

    if (nResult != TOKEN_TYPE_CLOSE_PAREN)
        return(FALSE);

	return(TRUE);
}

int Compile
(
    FILE *                          in,
	FILE *                          out
)
{
	int                             nResult, nIndex, i, nLen;
    long                            lValue;
    BYTE                            byMask;
    char                            szString[128];
    BYTE                            bySig[512];
    int                             nFBR, nMBR, nPBR, nCurSig = 0;

    nFBR = nMBR = nPBR = 0;
	nCurSig = nIndex = 0;

	fprintf(out,"#ifndef _BOODEFS_H\n");
	fprintf(out,"#define _BOODEFS_H\n\n");

    fprintf(out,"\n#ifdef BOO_DEFINE_DATA\n\n");

	do
	{
		nResult = GetNextToken(in, &lValue, &byMask, szString);

		switch (nResult)
		{
			case TOKEN_TYPE_THRESHOLDS:
				if (GetOpenParen(in) != TRUE)
				{
					printf("error: invalid threshold function on line %ld.\n",gnLines);
					return(-1);
				}

				if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
                    printf("error: invalid threshold function on line %ld.\n",gnLines);
                    return(-1);
                }

                nFBR = (int)lValue;

                if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
                    printf("error: invalid threshold function on line %ld.\n",gnLines);
                    return(-1);
                }

                nPBR = (int)lValue;

                if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
                    printf("error: invalid threshold function on line %ld.\n",gnLines);
                    return(-1);
                }

                nMBR = (int)lValue;

                if (GetCloseParen(in) != TRUE)
                {
                    printf("error: invalid threshold function on line %ld.\n",gnLines);
                    return(-1);
                }

                break;

            case TOKEN_TYPE_BEGIN:
                nIndex = 0;
                if (GetOpenParen(in) != TRUE)
                {
                    printf("error: invalid new function on line %ld.\n",gnLines);
                    return(-1);
                }

                if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
                    printf("error: invalid new function on line %ld.\n",gnLines);
                    return(-1);
                }

                bySig[nIndex++] = (int)lValue;

                if (GetCloseParen(in) != TRUE)
                {
                    printf("error: invalid new function on line %ld.\n",gnLines);
                    return(-1);
                }
				break;

            case TOKEN_TYPE_VALUE:

                bySig[nIndex++] = BOOT_FUNC_MATCH;
                bySig[nIndex++] = (BYTE)lValue;
                bySig[nIndex++] = byMask;
                break;

            case TOKEN_TYPE_SCAN:

                if (GetOpenParen(in) != TRUE)
                {
                    printf("error: invalid scan function on line %ld.\n",gnLines);
                    return(-1);
                }

                bySig[nIndex++] = BOOT_FUNC_SCAN;

                if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
                    printf("error: invalid scan function on line %ld.\n",gnLines);
                    return(-1);
                }

                bySig[nIndex++] = (int)lValue;

                if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
                    printf("error: invalid scan function on line %ld.\n",gnLines);
                    return(-1);
                }

                bySig[nIndex++] = (int)lValue;
                bySig[nIndex++] = byMask;

                if (GetCloseParen(in) != TRUE)
                {
                    printf("error: invalid scan function on line %ld.\n",gnLines);
                    return(-1);
                }
                break;
            case TOKEN_TYPE_BINGO:

                if (GetOpenParen(in) != TRUE)
                {
                    printf("error: invalid scan function on line %ld.\n",gnLines);
                    return(-1);
                }

                bySig[nIndex++] = BOOT_FUNC_BINGO;

                if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
                    printf("error: invalid bingo function on line %ld.\n",gnLines);
                    return(-1);
                }

                bySig[nIndex++] = (BYTE)(lValue & 0xFF);
                bySig[nIndex++] = (BYTE)((lValue >> 8) & 0xFF);

                if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
                    printf("error: invalid bingo function on line %ld.\n",gnLines);
                    return(-1);
                }

                bySig[nIndex++] = (BYTE)(lValue & 0xFF);
                bySig[nIndex++] = (BYTE)((lValue >> 8) & 0xFF);

                if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_VALUE)
                {
					printf("error: invalid bingo function on line %ld.\n",gnLines);
                    return(-1);
                }

                bySig[nIndex++] = (BYTE)(lValue & 0xFF);
                bySig[nIndex++] = (BYTE)((lValue >> 8) & 0xFF);

				if (GetNextToken(in, &lValue, &byMask, szString) != TOKEN_TYPE_STRING)
                {
                    printf("error: invalid bingo function on line %ld.\n",gnLines);
                    return(-1);
				}

				if (GetCloseParen(in) != TRUE)
				{
					printf("error: invalid scan function on line %ld.\n",gnLines);
					return(-1);
				}

                nLen = strlen("BYTE gabySig0000[] = {");
                fprintf(out,"BYTE gabySig%04d[] = {",nCurSig);

				for (i=0;i<nIndex;i++)
				{

					if (nLen > 70)
					{
                        nLen = strlen("BYTE gabySig0000[] = {");
                        fprintf(out,"\n                     ");
					}
					nLen += 7;
                    fprintf(out,"0x%02X",bySig[i]);
					if (i < nIndex - 1)
						fprintf(out,",");
				}

				fprintf(out,"};\n");

//				fprintf(out,"char gszName%04d[] = \"(%d) %s\";\n\n",nCurSig,nCurSig,szString);
				fprintf(out,"//char gszName%04d[] = \"(%d) %s\";\n\n",nCurSig,nCurSig,szString);

				nCurSig++;

				break;
		}
	}
	while (nResult != TOKEN_TYPE_EOF && nResult != TOKEN_TYPE_ERROR);

	if (nResult != TOKEN_TYPE_EOF)
		return(-1);

	fprintf(out,"\n\n");
	fprintf(out,"SIG_TYPE_T gastSigs[] = {\n");

	for (i=0;i<nCurSig;i++)
	{

	// no names anymore

        fprintf(out,"\t{ gabySig%04d /*, gszName%04d */ }",i,i);

		if (i < nCurSig - 1)
			fprintf(out,",\n");
		else
			fprintf(out,"\n");

	}

	fprintf(out,"};\n\n");

    fprintf(out,"\n#endif // #ifdef BOO_DEFINE_DATA\n\n");

	fprintf(out,"#define FBR_THRESHOLD 		%d\n",nFBR);
	fprintf(out,"#define PBR_THRESHOLD 		%d\n",nPBR);
	fprintf(out,"#define MBR_THRESHOLD 		%d\n",nMBR);

	fprintf(out,"\n#define NUM_BOO_SIGS		%d\n",nCurSig);

	fprintf(out,"\n#endif // #ifndef _BOODEFS_H\n");

    return(0);
}


int main(int argc, char *argv[])
{
    FILE                            *in, *out;
    int                             nResult;

    if (argc != 3)
    {
        printf("usage: booparse bootheur.dat outfile.cpp\n");
        return(-1);
    }

	in = fopen(argv[1],"rt");
    if (in == NULL)
    {
        printf("error: cannot open input file %s\n",argv[1]);
        return(-1);
    }

    out = fopen(argv[2],"wt");
    if (out == NULL)
    {
        fclose(in);
        printf("error: cannot open output file %s\n",argv[2]);
        return(-1);
    }

	nResult = Compile(in,out);

	fclose(in);
	fclose(out);

	if (nResult == 0)
		printf("Success!\n");

	return(nResult);
}
