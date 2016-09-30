@echo on
REM perl scripts
rsh %1 -l root mkdir /data/Avis/Macro/Scripts/perl
rsh %1 -l root rm -rf /data/Avis/Macro/Scripts/perl/*
rcp -a mrep.pl %1.root:/data/Avis/Macro/Scripts/perl
rcp -a mop.pl %1.root:/data/Avis/Macro/Scripts/perl
rcp -a ntshuse.pl %1.root:/data/Avis/Macro/Scripts/perl

REM perl modules
rsh %1 -l root rm -rf /usr/local/lib/perl5/5.00502/AVIS/*
rsh %1 -l root mkdir -p /usr/local/lib/perl5/5.00502/AVIS/Macro
rsh %1 -l root mkdir -p /usr/local/lib/perl5/5.00502/AVIS/Macro/Types
rcp -a DataFlow.pm %1.root:/usr/local/lib/perl5/5.00502
rcp -a Jockey.pm %1.root:/usr/local/lib/perl5/5.00502
rcp -a -r Emulator %1.root:/usr/local/lib/perl5/5.00502/AVIS
rcp -a FSTools.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS
rcp -a Local.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS
rcp -a Log.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS
rcp -a Logger.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS
rcp -a Command.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Connection.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a FTP.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Image.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Netbios.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Macrocrc.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Macrodb.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Mutils.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Replication.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Setup.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro
rcp -a Event.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro/Types
rcp -a Goat.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro/Types
rcp -a List.pm %1.root:/usr/local/lib/perl5/5.00502/AVIS/Macro/Types

REM setup files
rsh %1 -l root mkdir /data/Avis/Macro/Files
rsh %1 -l root rm -rf /data/Avis/Macro/Files/*
rsh %1 -l root mkdir -p /data/Avis/Macro/Files/Config/Images
rcp -r -b Commands %1.root:/data/Avis/Macro/Files
rcp -r -b Applications %1.root:/data/Avis/Macro/Files/Config
rsh %1 -l auto mkdir -p /home/auto/Jockey/Images
rcp -r -b Images %1.root:/data/Avis/Macro/Files/Config

REM programs
rsh %1 -l root rm -rf /data/Avis/Macro/Progs/*
rcp -b rc.exe %1.root:/data/Avis/Macro/Progs
rcp -b killpopup.exe %1.root:/data/Avis/Macro/Progs
rcp -b killapp.exe %1.root:/data/Avis/Macro/Progs
rcp -b killproc.exe %1.root:/data/Avis/Macro/Progs
rcp -b Msvcirt.dll %1.root:/data/Avis/Macro/Progs
rcp -b msvcrt.dll %1.root:/data/Avis/Macro/Progs
rcp -b mfc42.dll %1.root:/data/Avis/Macro/Progs
rcp -b texec.exe %1.root:/data/Avis/Macro/Progs

REM start mop.pl on the machine
REM rsh %1 -l auto perl /data/Avis/Macro/Scripts/perl/mop.pl
REM rsh %1 -l auto perl /data/Avis/Macro/Scripts/perl/mop.pl --reset ^&; exit

@echo off
