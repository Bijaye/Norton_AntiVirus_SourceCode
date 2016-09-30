
#ifndef QUARANTINE_H
#define QUARANTINE_H

#include "iquaran.h"
#include "QuarantineDllLoader.h"

class CQuarantine
{
public:
	// Constructor.
	CQuarantine();

	// Destructor.
	~CQuarantine();

	// Creates and initialize quarantine.
	bool Open(IScanner* pScanner);

	// Closes quarantine.
	void Close();

	// Quarantines the specified file.
	bool Add(char* szFileName, DWORD FileType,
		bool bDeleteOriginal, bool bOemFileName);

	// Quarantines the specified infection.
    bool Add(IScanInfection* pInfection, unsigned long ulQuarFileStatus, bool bDeleteOriginal,
        const char* szOriginalName, IQuarantineItem** pItemOut = NULL);

    bool AddWithoutFile(unsigned long ulQuarFileStatus, IQuarantineItem** pItemOut);

    bool LoadQuarantineItemFromFilenameVID(LPCSTR lpszSourceFileName, 
                                           DWORD dwVirusID, 
                                           IQuarantineItem **pMatchingQuarItem);

    bool LoadQuarantineItemFromUUID(UUID uuid, IQuarantineItem **pMatchingQuarItem);

	// Deletes the specified infection from quarantine.
	bool Delete(IScanInfection* pInfection);

    bool RetrieveAssociatedQuarantineItem(IScanInfection* pInfection, IQuarantineItem** pItemRetrieved);

	// Returns true if the specified infection can be quarantined.
	bool CanBeQuarantined(IScanInfection* pInfection);

    // Takes the system snapshot if we don't already have one
    bool TakeSystemSnapShot();
    bool AddSnapShotToQuarantineItem(IQuarantineItem* pItem);

    long GetQuarantineCount();

    bool AddFailedOnExtraction() const;

private:
	// NAVScan scanner.
	IScanner* m_pScanner;
	QuarDLL_Loader	m_QuarLoader;

	// Quarantine server.
    CSymPtr<IQuarantineDLL> m_pQuarServer;

	// The quarantine item map is used to keep track of all items quarantined. The ISymBase* key
    // allows us to look up quarantine items based on the IScanInfection it is associated with.
	typedef ::std::map< ISymBase*, CSymPtr<IQuarantineItem> > QUARANTINEMAP;
    QUARANTINEMAP m_ItemMap;

	// Quarantines a file infection.
	void HandleFileInfection(IScanFileInfection2* pInfection, unsigned long ulQuarFileStatus,
		bool bDeleteOriginal, const char* szOriginalName, IQuarantineItem*& pItemAdded);

	// Quarantines a compressed infection.
	void HandleCompressedInfection(IScanCompressedFileInfection2* pInfection,
		unsigned long ulQuarFileStatus, bool bDeleteOriginal, const char* szOriginalName,
		IQuarantineItem*& pItemAdded);

	// Safe deletes the specified file by truncating it before deleting it.
	BOOL SafeDeleteFile(const char* szFileName);

    bool m_bHaveSnapShot;
    bool m_bAddFailedOnExtraction;
};

#endif

