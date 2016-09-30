// Copyright 1997 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/ALERTSVC/VCS/rcvstart.cpv   1.1   08 May 1997 18:17:12   GDZIECI  $ 
// 
// Description:
//   Implements functions to start and stop listening for ATP, IPX and IP
// alerts.
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/ALERTSVC/VCS/rcvstart.cpv  $ 
// 
//    Rev 1.1   08 May 1997 18:17:12   GDZIECI
// Implemented StartAlertReceivers() and StopAlertReceivers().
// 
//    Rev 1.0   07 May 1997 15:26:54   GDZIECI
// Initial revision.
//***************************************************************************

#include "platform.h"

#include <lmcons.h>         // Required for alertsvc.h

#include "linksrv.h"        // Required for alertsvc.h
#include "alertsvc.h"
#include "alertstr.h"
#include "alrtrecv.h"


//***************************************************************************
// CAlertService::StartAlertReceivers()
// 
// Description:
//   Starts listening for ATP, IPX and IP alerts (if enabled).
// 
// Parameters:
//   None
// 
// Return Value: 
//   None
// 
// See Also: 
// 
//***************************************************************************
// 05/07/1997 GDZIECI Function Created.                                 
//***************************************************************************

VOID CAlertService::StartAlertReceivers()
{
    CAlertOptions alertOptions;

    alertOptions.LoadOptions();

    if (!m_pATPReceiver && alertOptions.GetATPAlertsEnabled())
    {
        m_pATPReceiver = new ATPReceiver();

        if (m_pATPReceiver)
        {
            if (m_pATPReceiver->init())
            {
                if (!m_pATPReceiver->run())
                    ErrorHandler ( ID_ERR_ATP_THREAD_START );
            }
            else
                ErrorHandler ( ID_ERR_NBP_REGISTRATION );
        }
    }

    if (!m_pIPXReceiver && alertOptions.GetIPXAlertsEnabled())
    {
        m_pIPXReceiver = new IPXReceiver();

        if (m_pIPXReceiver)
        {
            if (m_pIPXReceiver->init())
            {
                if (!m_pIPXReceiver->run())
                    ErrorHandler ( ID_ERR_IPX_THREAD_START );
            }
            else
                ErrorHandler ( ID_ERR_SAP_START );
        }
    }

    if (!m_pIPReceiver && alertOptions.GetIPAlertsEnabled())
    {
        m_pIPReceiver = new IPReceiver();

        if (m_pIPReceiver)
        {
            if (m_pIPReceiver->init())
            {
//                if (!m_pIPReceiver->run())
//                    ErrorHandler (  );
            }
//            else
//                ErrorHandler (  );
        }
    }
}


//***************************************************************************
// CAlertService::StopAlertReceivers()
// 
// Description:
//   Stops listening for ATP, IPX and IP alerts.
// 
// Parameters:
//   None
// 
// Return Value: 
//   None
// 
// See Also: 
// 
//***************************************************************************
// 05/07/1997 GDZIECI Function Created.                                 
//***************************************************************************

VOID CAlertService::StopAlertReceivers()
{
    if (m_pATPReceiver)
    {
        // the destructor stops the listening thread,
        // unregisters our service and cleans up
        delete m_pATPReceiver;
        m_pATPReceiver = NULL;
    }

    if (m_pIPXReceiver)
    {
        // the destructor stops the listening thread,
        // unregisters our service and cleans up
        delete m_pIPXReceiver;
        m_pIPXReceiver = NULL;
    }

    if (m_pIPReceiver)
    {
        // the destructor stops the listening thread,
        // unregisters our service and cleans up
        delete m_pIPReceiver;
        m_pIPReceiver = NULL;
    }
}
