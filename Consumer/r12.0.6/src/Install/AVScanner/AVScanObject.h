
#pragma once

const TCHAR cszRelativeDefsDir[] = "\\VirusDef";
const TCHAR csRelIniPath[] = "\\VirusDefs.ini";
const TCHAR csIniSection[] = "VirusDefs";
const TCHAR csIniField[] = "Directory";
const TCHAR csHardCodedDefsPath[] = "C:\\VirusDef";

class CAVScanObject : public CPreFlightScanObjectBase
{
public:
    CAVScanObject(void) throw();
    virtual ~CAVScanObject(void) throw();

	SYMRESULT RequestAbort();
	SYMRESULT Scan(HWND m_hParentWnd);

private:
	SYMRESULT FindDefs(LPTSTR szDefsDir, DWORD& dwSize);
    
    // scan sink
	CEraserScanSink* m_pSink;

    // abort flag
    bool m_bAbort;

public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( IID_PreFlightScanner, IPreFlightScanner )
    SYM_INTERFACE_MAP_END()

public:
};

// {0E6384BB-B8BF-4639-B415-EE716AFCF746}
SYM_DEFINE_OBJECT_ID(CLSID_AVScanObject, 0xE6384BB, 0xB8BF, 0x4639, 0xB4, 0x15, 0xEE, 0x71, 0x6A, 0xFC, 0xF7, 0x46);
