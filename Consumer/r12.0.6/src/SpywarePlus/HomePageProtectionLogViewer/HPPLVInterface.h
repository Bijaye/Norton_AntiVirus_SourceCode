#pragma once

//
//  todo: After next cc build, make sure this id is in the appropriate header files, and
//        remove the code segment below.
//			- HackMaster P
//
#pragma message("CC_SSP_PLUGIN_ID should be defined in ccLogViewerPluginId.h in common client area, not here.")
#ifndef CC_HPPLV_PLUGIN_ID
#define CC_HPPLV_PLUGIN_ID 7000
#endif

#define CC_HPPLV_CATID_ACTIVITY CC_HPPLV_PLUGIN_ID + 1

// {83EC3E44-CBF9-4d97-A3F9-1CC86CA4CA6C}
SYM_DEFINE_INTERFACE_ID(CLSID_HPPLV_PLUGIN_ID, 
                        0x83ec3e44, 0xcbf9, 0x4d97, 0xa3, 0xf9, 0x1c, 0xc8, 0x6c, 0xa4, 0xca, 0x6c);

// {6A6C537A-B3AB-43dd-986B-80700A6A317E}
SYM_DEFINE_INTERFACE_ID(CLSID_HPPLV_CAT_ACTIVITY, 
                        0x6a6c537a, 0xb3ab, 0x43dd, 0x98, 0x6b, 0x80, 0x70, 0xa, 0x6a, 0x31, 0x7e);

