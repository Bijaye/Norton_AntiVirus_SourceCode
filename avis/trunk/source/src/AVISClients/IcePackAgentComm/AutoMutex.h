//
//	IBM AntiVirus Immune System
//
//	File Name:	AutoMutex.h
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

#ifndef __AUTOMUTEX_H__
#define __AUTOMUTEX_H__


class Mutex
{
private:
	HANDLE	_handle;
public:
	Mutex();
	virtual ~Mutex();

	void	wait();
	void	release();
};


class AutoMutex
{
private:
	Mutex&	_mutex;
public:
	AutoMutex( Mutex& );
	virtual ~AutoMutex();
};


#endif __AUTOMUTEX_H__
