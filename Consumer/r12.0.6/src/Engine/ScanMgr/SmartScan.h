
#ifndef SMART_SCAN_H
#define SMART_SCAN_H

#include <list>
typedef std::list<::std::string> EXTENSIONS_LIST;

#include "NavSettings.h"

class CSmartScan
{
public:
	// Constructor.
	CSmartScan();

	// Initializes the class.
	bool Initialize();

	// Is there a match with the specified file?
	bool IsMatch(const char* szOemFileName) const;

private:
	// Are we ready to match files?
	bool m_bReady;

	// List of program files and documents extensions.
	EXTENSIONS_LIST m_ExtensionsList;

private:
	// Reads the extension list from NAV options (as a string).
    bool LoadExtensions(::std::string& strExtensions);

	// Parses the specified extension string into a list of extensions.
    bool ParseExtensions(const ::std::string& strExtensions);

	// Returns true if the specified file is in the extension list.
	bool IsFileInExtensionList(const char* szFileName) const;

	// Returns true if the specified file is an Office document or an executable.
	bool IsOfficeDocumentOrExecutable(const char* szFileName) const;

	// Returns true if the specified extension matches the extension pattern.
	bool IsMatchingExtension(const char* szPattern, const char* szExtension) const;
	
	NAVToolbox::CCSettings m_ccSettings;
};

#endif

