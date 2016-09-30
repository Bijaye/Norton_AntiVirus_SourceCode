#include "pamapi.h"
#include "region.h"

void RegionInit(LPREGIONS   lpRegions)
{
    int i;

    for (i=0;i<MAX_REGION;i++)
        lpRegions->stRegion[i].dwRegionStart =
            lpRegions->stRegion[i].dwRegionEnd = INVALID_REGION_VALUE;
}


void RegionInsertEA(LPREGIONS lpRegions, DWORD dwEA)
{
    int i;

	for (i=0;i<MAX_REGION;i++)
    {
		if (lpRegions->stRegion[i].dwRegionStart == INVALID_REGION_VALUE)
		{
			lpRegions->stRegion[i].dwRegionStart =
				lpRegions->stRegion[i].dwRegionEnd = dwEA;
			return;
		}

		if (dwEA < lpRegions->stRegion[i].dwRegionStart &&
            dwEA >= lpRegions->stRegion[i].dwRegionStart - sizeof(DWORD))
		{
			lpRegions->stRegion[i].dwRegionStart = dwEA;
			return;
		}

		if (dwEA > lpRegions->stRegion[i].dwRegionEnd &&
            dwEA <= lpRegions->stRegion[i].dwRegionEnd + sizeof(DWORD))
		{
			lpRegions->stRegion[i].dwRegionEnd = dwEA;
			return;
		}

		if (lpRegions->stRegion[i].dwRegionStart <= dwEA &&
			lpRegions->stRegion[i].dwRegionEnd >= dwEA)
            return;
    }
}

DWORD RegionGetMax(LPREGIONS  lpRegions)
{
    int         i;
    DWORD       dwMax = 0, dwCur;

    for (i=0;i<MAX_REGION;i++)
    {
		dwCur = lpRegions->stRegion[i].dwRegionEnd -
			lpRegions->stRegion[i].dwRegionStart;
		if (dwCur > dwMax)
			dwMax = dwCur;
	}

    return(dwMax+1);
}


