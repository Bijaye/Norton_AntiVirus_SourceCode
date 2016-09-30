
NAVUIps.dll: dlldata.obj NAVUI_p.obj NAVUI_i.obj
	link /dll /out:NAVUIps.dll /def:NAVUIps.def /entry:DllMain dlldata.obj NAVUI_p.obj NAVUI_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del NAVUIps.dll
	@del NAVUIps.lib
	@del NAVUIps.exp
	@del dlldata.obj
	@del NAVUI_p.obj
	@del NAVUI_i.obj
