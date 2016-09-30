cd dfeval
nmake -f dfevvcnt.mak depend
nmake -f dfevvcnt.mak compile
nmake -f dfevvcnt.mak library
cd ..\dfmanager
nmake -f dfmgvcnt.mak depend
nmake -f dfmgvcnt.mak compile
nmake -f dfmgvcnt.mak library
cd ..\avisdfrl
nmake -f dfrlvcnt.mak depend
nmake -f dfrlvcnt.mak compile
nmake -f dfrlvcnt.mak library
nmake -f dfrlvcnt.mak exedll
cd ..\dfgui
nmake -f dfguvcnt.mak depend
nmake -f dfguvcnt.mak compile
nmake -f dfguvcnt.mak exedll
cd ..

