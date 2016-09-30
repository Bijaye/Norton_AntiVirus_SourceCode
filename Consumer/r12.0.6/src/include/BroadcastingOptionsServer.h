// BroadcastingOptionsServer.h

#ifndef __BroadcastingOptionsServer_h__
#define __BroadcastingOptionsServer_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OptionsServer.h"

// This class is exported from the OptionsServer.dll
class OPTIONSSERVER_API CBroadcastingOptionsServer : public COptionsServer {
public:
	CBroadcastingOptionsServer(void);
	virtual ~CBroadcastingOptionsServer(void);

	virtual O_RESULT Save(char const FAR* pszcFileName = NULL);
	virtual O_RESULT Save(char const FAR* pszcFileName, char const FAR* pszcMessage);
};

#endif __BroadcastingOptionsServer_h__
