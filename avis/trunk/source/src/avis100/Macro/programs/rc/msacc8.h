
/////////////////////////////////////////////////////////////////////////////
// _ACCApplication wrapper class

class _ACCApplication : public COleDispatchDriver
{
public:
	_ACCApplication() {}		// Calls COleDispatchDriver default constructor
	_ACCApplication(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_ACCApplication(const _ACCApplication& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	BOOL GetVisible();
	void SetVisible(BOOL bNewValue);
	CString GetCurrentObjectName();
	void NewCurrentDatabase(LPCTSTR filepath);
	void OpenCurrentDatabase(LPCTSTR filepath, BOOL Exclusive);
	void CloseCurrentDatabase();
	LPDISPATCH CurrentDb();
	void Quit(long Option);

};
