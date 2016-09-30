// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// WordBasic wrapper class

class WordBasic : public COleDispatchDriver
{
public:
	WordBasic() {}		// Calls COleDispatchDriver default constructor
	WordBasic(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	WordBasic(const WordBasic& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void AppActivate(LPCTSTR WindowName, const VARIANT& Immediate);
	CString FileType(LPCTSTR File);
	void SaveTemplate();
	void OK();
	void Cancel();
	void FileNew(const VARIANT& Template, const VARIANT& NewTemplate);
	void FileOpen(const VARIANT& Name, const VARIANT& ConfirmConversions, const VARIANT& ReadOnly, const VARIANT& AddToMru, const VARIANT& PasswordDoc, const VARIANT& PasswordDot, const VARIANT& Revert, const VARIANT& WritePasswordDoc, 
		const VARIANT& WritePasswordDot);
	void FileSave();
	void FileSaveAs(const VARIANT& Name, const VARIANT& Format, const VARIANT& LockAnnot, const VARIANT& Password, const VARIANT& AddToMru, const VARIANT& WritePassword, const VARIANT& RecommendReadOnly, const VARIANT& EmbedFonts, 
		const VARIANT& NativePictureFormat, const VARIANT& FormsData, const VARIANT& SaveAsAOCELetter);
	void FileFind(const VARIANT& SearchName, const VARIANT& SearchPath, const VARIANT& Name, const VARIANT& SubDir, const VARIANT& Title, const VARIANT& Author, const VARIANT& Keywords, const VARIANT& Subject, const VARIANT& Options, 
		const VARIANT& MatchCase, const VARIANT& Text, const VARIANT& PatternMatch, const VARIANT& DateSavedFrom, const VARIANT& DateSavedTo, const VARIANT& SavedBy, const VARIANT& DateCreatedFrom, const VARIANT& DateCreatedTo, 
		const VARIANT& View, const VARIANT& SortBy, const VARIANT& ListBy, const VARIANT& SelectedFile, const VARIANT& Add, const VARIANT& Delete, const VARIANT& ShowFolders);
	void DocRestore();
	void FileNewDefault();
	short ShowAll(const VARIANT& On);
	CString FileName(const VARIANT& Number);
	CString WindowName(const VARIANT& Number);
	short Window();
	short AppMinimize(const VARIANT& WindowName, const VARIANT& State);
	short AppMaximize(const VARIANT& WindowName, const VARIANT& State);
	short AppRestore(const VARIANT& WindowName);
	short DocMaximize(const VARIANT& State);
	short Hidden(const VARIANT& On);
	void SendKeys(LPCTSTR Keys, const VARIANT& Wait);
	void DocClose(const VARIANT& Save);
	void FileClose(const VARIANT& Save);
	CString Files(const VARIANT& FileSpec);
	void FileSaveAll(const VARIANT& Save, const VARIANT& OriginalFormat);
	void SetAttr(LPCTSTR FileName, short Attribute);
	short DocMinimize();
	void AppClose(const VARIANT& WindowName);
	short AppIsRunning(LPCTSTR WindowName);
	short GetAttr(LPCTSTR FileName);
	void AppShow(const VARIANT& WindowName);
	void FileCloseAll(const VARIANT& Save);
	CString FileNameFromWindow(const VARIANT& WindowNumber);
	CString GetText(long Pos1, long Pos2);
	short StartOfDocument(const VARIANT& Select);
	short EndOfDocument(const VARIANT& Select);
		void Insert(LPCTSTR Text);
};
