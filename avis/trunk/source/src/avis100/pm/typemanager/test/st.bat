copy C:\till\is\pm\avis\bytes2\*.* D:\perl\lib\avis\
copy C:\till\is\pm\avis\fstools\*.* D:\perl\lib\avis\
copy C:\till\is\pm\avis\local\*.* D:\perl\lib\avis\
copy C:\till\is\pm\avis\log\*.* D:\perl\lib\avis\
copy C:\till\is\pm\avis\macros\*.* d:\perl\lib\avis\
rem copy C:\till\is\pm\dataflow\*.* D:\perl\lib\
if "%1"=="" goto perl
del C:\test\vrun.lst
:perl
perl c:\till\is\analyzer\vrepctrl.pl --Cookie=4711 --ProcessID=99 --Parameter=hallo --unc=c:\test
