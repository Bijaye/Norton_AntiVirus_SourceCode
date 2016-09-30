// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// NewProductCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

int main(int argc, char* argv[])
{
	int retval = 1;
	unsigned char __RPC_FAR * strGUID = 0;
	GUID myGUID;
	if( RPC_S_OK == UuidCreate( &myGUID ) )
	{
		if( RPC_S_OK == UuidToString( &myGUID, &strGUID ) )
		{
			CharUpperBuff( (LPSTR) strGUID, (DWORD) strlen( (LPCSTR) strGUID ) );
			cout << "NewProductCode={";
			cout << strGUID;
			cout << "}" << endl;

			RpcStringFree( &strGUID );

			retval = 0;
		}
	}
	return retval;
}
