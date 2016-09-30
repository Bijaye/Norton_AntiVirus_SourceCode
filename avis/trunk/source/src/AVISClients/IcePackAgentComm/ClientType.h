//
//	IBM AntiVirus Immune System
//
//	File Name:	ClientType.h
//	Author:		Milosz Muszynski
//
//	This header defines template function performing common
//	initialization tasks for all types of clients
//
//	$Log: $
//

//
// initClient
//

template <class ClientType, class CommType>
bool initClient(	CommType&						comm,
					ClientType **					si, 
					void **							_si, 
					MyTraceClient&					_tc, 
					const std::string&				_logFileName, 
					const std::string&				_traceFileName, 
					const std::string&				_agentName, 
					const std::string&				gatewayURL,
					IcePackAgentComm::ErrorCode&	_errorCode,
					RawEvent&						_eventStop )
{
//	if ( &_tc == &defaultTraceClient )												// this call will only take place
		//comm.SetTraceClient( _tc, _logFileName, _traceFileName, "main thread: " );	// when the caller failes to set his own trace client

	::initCommunications();

	if ( *_si == NULL )
	{
		*_si = static_cast<ClientType*>( new ClientType (	_logFileName, _traceFileName, _agentName, _eventStop, _tc ) );
		*si = static_cast<ClientType*>( *_si );

		if ( *si == NULL )
		{
			_tc.critical("failed creating \"%s\" client.", (*si)->GetAgentName());
			_errorCode = OutOfMemoryError;
			return false;
		}

		_tc.debug("starting \"%s\" client for %s...", (*si)->GetAgentName(), gatewayURL.c_str() );
		if (!(*si)->Run())
		{
			_tc.critical("failed running \"%s\" client.", (*si)->GetAgentName());
			_errorCode = ThreadCouldNotStartError;
			return false;
		}
		_tc.debug("started successfully" );
	}

	*si = static_cast<ClientType*>( *_si );

	//_tc.debug("%s started for URL = %s", (*si)->GetAgentName(), gatewayURL.c_str() );

	return true;
}


//
// finishClient
//

template <class ClientType>
void finishClient(	ClientType *					si, 
					MyTraceClient&					_tc, 
					const std::string&				gatewayURL,
					IcePackAgentComm::ErrorCode&	_errorCode,
					unsigned long&					_systemExceptionCode )
{
	_tc.debug("waiting for %s for URL = %s", si->GetAgentName(), gatewayURL.c_str() );

	si->Wait();

	_errorCode				= si->Error();
	_systemExceptionCode	= si->SystemExceptionCode();

	_tc.debug("finished %s for %s with rc = %d", si->GetAgentName(), gatewayURL.c_str(), (unsigned int)_errorCode );
	std::string errorMessage;
	IcePackAgentComm::GetErrorMessage( _errorCode, errorMessage );
	_tc.msg("IcePackAgentComm exits with rc = \"%s\"", errorMessage.c_str() );
}


//
// terminateClient
//

template <class ClientType>
void terminateClient(	void **				_si, 
						MyTraceClient&		_tc	)
{
	if ( *_si != NULL )
	{
		ClientType * si = static_cast<ClientType*>( *_si );
		si->Terminate();
		_tc.debug("waiting for \"%s\"", si->GetAgentName() );
		si->WaitForThreadToTerminate();
		_tc.debug("deleting object for \"%s\"", si->GetAgentName() );
		delete static_cast<ClientType*>( *_si );
		*_si = NULL;
	}
}
