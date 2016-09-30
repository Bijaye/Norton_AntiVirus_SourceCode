////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// When incrementing the build number, be sure to change it in all places!
// You must update the version number in build.h as appropriate
//
// SW_PRODUCT_NAME should be the product code name until Green Masters, at
// which time it is changed to the real name.  The code name remains embedded
// in SW_BUILD_STRING.
//

#ifndef INCL_SWVER
#define INCL_SWVER

#ifndef VER_STR_COMPANYNAME
#include "build.h"
#endif //VER_STR_COMPANYNAME

// Uncomment this to display the build number in the version info
#define DISPLAY_BUILD_NO

// Uncomment this to display the build date in the version info
#define DISPLAY_BUILD_DATE

// String resources
#ifdef DISPLAY_BUILD_NO
#define NAV_RES_FILEVERSIONSTRING       VER_STR_PRODVERMAJOR "." VER_STR_PRODVERMINOR "." VER_STR_PRODVERSUBMINOR "." VER_STR_BUILDNUMBER
#define NAV_RES_PRODUCTVERSIONSTRING    VER_STR_PRODVERMAJOR "." VER_STR_PRODVERMINOR "." VER_STR_PRODVERSUBMINOR
#else
#define NAV_RES_FILEVERSIONSTRING       VER_STR_PRODVERMAJOR "." VER_STR_PRODVERMINOR "." VER_STR_PRODVERSUBMINOR
#define NAV_RES_PRODUCTVERSIONSTRING    VER_STR_PRODVERMAJOR "." VER_STR_PRODVERMINOR "." VER_STR_PRODVERSUBMINOR
#endif

#define NAV_RES_PRODUCT_NAME        VER_STR_PRODUCT_SIRNAME


#endif // INCL_SWVER


