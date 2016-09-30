// SampleStatus.cpp: implementation of the SampleStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SampleStatus.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SampleStatus::SampleStatus()
{

}

SampleStatus::~SampleStatus()
{

}


const char* SampleStatus::ToConstChar(Status status)
{
	switch (status)
	{
	case quarantined:	return Quarantined();
	case submitted:		return Submitted();
	case held:			return Held();
	case released:		return Released();
	case unneeded:		return UnNeeded();
	case needed:		return Needed();
	case available:		return Available();
	case distributed:	return Distributed();
	case installed:		return Installed();
	case attention:		return Attention();
	case error:			return Error();
	case notInstalled:	return NotInstalled();
	case restart:		return Restart();
	case legacy:		return Legacy();
	case distribute:	return Distribute();

	case unknown:
	default:
		return Unknown();
	}
}


SampleStatus::Status SampleStatus::	FromInt(const int from)
{
	switch (from)
	{
	case quarantined:	return quarantined;
	case submitted:		return submitted;
	case held:			return held;
	case released:		return released;
	case unneeded:		return unneeded;
	case needed:		return needed;
	case available:		return available;
	case distributed:	return distributed;
	case installed:		return installed;
	case attention:		return attention;
	case error:			return error;
	case notInstalled:	return notInstalled;
	case restart:		return restart;
	case legacy:		return legacy;
	case distribute:	return distribute;

	case unknown:
	default:
		return unknown;
	}
}



SampleStatus::Status SampleStatus::ToStatus(const char* status)
{
	Status	s	= unknown;

	if ('a' == status[0])
	{
		if (!strcmp(Attention(), status))
			s	= attention;
		else if (!strcmp(Available(), status))
			s	= available;
	}
	else if ('d' == status[0])
	{
		if (!strcmp(Distribute(), status))
			s	= distribute;
		else if (!strcmp(Distributed(), status))
			s	= distributed;
	}
	else if ('e' == status[0])
	{
		if (!strcmp(Error(), status))
			s	= error;
	}
	else if ('h' == status[0])
	{
		if (!strcmp(Held(), status))
			s	= held;
	}
	else if ('i' == status[0])
	{
		if (!strcmp(Installed(), status))
			s	= installed;
	}
	else if ('l' == status[0])
	{
		if (!strcmp(Legacy(), status))
			s	= installed;
	}
	else if ('n' == status[0])
	{
		if (!strcmp(Needed(), status))
			s	= needed;
		else if (!strcmp(NotInstalled(), status))
			s	= notInstalled;
	}
	else if ('q' == status[0])
	{
		if (!strcmp(Quarantined(), status))
			s	= quarantined;
	}
	else if ('r' == status[0])
	{
		if (!strcmp(Released(), status))
			s	= released;
		else if (!strcmp(Restart(), status))
			s	= restart;
	}
	else if ('s' == status[0])
	{
		if (!strcmp(Submitted(), status))
			s	= submitted;
	}
	else if ('u' == status[0])
	{
		if (!strcmp(UnNeeded(), status))
			s	= unneeded;
	}

	return s;
}

