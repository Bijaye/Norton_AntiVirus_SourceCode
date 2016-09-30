

#include "stdafx.h"

#include <time.h>

#include <Logger.h>

#include "ScanMonitor.h"
#include "ScanServiceGlobals.h"
#include "ScanService.h"

using namespace std;

CMclCritSec	ScanMonitor::intervalInfoLock;
ulong		ScanMonitor::scanCountTotal			= 0;
ulong		ScanMonitor::intervalScanCount		= 0;
ulong		ScanMonitor::intervalScanTicks		= 0;
ulong		ScanMonitor::maxScanTimeInInterval	= 0;
ulong		ScanMonitor::minScanTimeInInterval	= 0;

ulong		ScanMonitor::scanOnlyCount;			// time doing actual scan
ulong		ScanMonitor::waitToScanCount;		// time waiting for wait to scan to return
ulong		ScanMonitor::isInfectableCount;		// time waiting for IsInfectable to return
ulong		ScanMonitor::scanFileCount;			// time waiting for scan api to return
ulong		ScanMonitor::virusInfoCount1;		// 1st call to virus info
ulong		ScanMonitor::virusInfoCount2;		// 2nd call to virus info
ulong		ScanMonitor::virusInfoCount3;		// 3st call to virus info
ulong		ScanMonitor::virusInfoCount4;		// 4nd call to virus info
ulong		ScanMonitor::repairCount;			// time trying to repair
ulong		ScanMonitor::writeToPipeCount;		// time writing reply onto pipe


uint		ScanMonitor::defChangeCount			= 0;

inline	float	CalcSeconds(ulong deltaClockTicks)
{
	return ((float)deltaClockTicks)/((float) CLOCKS_PER_SEC);
}

unsigned ScanMonitor::ThreadHandlerProc()
{
	EntryExit	entryExit(Logger::LogAvisScanner, "ScanMonitor::ThreadHandlerProc");

	uint	totalSlept;
	uint	isDefChangeCount;
	ulong	scTotal, isCount, isTicks, maxScanTime, minScanTime;
	ulong	itScanOnly, itWaitToScan, itIsInfectable, itScanFile;
	ulong	itVI1, itVI2, itVI3, itVI4;
	ulong	itRepair, itWrite;
	char	buffer[256];
	float	aveTime;
	uint	howManyScanJobs;
	Scan	*scanJobs	= ScanService::Current().ScanJobs(howManyScanJobs);
	uint	activeCount;
	uint	overDueCount, veryOverDueCount;
	uint	overDueClicks		= CLOCKS_PER_SEC*45;
	uint	veryOverDueClicks	= CLOCKS_PER_SEC*120;
	clock_t	now;
	clock_t	delta;
	uint	currDefChangeCount;

	while (!ScanServiceGlobals::stopService)
	{
		totalSlept	= 0;
		while (!ScanServiceGlobals::stopService && totalSlept < logInterval)
		{
			Sleep(5000);
			totalSlept	+= 5000;
		}
		if (!ScanServiceGlobals::stopService)
		{
			{
				CMclAutoLock	autoLock(intervalInfoLock);
				scTotal		= scanCountTotal;
				isCount		= intervalScanCount;
				isTicks		= intervalScanTicks;
				maxScanTime	= maxScanTimeInInterval;
				minScanTime	= minScanTimeInInterval;
				isDefChangeCount	= defChangeCount;

				itScanOnly	= scanOnlyCount;
				itWaitToScan= waitToScanCount;
				itIsInfectable= isInfectableCount;
				itScanFile	= scanFileCount;
				itVI1		= virusInfoCount1;
				itVI2		= virusInfoCount2;
				itVI3		= virusInfoCount3;
				itVI4		= virusInfoCount4;
				itRepair	= repairCount;
				itWrite		= writeToPipeCount;

				intervalScanCount		= 0;
				intervalScanTicks		= 0;
				maxScanTimeInInterval	= 0;
				minScanTimeInInterval	= 0x7FFFFFFF;
				defChangeCount			= 0;

				scanOnlyCount			= 0;
				waitToScanCount			= 0;
				isInfectableCount		= 0;
				scanFileCount			= 0;
				virusInfoCount1			= 0;
				virusInfoCount2			= 0;
				virusInfoCount3			= 0;
				virusInfoCount4			= 0;
				repairCount				= 0;
				writeToPipeCount		= 0;

			}
			if (isCount > 0)
			{
				aveTime	= CalcSeconds(isTicks/isCount);
				sprintf(buffer, "scanned %d files, average scan time = %f, min/max (%f/%f), def changes = %d",
								isCount, aveTime,
								CalcSeconds(minScanTime),
								CalcSeconds(maxScanTime),
								isDefChangeCount);
				Logger::Log(Logger::LogError, Logger::LogAvisScanner, buffer);
				sprintf(buffer, "intermediate times [%f, %f, %f, %f, %f, %f, %f, %f, %f, %f]",
								CalcSeconds(itScanOnly/isCount),
								CalcSeconds(itWaitToScan/isCount),
								CalcSeconds(itIsInfectable/isCount),
								CalcSeconds(itScanFile/isCount),
								CalcSeconds(itVI1/isCount),
								CalcSeconds(itVI2/isCount),
								CalcSeconds(itVI3/isCount),
								CalcSeconds(itVI4/isCount),
								CalcSeconds(itRepair/isCount),
								CalcSeconds(itWrite/isCount));
			}

			activeCount	= overDueCount = veryOverDueCount = 0;
			now			= clock();
			for (int i = 0; i < howManyScanJobs; i++)
			{
				if (scanJobs[i].Active())
				{
					activeCount++;
					delta = now - scanJobs[i].StartTicks();
					if (delta > veryOverDueClicks)
						veryOverDueCount++;
					else if (delta > overDueClicks)
						overDueCount++;
				}
			}
			if (activeCount > 0)
			{
				sprintf(buffer, "%d scans currently active, %d are over due, and %d are probably locked\n",
							activeCount, overDueCount, veryOverDueCount);
				Logger::Log(0 < veryOverDueCount ? Logger::LogError : Logger::LogError,
								Logger::LogAvisScanner, buffer);
			}

			currDefChangeCount	= defChangeCount;
			defChangeCount		= 0;
			if (currDefChangeCount > 0)
			{
				sprintf(buffer, "The definition file changed %d times", currDefChangeCount);
				Logger::Log(Logger::LogError, Logger::LogAvisScanner, buffer);
			}
		}
	}

	return true;
}
