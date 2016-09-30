#include "stdafx.h"
#include ".\requeststatusthread.h"
#include "avevents.h"
#include "NAVEventFactoryLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"

CRequestStatusThread::CRequestStatusThread(CRequestStatusThreadSink* pSink)
{
    m_pSink = pSink;

    // We need COM to send out the update events
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
    m_Options.m_bPumpMessages = TRUE;
}

CRequestStatusThread::~CRequestStatusThread(void)
{
}

void CRequestStatusThread::SetData ( CEventData* pedData )
{
    m_pedData = std::auto_ptr <CEventData>(pedData);
}


int CRequestStatusThread::Run (void)
{
    if ( !m_pSink || !m_pedData.get() )
        return -1;

	// Initialize here.
	//
    AV::IAvEventFactoryPtr pProvider;

    if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &pProvider)) ||
        pProvider.m_p == NULL )
    {
        CCTRACEE ( "Could not create IAvFactory object. - %d", ::GetLastError() );
        pProvider = NULL;
    }
    else
    {
        CEventData* pEventReturn = NULL;

        for (;;)
        {
            // If StatusHP isn't running this Send call will return immediately. If StatusHP
            // is up but busy we wait INFINITE until it returns with our request.
            //
            if ( pProvider->SendAvEvent (*(m_pedData.get()), pEventReturn, INFINITE, m_Terminating.GetHandle()))
            {
                if ( pEventReturn )
                {
                    CEventData edNewData;

                    bool bResult = pEventReturn->GetNode (AVStatus::propCurrentStatus, edNewData );

                    delete pEventReturn;
                    pEventReturn = NULL;

                    if (bResult)
                    {
                        CCTRACEI("RequestStatusThread - got status");   
                        m_pSink->OnRequestStatusReceived (edNewData);
                        CCTRACEI("RequestStatusThread - exiting successfully");   
                        return 1;
                    }
                    else
                        CCTRACEE ( "RequestStatusThread - No result" );
                }

                // StatusHP isn't running, start polling. Terminate if we are told to.
                //
                if ( WaitForTerminate(1000))
                {
                    CCTRACEI("RequestStatusThread - terminating");
                    return 0;
                }
            }
        }
    }

    return 0;
}
