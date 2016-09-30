
#ifndef SCANMONITOR_H
#define SCANMONITOR_H

#include <AVIS.h>

#include <CMclThread.h>
#include <CMclAutoLock.h>

class ScanService;

class ScanMonitor : public CMclThreadHandler
{
public:
	ScanMonitor(uint intervalInMilliseconds) : CMclThreadHandler(),
												logInterval(intervalInMilliseconds)
	{};


	unsigned ThreadHandlerProc(void);

	static	void		LogFinished(ulong howLongInTicks)
	{
		CMclAutoLock	autoLock(intervalInfoLock);

		++scanCountTotal;
		++intervalScanCount;
		intervalScanTicks	+= howLongInTicks;
		if (maxScanTimeInInterval < howLongInTicks)
			maxScanTimeInInterval	= howLongInTicks;
		if (minScanTimeInInterval > howLongInTicks)
			minScanTimeInInterval	= howLongInTicks;
	}

	static	void TrackIntermediateTimes(ulong scanOnly, ulong waitToScan, ulong isInfectable,
										ulong scanFile, ulong virusInfo1, ulong virusInfo2,
										ulong virusInfo3, ulong virusInfo4, ulong repairTime,
										ulong writeToPipe)
	{
		CMclAutoLock	autoLock(intervalInfoLock);

		scanOnlyCount		+= scanOnly;
		waitToScanCount		+= waitToScan;
		isInfectableCount	+= isInfectable;
		scanFileCount		+= scanFile;
		virusInfoCount1		+= virusInfo1;
		virusInfoCount2		+= virusInfo2;
		virusInfoCount3		+= virusInfo3;
		virusInfoCount4		+= virusInfo4;
		repairCount			+= repairTime;
		writeToPipeCount	+= writeToPipe;
	}


	static void			DefChanged(void)	{ ++defChangeCount; }

private:
	static	CMclCritSec	intervalInfoLock;
	static	ulong		scanCountTotal;			// how many scans
	static	ulong		intervalScanCount;		// how many in last interval
	static	ulong		intervalScanTicks;		// how many ticks to do scans in last interval
	static	ulong		maxScanTimeInInterval;	// longest scan time durring interval
	static	ulong		minScanTimeInInterval;	// shortest scan time durring interval

	//
	//	all of the following really need their own count to be completely accurate,
	//	on the other hand we are currently looking for a gross loss of time so this
	//	should more than enough to track that loss of time.

	static	ulong		scanOnlyCount;			// time doing actual scan
	static	ulong		waitToScanCount;		// time waiting for wait to scan to return
	static	ulong		isInfectableCount;		// time waiting for IsInfectable to return
	static	ulong		scanFileCount;			// time waiting for scan api to return
	static	ulong		virusInfoCount1;		// 1st call to virus info
	static	ulong		virusInfoCount2;		// 2nd call to virus info
	static	ulong		virusInfoCount3;		// 3st call to virus info
	static	ulong		virusInfoCount4;		// 4nd call to virus info
	static	ulong		repairCount;			// time trying to repair
	static	ulong		writeToPipeCount;		// time writing reply onto pipe

	static	uint		defChangeCount;			// how many times did the def file change

	uint	logInterval;

};

#endif
