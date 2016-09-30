//
//	IBM AntiVirus Immune System
//
//	File Name:	AutoEvent.cpp
//	Author:		Milosz Muszynski
//
//	This class allows for an automatic construction and destruction
//  of WIN32 event object and provides a wrapper for the subset
//  of event API
//
//	$Log: $
//

// ------ standard headers --------------------------------------------
#include "base.h"

// ------ custom headers ----------------------------------------------
#include "AutoEvent.h"

// ------ class Mutex -------------------------------------------------

RawEvent::RawEvent() : _handle( NULL )
{
	_handle = CreateEvent( NULL, FALSE, FALSE, NULL );
}

RawEvent::RawEvent( HANDLE handle ) : _handle( handle )
{
}

RawEvent::~RawEvent()
{
}

void RawEvent::init( HANDLE handle )
{
	_handle = handle;
}

void RawEvent::set()
{
	if ( _handle != NULL )
		SetEvent( _handle );
}

void RawEvent::reset()
{
	if ( _handle != NULL )
		ResetEvent( _handle );
}

void RawEvent::wait()
{
	if ( _handle != NULL )
		WaitForSingleObject( _handle, INFINITE );
}

bool RawEvent::wait( RawEvent& event ) // wait for 2 events, returns true if the one passed as param was signaled
{
	bool b = false;
	if ( _handle != NULL && event._handle != NULL )
	{
		HANDLE array[ 2 ];
		0[ array ] = _handle;
		1[ array ] = event._handle;
		DWORD rc = WaitForMultipleObjects( 2, array, FALSE, INFINITE );
		if ( rc == (WAIT_OBJECT_0 + 1) )
			b = true;
	}
	return b;
}

bool RawEvent::isSet()
{
	bool b = false;
	if ( _handle != NULL )
	{
		DWORD rc = WaitForSingleObject( _handle, 0 );
		b = ( rc == WAIT_OBJECT_0 );
	}
	return b;
}

HANDLE RawEvent::getHandle() const
{
	return _handle;
}



AutoEvent::~AutoEvent()
{
	if ( _handle != NULL )
		CloseHandle( _handle );
	_handle = NULL;
}

