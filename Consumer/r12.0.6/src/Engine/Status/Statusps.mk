
Statusps.dll: dlldata.obj Status_p.obj Status_i.obj
	link /dll /out:Statusps.dll /def:Statusps.def /entry:DllMain dlldata.obj Status_p.obj Status_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del Statusps.dll
	@del Statusps.lib
	@del Statusps.exp
	@del dlldata.obj
	@del Status_p.obj
	@del Status_i.obj
