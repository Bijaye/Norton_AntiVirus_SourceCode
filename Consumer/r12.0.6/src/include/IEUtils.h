//
// Collection of functionality for common use with 
// NAV's MSIE Plug-in.
//

#ifndef __CIEUtils_H
#define __CIEUtils_H

#include "tchar.h"

namespace NAVToolbox
{
class CIEUtils
{ 

public :
    CIEUtils ();
    virtual ~CIEUtils (){};

    int m_iMajorVersion;
    int m_iMinorVersion;
    int m_iBuild;
    TCHAR m_szVersionString [128];
};
} // end namespace NAVToolbox

#endif // __CIEUtils_H