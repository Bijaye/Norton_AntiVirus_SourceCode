// TestThread.cpp : implementation file
//
#include "afxwin.h"
#include "afxtempl.h"

#include <iostream>
#include <fstream>
#include <iostream>
#include <ios>
#include <strstream>


#include "dferror.h"
#include "dfevalstatus.h"
#include "dfevalstate.h"

#include "dfjob.h"
#include "dfsample.h"
#include "dfpackthread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFPackThread
UINT CreatePackageAndSignaturFiles( LPVOID pParam );
int IsFileFound(char *fileName );
void AppendPath(char *lpszFirst,char *lpszSec);


IMPLEMENT_DYNCREATE(CDFPackThread, CWinThread)

CDFPackThread::CDFPackThread(DFManager *mgr, DFSample *dfSample)
{
	manager = mgr;
	pSample = dfSample;
	m_bAutoDelete = FALSE;
	m_pThreadParams = this;
	m_pfnThreadProc = CreatePackageAndSignaturFiles;
}

CDFPackThread::~CDFPackThread()
{
	int	a = 1;
}

BOOL CDFPackThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CDFPackThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CDFPackThread, CWinThread)
	//{{AFX_MSG_MAP(CDFPackThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFPackThread message handlers

int CDFPackThread::Run() 
{
	// TODO: Add your specialized code here and/or call the base class
    DWORD bStatus;
	::GetExitCodeThread(m_hThread, &bStatus);

	if (bStatus == STILL_ACTIVE)
		return TRUE;
	else 
		return FALSE;

	return CWinThread::Run();
}
void CDFPackThread::Kill() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	bKill = TRUE;
	WaitForSingleObject(m_hThread, INFINITE); 
}

BOOL CDFPackThread::Start() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	bKill = FALSE;
	return CreateThread(); 
}

BOOL CDFPackThread::Running() 
{
	DWORD bStatus;
   ::GetExitCodeThread(m_hThread, &bStatus);
		if (bStatus == STILL_ACTIVE) 
			return TRUE;
		else 
			return FALSE;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      CreatePackageAndSignaturFiles                         */
/* Description:                                                               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              DF_SUCCESS - success                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
UINT CreatePackageAndSignaturFiles( LPVOID pParam )
{
    char packagePath[MAX_LENGTH];
    char filesPath[MAX_LENGTH];
    char defSeqPath[MAX_LENGTH];
    char newName1[MAX_LENGTH];
    char defSeqNumber[MAX_LENGTH];
    char doneFile[MAX_LENGTH];
    char defBaseDir[MAX_LENGTH];
    char buildDefFilename[MAX_LENGTH];
    CDFPackThread * packThread = (CDFPackThread*) pParam; 
	if (packThread && packThread->bKill)
		return 0;
	strcpy(packagePath, packThread->pSample->buildDefFilename);
	strcpy(buildDefFilename, packThread->pSample->buildDefFilename);

	if (IsFileFound(packagePath)) 
		return 1;

 	char *p = strrchr(packagePath, '.');

	*p = '\0';

	strcpy(filesPath, packagePath);

	AppendPath(filesPath, "\\*.*");
	strcpy(defSeqPath, packThread->pSample->defBaseDir);
	strcpy(defBaseDir, packThread->pSample->defBaseDir);

	strcpy(defSeqNumber, (LPTSTR) (LPCTSTR) packThread->pSample->GetDefSeqNumber());
	AppendPath(defSeqPath, 	defSeqNumber);

	strcpy(doneFile, defSeqPath);
    CreateDirectory(defSeqPath, NULL);
    // copy files
    char temp[MAX_PATH]; // temporary for directory
    char temp1[MAX_PATH]; // temporary for directory
    BOOL rc;              // return code from findfirst
    HANDLE hFile;


    WIN32_FIND_DATA  result; 
    strcpy(temp, packagePath);
    strcat(temp,"\\*.*");
	strcpy(newName1, defSeqPath);

    hFile = FindFirstFile(temp, &result);
    rc = (hFile == INVALID_HANDLE_VALUE) ? FALSE : TRUE;

    while (rc == TRUE) {
		if (strcmp(result.cFileName, ".") != 0 && strcmp(result.cFileName, "..") != 0) { 
		    strcpy(temp1, packagePath);
			AppendPath(temp1,result.cFileName);
			strcpy(newName1, defSeqPath);
			AppendPath(newName1,result.cFileName);
            CopyFile(temp1, newName1, FALSE);
        } 
		rc = FindNextFile(hFile, &result);

    }
	FindClose( hFile );
// copy .exe

	strcpy(newName1, defBaseDir);
	strcat(newName1, "\\");
	strcat(newName1, defSeqNumber);
	CString buildName = CString(buildDefFilename);
	CString  extString =  buildName.Right(4);
	strcat(newName1, (LPTSTR) (LPCTSTR) extString);
 
	if (CopyFile(buildDefFilename, newName1, FALSE))
      packThread->pSample->packageName = newName1;
    else     
      return 0;

	//check the directory
	strcat(doneFile, "\\zdone.dat");

	if (IsFileFound(doneFile)) 
		return TRUE;

    packThread->pSample->packDone = TRUE;
	AfxGetApp()->m_pMainWnd->PostMessage(THREAD_ID, WPARAM(packThread->pSample), 0);
    return 0;
}

int IsFileFound(char *fileName )
{
    LPCTSTR temp = (LPCTSTR) fileName; // temporary for directory
    int rc;         // return code from findfirst
    HANDLE hFindFile;
	WIN32_FIND_DATA result;   
//    strcpy(temp, fileName);
    
    hFindFile = FindFirstFile(temp, &result);
	if (hFindFile == INVALID_HANDLE_VALUE) 
		rc = 1;
	else
		rc = 0;

    FindClose( hFindFile );
          
	return rc;



} 
/*----------------------------------------------------------------------------*/
/* Procedure name:      AppendPath                                            */
/* Description:         To append one path to another one                     */
/*                                                                            */
/* Input:               lpszFirst: Specifies the first path to which second   */ 
/*                                   path is to be appended.                  */
/*                        lpszSecond: Specifies the path to be appenced to    */ 
/*						            lpszFirst                                 */ 
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* Output:              None                                                  */
/* Return:              pszFirst: Containst he resultant path on return.      */
/*                                                                            */
/* Global References:   None                                                  */
/* Notes: This function combines two paths and takes care of trailing         */
/*          slashes in both of them. It ensures that when the two             */ 
/*          paths are appended, there is a single slash separating            */
/*          them. For ex: if first path has a trailing slash and              */
/*          second path has a starting slash, it ensures to remove            */
/*          one of the slashes. Similarly, if first path does not             */
/*          have a trailing slash and second path does not have               */
/*          a starrting slash, a single slash is appended to the first        */
/*          path before appending the second path.                            */ 
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void AppendPath(char *lpszFirst,char *lpszSec)
{
        if(lpszFirst == NULL || lpszSec == NULL) return;

        int nLen1 = strlen(lpszFirst);

        // If last char in First is not a slash and first char
        // in Sec is not a slash, append slash.
                                        
        if(lpszFirst[nLen1-1] != '\\' && lpszSec[0] != '\\')                                                                                           
               strcat(lpszFirst,"\\");
        else if(lpszFirst[nLen1-1] == '\\' && lpszSec[0] == '\\')                                                   
                lpszFirst[nLen1-1] = 0;

        strcat(lpszFirst,lpszSec);
}
   
