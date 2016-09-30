
#ifndef FILE_API_H
#define FILE_API_H

class CFileAPI
{
public:
	// Constructor.
	CFileAPI();
	
	// Destructor.
	~CFileAPI();

	// Switches to ANSI file APIs.
	void SwitchAPIToANSI();

	// Switches to OEM file APIs.
	void SwitchAPIToOEM();

	// Restores the original file APIs.
	void RestoreOriginalAPI();

	// Is the original file API ANSI?
	bool IsOriginalANSI() const;

	// Constructs a Unicode path for super long file names
    static void ConvertLongFileToUnicodeBuffer( const char * szFileName, ::std::vector<WCHAR> &vUnicodeBuffer );

private:
	// Did we start with ANSI file APIs?
	bool m_bOriginalANSI;
};

#endif

