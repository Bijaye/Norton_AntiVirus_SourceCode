// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "Errorlog.h"

struct  TokenStruct {
  int token;
	CString tokenvalue;
};

struct  ScriptStruct {
  TokenStruct TS[50];
  int NumEntries;
  CString ScriptFileName;
};

const int PROGRAM_NOT_FOUND = -2;
const int UNINSTALL_NOT_ALLOWED = -3;

BOOL IsNT();

	enum enumValidateRetCode  {VALIDATE_SCRIPT_SUCCESS, VALIDATE_SCRIPT_FAILURE};  

	class ValidateScriptToRun {
public:

	CString TempPath;
	BOOL ProgramNameIsGood;
	BOOL IsPlatform;
	BOOL SecondRun;
	int Token;
	int StartLineCount;
  CStdioFile *pFile1;
  CFileException Err;
  ScriptStruct *SS;
	BOOL bFirstSyntaxError;
	BOOL bSyntaxError;
	CString TokenValue;
	CString SecondRunProgramName;
	CString OurFileName;
	CString OurFullPath;
	CString ScriptPath;
  CString FileName1;   //script filename
	CString UninstallProgramName;

  ValidateScriptToRun(CString TPath);
  ~ValidateScriptToRun();
  void Error(const int StrId);
	void SyntaxError(int Token, int LineNum);
  enumValidateRetCode OpenFile(); // tnn
  int CloseFile();
  enumValidateRetCode ReadFile(bool bIgnorePlatformCheck);
  int SyntaxCheck();
  int IdentifyToken(CString ts);
  int OldGetCmdLine();
  int SetCmdLineVars(CString ourfilename, CString ourfullpath, CString scripttorun, CString scriptpath);
	int CheckSecondRun();
  enumValidateRetCode ValidatePlatform();
  enumValidateRetCode ValidateProgramName();
  int ValidateProgramAndRegistry();
  bool FileExist (const CString FileName);
  void WriteScriptHeader(CString& strPath, CString& strScriptFile);
  void WriteScriptLine(CString& strPath, CString& strScriptFile, int strID);

};

class ReadScript {
public:
	CString TempPath;
	CString SecondRunProgramName;
	CString OurFileName;
	CString OurFullPath;
	CString ScriptPath;
  CString FileName1;
	BOOL bFirstSyntaxError;
	BOOL bSyntaxError;
  CStdioFile *pFile1;
  CFileException Err;
  ScriptStruct *SS;
	int StartLineCount;

  ReadScript(ScriptStruct *ss, int SLNum, CString FName1, CString OFName,
		         CString OFPath, CString SPath, CString SRPName, CString TPath);
  ~ReadScript();
  void Error(const int StrId);
	void SyntaxError(int Token, int LineNum);
  int OpenFile();
  int CloseFile();
  int ReadFile();
  int SyntaxCheck(ScriptStruct *ss);
  int IdentifyToken(CString ts);

};

class RunScript {
public:
  BOOL    bSecondRun;
	CString TempPath;
	CString SecondRunProgramName;
	CString OurFileName;
  CString UninstallScriptPath;  //if installshield is being used?
	CString UninstallScriptName;
	CString UninstallProgramPath;
	CString UninstallProgramName;
  CString strOurNewPath;           //path to store ourselves (bypass NSC limit)c:\temp\jettemp
  CString strOurNewFileLocation;   //the filename from OurFileName + strOurNewPath;
	CString UninstallParameters;
	CString ScriptPath;
	DWORD InteractDuration;
	BOOL RunWinUninstall;
	BOOL bFirstSyntaxError;
  ScriptStruct *SS;
  WIN32_FIND_DATA Findfd;
  HANDLE Findfh;

  RunScript(const BOOL secondrun, ScriptStruct *ss, CString Path, CString TPath, CString ournewpath, CString ournewfilelocation);
  ~RunScript();
  void Error(const int StrId);
	CString GetUninstallPath(HKEY hKey, CString SubKey, CString ReadKey);
	int CallSendWindowsMessage(CString WinName, CString msg);
	int CallSendWindowsMessageAndWait(CString WinName, CString msg, int Duration);
	int GetUninstallProgramAndPath(HKEY hKey, CString SubKey, CString ReadKey);
  int GetUninstallShieldProgramAndPath(HKEY hKey, CString SubKey, CString ReadKey);
	int RestartComputer(DWORD LineCount, CString FileName);
	int ClearRegistration(HKEY hKey, CString SubKey, CString ClearKey);
	int SetRegistryString(HKEY hKey, CString SubKey, CString WriteKey, CString Value, BOOL Create);
	int SetRegistryDword(HKEY hKey, CString SubKey, CString WriteKey, DWORD Value, BOOL Create);
	int CallRunRobot(BOOL SendCancel);
	int CallSpecialRunRobot(DWORD LineCount, CString FileName, CString ReadKey, CString HoldWinName);
	int GetHKey(CString szHKey, HKEY &hKey);
  int Run(int LineNum, CString rsFileName, bool bUninstallSymantecOk, bool bIgnorePlatformCheck);
  int PauseForNotification(DWORD LineCount, CString FileName, CString RunKey);
	int StopProgramForReboot(DWORD LineCount, CString FileName);
	int CheckAndSetRegistryForSecondRun(DWORD LineCount, CString ScriptFileName, CString RunKeyName);
  int ReadRegistryString(HKEY hKey, CString SubKey, CString ReadKey, CString &RetStr);
	int CallSendAffirmativeToWindow(CString WinName, int Duration);
	int CallSendAffirmativeToWindowAfterEnable(CString WinName, CString ButtonName, int Duration);
	int CallSendAffirmativeToWindowWithButtonClick(CString WinName, CString ButtonName, int Duration); //  TNN
	int CallSendTextToEditWindow(CString WinName, CString strText, int Duration);

  bool CheckExistProgram();
  int CallRunProgram(HKEY hKey, CString SubKey, CString ReadKey, bool bUninstallSymantecOk);
  int CopyOurDirectoryToTemp();
  int FindNextJetFile(CString &FName);
  int FindFirstJetFile(CString SearchForPath, CString &FName);
  int DeleteDirectoryAndContents(CString strPath);
  bool IsSymantecProduct (HKEY hKey, CString SubKey, CString ReadKey);

};

class CProcessor	 {
  private:
  ValidateScriptToRun *pVS;
  CString strScriptFileName;
  CString strFileName1;
  CString strOurFileName;
  CString strOurFullPath;
  CString strScriptPath;
  CString strOurNewPath; //points to the temp directory\jettemp
  CString strOurNewFileLocation;
  ErrorLog *ELog;
  BOOL bWroteToFile;
  WIN32_FIND_DATA Findfd;
  HANDLE Findfh;

  bool m_bUninstallSymantecOk;  //  Switch indicates it's ok tp Uninstall Symantec Products.
  bool m_bIgnorePlatform;		//  Switch means ignore Platform line in script file
  bool m_bProcessAllScripts;    // Switch means process all script files in current directory.


  int ProcessScript(ValidateScriptToRun *pVS);
  int GetCmdLine();
  bool CheckforCommandSwitch (CString strSwitch, CString& strCmdLine);

  int RemoveStatusFile();
  int Error(const int StrID);
  int Error( const CString strErr );
  int Success(const int StrID);
  int FindNextNuke(CString &FName);
  int FindFirstNuke(CString &FName);


  public:
  CProcessor();
  ~CProcessor();
  int DoWork();
};

