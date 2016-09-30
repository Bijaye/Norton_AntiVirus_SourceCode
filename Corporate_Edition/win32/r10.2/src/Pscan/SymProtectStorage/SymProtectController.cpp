// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "ccLib.h"
#include "ccCoInitialize.h"
#include "ccProviderHelper.h"
#define SYMPROTECT_CONTROLLER_CCSUPPORT
#include "bbsystemeventsinterface.h"
#include "SymProtectController.h"
#include "ccproviderhelper.h"
#include "SavAssert.h"

static const int SYMPROTECT_EVENT_TIMEOUT_MS = 60000;

// ** MACROS **
#define SET_ERROR_LOCATION(varName, locationConstant)	if (varName != NULL) *varName = locationConstant

// The following macros are swiped from VPExceptionHandling to maintain SymProtectController's
// stand-aloneness.  The original names were prefixed with VP_ and have been renamed to clarify
// that this is a copy.
/** This file defines a macro for catching memory exceptions.
  * Include this header after including any MFC/ATL header files.
  * See CATCH_MEMORYEXCEPTIONS below.
  */

// Some helper macros to simplify defining CATCH_MEMORYEXCEPTIONS.
// You probably should not use these.
#ifdef __AFX_H__
    #define CATCH_CMEMORYEXCEPTION(mReturnLogic) \
        catch (CMemoryException *pMemException)     \
        {                                           \
            pMemException->Delete();                \
            mReturnLogic                            \
        }
#else
    #define CATCH_CMEMORYEXCEPTION(mReturnLogic)
#endif

#ifdef __ATLEXCEPT_H__
    #define CATCH_CATLEXCEPTION(mReturnLogic)                \
        catch (ATL::CAtlException &eATLException)                    \
        {                                                       \
            /* If this is not a memory exception, re-throw. */  \
            if (eATLException.m_hr != E_OUTOFMEMORY)            \
                throw;                                          \
            mReturnLogic                                        \
        }
#else
    #define CATCH_CATLEXCEPTION(mReturnLogic)
#endif

#ifdef _INC_COMDEF
    #define CATCH__COM_ERROR(mReturnLogic)                   \
        catch (_com_error &eCOMError)                           \
        {                                                       \
            /* If this is not a memory exception, re-throw. */  \
            if (eCOMError.Error() != E_OUTOFMEMORY)             \
                throw;                                          \
            mReturnLogic                                        \
        }
#else
    #define CATCH__COM_ERROR(mReturnLogic)
#endif

/** This macro should catch any kind of memory exception that can be thrown by
  * code in a particular source file.  Don't count on this macro to catch
  * memory exceptions thrown by nested function calls, unless those nested
  * functions are in source files that make the same use of MFC/ATL.
  * @param mReturnLogic This parameter contains source code (such as a return
  * call) you wish to be run when a memory exception is caught.  To do nothing,
  * specify ";" (to avoid a compiler warning).
  */
#define CATCH_MEMORYEXCEPTIONS(mReturnLogic) \
    CATCH_CMEMORYEXCEPTION(mReturnLogic)     \
    CATCH_CATLEXCEPTION(mReturnLogic)        \
    CATCH__COM_ERROR(mReturnLogic)           \
    catch (std::bad_alloc &)                    \
    {                                           \
        mReturnLogic                            \
    }

HRESULT CSymProtectController::Initialize( ErrorLocation* errorLocation )
// Initialize the controller.  Returns S_OK on success, S_FALSE if already initialized, else the error code of the failure.
{
	HRESULT			returnValHR		= E_FAIL;
	SYMRESULT		returnValSR		= SYMERR_UNKNOWN;

    returnValHR = ccLib::CCoInitialize::CoInitialize(ccLib::CCoInitialize::eMTAModel);
	if (SUCCEEDED(returnValHR))
	{
		returnValSR = eventFactoryLoader.CreateObject(&eventFactory);
		if (SYM_SUCCEEDED(returnValSR))
		{
			returnValSR = proxyFactoryLoader.CreateObject(&proxyFactory);
			if (SYM_SUCCEEDED(returnValSR))
			{
				if (eventManager.Create(eventFactory, proxyFactory))
				{
					if (providerHelper.Create(&eventManager, TRUE))
					{
						returnValSR = SYM_OK;
					}
					else
					{
						returnValSR = SYMERR_UNKNOWN;
                        SET_ERROR_LOCATION(errorLocation, ErrorLocation_ProviderHelperCreate);
					}
				}
				else
				{
					returnValSR = SYMERR_UNKNOWN;
					SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventManagerCreate);
				}
			}
			else
			{
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_ProxyFactoryCreate);
			}
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocaton_EventFactoryCreate);
		}

		if (SYM_SUCCEEDED(returnValSR))
			returnValHR = S_OK;
		else
			returnValHR = SYMRESULT_TO_HRESULT(returnValSR);
	}

	// Set state, return result
	if (SUCCEEDED(returnValHR))
		initialized = true;
	else
		Shutdown();
	return returnValHR;
}

HRESULT CSymProtectController::Shutdown( ErrorLocation* errorLocation )
// Shuts down the controller.  Returns S_OK on success, S_FALSE if already shutdown, else the error code of the failure.
{
	if (!initialized)
		return S_FALSE;

	initialized = false;
    providerHelper.Destroy();
    eventManager.Destroy();
    proxyFactory.Release();
	eventFactory.Release();
	SET_ERROR_LOCATION(errorLocation, ErrorLocation_None);

	return S_OK;
}

HRESULT CSymProtectController::ManifestRefresh( ErrorLocation* errorLocation )
// Ask SymProtect to refresh it's manifests
{
	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	return SendEvent(SymProtectEvt::CManifestChangeEventEx::TypeId, timeoutDefault, errorLocation);
}

// Core function to send a ccEvt event and wait for it to be canceled
HRESULT CSymProtectController::SendSPEvent(ccEvtMgr::CEventExPtr pEvent, 
                                        ccEvtMgr::CEventEx** ppReturnEvent,
                                        ErrorLocation* errorLocation)
{
    // Send the event and wait for it to return
    long nEventId = 0;
    ccEvtMgr::CError::ErrorType res;
    ccEvtMgr::CEventExPtr pReturnEventPlaceholder;
    res = providerHelper.CreateEvent(*pEvent, false, nEventId, NULL, 
                                     SYMPROTECT_EVENT_TIMEOUT_MS, TRUE, 
                                     pReturnEventPlaceholder.m_p);
    if( ccEvtMgr::CError::eNoError != res )
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
        providerHelper.Destroy();
        return CERROR_TO_HRESULT(res);
    }

    // If the caller wants the resulting event back...
    if( ppReturnEvent )
    {
        // Make an addref'd copy
        *ppReturnEvent = pReturnEventPlaceholder;
        (*ppReturnEvent)->AddRef();
    }

    return S_OK;
}


HRESULT CSymProtectController::SetBBSystemEnabled(bool bEnable, ErrorLocation* errorLocation)
{
    ccEvtMgr::CEventExPtr pEvent;
    eventManager.NewEvent(bbSystemEvt::ISymBBStateSetEvent::TypeId,
                                  pEvent.m_p);
    if( !pEvent )
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
        return E_UNEXPECTED;
    }

    bbSystemEvt::ISymBBStateSetEventQIPtr pSetEvent(pEvent);
    if( !pSetEvent )
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
        ASSERT(pSetEvent);
        return E_UNEXPECTED;
    }

    SYMRESULT res = SYMERR_UNKNOWN;
    if( bEnable )
        res = pSetEvent->Enable();
    else
        res = pSetEvent->Disable();
    if( SYM_FAILED(res) )
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_SendEvent);
        return SYMRESULT_TO_HRESULT(res);
    }

    ccEvtMgr::CEventExPtr pReturnEvent;
    if( SendSPEvent(pSetEvent.m_p, &pReturnEvent, errorLocation) )
    {
        bbSystemEvt::ISymBBStateSetEventQIPtr pReturnedSetEvent(pReturnEvent);
        ASSERT(pReturnedSetEvent);
        if( !pReturnedSetEvent )
        {
            SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
            return E_UNEXPECTED;
        }

        SymProtectEvt::ISymBBSettingsEvent::ErrorTypes eError;
        if( SYM_SUCCEEDED(pReturnedSetEvent->GetResult(eError)) &&
            eError != SymProtectEvt::ISymBBSettingsEvent::UNRECOVERABLE_ERROR )
        {
            // Print a warning if there was any error recovery
            if( eError == SymProtectEvt::ISymBBSettingsEvent::RECOVERED_FROM_ERROR )
            {
                SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
            }

            return S_OK;
        }
        else
        {
            SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
            return E_UNEXPECTED;
        }
    }
    else
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
        return E_UNEXPECTED;
    }
}


HRESULT CSymProtectController::ValidateSettings( ErrorLocation* errorLocation )
// Ask SymProtect to validate it's settings
{
	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	return SendEvent(SymProtectEvt::ISymBBSettingsValidationEvent::TypeId, timeoutDefault, errorLocation);
}

HRESULT CSymProtectController::ForceRefreshSettings( ErrorLocation* errorLocation )
// Ask SymProtect to force reload all of it's settings
{
	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	return SendEvent(SymProtectEvt::ISymBBSettingsForceRefreshEvent::TypeId, timeoutDefault, errorLocation);
}

HRESULT CSymProtectController::SetSymProtectAndBehaviorBlockingEnabled( bool protectionEnabled, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
    SymProtectEvt::IBBSymProtectSetConfigEventPtr       setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

    returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(SymProtectEvt::IBBSymProtectSetConfigEvent::TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
        returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_SymProtectSetConfigEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->SetSymProtectComponentState(protectionEnabled));
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	// Cleanup and return
	return returnValHR;
}

HRESULT CSymProtectController::GetSymProtectAndBehaviorBlockingEnabled( bool* protectionEnabled, ErrorLocation* errorLocation  )
{
    SymProtectEvt::IBBSymProtectQueryConfigEventPtr     queryResponseConfigPtr;
	bool												componentState				= false;
	bool												errorOccuredTemp            = false;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
    if (protectionEnabled == NULL)
        return E_POINTER;

    returnValHR = SendQueryConfigEvent(&queryResponseConfigPtr, errorLocation);
	if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(queryResponseConfigPtr->GetSymProtectComponentState(componentState, errorOccuredTemp));
		if (SUCCEEDED(returnValHR))
		{
			*protectionEnabled = componentState;
			returnValHR = S_OK;
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
		}
	}

	// Cleanup and return
	return returnValHR;
}

HRESULT CSymProtectController::GetErrorFlag( bool* errorOccured, ErrorLocation* errorLocation )
{
    SymProtectEvt::IBBSymProtectQueryConfigEventPtr     queryResponseConfigPtr;
    bool                                                componentState              = false;
    bool                                                errorOccuredTemp            = false;
    HRESULT                                             returnValHR                 = E_FAIL;

    // Validate state and parameter
    if (!initialized)
        return E_UNEXPECTED;
    if (errorOccured == NULL)
        return E_POINTER;

    returnValHR = SendQueryConfigEvent(&queryResponseConfigPtr, errorLocation);
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = SYMRESULT_TO_HRESULT(queryResponseConfigPtr->GetSymProtectComponentState(componentState, errorOccuredTemp));
        if (SUCCEEDED(returnValHR))
        {
            *errorOccured = errorOccuredTemp;
            returnValHR = S_OK;
        }
        else
        {
            SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
        }
    }

    // Cleanup and return
    return returnValHR;
}

HRESULT CSymProtectController::TemporaryDisable( DWORD noMinutes, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBSymProtectSetConfigEventPtr		setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(SymProtectEvt::IBBSymProtectSetConfigEvent::TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_SymProtectSetConfigEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->DisableSymProtect(noMinutes));
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	// Cleanup and return
	return returnValHR;
}

HRESULT CSymProtectController::IsTemporarillyDisabled( bool* isTemporaryDisable, DWORD* minutesRemaining, ErrorLocation* errorLocation )
{
	SymProtectEvt::IBBSymProtectQueryConfigEventPtr		queryResponseConfigPtr;
	unsigned int										actualMinutesRemaining		= 0;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameters
	if (!initialized)
		return E_UNEXPECTED;
    if ((isTemporaryDisable == NULL) && (minutesRemaining == NULL))
        return E_POINTER;

	returnValHR = SendQueryConfigEvent(&queryResponseConfigPtr, errorLocation);
	if (SUCCEEDED(returnValHR))
	{
        if (isTemporaryDisable != NULL)
    		returnValHR = SYMRESULT_TO_HRESULT(queryResponseConfigPtr->IsSymProtectTemporarilyDisabled(*isTemporaryDisable));
		if (SUCCEEDED(returnValHR))
		{
            if (minutesRemaining != NULL)
            {
                returnValHR = SYMRESULT_TO_HRESULT(queryResponseConfigPtr->GetDisabledTimeRemaining(actualMinutesRemaining));
                if (SUCCEEDED(returnValHR))
        			*minutesRemaining = (DWORD) actualMinutesRemaining;
            }
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
		}
	}

	return returnValHR;
}

HRESULT CSymProtectController::CancelTemporaryDisable( ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBSymProtectSetConfigEventPtr		setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(SymProtectEvt::IBBSymProtectSetConfigEvent::TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_SymProtectSetConfigEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->CancelDisableSymProtect());
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	// Cleanup and return
	return returnValHR;
}

HRESULT CSymProtectController::GetState( SymProtectEvt::CQueryStateEventEx::SP_QUERY_STATE* currState, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr                               queryEvent;
    ccEvtMgr::CEventExPtr                               queryResponsePtr;
    SymProtectEvt::CQueryStateEventExPtr                queryResponseStateEventPtr;
    HRESULT                                             returnValHR                 = E_FAIL;

    // Validate state and parameter
    if (!initialized)
        return E_UNEXPECTED;
	if (currState == NULL)
		return E_POINTER;

    returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(queryResponseStateEventPtr->TypeId, queryEvent.m_p));
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = SendEvent(queryEvent, &queryResponsePtr, false, NULL, timeoutQuery, errorLocation);
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = SYMRESULT_TO_HRESULT(queryResponsePtr->QueryInterface(SymProtectEvt::IID_QueryStateEvent, (void**) &queryResponseStateEventPtr));
            if (SUCCEEDED(returnValHR))
            {
                returnValHR = SYMRESULT_TO_HRESULT(queryResponseStateEventPtr->GetSymProtectState(*currState));
                if (SUCCEEDED(returnValHR))
                    returnValHR = S_OK;
                else
                    SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
            }
            else
            {
                SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_ConvertEvent);
            }
        }
    }
    else
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
    }
    return returnValHR;
}


// ** Protection level settings **
HRESULT CSymProtectController::SetFileProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBSymProtectSetConfigEventPtr		setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(SymProtectEvt::IBBSymProtectSetConfigEvent::TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_SymProtectSetConfigEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->SetFileProtectionLevel(protectionLevel));
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::SetRegistryProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBSymProtectSetConfigEventPtr		setEventConfigPtr;
	SymProtectEvt::SP_PROTECTION_LEVEL					spLevel						= SymProtectEvt::PROTECTION_ENABLED;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(SymProtectEvt::IBBSymProtectSetConfigEvent::TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_SymProtectSetConfigEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->SetRegistryProtectionLevel(protectionLevel));
			if (SUCCEEDED(returnValHR))
                returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::SetProcessProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBSymProtectSetConfigEventPtr		setEventConfigPtr;
	SymProtectEvt::SP_PROTECTION_LEVEL					spLevel						= SymProtectEvt::PROTECTION_ENABLED;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(SymProtectEvt::IBBSymProtectSetConfigEvent::TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_SymProtectSetConfigEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->SetProcessProtectionLevel(protectionLevel));
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::SetNamedObjectProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr                               setEvent;
    SymProtectEvt::IBBSymProtectSetConfigEvent2Ptr      setEventConfig2Ptr;
    SymProtectEvt::SP_PROTECTION_LEVEL                  spLevel                     = SymProtectEvt::PROTECTION_ENABLED;
    HRESULT                                             returnValHR                 = E_FAIL;

    // Validate state
    if (!initialized)
        return E_UNEXPECTED;

    returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(SymProtectEvt::IBBSymProtectSetConfigEvent2::TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_SymProtectSetConfigEvent2, (void**) &setEventConfig2Ptr));
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = SYMRESULT_TO_HRESULT(setEventConfig2Ptr->SetNamedObjectProtectionLevel(protectionLevel));
            if (SUCCEEDED(returnValHR))
                returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
            else
                SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
        }
        else
        {
            SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
        }
    }
    else
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
    }

    return returnValHR;
}

HRESULT CSymProtectController::SetAllProtection( bool spAndBBenabled, SymProtectEvt::SP_PROTECTION_LEVEL fileProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL registryProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL processProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL namedObjectProtectionLevel, ErrorLocation* errorLocation )
// Sets all protection levels at once
{
    ccEvtMgr::CEventExPtr                               setEvent;
    SymProtectEvt::IBBSymProtectSetConfigEvent2Ptr      setEventConfig2Ptr;
    SymProtectEvt::SP_PROTECTION_LEVEL                  spLevel                     = SymProtectEvt::PROTECTION_ENABLED;
    HRESULT                                             returnValHR                 = E_FAIL;

    // Validate state
    if (!initialized)
        return E_UNEXPECTED;

    returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(SymProtectEvt::IBBSymProtectSetConfigEvent2::TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_SymProtectSetConfigEvent2, (void**) &setEventConfig2Ptr));
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = SYMRESULT_TO_HRESULT(setEventConfig2Ptr->SetSymProtectComponentState(spAndBBenabled));
            if (SUCCEEDED(returnValHR))
                returnValHR = SYMRESULT_TO_HRESULT(setEventConfig2Ptr->SetFileProtectionLevel(fileProtectionLevel));
            if (SUCCEEDED(returnValHR))
                returnValHR = SYMRESULT_TO_HRESULT(setEventConfig2Ptr->SetRegistryProtectionLevel(registryProtectionLevel));
            if (SUCCEEDED(returnValHR))
                returnValHR = SYMRESULT_TO_HRESULT(setEventConfig2Ptr->SetProcessProtectionLevel(processProtectionLevel));
            if (SUCCEEDED(returnValHR))
                returnValHR = SYMRESULT_TO_HRESULT(setEventConfig2Ptr->SetNamedObjectProtectionLevel(namedObjectProtectionLevel));
            if (SUCCEEDED(returnValHR))
                returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
            else
                SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
        }
        else
        {
            SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
        }
    }
    else
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
    }

    return returnValHR;
}

HRESULT CSymProtectController::GetFileProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation )
{
	SymProtectEvt::IBBSymProtectQueryConfigEventPtr		queryResponseConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (protectionLevel == NULL)
		return E_POINTER;

	returnValHR = SendQueryConfigEvent(&queryResponseConfigPtr, errorLocation);
	if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(queryResponseConfigPtr->GetFileProtectionLevel(*protectionLevel));
		if (SUCCEEDED(returnValHR))
			returnValHR = S_OK;
		else
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
	}

	// Cleanup and return
	return returnValHR;
}

HRESULT CSymProtectController::GetRegistryProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation )
{
	SymProtectEvt::IBBSymProtectQueryConfigEventPtr		queryResponseConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (protectionLevel == NULL)
		return E_POINTER;

	returnValHR = SendQueryConfigEvent(&queryResponseConfigPtr, errorLocation);
	if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(queryResponseConfigPtr->GetRegistryProtectionLevel(*protectionLevel));
		if (SUCCEEDED(returnValHR))
			returnValHR = S_OK;
		else
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
	}

	// Cleanup and return
	return returnValHR;
}

HRESULT CSymProtectController::GetProcessProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation )
{
	SymProtectEvt::IBBSymProtectQueryConfigEventPtr		queryResponseConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (protectionLevel == NULL)
		return E_POINTER;

	returnValHR = SendQueryConfigEvent(&queryResponseConfigPtr, errorLocation);
	if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(queryResponseConfigPtr->GetProcessProtectionLevel(*protectionLevel));
		if (SUCCEEDED(returnValHR))
			returnValHR = S_OK;
		else
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
	}

	// Cleanup and return
	return returnValHR;
}

HRESULT CSymProtectController::GetNamedObjectProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation )
{
    SymProtectEvt::IBBSymProtectQueryConfigEvent2Ptr    queryResponseConfig2Ptr;
    HRESULT                                             returnValHR                 = E_FAIL;

    // Validate state and parameter
    if (!initialized)
        return E_UNEXPECTED;
    if (protectionLevel == NULL)
        return E_POINTER;

    returnValHR = SendQueryConfigEvent2(&queryResponseConfig2Ptr, errorLocation);
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = SYMRESULT_TO_HRESULT(queryResponseConfig2Ptr->GetNamedObjectProtectionLevel(*protectionLevel));
        if (SUCCEEDED(returnValHR))
            returnValHR = S_OK;
        else
            SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
    }

    // Cleanup and return
    return returnValHR;
}

// ** Authorizations **
HRESULT CSymProtectController::AuthorizationUserAdd( LPCTSTR accountName, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBAuthorizedUsersSetEventPtr		setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;
	_bstr_t												wcConverter;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (accountName == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(setEventConfigPtr->TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedUsersSetEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
            try
            {
    			wcConverter = accountName;
    			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->AddUser((const wchar_t*) wcConverter));
            }
            CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationUserDelete( LPCTSTR accountName, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBAuthorizedUsersSetEventPtr		setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;
	_bstr_t												wcConverter;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (accountName == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(setEventConfigPtr->TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedUsersSetEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
            try
            {
    			wcConverter = accountName;
    			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->DeleteUser((const wchar_t*) wcConverter));
            }
            CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationUserEnum( StringList* accounts, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								queryEvent;
    ccEvtMgr::CEventExPtr								queryResponsePtr;
	SymProtectEvt::IBBAuthorizedUsersQueryEventPtr		queryResponseUsersPtr;
	ULONG												noUsers						= 0;
	ULONG												currUserNo					= 0;
	const wchar_t*										username					= NULL;
	_bstr_t												wcConverter;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (accounts == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(queryResponseUsersPtr->TypeId, queryEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SendEvent(queryEvent, &queryResponsePtr, true, NULL, timeoutQuery, errorLocation);
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(queryResponsePtr->QueryInterface(SymProtectEvt::IID_BBAuthorizedUsersQueryEvent, (void**) &queryResponseUsersPtr));
			if (SUCCEEDED(returnValHR))
			{
				returnValHR = SYMRESULT_TO_HRESULT(queryResponseUsersPtr->GetUserCount(noUsers));
				if (SUCCEEDED(returnValHR))
				{
					accounts->clear();
					for(currUserNo = 0; (currUserNo < noUsers) && SUCCEEDED(returnValHR); currUserNo++)
					{
						returnValHR = SYMRESULT_TO_HRESULT(queryResponseUsersPtr->GetUser(currUserNo, &username));
						if (SUCCEEDED(returnValHR))
						{
                            try
                            {
    							wcConverter = username;
    							accounts->push_back((LPCTSTR) wcConverter);
                            }
                            CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
							// Username now points to SymProtect's string - do NOT attempt to free it
							username = NULL;
						}
					}
				}
				if (SUCCEEDED(returnValHR))
					returnValHR = S_OK;
				else
					SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
			}
			else
			{
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_ConvertEvent);
			}
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}
	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationHashAdd( LPCTSTR name, ULONG method, BYTE* hashData, DWORD hashDataSize, DWORD fileSize, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBAuthorizedHashesSetEventPtr		setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;
	_bstr_t												wcConverter;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if ((name == NULL) || (hashData == NULL))
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(setEventConfigPtr->TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedHashesSetEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
            try
            {
    			wcConverter = name;
    			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->AddHash((LPWSTR) wcConverter, method, hashData, hashDataSize, fileSize));
            }
            CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationHashDelete( LPCTSTR name, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBAuthorizedHashesSetEventPtr		setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;
	_bstr_t												wcConverter;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (name == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(setEventConfigPtr->TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedHashesSetEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
            try
            {
    			wcConverter = name;
    			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->DeleteHash((LPWSTR) wcConverter));
            }
            CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationHashEnum( HashInfoList* hashes, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								queryEvent;
    ccEvtMgr::CEventExPtr								queryResponsePtr;
	SymProtectEvt::IBBAuthorizedHashesQueryEventPtr		queryResponseHashesPtr;
	ULONG												noHashes					= 0;
	ULONG												currHashNo					= 0;
	ULONG												hashMethod					= 0;
	const wchar_t*										hashName					= NULL;
	_bstr_t												wcConverter;
	const void*											hashData					= NULL;
	ULONG												hashDataSize				= 0;
	ULONG												hashFileSize				= 0;
	HashInfo											newHashInfo;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (hashes == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(queryResponseHashesPtr->TypeId, queryEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SendEvent(queryEvent, &queryResponsePtr, true, NULL, timeoutQuery, errorLocation);
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(queryResponsePtr->QueryInterface(SymProtectEvt::IID_BBAuthorizedHashesQueryEvent, (void**) &queryResponseHashesPtr));
			if (SUCCEEDED(returnValHR))
			{
				returnValHR = SYMRESULT_TO_HRESULT(queryResponseHashesPtr->GetHashCount(noHashes));
				if (SUCCEEDED(returnValHR))
				{
					hashes->clear();
					for(currHashNo = 0; (currHashNo < noHashes) && SUCCEEDED(returnValHR); currHashNo++)
					{
						returnValHR = SYMRESULT_TO_HRESULT(queryResponseHashesPtr->GetHash(currHashNo, hashMethod, &hashName, &hashData, hashDataSize, hashFileSize));
						if (SUCCEEDED(returnValHR))
						{
                            try
                            {
                                wcConverter					= hashName;
                                newHashInfo.description		= (LPCTSTR) wcConverter;
                                newHashInfo.method          = hashMethod;
                                newHashInfo.fileSize        = hashFileSize;
                                newHashInfo.hashData        = new(std::nothrow) BYTE[hashDataSize];
                                if (newHashInfo.hashData != NULL)
                                {
                                    memcpy(newHashInfo.hashData, hashData, hashDataSize);
                                    newHashInfo.hashDataSize = hashDataSize;
                                    hashes->push_back(newHashInfo);
                                }
                                else
                                {
                                    returnValHR = E_OUTOFMEMORY;
                                }
                                // hashName and hashData point to internal SymProtect buffers - do NOT attempt to free
                            }
                            CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
						}
					}
				}
				if (SUCCEEDED(returnValHR))
				{
					returnValHR = S_OK;
				}
				else
				{
					SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
					hashes->clear();
				}
			}
			else
			{
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_ConvertEvent);
			}
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}
	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationLocationAdd( LPCTSTR pathname, bool isFile, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBAuthorizedLocationsSetEventPtr	setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;
	_bstr_t												wcConverter;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (pathname == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(setEventConfigPtr->TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedLocationsSetEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
            try
            {
    			wcConverter = pathname;
    			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->AddLocation((LPWSTR) wcConverter, isFile));
    			if (SUCCEEDED(returnValHR))
    				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
            }
            CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
            if (FAILED(returnValHR))
                SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationLocationDelete( LPCTSTR pathname, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBAuthorizedLocationsSetEventPtr	setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;
	_bstr_t												wcConverter;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (pathname == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(setEventConfigPtr->TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedLocationsSetEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
            try
            {
    			wcConverter = pathname;
    			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->DeleteLocation((LPWSTR) wcConverter));
    			if (SUCCEEDED(returnValHR))
    				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
             }
             CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
             if (FAILED(returnValHR))
                SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationLocationEnum( LocationAuthorizationList* locations, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								queryEvent;
    ccEvtMgr::CEventExPtr								queryResponsePtr;
	SymProtectEvt::IBBAuthorizedLocationsQueryEventPtr	queryResponseLocationsPtr;
	ULONG												noLocations					= 0;
	ULONG												currLocationNo				= 0;
	const wchar_t*										path						= NULL;
	_bstr_t												wcConverter;
	bool												isFile						= false;
	LocationAuthorization								newLocation;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (locations == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(queryResponseLocationsPtr->TypeId, queryEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SendEvent(queryEvent, &queryResponsePtr, true, NULL, timeoutQuery, errorLocation);
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(queryResponsePtr->QueryInterface(SymProtectEvt::IID_BBAuthorizedLocationsQueryEvent, (void**) &queryResponseLocationsPtr));
			if (SUCCEEDED(returnValHR))
			{
				returnValHR = SYMRESULT_TO_HRESULT(queryResponseLocationsPtr->GetLocationCount(noLocations));
				if (SUCCEEDED(returnValHR))
				{
					locations->clear();
					for(currLocationNo = 0; (currLocationNo < noLocations) && SUCCEEDED(returnValHR); currLocationNo++)
					{
						returnValHR = SYMRESULT_TO_HRESULT(queryResponseLocationsPtr->GetLocation(currLocationNo, &path, isFile));
						if (SUCCEEDED(returnValHR))
						{
                            try
                            {
    							wcConverter					= path;
    							newLocation.pathname		= (LPCTSTR) wcConverter;
    							newLocation.isDirectory		= !isFile;
    							// pathname points to internal an SymProtect buffer - do NOT attempt to free
    							locations->push_back(newLocation);
                            }
                            CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
						}
					}
				}
				if (SUCCEEDED(returnValHR))
				{
					returnValHR = S_OK;
				}
				else
				{
					locations->clear();
					SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
				}
			}
			else
			{
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_ConvertEvent);
			}
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}
	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationDeviceAdd( ULONG deviceID, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBAuthorizedDevicesSetEventPtr		setEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(setEventConfigPtr->TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedDevicesSetEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->SetDeviceTypeAuthorization(deviceID, true));
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationDeviceDelete( ULONG deviceID, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								setEvent;
	SymProtectEvt::IBBAuthorizedDevicesSetEventPtr		setEventConfigPtr;
	ULONG												spDeviceID					= 0;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(setEventConfigPtr->TypeId, setEvent.m_p));
    if (SUCCEEDED(returnValHR))
	{
		returnValHR = SYMRESULT_TO_HRESULT(setEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedDevicesSetEvent, (void**) &setEventConfigPtr));
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(setEventConfigPtr->SetDeviceTypeAuthorization(deviceID, false));
			if (SUCCEEDED(returnValHR))
				returnValHR = SendEvent(setEvent, NULL, true, NULL, timeoutSet, errorLocation);
			else
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_EventInteraction);
		}
		else
		{
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_ConvertEvent);
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::AuthorizationDeviceGet( ULONG deviceID, bool* authorized, ErrorLocation* errorLocation )
{
    ccEvtMgr::CEventExPtr								queryEvent;
    ccEvtMgr::CEventExPtr								responseEvent;
	SymProtectEvt::IBBAuthorizedDevicesQueryEventPtr	responseEventConfigPtr;
	HRESULT												returnValHR					= E_FAIL;

	// Validate state and parameter
	if (!initialized)
		return E_UNEXPECTED;
	if (authorized == NULL)
		return E_POINTER;

	returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(responseEventConfigPtr->TypeId, queryEvent.m_p));
	if (SUCCEEDED(returnValHR))
	{
		returnValHR = SendEvent(queryEvent, &responseEvent, true, NULL, timeoutQuery, errorLocation);
		if (SUCCEEDED(returnValHR))
		{
			returnValHR = SYMRESULT_TO_HRESULT(queryEvent->QueryInterface(SymProtectEvt::IID_BBAuthorizedDevicesQueryEvent, (void**) &responseEventConfigPtr));
			if (SUCCEEDED(returnValHR))
			{
				returnValHR = SYMRESULT_TO_HRESULT(responseEventConfigPtr->GetDeviceTypeAuthorization(deviceID, *authorized));
				if (SUCCEEDED(returnValHR))
					returnValHR = S_OK;
				else
					SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_EventInteraction);
			}
			else
			{
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_ConvertEvent);
			}
		}
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}

	return returnValHR;
}

HRESULT CSymProtectController::SendEvent( ccEvtMgr::CEventExPtr eventToSend, ccEvtMgr::CEventEx** eventResponse, bool validateResponse, long* responseEventType, DWORD timeout, ErrorLocation* errorLocation )
// Sends an event to SymProtect and waits for a response
// On success, sets *eventResponse equal to a copy of the response event, *responseEventType to the type ID of the response event.
// If validateResponse is TRUE, looks for an ISymBBSettingsEvent-compatible response event and verifies that the error code is != UNRECOVERABLE_ERROR
// Return code varies based on validateResonse.  If FALSE, returns S_OK if successfully sends event.  If TRUE, returns S_OK if response event supports
// ISymBBSettingsEvent and the error code is != UNRECOVERABLE_ERROR, else returns E_FAIL.
{
	ccEvtMgr::CEventExPtr							eventResponseTemp;
	SymProtectEvt::ISymBBSettingsEventPtr			eventResponseSettingsPtr;
	long											actualResponseEventType		= 0;
	SymProtectEvt::ISymBBSettingsEvent::ErrorTypes	errorCode;
	HRESULT											returnValHR					= E_FAIL;

	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	// Send the event
    returnValHR = CERROR_TO_HRESULT(providerHelper.CreateEvent(*eventToSend, false, actualResponseEventType, NULL, timeout, TRUE, eventResponseTemp.m_p));
	if (SUCCEEDED(returnValHR))
    {
		// Copy out the results, as requested
		returnValHR = S_OK;
		if (responseEventType != NULL)
			*responseEventType = actualResponseEventType;
		if (eventResponse != NULL)
		{
			*eventResponse = eventResponseTemp;
			(*eventResponse)->AddRef();
		}

		if (validateResponse)
		{
			returnValHR = eventResponseTemp->QueryInterface(SymProtectEvt::IID_BBSettingsEvent, (void**) &eventResponseSettingsPtr);
			if (SUCCEEDED(returnValHR))
			{
				returnValHR = SYMRESULT_TO_HRESULT(eventResponseSettingsPtr->GetResult(errorCode));
				if (SUCCEEDED(returnValHR))
				{
					if (errorCode != SymProtectEvt::ISymBBSettingsEvent::UNRECOVERABLE_ERROR)
						returnValHR = S_OK;
					else
						returnValHR = E_FAIL;
				}
				else
				{
					SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_Validate_GetResult);
				}
			}
			else
			{
				SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_Validate_ConvertEvent);
			}
		}
    }
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_SendEvent);
	}
    return returnValHR;
}

HRESULT CSymProtectController::SendEvent( long eventID, DWORD timeout, ErrorLocation* errorLocation )
// Sends a new event object of the specified type to SymProtect, no response wait
{
	ccEvtMgr::CEventEx*				sentEvent		= NULL;
	ccEvtMgr::CEventEx*				responseEvent	= NULL;
	HRESULT							returnValHR		= E_FAIL;

	// Validate state
	if (!initialized)
		return E_UNEXPECTED;

	// Create the event
    returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(eventID, sentEvent));
	// Send the event
	if (SUCCEEDED(returnValHR))
	{
		returnValHR = CERROR_TO_HRESULT(providerHelper.CreateEvent(*sentEvent, false, eventID, NULL, timeout, TRUE, responseEvent));
		if (SUCCEEDED(returnValHR))
			returnValHR = S_OK;
		else
			SET_ERROR_LOCATION(errorLocation, ErrorLocation_SendEvent);
	}
	else
	{
		SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
	}


    // Shutdown and return
    if (responseEvent != NULL)
        eventManager.DeleteEvent(responseEvent);
    eventManager.DeleteEvent(sentEvent);

    return returnValHR;
}

HRESULT CSymProtectController::SendQueryConfigEvent( SymProtectEvt::IBBSymProtectQueryConfigEvent** queryResponse, ErrorLocation* errorLocation )
// Sends a IBBSymProtectQueryConfigEvent event to SymProtect and gets the response
// Returns S_OK on success, else the error code of the failure.
{
    return SendQueryEvent<SymProtectEvt::IBBSymProtectQueryConfigEvent, SymProtectEvt::IID_SymProtectQueryConfigEvent>(queryResponse, errorLocation);
}

HRESULT CSymProtectController::SendQueryConfigEvent2( SymProtectEvt::IBBSymProtectQueryConfigEvent2** queryResponse, ErrorLocation* errorLocation )
// Sends a IBBSymProtectQueryConfigEvent2 event to SymProtect and gets the response
// Returns S_OK on success, else the error code of the failure.
{
    return SendQueryEvent<SymProtectEvt::IBBSymProtectQueryConfigEvent2, SymProtectEvt::IID_SymProtectQueryConfigEvent2>(queryResponse, errorLocation);
}

template<class QueryClass, const SYMGUID& iid> HRESULT CSymProtectController::SendQueryEvent( QueryClass** queryResponse, ErrorLocation* errorLocation )
// Sends a QueryClass-type event to SymProtect and returns a response of the same type
// iid is the interface ID corresponding to QueryClass.  This is typically a constant named IID_QueryClass
// Returns S_OK on success, else the error code of the failure.
{
    ccEvtMgr::CEventExPtr                               queryEvent;
    ccEvtMgr::CEventExPtr                               responseEvent;
    HRESULT                                             returnValHR                 = E_FAIL;

    // Validate state and parameter
    if (!initialized)
        return E_UNEXPECTED;
    if (queryResponse == NULL)
        return E_POINTER;

    returnValHR = CERROR_TO_HRESULT(eventManager.NewEvent(QueryClass::TypeId, queryEvent.m_p));
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = SendEvent(queryEvent, &responseEvent, true, NULL, timeoutQuery, errorLocation);
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = SYMRESULT_TO_HRESULT(responseEvent->QueryInterface(iid, (void**) queryResponse));
            if (SUCCEEDED(returnValHR))
                returnValHR = S_OK;
            else
                SET_ERROR_LOCATION(errorLocation, ErrorLocation_Response_ConvertEvent);
        }
    }
    else
    {
        SET_ERROR_LOCATION(errorLocation, ErrorLocation_NewEvent);
    }

    // Cleanup and return
    return returnValHR;
}

HRESULT CSymProtectController::CERROR_TO_HRESULT( ccEvtMgr::CError::ErrorType originalCode )
// Translates a CError code to an equivalent Windows HRESULT
{
	switch (originalCode)
	{
	case ccEvtMgr::CError::eBadParameterError:
		return E_INVALIDARG;
	case ccEvtMgr::CError::eBusyError:
		return HRESULT_FROM_WIN32(ERROR_BUSY);
	case ccEvtMgr::CError::eCancelledError:
		return HRESULT_FROM_WIN32(ERROR_REQUEST_ABORTED);
	case ccEvtMgr::CError::eFatalError:
		return E_FAIL;
	case ccEvtMgr::CError::eNoError:
		return S_OK;
	case ccEvtMgr::CError::eNotImplementedError:
		return E_NOTIMPL;
	case ccEvtMgr::CError::eOutOfMemoryError:
		return E_OUTOFMEMORY;
	case ccEvtMgr::CError::eUnknownError:
	default:
		return E_FAIL;
	}
}

HRESULT CSymProtectController::SYMRESULT_TO_HRESULT( SYMRESULT originalCode )
// Translates a SymResult code to an equivalent Windows HRESULT
{
	switch (originalCode)
	{
	case SYM_OK:
		return S_OK;
	case SYM_FALSE:
		return S_FALSE;
	case SYMERR_NOINTERFACE:
		return E_NOINTERFACE;
	case SYMERR_OUTOFMEMORY:
		return E_OUTOFMEMORY;
	case SYMERR_NOTIMPLEMENTED:
		return E_NOTIMPL;
	case SYMERR_INVALIDARG:
		return E_INVALIDARG;
	case SYMERR_ACCESSDENIED:
		return E_ACCESSDENIED;
	case SYMERR_UNKNOWN:
		return E_FAIL;
	case SYMERR_PATH_NOT_FOUND:
		return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
	case SYMERR_MISSING_EXPORTS:
		return HRESULT_FROM_WIN32(ERROR_STATIC_INIT);
	case SYMERR_INVALID_FILE:
		return HRESULT_FROM_WIN32(NTE_BAD_SIGNATURE);
	default:
		SAVASSERT("Unrecognized SYMRESULT code");
		SAVASSERT(originalCode);
		if (SYM_SUCCEEDED(originalCode))
			return S_OK;
		else
			return E_FAIL;
	}
}

// Constructor-destructor
CSymProtectController::CSymProtectController() : initialized(false), timeoutQuery(DefaultTimeout), timeoutSet(DefaultTimeout), timeoutDefault(DefaultTimeout)

{
	// Nothing for now
}

CSymProtectController::~CSymProtectController()
{
	Shutdown();
}

HRESULT CSymProtectController::SetEventTimeout( DWORD newQueryTimeout, DWORD newSetTimeout, DWORD newDefaultTimeout )
// Sets the message communication timeouts to use when communicating with SymProtect via ccEventManager.
// If no response is received within this timeout, the ccEventMAnager fails the send.
// defaultTimeout applies to overall control events
{
	if (!initialized)
		return E_UNEXPECTED;

	timeoutQuery = newQueryTimeout;
	timeoutSet = newSetTimeout;
	timeoutDefault = newDefaultTimeout;

	return S_OK;
}

HRESULT CSymProtectController::GetEventTimeout( DWORD* queryTimeoutValue, DWORD* setTimeoutValue, DWORD* defaultTimeoutValue )
// If provided, sets queryTimeoutValue = the query event timeout and *setTimeoutValue = the timeout for
// modification events, *defaultTimeoutValue equal to the timeout for all other events
{
	// Validate state and parameters
    if (!initialized)
        return E_UNEXPECTED;
	if ((queryTimeoutValue == NULL) && (setTimeoutValue == NULL) && (defaultTimeoutValue == NULL))
		return E_POINTER;

	// Retrieve the requested data
	if (queryTimeoutValue != NULL)
		*queryTimeoutValue = timeoutQuery;
	if (setTimeoutValue != NULL)
		*setTimeoutValue = timeoutSet;
	if (defaultTimeoutValue != NULL)
		*defaultTimeoutValue = timeoutDefault;

	return S_OK;
}

// ** ISymProtectController **
HRESULT ISymProtectController::Create( ISymProtectController** newObject )
// Creates a new SymProtectController object
{
	CSymProtectController*		newController		= NULL;
	HRESULT						returnValHR			= E_FAIL;

	if (newObject == NULL)
		return E_POINTER;
	if (*newObject != NULL)
		return E_INVALIDARG;

    newController = new(std::nothrow) CSymProtectController;
    if (newController != NULL)
    {
        *newObject = dynamic_cast<ISymProtectController*>(newController);
        returnValHR = S_OK;
    }
    else
    {
        returnValHR = E_OUTOFMEMORY;
    }

	return returnValHR;
}

HRESULT ISymProtectController::Destroy( ISymProtectController** targetObject )
// Destroys a SymProtectController object
{
	CSymProtectController*		targetController		= NULL;
	HRESULT						returnValHR				= E_FAIL;

	// Validate parameters
	if (targetObject == NULL)
		return E_POINTER;
	if (*targetObject == NULL)
		return E_INVALIDARG;

	targetController = dynamic_cast<CSymProtectController*>(*targetObject);
	delete targetController;
	*targetObject = NULL;
	return S_OK;
}

HRESULT ISymProtectController::IsValidActionType( DWORD actionType )
// Returns S_OK if actionType equals the value of a valid SP_ACTION_TYPE enum, else E_INVALIDARG
{
    ProtectionType      spProtectionType    = ProtectionType_Process;
	HRESULT		        returnValHR		    = E_FAIL;

    returnValHR = GetActionProtectionType(actionType, &spProtectionType);

	return returnValHR;
}

HRESULT ISymProtectController::IsValidProtectionLevel( DWORD protectionLevel )
// Returns S_OK if actionType equals the value of a valid SP_PROTECTION_LEVEL enum, else E_INVALIDARG
{
	HRESULT			returnValHR		= E_FAIL;
	switch (protectionLevel)
	{
	case SymProtectEvt::PROTECTION_DISABLED:
	case SymProtectEvt::PROTECTION_ENABLED:
	case SymProtectEvt::PROTECTION_LOG_ONLY:
		returnValHR = S_OK;
		break;
	default:
		returnValHR = E_INVALIDARG;
		break;
	}

	return returnValHR;
}

HRESULT ISymProtectController::IsValidDeviceID( DWORD deviceID )
// Returns S_OK if actionType equals the value of a valid BB_DEVICE constant, else E_INVALIDARG
{
	HRESULT returnValHR = E_FAIL;

	switch (deviceID)
	{
	case SymProtectEvt::BB_DEVICE_INVALID:
	case SymProtectEvt::BB_DEVICE_CD:
		returnValHR = S_OK;
		break;
	default:
		returnValHR = E_INVALIDARG;
		break;
	}

	return returnValHR;
}

HRESULT ISymProtectController::IsValidHashMethod( DWORD hashMethod )
// Returns S_OK if hashMethod equals the value of a valid BB_HASH constant, else E_INVALIDARG
{
	HRESULT returnValHR = E_FAIL;

	switch (hashMethod)
	{
	case SymProtectEvt::BB_HASH_INVALID:
	case SymProtectEvt::BB_HASH_MD5:
	case SymProtectEvt::BB_HASH_SHA1:
		returnValHR = S_OK;
		break;
	default:
		returnValHR = E_INVALIDARG;
		break;
	}

	return returnValHR;
}

LPCTSTR ISymProtectController::ErrorLocationToString( ISymProtectController::ErrorLocation location )
// Returns a string name of the specified error location for debug logging
{
    switch (location)
    {
    case ErrorLocaton_EventFactoryCreate:
        return "Event factory create";
    case ErrorLocation_ProxyFactoryCreate:
        return "Proxy factory create";
    case ErrorLocation_EventManagerCreate:
        return "Event manager create";
    case ErrorLocation_ProviderHelperCreate:
        return "Provider helper create";
    case ErrorLocation_NewEvent:
        return "NewEvent";
    case ErrorLocation_ConvertEvent:
        return "Convert event";
    case ErrorLocation_EventInteraction:
        return "Event interaction";
    case ErrorLocation_SendEvent:
        return "SendEvent";
    case ErrorLocation_Response_Validate_ConvertEvent:
        return "Convert response validation event";
    case ErrorLocation_Response_Validate_GetResult:
        return "Getresult from response validation event";
    case ErrorLocation_Response_ConvertEvent:
        return "Convert response event";
    case ErrorLocation_Response_EventInteraction:
        return "Response event interaction";
    case ErrorLocation_Other:
        return "Other";
    case ErrorLocation_None:
        return "None";
    default:
        return "Unknown";
    }
}

HRESULT ISymProtectController::GetActionProtectionType( DWORD actionCode, ProtectionType* spProtectionType)
// Sets *spProtectionType equal to the protection type corresponding to the specified SymProtect event action type code
// Returns S_OK on success, E_INVALIDARG if actionCode not recognized
{
    HRESULT         returnValHR     = E_INVALIDARG;
    
    // Validate out parameter
    if (spProtectionType == NULL)
        return E_POINTER;

    // Determine the protection type based on the action code supplied
    switch (actionCode)
    {
    // File
    case SymProtectEvt::ACTION_FS_CREATE:
    case SymProtectEvt::ACTION_FS_DELETE:
    case SymProtectEvt::ACTION_FS_OPEN:
    case SymProtectEvt::ACTION_FS_RENAME:
    case SymProtectEvt::ACTION_FS_SETATTRIBUTES:
    case SymProtectEvt::ACTION_FS_DIRECTORY_CREATE:
    case SymProtectEvt::ACTION_FS_DIRECTORY_DELETE:
    case SymProtectEvt::ACTION_FS_DIRECTORY_RENAME:
    case SymProtectEvt::ACTION_FS_DIRECTORY_SETATTRIBUTES:
        *spProtectionType = ProtectionType_File;
        returnValHR = S_OK;
        break;

    // Process
    case SymProtectEvt::ACTION_API_ZWOPENPROCESS:
    case SymProtectEvt::ACTION_API_ZWOPENTHREAD:
    case SymProtectEvt::ACTION_API_TERMINATE_PROCESS:
    case SymProtectEvt::ACTION_API_SET_INFO_PROCESS:
    case SymProtectEvt::ACTION_API_IMPERSONATE_ANON_TOKEN:
    case SymProtectEvt::ACTION_API_OPEN_PROCESS_TOKEN:
    case SymProtectEvt::ACTION_API_OPEN_THREAD_TOKEN:
    case SymProtectEvt::ACTION_API_ALLOC_VIRTUAL_MEM:
    case SymProtectEvt::ACTION_API_WRITE_VIRTUAL_MEM:
    case SymProtectEvt::ACTION_API_FREE_VIRTUAL_MEM:
    case SymProtectEvt::ACTION_API_MAP_VIEW_OF_SECTION:
    case SymProtectEvt::ACTION_API_UNMAP_VIEW_OF_SECTION:
    case SymProtectEvt::ACTION_API_CREATE_THREAD:
    case SymProtectEvt::ACTION_API_SET_CONTEXT_THREAD:
    case SymProtectEvt::ACTION_API_SET_INFO_THREAD:
    case SymProtectEvt::ACTION_API_TERMINATE_THREAD:
    case SymProtectEvt::ACTION_API_SUSPEND_THREAD:
    case SymProtectEvt::ACTION_API_RESUME_THREAD:
    case SymProtectEvt::ACTION_API_ALERT_THREAD:
    case SymProtectEvt::ACTION_API_ALERT_RESUME_THREAD:
    case SymProtectEvt::ACTION_API_IMPERSONATE_THREAD:
        *spProtectionType = ProtectionType_Process;
        returnValHR = S_OK;
        break;

    // Registry
    case SymProtectEvt::ACTION_REG_OPEN_KEY:
    case SymProtectEvt::ACTION_REG_CREATE_KEY:
    case SymProtectEvt::ACTION_REG_DELETE_KEY:
    case SymProtectEvt::ACTION_REG_DELETE_VALUE:
    case SymProtectEvt::ACTION_REG_SET_VALUE:
        *spProtectionType = ProtectionType_Registry;
        returnValHR = S_OK;
        break;

    // Named Objects
    case SymProtectEvt::ACTION_API_CREATE_MUTEX:
    case SymProtectEvt::ACTION_API_OPEN_MUTEX:
    case SymProtectEvt::ACTION_API_CREATE_EVENT:
    case SymProtectEvt::ACTION_API_OPEN_EVENT:
        *spProtectionType = ProtectionType_NamedObject;
        returnValHR = S_OK;
        break;
    }
    
    return returnValHR;
}
