//
//	IBM AntiVirus Immune System
//
//	File Name:	AutoEvent.h
//	Author:		Milosz Muszynski
//
//	This class allows for an automatic construction and destruction
//  of WIN32 event object and provides a wrapper for the subset
//  of event API
//
//	$Log: $
//

#ifndef __AUTOEVENT_H__
#define __AUTOEVENT_H__


class RawEvent
{
protected:
	HANDLE		_handle;
public:
	RawEvent();
	RawEvent( HANDLE handle );
	virtual ~RawEvent();

	void	init( HANDLE handle );
	void	set();
	void	reset();
	void	wait();
	bool	wait( RawEvent& event ); // wait for 2 events, returns true if the one passed as param was signaled
	bool	isSet();

	HANDLE	getHandle() const;
};

class AutoEvent : public RawEvent
{
public:
	AutoEvent() : RawEvent() {}
	virtual ~AutoEvent();
};


#endif __AUTOEVENT_H__
