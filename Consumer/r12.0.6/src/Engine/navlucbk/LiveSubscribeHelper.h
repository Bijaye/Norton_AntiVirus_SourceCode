// LiveSubscribeHelper.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_LIVESUBSCRIBEHELPER_H__B2C93EC9_D4EA_4f82_90F2_836F0347D0E0__INCLUDED_)
#define _LIVESUBSCRIBEHELPER_H__B2C93EC9_D4EA_4f82_90F2_836F0347D0E0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#import "LSClient2.tlb" no_namespace	// LiveSubscribe interfaces. 2.0+


struct LSH_HRX
{
	LSH_HRX():m_hrLastError(S_OK),m_bS_OK_Is_Only_Success(false){}
	LSH_HRX(HRESULT hr):m_bS_OK_Is_Only_Success(false)
	{
		if(		
				FAILED(hr) 
			|| ( m_bS_OK_Is_Only_Success?(hr != S_OK):false )
		  )
		{
			throw _com_error(hr);
		}
	}

	LSH_HRX& operator<<(HRESULT hr)
	{
		m_hrLastError = hr;
		if(		
				FAILED(hr) 
			|| ( m_bS_OK_Is_Only_Success?(hr != S_OK):false )
		  )
		{
			throw _com_error(hr);
		}
		return *this;
	}
	HRESULT GetLastError(){return m_hrLastError;}
	bool m_bS_OK_Is_Only_Success;
private:
	// DONT ALLOW THIS OPERATOR
	LSH_HRX& operator=(HRESULT hr)
	{
		// DONT ALLOW THIS OPERATOR
		return *this;
	}
	HRESULT m_hrLastError;
};

class CLiveSubscribeHelper
{
public:
    
    static bool GetServiceID(_bstr_t& bstrID)
    {
		TCHAR szServID[MAX_PATH] = {0};
        CRegKey Key;
        LONG rc = Key.Open(HKEY_LOCAL_MACHINE,cstrNAVDefnnuityKey,KEY_READ);
        if(rc == ERROR_SUCCESS)
        {
			DWORD dwSize = sizeof(szServID);
			if (ERROR_SUCCESS == Key.QueryValue(szServID,cstrServID,&dwSize))
			{
				bstrID = szServID;
                return true;
			}
        }

        bstrID = cstrNavLSServiceID;
        return true;
    }
    
    static bool IsK2Present()
    {   
        // if we can instantiate a K2 object we must have K2.
        try
        {
            LSH_HRX hrx;
            hrx.m_bS_OK_Is_Only_Success = true;
            // A smart pointer to the LiveSubscribe COM object.
            ISubscriptionInfoPtr	ptrSubscriptionInfo;
            
            // Create an instance of the LiveSubscribe object.
            hrx << ptrSubscriptionInfo.CreateInstance( __uuidof(SubscriptionInfo) ,NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ));
            
            return true;
        }
        catch( _com_error& /*e*/)
        {
            _ASSERTE(FALSE);
            return false;
        }
        catch(...)
        {
            _ASSERTE(FALSE);
            return false;
        }
    }
    /////////////////////////////////////////////////////////////////////////////
    // Unregister -
    //		This function should be called when this callback is unregistered.  This 
    // function will disable our subscription with LiveSubscribe.
    static HRESULT Unregister(LPCTSTR lpcLSServiceID)
    {
        LSH_HRX hrx;
        hrx.m_bS_OK_Is_Only_Success = true;
        
        try
        {
            // A smart pointer to the LiveSubscribe COM object.
            ISubscriptionInfoPtr	ptrSubscriptionInfo;
            
            // Create an instance of the LiveSubscribe object.
            hrx << ptrSubscriptionInfo.CreateInstance( __uuidof(SubscriptionInfo) ,NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ));
            
            // Attach to the virus definitions' subscription.  If we can't 
            // attach to our subscription, then there will be nothing to
            // disable.
            ptrSubscriptionInfo->AttachTo( lpcLSServiceID );
            
            // Disable our subscription.
            ptrSubscriptionInfo->PutDisabled( TRUE );
            
            // Set the subscription state to the return code of this function.  Only if
            //the state is set to S_OK will the virus definitions be enabled.
            return S_OK;
        }
        catch( _com_error& e)
        {
            _ASSERTE(FALSE);
            return e.Error();
        }
        catch(...)
        {
            _ASSERTE(FALSE);
            return E_UNEXPECTED;
        }
        
    } // Unregister
    
    
};

#endif // !defined(_LIVESUBSCRIBEHELPER_H__B2C93EC9_D4EA_4f82_90F2_836F0347D0E0__INCLUDED_)
