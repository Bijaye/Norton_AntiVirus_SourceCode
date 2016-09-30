#pragma once

class CSSPRunnableJob : public StahlSoft::CRunnable
{
public:
	CSSPRunnableJob(void);
	virtual ~CSSPRunnableJob(void);


	STDMETHOD(IsRunning)();
	STDMETHOD(RequestExit)();

protected:
	StahlSoft::CSmartHandle m_hRunning;

};
