// SymCorpUI-newDoc.cpp : implementation of the CSymCorpUIDoc class
//

#include "stdafx.h"
#include "SymCorpUI.h"
#include "SymCorpUIDoc.h"
#include "TrustUtil.h"


// CSymCorpUIDoc
IMPLEMENT_DYNCREATE(CSymCorpUIDoc, CDocument)

BEGIN_MESSAGE_MAP(CSymCorpUIDoc, CDocument)
END_MESSAGE_MAP()


// CSymCorpUIDoc construction/destruction

CSymCorpUIDoc::CSymCorpUIDoc()
{
	// TODO: add one-time construction code here

}

CSymCorpUIDoc::~CSymCorpUIDoc()
{
    // Release protection providers
    UnloadProviders();
}

BOOL CSymCorpUIDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

    if (SUCCEEDED(LoadProviders()))
    	return TRUE;
    else
        return FALSE;
}


HRESULT CSymCorpUIDoc::LoadProviders()
{
	// Initialize listviews
	CTrustVerifier													trustVerifier;
	ProtectionProviderLib::ProtectionProviderList::iterator			currProtectionProvider;
	ProtectionProviderLib::IProtectionPtr							currProtectionProviderPtr;
	CComBSTR														shortDisplayName;
#ifdef _DEBUG
	CLSIDList::size_type											untrustedNoComponents		= 0;
#endif
    CString                                                         errorMessage;
    ProtectionProviderLib::ProtectionProviderList::size_type        noProvidersBeforeReload     = 0;
	HRESULT															returnValHR					= E_FAIL;

    // Release any existing protection technology objects
    noProvidersBeforeReload = protectionProviders.size();
    UnloadProviders(false);

    // Trust verify all ProtectionProvider components
	// NOTE:  In debug builds, it takes 500ms to create a trust loader, but in Release builds it's quite
	// fast.
	returnValHR = trustVerifier.Initialize(CTrustVerifier::VerifyMode_CommonClient);
	if (SYM_SUCCEEDED(returnValHR))
	{
#pragma message("*** COMCAT adds 0.5 - 2 seconds here, reimplement without it.")
		returnValHR = EnumCategoryComponents(CATID_ProtectionProvider, &protectionProvidersIDs);
		if (SUCCEEDED(returnValHR))
		{
	#ifdef _DEBUG
			untrustedNoComponents = protectionProvidersIDs.size();
	#endif
            trustVerifier.TrustVerifyComponentList(&protectionProvidersIDs);
			trustVerifier.Shutdown();
		}
        else
        {
            errorMessage.FormatMessage(IDS_ERROR_ENUMERATINGPROVIDERS, returnValHR);
            AfxMessageBox(errorMessage);
        }
	}
	else
	{
        errorMessage.FormatMessage(IDS_ERROR_CREATINGTRUSTVERIFIER, returnValHR);
		AfxMessageBox(errorMessage);
	}

    // Create the protection providers
    if (SUCCEEDED(returnValHR))
    {
#ifdef _DEBUG
        if (protectionProvidersIDs.size() != untrustedNoComponents)
        {
            if (protectionProvidersIDs.size() == 0)
                AfxMessageBox(_T("All protection providers failed digital signature validation.  Did you remember to turn ccVerifyTrust off?"));
            else
                AfxMessageBox(_T("Some protection providers failed digital signature validation.  Did you remember to turn ccVerifyTrust off?"));
        }
#endif // _DEBUG
        if (protectionProvidersIDs.size() == 0)
        {
            errorMessage.LoadString(IDS_ERROR_NOPROVIDERSFOUND);
            AfxMessageBox(errorMessage);
            returnValHR = S_FALSE;
        }
        else
        {
            returnValHR = CreateComponents<ProtectionProviderLib::IProtection_Provider>(protectionProvidersIDs, &protectionProviders);
        }
    }

    // If we had any protection providers to start with or have some now, notify views
    // that we've changed since objects have been deleted and re-created
    if ((noProvidersBeforeReload != 0) || (protectionProviders.size() != 0))
    {
        SetModifiedFlag(TRUE);
        UpdateAllViews(NULL);
    }
    return returnValHR;
}

HRESULT CSymCorpUIDoc::UnloadProviders( bool updateViews )
// Unloads all protection providers
{
    HRESULT                                                     returnValHR = S_OK;
	ProtectionProviderLib::ProtectionProviderList::iterator     currProtectionProvider;

    if (protectionProviders.size() == 0)
        returnValHR = S_FALSE;
	for (currProtectionProvider = protectionProviders.begin(); currProtectionProvider != protectionProviders.end(); currProtectionProvider++)
		(*currProtectionProvider)->Release();
	protectionProviders.clear();
    protectionProvidersIDs.clear();

    // Update the view that the data has changed
    if (updateViews)
        UpdateAllViews(NULL);
    if (returnValHR != S_FALSE)
        SetModifiedFlag(TRUE);
    return returnValHR;
}

// CSymCorpUIDoc serialization
void CSymCorpUIDoc::Serialize(CArchive& ar)
{
    // Nothing to load, nothing to save
}


// CSymCorpUIDoc diagnostics
#ifdef _DEBUG
    void CSymCorpUIDoc::AssertValid() const
    {
	    CDocument::AssertValid();
    }

    void CSymCorpUIDoc::Dump(CDumpContext& dc) const
    {
	    CDocument::Dump(dc);
    }
#endif //_DEBUG


// CSymCorpUIDoc commands
