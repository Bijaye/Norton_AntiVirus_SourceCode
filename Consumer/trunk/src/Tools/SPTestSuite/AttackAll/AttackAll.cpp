////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AttackAll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AttackFunctions.h"

int _tmain(int argc, _TCHAR* argv[])
{

	CAttackFunctions at;

	at.OpenSymFile();
	at.ModifyMutex();
	at.ModifyEvent();

	return 0;
}

