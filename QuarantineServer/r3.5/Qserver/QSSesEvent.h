/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QSSesEvent.h

#ifndef QSSESEVENT_h
#define QSSESEVENT_h


//include Naboo headers
//#include "events.h"
//#include "appids.h"
#include "applib.h"
#include "SESBase_enum.h"
#include "SESBase_ep_Ids.h"
#include "CentralQuarantine_ep_ids.h"
//include our evnets
//#include "CQEvents.h"
#include "aviseventdetection.h"
#define DEFAULT_IP          "127.0.0.1"     //IP on which the agent is running, loopback IP
#define DEFAULT_PORT        0            //Port on which the agent is listening


typedef struct tagGENEVENTTABLE
{
	int iCQEventID;
	int iSESEventID;
} GENEVENTTABLE;



class CEvent;

class QSSesInterface : public SESInterface
{
public:
    // constructor - initialize the applib (using SESInterface::clientInit()) and
    // construct an AppStart event.
    QSSesInterface(char *lpstrAgentIP, int nAgentPort)
    {
        _ASSERT(initialized == 0);

        if (S_OK == clientInit(SESPRODUCTID_SYMANTEC_CENTRAL_QUARANTINE,       //appid this is sample
								SESSWFID_SYMANTEC_CENTRAL_QUARANTINE___QUARANTINE_SERVER ,	// featue id (only one for cq)
								QSVERSION_STRING,
                                lpstrAgentIP,       //ip on which the agent
                                nAgentPort))        //port on which the agent
        {
            initialized = 1;
//            AppStart();                             //indicate that the app has started
        }
    }

	virtual ~QSSesInterface() {shutdown();}
    // shutdown() sends an AppStop event and de-initializes the applib
    void shutdown()
    {
//        _ASSERT(initialized == 1);
//        AppStop();                              //indicate that the app has stopped
        if(initialized)
			clientShutdown();
        initialized = 0;
    }

private:
    static int initialized;
};

class QSSesGenEvent
{

public:
	QSSesGenEvent (QSSesInterface* pSesInterface)
	{
		m_pQSEvent = NULL;
//		m_pSesEvent = NULL;
		m_pQSSesInterface = pSesInterface;
		m_iSESEventID = 0;

	}

	~QSSesGenEvent (){};

	void Init (CAlertEvent * pQSEvent);
	HRESULT SendQSSESEvent(CAlertEvent *pQSEvent);
	

protected:
	CAlertEvent *m_pQSEvent;
	QSSesInterface* m_pQSSesInterface;
    SESEvent m_SesEvent;
	int m_iSESEventID;
	SESSeverity m_nSeverity;
	CString m_sEventClass;


	HRESULT CreateSesEvent(void);

	int LookupSESEventID (int QSEventID, GENEVENTTABLE * pIdTable, int iNoItems);

	int LookupSESSeverity (int QSSeverity);

	HRESULT SendQSSESEvent(void);


	HRESULT SetGenEventFields(void);
	HRESULT SetEventAttrib(void);
	
	
};



class QSSesSampleEvent : public QSSesGenEvent
{

public: 
	QSSesSampleEvent (QSSesInterface* pSesInterface): QSSesGenEvent(pSesInterface){};

	HRESULT CreateSesEvent(void);

	~QSSesSampleEvent (){};

	virtual HRESULT SendQSSESEvent(CAlertEvent *pQSEvent);
	HRESULT SendQSSESEvent(CAlertEvent *pQSEvent,int iSESEventID);

private:
	virtual HRESULT SetGenEventFields(void);
	virtual HRESULT SetEventAttrib(void);

};
#endif  // QSSESEVENT_H
