#include "fileserv.h"
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SETS  50

void CvtDrive(LPSTR lpszBuffer, LPSTR lpszVol)
{
	LPSTR lpszTemp, lpPoint;
	int nCount;

	lpszTemp = (LPSTR) malloc(255);

	strcpy(lpszTemp, lpszBuffer);

	lpszTemp++;

	strcpy(lpszBuffer, lpszVol);
	strcat(lpszBuffer, lpszTemp);

	lpPoint = lpszBuffer;

	nCount = strlen(lpszBuffer);
/*
	while(nCount > 0)
	{
		if (lpPoint[0] == '\\')
			lpPoint[0] = '/';
		lpPoint++;
		nCount--;
	}
*/
	free(lpszTemp);
}



int FileWriteLine(HANDLE hFile, LPSTR lpszBuffer)
{

	DWORD dwWritten=0, dwWrite=0;

	strcat(lpszBuffer, "\r\n");
	dwWrite = strlen(lpszBuffer);

	if (0 == (WriteFile(hFile, lpszBuffer, dwWrite, &dwWritten, NULL)))
		return -1;

	return dwWritten;
}



int FileReadLine(HANDLE hFile, LPSTR lpszBuffer)
{
	int nCount=0, bDone=0;
	DWORD dwRead;
	LPSTR lpszRead;

	lpszRead = (LPSTR) malloc(5);

	strcpy(lpszBuffer, "");

	while(!bDone)
	{
		if (ReadFile(hFile, lpszRead, 1, &dwRead, NULL) == 0)
		{
			bDone = 1;
//			cout << "EOF";
		}
		else if (0 == dwRead)
		{
			bDone = 1;
//			cout << "EOF";
		}
		else if ('\n' == lpszRead[0])
		{
			bDone = 1;
//			cout << "EOL";
		}
		else
		{
			strncat(lpszBuffer, lpszRead, 1);
			//cout << lpszRead;
			nCount++;
		}
	}

	free(lpszRead);
	return nCount;

}

WORD MakeScript(LPSTR lpszOutFile)
{
	LPSTR lpszVol, lpszInFile, lpszBuffer, lpszDefs, lpszLine,
		lpszHeur, lpszMapped;
	HANDLE hOutFile;
	int bDone=0, nRead=0, nWrite=0, bScan=0, bRepair=0, bReScan=0, nHeur=0;
	char chAns=' ';
	CFileServer *lpFileServer = new CFileServer();


	if ((lpszVol = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszVol.\n";
		return(-1);
	}
	if ((lpszInFile = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszInFile.\n";
		return(-1);
	}
	if ((lpszBuffer = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszBuffer.\n";
		return(-1);
	}
	if ((lpszDefs = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszDefs.\n";
		return(-1);
	}
	if ((lpszLine = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszLine.\n";
		return(-1);
	}
	if ((lpszHeur = (LPSTR) malloc(10)) == NULL)
	{
		cout << "Error allocating memory for lpszHeur.\n";
		return(-1);
	}
	if ((lpszMapped = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszMapped.\n";
		return(-1);
	}

	cout << "Enter NLM volume name (e.g., 'sys'): ";
	cin >> lpszVol;

	cout << "Enter mapped drive path to files (e.g., 'k:\\virus'): ";
	cin >> lpszMapped;

	cout << "Enter the defs path (e.g., 'sys:/stscan/defs'): ";
	cin >> lpszDefs;

	cout << "Heuristics level (0, 1, 2, or 3): ";
	cin >> nHeur;

	cout << "Scan files first (Y/N)? ";

	cin >> chAns;
	if (toupper(chAns) == 'Y')
		bScan=1;
	
	cout << "Repair files (Y/N)? ";
	cin >> chAns;
	if (toupper(chAns) == 'Y')
	{
		bRepair=1;

		cout << "Scan files after repair (Y/N)? ";
		cin >> chAns;
		if (toupper(chAns) == 'Y')
			bReScan=1;
	}


	if ((hOutFile = CreateFile(lpszOutFile, GENERIC_WRITE, 0, 
		NULL, CREATE_ALWAYS, NULL, NULL)) == INVALID_HANDLE_VALUE)
	{
		cout << "Error: Unable to open " << lpszOutFile << " for writing.\n";
		return(-1);
	}

	lpFileServer->ServerInit(lpszMapped);

	strcpy(lpszLine, "a ");
	strcat(lpszLine, lpszDefs);
	sprintf(lpszHeur, " 0 %d 0\0", nHeur);
	strcat(lpszLine, lpszHeur);
	FileWriteLine(hOutFile, lpszLine);

	lpszBuffer = lpFileServer->RequestFile(FALSE);
/*	if (NULL != lpszBuffer)
		cout << "Got: " << lpszBuffer << endl;
	else
		cout << "NULL!\n";
*/
	while (NULL != lpszBuffer)
	{
		CvtDrive(lpszBuffer, lpszVol);
		
		
		if (bScan)
		{
			strcpy(lpszLine, "d ");
			strcat(lpszLine, lpszBuffer);
			FileWriteLine(hOutFile, lpszLine);
			strcpy(lpszLine, "m 0");
			FileWriteLine(hOutFile, lpszLine);
			strcpy(lpszLine, "l");
			FileWriteLine(hOutFile, lpszLine);
		}
		if (bRepair)
		{
			strcpy(lpszLine, "h ");
			strcat(lpszLine, lpszBuffer);
			FileWriteLine(hOutFile, lpszLine);

			if (bReScan)
			{
				strcpy(lpszLine, "d ");
				strcat(lpszLine, lpszBuffer);
				FileWriteLine(hOutFile, lpszLine);
				strcpy(lpszLine, "m 0");
				FileWriteLine(hOutFile, lpszLine);
				strcpy(lpszLine, "l");
				FileWriteLine(hOutFile, lpszLine);
			
			}
		}

		lpszBuffer = lpFileServer->RequestFile(TRUE);
/*		if (NULL != lpszBuffer)
			cout << "Got: " << lpszBuffer << endl;
		else
			cout << "Loop NULL!";
*/
	}


	strcpy(lpszLine, "b");
	FileWriteLine(hOutFile, lpszLine);
	strcpy(lpszLine, "5");
	FileWriteLine(hOutFile, lpszLine);

	CloseHandle(hOutFile);

	free(lpszVol);
	free(lpszInFile);
	free(lpszBuffer);
	free(lpszDefs);
	free(lpszLine);
	free(lpszHeur);
	free(lpszMapped);
	return(0);
}

WORD MakeSets(LPSTR lpszOutFile, WORD wSets)
{
	LPSTR lpszFiles[MAX_SETS], lpszVol, lpszBuffer, lpszMapped;
	WORD wCount=0, wCurrent=0;
	HANDLE hFiles[MAX_SETS];
	CFileServer *lpFileServer = new CFileServer();

	for (wCount=0; wCount<wSets; wCount++)
	{
		lpszFiles[wCount] = (LPSTR)malloc(strlen(lpszOutFile)+5);
		sprintf(lpszFiles[wCount], "%s%d", lpszOutFile, wCount+1);

		if ((hFiles[wCount] = CreateFile(lpszFiles[wCount], GENERIC_WRITE, 0, 
			NULL, CREATE_ALWAYS, NULL, NULL)) == INVALID_HANDLE_VALUE)
		{
			cout << "Error: Unable to open " << lpszOutFile << " for writing.\n";
			return(-1);
		}
	}

	if ((lpszVol = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszVol.\n";
		return(-1);
	}
	if ((lpszBuffer = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszBuffer.\n";
		return(-1);
	}
	if ((lpszMapped = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszMapped.\n";
		return(-1);
	}

	cout << "Enter NLM volume name (e.g., 'sys'): ";
	cin >> lpszVol;

	cout << "Enter mapped drive path to files (e.g., 'k:\\virus'): ";
	cin >> lpszMapped;

	lpFileServer->ServerInit(lpszMapped);

	lpszBuffer = lpFileServer->RequestFile(FALSE);

	wCount = 0;

	while (NULL != lpszBuffer)
	{
		CvtDrive(lpszBuffer, lpszVol);

		wCurrent = wCount % wSets;

		FileWriteLine(hFiles[wCurrent], lpszBuffer);
		
		wCount++;
		lpszBuffer = lpFileServer->RequestFile(TRUE);
	}

	for (wCount=0; wCount<wSets; wCount++)
	{
		free(lpszFiles[wCount]);
		CloseHandle(hFiles[wCount]);
	}

	return(0);

}

void Menu(void)
{
	cout << "STScScr v1.1   StScan Companion Tool for NLM                           09/02/98\n\n";
	cout << "For script generation:  stscscr OUTPUTFILE\n\n";
	cout << "For file set generation:  stscscr BASENAME NUMBER_OF_SETS\n";
	cout << "                             where BASENAME is the name used to generate\n";
	cout << "                             filenames ending in 1..NUMBER_OF_SETS.\n";
	cout << "                             NUMBER_OF_SETS should be between 0 and 50.\n\n";
	cout << "Program function is based upon the number of commandline arguments.\n\n";
}

void main(int argc, char *argv[])
{
	LPSTR lpszOutFile;
	WORD wSets=0;


	if (argc < 2)
	{
		Menu();
		exit(1);
	}

	if ((lpszOutFile = (LPSTR) malloc(255)) == NULL)
	{
		cout << "Error allocating memory for lpszOutFile.\n";
		exit(1);
	}

	if (3 == argc)
	{
		strcpy(lpszOutFile, argv[1]);
		wSets = atoi(argv[2]);
		MakeSets(lpszOutFile, wSets);
	}
	else if (2 == argc)
	{
		strcpy(lpszOutFile, argv[1]);
		MakeScript(lpszOutFile);
	}
	else
	{
		free(lpszOutFile);
		Menu();
		exit(1);
	}
	
	free(lpszOutFile);
}


		
