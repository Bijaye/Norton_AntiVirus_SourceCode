// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// _PPTApplication wrapper class

class _PPTApplication : public COleDispatchDriver
{
public:
	_PPTApplication() {}		// Calls COleDispatchDriver default constructor
	_PPTApplication(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_PPTApplication(const _PPTApplication& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetPresentations();
	LPDISPATCH GetWindows();
	LPDISPATCH GetActiveWindow();
	LPDISPATCH GetActivePresentation();
	CString GetPath();
	CString GetName();
	CString GetCaption();
	void SetCaption(LPCTSTR lpszNewValue);
	void Quit();
	long GetVisible();
	void SetVisible(long nNewValue);
};
/////////////////////////////////////////////////////////////////////////////
// _Presentation wrapper class

class _Presentation : public COleDispatchDriver
{
public:
	_Presentation() {}		// Calls COleDispatchDriver default constructor
	_Presentation(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Presentation(const _Presentation& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetWindows();
	CString GetFullName();
	CString GetName();
	CString GetPath();
	LPDISPATCH GetSlides();  
	LPDISPATCH GetSlideShowWindow();
	long GetSaved();
	void SetSaved(long nNewValue);
	void Save();
	void SaveAs(LPCTSTR FileName, long FileFormat, long EmbedTrueTypeFonts);
	void SaveCopyAs(LPCTSTR FileName, long FileFormat, long EmbedTrueTypeFonts);
	void Close();
};
/////////////////////////////////////////////////////////////////////////////
// Presentations wrapper class

class Presentations : public COleDispatchDriver
{
public:
	Presentations() {}		// Calls COleDispatchDriver default constructor
	Presentations(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Presentations(const Presentations& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetCount();
	LPDISPATCH GetApplication();
	LPDISPATCH GetSlides();
	LPDISPATCH GetParent();
	LPDISPATCH Item(const VARIANT& index);
	LPDISPATCH Add(long WithWindow);
	LPDISPATCH Open(LPCTSTR FileName, long ReadOnly, long Untitled, long WithWindow);

};
/////////////////////////////////////////////////////////////////////////////
// Slides wrapper class

class Slides : public COleDispatchDriver
{
public:
	Slides() {}		// Calls COleDispatchDriver default constructor
	Slides(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Slides(const Slides& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetCount();
	LPDISPATCH Add(long index, long Layout);
};
/////////////////////////////////////////////////////////////////////////////
// _Slide wrapper class

class _Slide : public COleDispatchDriver
{
public:
	_Slide() {}		// Calls COleDispatchDriver default constructor
	_Slide(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_Slide(const _Slide& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetApplication();
	CString GetName();
	void SetName(LPCTSTR lpszNewValue);
	long GetSlideID();
};
