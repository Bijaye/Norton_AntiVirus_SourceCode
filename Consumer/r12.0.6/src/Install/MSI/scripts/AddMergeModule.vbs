' Wise for Windows Installer utility to add a merge module (MSM) to a WSI database
' For use with Windows Scripting Host, CScript.exe or WScript.exe
' Copyright (c) 1999, Symantec Corporation
'
Option Explicit
'global variables access across functions
Public isGUI, installer, WSIdatabase, MMdatabase, message, WSIdatabasePath, MMdatabasePath, FeatureList, RedirectDir

Const msiOpenDatabaseModeReadOnly = 0
Const msiOpenDatabaseModeTransact = 1
Const msiOpenDatabaseModeDirect   = 2
Const msiDbNullInteger            = &h80000000
Const msiViewModifyInsert         = 1
Const msiViewModifyUpdate         = 2
Const msiViewModifyDelete         = 6

' Check if run from GUI script host, in order to modify display
If UCase(Mid(Wscript.FullName, Len(Wscript.Path) + 2, 1)) = "W" Then isGUI = True

' Show help if no arguments or if argument contains ?
Dim argCount:argCount = Wscript.Arguments.Count
If argCount > 0 Then If InStr(1, Wscript.Arguments(0), "?", vbTextCompare) > 0 Then argCount = 0
If argCount < 2 or argCount > 6 Then
	Wscript.Echo "Wise for Windows Installer utility to add a merge module(MSM) to a WSI database" &_
	    vbLf & "-------------------------------------------------------------------------------" &_
		vbLf & " The 1st argument is the path to a WSI database, relative or complete path" &_
		vbLf & " The 2nd argument is the path to a MSM merge module database, complete path" &_
		vbLf & " The 3rd argument is the FeatureList value, default=Complete" &_
		vbLf & " The 4th argument is the RedirectDir value, default=INSTALLDIR" &_
		vbLf & vbLf & "  --------------------------------------------------------" &_
		vbLf & "  -- Be sure to put all arguments with spaces in quotes --" &_		
		vbLf & "  --------------------------------------------------------"
	Wscript.Quit 1
End If

' Connect to Windows Installer object
REM On Error Resume Next
Set installer = Nothing
Set installer = Wscript.CreateObject("WindowsInstaller.Installer") : CheckError

' save the arguments
If argCount >= 3 Then
	FeatureList = Wscript.Arguments(2)
	If argCount >= 4 Then
		RedirectDir = Wscript.Arguments(3)
	End If
End If

' fix the arguments
If Len(FeatureList) <= 0 Then
	FeatureList = "Complete"
End If

If Len(RedirectDir) <= 0 Then
	RedirectDir = "INSTALLDIR"
End If

' Open database
WSIdatabasePath = Wscript.Arguments(0)
MMdatabasePath  = Wscript.Arguments(1)
Set WSIdatabase = installer.OpenDatabase(WSIdatabasePath, msiOpenDatabaseModeTransact) : CheckError
Set MMdatabase  = installer.OpenDatabase(MMdatabasePath, msiOpenDatabaseModeReadOnly) : CheckError
Dim sig, lang, ver

' Get the module signature from the MM database, then push all the info into the WSI database
GetModuleSignature MMdatabase, sig, lang, ver
InsertModuleSignature WSIdatabase, sig, lang, ver, MMdatabasePath, FeatureList, RedirectDir

Wscript.Echo "The following row was added to " & WSIdatabasePath & " in the WiseModuleSignature table:"
Wscript.Echo "-------------------------------------------------------------------------------"
Wscript.Echo "   ModuleID: " & sig
Wscript.Echo "   Language: " & lang
Wscript.Echo "    Version: " & ver
Wscript.Echo " SourcePath: " & MMdatabasePath
Wscript.Echo "FeatureList: " & FeatureList
Wscript.Echo "RedirectDir: " & RedirectDir
Wscript.Echo "Description: {null}"
Wscript.Echo " Exclusions: {null}"
Wscript.Echo vbLf & "The correct refrence was also made in WiseSourcePath"

Wscript.Quit 0


Sub GetModuleSignature(database, signature, language, version)
	Dim view, record, moduleSig
	Set view = database.OpenView("SELECT * FROM `ModuleSignature`") : CheckError
	Set moduleSig = installer.CreateRecord(1)
	view.Execute moduleSig : CheckError
	Set record = view.Fetch : CheckError
	
	' return the record
	signature = record.StringData(1)
	language = record.StringData(2)
	version = record.StringData(3)
End Sub

Sub InsertModuleSignature(database, moduleID, language, version, sourcepath, featureList, redirectdir)
	Dim view, record, newRecord, sqlInsert, sqlSelect, Columns
	Dim guid:guid = InStrRev(moduleID, ".")	
	If guid = 0 Then Fail("Invalid Module Signature in " & sourcepath)
	guid = Right(moduleID, Len(moduleID) - guid)

	' Insert the item into WiseModuleSignature, if it doesn't exist
	sqlSelect = "SELECT * FROM `WiseModuleSignature` WHERE `ModuleID` = '" & moduleID & "'"
	Set view = database.OpenView(sqlSelect) : CheckError
	view.Execute : CheckError
	Set record = view.Fetch : CheckError
	If record Is Nothing Then
		Columns = "(`ModuleID`, `Language`, `Version`, `SourcePath`, `FeatureList`, `RedirectDir`)"
		sqlInsert = "INSERT INTO `WiseModuleSignature` " & Columns & " VALUES ("
		sqlInsert = sqlInsert & "'" & moduleID & "', "
		sqlInsert = sqlInsert & "'" & language & "', "
		sqlInsert = sqlInsert & "'" & version & "', "
		sqlInsert = sqlInsert & "'" & sourcepath & "', "
		sqlInsert = sqlInsert & "'" & featureList & "', "
		sqlInsert = sqlInsert & "'" & redirectdir & "') "
		Set view = database.OpenView(sqlInsert) : CheckError
		view.Execute : CheckError
	End If
		
	' Insert the item into WiseSourcePath, if it doesn't exist
	sqlSelect = "SELECT * FROM `WiseSourcePath` WHERE `File_` = '" & guid & "'"
	Set view = database.OpenView(sqlSelect) : CheckError
	view.Execute : CheckError
	Set record = view.Fetch : CheckError
	If record Is Nothing Then
		sqlInsert = "INSERT INTO `WiseSourcePath` (`File_`, `SourcePath`, `Date`, `Attributes`) VALUES ("
		sqlInsert = sqlInsert & "'" & guid & "', "
		sqlInsert = sqlInsert & "'" & sourcepath & "*" & redirectdir & "', 0, 0)"
		Set view = database.OpenView(sqlInsert) : CheckError
		view.Execute : CheckError
	End If
		
	database.Commit
End Sub

Sub CheckError
	Dim message, errRec
	If Err = 0 Then Exit Sub
	message = Err.Source & " " & Hex(Err) & ": " & Err.Description
	If Not installer Is Nothing Then
		Set errRec = installer.LastErrorRecord
		If Not errRec Is Nothing Then message = message & vbLf & errRec.FormatText
	End If
	Fail message
End Sub

Sub Fail(message)
	Wscript.Echo message
	Wscript.Quit 2
End Sub
