
ccPasswdps.dll: dlldata.obj ccPasswd_p.obj ccPasswd_i.obj
	link /dll /out:ccPasswdps.dll /def:ccPasswdps.def /entry:DllMain dlldata.obj ccPasswd_p.obj ccPasswd_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del ccPasswdps.dll
	@del ccPasswdps.lib
	@del ccPasswdps.exp
	@del dlldata.obj
	@del ccPasswd_p.obj
	@del ccPasswd_i.obj
