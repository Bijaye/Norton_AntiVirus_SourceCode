// Machine generated IDispatch wrapper class(es) created with ClassWizard


/////////////////////////////////////////////////////////////////////////////
// _Application wrapper class

class _Application : public COleDispatchDriver
{
public:
	_Application() {}		// Calls COleDispatchDriver default constructor
	_Application(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Application(const _Application& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}


// Attributes
public:

// Operations
public:
	LPDISPATCH GetActiveWindow();
	LPDISPATCH GetActiveWorkbook();
	void ActivateMicrosoftApp(long Index);
	void SendKeys(const VARIANT& Keys, const VARIANT& Wait);
	LPDISPATCH GetWindows();
	LPDISPATCH GetWorkbooks();
	CString GetName();
	CString GetPath();
	void Quit();
	CString GetStartupPath();
	CString GetTemplatesPath();
	BOOL GetVisible();
	void SetVisible(BOOL bNewValue);
};
/////////////////////////////////////////////////////////////////////////////
// _Workbook wrapper class

class _Workbook : public COleDispatchDriver
{
public:
	_Workbook() {}		// Calls COleDispatchDriver default constructor
    _Workbook(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Workbook(const _Workbook& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void Close(const VARIANT& SaveChanges, const VARIANT& Filename, const VARIANT& RouteWorkbook);
	CString GetFullName();
	CString GetName();
	CString GetPath();
	void Save();
	void SaveAs(const VARIANT& Filename, const VARIANT& FileFormat, const VARIANT& Password, const VARIANT& WriteResPassword, const VARIANT& ReadOnlyRecommended, const VARIANT& CreateBackup, long AccessMode, const VARIANT& ConflictResolution, 
		const VARIANT& AddToMru, const VARIANT& TextCodepage, const VARIANT& TextVisualLayout);
	void SaveCopyAs(const VARIANT& Filename);
	BOOL GetSaved();
	void SetSaved(BOOL bNewValue);
	LPDISPATCH GetWindows();
};
/////////////////////////////////////////////////////////////////////////////
// Workbooks wrapper class

class Workbooks : public COleDispatchDriver
{
public:
	Workbooks() {}		// Calls COleDispatchDriver default constructor
	Workbooks(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Workbooks(const Workbooks& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH Add(const VARIANT& Template);
	void Close();
	LPDISPATCH Open(LPCTSTR Filename, const VARIANT& UpdateLinks, const VARIANT& ReadOnly, const VARIANT& Format, const VARIANT& Password, const VARIANT& WriteResPassword, const VARIANT& IgnoreReadOnlyRecommended, const VARIANT& Origin, 
		const VARIANT& Delimiter, const VARIANT& Editable, const VARIANT& Notify, const VARIANT& Converter, const VARIANT& AddToMru);
};
/////////////////////////////////////////////////////////////////////////////
// Window wrapper class

class Window : public COleDispatchDriver
{
public:
	Window() {}		// Calls COleDispatchDriver default constructor
	Window(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Window(const Window& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void Close(const VARIANT& SaveChanges, const VARIANT& Filename, const VARIANT& RouteWorkbook);
	BOOL GetVisible();
	void SetVisible(BOOL bNewValue);
};
/////////////////////////////////////////////////////////////////////////////
// Windows wrapper class

class Windows : public COleDispatchDriver
{
public:
	Windows() {}		// Calls COleDispatchDriver default constructor
	Windows(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Windows(const Windows& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetCount();
	LPDISPATCH GetItem(const VARIANT& Index);
};
