// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//PasswordManager.h

#ifndef SCSPASSMAN_PASSWORDMANAGER_H
#define SCSPASSMAN_PASSWORDMANAGER_H

#include "nsctop_i.c"
#include "nsctop.h"

#include <string>

class PasswordManager
{
public:
	static PasswordManager* getInstance();
	static void destroy();
	BOOL unlockServerGroup( LPCTSTR serverGroup, LPCTSTR password, LPCTSTR userName);
	BOOL changeServerGroupPassword( LPCTSTR serverGroup, LPCTSTR userName, LPCTSTR oldPassword, LPCTSTR newPassword );

protected:
	//structors
	PasswordManager();
	~PasswordManager();

private:
	static PasswordManager* m_instance;
	IPasswordBroker* m_pPasswordBroker;
};

#endif //SCSPASSMAN_PASSWORDMANAGER_H