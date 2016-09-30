#include "StdAfx.h"
#include "HPPLVActivityLogEvent.h"

#include "resource.h"
#include <string>
#include <sstream>

#define COLUMNTIME 0
#define COLUMNPROCESS 1
#define COLUMNACTION 2

CHPPLVActivityLogEvent::CHPPLVActivityLogEvent(void)
{
}

CHPPLVActivityLogEvent::~CHPPLVActivityLogEvent(void)
{
}

HRESULT CHPPLVActivityLogEvent::GetImageListIndex(int& iImageListIndex)
{
    iImageListIndex = NULL;
    return S_OK;
}


HRESULT CHPPLVActivityLogEvent::GetColumnData(int iColumn, 
                                              DWORD dwFlags,
                                              VARIANT& varData)
{
    HRESULT hrReturn = E_FAIL;

    switch(iColumn)
    {
        case COLUMNTIME:
            {
                SYSTEMTIME stNow;
				SYSTEMTIME systimeLocal;
                DATE dtDate;

                if(m_spEvent->GetPropertySYSTEMTIME(HPP::Event_LogEntry_HomePageActivity_propTimestamp, &stNow))
                {
					if ( LV_DATE_FORMAT_LOCAL == dwFlags )
					{
						 UTCSystemTimeToLocalTime ( &stNow, &systimeLocal );
						 SystemTimeToVariantTime (&systimeLocal, &dtDate);
					}
					else
						SystemTimeToVariantTime (&stNow, &dtDate);

                    V_VT(&varData) = VT_DATE;
                    V_DATE(&varData) = dtDate;

                    hrReturn = S_OK;
                }
            }
            break;
            
        case COLUMNPROCESS:
            {
                _bstr_t cbszProcName;
				BSTR cbszProcessName; 
                if(m_spEvent->GetPropertyBSTR(HPP::Event_LogEntry_HomePageActivity_propProcessName, cbszProcName.GetAddress()))
                {
                    V_VT(&varData) = VT_BSTR;
					cbszProcessName = cbszProcName.copy();
                    V_BSTR(&varData) = cbszProcessName;
					SysFreeString(cbszProcessName);
                    hrReturn = S_OK;
               }
            }
            break;

        case COLUMNACTION:
            {
				CString cszTempStr;
				g_Resources.LoadString(IDS_CAT_COL_UNKNOWN, cszTempStr);
                _bstr_t cbszAction = _T(cszTempStr);
                long lBlockAction;
                if(m_spEvent->GetPropertyLONG(HPP::Event_LogEntry_HomePageActivity_propActionResult, &lBlockAction))
                {
                    if(HPP::HPP_ACTION_BLOCK == lBlockAction)
                    {
						g_Resources.LoadString(IDS_CAT_COL_BLOCKED, cszTempStr);
                        cbszAction = _T(cszTempStr);
                    }
                    else if(HPP::HPP_ACTION_ALLOW == lBlockAction)
                    {
                        g_Resources.LoadString(IDS_CAT_COL_ALLOWED, cszTempStr);
                        cbszAction = _T(cszTempStr);
                    }
                    else
                    {
                        CCTRACEE(_T("GetPropertyLONG(Event_LogEntry_HomePageActivity_propActionResult) return unexpected value.  lBlockAction == %d"), lBlockAction);
                    }
                
                }

                V_VT(&varData) = VT_BSTR;
                V_BSTR(&varData) = cbszAction.copy();
 
                hrReturn = S_OK;
            }
            break;

        default:
            break;
    }

    return hrReturn;
}


HRESULT CHPPLVActivityLogEvent::GetDetails(cc::LVEVENT_DETAILSTYPE eType, 
                                           DWORD dwFlags,
										   LPTSTR szDetailText, 
                                           DWORD& dwSize)
{
	
		CString cszDetailsSHORT = _T("Details: Short");
		CString cszDetailsNORMAL = _T("Details: Normal");
		CString cszDetailsHTM = _T("Details: HTML");
		CString cszReturn;
		CString cszStrName;
		CString cszHtmlSource;
		CString cszTemp;
		_bstr_t cbszTempStr;
		HRESULT hr;
		hr = LoadHtmlResource(IDR_HTM_LOGENTRY, cszHtmlSource);
		if(SUCCEEDED(hr))
		{   
			CString cszBody;
			CString cszSeperator;
			g_Resources.LoadString(IDS_CAT_DETAILS_SEPERATOR_NORMAL, cszSeperator);
			g_Resources.LoadString(IDS_CAT_DETAILS, cszStrName);
	   		cszBody += cszStrName;   	

				//
				//Displaying Time
				//

			SYSTEMTIME stNow;
			SYSTEMTIME systimeLocal;
            DATE dtDate;
			_variant_t varData;
            if(m_spEvent->GetPropertySYSTEMTIME(HPP::Event_LogEntry_HomePageActivity_propTimestamp, &stNow))
            {
                if ( LV_DATE_FORMAT_LOCAL == dwFlags )
				{
					UTCSystemTimeToLocalTime ( &stNow, &systimeLocal );
					SystemTimeToVariantTime (&systimeLocal, &dtDate);
				}
				else
					SystemTimeToVariantTime (&stNow, &dtDate);
                V_VT(&varData) = VT_DATE;
                V_DATE(&varData) = dtDate;
				CString strData;
				if (varData.vt == VT_DATE)
				{
					// Convert from DATE to SYSTEMTIME.
					TCHAR szDateTime[MAX_PATH] = {0};
					DWORD dwSize = sizeof(szDateTime)/sizeof(TCHAR);
					SYSTEMTIME sysTime = {0};
					VariantTimeToSystemTime(varData.date, &sysTime);

					// Format the date.

					int iCount = GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE, &sysTime, NULL, szDateTime, dwSize);

					_tcscat(szDateTime, _T(" "));

					// Add the time string to the end.

					TCHAR* pEnd = &szDateTime[iCount];

					// Set size to size of unused data in buffer.

					dwSize -= static_cast<DWORD>((pEnd - szDateTime)/sizeof(TCHAR));

					// Format the time.

					iCount = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sysTime, NULL, pEnd, dwSize); 

					strData = szDateTime;
				}
				else if(varData.vt == VT_CY)
				{
					// Defect 1-2E67IB a VT_CY value is actually a numeric
					// value that is to be represented as a duration of time.
					// it should be formatted as hours:minutes:seconds.fractionofsecond
					::ATL::CAtlString strATLData;
					strATLData.Format(_T("%I64d:%02I64d:%02I64d.%03I64d"), 
                                    (varData.cyVal.int64 / 3600000),                    // Hours
                                    ((varData.cyVal.int64 % 3600000) / 60000),          // Minutes
                                    (((varData.cyVal.int64 % 360000) % 60000) / 1000),  // Seconds
                                    (((varData.cyVal.int64 % 360000) % 60000) % 1000)); // Fraction
                    
					strData = strATLData;                   
				}
				else
				{
					// Convert the data to string.
					USES_CONVERSION;
					varData.ChangeType(VT_BSTR);
					strData = OLE2T(varData.bstrVal);
				}
				g_Resources.LoadString(IDS_CAT_DETAILS_TIME, cszTemp);
				cszStrName.FormatMessage(cszTemp, strData);
				cszBody += cszStrName;
				g_Resources.LoadString(IDS_CAT_DETAILS_TIME_NORMAL, cszTemp);
				cszStrName.FormatMessage(cszTemp, strData);
				cszDetailsNORMAL = cszStrName;
				cszDetailsNORMAL += cszSeperator;
			}

			//
			//Displaying Process Name/Path and PID 
			//
			LONG lpPID;
			if(m_spEvent->GetPropertyBSTR(HPP::Event_LogEntry_HomePageActivity_propProcessPath, cbszTempStr.GetAddress())
				&& m_spEvent->GetPropertyLONG(HPP::Event_LogEntry_HomePageActivity_propProcessID, &lpPID))
			{
				g_Resources.LoadString(IDS_CAT_DETAILS_ACTOR, cszTemp);
				cszStrName.FormatMessage(cszTemp, (LPCTSTR)cbszTempStr, lpPID);
				cszBody += cszStrName;
				g_Resources.LoadString(IDS_CAT_DETAILS_ACTOR_NORMAL, cszTemp);
				cszStrName.FormatMessage(cszTemp, (LPCTSTR)cbszTempStr, lpPID);
				cszDetailsNORMAL += cszStrName;
				cszDetailsNORMAL += cszSeperator;
			}
				
			LONG lKeyLocation = HPP::HPP_LOCATIONS_HKCU;
            m_spEvent->GetPropertyLONG(HPP::Event_LogEntry_HomePageActivity_propActionLocation, &lKeyLocation);
            bool bLocation = HPP::HPP_LOCATIONS_HKCU == lKeyLocation ? true : false;
			CString cszLocation = true == bLocation ? _T("true") : _T("false");
			
			if(bLocation)  //change to HKCU, display target as "Internet Explorer Home Page"
			{
				//
				//Displaying Target (IE)
				//
				g_Resources.LoadString(IDS_CAT_DETAILS_TARGET, cszStrName);
				cszBody += cszStrName;
				g_Resources.LoadString(IDS_CAT_DETAILS_TARGET_NORMAL, cszStrName);
				cszDetailsNORMAL += cszStrName;
				cszDetailsNORMAL += cszSeperator;
			}
			else
			{
				//
				//Displaying Target (Internet Explorer SYSTEM Home Page
				//
				g_Resources.LoadString(IDS_CAT_DETAILS_TARGET_HKLM, cszStrName);
				cszBody += cszStrName;
				g_Resources.LoadString(IDS_CAT_DETAILS_TARGET_HKLM_NORMAL, cszStrName);
				cszDetailsNORMAL += cszStrName;
				cszDetailsNORMAL += cszSeperator;
			}
			//
			//Displaying Original Home Page 
			//
			if(m_spEvent->GetPropertyBSTR(HPP::Event_LogEntry_HomePageActivity_propStartValue, cbszTempStr.GetAddress()))
			{
				g_Resources.LoadString(IDS_CAT_DETAILS_ORIGINALVALUE, cszTemp);
				cszStrName.FormatMessage(cszTemp, (LPCTSTR)cbszTempStr);
				cszBody += cszStrName;

				g_Resources.LoadString(IDS_CAT_DETAILS_ORIGINALVALUE_NORMAL, cszTemp);
				cszStrName.FormatMessage(cszTemp, (LPCTSTR)cbszTempStr);
				cszDetailsNORMAL += cszStrName;
				cszDetailsNORMAL += cszSeperator;
                  
				
                //
				//Displaying Reset Home Page Link
				//
				g_Resources.LoadString(IDS_HTML_LINK, cszTemp);
				cszStrName.FormatMessage(cszTemp, (LPCTSTR)cbszTempStr, cszLocation);
				cszBody += cszStrName;
			}
			
			//
			//Displaying Allowed/Blocked Home Page
			//
			
			LONG lpResult = 1;
			if(m_spEvent->GetPropertyLONG(HPP::Event_LogEntry_HomePageActivity_propActionResult, &lpResult) && 
				m_spEvent->GetPropertyBSTR(HPP::Event_LogEntry_HomePageActivity_propActionValue, cbszTempStr.GetAddress()))
			{
				CString cszTempNormal;
				if(lpResult == 1)
				{
					g_Resources.LoadString(IDS_CAT_DETAILS_BLOCKEDVALUE, cszTemp);
					g_Resources.LoadString(IDS_CAT_DETAILS_BLOCKEDVALUE_NORMAL, cszTempNormal);
				}
				else if(lpResult == 2)
				{
					g_Resources.LoadString(IDS_CAT_DETAILS_ALLOWEDVALUE, cszTemp);
					g_Resources.LoadString(IDS_CAT_DETAILS_ALLOWEDVALUE_NORMAL, cszTempNormal);
				}
				cszStrName.FormatMessage(cszTemp, (LPCTSTR)cbszTempStr);
				cszBody += cszStrName;
				cszStrName.FormatMessage(cszTempNormal, (LPCTSTR)cbszTempStr);
				cszDetailsNORMAL += cszStrName;

                //
				//Displaying Reset Home Page Link
				//
				g_Resources.LoadString(IDS_HTML_LINK, cszTemp);
				cszStrName.FormatMessage(cszTemp, (LPCTSTR)cbszTempStr, cszLocation);
				cszBody += cszStrName;
			}
			
			cszDetailsHTM.FormatMessage(cszHtmlSource, cszBody);
            CCTRACEI(_T("HTML String:\r\n%s\r\n"), cszDetailsHTM);

            switch(eType)
			{
				case cc::LVEDT_SHORT:
					cszReturn = cszDetailsSHORT;
					break;

				case cc::LVEDT_NORMAL:
					cszReturn = cszDetailsNORMAL;
					break;

				case cc::LVEDT_HTML:
					cszReturn = cszDetailsHTM;
					break;

				default:
					cszReturn = _T("");

			}

			if(dwSize <= (DWORD)cszReturn.GetLength())
			{
				dwSize = cszReturn.GetLength() + 1;
					return HRESULT_FROM_WIN32(E_MORE_DATA);
			}
	}
	_tcscpy(szDetailText, cszReturn);

	
	return S_OK;
}

HRESULT CHPPLVActivityLogEvent::LoadHtmlResource(UINT iResourceId, CString &cszHtmlResource)
{
    HRESULT hrReturn = E_FAIL;


    HMODULE hModule = AtlFindResourceInstance(MAKEINTRESOURCE(iResourceId), RT_HTML);
    if(!hModule)
        return E_FAIL;

    HRSRC hResource = ::FindResource( hModule, MAKEINTRESOURCE(iResourceId), RT_HTML );
    if(!hResource)
        return E_FAIL;

    HGLOBAL hg = 0;

    // Load up the resource into an HGLOBAL
    DWORD dwSizeofResource = SizeofResource(hModule, hResource);
    hg = ::LoadResource( hModule, hResource );

    if( hg )
    {
        // Lock the resource, and get an LPVOID ptr to it
        LPVOID  lpResourceData = NULL;
        lpResourceData = ::LockResource( hg );

        LPTSTR szBuf = cszHtmlResource.GetBuffer(dwSizeofResource + 1);
        if(!szBuf)
			return E_OUTOFMEMORY;

		ZeroMemory(szBuf, dwSizeofResource+1);
        memcpy(szBuf, lpResourceData, dwSizeofResource);
        cszHtmlResource.ReleaseBuffer();

        ::DeleteObject( hg );

        hrReturn = S_OK;
    }

    return hrReturn;
}

// Converts UTC SYSTEMTIME to Local SYSTEMTIME
//
bool CHPPLVActivityLogEvent::UTCSystemTimeToLocalTime ( const SYSTEMTIME* psystimeUTC, SYSTEMTIME* psystimeLocal )
{
    // Log is in UTC System time. We need Variant Local Time. Ugh.
    //
    FILETIME ftimeUTC;
    FILETIME ftimeLocal;

    if ( SystemTimeToFileTime ( psystimeUTC, &ftimeUTC ) &&
         FileTimeToLocalFileTime ( &ftimeUTC, &ftimeLocal ) &&
         FileTimeToSystemTime ( &ftimeLocal, psystimeLocal ))
         return true;
    else 
        return false;
}

// Converts Local SYSTEMTIME to UTC SYSTEMTIME
//
bool CHPPLVActivityLogEvent::LocalTimeToUTCSystemTime ( const SYSTEMTIME* psystimeLocal, SYSTEMTIME* psystimeUTC )
{
    FILETIME ftimeUTC;
    FILETIME ftimeLocal;

    if ( SystemTimeToFileTime ( psystimeLocal, &ftimeLocal ) &&
         LocalFileTimeToFileTime ( &ftimeLocal, &ftimeUTC ) &&
         FileTimeToSystemTime ( &ftimeUTC, psystimeUTC ))
         return true;
    else 
        return false;
}

