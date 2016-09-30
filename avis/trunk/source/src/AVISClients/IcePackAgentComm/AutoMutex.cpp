//
//	IBM AntiVirus Immune System
//
//	File Name:	AutoMutex.cpp
//	Author:		Milosz Muszynski
//
//	This class allows for an automatic construction and destruction
//  of WIN32 mutex object and provides a wrapper for the subset
//  of mutex API.
//  Class AutoMutex allows for automatic lcoking and releasing a mutex
//  within the scope of the object's lifetime
//
//	$Log: $
//

// ------ standard headers --------------------------------------------
#include "base.h"

// ------ custom headers ----------------------------------------------
#include "AutoMutex.h"

// ------ class Mutex -------------------------------------------------

Mutex::Mutex() : _handle( NULL )
{
	_handle = CreateMutex( NULL, FALSE, NULL );
}

Mutex::~Mutex()
{
	CloseHandle( _handle );
}

void Mutex::wait()
{
	WaitForSingleObject( _handle, INFINITE );
}

void Mutex::release()
{
	ReleaseMutex( _handle );
}

// ------ class AutoMutex ---------------------------------------------

AutoMutex::AutoMutex( Mutex& mutex ) : _mutex( mutex )
{
	_mutex.wait();
}

AutoMutex::~AutoMutex()
{
	_mutex.release();
}






