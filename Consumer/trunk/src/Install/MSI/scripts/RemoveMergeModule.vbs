' Wise for Windows Installer utility to remove a merge module (MSM) from a WSI database
' For use with Windows Scripting Host, CScript.exe or WScript.exe
' Copyright (c) 1999, Symantec Corporation
'
Option Explicit
'global variables access across functions
Public isGUI, installer, WSIdatabase, MMdatabase, message, WSIdatabasePath, MMdatabasePath

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
If argCount <> 2 Then
	Wscript.Echo "Wise for Windows Installer utility to remove a merge module(MSM) from a WSI database" &_
	    vbLf & "-------------------------------------------------------------------------------" &_
		vbLf & " The 1st argument is the path to a WSI database, relative or complete path" &_
		vbLf & " The 2nd argument is the path to a MSM merge module database, complete path" &_
		vbLf & vbLf & "  --------------------------------------------------------" &_
		vbLf & "  -- Be sure to put all arguments with spaces in quotes --" &_		
		vbLf & "  --------------------------------------------------------"
	Wscript.Quit 1
End If

' Connect to Windows Installer object
REM On Error Resume Next
Set installer = Nothing
Set installer = Wscript.CreateObject("WindowsInstaller.Installer") : CheckError

' Open database
WSIdatabasePath = Wscript.Arguments(0)
MMdatabasePath  = Wscript.Arguments(1)
Set WSIdatabase = installer.OpenDatabase(WSIdatabasePath, msiOpenDatabaseModeTransact) : CheckError
Set MMdatabase  = installer.OpenDatabase(MMdatabasePath, msiOpenDatabaseModeReadOnly) : CheckError
Dim sig, lang, ver

' Get the module signature from the MM database, then remove it from the WSI database
GetModuleSignature MMdatabase, sig, lang, ver
RemoveModuleSignature WSIdatabase, sig, lang, ver

Wscript.Echo "The following merge module was removed from " & WSIdatabasePath & " in the:"
Wscript.Echo "-------------------------------------------------------------------------------"
Wscript.Echo "   ModuleID: " & sig
Wscript.Echo "   Language: " & lang
Wscript.Echo "    Version: " & ver
Wscript.Echo vbLf & "The refrence in the WiseSourcePath table was also removed"

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

Sub RemoveModuleSignature(database, moduleID, language, version)
	Dim view, record, newRecord, sqlInsert, sqlSelect, Columns, removed
	Dim guid:guid = InStrRev(moduleID, ".")	
	If guid = 0 Then Fail("Invalid Module Signature in " & sourcepath)
	guid = Right(moduleID, Len(moduleID) - guid)
	removed = 0

	' Remove the item from WiseModuleSignature
	sqlSelect = "SELECT * FROM `WiseModuleSignature` WHERE "
	sqlSelect = sqlSelect & "`ModuleID` = '" & moduleID & "'"
	Set view = database.OpenView(sqlSelect) : CheckError
	view.Execute : CheckError
	Do
		Set record = view.Fetch : CheckError
		If record Is Nothing Then Exit Do
			
		' Delete this record
		view.Modify msiViewModifyDelete, record
		removed = removed + 1
	Loop
		
	' Remove the item from WiseSourcePath
	sqlSelect = "SELECT * FROM `WiseSourcePath` WHERE `File_` = '" & guid & "'"
	Set view = database.OpenView(sqlSelect) : CheckError
	view.Execute : CheckError
	Do
		Set record = view.Fetch : CheckError
		If record Is Nothing Then Exit Do
			
		' Delete this record
		view.Modify msiViewModifyDelete, record
		removed = removed + 1
	Loop

	If removed > 0 Then		
		database.Commit
	Else
		Fail "The merge module, " & moduleID & " was not found in " & WSIdatabasePath
	End If
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
