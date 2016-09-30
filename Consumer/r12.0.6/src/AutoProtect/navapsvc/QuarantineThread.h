// QuarantineThread.h: interface for the CQuarantineThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUARANTINETHREAD_H__F88239EA_0394_454D_90A0_95E3991D2E61__INCLUDED_)
#define AFX_QUARANTINETHREAD_H__F88239EA_0394_454D_90A0_95E3991D2E61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQuarantineThread : public ccLib::CThread  
{
public:
	CQuarantineThread();
	virtual ~CQuarantineThread();

	void Signal();
	int Run();

protected:
	bool itemsToProcess();
    void processItems ();

private:
	// Handle to incomming event.
    ccLib::CEvent	m_eventIncoming;

	// Location of incomming quarantine items
	std::string     m_sIncomingDir;
};

#endif // !defined(AFX_QUARANTINETHREAD_H__F88239EA_0394_454D_90A0_95E3991D2E61__INCLUDED_)
