# Microsoft Developer Studio Project File - Name="Avenge" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Avenge - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Avenge.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avenge.mak" CFG="Avenge - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avenge - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Avenge - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Avenge"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Avenge - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /Zd /O2 /I "inc.avcore" /I "inc.core" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "SYM_WIN" /D "SYM_WIN32" /D "QCUST" /D "_WINDOWS" /D Q_VER=\"P\" /D "_X86" /D "_M_X86" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Avenge - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /Zi /Od /I "inc.avcore" /I "inc.core" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "SYM_WIN" /D "SYM_WIN32" /D "SYM_DEBUG" /D "QCUST" /D "_WINDOWS" /D Q_VER=\"P\" /D "_X86" /D "_M_X86" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Avenge - Win32 Release"
# Name "Avenge - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\avenge\ALGLOAD.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\ALGSCAN.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\AVCACHE.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\AVENGE.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\BOOLOAD.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\BOOTREP.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\CRCLOAD.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\CRCSCAN.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\DELENTRY.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\ENGCOPYF.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\FILEREP.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\FSTART.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\GETBULK.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\GETINFO.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\INFC.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\INFOAUX.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\MEMLOAD.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\MEMSCAN.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\NAMEC.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\REPARAUX.CPP
# End Source File
# Begin Source File

SOURCE=.\avenge\VDATFILE.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc.avcore\ACTIONS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APCFG.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APCOMM.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APNTKDrv.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APQUAR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APSCLI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APSCOMM.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\apw2kddk.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\ASFCACHE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVAPI_R1.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVCB.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVDATFIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVENDIAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVENGE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\avexcl.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\avexcll.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVOBJECT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVTYPES.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CALCTABS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CALLBACK.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CALLBK15.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\COPYFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\COVER.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CPU.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CTASK.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CTS4.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CTSN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\DATAFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\DBPROTO.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\DefAnnuity.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\DLLRCSTA.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\ENUMFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\EXCLUDE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\EXE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\FILEMON.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\FNAME.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\FPROT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\HASHDB.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\IDENT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\INOCAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\IPFILTER.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\IQUARAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\iscandeliver.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LASTSCAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LINKCBK.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LINKCLI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LINKSRV.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LINKVAL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LOG.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LOGIO.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LUISHTML.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LUISSERV.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LUISSTR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LUISTEXT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\N30TYPE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\napiver.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVAP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVAPEL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVBIN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVBNHDR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVBTMAP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVCB.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVDEFS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVDOABT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVEX15.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NavLuUtils.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVNLMEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\navntutl.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVOPTS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVOR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVRPC.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVSDK.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVSDKR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVWHLP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVWHLP5.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NETBRWSD.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NETTREE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NPS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NPS_HLP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NPSAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NPSSVC.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NRESCONT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NSTDHELP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\OPCODE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\OPTIONS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\OSSOCKET.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PA.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PAGE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PAMAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PAMCACHE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PREPAIR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PROFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PROTO.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\QCONRES.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\QUAR32.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\QUARADD.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\QUARSET.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\READSIG.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\REPAIR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\REPAIRN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\ResqLoc.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\RPCPROT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\s32rasu.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\scandres.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SCANINST.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SCANMEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdfilter.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdpack.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdrcinit.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdsendme.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdsocket.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdstrip.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SEARCH.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\secdesc.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SIGEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SMTPMAIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SUPPORT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMALOG.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMINTEG.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMSERVE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMSYNC.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMTLA.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\UNIUTIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VIRINFDE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VIRLIST.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VIRSCAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VIRUSINF.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VLIBVER.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\WNDSIZE.H
# End Source File
# End Group
# End Target
# End Project
