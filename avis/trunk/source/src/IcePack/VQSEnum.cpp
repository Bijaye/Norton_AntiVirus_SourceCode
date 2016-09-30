// VQSEnum.cpp: implementation of the VQSEnum class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VQSEnum.h"
#include <AttributeKeys.h>

#include <atlbase.h>
#include <algorithm>
#include <assert.h>

#include <Logger.h>


using namespace std;

static string	GenErrorMsg(HRESULT hr, const char* where)
{
	std::string	errMsg(where);
	errMsg	+= " [";
	char		buffer[32];
	sprintf(buffer, "%x", hr);
	errMsg	+= buffer;
	errMsg	+= "]";
	if (E_INVALIDARG == hr)
		errMsg += " E_INVALIDARG";

	return errMsg;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VQSEnum::VQSEnum() : sampleID(0), items(NULL)
{

}

VQSEnum::~VQSEnum()
{
	if (NULL != items)
		items->Release();
}

bool VQSEnum::Set(IEnumQuarantineServerItems* it)
{
	EntryExit	entryExit(Logger::LogIcePackSample, "VQSEnum::Set");

	if (items)
	{
		HRESULT	hr = items->Release();
		throw IcePackException(IcePackException::VQSEnum,
				SampleBase::GenErrorMsg(hr, "VQSEnum::Set, items->Release()", 0));
	}
	items = it;

	return true;
}

bool VQSEnum::Reset()
{
	EntryExit	entryExit(Logger::LogIcePackSample, "VQSEnum::Reset");

	bool	rc = false;

	HRESULT	hr = items->Reset();
	if (FAILED(hr))
	{
		throw IcePackException(IcePackException::VQSEnum, 
				SampleBase::GenErrorMsg(hr, "VQSEnum::Reset, items->Reset()", 0));
	}

	return rc;
}

bool VQSEnum::Next(Sample& sample, list<Sample>& existing, bool& newRecord,
				   list<Sample>::iterator& i)
{
	EntryExit	entryExit(Logger::LogIcePackSample, "VQSEnum::Next");

	bool					fetchAgain = true;
	bool					rc	= false;
	IQuarantineServerItem	*item;
	ULONG					howManyFetched;
	static _bstr_t			dateCompletedKey(AttributeKeys::DateCompleted().c_str());
	static _bstr_t			submissionRouteKey(AttributeKeys::SampleSubmissionRoute().c_str());
	static _bstr_t			sampleStatusKey(AttributeKeys::SampleStatus().c_str());
	static _bstr_t			sampleIDKey(AttributeKeys::SampleFileID().c_str());
	static _bstr_t			sampleChangesKey(AttributeKeys::SampleChanges().c_str());

#if 0
	static _bstr_t			platformDeliverKey(AttributeKeys::PlatformDeliver().c_str());

	_bstr_t					platformDeliverValue;
#endif /* 0 */
  char                loggingBuffer[128];

	HRESULT	hr;

	newRecord	= false;

	do
	{
		i = existing.end();

		hr = items->Next(1, &item, &howManyFetched);
		if (FAILED(hr))
		{
			throw IcePackException(IcePackException::VQSEnum, 
					SampleBase::GenErrorMsg(hr, "VQSEnum::Next, items->Next()", 0));
		}

		if (0 == howManyFetched)
		{
			sample.ID(NULL);
			fetchAgain = false;
      Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor,
                  "Next() thinks it's at the end of the list");

		}
		else
		{
			VARIANT	         	vValue;
      unsigned long int sampleFileID;

      /*
       * This chunk of code retrieves the sample's FileID for logging
       * purposes.
       */
      {
        VARIANT   vSampleFileID;
        
        VariantInit(&vSampleFileID);
        VariantClear(&vSampleFileID);

        hr = item->GetValue(sampleIDKey, &vSampleFileID);
			  if (FAILED(hr))
			  {
  				throw IcePackException(IcePackException::VQSEnum, 
							  GenErrorMsg(hr, "VQSEnum::Next, id->GetValue(X-Sample-FileID)"));
			  }

        sampleFileID = (unsigned long int)vSampleFileID.lVal;
      }

      VariantInit(&vValue);

			hr = item->GetValue(dateCompletedKey, &vValue);
			if (FAILED(hr))
			{
				throw IcePackException(IcePackException::VQSEnum, 
							GenErrorMsg(hr, "VQSEnum::Next, id->GetValue(X-Date-Completed)"));
			}
			if ((SysStringLen(vValue.bstrVal) == 0) || (isThisAttributePseudoDeleted(vValue)))
			{
				VariantClear(&vValue);

				hr = item->GetValue(submissionRouteKey, &vValue);
				if (FAILED(hr))
				{
					throw IcePackException(IcePackException::VQSEnum, 
								GenErrorMsg(hr, "VQSEnum::Next, id->GetValue(X-Sample-Submission-Route)"));
				}
				if ((0 == vValue.lVal) || (isThisAttributePseudoDeleted(vValue)))
				{
					VariantClear(&vValue);

					hr = item->GetValue(sampleStatusKey, &vValue);
					if (FAILED(hr))
					{
						throw IcePackException(IcePackException::VQSEnum, 
									GenErrorMsg(hr, "VQSEnum::Next, id->GetValue(X-Sample-Status)"));
					}

					ulong	status = vValue.lVal;

					//
					// IcePack only looks at sample in the quarantine state when the
					// submission route is set to scan and deliver.
					if (SampleStatus::quarantined == status)
					{
						fetchAgain	= false;
						newRecord	= true;
            sprintf(loggingBuffer, "Next() using sample %08x -- X-Sample-Status == 'quarantined'", sampleFileID);
            Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, loggingBuffer);
					}
          else /* No X-Sample-Submission-Route, X-Sample-Status not 'quarantined' */
          {
            sprintf(loggingBuffer, "Next() skipping sample %08x -- no X-Sample-Submission-Route, and X-Sample-Status != 'quarantined'", sampleFileID);
            Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, loggingBuffer);
          }
				}
				else
				{
          /*
           * This chunk of code is a mystery to me.
           * inw 2000-04-24
           */
					VariantClear(&vValue);
					hr = item->GetValue(sampleIDKey, &vValue);
					if (FAILED(hr))
					{
						throw IcePackException(IcePackException::SampleBase,
									GenErrorMsg(hr, "VQSEnum::Next, id->GetValue(X-Sample-Fileid)"));
					}
					CompSampleByKeyOnly	comp(vValue.lVal);
					i = find_if(existing.begin(), existing.end(), comp);
					newRecord = (i == existing.end());
					if (!newRecord)
					{
						VariantClear(&vValue);
						hr = item->GetValue(sampleChangesKey, &vValue);
						if (FAILED(hr))
						{
							throw IcePackException(IcePackException::SampleBase,
										GenErrorMsg(hr, "VQSEnum::Next, id->GetValue(X-Sample-Changes)"));
						}
						if (i->Changes() != vValue.lVal)
            {
							fetchAgain = false;
              sprintf(loggingBuffer, "Next() using sample %08x -- sample has changed", sampleFileID);
              Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, loggingBuffer);
            }
						else
            {
							i->Mark(Sample::sUnchanged);
              sprintf(loggingBuffer, "Next() skipping sample %08x -- sample has not changed", sampleFileID);
              Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, loggingBuffer);
            }
					}
					else
          {
						fetchAgain = false;
            sprintf(loggingBuffer, "Next() using sample %08x -- happy with copy", sampleFileID);
            Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, loggingBuffer);
          }
				}
			}
      else /* X-Date-Completed is set */
      {
        sprintf(loggingBuffer, "Next() skipping sample %08x -- X-Date-Completed set", sampleFileID);
        Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, loggingBuffer);                    
      }

			VariantClear(&vValue);

			if (!fetchAgain)
			{
				sample.ID(item);
				rc = true;
			}
			else
				item->Release();
		}
	} while (fetchAgain);

	return rc;
}


void VQSEnum::GetErrorMessage(std::string& errMsg)
{

}


