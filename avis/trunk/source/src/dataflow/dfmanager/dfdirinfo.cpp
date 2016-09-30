// DFDirInfo.cpp: implementation of the DFDirInfo class.
//
//////////////////////////////////////////////////////////////////////
#include "afxtempl.h"
#include "afxwin.h"
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
//#include <string.h>

#include "DFDirInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DFDirInfo::DFDirInfo(CString path, CTime time):
   dirPath(path),
   dirTime(time)
{

}

DFDirInfo::~DFDirInfo()
{

}
