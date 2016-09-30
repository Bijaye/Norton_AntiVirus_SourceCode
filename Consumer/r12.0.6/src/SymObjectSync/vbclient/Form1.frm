VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5325
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   5325
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox TimeOut 
      Height          =   375
      Left            =   2280
      TabIndex        =   10
      Text            =   "-1"
      Top             =   2640
      Width           =   1335
   End
   Begin VB.CommandButton Wait 
      Caption         =   "Wait"
      Height          =   375
      Left            =   240
      TabIndex        =   9
      Top             =   2640
      Width           =   1815
   End
   Begin VB.CommandButton ResetEvent 
      Caption         =   "ResetEvent"
      Height          =   375
      Left            =   2520
      TabIndex        =   8
      Top             =   2040
      Width           =   1815
   End
   Begin VB.CommandButton SetEvent 
      Caption         =   "SetEvent"
      Height          =   375
      Left            =   2520
      TabIndex        =   7
      Top             =   1560
      Width           =   1815
   End
   Begin VB.CommandButton Initialize 
      Caption         =   "Initialize"
      Height          =   375
      Left            =   2520
      TabIndex        =   6
      Top             =   960
      Width           =   1935
   End
   Begin VB.TextBox InitialState 
      Height          =   315
      Left            =   1200
      TabIndex        =   5
      Text            =   "0"
      Top             =   960
      Width           =   975
   End
   Begin VB.TextBox ManualReset 
      BeginProperty DataFormat 
         Type            =   5
         Format          =   ""
         HaveTrueFalseNull=   1
         TrueValue       =   "True"
         FalseValue      =   "False"
         NullValue       =   ""
         FirstDayOfWeek  =   0
         FirstWeekOfYear =   0
         LCID            =   1033
         SubFormatType   =   7
      EndProperty
      Height          =   285
      Left            =   1200
      TabIndex        =   2
      Text            =   "0"
      Top             =   600
      Width           =   975
   End
   Begin VB.TextBox NamedEvent 
      Height          =   375
      Left            =   1200
      TabIndex        =   0
      Text            =   "NamedEvent"
      Top             =   120
      Width           =   3495
   End
   Begin VB.Label Label3 
      Caption         =   "InitialState"
      Height          =   255
      Left            =   240
      TabIndex        =   4
      Top             =   1080
      Width           =   855
   End
   Begin VB.Label Label2 
      Caption         =   "Manual Reset:"
      Height          =   255
      Left            =   0
      TabIndex        =   3
      Top             =   720
      Width           =   1095
   End
   Begin VB.Label Label1 
      Caption         =   "Name:"
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   240
      Width           =   615
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim oObj As SYMOBJECTSYNCLib.SymSyncEvent

Private Sub Text1_Change()

End Sub

Private Sub Command1_Click()

End Sub

Private Sub Form_Load()
Set oObj = New SYMOBJECTSYNCLib.SymSyncEvent

End Sub

Private Sub Initialize_Click()
    Call oObj.InitializeEvent(ManualReset.Text, InitialState.Text, NamedEvent.Text)
End Sub

Private Sub ResetEvent_Click()
Dim nRet As Long
nRet = oObj.ResetEvent()
    MsgBox "Reset " + Str(nRet)

End Sub

Private Sub SetEvent_Click()
Dim nRet As Long
nRet = oObj.SetEvent()
    MsgBox "SetEvent " + Str(nRet)
End Sub

Private Sub Wait_Click()
    Dim nRet As Long
    nRet = oObj.Wait(TimeOut.Text, 1)
    MsgBox "Wait " + Str(nRet)
End Sub
