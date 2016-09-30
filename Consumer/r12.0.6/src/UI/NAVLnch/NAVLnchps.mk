
NAVLnchps.dll: dlldata.obj NAVLnch_p.obj NAVLnch_i.obj
	link /dll /out:NAVLnchps.dll /def:NAVLnchps.def /entry:DllMain dlldata.obj NAVLnch_p.obj NAVLnch_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del NAVLnchps.dll
	@del NAVLnchps.lib
	@del NAVLnchps.exp
	@del dlldata.obj
	@del NAVLnch_p.obj
	@del NAVLnch_i.obj
