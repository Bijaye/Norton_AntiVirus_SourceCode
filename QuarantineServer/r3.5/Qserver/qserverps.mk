
qserverps.dll: dlldata.obj qserver_p.obj qserver_i.obj qserverps.res
	link /dll /out:qserverps.dll /def:qserverps.def /entry:DllMain dlldata.obj qserver_p.obj qserver_i.obj qserverps.res \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0500 /DREGISTER_PROXY_DLL \
		$<
.rc.res:
	rc /i ..\Shared\Include /DWIN32 /D_WIN32_WINNT=0x0400 $<

clean:
	@del qserverps.dll
	@del qserverps.lib
	@del qserverps.exp
	@del dlldata.obj
	@del qserver_p.obj
	@del qserver_i.obj
	@del qserverps.res
