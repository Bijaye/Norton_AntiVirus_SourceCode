#pragma once

static const LPCTSTR NAVSnoozeSettingsKeyName = "Norton AntiVirus\\NAVOPTS.DAT\\SNOOZE";
static const LPCTSTR NAVSnoozeSettingsChangedEvent = "Symantec.Norton.AntiVirus.Snooze.Changed";
static const LPCTSTR NAVSnoozeAlertDialogMutex = "Symantec.Norton.AntiVirus.Snooze.DialogShown";
static const int NAVSnoozeSettingsCount = 5;
static const SnoozeFeature NAVSnoozeFeatures[NAVSnoozeSettingsCount] =
		{	SnoozeAP, SnoozeIWP, SnoozeALU, SnoozeEmailIn, SnoozeEmailOut };
static const LPCTSTR NAVSnoozeSettingsNames[NAVSnoozeSettingsCount] =
		{	"AP", "IWP", "ALU", "EmailIn", "EmailOut" };

inline int GetNAVSnoozeFeatureIndex(SnoozeFeature snoozeFeature)
{
	switch (snoozeFeature)
	{
	case SnoozeAP:
		return 0;

	case SnoozeIWP:
		return 1;

	case SnoozeALU:
		return 2;

	case SnoozeEmailIn:
		return 3;

	case SnoozeEmailOut:
		return 4;

	default:
#ifdef CCTRACEE
		CCTRACEE("Invalid snooze feature");
#endif
		throw -1;
	}
}

inline LPCTSTR GetNAVSnoozeSettingsName(SnoozeFeature snoozeFeature)
{
	return NAVSnoozeSettingsNames[GetNAVSnoozeFeatureIndex(snoozeFeature)];
}

inline bool IsSnoozeAlertDialogShown()
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, NAVSnoozeAlertDialogMutex);

	bool running = hMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS;

	if (hMutex != NULL)
		CloseHandle(hMutex);

	return running;
}
