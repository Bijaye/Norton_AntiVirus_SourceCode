[InstallShield Silent]
Version=v5.00.000
File=Response File

[Application]
Name=AMS Server
Version=1.00.000
Company=Symantec
Lang=0009

[File Transfer]
OverwriteReadOnly=NoToAll

[DlgOrder]
Dlg0=SdWelcome-0
Dlg1=SdLicense-0
Dlg2=SdAskDestPath-0
Dlg3=SdFinishReboot-0
Count=4

[SdWelcome-0]
Result=1

[SdLicense-0]
Result=1

[SdAskDestPath-0]
szDir=D:\Program Files\AMS Server
Result=1

[SdFinishReboot-0]
Result=1
BootOption=0
