// MyUtils.cpp: implementation of the MyUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MyUtils.h"
#include <stdlib.h>
#include <time.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyUtils::CMyUtils()
{

}

CMyUtils::~CMyUtils()
{

}

void CMyUtils::SeedRand() const
{
    static BOOL bRandSeeded = FALSE;

    if (!bRandSeeded)
    {
        // Seed the random-number generator with current time so that
        // the numbers will be different every time we run.
        srand ((unsigned)time(NULL));

        bRandSeeded = TRUE;
    }
}

int CMyUtils::GetRandomNum() const
{
    SeedRand ();
    return rand ();
}
