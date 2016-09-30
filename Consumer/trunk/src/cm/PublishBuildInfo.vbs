Option Explicit
On Error Resume Next

'-----------------------------------------------
' Tool to send build information to the Localization teams
'-----------------------------------------------

Dim oArgs, ArgNum
Dim strProductName, strCodename, strVersion, strBEPath, strLayoutPath, strP4Label
Set oArgs = WScript.Arguments
ArgNum = 0

strProductName = ""
strCodename = ""
strVersion = ""
strBEPath = ""
strLayoutPath = ""
strP4Label = ""

WScript.Echo "-------------------------------------------------------------------"
WScript.Echo "Inform Localization Web Service that a build has been completed"
WScript.Echo "-------------------------------------------------------------------"

'WScript.Echo "Result: " & c.SubmitBuild("NortonAntiVirus","CHAKA","12.0.0.94b","\\ussm-cpd\ussm-cpd\build_environments\nortonantivirus_12\94b\xyz.zip","\\ussm-cpd\ussm-cpd\CoreComponents\xyz\94b")



While ArgNum < oArgs.Count
	If (ArgNum + 1) >= oArgs.Count Then
		Call DisplayHelp
	End If 

	Select Case LCase(oArgs(ArgNum))
		Case "-p","/p":
			ArgNum = ArgNum + 1
			strProductName = oArgs(ArgNum)
		Case "-c","/c":
			ArgNum = ArgNum + 1
			strCodename = oArgs(ArgNum)
		Case "-v","/v":
			ArgNum = ArgNum + 1
			strVersion = oArgs(ArgNum)
		Case "-z","/z":
			ArgNum = ArgNum + 1
			strBEPath = oArgs(ArgNum)
		Case "-l","/l":
			ArgNum = ArgNum + 1
			strLayoutPath = oArgs(ArgNum)
		Case "-4","/4":
			ArgNum = ArgNum + 1
			strP4Label = oArgs(ArgNum)
		Case "?","-h","/h":
			Call DisplayHelp
	End Select 
	ArgNum = ArgNum + 1
Wend

If (strProductName = "") OR (strCodename = "") OR (strVersion = "")  Then
        WScript.Echo " " 
 	WScript.Echo "*** WARNING: Insufficient parameters! ***"
        WScript.Echo " " 
		Call DisplayHelp
End If 


'-----------------------------------------------
' Echo out the parameters we are going to send
'-----------------------------------------------


If strBEPath = "" then
	strBEPath = "null"
End If
If strLayoutPath = "" then
	strLayoutPath = "null"
End If
If strP4Label = "" then
	strP4Label = "null"
End If

WScript.Echo "Product Name:   " & strProductName
WScript.Echo "Code Name:      " & strCodename
WScript.Echo "Version:        " & strVersion
WScript.Echo "BE Path:        " & strBEPath
WScript.Echo "Layout Path:    " & strLayoutPath
WScript.Echo "perforce Label: " & strP4Label


'-----------------------------------------------
' Now sed the build details
'-----------------------------------------------


'----
' Depending on what is installed on the Client/Build machine, there are 
' a variety of mechanisms for sending the build details. We will first 
' attempt to do so using MSSOAP.SOAPClient that gets installed with .NET,
' then we will try MSOSOAP.SOAPClient30, then WinHttp.WinHttpRequest.5.1
' and finally WinHttp.WinHttpRequest.5
'----

Dim oTestObject
set oTestObject = CreateObject("MSSOAP.SOAPClient")

WScript.Echo "Trying [MSSOAP.SOAPClient] ...."
If Err.Number <> 0 Then 
	Err.Clear
	WScript.Echo "Trying [MSOSOAP.SOAPClient30] ...."
	set oTestObject = Nothing
	set oTestObject = CreateObject("MSOSOAP.SOAPClient30")

	If Err.Number <> 0 Then 
		Err.Clear
		WScript.Echo "Trying [WinHttp.WinHttpRequest.5.1] ...."
		set oTestObject = Nothing
		set oTestObject = CreateObject("WinHttp.WinHttpRequest.5.1")
		'WScript.Echo "ROB"

		If Err.Number <> 0 Then 
			Err.Clear
			set oTestObject = Nothing
			set oTestObject = CreateObject("WinHttp.WinHttpRequest.5")
			WScript.Echo "Trying [WinHttp.WinHttpRequest.5] ...."

			If Err.Number <> 0 Then 
				WScript.Echo "Error: Unable to send build information to Localization Web server!"
			Else
				SendBuildInfo_WinHttpRequest_5()
			End If
		Else
			SendBuildInfo_WinHttpRequest_5_1()
		End If
	Else
		SendBuildInfoSOAPClient30()
	End If
Else
	SendBuildInfoSOAPClient()
End If

'-----------------------------------------------
' Send Build Information using MSSOAP.SOAPClient
'-----------------------------------------------
Sub SendBuildInfoSOAPClient()
	WScript.Echo ""
	WScript.Echo "interface:> MSSOAP.SOAPClient"
	WScript.Echo ""

	Dim Service
	set Service = CreateObject("MSSOAP.SOAPClient")

	Service.mssoapinit("http://localisation.symantec.com/orbit/Services/PublishBuildInfo.asmx?WSDL")
	WScript.Echo "Result: " & Service.SubmitBuild(strProductName, strCodename, strVersion, strBEPath, strLayoutPath, strP4Label) 
	Set Service = Nothing
End Sub

'-----------------------------------------------
' Send Build Information using MSOSOAP.SOAPClient30
'-----------------------------------------------
Sub SendBuildInfoSOAPClient30()
	WScript.Echo ""
	WScript.Echo "interface:> MSOSOAP.SOAPClient30"
	WScript.Echo ""

	Dim Service
	set Service = CreateObject("MSOSOAP.SOAPClient30")
	service.mssoapinit("http://localisation.symantec.com/orbit/Services/PublishBuildInfo.asmx?WSDL")
	WScript.Echo "Result: " & service.SubmitBuild(strProductName, strCodename, strVersion, strBEPath, strLayoutPath, strP4Label) 
	Set Service = Nothing

End Sub

'-----------------------------------------------
' Send Build Information using WinHttp.WinHttpRequest.5.1
'-----------------------------------------------
Sub SendBuildInfo_WinHttpRequest_5_1()
	WScript.Echo ""
	WScript.Echo "interface:> WinHttp.WinHttpRequest.5.1"
	WScript.Echo ""

	Dim objWinHttp
	Dim strHTML
	Dim strURL

	strURL = "http://localisation.symantec.com/orbit/Services/PublishBuildInfo.aspx"

	strURL = strURL & "?pn=" & strProductName
	strURL = strURL & "&cn=" & strCodename
	strURL = strURL & "&vn=" & strVersion
	strURL = strURL & "&be=" & strBEPath
	strURL = strURL & "&p=" & strP4Label
	strURL = strURL & "&l=" & strLayoutPath

	Set objWinHttp = CreateObject("WinHttp.WinHttpRequest.5.1")
	objWinHttp.Open "GET", strURL
	objWinHttp.Send

	If objWinHttp.Status = "200" Then
		'WScript.Echo "Status: " & objWinHttp.Status & " " & objWinHttp.StatusText 
		strHTML = objWinHttp.ResponseText
		WScript.Echo "Result: " & strHTML 
	Else
		WScript.Echo "Error: Unable to contact localization web server!"
	End If

	Set objWinHttp = Nothing

End Sub

'-----------------------------------------------
' Send Build Information using WinHttp.WinHttpRequest.5
'-----------------------------------------------
Sub SendBuildInfo_WinHttpRequest_5()
	WScript.Echo ""
	WScript.Echo "interface:> WinHttp.WinHttpRequest.5"
	WScript.Echo ""

	Dim objWinHttp
	Dim strHTML
	Dim strURL

	strURL = "http://localisation.symantec.com/orbit/Services/PublishBuildInfo.aspx"

	strURL = strURL & "?pn=" & strProductName
	strURL = strURL & "&cn=" & strCodename
	strURL = strURL & "&vn=" & strVersion
	strURL = strURL & "&be=" & strBEPath
	strURL = strURL & "&p=" & strP4Label
	strURL = strURL & "&l=" & strLayoutPath

	Set objWinHttp = CreateObject("WinHttp.WinHttpRequest.5")
	objWinHttp.Open "GET", strURL
	objWinHttp.Send

	If objWinHttp.Status = "200" Then
		'WScript.Echo "Status: " & objWinHttp.Status & " " & objWinHttp.StatusText 
		strHTML = objWinHttp.ResponseText
		WScript.Echo "Result: " & strHTML 
	Else
		WScript.Echo "Error: Unable to contact localization web server!"
	End If

	Set objWinHttp = Nothing

End Sub

'-----------------------------------------------
' Display Help
'-----------------------------------------------
Sub DisplayHelp()
 	WScript.Echo "Syntax:"
        WScript.Echo "   *[-p] Product Name, e.g. NortonAntiVirus"
        WScript.Echo "   *[-c] Project Codename, e.g. CHAKA"
        WScript.Echo "   *[-v] Project Version, including build number, e.g. 12.0.0.94" 
        WScript.Echo "    [-z] Fully qualified path to build environment archive (ZIP) in UNC format" 
        WScript.Echo "    [-l] Fully qualified path to Layout using UNC format" 
        WScript.Echo "    [-4] Perforce Label used to label this build" 
        WScript.Echo "    [-h] Display Help" 
        WScript.Echo " " 
        WScript.Echo "   * = Required Paramers" 
        WScript.Echo " " 
        WScript.Echo "Example: " 
        WScript.Echo "   cscript xyz.vbs"
	WScript.Echo "                   -p 'NortonAntiVirus' "
	WScript.Echo "                   -c 'CHAKA' -v '12.0.0.94'"
	WScript.Echo "                   -z '\\ussm-cpd\ussm-cpd\build_environments\nortonantivirus_12\94b\xyz.zip'"
        WScript.Echo "                   -l '\\ussm-cpd\ussm-cpd\CoreComponents\xyz\94b'" 
        WScript.Echo "                   -4 'Perforce_Label'" 
        WScript.Echo " " 
        WScript.Echo " View submitted details at: " 
        WScript.Echo "     http://localisation.symantec.com/orbit/Public/ShowBuildInfo.aspx" 
        WScript.Echo " " 

	WScript.Quit -1
End Sub

