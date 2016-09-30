//
//	IBM AntiVirus Immune System
//
//	File Name:	AutoHandle.h
//	Author:		Milosz Muszynski
//
//	This class allows for an automatic release
//  of a WIN32 handle
//
//	$Log: $
//

#ifndef __AUTOHANDLE_H__
#define __AUTOHANDLE_H__

class AutoHandle
{
private:
	HANDLE		_handle;
	bool		_valid;
public:
	AutoHandle();
	virtual ~AutoHandle();

	void	set( HANDLE handle );
	HANDLE	get();
	void	close();

	bool	isValid(){ return _valid; }
};

#endif __AUTOEVENT_H__
