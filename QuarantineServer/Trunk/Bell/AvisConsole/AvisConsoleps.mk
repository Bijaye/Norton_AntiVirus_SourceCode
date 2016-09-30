
AvisConsoleps.dll: dlldata.obj AvisConsole_p.obj AvisConsole_i.obj
	link /dll /out:AvisConsoleps.dll /def:AvisConsoleps.def /entry:DllMain dlldata.obj AvisConsole_p.obj AvisConsole_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del AvisConsoleps.dll
	@del AvisConsoleps.lib
	@del AvisConsoleps.exp
	@del dlldata.obj
	@del AvisConsole_p.obj
	@del AvisConsole_i.obj
