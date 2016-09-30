////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TestExclu.cpp : Defines the entry point for the console application.
//
/*
#define _WIN32_DCOM
#ifdef WIN32
    #pragma warning ( disable : 4250 )
    #pragma warning( disable : 4290 )
#endif

#define INITIIDS
#include "ccLibStd.h"
*/
#include "stdafx.h"
#include "TestEngine.h"
#include "ccString.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 2)
	{
		TestEngine te;
	
		CString strInputFile = argv[1] ;

		if (!te.ReadFromFile(strInputFile) )
		{
			return 0;
		}

		te.Process();

		return 0;

	}else {
	
	
		TestEngine te;
		TCHAR cIn[MAX_PATH];



	
		while(1)
		{
			fflush(stdin);
			fprintf(stdout, _T(">TestExclu>>"));
			scanf("\n%[^\n]", cIn);
			CString strIn(cIn);

			if(!strIn.Compare("help"))
			{
				printf("Usages: \n");
				printf("------------------------------------------------------------------------\n");
				printf("help			Display this menu \n");
				printf("automate		Execute a given input script \n");
				printf("addfs			Add a file system exclusion \n");
				printf("adda			Add an anomaly exclusion \n");
				printf("findfs			Find a file system exclusion \n");
				printf("finda			Find an anomaly exclusion \n");
				printf("get			Get an exclusion using id \n");
				printf("isfa			Is file system exculsion? state = 0 if not \n");
				printf("isa			Is anomaly exclusion? state = 0 if not \n");
				printf("list			List all exclusion items \n");
				printf("remove			Removes a exculsion by id \n");
				printf("replace			Replace (edit) an existing exclusion \n");
				printf("reload			Reloads states from ccSettings \n");
				printf("save			Save state to ccSettings \n");
				printf("stateflag		Set exclusion state by turn each bit ON or OFF \n");
				printf("tofile			Save exclusion settings into a file \n");
				printf("fromfile		Load exclusion settings from a file \n");
				printf("default			Load default setting to manager \n");
				printf("setauto+		Set AutoCommit on \n");
				printf("setauto-		Set AutoCommit off \n");
				printf("isauto			Check the state of AutoCommit \n");
				printf("viewsettings		View stored exclusions in ccSetttings \n");
				printf("exit			Exit program \n");
				printf("------------------------------------------------------------------------\n");
			}
			else if (!strIn.Compare(_T("addfs")))
			{
				TCHAR strPath [32768];
				time_t tTime;
				int iState;

				fflush(stdin);	
				fprintf(stdout, "Please enter exclusion dir path: ");
				scanf("\n%[^\n]", strPath);

				fflush(stdin);
				fprintf(stdout, "Please enter exculsion expiry time: ");
				scanf("\n%ul", &tTime);

				fprintf(stdout, "	0 = NOT_EXCLUDED (Can not add item with only this state) \n");
				fprintf(stdout, "	1 = EXCLUDE_VIRAL \n");
				fprintf(stdout, "	2 = EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	3 = EXCLUDE_BOTH \n");
				fprintf(stdout, "	4 = EXCLUDE_SUBDIRS_VIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	5 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_VIRAL \n" );
				fprintf(stdout, "	6 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	7 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	8 = EXCLUDE_SUBDIRS_NONVIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	9 = EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	10= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_NONVIRAL \n") ;
				fprintf(stdout, "	11= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	12= EXCLUDE_INCLUDE_SUBDIRS (Can not add item with only this state) \n" );
				fprintf(stdout, "	13=	EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	14= EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	15= EXCLUDE_ALL \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion state: ");
				scanf("\n%d", &iState);

				te.Addfs(strPath,tTime,iState, false);
			}
			else if (!strIn.Compare(_T("adda")))
			{	
				time_t tTime;
				ULONG ulVid = NULL;
				int iState;
	
				TCHAR cGuid[MAX_PATH] = "";
				TCHAR cName[MAX_PATH] = "";

				fflush(stdin);
				fprintf(stdout, "Please enter (Virus Id) Vid: ");
				scanf("\n%u", &ulVid);

				fflush(stdin);
				fprintf(stdout, "Please enter (Anomaly GUID) Guid: ");
				scanf("%[^\n]", cGuid);

				fflush(stdin);
				fprintf(stdout, "Please enter exculsion expiry time: ");
				scanf("\n%ul", &tTime);

				fflush(stdin);
				fprintf(stdout, "Please enter display name: ");
				scanf("%[^\n]",cName);

				fprintf(stdout, "	0 = NOT_EXCLUDED \n");
				fprintf(stdout, "	1 = EXCLUDE_VIRAL \n");
				fprintf(stdout, "	2 = EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	3 = EXCLUDE_BOTH \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion state: ");
				scanf("\n%d", &iState);

				te.Adda(ulVid, cGuid, tTime, cName, iState);
		
			}
			else if (!strIn.Compare(_T("findfs")))
			{
				TCHAR strPath [32768];

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion dir path: ");
				scanf("%[^\n]", strPath);
			
				te.Findfs(strPath,false);
			}
			else if (!strIn.Compare(_T("finda")))
			{
				ULONG ulVid;
				TCHAR cGuid[MAX_PATH] = "";

				fflush(stdin);
				fprintf(stdout, "Please enter (Virus Id) Vid: ");
				scanf("%u", &ulVid);
			
				fflush(stdin);
				fprintf(stdout, "Please enter (Anomaly GUID) Guid: ");
				scanf("%[^\n]",cGuid);

				te.Finda(ulVid,cGuid);
			}
			else if (!strIn.Compare(_T("get")))
			{
				ULONG iTempId = 0;

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion Id: ");
				scanf("%u", &iTempId);

				te.Get(iTempId);

			}
			else if (!strIn.Compare(_T("isfs")))
			{
				TCHAR strPath [32768];
				int iState;


				fflush(stdin);
				fprintf(stdout, "Please enter exclusion dir path: ");
				scanf("\n%[^\n]", strPath);

				fprintf(stdout, "	0 = NOT_EXCLUDED (Can not add item with only this state) \n");
				fprintf(stdout, "	1 = EXCLUDE_VIRAL \n");
				fprintf(stdout, "	2 = EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	3 = EXCLUDE_BOTH \n");
				fprintf(stdout, "	4 = EXCLUDE_SUBDIRS_VIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	5 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_VIRAL \n" );
				fprintf(stdout, "	6 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	7 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	8 = EXCLUDE_SUBDIRS_NONVIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	9 = EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	10= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_NONVIRAL \n") ;
				fprintf(stdout, "	11= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	12= EXCLUDE_INCLUDE_SUBDIRS (Can not add item with only this state) \n" );
				fprintf(stdout, "	13=	EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	14= EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	15= EXCLUDE_ALL \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion state: ");
				scanf("\n%d", &iState);

				te.Isfs(strPath, false, iState);

			}
			else if (!strIn.Compare(_T("isa")))
			{
				int iState;
				ULONG ulVid;
				TCHAR cGuid[MAX_PATH] = "";

				fflush(stdin);
				fprintf(stdout, "Please enter (Virus Id) ulVid: ");
				scanf("%u", &ulVid);
			
				fflush(stdin);
				fprintf(stdout, "Please enter (Anomaly GUID)pstrGuid: ");
				scanf("%[^\n]",cGuid);

				fprintf(stdout, "	0 = NOT_EXCLUDED \n");
				fprintf(stdout, "	1 = EXCLUDE_VIRAL \n");
				fprintf(stdout, "	2 = EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	3 = EXCLUDE_BOTH \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion state: ");
				scanf("\n%d", &iState);

				te.Isa(ulVid, cGuid, iState);
			}
			else if (!strIn.Compare(_T("remove")))
			{	
				ULONG iTempId;
				fflush(stdin);
				fprintf(stdout, "Please enter Exclusion id: ");
				scanf("%u", &iTempId);
	
				te.Remove(iTempId);
			}
			else if (!strIn.Compare(_T("list")))
			{
				te.List();
			}
			else if (!strIn.Compare(_T("replace")))
			{
				ULONG iTempId;

				ULONG ulVid = NULL;
				//IExclusion::ExclusionType type;

				TCHAR strPath[32768];
				TCHAR cGuid[MAX_PATH] = "";
				TCHAR cName[MAX_PATH] = "";
			
				time_t tTime;
				int iState;

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion id: ");
				scanf("%u", &iTempId);

				if (te.GetType(iTempId) == 1 ) //replacing file system exclusion
				{

					fprintf(stdout, "Replacing File System Exclusion \n");
					fflush(stdin);	
					fprintf(stdout, "Please enter exclusion dir path: ");
					scanf("\n%[^\n]", strPath);

					fflush(stdin);
					fprintf(stdout, "Please enter exculsion expiry time: ");
					scanf("\n%ul", &tTime);

				//if (tTime == 0)
				//	tNowTime = 0;

					fprintf(stdout, "	0 = NOT_EXCLUDED (Can not add item with only this state) \n");
					fprintf(stdout, "	1 = EXCLUDE_VIRAL \n");
					fprintf(stdout, "	2 = EXCLUDE_NONVIRAL \n");
					fprintf(stdout, "	3 = EXCLUDE_BOTH \n");
					fprintf(stdout, "	4 = EXCLUDE_SUBDIRS_VIRAL (Can not add item with only this state) \n");
					fprintf(stdout, "	5 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_VIRAL \n" );
					fprintf(stdout, "	6 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_NONVIRAL \n");
					fprintf(stdout, "	7 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_BOTH \n" );
					fprintf(stdout, "	8 = EXCLUDE_SUBDIRS_NONVIRAL (Can not add item with only this state) \n");
					fprintf(stdout, "	9 = EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_VIRAL \n");
					fprintf(stdout, "	10= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_NONVIRAL \n") ;
					fprintf(stdout, "	11= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_BOTH \n" );
					fprintf(stdout, "	12= EXCLUDE_INCLUDE_SUBDIRS (Can not add item with only this state) \n" );
					fprintf(stdout, "	13=	EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_VIRAL \n");
					fprintf(stdout, "	14= EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_NONVIRAL \n");
					fprintf(stdout, "	15= EXCLUDE_ALL \n");

					fflush(stdin);
					fprintf(stdout, "Please enter exclusion state: ");
					scanf("\n%d", &iState);
			
					te.ReplaceFS(iTempId, strPath, tTime, iState);
				
				}else if (te.GetType(iTempId) == 2 )//replacing anomaly exclusion
				{
					fprintf(stdout, "Replacing Anomaly Exclusion \n");

					fflush(stdin);
					fprintf(stdout, "Please enter (Virus Id) ulVid: ");
					scanf("\n%u", &ulVid);

					fflush(stdin);
					fprintf(stdout, "Please enter (Anomaly GUID) pstrGuid: ");
					scanf("%[^\n]",cGuid);

					fflush(stdin);
					fprintf(stdout, "Please enter exculsion expiry time: ");
					scanf("\n%ul", &tTime);

					fflush(stdin);
					fprintf(stdout, "Please enter display name: ");
					scanf("\n%[^\n]",cName);

					fprintf(stdout, "0 = NOT_EXCLUDED \n");
					fprintf(stdout, "1 = EXCLUDE_VIRAL \n");
					fprintf(stdout, "2 = EXCLUDE_NONVIRAL \n");
					fprintf(stdout, "3 = EXCLUDE_BOTH \n");
					
					fflush(stdin);
					fprintf(stdout, "Please enter exclusion state: ");
					scanf("\n%d", &iState);
			
					te.ReplaceA(iTempId, ulVid, cGuid, tTime, cName, iState);
				}
			}
			else if (!strIn.Compare(_T("reload")))
			{
				te.Reload();
			}
			else if (!strIn.Compare(_T("save")))
			{
				te.Save();
			}
			else if (!strIn.Compare(_T("tofile")))
			{
				TCHAR cFilePath [MAX_PATH] = "";
			
				fflush(stdin);
				fprintf(stdout, "Please enter file full path: ");
				scanf("%[^\n]", cFilePath );
	
				te.ToFile(cFilePath);

			}
			else if (!strIn.Compare(_T("fromfile")))
			{
				TCHAR cFilePath [MAX_PATH] = "";
				int iState;
				int iType;

				fflush(stdin);
				fprintf(stdout, "Please enter file full path: ");
				scanf("%[^\n]", cFilePath );

				fprintf(stdout, "	0 = NOT_EXCLUDED (Can not add item with only this state) \n");
				fprintf(stdout, "	1 = EXCLUDE_VIRAL \n");
				fprintf(stdout, "	2 = EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	3 = EXCLUDE_BOTH \n");
				fprintf(stdout, "	4 = EXCLUDE_SUBDIRS_VIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	5 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_VIRAL \n" );
				fprintf(stdout, "	6 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	7 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	8 = EXCLUDE_SUBDIRS_NONVIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	9 = EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	10= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_NONVIRAL \n") ;
				fprintf(stdout, "	11= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	12= EXCLUDE_INCLUDE_SUBDIRS (Can not add item with only this state) \n" );
				fprintf(stdout, "	13=	EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	14= EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	15= EXCLUDE_ALL \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion state: ");
				scanf("\n%d", &iState);

				fprintf(stdout, "0 = INVALID_TYPE \n");
				fprintf(stdout, "1 = FILESYSTEM \n");
				fprintf(stdout, "2 = ANOMALY \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion type: ");
				scanf("\n%d", &iType);

				te.FromFile(cFilePath, iState, iType);

			}
			else if (!strIn.Compare(_T("setauto+")))
			{
				te.SetAuto(true);
			}
			else if (!strIn.Compare(_T("setauto-")))
			{
				te.SetAuto(false);
			}
			else if (!strIn.Compare(_T("isauto")))
			{
				te.IsAuto();	
			}
			else if (!strIn.Compare(_T("stateflag")))
			{
				ULONG iTempId = 0;
				int iState;
				int iFlag;
				bool bFlag;

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion id: ");
				scanf("%u", &iTempId);

				fprintf(stdout, "	0 = NOT_EXCLUDED (Can not add item with only this state) \n");
				fprintf(stdout, "	1 = EXCLUDE_VIRAL \n");
				fprintf(stdout, "	2 = EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	3 = EXCLUDE_BOTH \n");
				fprintf(stdout, "	4 = EXCLUDE_SUBDIRS_VIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	5 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_VIRAL \n" );
				fprintf(stdout, "	6 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	7 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	8 = EXCLUDE_SUBDIRS_NONVIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	9 = EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	10= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_NONVIRAL \n") ;
				fprintf(stdout, "	11= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	12= EXCLUDE_INCLUDE_SUBDIRS (Can not add item with only this state) \n" );
				fprintf(stdout, "	13=	EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	14= EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	15= EXCLUDE_ALL \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion state: ");
				scanf("\n%d", &iState);

				fflush(stdin);
				fprintf(stdout, "Please enter 0 = false or 1 = true: ");
				scanf("%d", &iFlag);

				if(iFlag == 1){
			
					bFlag = true;
				}else if (iFlag == 0)
				{
					bFlag = false;
				}else
				{
					fprintf(stderr, "bad flag input\n");
					break;
				}

				te.SetStateFlag(iTempId, iState, bFlag);
			}
			else if (!strIn.Compare(_T("default")))
			{
				int iState;
				int iType;

				fprintf(stdout, "	0 = NOT_EXCLUDED (Can not add item with only this state) \n");
				fprintf(stdout, "	1 = EXCLUDE_VIRAL \n");
				fprintf(stdout, "	2 = EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	3 = EXCLUDE_BOTH \n");
				fprintf(stdout, "	4 = EXCLUDE_SUBDIRS_VIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	5 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_VIRAL \n" );
				fprintf(stdout, "	6 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	7 = EXCLUDE_SUBDIRS_VIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	8 = EXCLUDE_SUBDIRS_NONVIRAL (Can not add item with only this state) \n");
				fprintf(stdout, "	9 = EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	10= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_NONVIRAL \n") ;
				fprintf(stdout, "	11= EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_BOTH \n" );
				fprintf(stdout, "	12= EXCLUDE_INCLUDE_SUBDIRS (Can not add item with only this state) \n" );
				fprintf(stdout, "	13=	EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_VIRAL \n");
				fprintf(stdout, "	14= EXCLUDE_INCLUDE_SUBDIRS | EXCLUDE_NONVIRAL \n");
				fprintf(stdout, "	15= EXCLUDE_ALL \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion state: ");
				scanf("\n%d", &iState);

				fprintf(stdout, "0 = INVALID_TYPE \n");
				fprintf(stdout, "1 = FILESYSTEM \n");
				fprintf(stdout, "2 = ANOMALY \n");

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion type: ");
				scanf("\n%d", &iType);

				te.Default(iState, iType);

			}
			else if (!strIn.Compare(_T("viewsettings")))
			{
				te.ViewSettings();
			}
			else if (!strIn.Compare(_T("automate")))
			{
				TCHAR cFilePath [MAX_PATH] = "";

				fflush(stdin);
				fprintf(stdout, "Please enter exclusion dir path: ");
				scanf("\n%[^\n]", cFilePath);

				if (!te.ReadFromFile(cFilePath) )
				{
					return 0;
				}
				te.Process();
			}
			else if (!strIn.Compare(_T("exit")) || !strIn.Compare(_T("quit")) )
			{
				break;
			}
			else
			{
				fprintf(stdout, "Bad command! Type help for assistance! \n");
			}
		}
	}
	
}

