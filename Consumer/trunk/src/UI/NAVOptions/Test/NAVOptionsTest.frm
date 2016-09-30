VERSION 5.00
Begin VB.Form OptionsTest 
   Caption         =   "Form1"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
End
Attribute VB_Name = "OptionsTest"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Load()
Dim navexcs As New NAVExclusions, navexc As NAVExclusion, l As Long

l = navexcs.Count
Set navexc = New NAVExclusion
navexc.Path = "*.bbb"
navexc.ChangeReadOnlyAttribute = True
navexc.SubFolders = True
Call navexcs.Add(navexc)
l = navexcs.Count
l = navexcs.Item(0).ChangeReadOnlyAttribute
Call navexcs.Save

'Dim navopt As New NAVOptions, v As Variant, s As String, l As Long

'Call navopt.Initialize("{FD26F288-761B-4E23-860D-D6C31151F24A}")
'Call navopt.Load

'l = navopt.TSR.ScanFiles
'l = navopt.SCANNER.ScanFiles
'navopt.SCANNER.ScanFiles = 2
's = navopt.AP.Str
'Call navopt.Save
End Sub
