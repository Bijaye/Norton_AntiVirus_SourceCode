
#pragma once

#include "apwutil.h"

class CDefAuthenticationAlertQueue : public CSyncQueue< DWORD >
{
public:
	CDefAuthenticationAlertQueue()
    {
        // This will require COM
        // If you make this into CSyncDequeue, remove the mandatory COM here.
        m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
        m_Options.m_bPumpMessages = TRUE;
    }
	virtual ~CDefAuthenticationAlertQueue()
    {
    }

    // Display the alert
	virtual void processQueue()
    {
        CCTRACEI(_T("CDefAuthenticationAlertQueue::processQueue() - processing %d queue items"), GetSize());
        DWORD dwFailure = 0;
        DWORD dwFailurePreviouslyDisplayed = -1;

        // Clear the entire queue in case multiples of the same event come in
        while( getQueueFront( dwFailure ) )
        {
            if( dwFailurePreviouslyDisplayed != dwFailure )
            {
                CCTRACEI(_T("CDefAuthenticationAlertQueue::processQueue() - Recieved a new authentication alert to display. ID = %d"), dwFailure);
                ApwErrorMessageBox(NULL, dwFailure);
            }
            else
                CCTRACEI(_T("CDefAuthenticationAlertQueue::processQueue() - Recieved an identical authentication alert, not displaying it. ID = %d"), dwFailure);

            // Save the error we just displayed
            dwFailurePreviouslyDisplayed = dwFailure;
        }
    }

private:
	// Disallowed
	CDefAuthenticationAlertQueue( CDefAuthenticationAlertQueue& other );
};