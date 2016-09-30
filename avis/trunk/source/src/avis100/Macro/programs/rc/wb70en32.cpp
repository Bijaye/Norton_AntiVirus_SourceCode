// Machine generated IDispatch wrapper class(es) created with ClassWizard
// with unused methods removed
#include "stdafx.h"
#include "wb70en32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// WordBasic properties

/////////////////////////////////////////////////////////////////////////////
// WordBasic operations




CString WordBasic::FileType(LPCTSTR File)
{
	CString result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x81f4, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		File);
	return result;
}


void WordBasic::OK()
{
	InvokeHelper(0x47, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void WordBasic::Cancel()
{
	InvokeHelper(0x48, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}


void WordBasic::FileNew(const VARIANT& Template, const VARIANT& NewTemplate)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x4f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Template, &NewTemplate);
}

void WordBasic::FileOpen(const VARIANT& Name, const VARIANT& ConfirmConversions, const VARIANT& ReadOnly, const VARIANT& AddToMru, const VARIANT& PasswordDoc, const VARIANT& PasswordDot, const VARIANT& Revert, const VARIANT& WritePasswordDoc, 
		const VARIANT& WritePasswordDot)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x50, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Name, &ConfirmConversions, &ReadOnly, &AddToMru, &PasswordDoc, &PasswordDot, &Revert, &WritePasswordDoc, &WritePasswordDot);
}

void WordBasic::FileSave()
{
	InvokeHelper(0x53, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void WordBasic::FileSaveAs(const VARIANT& Name, const VARIANT& Format, const VARIANT& LockAnnot, const VARIANT& Password, const VARIANT& AddToMru, const VARIANT& WritePassword, const VARIANT& RecommendReadOnly, const VARIANT& EmbedFonts, 
		const VARIANT& NativePictureFormat, const VARIANT& FormsData, const VARIANT& SaveAsAOCELetter)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x54, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Name, &Format, &LockAnnot, &Password, &AddToMru, &WritePassword, &RecommendReadOnly, &EmbedFonts, &NativePictureFormat, &FormsData, &SaveAsAOCELetter);
}




void WordBasic::FileFind(const VARIANT& SearchName, const VARIANT& SearchPath, const VARIANT& Name, const VARIANT& SubDir, const VARIANT& Title, const VARIANT& Author, const VARIANT& Keywords, const VARIANT& Subject, const VARIANT& Options, 
		const VARIANT& MatchCase, const VARIANT& Text, const VARIANT& PatternMatch, const VARIANT& DateSavedFrom, const VARIANT& DateSavedTo, const VARIANT& SavedBy, const VARIANT& DateCreatedFrom, const VARIANT& DateCreatedTo, 
		const VARIANT& View, const VARIANT& SortBy, const VARIANT& ListBy, const VARIANT& SelectedFile, const VARIANT& Add, const VARIANT& Delete, const VARIANT& ShowFolders)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT 
		VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x63, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &SearchName, &SearchPath, &Name, &SubDir, &Title, &Author, &Keywords, &Subject, &Options, &MatchCase, &Text, &PatternMatch, &DateSavedFrom, &DateSavedTo, &SavedBy, &DateCreatedFrom, &DateCreatedTo, &View, &SortBy, &ListBy, &SelectedFile, 
		&Add, &Delete, &ShowFolders);
}

void WordBasic::DocRestore()
{
	InvokeHelper(0xe9, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void WordBasic::FileNewDefault()
{
	InvokeHelper(0x12d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}



CString WordBasic::FileName(const VARIANT& Number)
{
	CString result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x8025, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		&Number);
	return result;
}


CString WordBasic::WindowName(const VARIANT& Number)
{
	CString result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x803b, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		&Number);
	return result;
}

short WordBasic::Window()
{
	short result;
	InvokeHelper(0x803e, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

short WordBasic::AppMinimize(const VARIANT& WindowName, const VARIANT& State)
{
	short result;
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x8041, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		&WindowName, &State);
	return result;
}

short WordBasic::AppMaximize(const VARIANT& WindowName, const VARIANT& State)
{
	short result;
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x8042, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		&WindowName, &State);
	return result;
}

short WordBasic::AppRestore(const VARIANT& WindowName)
{
	short result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x8043, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		&WindowName);
	return result;
}

short WordBasic::DocMaximize(const VARIANT& State)
{
	short result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x8044, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		&State);
	return result;
}


short WordBasic::Hidden(const VARIANT& On)
{
	short result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x804d, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		&On);
	return result;
}


void WordBasic::SendKeys(LPCTSTR Keys, const VARIANT& Wait)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x8064, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Keys, &Wait);
}



void WordBasic::DocClose(const VARIANT& Save)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x809f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Save);
}

void WordBasic::FileClose(const VARIANT& Save)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x80a0, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Save);
}

CString WordBasic::Files(const VARIANT& FileSpec)
{
	CString result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x80a1, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		&FileSpec);
	return result;
}

void WordBasic::FileSaveAll(const VARIANT& Save, const VARIANT& OriginalFormat)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x80a3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Save, &OriginalFormat);
}


void WordBasic::SetAttr(LPCTSTR FileName, short Attribute)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I2;
	InvokeHelper(0x8103, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName, Attribute);
}

short WordBasic::DocMinimize()
{
	short result;
	InvokeHelper(0x8105, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

void WordBasic::AppActivate(LPCTSTR WindowName, const VARIANT& Immediate)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x8063, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 WindowName, &Immediate);
}
void WordBasic::AppClose(const VARIANT& WindowName)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x8107, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &WindowName);
}


short WordBasic::AppIsRunning(LPCTSTR WindowName)
{
	short result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x810b, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		WindowName);
	return result;
}


short WordBasic::GetAttr(LPCTSTR FileName)
{
	short result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x810f, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		FileName);
	return result;
}


void WordBasic::AppShow(const VARIANT& WindowName)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x8121, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &WindowName);
}


void WordBasic::FileCloseAll(const VARIANT& Save)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x814a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Save);
}


CString WordBasic::FileNameFromWindow(const VARIANT& WindowNumber)
{
	CString result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x8153, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		&WindowNumber);
	return result;
}

CString WordBasic::GetText(long Pos1, long Pos2)
{
	CString result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x8172, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		Pos1, Pos2);
	return result;
}


short WordBasic::StartOfDocument(const VARIANT& Select)
{
	short result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0xc010, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		&Select);
	return result;
}

short WordBasic::EndOfDocument(const VARIANT& Select)
{
	short result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0xc011, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		&Select);
	return result;
}

void WordBasic::Insert(LPCTSTR Text)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x8012, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Text);
}


