//
//	IBM AntiVirus Immune System
//
//	File Name:	AutoHandle.cpp
//	Author:		Milosz Muszynski
//
//	This class allows for an automatic release
//  of a WIN32 handle
//
//	$Log: $
//

// ------ standard headers --------------------------------------------
#include "base.h"

// ------ custom headers ----------------------------------------------
#include "AutoHandle.h"

// ------ class AutoHandle --------------------------------------------

AutoHandle::AutoHandle() : _handle( NULL ), _valid( false )
{
}

AutoHandle::~AutoHandle()
{
	CloseHandle( _handle );
}

void AutoHandle::set( HANDLE handle )
{
	_handle = handle;
	_valid = true;
}

HANDLE AutoHandle::get()
{
	return _handle;
}

void AutoHandle::close()
{
	_valid = false;
	CloseHandle( _handle );
	_handle = NULL;
}
