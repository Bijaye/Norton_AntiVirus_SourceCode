attrib -r %2\report.rep
attrib -r %2\Scanlog.txt
del %2\report.rep
del %2\Scanlog.txt
%2\scan32 %1 /nosplash /alwaysex /all /continue /autoscan /autoexit /uinone /nobeep /nomem /noboot
rename %2\ScanLog.txt report.rep

