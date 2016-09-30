// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// savaudit_Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <conio.h>

#include "ScsAudit.h"

int main(int argc, char* argv[])
{
	DWORD dwRet;

	DESIRED_STATE DesiredState;

	PACTUAL_STATE pActualState = NULL;

	// set up the desired state

    DesiredState.dwExpectedOnOff = 1;    // A/P enabled
    DesiredState.dwExpectedWrites= 1;    // scan on modify
    DesiredState.dwExpectedExecs = 1;    // scan on exec
    DesiredState.dwExpectedReads = 1;    // scan on open
    DesiredState.dwExpectedFileType = 0;    // scan all extensions
    DesiredState.dwExpectedHeuristics = 1;    // heuristic engine enabled
    DesiredState.dwMinHeuristicsLevel = 2;    // default level

    DesiredState.nMinPattVerYear = 2002;
    DesiredState.nMinPattVerMonth= 06;
    DesiredState.nMinPattVerDay  = 25;
    DesiredState.nMinPattVerRev  = 1;

    DesiredState.nMaxPattVerYear = 2002;
    DesiredState.nMaxPattVerMonth= 07;
    DesiredState.nMaxPattVerDay  = 22;
    DesiredState.nMaxPattVerRev  = 1;

    DesiredState.dwMinEngineVersion = 0x04010006;
    DesiredState.dwMaxEngineVersion = 0x04010103;

    DesiredState.nMinProductVersion = 7;
    DesiredState.nMinProductSubver = 50;
    DesiredState.nMinProductBuild = 948;

    DesiredState.nMaxProductVersion = 7;
    DesiredState.nMaxProductSubver = 61;
    DesiredState.nMaxProductBuild = 926;
	
	DesiredState.bAuditSymantecClientFirewall = TRUE;

	BOOL bRet = AuditSCS( &DesiredState, &pActualState, &dwRet );

	// you can look at the actual state as returned

	if ( bRet )
	{
		printf( "audit succeeded\n" );
	}
	else
	{
		printf( "audit failed, return code %x\n", dwRet );
	}

	FreeActualState( pActualState );

	printf( "hit any key to exit!\n" );
	getch( );

	return ! bRet;
}