// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// _Application wrapper class

class _WApplication : public COleDispatchDriver
{
public:
	_WApplication() {}		// Calls COleDispatchDriver default constructor
    _WApplication(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_WApplication(const _WApplication& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}


// Attributes
public:

// Operations
public:
	LPDISPATCH GetWordBasic();
	BOOL GetVisible();
	void SetVisible(BOOL bNewValue);
	LPDISPATCH GetDocuments();
};

/////////////////////////////////////////////////////////////////////////////
// Documents wrapper class

class Documents : public COleDispatchDriver
{
public:
	Documents() {}		// Calls COleDispatchDriver default constructor
	Documents(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Documents(const Documents& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Operations
public:
	LPDISPATCH Open(VARIANT* FileName, VARIANT* ConfirmConversions, VARIANT* ReadOnly, VARIANT* AddToRecentFiles, VARIANT* PasswordDocument, VARIANT* PasswordTemplate, VARIANT* Revert, VARIANT* WritePasswordDocument, 
		VARIANT* WritePasswordTemplate, VARIANT* Format);
};
class _Document : public COleDispatchDriver
{
public:
	_Document() {}		// Calls COleDispatchDriver default constructor
	_Document(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Document(const _Document& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}
};
