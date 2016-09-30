
#ifndef XSCANRESULTS_H
#define XSCANRESULTS_H

#include "AVISCommon.h"

class AVISCOMMON_API XScanResults
{
public:
	static const char*	BadRepair()		{ return "badrepair";	}
	static const char*	BadScan()		{ return "badscan";		}
	static const char*	Heuristic()		{ return "heuristic";	}
	static const char*	NoDetect()		{ return "nodetect";	}
	static const char*	NoRepair()		{ return "norepair";	}
	static const char*	Overrun()		{ return "overrun";		}
	static const char*	Repaired()		{ return "repaired";	}
	static const char*	UnderRun()		{ return "underrun";	}
	static const char*	UnRepairable()	{ return "unrepairable";	}
	static const char*	UnInfectable()	{ return "uninfectable";	}
	static const char*	UnSubmittable()	{ return "unsubmittable";	}

private:
	XScanResults();
	~XScanResults();
};


#endif
