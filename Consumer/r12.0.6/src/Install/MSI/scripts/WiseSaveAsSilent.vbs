' Wise for Windows Installer utility to compile a WSI project using 'Save As'
' For use with Windows Scripting Host, CScript.exe or WScript.exe
' Copyright (c) 2000, Symantec Corporation
'
Option Explicit
Public isGUI, installer
Dim nReturn, nCompile
nCompile = 0

' Check if run from GUI script host, in order to modify display
If UCase(Mid(Wscript.FullName, Len(Wscript.Path) + 2, 1)) = "W" Then isGUI = True

' Show help if no arguments or if argument contains ?
Dim argCount:argCount = Wscript.Arguments.Count
If argCount > 0 Then If InStr(1, Wscript.Arguments(0), "?", vbTextCompare) > 0 Then argCount = 0
If argCount < 2 Then argCount = 0
If argCount = 0 Then
	Wscript.Echo "-------------------------------------------------------------------------------" &_
		vbLf & "Wise for Windows Installer utility to compile a WSI project using 'Save As'." &_
		vbLf & " The 1st argument is the complete path to a WSI project" &_
		vbLf & " The 2nd argument is the complete path to the output file" &_
		vbLf & "  - The output must have end in .MSI, .EXE, or .WSI if the input is a .WSI" &_
		vbLf & "  - The output must have end in .MSM or .WSM if the input is a .WSM" &_
		vbLf & " Any extra arguments are property settings in the format of:" &_
		vbLf & " ""Property"" ""Value"" ""Property"" ""Value"""  &_
		vbLf & "-------------------------------------------------------------------------------"
	Wscript.Quit 1
End If

If Right(Wscript.Arguments(0), 4) = ".wsi" Then
	If Right(Wscript.Arguments(1), 4) = ".exe" OR Right(Wscript.Arguments(1), 4) = ".msi" Then
		nCompile = 1
	ElseIf Right(Wscript.Arguments(1), 4) = ".wsi" Then
		nCompile = 0
	Else
		Fail "Invalid File Extension " & Right(Wscript.Arguments(1), 4) & ", the output must have the ext. .MSI, .EXE, or .WSI"
	End If
ElseIf Right(Wscript.Arguments(0), 4) = ".wsm" Then
	If Right(Wscript.Arguments(1), 4) = ".msm" Then
		nCompile = 1
	ElseIf Right(Wscript.Arguments(1), 4) = ".wsm" Then
		nCompile = 0
	Else
		Fail "Invalid File Extension " & Right(Wscript.Arguments(1), 4) & ", the output must have the ext. .WSM or .MSM"
	End If
Else
		Fail "Invalid File Extension " & Right(Wscript.Arguments(0), 4) & ", the input must be a .WSI or a .WSM"	
End If

' Connect to Wise for Windows Installer object
REM On Error Resume Next
Set installer = Nothing
Set installer = Wscript.CreateObject("WfWi.Document") : CheckNonZeroError("While connecting to WfWi.Document")

' Set WfWi to silent mode
installer.SetSilent()

' Open database
Wscript.Echo "Opening " & Wscript.Arguments(0) & "..."
nReturn = installer.Open(Wscript.Arguments(0))
CheckZeroError("While Opening " & Wscript.Arguments(0))

' Set an option so that it will compile as a single EXE if that's what was asked for
If Right(Wscript.Arguments(1), 4) = ".exe" Then
	nReturn = installer.SetMediaOption("EXE Build", "1")
End If

' Loop through all the aditional parameters, setting each property
Dim arg:arg = 2
Do
	If arg >= argCount Then Exit Do
	nReturn = installer.SetProperty(Wscript.Arguments(arg), Wscript.Arguments(arg + 1))
	CheckNonZeroError("While setting the property """ & Wscript.Arguments(arg) & """ to """ & Wscript.Arguments(arg + 1) & """")
	Wscript.Echo "Set the property """ & Wscript.Arguments(arg) & """ to """ & Wscript.Arguments(arg + 1) & """"
	arg = arg + 2
Loop

If nCompile = 1 Then
	' Compile the database
	Wscript.Echo "Compiling to " & Wscript.Arguments(1) & "..."
	nReturn = installer.Compile(Wscript.Arguments(1))
	CheckZeroError("While Compiling " & Wscript.Arguments(1))
ElseIf nCompile = 0 Then
	' Save the database
	Wscript.Echo "Saving to " & Wscript.Arguments(1) & "..."
	nReturn = installer.Save(Wscript.Arguments(1))
	CheckZeroError("While Saving " & Wscript.Arguments(1))
End If

' Quit
Wscript.Quit 0

Sub CheckNonZeroError(doing)
	Dim message, errRec
	If Err = 0 Then Exit Sub
	If Not installer Is Nothing Then
		message = "Error: " & doing & vbLf & installer.GetLastError()
	End If
	Fail message
End Sub

Sub CheckZeroError(doing)
	Dim message
	If nReturn <> 0 Then Exit Sub
	If Not installer Is Nothing Then
		message = "Error: " & doing & vbLf & installer.GetLastError()
	End If
	Fail message
End Sub

Sub Fail(message)
	Wscript.Echo message
	Wscript.Quit 2
End Sub
