//
//	IBM AntiVirus Immune System
//
//	File Name:	AVIS.h
//	Author:		Andy Klapper
//
//	This file stores all of the global enumerations and other project
//	common stuff.  It does not belong in the database class libraries
//	directory so it will have to find a new home at some point in time.
//
//	$Log: /AVISdb/AVISdb/AVIS.h $
//	 
//


#ifndef AVIS_H
#define AVIS_H

typedef unsigned char	uchar;
typedef unsigned int	uint;
typedef unsigned long	ulong;
typedef unsigned short	ushort;



namespace AVIS
{
	//
	//	These are the kinds of errors that can result when an http message is sent
	enum TransError		{ UnknownTransError, TRcorrupted, TRmalformed, TRoverrun,
							TRunderrun };

	//
	//	Samples fall into one of these originator categories
	enum SampleCategory { UnknownSampleCategory, Customer, Demo, Expert, Junk,
							Ping, Zoo };

	//
	//	Samples fall into one of these virus types
	enum SampleType		{ UnknownSampleType, Executable, Macro, Boot, Memory };

	//
	//	A sample's infected/not infected status will be determined by one of these
	//	checks.
	enum SampleCheck	{ UnknownSampleCheck, NotFinished, CrcLookup, Classify,
							BlessedScan, LatestScan, AutomatedAnalysis,
							HumanAnalysis };

	//
	//	A sample must be in one of these states at any given point in time.
	enum SampleState	{ UnknownSampleState, SampleCheckError, Accepted,
							Building, Built, Captured, Declined, Deferred,
							Exectuting, Extracted, Extracting, Imported,
							Importing, Infected, Pong, Released, Replicated,
							Scored, Scoring, Segragated, Segregating,
							Submitting, Testing, UnInfected, Corrupted, Malformed,
							Overrun, Underrun,
							CookieCrumbled, NoMatchingCheckSum };

};



#endif
