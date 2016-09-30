////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymInterface.h"
#include "ccKeyValueCollectionInterface.h"

namespace ccFirewallSettings
{

enum Result {
	NoError = 0,		// Success
	NotFound,			// The requested item was not found
	OutOfMemory,		// Problem allocating memory, including STL, ATL, CString
	InvalidArgument,	// An invalid argument was passed in
	SymNetiFailure,		// SymNeti returned an error code
	ccSettingsFailure,	// ccSettings returned an error code
	NotInitialized,		// The object hasn't been properly initialized
	CannotInsertHere,	// The item cannot be inserted at this position
	CharacterConversionFailure,	// failure converting from unicode to multi-byte characters
	InsufficientBuffer,	// The buffer was not large enough to hold the requested data
	InvalidType,		// Type method cannot be called on this type of object,
						// for example calling IIPItem::GetRangeStart on a raw ip.
	AlreadyExists,		// The item already exists and cannot be inserted again
    SyncronizationError,// Error getting a syronization object (e.g. mutex)
    NoDefaultLocation,  // When calling, IFirewallSettings::SetLocations(), the ILocationList must have a default location
    EventCreationError, // A failure occured when creating the FirewallSettings change event
    WinsockError,       // A failure occured initializing or using Windows Sockets

    // New results must go before this. Also make sure to update ccFWSettgUtils' szResultText when any results are changed.
    NumResults
} ;

enum SettingChangeType {
    Change_Locations = 0, //Location setting changes event
    Change_SystemRules,   //System Rule setting changes event
    Change_TrojanRules,   //Trojan Rule setting changes event
    Change_ApplicationRules, //Application Rule setting changes event
    Change_Zones,         //Zone setting changes event
    Change_Categories     //Category setting changes event
};

inline bool Succeeded(Result result) {
	return result == NoError;
}

///////////////////////////////////////////////////////////////////////////////
// ILocation
class ILocation : public ISymBase
{
public:
	// Gets the ID of the location. This is automatically assigned.
	virtual Result GetID(DWORD& pVal) const throw() = 0;

	// Returns true if the location has been saved for use by the firewall
	virtual Result IsPersisted(bool& bPersisted) const throw() = 0;

	// Makes this item the Default location at the next save (via IFirewallSettings::SetLocations()).
	// The Default location is the location that is active when no other locations are
	// active. New locations may copy their settings (System rules, Trojan rules, Zones, Applications)
	// from the default location.
	// There should be only one Default location. You may create as many Default locations
	// as you wish per list, but IFirewallSettings::SetLocations() will return InvalidArgument
	// if there is not exactly one Default location.
	virtual Result SetDefaultState(bool bDefault) throw() = 0;
	virtual Result GetDefaultState(bool& bDefault) const throw() = 0;

	// Get/Set the name of the location.
	virtual Result GetName(LPSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result GetName(LPWSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result SetName(LPCSTR newVal) throw() = 0;
	virtual Result SetName(LPCWSTR newVal) throw() = 0;

	// Automatic Program Control determines if rules are automatically created in this location
	// via the ALE engine.
	virtual Result GetAutomaticProgramControl(bool& bOut) const throw() = 0;
	virtual Result SetAutomaticProgramControl(bool newVal) throw() = 0;

	// Gets/Sets a client defined DWORD.
	virtual Result GetPermissions(DWORD& pVal) const throw() = 0;
	virtual Result SetPermissions(DWORD newVal) throw() = 0;

	// Returns whether the location is active or not.
	virtual Result GetActive(bool& bOut) const throw() = 0;

    // Is the location locked (not changable by users) or unlocked. This is used
    // by enterprise products.
    virtual Result GetLockState(bool& bLocked) const throw() = 0;
    virtual Result SetLockState(bool bLocked) throw() = 0;

	// Flag to determine if networks have been deleted from the given location
    virtual Result GetDeleteNetworks(bool& bVal) const throw() = 0;
    virtual Result SetDeleteNetworks(bool bVal) throw() = 0;

	// Delete all networks associated with the location
	virtual Result DeleteNetworks() throw() = 0;

	// Add a network to the location
	virtual Result AddNetSpec(LPCSTR newVal) throw() = 0;
	virtual Result AddNetSpec(LPCWSTR newVal) throw() = 0;

	// Get the number of networks associated with the location. This includes 
	// reserved and non-reserved networks.
	virtual Result GetNumberOfNetworks(long& dwOut) const throw() = 0;

	// Get the name of a network
	virtual Result GetNetSpec(long lNum, LPSTR szOut, DWORD& dwSize) const throw() = 0;
	virtual Result GetNetSpec(long lNum, LPWSTR szOut, DWORD& dwSize) const throw() = 0;

	// Determine whether the contents of two items are identical.
	virtual Result IsEqual(const ILocation* pLocation, bool& bOut) const throw() = 0;

	// Create a copy of an existing item.
	virtual Result Assign(const ILocation* pLocation) throw() = 0;

    // Determines what the firewall action should be in this location if no rules match
    enum eDefaultFirewallAction
	{
        DefaultFirewallAction_Start = 0,
		DefaultFirewallAction_Block = 0,
		DefaultFirewallAction_Permit = 1,
		DefaultFirewallAction_Prompt = 2,
        DefaultFirewallAction_End = DefaultFirewallAction_Prompt
	};
    virtual Result SetDefaultFirewallAction(eDefaultFirewallAction eAction) throw() = 0;
    virtual Result GetDefaultFirewallAction(eDefaultFirewallAction& eAction) const throw() = 0;

    // Determines if the firewall should be turned on when this location becomes active
    virtual Result SetForceEnableFirewall(bool bForceEnable) throw() = 0;
    virtual Result GetForceEnableFirewall(bool& bForceEnable) const throw() = 0;
    //Access attached user defined properties on this location
    virtual Result GetCustomProperties(cc::IKeyValueCollection*& pProperties) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_LocationInterface, 
						0xc60e867, 0x2854, 0x49fc, 0x87, 0xa8, 0xb1, 0x29, 0x19, 0x12, 0x2c, 0x77);
typedef CSymPtr<ILocation> ILocationPtr;
typedef CSymQIPtr<ILocation, &IID_LocationInterface> ILocationQIPtr;

///////////////////////////////////////////////////////////////////////////////
class ILocationList : public ISymBase
{
public:
	// Get a specific item in the list
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	// Changes made to the returned item affect the list.
	virtual Result GetItem(long lIndex, ILocation*& pOut) const throw() = 0;

	// Get the number of items in the list.
	virtual Result GetItemCount(long &lItemCount) const throw() = 0;

	// Adds one or more locations to the list
	virtual Result AddItem(const ILocation* pLocation) throw() = 0;
	virtual Result AddItems(const ILocationList* pLocationList) throw() = 0;

    // Marks the list as containing all current and future locations.
    // Turning on SetAll adds all current locations to the list. Subsequently
    // turning off SetAll leaves all current locations in the list, but does
    // not include future locations.
    // The second SetHasAllLocations overload takes the list of current locations as its second
    // argument. When bAll is true, the second overload will be significantly faster than the first overload.
    virtual Result SetHasAllLocations(bool bAll) throw() = 0;
    virtual Result SetHasAllLocations(bool bAll, const ILocationList* pCurrentLocations) throw() = 0;
    virtual Result GetHasAllLocations(bool& bAll) const throw() = 0;

	// Delete an item from the list.
	virtual Result DeleteItem(const ILocation* pItem) throw() = 0;
	virtual Result DeleteItem(long lIndex) throw() = 0;
	virtual Result DeleteItems(const ILocationList* pList) throw() = 0;

	// Delete all items from the list.
	virtual Result DeleteAll() throw() = 0;

	// Gets an item
	virtual Result GetItemByName(LPCSTR locationName, ILocation*& pOut ) const throw() = 0;
	virtual Result GetItemByName(LPCWSTR locationName, ILocation*& pOut ) const throw() = 0;
	virtual Result GetItemByID(DWORD dwLocation, ILocation*& pOut ) const throw() = 0;
	virtual Result GetDefaultItem(ILocation*& pOut ) const throw() = 0;

	// Determines if a location is in the list
	virtual Result IsInList(const ILocation* pLocation, bool& bInList) const throw() = 0;

	// Create a copy of an existing item.
	virtual Result Assign(const ILocationList* pLocationList) throw() = 0;

	// Determine whether the contents of two lists are identical.
	virtual Result IsEqual(const ILocationList* pLocationList, bool& bOut) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_LocationList, 
						0xfd3e637c, 0x2f8a, 0x4954, 0x8f, 0x7f, 0xa6, 0xc9, 0x32, 0xf0, 0xcf, 0x1b);
typedef CSymPtr<ILocationList> ILocationListPtr;
typedef CSymQIPtr<ILocationList, &IID_LocationList> ILocationListQIPtr;

///////////////////////////////////////////////////////////////////////////////
// ILocation
class ILocationManager : public ISymBase
{
public:
    // The primary location becomes active when no other locations are active.
    // The primary location is the default location is not otherwise set.
    // Call SetPrimaryLocation(NULL) to remove any primary location.
    virtual Result GetPrimaryLocation(ILocation*& pLocation) const throw() = 0;
    virtual Result SetPrimaryLocation(const ILocation* pLocation) throw() = 0;

    // Determine if new user locations may be created. This restriction is carried
    // out by the product. Firewall Settings does not prevent creation of new
    // locations regardless of the state of this flag.
    virtual Result SetAllowNewLocations(bool bAllowNewLocations) throw() = 0;
    virtual Result GetAllowNewLocations(bool& bAllowNewLocations) const throw() = 0;
    //Access attached user defined properties on this location
    virtual Result GetCustomProperties(cc::IKeyValueCollection*& pProperties) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_LocationManager, 
                        0xc71d7fbc, 0xe18a, 0x4784, 0xa9, 0xaa, 0x75, 0xfa, 0xdf, 0xad, 0x56, 0xc4);
typedef CSymPtr<ILocationManager> ILocationManagerPtr;
typedef CSymQIPtr<ILocationManager, &IID_LocationManager> ILocationManagerQIPtr;

///////////////////////////////////////////////////////////////////////////////
// IFilter stores information so lists may be filtered.
class IFilter : public ISymBase
{
public:
	// General operations
	virtual Result Clear() throw() = 0;
	virtual Result IsEmpty(bool& bEmpty) const throw() = 0;
	virtual Result Assign(const IFilter* pFilter) throw() = 0;

	// Filter by locations
	virtual Result AddLocation(const ILocation* pLocation) throw() = 0;
	virtual Result AddLocations(const ILocationList* pLocationList) throw() = 0;
	virtual Result DeleteLocation(const ILocation* pLocation) throw() = 0;
	virtual Result DeleteAllLocations() throw() = 0;
	virtual Result GetLocations(ILocationList*& pLocationList) const throw() = 0;
	virtual Result IsLocationInFilter(const ILocation* pLocation, bool& bInFilter) const throw() = 0;
	virtual Result GetNumLocations(long& lCount) const throw() = 0;
	virtual Result GetLocation(long lIndex, ILocation*& pLocation) const throw() = 0;

    // Filter by locked status
    enum LockState
    {
        eLocked,
        eUnlocked,
        eDontCare
    } ;
    virtual Result SetLockState(LockState eLockState) throw() = 0;
    virtual Result GetLockState(LockState& eLockState) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_Filter, 
						0xf6fb486a, 0xcfd8, 0x45c9, 0x9c, 0x66, 0x1d, 0x70, 0xf0, 0xb3, 0xd7, 0xd9);
typedef CSymPtr<IFilter> IFilterPtr;
typedef CSymQIPtr<IFilter, &IID_Filter> IFilterQIPtr;

///////////////////////////////////////////////////////////////////////////////
// Categories store information used in parental control.
class ICategory : public ISymBase
{
public:
	// Getters/Setters
	virtual Result GetName(LPSTR sName,DWORD& dwSize) const throw() = 0;
	virtual Result GetName(LPWSTR sName,DWORD& dwSize) const throw() = 0;
	virtual Result SetName(LPCSTR sName) throw() = 0;
	virtual Result SetName(LPCWSTR sName) throw() = 0;
	virtual Result GetValue(long& lVal) const throw() = 0;
	virtual Result SetValue(long lVal) throw() = 0;
	virtual Result IsEqual(const ICategory* pCategory, bool& bEqual) const throw() = 0;
    //Access attached user defined properties on this location
    virtual Result GetCustomProperties(cc::IKeyValueCollection*& pProperties) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_Category, 
						0x639a1c2a, 0x8e1e, 0x4423, 0xb4, 0x17, 0x43, 0xcd, 0x61, 0x1, 0x3d, 0x39);
typedef CSymPtr<ICategory> ICategoryPtr;
typedef CSymQIPtr<ICategory, &IID_Category> ICategoryQIPtr;

///////////////////////////////////////////////////////////////////////////////
class ICategoryList : public ISymBase
{
public:
	// Get a specific item in the list
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	// Changes made to the returned item affect the list.
	virtual Result GetItem(long lIndex, ICategory*& pOut) const throw() = 0;
	virtual Result GetDefaultItem(ICategory*& pOut) const throw() = 0;

	// Get the number of items in the list.
	virtual Result GetItemCount(long &lCount) const throw() = 0;

	// Add an item to the list. The list is sorted alphabetically.
	// The reference count on the object is incremented to show ownership by the list. Callers must
	// still Release their copy of the item.
	virtual Result AddItem(const ICategory* pItem) throw() = 0;

	// Find an item with a given value
	virtual Result FindCategory(long lValue, ICategory*& pCategory) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_CategoryList, 
						0x3b0ea49c, 0xaa23, 0x4953, 0x84, 0xab, 0xf8, 0x4f, 0xfe, 0xc3, 0xbc, 0xd6);
typedef CSymPtr<ICategoryList> ICategoryListPtr;
typedef CSymQIPtr<ICategoryList, &IID_CategoryList> ICategoryListQIPtr;

///////////////////////////////////////////////////////////////////////////////
// IIPHelper
// Static methods that offer helper functionality for IP addresses and
// DNS functionality.
class IIPHelper : public ISymBase
{
public:
	// Determines if the given raw IP is valid
	virtual Result IsValidIP(LPCSTR sIP,bool& bOut) const throw() = 0;
	virtual Result IsValidIP(LPCWSTR sIP,bool& bOut) const throw() = 0;

	// Determines if the given domain name is valid.
	virtual Result IsValidDomainName(LPCSTR sIP,bool &bOut) const throw() = 0;
	virtual Result IsValidDomainName(LPCWSTR sIP,bool &bOut) const throw() = 0;

	// Determines if the given domain name resolves vis DNS.
	// This relies on the system's DNS lookup, and may be slow.
	virtual Result DoesDomainNameResolve(LPCSTR sIP,bool& bOut) const throw() = 0;
	virtual Result DoesDomainNameResolve(LPCWSTR sIP,bool& bOut) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_IPHelper, 
						0x47b7c733, 0xb062, 0x4ff5, 0x9d, 0xb2, 0x2a, 0x24, 0x39, 0xb1, 0x95, 0x2c);
typedef CSymPtr<IIPHelper> IIPHelperPtr;

///////////////////////////////////////////////////////////////////////////////
// IPItems store information about an IP address or group of IP addresses.
// This refers only to IPv4 IP items.
class IIPItem : public ISymBase
{
public:
	// The types of IPItems
	enum IPType
	{
		IPTypeStart = 0,
		IPTypeRaw = IPTypeStart,	//e.g. 192.168.0.1
		IPTypeNamed,				//e.g. www.symantec.com
		IPTypeRange,				//e.g. 192.168.0.1-192.168.0.50 or 1.1.1.1-2.50.199.25
		IPTypeNetwork,				//e.g. 192.168.0.1 255.255.255.0
		IPTypeEnd = IPTypeNetwork
	} ;

	// Getters/Setters
	// Warning: You may get properties of the incorrect type without errors.
	//	For example, if the type is IPTypeRaw, you can call GetNamedIP without error, but
	//	the result is undefined.
	// Calling setters change the type. For example, calling SetNetworkMask sets the type
	// to network.
	virtual Result GetType(IPType& pVal) const throw() = 0;
	virtual Result SetType(IPType newVal) throw() = 0;
	virtual Result GetNamedIP(LPSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result GetNamedIP(LPWSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result SetNamedIP(LPCSTR newVal) throw() = 0;
	virtual Result SetNamedIP(LPCWSTR newVal) throw() = 0;
	virtual Result GetNetworkIP(LPSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetNetworkIP(LPWSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetNetworkIP(DWORD& iIP) const throw() = 0;
	virtual Result SetNetworkIP(LPCSTR newVal) throw() = 0;
	virtual Result SetNetworkIP(LPCWSTR newVal) throw() = 0;
	virtual Result SetNetworkIP(DWORD iIP) throw() = 0;
	virtual Result GetNetworkMask(LPSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetNetworkMask(LPWSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetNetworkMask(DWORD& iIP) const throw() = 0;
	virtual Result SetNetworkMask(LPCSTR newVal) throw() = 0;
	virtual Result SetNetworkMask(LPCWSTR newVal) throw() = 0;
	virtual Result SetNetworkMask(DWORD iIP) throw() = 0;
	virtual Result GetRangeStart(LPSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetRangeStart(LPWSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetRangeStart(DWORD& iIP) const throw() = 0;
	virtual Result SetRangeStart(LPCSTR newVal) throw() = 0;
	virtual Result SetRangeStart(LPCWSTR newVal) throw() = 0;
	virtual Result SetRangeStart(DWORD iIP) throw() = 0;
	virtual Result GetRangeEnd(LPSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetRangeEnd(LPWSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetRangeEnd(DWORD& iIP) const throw() = 0;
	virtual Result SetRangeEnd(LPCSTR newVal) throw() = 0;
	virtual Result SetRangeEnd(LPCWSTR newVal) throw() = 0;
	virtual Result SetRangeEnd(DWORD iIP) throw() = 0;
	virtual Result GetRawIP(LPSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetRawIP(LPWSTR pVal,DWORD& dwSize) const throw() = 0;
	virtual Result GetRawIP(DWORD& iIP) const throw() = 0;
	virtual Result SetRawIP(LPCSTR newVal) throw() = 0;
	virtual Result SetRawIP(LPCWSTR newVal) throw() = 0;
	virtual Result SetRawIP(DWORD iIP) throw() = 0;

	// Determine whether the contents of two items are identical.
	virtual Result IsEqual(const IIPItem* pIPItem, bool& bOut) const throw() = 0;

	// Expresses the IPItem as a string
	virtual Result GetAsString(LPSTR sString,DWORD& dwSize) const throw() = 0;
	virtual Result GetAsString(LPWSTR sString,DWORD& dwSize) const throw() = 0;

	// Determines if the current data are valid or not.
	virtual Result IsValid(bool& bOut) const throw() = 0;

	// Create a copy of an existing item.
	virtual Result Assign(const IIPItem* pFrom) throw() = 0;
    //Access attached user defined properties on this location
    virtual Result GetCustomProperties(cc::IKeyValueCollection*& pProperties) throw() = 0;
} ;

SYM_DEFINE_INTERFACE_ID(IID_IPItem, 
						0xd6a51b39, 0x3d5f, 0x4641, 0x9b, 0x90, 0x5e, 0xa6, 0x5, 0x2b, 0x8, 0x60);
typedef CSymPtr<IIPItem> IIPItemPtr;
typedef CSymQIPtr<IIPItem, &IID_IPItem> IIPItemQIPtr;

///////////////////////////////////////////////////////////////////////////////
class IIPList : public ISymBase
{
public:
	// Get a specific item in the list
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	// Changes made to the returned item affect the list.
	virtual Result GetItem(long lIndex, IIPItem*& pOut) const throw() = 0;

	// Get the number of items in the list.
	virtual Result GetItemCount(long &lItemCount) const throw() = 0;

	// Add or insert an item to the list.
	// The actual item is added to the list, not a copy, so subsequent changes
	// made to the item affect the list.
	// The reference count on the object is incremented to show ownership by the list. Callers must
	// still Release their copy of the item.
	virtual Result AppendItem(const IIPItem* pItem) throw() = 0;
	virtual Result InsertItem(long lIndex, const IIPItem* pItem) throw() = 0;

	// Append a list to the end of the current list. Items are copied out of the existing list.
	virtual Result AppendList(const IIPList* pList) throw() = 0;

	// Delete an item from the list.
	virtual Result DeleteItem(long lIndex) throw() = 0;
	virtual Result DeleteItem(const IIPItem* pItem) throw() = 0;

	// Delete all items from the list.
	virtual Result DeleteAll() throw() = 0;

	// Create a copy of the list.
	virtual Result Assign(const IIPList* pFrom) throw() = 0;

	// Determine whether the contents of two lists are identical.
	virtual Result IsEqual(const IIPList* pList, bool& bOut) const throw() = 0;

	// Delete any items from the list that overlap the given item.
	virtual Result RemoveOverlappingIPs(const IIPItem* pIPItem) throw() = 0;

	// Determines if a matching item exists in the list. If one is found, it is returned in
	// pFound and the method returns NoError. Changes to the returned item
	// affect the list. The returned object is reference counted; it is the caller's responsibility to call Release.
	// If no match is found, the method returns NotFound.
	virtual Result IsIPInList(const IIPItem* pItem, IIPItem*& pFound) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_IPList, 
						0xf77562c8, 0x951a, 0x4102, 0xaf, 0x5b, 0x16, 0x4f, 0xd1, 0xce, 0xe0, 0x6b);
typedef CSymPtr<IIPList> IIPListPtr;
typedef CSymQIPtr<IIPList, &IID_IPList> IIPListQIPtr;

///////////////////////////////////////////////////////////////////////////////
// ZoneItems store IPLists for use in Zones. A Zone (e.g. Trusted) may contain
// multiple ZoneItems, for example the Trusted Zone for the Office location under
// the user's control, or the Restricted Zone for the Default location under the
// administrator's control (used for Enterprise products).
class IZoneItem : public ISymBase
{
public:
	// Create a copy of an existing item.
	virtual Result Assign(const IZoneItem* pFrom) throw() = 0;

	// Determine whether the contents of two items are identical.
	virtual Result IsEqual(const IZoneItem* pLocation, bool& bOut) const throw() = 0;

	// Getters/Setters
	virtual Result GetIPList(IIPList*& pIPList) const throw() = 0;
	virtual Result SetIPList(const IIPList* pIPList) throw() = 0;
    virtual Result GetLockState(bool& bLocked) const throw() = 0;
    virtual Result SetLockState(bool bLocked) throw() = 0;
	virtual Result SetLocation(const ILocation* pLocation) throw() = 0;
    virtual Result GetLocation(ILocation*& pLocation) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_ZoneItem, 
						0xcf1ca016, 0x8e73, 0x4989, 0x80, 0xbb, 0xee, 0xb5, 0xd6, 0x5e, 0x61, 0xfc);
typedef CSymPtr<IZoneItem> IZoneItemPtr;
typedef CSymQIPtr<IZoneItem, &IID_ZoneItem> IZoneItemQIPtr;

///////////////////////////////////////////////////////////////////////////////
class IZoneItemList : public ISymBase
{
public:
	// Get a specific item in the list
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	// Changes made to the returned item affect the list.
	virtual Result GetItem(long lIndex, IZoneItem*& pOut) const throw() = 0;

	// Get the number of items in the list.
	virtual Result GetItemCount(long &lItemCount) const throw() = 0;

	// Add or insert an item to the list.
	// The actual item is added to the list, not a copy, so subsequent changes
	// made to the item affect the list.
	// The reference count on the object is incremented to show ownership by the list. Callers must
	// still Release their copy of the item.
	// InsertItem may be quite a bit slower than AppendItem.
	virtual Result AppendItem(const IZoneItem* pItem) throw() = 0;
	virtual Result InsertItem(long lIndex, const IZoneItem* pItem) throw() = 0;

	// Append a list to the end of the current list. Items are copied out of the existing list.
	virtual Result AppendList(const IZoneItemList* pList) throw() = 0;

	// Delete an item from the list.
	virtual Result DeleteItem(long lIndex) throw() = 0;
	virtual Result DeleteItem(const IZoneItem* pItem) throw() = 0;

	// Delete all items from the list.
	virtual Result DeleteAll() throw() = 0;

	// Create a copy of the list.
	virtual Result Assign(const IZoneItemList* pFrom) throw() = 0;       

	// Determine whether the contents of two lists are identical.
	virtual Result IsEqual(const IZoneItemList* pList, bool& bOut) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_ZoneItemList, 
						0x93112970, 0xd144, 0x4c4d, 0x83, 0x9a, 0xda, 0xc9, 0x1e, 0xe3, 0xf9, 0x80);
typedef CSymPtr<IZoneItemList> IZoneItemListPtr;
typedef CSymQIPtr<IZoneItemList, &IID_ZoneItemList> IZoneItemListQIPtr;

///////////////////////////////////////////////////////////////////////////////
// A Zone is a group IP addresses. The zone contains information about multiple
// locations and Admin/User.
class IZone : public ISymBase
{
public:
	// Getters/Setters
	virtual Result GetName(LPSTR sName,DWORD& dwSize) const throw() = 0;
	virtual Result GetName(LPWSTR sName,DWORD& dwSize) const throw() = 0;
	virtual Result SetName(LPCSTR sName) throw() = 0;
	virtual Result SetName(LPCWSTR sName) throw() = 0;

	// Create a copy of the given item.
	virtual Result Assign(const IZone* pFrom) throw() = 0;

	// Determine whether the contents of two items are identical.
	virtual Result IsEqual(const IZone* pZone, bool& bOut) const throw() = 0;

	// Get/Set the IPList for a given location/locked status.
	virtual Result GetAddressRange(const ILocation* pLocation,bool bLocked,IIPList *&pAddressRangeList) throw() = 0;
	virtual Result SetAddressRange(const ILocation* pLocation,bool bLocked,const IIPList* pAddressRangeList) throw() = 0;

	// Get/Set the IPLists for all location/lock status combinations.
	virtual Result SetZoneItemList(const IZoneItemList* pVal) throw() = 0;
	virtual Result GetZoneItemList(IZoneItemList*& pVal) const throw() = 0;
    //Access attached user defined properties on this location
    virtual Result GetCustomProperties(cc::IKeyValueCollection*& pProperties) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_Zone, 
						0x133aad2b, 0xc154, 0x4f0a, 0xa4, 0xc9, 0xb5, 0x5e, 0x19, 0xe3, 0x47, 0x16);
typedef CSymPtr<IZone> IZonePtr;
typedef CSymQIPtr<IZone, &IID_Zone> IZoneQIPtr;

///////////////////////////////////////////////////////////////////////////////
// A list of Zones. This might hold the Trusted, Restricted, Local and Scanning Zones.
class IZoneList : public ISymBase
{
public:
	// Get a specific item in the list
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	// Changes made to the returned item affect the list.
	virtual Result GetItem(long lIndex, IZone*& pOut) const throw() = 0;

	// Get the number of items in the list.
	virtual Result GetItemCount(long &lItemCount) const throw() = 0;

	// Add or insert an item to the list.
	// The actual item is added to the list, not a copy, so subsequent changes
	// made to the item affect the list.
	// The reference count on the object is incremented to show ownership by the list. Callers must
	// still Release their copy of the item.
	// InsertItem may be significantly slower than AppendItem.
	virtual Result AppendItem(const IZone* pItem) throw() = 0;
	virtual Result InsertItem(long lIndex, const IZone* pItem) throw() = 0;

	// Append a list to the end of the current list. Items are copied out of the existing list.
	virtual Result AppendList(const IZoneList* list) throw() = 0;

	// Delete an item from the list.
	virtual Result DeleteItem(long lIndex) throw() = 0;
	virtual Result DeleteItem(const IZone* pItem) throw() = 0;

	// Delete all items from the list.
	virtual Result DeleteAll() throw() = 0;

	// Return a Zone given the name. Returns NoError if the item is found,
	// NotFound if not.
	// Changes made to the returned item affect the list.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	virtual Result FindZone(LPCSTR sName, IZone*& pOut) const throw() = 0;
	virtual Result FindZone(LPCWSTR sName, IZone*& pOut) const throw() = 0;

	// Create a copy of the list.
	virtual Result Assign(const IZoneList* pFrom) throw() = 0;       

	// Determine whether the contents of two lists are identical.
	virtual Result IsEqual(const IZoneList* pList, bool& bOut) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_ZoneList, 
						0xf65733c0, 0x9381, 0x46c5, 0xb5, 0xd8, 0x75, 0xca, 0x10, 0x40, 0x2b, 0x63);
typedef CSymPtr<IZoneList> IZoneListPtr;
typedef CSymQIPtr<IZoneList, &IID_ZoneList> IZoneListQIPtr;

///////////////////////////////////////////////////////////////////////////////
// IPort
// Stores information about communication ports. For example, browser traffic
// is typically handled on port 80.
class IPort : public ISymBase
{
public:
	// The types of Ports
	enum PortType {
		eSinglePort,	// A single port, like port 80
		ePortRange		// A range of ports, like 111-125
	} ;

	// Get/Set which type of IPort this represents: a single port or a range of ports.
	virtual Result GetType(PortType &val) const throw() = 0;
	virtual Result SetType(PortType newVal) throw() = 0;

	// Getters/Setters for IPorts with type eSinglePort
	// Warning: you may get/set properties of the incorrect type without errors.
	// Calling setters change the type. For example, calling SetNetworkMask sets the type
	// to network.
	virtual Result GetPortNum(DWORD &val) const throw() = 0;
	virtual Result SetPortNum(DWORD newVal) throw() = 0;

	// Getters/Setters for IPorts with type ePortRange
	// Warning: you may get/set properties of the incorrect type without errors.
	// Calling setters change the type. For example, calling SetNetworkMask sets the type
	// to network.
	virtual Result GetRangeStart(DWORD &val) const throw() = 0;
	virtual Result SetRangeStart(DWORD newVal) throw() = 0;
	virtual Result GetRangeEnd(DWORD &val) const throw() = 0;
	virtual Result SetRangeEnd(DWORD newVal) throw() = 0;

	// Determine whether the contents of two items are identical.
	virtual Result IsEqual(const IPort* pPort, bool& bOut) const throw() = 0;

	// Create a copy of an existing item.
	virtual Result Assign(const IPort* pFrom) throw() = 0;
    //Access attached user defined properties on this location
    virtual Result GetCustomProperties(cc::IKeyValueCollection*& pProperties) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_Port, 
						0x6e8a0997, 0xa7cf, 0x47ed, 0xb7, 0xcd, 0xc6, 0x8b, 0xb9, 0xca, 0xc5, 0x7);
typedef CSymPtr<IPort> IPortPtr;
typedef CSymQIPtr<IPort, &IID_Port> IPortQIPtr;

///////////////////////////////////////////////////////////////////////////////
// IPortList
class IPortList : public ISymBase
{
public:
	// Get a specific item in the list
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	// Changes made to the returned item affect the list.
	virtual Result GetItem(long lIndex, IPort*& pOut) const throw() = 0;

	// Get the number of items in the list.
	virtual Result GetItemCount(long &lItemCount) const throw() = 0;

	// Add or insert an item to the list.
	// The actual item is added to the list, not a copy, so subsequent changes
	// made to the item affect the list.
	// The reference count on the object is incremented to show ownership by the list. Callers must
	// still Release their copy of the item.
	// InsertItem may be significantly slower than AppendItem.
	virtual Result AppendItem(const IPort* pItem) throw() = 0;
	virtual Result InsertItem(long lIndex, const IPort* pItem) throw() = 0;

	// Append a list to the end of the current list. Items are copied out of the existing list.
	virtual Result AppendList(const IPortList* pList) throw() = 0;

	// Delete an item from the list.
	virtual Result DeleteItem(long lIndex) throw() = 0;
	virtual Result DeleteItem(const IPort* pItem) throw() = 0;

	// Delete all items from the list.
	virtual Result DeleteAll() throw() = 0;

	// Create a copy of the list.
	virtual Result Assign(const IPortList* pFrom) throw() = 0;       

	// Determine whether the contents of two lists are identical.
	virtual Result IsEqual(const IPortList* pList, bool& bOut) const throw() = 0;

	// Determines if a matching item exists in the list. If one is found, it is returned in
	// pFound and the method returns NoError. Changes to the returned item
	// affect the list. The returned object is reference counted; it is the caller's responsibility to call Release.
	// If no match is found, the method returns NotFound.
	virtual Result IsPortInList(const IPort* port, IPort*& pFound) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_PortList, 
						0xbbced78d, 0xe89c, 0x4bea, 0x96, 0x28, 0x3d, 0xfa, 0x3c, 0x74, 0x24, 0xbb);
typedef CSymPtr<IPortList> IPortListPtr;
typedef CSymQIPtr<IPortList, &IID_PortList> IPortListQIPtr;

///////////////////////////////////////////////////////////////////////////////
// IRule
// 
class IRule : public ISymBase
{
public:
	// The rule description
	virtual Result GetDescription(LPSTR sDescription,DWORD& dwSize) const throw() = 0;
	virtual Result GetDescription(LPWSTR sDescription,DWORD& dwSize) const throw() = 0;
	virtual Result SetDescription(LPCSTR sDescription) throw() = 0;
	virtual Result SetDescription(LPCWSTR sDescription) throw() = 0;

	// These are data members for passing information via the alerting mechanism when the rule is triggered.
	virtual Result GetLookupKey(LPSTR sLookupKey,DWORD& dwSize) const throw() = 0;
	virtual Result GetLookupKey(LPWSTR sLookupKey,DWORD& dwSize) const throw() = 0;
	virtual Result SetLookupKey(LPCSTR sLookupKey) throw() = 0;
	virtual Result SetLookupKey(LPCWSTR sLookupKey) throw() = 0;
	virtual Result GetSummary(LPSTR sSummary,DWORD& dwSize) const throw() = 0;
	virtual Result GetSummary(LPWSTR sSummary, DWORD& dwSize) const throw() = 0;
	virtual Result SetSummary(LPCSTR sSummary) throw() = 0;
	virtual Result SetSummary(LPCWSTR sSummary) throw() = 0;
	virtual Result GetDetails(LPSTR sDetailsKey, DWORD& dwSize) const throw() = 0;
	virtual Result GetDetails(LPWSTR sDetailsKey, DWORD& dwSize) const throw() = 0;
	virtual Result SetDetails(LPCSTR sDetailsKey) throw() = 0;
	virtual Result SetDetails(LPCWSTR sDetailsKey) throw() = 0;

	// Method to deal with which location the rule exists in. This is distinct from InUse locations.
	// A rule may exist in a location, but not be in use in that location. In this case the rule
	// is displayed in that location as unchecked.
    virtual Result AddToLocation(const ILocation* pLocation) throw() = 0;
	virtual Result AddToLocations(const ILocationList* pLocationList) throw() = 0;
    virtual Result GetLocationList(ILocationList*& pLocationList) const throw() = 0;
    virtual Result SetLocationList(const ILocationList* pLocationList) throw() = 0;
    virtual Result IsLocationActive(const ILocation* pLocation, bool& bOut) const throw() = 0;
    virtual Result RemoveFromLocation(const ILocation* pLocation) throw() = 0;
    virtual Result ClearLocations() throw() = 0;
	virtual Result GetNumLocations(long& lOut) const throw() = 0;
	virtual Result HasActiveLocations(bool& bOut) const throw() = 0;

	// Flags to determine if the rule is in use (checked) or not.
    virtual Result AddInUseLocation(const ILocation* pLocation) throw() = 0;
	virtual Result AddInUseLocations(const ILocationList* pLocationList) throw() = 0;
	virtual Result GetInUseList(ILocationList*& pInUseList) const throw() = 0;
	virtual Result SetInUseList(const ILocationList* pInUseList) throw() = 0;
    virtual Result IsInUseForLocation(const ILocation* pLocation, bool& bInUse) const throw() = 0;
    virtual Result RemoveInUseLocation(const ILocation* pLocation) throw() = 0;
    virtual Result ClearInUseLocations() throw() = 0;

	// Determines what the rule does: block, permit, etc.
	enum Action {
		Block = 1,
		Permit,
		Ignore,
		Monitor
	} ;
	virtual Result GetAction(Action& lAction) const throw() = 0;
	virtual Result SetAction(Action lAction) throw() = 0;

	// Determines which protocol the rule applies to: TCP, UDP, etc
	enum Protocol {
		TCPUDP = 0,
		ICMP = 1,
		TCP = 6,
		UDP = 17
	} ;
	virtual Result GetProtocol(Protocol& lProtocol) const throw() = 0;
	virtual Result SetProtocol(Protocol lProtocol) throw() = 0;

	// Determines when what conditions the rule logs. The threshold is the number
	// of times the rule is used before logging starts.
	enum LoggingType {
		None = 0,
		Logging = 1,
		Dialog = 16,
		AlertTracker = 32
	} ;
	virtual Result GetLogging(LoggingType& eLogging) const throw() = 0;
	virtual Result SetLogging(LoggingType eLogging) throw() = 0;
	virtual Result GetLogThreshold(int& pVal) const throw() = 0;
	virtual Result SetLogThreshold(int newVal) throw() = 0;

	// Determines the rule's category. See ICategory->GetValue().
	virtual Result GetCategory(ICategory*& pCategory) const throw() = 0;
	virtual Result SetCategory(const ICategory* pCategory) throw() = 0;

	// Determines if the rule applies to incoming traffic, outgoing traffic, or both.
	enum Direction {
		Out = 1,
		In,
		Both
	} ;
	virtual Result GetDirection(Direction& pVal) const throw() = 0;
	virtual Result SetDirection(Direction newVal) throw() = 0;

    // Is the rule locked (not changable by users) or unlocked. This is used
    // by enterprise products.
    virtual Result GetLockState(bool& bLocked) const throw() = 0;
    virtual Result SetLockState(bool bLocked) throw() = 0;

	// The list of IPs/Ports to which this rule.
	// Changes made to the returned item affect the list.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	virtual Result GetRemoteIPs(IIPList*& pVal) const throw() = 0;
	virtual Result SetRemoteIPs(const IIPList* pVal) throw() = 0;
	virtual Result GetLocalIPs(IIPList*&pVal) const throw() = 0;
	virtual Result SetLocalIPs(const IIPList* pVal) throw() = 0;
	virtual Result GetLocalPorts(IPortList*& pVal) const throw() = 0;
	virtual Result SetLocalPorts(const IPortList* pVal) throw() = 0;
	virtual Result GetRemotePorts(IPortList*& pVal) const throw() = 0;
	virtual Result SetRemotePorts(const IPortList* pVal) throw() = 0;

	// Create a copy of an existing item.
	virtual Result Assign(const IRule* pFrom) throw() = 0;

	// Determine whether the contents of two items are identical.
    // IsEqual always ignores differences in location and in use location.
    // IsEqualEx may optionally ignore locations or InUse locations.
	virtual Result IsEqual(const IRule* pRule, bool& bOut) const throw() = 0;
	virtual Result IsEqualEx(const IRule* pRule, bool bIncludeLocations, bool bIncludeIsUse, bool& bOut) const throw() = 0;
    //Access attached user defined properties on this location
    virtual Result GetCustomProperties(cc::IKeyValueCollection*& pProperties) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_Rule, 
						0x50440b4a, 0x9314, 0x4339, 0x98, 0xd4, 0xcd, 0xf4, 0x49, 0x85, 0xaa, 0xed);
typedef CSymPtr<IRule> IRulePtr;
typedef CSymQIPtr<IRule, &IID_Rule> IRuleQIPtr;

///////////////////////////////////////////////////////////////////////////////
// IRuleList
class IRuleList : public ISymBase
{
public:
	// Get a specific item in the list
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	// If a filter is set (see SetFilter below) GetItem() returns the nth filtered item.
	// Changes made to the returned item affect the list.
	virtual Result GetItem(long lIndex, IRule*& pOut) const throw() = 0;

	// Get the number of items in the filtered list.
	virtual Result GetItemCount(long &lItemCount) const throw() = 0;

	// Add or insert an item to the list.
	// The actual item is added to the list, not a copy, so subsequent changes
	// made to the item affect the list.
	// InsertItem's lIndex refers to an index in the filtered list.
	// InsertItem is likely to be more expensive than AppendItem.
	// Appended or inserted items are added to the filtered list if they
	// meet the filter criteria.
	// Be cafeful adding item if you're iterating through a list. Even AppendItem
	// may insert in the middle of the list to maintain priority order (all locked rules preceed unlocked rules).
	// The reference count on the object is incremented to show ownership by the list. Callers must
	// still Release their copy of the item.
	// DO NOT change the locked status of an item already in the list. This list will not be
	// resorted! Instead remove the item, change the lock status, and add the item.
	virtual Result AppendItem(const IRule* pItem) throw() = 0;
	virtual Result InsertItem(long lIndex, const IRule* pItem) throw() = 0;

	// Append a list to the end of the current list. Items are copied out of the existing list.
	// Be cafeful adding item if you're iterating through a list. Even AppendItem
	// may insert in the middle of the list to maintain priority order (all locked rules preceed unlocked rules).
	virtual Result AppendList(const IRuleList* pList) throw() = 0;

	// Delete an item from the list. If a filter is set (see SetFilter below)
	// the item is deleted only from the filtered location(s).
	// If a filter is set, the filter is reapplied; any items that have been
	// modified by previous calls to change their filtered status is applied after the
	// DeleteItem.
	virtual Result DeleteItem(long lIndex) throw() = 0;
	virtual Result DeleteItem(const IRule* pItem) throw() = 0;

	// Delete all items from the list. If the list is filtered, only filtered
	// items are deleted.
	virtual Result DeleteAll() throw() = 0;

	// Handle the filter. This causes GetItemCount, GetItem, DeleteItem, and
	// DeleteAll to restrict items to those in the filtered list.
	// Pass in NULL or an empty pFilter to SetFilter clear filtering.
	// Changes made to a filter received from GetFilter do not affect
	// filtering until another call to SetFilter is made.
	// Changes to an item that would add/remove it to/from the filter does not
	// add/remove until the filter is reapplied. Appended/Inserted/Deleted items are
	// added/removed from the filter.
	virtual Result SetFilter(const IFilter* pFilter) throw() = 0;
	virtual Result GetFilter(IFilter*& pFilter) const throw() = 0;

	// Create a copy of an existing item.
	virtual Result Assign(const IRuleList* pFrom) throw() = 0;

	// Determine whether the contents of two lists are identical.
	virtual Result IsEqual(const IRuleList* pList, bool& bOut) const throw() = 0;

	// Find a rule whose contents match the given rule.
	// Returns NoError if the item is found, NotFound if not.
	// Changes made to the returned item affect the list.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	virtual Result FindMatchingRule(const IRule* pIn, IRule*& pOut) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_RuleList, 
						0x6413331, 0xda5, 0x46bf, 0x90, 0xc3, 0x40, 0xe0, 0x84, 0x8b, 0xd2, 0xb7);
typedef CSymPtr<IRuleList> IRuleListPtr;
typedef CSymQIPtr<IRuleList, &IID_RuleList> IRuleListQIPtr;

///////////////////////////////////////////////////////////////////////////////
// IApplication
class IApplication : public ISymBase
{
public:
	// Get/Set the friendly application name.
	virtual Result GetDescription(LPSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result GetDescription(LPWSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result SetDescription(LPCSTR newVal) throw() = 0;
	virtual Result SetDescription(LPCWSTR newVal) throw() = 0;

	// Get/Set the application full path.
	// In ccFWSettg from common client 2.0, this was Get/SetName
	virtual Result GetPath(LPSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result GetPath(LPWSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result SetPath(LPCSTR newVal) throw() = 0;
	virtual Result SetPath(LPCWSTR newVal) throw() = 0;

	// Get the application's fingerprint. In general, fingerprints are set
	// automatically by SymNeti.
	virtual Result GetFingerprint(LPSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result GetFingerprint(LPWSTR pVal, DWORD& dwSize) const throw() = 0;
	virtual Result SetFingerprint(LPCSTR newVal) throw() = 0;
	virtual Result SetFingerprint(LPCWSTR newVal) throw() = 0;

	// Get the application's rule list.
	// Changes made to the returned item affect the list.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	virtual Result GetRules(IRuleList*& pRuleList) const throw() = 0;

	// Set the application's rule list.
	// The given rule list's reference count is incremented and stored in the application.
	// Subsequent changes to the rule list affect the application. Callers are responsible
	// for releasing their own copy of the rule list.
	virtual Result SetRules(const IRuleList* pRuleList) throw() = 0;

	// Create a copy of an existing item.
	virtual Result Assign(const IApplication* pFrom) throw() = 0;

	// Determines if the application has any rules for the given location
	virtual Result HasRulesForLoc(const ILocation* pLocation, bool &bRet) const throw() = 0;

	// Returns the access type (e.g. Permit All, Block All, Automatic, Manual) for the application
	// in the given location.
	enum AccessType {
		Cancel = -1,
		Auto,
		Permit,
		Block,
		Custom,
		Ask
	} ;

	virtual Result GetAccessType(const ILocation* pLocation, AccessType& eAccessType) const throw() = 0;

	// Methods to handle Auto Config.
	// Auto config is enabled or disabled in each location. An app with Auto Config
	// on gets its rules from ccALE.
	virtual Result AddAutoConfigLocation(const ILocation* pLocation) throw() = 0;
    virtual Result SetAutoConfigLocations(const ILocationList* pLocationList) throw() = 0;
	virtual Result RemoveAutoConfigLocation(const ILocation* pLocation) throw() = 0;
	virtual Result RemoveAutoConfigLocations(const ILocationList* pLocationList) throw() = 0;
	virtual Result ClearAutoConfigLocations() throw() = 0;
	virtual Result GetAutoConfigLocationList(ILocationList*& pLocationList) const throw() = 0;

	// Merges the rules from a given app. This is used by ccALE, which
	// 1) creates a temporary application
	// 2) fills it in with the automatic rules
	// 3) calls MergeAppRules to add the ale rules to the application
    // MergeAppRules returns InvalidArgument if pApp's rules aren't the same
    // for all locations.
	virtual Result MergeAppRules(const IApplication* pApp) throw() = 0;

	// Gets the first location for which the application has rules.
	virtual Result GetFirstLocation(ILocation*& pLocation) const throw() = 0;

	// Gets a List of all locations for which the application has rules.
	// Modifying this list has no effect on the application. Instead
	// modify the location of the applications rules.
	virtual Result GetLocationList(ILocationList*& pLocationList) const throw() = 0;

	// Determine whether the contents of two items are identical.
	virtual Result IsEqual(const IApplication* pApp, bool& bOut) const throw() = 0;
    //Access attached user defined properties on this location
    virtual Result GetCustomProperties(cc::IKeyValueCollection*& pProperties) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_Application, 
						0x917f8e74, 0x7a3, 0x47f9, 0x8e, 0xf5, 0x48, 0x9, 0x26, 0x90, 0x22, 0xad);
typedef CSymPtr<IApplication> IApplicationPtr;
typedef CSymQIPtr<IApplication, &IID_Application> IApplicationQIPtr;

///////////////////////////////////////////////////////////////////////////////
// IApplicationList
class IApplicationList : public ISymBase
{
public:
	// Get a specific item in the list
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	// If a filter is set (see SetFilter below) GetItem() returns the nth item
	// that is in the filtered location.
	virtual Result GetItem(long lIndex, IApplication*& pOut) const throw() = 0;

	// Get the number of items in the filtered list.
	virtual Result GetItemCount(long &lItemCount) const throw() = 0;

	// Add an item to the list.
	// The actual item is added to the list, not a copy, so subsequent changes
	// made to the item affect the list.
	// The reference count on the object is incremented to show ownership by the list. Callers must
	// still Release their copy of the item.
	// Items are added even if they are not in the current filter. The value returned by 
	// GetItemCount changes if app is in the filter.
	virtual Result AppendItem(const IApplication* pApp) throw() = 0;

	// Append a list to the end of the current list. Items are copied out of the existing list.
	// Items are added even if they are not in the current filter. The value returned by 
	// GetItemCount changes if app is in the filter.
	virtual Result AppendList(const IApplicationList* pList) throw() = 0;

	// Delete an item from the list. If a filter is set (see SetFilter below)
	// the item is deleted only from the filtered locations.
	// If a filter is set, the filter is reapplied to both the appliation list
	// and the contained rule list; any items that have been
	// modified to change their filtered status is applied after the
	// DeleteItem.
	virtual Result DeleteItem(long lIndex) throw() = 0;
	virtual Result DeleteItem(const IApplication* pApp) throw() = 0;

	// Delete all items from the list. If a filter is set (see SetFilter below)
	// the items are deleted only from the filtered locations.
	virtual Result DeleteAll() throw() = 0;

	// Finds an application with the given full path and fingerprint. 
	// Returns NoError if one is found, NotFound otherwise. 
	// Changes made to the returned item affect the list.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	virtual Result FindApplication(LPCSTR sPath, IApplication*& pApp) const throw() = 0;
	virtual Result FindApplication(LPCWSTR sPath, IApplication*& pApp) const throw() = 0;
	virtual Result FindApplicationEx(LPCSTR sPath, LPCSTR bsFingerprint, IApplication*& pApp) const throw() = 0;
	virtual Result FindApplicationEx(LPCWSTR sPath, LPCWSTR bsFingerprint, IApplication*& pApp) const throw() = 0;

	// Create a copy of an existing item.
	virtual Result Assign(const IApplicationList* pAppList) throw() = 0;

	// Determine whether the contents of two lists are identical.
	virtual Result IsEqual(const IApplicationList* pList, bool& bOut) const throw() = 0;

	// Handle the filter. This causes GetItemCount, GetItem, DeleteItem, and
	// DeleteAll to restrict items to those in the filtered list.
	// Pass in NULL or an empty pFilter to SetFilter clear filtering.
	// Changes made to a filter received from GetFilter do not affect
	// filtering until another call to SetFilter is made.
	// Locked status settings in the filter are ignored.
	// Changes to an item that would add/remove it to/from the filter does not
	// add/remove until the filter is reapplied. Appended/Inserted/Deleted items are
	// added/removed from the filter.
    // Filters are not passed to sub-items. For example, if a filter is applied to an
    // IApplicationList, then an IApplication is retreived (via GetItem), then
    // a rule list is retreived from the IApplication (via GetRules), the rule list does
    // not have a filter applied.
	virtual Result SetFilter(const IFilter* pFilter) throw() = 0;
	virtual Result GetFilter(IFilter*& pFilter) const throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_ApplicationList, 
						0xe39f8106, 0x4bd4, 0x4ce5, 0x80, 0x6a, 0x74, 0x76, 0xb7, 0x7a, 0x58, 0x88);
typedef CSymPtr<IApplicationList> IApplicationListPtr;
typedef CSymQIPtr<IApplicationList, &IID_ApplicationList> IApplicationListQIPtr;

///////////////////////////////////////////////////////////////////////
class IFirewallSettingChangeNotify : public ISymBase
{
public:
    virtual void OnFirewallSettingChange(SettingChangeType type) throw() = 0;
};
SYM_DEFINE_INTERFACE_ID(IID_FirewallSettingChangeNotify, 
						0x761dd808, 0xe5b, 0x45d9, 0xaa, 0xff, 0xd, 0xc1, 0x44, 0x4a, 0x20, 0x2d);

typedef CSymPtr<IFirewallSettingChangeNotify> IFirewallSettingChangeNotifyPtr;
typedef CSymQIPtr<IFirewallSettingChangeNotify, &IID_FirewallSettingChangeNotify> IFirewallSettingChangeNotifyQIPtr;

///////////////////////////////////////////////////////////////////////////////
class IFirewallSettings: public ISymBase
{
public:
	// Gets the system rules. 
	// The returned object is reference counted; it is the caller's responsibility to call Release.
    // The second overload will be faster if you have a copy of the current system locations around.
	virtual Result GetSystemRules(IRuleList*& pRuleList) const throw() = 0;
	virtual Result GetSystemRules(IRuleList*& pRuleList, const ILocationList* pCurrentLocations) const throw() = 0;

	// Commits the system rules to SymNeti.
    // Rules not in any location are removed.
	virtual Result SetSystemRules(IRuleList* pRuleList) throw() = 0;

	// Gets the trojan rules. 
	// The returned object is reference counted; it is the caller's responsibility to call Release.
    // The second overload will be faster if you have a copy of the current system locations around.
	virtual Result GetTrojanRules(IRuleList*& pVal) const throw() = 0;
	virtual Result GetTrojanRules(IRuleList*& pVal, const ILocationList* pCurrentLocations) const throw() = 0;

	// Commits the trojan rules to SymNeti.
    // Rules not in any location are removed.
	virtual Result SetTrojanRules(IRuleList* pVal) throw() = 0;

	// Gets a list of applications.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
    // The second overload will be faster if you have a copy of the current system locations around.
	virtual Result GetApplicationRules(IApplicationList*& pVal) const throw() = 0;
	virtual Result GetApplicationRules(IApplicationList*& pVal, const ILocationList* pCurrentLocations) const throw() = 0;

	// Commits the application rules to SymNeti.
    // Note that the IApplicationList is NOT const. Fingerprints are filled in for applications
    // that don't already have them.
    // Rules not in any location are removed. Applications without any rules are removed.
	virtual Result SetApplicationRules(IApplicationList* pVal) throw() = 0;

	// Gets a list of zones.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
    // The second overload will be faster if you have a copy of the current system locations around.
	virtual Result GetZones(IZoneList*& pVal) const throw() = 0;
	virtual Result GetZones(IZoneList*& pVal, const ILocationList* pCurrentLocations) const throw() = 0;

	// Commits the zones to SymNeti.
	virtual Result SetZones(const IZoneList* pVal) throw() = 0;

	// Gets the list of categories from ccSettings.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	virtual Result GetCategories(ICategoryList*& pVal) const throw() = 0;

	// Sets the list of categories from ccSettings.
	virtual Result SetCategories(const ICategoryList* pVal) throw() = 0;

	// Gets the list of locations.
	// The returned object is reference counted; it is the caller's responsibility to call Release.
	virtual Result GetLocations(ILocationList*& pVal) const throw() = 0;

	// Sets the list of locations.
	// InvalidArgument is returned if there is not exactly one default location.
	// See ILocation::SetDefault() for more information.
	// Calling SetLocations may change the location IDs, thus pVal is not const.
	virtual Result SetLocations(ILocationList* pVal) throw() = 0;

    // Gets/Sets the LocationManager objects. See ILocationManager for more information.
    // The Get call loads the information. The Set call persists the information to the datastore.
    virtual Result GetLocationManager(ILocationManager*& pLocationManager) const throw() = 0;
    virtual Result SetLocationManager(const ILocationManager* pLocationManager) throw() = 0;

    enum Properties
    {
        // Do not disable the settings change event unless you do NOT want other clients to know
        // The the firewall configuration has been updated
        SendChangeEvent = 0,                // bool Value in properties collection
    };

    // Gets a Value collection of properties for the object
    virtual Result GetProperties(cc::IKeyValueCollection*& pProps) throw() = 0;

    //Register/Unregister callback notification
    virtual Result RegisterNotify(IFirewallSettingChangeNotify* pCallback, DWORD& dwID) throw() = 0;
    virtual Result UnRegisterNotify(DWORD dwID) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_FirewallSettings, 
						0x91d6c73d, 0xca45, 0x4d1c, 0x94, 0xad, 0xb0, 0x8b, 0x64, 0xa2, 0x1d, 0xe9);
typedef CSymPtr<IFirewallSettings> IFirewallSettingsPtr;
typedef CSymQIPtr<IFirewallSettings, &IID_FirewallSettings> IFirewallSettingsQIPtr;

///////////////////////////////////////////////////////////////////////////////
class IRuleUtil : public ISymBase
{
public:
	// Initializes the class, can be called multiple times to change the location list and locked status
	virtual bool Initialize(ILocationList* pLocationList, bool bLocked) throw() = 0;

	// Creates a single block all rule for the application and saves the application in SymNeti
	virtual bool CreateBlockRule(LPCSTR pszPath, const ICategory* pCategory) throw() = 0;
	virtual bool CreateBlockRule(LPCWSTR pwszPath, const ICategory* pCategory) throw() = 0;

	// Creates a single permit all rule for the application and saves the application in SymNeti
	virtual bool CreatePermitRule(LPCSTR pszPath, const ICategory* pCategory) throw() = 0;
	virtual bool CreatePermitRule(LPCWSTR pwszPath, const ICategory* pCategory) throw() = 0;

	// Adds given ALE rules for the application and saves the application SymNeti, any Previous app rules are discarded
	virtual bool CreateALERules(LPCSTR pszPath, ccFirewallSettings::IApplication* pALEApplication) throw() = 0;
	virtual bool CreateALERules(LPCWSTR pwszPath, ccFirewallSettings::IApplication* pALEApplication) throw() = 0;

	// Looks up ALE rules for the application and creates the ALE rules with SymNeti, any Previous app rules are discarded
	virtual bool CreateALERules(LPCSTR pszPath) throw() = 0;
	virtual bool CreateALERules(LPCWSTR pwszPath) throw() = 0;

	// Adds given rule list to the application and saves the application SymNeti, any Previous app rules are saved
	virtual bool AppendRuleListToApp(LPCSTR pszPath, ccFirewallSettings::IRuleList* pRuleList) throw() = 0;
	virtual bool AppendRuleListToApp(LPCWSTR pwszPath, ccFirewallSettings::IRuleList* pRuleList) throw() = 0;

	// Adds a custom application rule to the Application and saves with SymNeti, any Previous app rules are saved
    virtual bool AppendRuleToApp(LPCSTR pszPath, ccFirewallSettings::IRule* pRule) throw() = 0;
    virtual bool AppendRuleToApp(LPCWSTR pwszPath, ccFirewallSettings::IRule* pRule) throw() = 0;

	// Adds a custom system rule and saves with SymNeti
	virtual bool CreateCustomSystemRule(ccFirewallSettings::IRule* pRule) throw() = 0;

	// Copies the Rules and Zones (System Rules, Trojan Rules, App Rules, Zones)
	// from the default location to the given location.
	// CreateDefaultSettingsForLocation copies all rules and zones.
	// CopyDefaultAdminSettingsToLocation copies only admin rules and zones.
	// Warning: These modify system rules, trojan rules, and applications rules. Any
	// copies you have may be out of date after these methods return.
	virtual Result CreateDefaultRulesAndZonesForLocation(const ILocation* pLocation) throw() = 0;
	virtual Result CopyDefaultAdminRulesAndZonesToLocation(const ILocation* pLocation) throw() = 0;

	// Removes all Rules And Zones (System Rules, Trojan Rules, App Rules, Zones) 
	// for the given location.
	// Warning: This modifies system rules, trojan rules, and applications rules. Any
	// copies you have may be out of date after this method return.
	virtual Result DeleteRulesAndZonesForLocation(const ILocation* pLocation) throw() = 0;

    // Utility methods to get/set the default rule lock stste.
    // Does NOT require Initialize() to be called first!
    virtual Result SetDefaultRuleLockState(bool bLockState) throw() = 0;
    virtual Result GetDefaultRuleLockState(bool &bLockState) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_RuleUtil, 
						0x930036fd, 0xaa4e, 0x49f2, 0xa6, 0xd5, 0x73, 0x41, 0x71, 0xa4, 0x9d, 0xe7);
typedef CSymPtr<IRuleUtil> IRuleUtilPtr;
typedef CSymQIPtr<IRuleUtil, &IID_RuleUtil> IRuleUtilQIPtr;

}	// namespace ccFirewallSettings
