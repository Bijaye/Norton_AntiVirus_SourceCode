// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AVNotify.h"


class CScanCouldntShowNotify : public CAVNotify
{
public:
	CScanCouldntShowNotify();

    virtual BOOL Start();

protected:
	virtual int Run();
};
