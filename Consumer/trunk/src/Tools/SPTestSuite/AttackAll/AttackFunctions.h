////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

class CAttackFunctions
{
public:

	bool OpenSymFile();
	bool ModifyMutex();
	bool ModifyEvent();

	CAttackFunctions(void);
	~CAttackFunctions(void);
};
