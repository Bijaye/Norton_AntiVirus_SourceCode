// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// app.h

/////////////////////////////////////////////////////////////////////////////
// CMyCommandLineInfo interface

class CMyCommandLineInfo : public CCommandLineInfo 
{
// Attributes
public:
   BOOL    m_bHelp;        // /H /? /HELP
   CString m_strParameter;
   BOOL    m_bSuccess;     // all switches ok
   BOOL    m_bProcessAll;

// Construction
public:
   CMyCommandLineInfo(void);

// Overrides
public:
   virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
};

/////////////////////////////////////////////////////////////////////////////
// Interface of some application class

class CApp : public CWinApp
{
// Attributes
protected:
   static CApp m_app;

// Construction
public:
   CApp(void);

// Attributes
public:
   CMyCommandLineInfo m_cmdInfo;

// Operations
public:
   static CApp& GetApp(void);

   inline void ParseCommandLine   (void) {        CWinApp::ParseCommandLine   (m_cmdInfo); }
   inline BOOL ProcessShellCommand(void) { return CWinApp::ProcessShellCommand(m_cmdInfo); }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CApp)
   public:
   virtual BOOL InitInstance();
   virtual int Run();
   virtual int ExitInstance();
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CApp(void);
};

