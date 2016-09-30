
/*
#include <stdio.h>

typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned char  BYTE;

typedef BYTE *LPBYTE;
typedef BYTE *LPWORD;
*/


#define INVALID_REGION_VALUE    ((DWORD)-1)
#define MAX_REGION              8

typedef struct
{
    DWORD dwRegionStart;
    DWORD dwRegionEnd;
} REGION_T;

typedef struct
{
    REGION_T    stRegion[MAX_REGION];
} REGIONS_T, *LPREGIONS;


// protos

void  RegionInit(LPREGIONS   lpRegions);
void  RegionInsertEA(LPREGIONS lpRegions, DWORD dwEA);
DWORD RegionGetMax(LPREGIONS  lpRegions);
