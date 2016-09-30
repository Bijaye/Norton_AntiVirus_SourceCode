#ifndef GLOBAL_H
#define GLOBAL_H

extern HINSTANCE g_hInstance;

enum MODULE_IDs
{
	CHKNAVFILES_ERR_START = 0
};

void ReportFatalError(UINT uMessageID);
bool IsNAVFileRemoved();

#endif