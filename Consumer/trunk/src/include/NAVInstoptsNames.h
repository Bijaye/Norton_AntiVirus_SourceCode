////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//NAVInstoptsNames.h
//This file should contain NAV Specific Instopts Names.

namespace AntiVirus
{
	//Set only the drivers in to a manual mode, so they are not launched on reboot
	//There is another option INSTOPTS:NODRIVERS - this option is a super set, it will
	//shut down all symantec modules
	const char INSTOPTS_DRIVERS_MANUAL_START[] = "INSTOPTS:STARTDRIVERSMANUALLY";
	
	//configwiz
	const char CFGWIZ_DRIVERS_MANUAL_START[] = "CFGWIZ:DRIVERSMANUALSTART";

    // DRM Upgrade Flags
    const char INSTOPTS_DRM_UPGRADE_PRODUCT_IDS[] = "INSTOPTS:UPGRADEPRODUCTIDS";
    const char CFGWIZ_DRM_UPGRADE_PRODUCT_IDS[] = "CFGWIZ:UPGRADEPRODUCTIDS";
}
