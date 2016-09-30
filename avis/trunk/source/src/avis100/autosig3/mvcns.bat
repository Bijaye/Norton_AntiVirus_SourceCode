nmake -a -f G:\c\src\autosig2\bldindex\bldindex.mak all
if errorlevel 1 pause
nmake -a -f G:\c\src\autosig2\combindx\combindx.mak all
if errorlevel 1 pause
nmake -a -f G:\c\src\autosig2\trie\trie.mak all
if errorlevel 1 pause
