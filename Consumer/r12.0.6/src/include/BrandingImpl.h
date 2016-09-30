// BrandingImpl.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_BRANDINGIMPL_H__5AA922B3_F47E_4ce2_AADF_B58EA807D279__INCLUDED_)
#define _BRANDINGIMPL_H__5AA922B3_F47E_4ce2_AADF_B58EA807D279__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "OEMProductIntegrationDefines.h"

class CBrandingImpl
{
public:
    CBrandingImpl(){}
    HRESULT Initialize()
    {
	    StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        try
        {
        
       
            ////////////////////////////////////////////////////////
            // Pull the UniqueOEM ID from the Branding.ini file
            TCHAR szFileName[_MAX_PATH * 2] = {0};
            ::GetModuleFileName(_Module.m_hInst,szFileName,sizeof(szFileName));
            TCHAR* p = _tcsrchr(szFileName,_T('\\'));
            *p = _T('\0');
            _tcscat(szFileName,_T("\\BRANDING.INI"));
            UINT uiRet = ::GetPrivateProfileInt(OEMDATFILE_Integrator,OEMDATFILE_UniqueOEMID,0,szFileName);
            hrx << ((uiRet == 0)?E_FAIL:S_OK);
            /////////////////////////////////////////////////////////
            // Mark this plugin's unique OEM ID from whatever was in the INI file.
            SetValue(OEMPQV_UniqueOEMID,DWORD(uiRet));
        
            UINT uiIndex = 0;
            do
            {
                ////////////////////////////////////////////////////////
                // LOOP through all of the IntegrateWith OEMID items and add them to my list
                TCHAR szTemp[64]	= OEMDATFILE_IntegrateWithOEMID;
                TCHAR szTemp2[64]	= {0};
                itoa(uiIndex,szTemp2,10);
                _tcscat(szTemp,szTemp2);
                uiRet = ::GetPrivateProfileInt(OEMDATFILE_Integrator,szTemp,0,szFileName);
                SetValue(OEMPQV_IntegrateWithUniqueOEMID0+uiIndex,DWORD(uiRet));
                ++uiIndex;
            }while(	uiRet != 0 
                &&	(OEMPQV_IntegrateWithUniqueLAST >= (OEMPQV_IntegrateWithUniqueOEMID0+uiIndex)) );
        
        }
        catch(_com_error& e)
        {
            hr = e.Error(); // SHIP WITH THIS
        }
        catch(...)
        {
            hr = E_UNEXPECTED;
        }
        return hr;
    
    }

	HRESULT SetValue(/*[in]*/ DWORD dwValueID, /*[in]*/ DWORD dwValue)
    {
        HRESULT hr = S_OK;
        try
        {
            //~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
            StahlSoft::CSmartCritSection smCrit(&m_critmapValue);
            //~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
            m_mapValue.insert(std::make_pair(dwValueID,dwValue));
        }
        catch(_com_error& e)
        {
            hr = e.Error();        
        }
        catch(...)
        {
            hr = E_UNEXPECTED;
        }
        return hr;

    }
	HRESULT GetValue(/*[in]*/ DWORD dwValueID, /*[out, retval]*/ DWORD* pdwValue)
    {
        HRESULT hr = S_OK;
        try
        {
            //~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
            StahlSoft::CSmartCritSection smCrit(&m_critmapValue);
            //~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
            std::map<DWORD,DWORD>::iterator it;
            it = m_mapValue.find(dwValueID);
            if(it == m_mapValue.end())
            {   
                *pdwValue = DWORD(-1);
                hr = S_FALSE;
            }
            else
            {
                *pdwValue = (*it).second;
            }
        }
        catch(_com_error& e)
        {
            hr = e.Error();        
        }
        catch(...)
        {
            hr = E_UNEXPECTED;
        }
        return hr;
    }
    bool IsInIntegrateWithList(DWORD dwProductUniqueOEMID)
    {

        StahlSoft::HRX hrx;
        bool bFound = false;
        HRESULT hr = S_OK;
        try
        {
            DWORD dwMyID = 0;
            hrx << GetValue(OEMPQV_UniqueOEMID,&dwMyID);
            DWORD dwIW = (DWORD)-1;
            DWORD dwIDIndex = OEMPQV_IntegrateWithUniqueOEMID0;
            HRESULT hr = S_OK;
            while(  !bFound 
                &&  SUCCEEDED(hr) 
                && (dwIDIndex <= OEMPQV_IntegrateWithUniqueLAST) )
            {
                hr = GetValue(dwIDIndex,&dwIW);
                if(SUCCEEDED(hr))
                {
                    bFound = (dwIW == dwProductUniqueOEMID);
                }
                ++dwIDIndex;
            }
        }
        catch(_com_error& e)
        {
            hr = e.Error();        
        }
        catch(...)
        {
            hr = E_UNEXPECTED;
        }
        return bFound;
    }

protected:
    /////////////////////////////////////////////////////////////////////
    // ALL Values go into this map whenever SetValue is called.
    // ALL Values are fetched from this map whenever GetValue is called.
    /////////////////////////////////////////////////////////////////////
    std::map<DWORD,DWORD>               m_mapValue;
    StahlSoft::CWin32CriticalSection    m_critmapValue;
};


#endif // !defined(_BRANDINGIMPL_H__5AA922B3_F47E_4ce2_AADF_B58EA807D279__INCLUDED_)
