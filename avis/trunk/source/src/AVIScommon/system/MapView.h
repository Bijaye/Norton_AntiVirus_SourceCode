// MapView.h: interface for the MapView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPVIEW_H__2F97112E_6933_11D2_892C_00A0C9DB9E9C__INCLUDED_)
#define AFX_MAPVIEW_H__2F97112E_6933_11D2_892C_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "AVIS.h"
#include "AVISCommon.h"
#include "Handle.h"

class AVISCOMMON_API MapView  
{
public:
	MapView() : fileMap(INVALID_HANDLE_VALUE), ptr(NULL) {};
	MapView(const std::string& fileName, uint mSize) :
			fileMap(CreateFileMapping((HANDLE) 0xFFFFFFFF, NULL,
										PAGE_READWRITE, 0, mSize+sizeof(ulong),
										fileName.c_str())),
			ptr((uchar*) MapViewOfFile(fileMap, FILE_MAP_READ | FILE_MAP_WRITE,
										0,0,0))
			{	if (ptr)
				{	*((ulong*) ptr) = mSize;  ptr += sizeof(ulong); }
			};
	
	uchar* SetMapView(Handle fileMap)
	{
		if (NULL != ptr) UnmapViewOfFile(ptr); 
		ptr = (uchar*) MapViewOfFile(fileMap, FILE_MAP_READ | FILE_MAP_WRITE,
										0,0,0);
		return ptr;
	}

	virtual ~MapView() { if (NULL != ptr) UnmapViewOfFile((ptr-sizeof(ulong)));  ptr = NULL; };

	operator void*() { return (void*) ptr; };
	operator uchar*(){ return ptr; };

private:
	Handle	fileMap;
	uchar*	ptr;
};

#endif // !defined(AFX_MAPVIEW_H__2F97112E_6933_11D2_892C_00A0C9DB9E9C__INCLUDED_)
