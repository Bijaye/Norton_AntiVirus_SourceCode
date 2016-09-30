///////////////////////////////////////////////////////////////
// setupini.cpp -- entry point into the application
// setupini.exe makes changes to the setup.ini file after build
// it is part of the layout creation process.. CDAVIS
//

#include <windows.h>

void main()
{
	char szIniName[MAX_PATH] = {0};
	char szVersionDatName[MAX_PATH] = {0};
	int iLength = 0;

	GetModuleFileName(NULL, szIniName, sizeof(szIniName));
	iLength = lstrlen(szIniName);
	szIniName[iLength - 12] = '\0';
	wsprintf(szVersionDatName, "%s..\\nobuilds\\version.dat", szIniName);
	wsprintf(szIniName, "%s..\\Final\\setup.ini", szIniName);
	
	//erase the Product Version string.. nobody uses it
	WritePrivateProfileString("WiseInstaller", "ProductVersion", NULL, szIniName);

	//set up the other values
	WritePrivateProfileString("WiseInstaller", "ProductFile", "NAV\\NAV.MSI", szIniName);
	WritePrivateProfileString("WiseInstaller", "Runtime9X", "MSI\\WIN9X\\instmsi.exe", szIniName);
	WritePrivateProfileString("WiseInstaller", "RuntimeNT", "MSI\\WINNT\\instmsiw.exe", szIniName);
}
