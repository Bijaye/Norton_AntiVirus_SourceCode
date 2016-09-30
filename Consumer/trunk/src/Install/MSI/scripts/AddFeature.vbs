' Wise for Windows Installer utility to add a Feature to a WSI database
' For use with Windows Scripting Host, CScript.exe or WScript.exe
' Copyright (c) 2002, Symantec Corporation
'
Option Explicit
'global variables access across functions
Public isGUI, installer, WSIdatabase, MMdatabase, message, WSIdatabasePath, NewFeauture, ParentFeature, Directory, Attributes

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
If argCount < 5 or argCount > 5 Then
	Wscript.Echo "Wise for Windows Installer utility to add a Feature to a WSI database" &_
	    vbLf & "-------------------------------------------------------------------------------" &_
		vbLf & " The 1st argument is the path to a WSI database, relative or complete path" &_
		vbLf & " The 2nd argument is the name of the new Feature" &_
		vbLf & " The 3rd argument is the name of the new Feature's Parent" &_
		vbLf & " The 4th argument is the name of the new Feature's Directory" &_
		vbLf & " The 5th argument is the name of the new Feature's Attributes" &_
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
NewFeauture = Wscript.Arguments(1)
ParentFeature = Wscript.Arguments(2)
Directory = Wscript.Arguments(3)
Attributes = Wscript.Arguments(4)

Set WSIdatabase = installer.OpenDatabase(WSIdatabasePath, msiOpenDatabaseModeTransact) : CheckError

InsertFeature WSIdatabase, NewFeauture, ParentFeature, Directory, Attributes

Wscript.Echo "The following row was added to " & WSIdatabasePath & " in the Feature table:"
Wscript.Echo "-------------------------------------------------------------------------------"
Wscript.Echo "       Feature: " & NewFeauture
Wscript.Echo "Feature_Parent: " & ParentFeature
Wscript.Echo "         Title: " & NewFeauture
Wscript.Echo "       Display: 0"
Wscript.Echo "         Level: 3"
Wscript.Echo "    Directory_: " & Directory
Wscript.Echo "    Attributes: " & Attributes
Wscript.Echo vbLf

Wscript.Quit 0

Sub InsertFeature(database, NewFeature, ParentFeature, Directory, Attributes)
	Dim view, record, newRecord, sqlInsert, sqlSelect, Columns

	' Insert the item into Feature, if it doesn't exist
	sqlSelect = "SELECT * FROM `Feature` WHERE `Feature` = '" & NewFeature & "'"
	Set view = database.OpenView(sqlSelect) : CheckError
	view.Execute : CheckError
	Set record = view.Fetch : CheckError
	If record Is Nothing Then
		Columns = "(`Feature`, `Feature_Parent`, `Title`, `Display`, `Level`, `Directory_`, `Attributes`)"
		sqlInsert = "INSERT INTO `Feature` " & Columns & " VALUES ("
		sqlInsert = sqlInsert & "'" & NewFeature & "', "
		sqlInsert = sqlInsert & "'" & ParentFeature & "', "
		sqlInsert = sqlInsert & "'" & NewFeature & "', "
		sqlInsert = sqlInsert & "'0', "
		sqlInsert = sqlInsert & "'3', "
		sqlInsert = sqlInsert & "'" & Directory & "', "
		sqlInsert = sqlInsert & "'" & Attributes & "') "
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
