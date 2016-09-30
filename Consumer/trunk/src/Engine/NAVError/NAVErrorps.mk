
NAVErrorps.dll: dlldata.obj NAVError_p.obj NAVError_i.obj
	link /dll /out:NAVErrorps.dll /def:NAVErrorps.def /entry:DllMain dlldata.obj NAVError_p.obj NAVError_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del NAVErrorps.dll
	@del NAVErrorps.lib
	@del NAVErrorps.exp
	@del dlldata.obj
	@del NAVError_p.obj
	@del NAVError_i.obj
