////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined CC_FIREWALLSETTINGSMODULELOADER_H_INCLUDED_
#define CC_FIREWALLSETTINGSMODULELOADER_H_INCLUDED_

#include "ccFirewallSettingsInterface.h"
#include "SymInterfaceLoader.h"
#include "SymInterfaceManagedLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccSymPathProvider.h"
#include "ccModuleNames.h"

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::ILocation, 
                               &ccFirewallSettings::IID_LocationInterface, 
                               &ccFirewallSettings::IID_LocationInterface> LocationLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::ILocation, 
                                      &ccFirewallSettings::IID_LocationInterface, 
                                      &ccFirewallSettings::IID_LocationInterface> MgdLocationLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::ILocationList, 
                               &ccFirewallSettings::IID_LocationList, 
                               &ccFirewallSettings::IID_LocationList> LocationListLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::ILocationList, 
                                      &ccFirewallSettings::IID_LocationList, 
                                      &ccFirewallSettings::IID_LocationList> MgdLocationListLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::ILocationManager, 
                               &ccFirewallSettings::IID_LocationManager, 
                               &ccFirewallSettings::IID_LocationManager> LocationManagerLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::ILocationManager, 
                                      &ccFirewallSettings::IID_LocationManager, 
                                      &ccFirewallSettings::IID_LocationManager> MgdLocationManagerLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader,
                               ccFirewallSettings::IFilter, 
                               &ccFirewallSettings::IID_Filter, 
                               &ccFirewallSettings::IID_Filter> FilterLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader,
                                      ccFirewallSettings::IFilter, 
                                      &ccFirewallSettings::IID_Filter, 
                                      &ccFirewallSettings::IID_Filter> MgdFilterLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::ICategory, 
                               &ccFirewallSettings::IID_Category, 
                               &ccFirewallSettings::IID_Category> CategoryLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::ICategory, 
                                      &ccFirewallSettings::IID_Category, 
                                      &ccFirewallSettings::IID_Category> MgdCategoryLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::ICategoryList, 
                               &ccFirewallSettings::IID_CategoryList, 
                               &ccFirewallSettings::IID_CategoryList> CategoryListLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::ICategoryList, 
                                      &ccFirewallSettings::IID_CategoryList, 
                                      &ccFirewallSettings::IID_CategoryList> MgdCategoryListLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IIPHelper, 
                               &ccFirewallSettings::IID_IPHelper, 
                               &ccFirewallSettings::IID_IPHelper> IPHelperLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IIPHelper, 
                                      &ccFirewallSettings::IID_IPHelper, 
                                      &ccFirewallSettings::IID_IPHelper> MgdIPHelperLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IIPItem, 
                               &ccFirewallSettings::IID_IPItem, 
                               &ccFirewallSettings::IID_IPItem> IPItemLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IIPItem, 
                                      &ccFirewallSettings::IID_IPItem, 
                                      &ccFirewallSettings::IID_IPItem> MgdIPItemLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IIPList, 
                               &ccFirewallSettings::IID_IPList, 
                               &ccFirewallSettings::IID_IPList> IPListLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IIPList, 
                                      &ccFirewallSettings::IID_IPList, 
                                      &ccFirewallSettings::IID_IPList> MgdIPListLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IZoneItem, 
                               &ccFirewallSettings::IID_ZoneItem, 
                               &ccFirewallSettings::IID_ZoneItem> ZoneItemLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IZoneItem, 
                                      &ccFirewallSettings::IID_ZoneItem, 
                                      &ccFirewallSettings::IID_ZoneItem> MgdZoneItemLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IZoneItemList, 
                               &ccFirewallSettings::IID_ZoneItemList, 
                               &ccFirewallSettings::IID_ZoneItemList> ZoneItemListLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IZoneItemList, 
                                      &ccFirewallSettings::IID_ZoneItemList, 
                                      &ccFirewallSettings::IID_ZoneItemList> MgdZoneItemListLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IZone, 
                               &ccFirewallSettings::IID_Zone, 
                               &ccFirewallSettings::IID_Zone> ZoneLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IZone, 
                                      &ccFirewallSettings::IID_Zone, 
                                      &ccFirewallSettings::IID_Zone> MgdZoneLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IZoneList, 
                               &ccFirewallSettings::IID_ZoneList, 
                               &ccFirewallSettings::IID_ZoneList> ZoneListLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IZoneList, 
                                      &ccFirewallSettings::IID_ZoneList, 
                                      &ccFirewallSettings::IID_ZoneList> MgdZoneListLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IPort, 
                               &ccFirewallSettings::IID_Port, 
                               &ccFirewallSettings::IID_Port> PortLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IPort, 
                                      &ccFirewallSettings::IID_Port, 
                                      &ccFirewallSettings::IID_Port> MgdPortLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IPortList, 
                               &ccFirewallSettings::IID_PortList, 
                               &ccFirewallSettings::IID_PortList> PortListLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IPortList, 
                                      &ccFirewallSettings::IID_PortList, 
                                      &ccFirewallSettings::IID_PortList> MgdPortListLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IRule, 
                               &ccFirewallSettings::IID_Rule, 
                               &ccFirewallSettings::IID_Rule> RuleLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IRule, 
                                      &ccFirewallSettings::IID_Rule, 
                                      &ccFirewallSettings::IID_Rule> MgdRuleLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IRuleList, 
                               &ccFirewallSettings::IID_RuleList, 
                               &ccFirewallSettings::IID_RuleList> RuleListLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IRuleList, 
                                      &ccFirewallSettings::IID_RuleList, 
                                      &ccFirewallSettings::IID_RuleList> MgdRuleListLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IApplication, 
                               &ccFirewallSettings::IID_Application, 
                               &ccFirewallSettings::IID_Application> ApplicationLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IApplication, 
                                      &ccFirewallSettings::IID_Application, 
                                      &ccFirewallSettings::IID_Application> MgdApplicationLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                               ccFirewallSettings::IApplicationList, 
                               &ccFirewallSettings::IID_ApplicationList, 
                               &ccFirewallSettings::IID_ApplicationList> ApplicationListLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader, // or CSymInterfaceTrustedLoader validation on the DLL
                                      ccFirewallSettings::IApplicationList, 
                                      &ccFirewallSettings::IID_ApplicationList, 
                                      &ccFirewallSettings::IID_ApplicationList> MgdApplicationListLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader,
                               ccFirewallSettings::IFirewallSettings, 
                               &ccFirewallSettings::IID_FirewallSettings, 
                               &ccFirewallSettings::IID_FirewallSettings> FirewallSettingsLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader,
                                      ccFirewallSettings::IFirewallSettings, 
                                      &ccFirewallSettings::IID_FirewallSettings, 
                                      &ccFirewallSettings::IID_FirewallSettings> MgdFirewallSettingsLoader;

typedef CSymInterfaceDLLHelper<&cc::sz_ccFWSettg_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader,
                               ccFirewallSettings::IRuleUtil, 
                               &ccFirewallSettings::IID_RuleUtil, 
                               &ccFirewallSettings::IID_RuleUtil> RuleUtilLoader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ccFWSettg_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader,
                                      ccFirewallSettings::IRuleUtil, 
                                      &ccFirewallSettings::IID_RuleUtil, 
                                      &ccFirewallSettings::IID_RuleUtil> MgdRuleUtilLoader;

#endif  //#if !defined CC_FIREWALLSETTINGSMODULELOADER_H_INCLUDED_