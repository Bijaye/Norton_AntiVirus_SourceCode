
NAVAPSCRps.dll: dlldata.obj NAVAPSCR_p.obj NAVAPSCR_i.obj
	link /dll /out:NAVAPSCRps.dll /def:NAVAPSCRps.def /entry:DllMain dlldata.obj NAVAPSCR_p.obj NAVAPSCR_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del NAVAPSCRps.dll
	@del NAVAPSCRps.lib
	@del NAVAPSCRps.exp
	@del dlldata.obj
	@del NAVAPSCR_p.obj
	@del NAVAPSCR_i.obj
