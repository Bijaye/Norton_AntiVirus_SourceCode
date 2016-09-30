// NAVOptionsReport.cpp: implementation of the CNAVOptionsReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "NAVOptionsReport.h"
#include "NAVInfo.h"
#include "FileIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVOptionsReport::CNAVOptionsReport()
{
    CNAVInfo NAVInfo;
    if ( NAVInfo.GetNAVDir ()[0] != '\0' )
        m_bIsAvailable = true;
}

CNAVOptionsReport::~CNAVOptionsReport()
{

}


bool CNAVOptionsReport::CreateReport ( LPCTSTR lpcszFolderPath )
{
    CFileIO FileIO;
    CNAVInfo NAVInfo;
    return FileIO.CopyFiles ( NAVInfo.GetNAVDir (), _T("*.dat"), lpcszFolderPath, _T("FILTER.DAT"));
}
