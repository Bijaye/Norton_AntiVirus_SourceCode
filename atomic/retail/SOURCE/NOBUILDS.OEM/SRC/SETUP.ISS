[InstallShield Silent]
Version=v5.00.000
File=Response File

[DlgOrder]
Dlg0=SdWelcome-0
Count=9
Dlg1=SdLicense-0
Dlg2=SdAskDestPath-0
Dlg3=DlgInitialSettings95-0
Dlg4=SdStartCopy-0
Dlg5=DlgAdvo1-0
Dlg6=DlgAdvo2-0
Dlg7=DlgScanForViruses95-0
Dlg8=SdFinishReboot-0

[SdWelcome-0]
Result=1

[SdLicense-0]
Result=1

[SdAskDestPath-0]
szDir=DEFAULT
Result=1

[DlgInitialSettings95-0]
StartAutoProtect=1
ScheduleScan=1
ScanAtStartup=1
Result=1

[SdStartCopy-0]
Result=1

[Application]
Name=Norton AntiVirus
Version=5.0
Company=Symantec

[DlgAdvo1-0]
Result=1

[DlgAdvo2-0]
Result=1

[DlgScanForViruses95-0]
RunLiveUpdateAfterInstall=0
Rescue=0
RunScanAfterInstall=0
Result=1

[SdFinishReboot-0]
Result=1
BootOption=0
