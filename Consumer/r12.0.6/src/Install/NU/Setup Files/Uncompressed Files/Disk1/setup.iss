[InstallShield Silent]
Version=v5.00.000
File=Response File

[File Transfer]
OverwriteReadOnly=NoToAll

[DlgOrder]
Dlg0=SdWelcome-0
Count=5
Dlg1=SdLicense-0
Dlg2=SdAskDestPath-0
Dlg3=SdStartCopy-0
Dlg4=SdFinishReboot-0

[SdWelcome-0]
Result=1

[SdLicense-0]
Result=1

[SdAskDestPath-0]
szDir=%INSTALLDIR%
Result=1

[SdStartCopy-0]
Result=1

[Application]
Name=MiniNU
Version=1.00.000
Company=Symantec Corp.
Lang=0009

[SdFinishReboot-0]
Result=1
BootOption=0
