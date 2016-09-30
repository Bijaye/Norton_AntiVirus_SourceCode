#ifndef __INTERNETEXCEPTION__H__
#define __INTERNETEXCEPTION__H__

class InternetException
{
public:
	InternetException(DWORD dwError);

// Attributes
	DWORD m_dwError;
	DWORD m_dwContext;

	virtual BOOL GetErrorMessage(LPTSTR lpstrError, UINT nMaxError,	PUINT pnHelpContext = NULL);
};


void ThrowInternetException(DWORD dwContext, DWORD dwError = 0);


class MemoryException
{
public:
	MemoryException();
};

void ThrowMemoryException();

#endif __INTERNETEXCEPTION__H__
