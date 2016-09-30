
NAVEventsps.dll: dlldata.obj NAVEvents_p.obj NAVEvents_i.obj
	link /dll /out:NAVEventsps.dll /def:NAVEventsps.def /entry:DllMain dlldata.obj NAVEvents_p.obj NAVEvents_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del NAVEventsps.dll
	@del NAVEventsps.lib
	@del NAVEventsps.exp
	@del dlldata.obj
	@del NAVEvents_p.obj
	@del NAVEvents_i.obj
