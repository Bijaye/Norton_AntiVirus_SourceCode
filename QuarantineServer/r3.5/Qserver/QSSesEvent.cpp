/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#include "stdafx.h"
#include "AvisEventDetection.h"   // in QuarantineServer\Include 
#include "EventObject.h"

#include "QSSesEvent.h"
#define NO_EVENT_IDS			11
#define NO_SAMPLE_EVENT_IDS		23


/* This table is a lookup table used to link the Quarantine AMS events to Naboo events for general type events*/
static GENEVENTTABLE SES2QSEventTable[NO_EVENT_IDS]={
//     QS Event ID									  SES Event ID				
	GENERAL_ATTENTION_DEFCAST_EVENTID,				SES_EVENT_DEFCAST_ERROR,			
	GENERAL_ATTENTION_DISK_EVENTID,					SES_EVENT_UNABLE_TO_ACCESS_DEFINITION_DIRECTORY,		
	GENERAL_ATTENTION_SCANEXPLICIT_EVENTID,			SES_EVENT_CANNOT_CONNECT_TO_QUARANTINE_SCANNER_SERVICE,		
	GENERAL_ATTENTION_TARGET_EVENTID,				SES_EVENT_CANNOT_INSTALL_DEFINITIONS_ON_TARGET_MACHINES,	
//	GENERAL_ATTENTION_UNAVAIABLE_EVENTID,	251  ,
	GENERAL_ATTENTION_GATEWAY_QUERY_ALERT_EVENTID,	SES_EVENT_UNABLE_TO_CONNECT_TO_GATEWAY,			
	GENERAL_ATTENTION_DISKQUOTA_LOW_WATER,			SES_EVENT_DISK_QUOTA_REMAINING_IS_LOW_FOR_QUARANTINE_DIR,	
	GENERAL_ATTENTION_DISKSPACE_LOW_WATER,			SES_EVENT_DISK_FREE_SPACE_LESS_THAN_QUARANTINE_MAX_SIZE,	
	DEF_ALERT_NEW_BLESSED_DEFS_EVENTID,				SES_EVENT_NEW_CERTIFIED_DEFINITIONS_ARRIVED,	
	DEF_ALERT_NEW_UNBLESSED_DEFS_EVENTID,			SES_EVENT_NEW_NON_CERTIFIED_DEFINITIONS_ARRIVED,	
	GENERAL_ATTENTION_SHUTDOWN_EVENTID,				SES_EVENT_THE_QUARANTINE_AGENT_SERVICE_HAS_STOPPED,	
	GENERAL_ATTENTION_SEND_TEST_EVENT,				SES_EVENT_CENTRAL_QUARANTINE_TEST_ALERT,	
	};

/* This table is a lookup table used to link the Quarantine AMS events to Naboo events for general type events*/
static GENEVENTTABLE SES2QSSampleEventTable[NO_SAMPLE_EVENT_IDS]={
//     QS Event ID									  SES Event ID				
	SAMPLE_ALERT_HELD_EVENTID,						SES_EVENT_SAMPLE_HELD_FOR_MANUAL_SUBMISSION,			
	SAMPLE_ALERT_QUARANTINED_EVENTID,				SES_EVENT_SAMPLE_TOO_LONG_WITH_QUARANTINED_STATUS,		
	SAMPLE_ALERT_SUBMITTED_EVENTID,					SES_EVENT_SAMPLE_TOO_LONG_WITH_SUBMITTED_STATUS,		
	SAMPLE_ALERT_RELEASED_EVENTID,					SES_EVENT_SAMPLE_TOO_LONG_WITH_RELEASED_STATUS,	
	SAMPLE_ALERT_NEEDED_EVENTID,					SES_EVENT_SAMPLE_TOO_LONG_WITH_NEEDED_STATUS,			
	SAMPLE_ALERT_DISTRIBUTED_EVENTID,				SES_EVENT_SAMPLE_TOO_LONG_WITH_DISTRIBUTED_STATUS,	
	SAMPLE_ALERT_DISTRIBUTE_EVENTID,				SES_EVENT_SAMPLE_TOO_LONG_WITHOUT_INSTALLING_NEW_DEFS,	
	QUARANTINE_ALERT_NOT_REPAIRED_EVENTID,			SES_EVENT_SAMPLE_WAS_NOT_REPAIRED,	
	SAMPLE_ALERT_ATTENTION_EVENTID,					SES_EVENT_SAMPLE_NEEDS_ATTENTION_FROM_TECH_SUPPORT,	
	SAMPLE_ALERT_ABANDONED_EVENTID,					SES_EVENT_ICEPACK_ABANDONED,	
	SAMPLE_ALERT_CONTENT_EVENTID ,					SES_EVENT_ICEPACK_CONTENT,	
	SAMPLE_ALERT_CRUMBLED_EVENTID,					SES_EVENT_ICEPACK_CRUMBLED,			
	SAMPLE_ALERT_DECLINED_EVENTID,					SES_EVENT_ICEPACK_DECLINED,		
	SAMPLE_ALERT_INTERNAL_EVENTID,					SES_EVENT_ICEPACK_INTERNAL,		
	SAMPLE_ALERT_LOST_EVENTID,						SES_EVENT_ICEPACK_LOST,	
	SAMPLE_ALERT_MALFORMED_EVENTID,					SES_EVENT_ICEPACK_MALFORMED,			
	SAMPLE_ALERT_MISSING_EVENTID,					SES_EVENT_ICEPACK_MISSING,	
	SAMPLE_ALERT_OVERRUN_EVENTID,					SES_EVENT_ICEPACK_OVERRUN,	
	SAMPLE_ALERT_SAMPLE_EVENTID,					SES_EVENT_ICEPACK_SAMPLE,	
	SAMPLE_ALERT_SUPERCEDED_EVENTID,				SES_EVENT_ICEPACK_SUPERCEDED,	
	SAMPLE_ALERT_UNDERRUN_EVENTID,					SES_EVENT_ICEPACK_UNDERRUN,	
	SAMPLE_ALERT_UNPACKAGE_EVENTID,					SES_EVENT_ICEPACK_UNPACKAGE,	
	SAMPLE_ALERT_UNPUBLISHED_EVENTID,				SES_EVENT_ICEPACK_UNPUBLISH,	
	};


//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::Init
//
//  Purpose : Init the class.  Sets up a pointer to a Quarantine Event class
//
//  Parameters : 
//
//    CEvent * pQSEvent Pointer to an CEvent class used in CQ for its event proccessing
//
//  Return Values :
//
//    None.
//
//	Written By:  Terry Marles 4-16-01
//////////////////////////////////////////////////////////////////////////////////////
void QSSesGenEvent::Init (CAlertEvent * pQSEvent) 
{
	m_pQSEvent = pQSEvent;
};


//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::CreateSesEvent
//
//  Purpose : Verifies that there is enought information to create a Naboo Event.  It then
//			creates the event.
//
//  Parameters :	None
//
//  Return Values : HRESULT.  E_FAIL if there is a problem S_OK otherwise.
//
//  Written By:		Terry Marles 4-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesGenEvent::CreateSesEvent(void)
{
	USES_CONVERSION;

	HRESULT hr = S_OK;

	// check to see if the class was created with a good interface SES interface in applib.
	_ASSERT(m_pQSSesInterface != NULL);
	try 
	{
		if (m_pQSSesInterface->isInitialized() &&
			!m_sEventClass.IsEmpty() &&
			m_iSESEventID != 0 &&
			m_nSeverity != 0)
		{
			
			// create the Naboo Event.
			m_SesEvent = m_pQSSesInterface->createEvent(SESSWFID_SYMANTEC_CENTRAL_QUARANTINE___QUARANTINE_SERVER,
							m_iSESEventID,								// event id
							SES_CAT_APPLICATION,
							m_nSeverity,								// severity
							(LPSTR)T2A(m_sEventClass.GetBuffer(0)));	// event class
			if (m_SesEvent == NULL)
			{
				fWidePrintString("QSSesGenEvent::CreateSesEvent Creation of SESA Event failed.");
				hr = E_FAIL;
			}
		}
		else
		{
			if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW)
			{
				fWidePrintString("QSSesGenEvent::CreateSesEvent Creation of SESA Event failed.  AppLib not initilized, or improperly initilized QSSesGenEvent");
			}
			hr = E_FAIL;
		}
	}
	catch (...)
	{
		if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW)
		{
			fWidePrintString("QSSesGenEvent::CreateSesEvent Creation of SESA Event failed.  Try block failed creating event");
		}
		hr = E_FAIL;
		_ASSERT(0);
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::LookupSESEventID
//
//  Purpose : This function is a private member function that will look up the SES Event
//				id based on the Quarantine Server event ID.  It uses a static table at the
//				top of this module.
//
//  Parameters :
//
//    int QSEventID:	Quarantine Server Event ID
//
//  Return Values : int:  the return value is the SES Event ID or 0 if not found
//
//  Written By:  Terry Marles 4-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
int QSSesGenEvent::LookupSESEventID (int QSEventID, GENEVENTTABLE * pIdTable, int iNoItems)
{
	int SesEventID = 0;

	// Look through the table to see if we can find the QSEvent
	for (int i = 0; i< iNoItems; ++i)
		if (pIdTable[i].iCQEventID == QSEventID)
		{
			// hey we found it.  all done
			SesEventID = pIdTable[i].iSESEventID;
			break;
		}

	return SesEventID;
}


//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::LookupSESSeverity 
//
//  Purpose : Looks up a similar SES Severity id as a Quarantine Server Serverity id
//
//  Parameters :
//
//    int QSServerity - this is the Quarantine Server Severity int.
//
//  Return Values : int returns the SES Severity id that is equlavent to the QS id
//
//  Written by  Terry Marles	04-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
int QSSesGenEvent::LookupSESSeverity (int QSSeverity)
{
	int SesSeverity = 0;
	
	switch (QSSeverity)
	{
		case _SEVERITY_CRITICAL:
			SesSeverity = SES_SEV_CRITICAL;
			break;

		case _SEVERITY_MAJOR:
			SesSeverity = SES_SEV_MAJOR;
			break;

		case _SEVERITY_MINOR:
			SesSeverity = SES_SEV_WARNING;
			break;

		case _SEVERITY_INFORMATIONAL:
		default:
			SesSeverity = SES_SEV_INFORMATIONAL;
			break;
	}
	return SesSeverity;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::SendQSSESEvent
//
//  Purpose : private member function.  Sends the SES event.
//
//  Parameters :
//
//  Return Values :
//
//    HRESUSLT the return code is trapped from applib and returned
//
//	Written by Terry Marles 04-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesGenEvent::SendQSSESEvent(void)
{
	HRESULT hr = S_OK;
	try
	{
		hr = m_SesEvent->send();			// send the event

		if ( hr != S_OK && (_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW)
		{
			fWidePrintString("QSSesGenEvent::SendQSSESEvent Error occured trying to send SESA event");
		}
	}
	catch(...)
	{
		_ASSERT(0);
		hr = E_FAIL;
		if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW)
		{
			fWidePrintString("QSSesGenEvent::SendQSSESEvent Exception occured trying to send SESA event");
		}
	}
	return hr;

}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::SendQSSESEvent
//
//  Purpose : public member function that sets up the SES Event and sends it off
//
//  Parameters :
//
//    CEvent * pQSEvnet  -  pointer to a Quarantine Server Event.
//
//  Return Values :  Returns S_OK if everything is okay an error otherwise
//
//  Written by Terry Marles 04-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesGenEvent::SendQSSESEvent(CAlertEvent *pQSEvent)
{
	HRESULT hr = S_OK;
	
	// check the params
	_ASSERT (pQSEvent != NULL);
	if (pQSEvent == NULL)
		return E_INVALIDARG;


	// save off a pointer to the event.
	m_pQSEvent = pQSEvent;
	
	// set the type
	m_sEventClass = SES_CLASS_QUARANTINE;

    // set up the information for the event attributes
	SetEventAttrib();
	// create the event.
	if (CreateSesEvent()!= S_OK)
		return E_FAIL;
	// add in all the information for the fields of the symc_quarantine_class 
	SetGenEventFields ();
	// send the event.
	hr = SendQSSESEvent();
    
    
	
	return hr;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::SetGenEventFields
//
//  Purpose : This fuction will set the fields for symc_quarantine_class event class.
//			It does not override any of the agent base class fields
//
//  Parameters :
//
//    None.
//
//  Return Values :  HResult
//
//  Written By Terry Marles 04-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesGenEvent::SetGenEventFields(void)
{
	HRESULT hr = S_OK;
	USES_CONVERSION;
	try
	{
		if (!m_pQSEvent->sMachineName.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_HOSTNAME, (LPSTR) T2A(m_pQSEvent->sMachineName.GetBuffer(0)));
		if (!m_pQSEvent->sMessage.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_ERROR_MESSAGE, (LPSTR) T2A(m_pQSEvent->sMessage.GetBuffer(0)));
		if (!m_pQSEvent->sDefaultOrHelpMsg.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_DEFAULT_MESSAGE, (LPSTR) T2A(m_pQSEvent->sDefaultOrHelpMsg.GetBuffer(0)));
	}
	catch(...)
	{
		hr = E_FAIL;
		_ASSERT (0);
	}
	return hr;

}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::SetEventAttrib
//
//  Purpose : Sets up all of the attribute information for a SES Event
//
//  Parameters :
//
//    None
//
//  Return Values : HRESULT
//
//  Written by Terry Marles 04.16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesGenEvent::SetEventAttrib(void)
{
	HRESULT hr = S_OK;

	m_iSESEventID = LookupSESEventID(m_pQSEvent->dwEventId, SES2QSEventTable,NO_EVENT_IDS);
	if (m_iSESEventID ==0)
		return E_FAIL;
	
	m_nSeverity = (SESSeverity)LookupSESSeverity(m_pQSEvent->dwAlertSeverityLevel);

	return hr;

}





//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesSampleEvent::SendQSSESEvent
//
//  Purpose : public member function that sets up the SES Event and sends it off
//
//  Parameters :
//
//    CEvent * pQSEvnet  -  pointer to a Quarantine Server Event.
//
//  Return Values :  Returns S_OK if everything is okay an error otherwise
//
//  Written by Terry Marles 04-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesSampleEvent::SendQSSESEvent(CAlertEvent *pQSEvent)
{
	HRESULT hr = S_OK;
	
	// check the params
	_ASSERT (pQSEvent != NULL);
	if (pQSEvent == NULL)
		return E_INVALIDARG;


	// save off a pointer to the event.
	m_pQSEvent = pQSEvent;
	
	// set the type
	m_sEventClass = SES_CLASS_QUARANTINE_SAMPLE;

    // set up the information for the event attributes
	SetEventAttrib();
	// create the event.
	if (CreateSesEvent()!= S_OK)
		return E_FAIL;
	// add in all the information for the fields of the symc_quarantine_class 
	SetGenEventFields ();
	// send the event.
	hr = QSSesGenEvent::SendQSSESEvent();
    
    
	
	return hr;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesSampleEvent::SendQSSESEvent
//
//  Purpose : public member function that sets up the SES Event and sends it off
//
//  Parameters :
//
//    CEvent * pQSEvnet  -  pointer to a Quarantine Server Event.
//
//  Return Values :  Returns S_OK if everything is okay an error otherwise
//
//  Written by Terry Marles 04-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesSampleEvent::SendQSSESEvent(CAlertEvent *pQSEvent, int iSESEventID)
{
	HRESULT hr = S_OK;
	
	// check the params
	_ASSERT (pQSEvent != NULL);
	if (pQSEvent == NULL)
		return E_INVALIDARG;


	// save off a pointer to the event.
	m_pQSEvent = pQSEvent;
	
	// set the type
	m_sEventClass = SES_CLASS_QUARANTINE_SAMPLE;

	m_nSeverity = (SESSeverity)LookupSESSeverity(m_pQSEvent->dwAlertSeverityLevel);
    // set up the information for the event attributes
	
	m_iSESEventID = iSESEventID;
	
	// create the event.
	if (CreateSesEvent() != S_OK)
		return E_FAIL;
	// add in all the information for the fields of the symc_quarantine_class 
	SetGenEventFields ();
	// send the event.
	hr = QSSesGenEvent::SendQSSESEvent();
    
    
	
	return hr;
}
//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::SetGenEventFields
//
//  Purpose : This fuction will set the fields for symc_quarantine_class event class.
//			It does not override any of the agent base class fields
//
//  Parameters :
//
//    None.
//
//  Return Values :  HResult
//
//  Written By Terry Marles 04-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesSampleEvent::SetGenEventFields(void)
{
	HRESULT hr = S_OK;
	USES_CONVERSION;

	// set base class fields
 	hr = QSSesGenEvent::SetGenEventFields();

	try
	{
		if (!m_pQSEvent->sPlatformName.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_PLATFORM, (LPSTR) T2A(m_pQSEvent->sPlatformName.GetBuffer(0)));
		if (!m_pQSEvent->sFileName.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_FILENAME, (LPSTR) T2A(m_pQSEvent->sFileName.GetBuffer(0)));
		if (!m_pQSEvent->sVirusName.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_VIRUS_NAME, (LPSTR) T2A(m_pQSEvent->sVirusName.GetBuffer(0)));

		if (!m_pQSEvent->sDefinitions.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_DEFS_NEEDED, (LPSTR) T2A(m_pQSEvent->sDefinitions.GetBuffer(0)));
		if (!m_pQSEvent->sSampleStatus.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_STATUS, (LPSTR) T2A(m_pQSEvent->sSampleStatus.GetBuffer(0)));
		if (!m_pQSEvent->sAgeOfSample.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_AGE, (LPSTR) T2A(m_pQSEvent->sAgeOfSample.GetBuffer(0)));
		if (!m_pQSEvent->m_sNote.IsEmpty())
			hr = m_SesEvent->add(SES_FIELD_NOTE, (LPSTR) T2A(m_pQSEvent->m_sNote.GetBuffer(0)));
	}
	catch (...)
	{
		hr  = E_FAIL;
		_ASSERT(0);
	}
	return hr;

}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesSampleEvent::CreateSesEvent
//
//  Purpose : Verifies that there is enought information to create a Naboo Event.  It then
//			creates the event.
//
//  Parameters :	None
//
//  Return Values : HRESULT.  E_FAIL if there is a problem S_OK otherwise.
//
//  Written By:		Terry Marles 4-16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesSampleEvent::CreateSesEvent(void)
{
	USES_CONVERSION;

	HRESULT hr = S_OK;

	// check to see if the class was created with a good interface SES interface in applib.
	_ASSERT(m_pQSSesInterface != NULL);
	try 
	{
		if (m_pQSSesInterface->isInitialized() &&
			!m_sEventClass.IsEmpty() &&
			m_iSESEventID != 0 &&
			m_nSeverity != 0)
		{
			
			// create the Naboo Event.
			m_SesEvent = m_pQSSesInterface->createEvent(SESSWFID_SYMANTEC_CENTRAL_QUARANTINE___QUARANTINE_AGENT,
							m_iSESEventID,								// event id
							SES_CAT_APPLICATION,
							m_nSeverity,								// severity
							(LPSTR)T2A(m_sEventClass.GetBuffer(0)));	// event class
			if (m_SesEvent == NULL)
			{
				fWidePrintString("QSSesGenEvent::CreateSesEvent Creation of SESA Event failed.");
				hr = E_FAIL;
			}
		}
		else
		{
			if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW)
			{
				fWidePrintString("QSSesGenEvent::CreateSesEvent Creation of SESA Event failed.  AppLib not initilized, or improperly initilized QSSesGenEvent");
			}
			hr = E_FAIL;
		}
	}
	catch (...)
	{
		if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW)
		{
			fWidePrintString("QSSesGenEvent::CreateSesEvent Creation of SESA Event failed.  Try block failed creating event");
		}
		hr = E_FAIL;
		_ASSERT(0);
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : QSSesGenEvent::SetEventAttrib
//
//  Purpose : Sets up all of the attribute information for a SES Event
//
//  Parameters :
//
//    None
//
//  Return Values : HRESULT
//
//  Written by Terry Marles 04.16-01
//
//////////////////////////////////////////////////////////////////////////////////////
HRESULT QSSesSampleEvent::SetEventAttrib(void)
{
	HRESULT hr = S_OK;

	m_iSESEventID = LookupSESEventID(m_pQSEvent->dwEventId, SES2QSSampleEventTable,NO_SAMPLE_EVENT_IDS);
	if (m_iSESEventID ==0)
		return E_FAIL;
	
	m_nSeverity = (SESSeverity)LookupSESSeverity(m_pQSEvent->dwAlertSeverityLevel);

	return hr;

}
