
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iShowCmd)
{
	// Navwnt.exe is a stub application that launches Navw32.exe.
	// In previous versions of NAV, the main scanner was named Navwnt.exe for
	// NT platforms and Navw32.exe for 9X platforms. We now have a single scanner
	// executable that runs on both 9X and NT platforms: Navw32.exe.
	// To prevent third-party applications such as WinZip from breaking, we've
	// added this stub executable which simply calls Navw32.exe.

	ShellExecute(NULL, "open", "navw32.exe", szCmdLine, NULL, iShowCmd);

	return 0;
}

