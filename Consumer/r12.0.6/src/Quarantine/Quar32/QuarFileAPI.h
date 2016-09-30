
#pragma once

class CQuarFileAPI
{
public:
	// Constructor.
	CQuarFileAPI();
	
	// Destructor.
	~CQuarFileAPI();

	// Switches to ANSI file APIs.
	void SwitchAPIToANSI();

	// Switches to OEM file APIs.
	void SwitchAPIToOEM();

	// Restores the original file APIs.
	void RestoreOriginalAPI();

	// Is the original file API ANSI?
	bool IsOriginalANSI() const;

private:
	// Did we start with ANSI file APIs?
	bool m_bOriginalANSI;
};
