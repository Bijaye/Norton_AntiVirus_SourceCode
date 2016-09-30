CALL d:\msdev\DevStudio\VC\bin\Vcvars32.bat
E:
cd \src
del Avis100 /s/f/q
cd E:\src\Avis100
ss label -I- -L%1 $/Avis100
ss get $/Avis100/Build/Isbuild.bat
Isbuild.bat %1
