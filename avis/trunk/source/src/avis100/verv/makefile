# Microsoft Developer Studio Generated NMAKE File, Based on verv.dsp
!IF "$(CFG)" == ""
CFG=verv - Win32 Release
!MESSAGE No configuration specified. Defaulting to verv - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "verv - Win32 Release" && "$(CFG)" != "verv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "verv.mak" CFG="verv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "verv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "verv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "verv - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\verv.exe"

!ELSE 

ALL : "$(OUTDIR)\verv.exe"

!ENDIF 

INSTALL: ALL
	echo "not implemented yet"

TEST:	ALL
	echo "not implemented yet"

CLEAN :
	-@erase "$(INTDIR)\allocsafe.obj"
	-@erase "$(INTDIR)\autoverv.obj"
	-@erase "$(INTDIR)\avfileio.obj"
	-@erase "$(INTDIR)\boundary.obj"
	-@erase "$(INTDIR)\compute_offset.obj"
	-@erase "$(INTDIR)\crc_setup.obj"
	-@erase "$(INTDIR)\create_invariant.obj"
	-@erase "$(INTDIR)\decrypt.obj"
	-@erase "$(INTDIR)\derive_correction.obj"
	-@erase "$(INTDIR)\eliminate_duplicate_keys.obj"
	-@erase "$(INTDIR)\entry.obj"
	-@erase "$(INTDIR)\equivalent_keys.obj"
	-@erase "$(INTDIR)\fileiterator.obj"
	-@erase "$(INTDIR)\filesize.obj"
	-@erase "$(INTDIR)\find_best_anchor.obj"
	-@erase "$(INTDIR)\find_match_len.obj"
	-@erase "$(INTDIR)\free_match.obj"
	-@erase "$(INTDIR)\free_pos.obj"
	-@erase "$(INTDIR)\get_file_type.obj"
	-@erase "$(INTDIR)\greedy_cover.obj"
	-@erase "$(INTDIR)\index_bigrams.obj"
	-@erase "$(INTDIR)\lcs.obj"
	-@erase "$(INTDIR)\make_key_list.obj"
	-@erase "$(INTDIR)\make_map_list.obj"
	-@erase "$(INTDIR)\make_recon_arrays.obj"
	-@erase "$(INTDIR)\make_reconstruction_list.obj"
	-@erase "$(INTDIR)\make_universal_goat.obj"
	-@erase "$(INTDIR)\mem_err.obj"
	-@erase "$(INTDIR)\patch_map_list.obj"
	-@erase "$(INTDIR)\polymath.obj"
	-@erase "$(INTDIR)\reconstruct.obj"
	-@erase "$(INTDIR)\reconstruct_status.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\write_bytes.obj"
	-@erase "$(INTDIR)\write_bytes2.obj"
	-@erase "$(INTDIR)\write_graphic.obj"
	-@erase "$(INTDIR)\write_vds.obj"
	-@erase "$(INTDIR)\write_vp.obj"
	-@erase "$(OUTDIR)\verv.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "_MBCS" /D "NOASM" /D "VERV_CODO" /D "USE_STRUCTURE" /Fp"$(INTDIR)\verv.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\verv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\verv.pdb" /machine:I386 /out:"$(OUTDIR)\verv.exe" 
LINK32_OBJS= \
	"$(INTDIR)\allocsafe.obj" \
	"$(INTDIR)\autoverv.obj" \
	"$(INTDIR)\avfileio.obj" \
	"$(INTDIR)\boundary.obj" \
	"$(INTDIR)\compute_offset.obj" \
	"$(INTDIR)\crc_setup.obj" \
	"$(INTDIR)\create_invariant.obj" \
	"$(INTDIR)\decrypt.obj" \
	"$(INTDIR)\derive_correction.obj" \
	"$(INTDIR)\eliminate_duplicate_keys.obj" \
	"$(INTDIR)\entry.obj" \
	"$(INTDIR)\equivalent_keys.obj" \
	"$(INTDIR)\fileiterator.obj" \
	"$(INTDIR)\filesize.obj" \
	"$(INTDIR)\find_best_anchor.obj" \
	"$(INTDIR)\find_match_len.obj" \
	"$(INTDIR)\free_match.obj" \
	"$(INTDIR)\free_pos.obj" \
	"$(INTDIR)\get_file_type.obj" \
	"$(INTDIR)\greedy_cover.obj" \
	"$(INTDIR)\index_bigrams.obj" \
	"$(INTDIR)\lcs.obj" \
	"$(INTDIR)\make_key_list.obj" \
	"$(INTDIR)\make_map_list.obj" \
	"$(INTDIR)\make_recon_arrays.obj" \
	"$(INTDIR)\make_reconstruction_list.obj" \
	"$(INTDIR)\make_universal_goat.obj" \
	"$(INTDIR)\mem_err.obj" \
	"$(INTDIR)\patch_map_list.obj" \
	"$(INTDIR)\polymath.obj" \
	"$(INTDIR)\reconstruct.obj" \
	"$(INTDIR)\reconstruct_status.obj" \
	"$(INTDIR)\write_bytes.obj" \
	"$(INTDIR)\write_bytes2.obj" \
	"$(INTDIR)\write_graphic.obj" \
	"$(INTDIR)\write_vds.obj" \
	"$(INTDIR)\write_vp.obj"

"$(OUTDIR)\verv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\Release\verv.exe
SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\verv.exe"
   xcopy .\Release\verv.exe \\altona\progs\Stage\bin
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\verv.exe" "$(OUTDIR)\verv.bsc"

!ELSE 

ALL : "$(OUTDIR)\verv.exe" "$(OUTDIR)\verv.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\allocsafe.obj"
	-@erase "$(INTDIR)\allocsafe.sbr"
	-@erase "$(INTDIR)\autoverv.obj"
	-@erase "$(INTDIR)\autoverv.sbr"
	-@erase "$(INTDIR)\avfileio.obj"
	-@erase "$(INTDIR)\avfileio.sbr"
	-@erase "$(INTDIR)\boundary.obj"
	-@erase "$(INTDIR)\boundary.sbr"
	-@erase "$(INTDIR)\compute_offset.obj"
	-@erase "$(INTDIR)\compute_offset.sbr"
	-@erase "$(INTDIR)\crc_setup.obj"
	-@erase "$(INTDIR)\crc_setup.sbr"
	-@erase "$(INTDIR)\create_invariant.obj"
	-@erase "$(INTDIR)\create_invariant.sbr"
	-@erase "$(INTDIR)\decrypt.obj"
	-@erase "$(INTDIR)\decrypt.sbr"
	-@erase "$(INTDIR)\derive_correction.obj"
	-@erase "$(INTDIR)\derive_correction.sbr"
	-@erase "$(INTDIR)\eliminate_duplicate_keys.obj"
	-@erase "$(INTDIR)\eliminate_duplicate_keys.sbr"
	-@erase "$(INTDIR)\entry.obj"
	-@erase "$(INTDIR)\entry.sbr"
	-@erase "$(INTDIR)\equivalent_keys.obj"
	-@erase "$(INTDIR)\equivalent_keys.sbr"
	-@erase "$(INTDIR)\fileiterator.obj"
	-@erase "$(INTDIR)\fileiterator.sbr"
	-@erase "$(INTDIR)\filesize.obj"
	-@erase "$(INTDIR)\filesize.sbr"
	-@erase "$(INTDIR)\find_best_anchor.obj"
	-@erase "$(INTDIR)\find_best_anchor.sbr"
	-@erase "$(INTDIR)\find_match_len.obj"
	-@erase "$(INTDIR)\find_match_len.sbr"
	-@erase "$(INTDIR)\free_match.obj"
	-@erase "$(INTDIR)\free_match.sbr"
	-@erase "$(INTDIR)\free_pos.obj"
	-@erase "$(INTDIR)\free_pos.sbr"
	-@erase "$(INTDIR)\get_file_type.obj"
	-@erase "$(INTDIR)\get_file_type.sbr"
	-@erase "$(INTDIR)\greedy_cover.obj"
	-@erase "$(INTDIR)\greedy_cover.sbr"
	-@erase "$(INTDIR)\index_bigrams.obj"
	-@erase "$(INTDIR)\index_bigrams.sbr"
	-@erase "$(INTDIR)\lcs.obj"
	-@erase "$(INTDIR)\lcs.sbr"
	-@erase "$(INTDIR)\make_key_list.obj"
	-@erase "$(INTDIR)\make_key_list.sbr"
	-@erase "$(INTDIR)\make_map_list.obj"
	-@erase "$(INTDIR)\make_map_list.sbr"
	-@erase "$(INTDIR)\make_recon_arrays.obj"
	-@erase "$(INTDIR)\make_recon_arrays.sbr"
	-@erase "$(INTDIR)\make_reconstruction_list.obj"
	-@erase "$(INTDIR)\make_reconstruction_list.sbr"
	-@erase "$(INTDIR)\make_universal_goat.obj"
	-@erase "$(INTDIR)\make_universal_goat.sbr"
	-@erase "$(INTDIR)\mem_err.obj"
	-@erase "$(INTDIR)\mem_err.sbr"
	-@erase "$(INTDIR)\patch_map_list.obj"
	-@erase "$(INTDIR)\patch_map_list.sbr"
	-@erase "$(INTDIR)\polymath.obj"
	-@erase "$(INTDIR)\polymath.sbr"
	-@erase "$(INTDIR)\reconstruct.obj"
	-@erase "$(INTDIR)\reconstruct.sbr"
	-@erase "$(INTDIR)\reconstruct_status.obj"
	-@erase "$(INTDIR)\reconstruct_status.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\write_bytes.obj"
	-@erase "$(INTDIR)\write_bytes.sbr"
	-@erase "$(INTDIR)\write_bytes2.obj"
	-@erase "$(INTDIR)\write_bytes2.sbr"
	-@erase "$(INTDIR)\write_graphic.obj"
	-@erase "$(INTDIR)\write_graphic.sbr"
	-@erase "$(INTDIR)\write_vds.obj"
	-@erase "$(INTDIR)\write_vds.sbr"
	-@erase "$(INTDIR)\write_vp.obj"
	-@erase "$(INTDIR)\write_vp.sbr"
	-@erase "$(OUTDIR)\verv.bsc"
	-@erase "$(OUTDIR)\verv.exe"
	-@erase "$(OUTDIR)\verv.ilk"
	-@erase "$(OUTDIR)\verv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE"\
 /D "_MBCS" /D "NOASM" /D "VERV_CODO" /D "USE_STRUCTURE" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\verv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\verv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\allocsafe.sbr" \
	"$(INTDIR)\autoverv.sbr" \
	"$(INTDIR)\avfileio.sbr" \
	"$(INTDIR)\boundary.sbr" \
	"$(INTDIR)\compute_offset.sbr" \
	"$(INTDIR)\crc_setup.sbr" \
	"$(INTDIR)\create_invariant.sbr" \
	"$(INTDIR)\decrypt.sbr" \
	"$(INTDIR)\derive_correction.sbr" \
	"$(INTDIR)\eliminate_duplicate_keys.sbr" \
	"$(INTDIR)\entry.sbr" \
	"$(INTDIR)\equivalent_keys.sbr" \
	"$(INTDIR)\fileiterator.sbr" \
	"$(INTDIR)\filesize.sbr" \
	"$(INTDIR)\find_best_anchor.sbr" \
	"$(INTDIR)\find_match_len.sbr" \
	"$(INTDIR)\free_match.sbr" \
	"$(INTDIR)\free_pos.sbr" \
	"$(INTDIR)\get_file_type.sbr" \
	"$(INTDIR)\greedy_cover.sbr" \
	"$(INTDIR)\index_bigrams.sbr" \
	"$(INTDIR)\lcs.sbr" \
	"$(INTDIR)\make_key_list.sbr" \
	"$(INTDIR)\make_map_list.sbr" \
	"$(INTDIR)\make_recon_arrays.sbr" \
	"$(INTDIR)\make_reconstruction_list.sbr" \
	"$(INTDIR)\make_universal_goat.sbr" \
	"$(INTDIR)\mem_err.sbr" \
	"$(INTDIR)\patch_map_list.sbr" \
	"$(INTDIR)\polymath.sbr" \
	"$(INTDIR)\reconstruct.sbr" \
	"$(INTDIR)\reconstruct_status.sbr" \
	"$(INTDIR)\write_bytes.sbr" \
	"$(INTDIR)\write_bytes2.sbr" \
	"$(INTDIR)\write_graphic.sbr" \
	"$(INTDIR)\write_vds.sbr" \
	"$(INTDIR)\write_vp.sbr"

"$(OUTDIR)\verv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\verv.pdb" /debug /machine:I386 /out:"$(OUTDIR)\verv.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\allocsafe.obj" \
	"$(INTDIR)\autoverv.obj" \
	"$(INTDIR)\avfileio.obj" \
	"$(INTDIR)\boundary.obj" \
	"$(INTDIR)\compute_offset.obj" \
	"$(INTDIR)\crc_setup.obj" \
	"$(INTDIR)\create_invariant.obj" \
	"$(INTDIR)\decrypt.obj" \
	"$(INTDIR)\derive_correction.obj" \
	"$(INTDIR)\eliminate_duplicate_keys.obj" \
	"$(INTDIR)\entry.obj" \
	"$(INTDIR)\equivalent_keys.obj" \
	"$(INTDIR)\fileiterator.obj" \
	"$(INTDIR)\filesize.obj" \
	"$(INTDIR)\find_best_anchor.obj" \
	"$(INTDIR)\find_match_len.obj" \
	"$(INTDIR)\free_match.obj" \
	"$(INTDIR)\free_pos.obj" \
	"$(INTDIR)\get_file_type.obj" \
	"$(INTDIR)\greedy_cover.obj" \
	"$(INTDIR)\index_bigrams.obj" \
	"$(INTDIR)\lcs.obj" \
	"$(INTDIR)\make_key_list.obj" \
	"$(INTDIR)\make_map_list.obj" \
	"$(INTDIR)\make_recon_arrays.obj" \
	"$(INTDIR)\make_reconstruction_list.obj" \
	"$(INTDIR)\make_universal_goat.obj" \
	"$(INTDIR)\mem_err.obj" \
	"$(INTDIR)\patch_map_list.obj" \
	"$(INTDIR)\polymath.obj" \
	"$(INTDIR)\reconstruct.obj" \
	"$(INTDIR)\reconstruct_status.obj" \
	"$(INTDIR)\write_bytes.obj" \
	"$(INTDIR)\write_bytes2.obj" \
	"$(INTDIR)\write_graphic.obj" \
	"$(INTDIR)\write_vds.obj" \
	"$(INTDIR)\write_vp.obj"

"$(OUTDIR)\verv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "verv - Win32 Release" || "$(CFG)" == "verv - Win32 Debug"
SOURCE=.\src\allocsafe.cpp

!IF  "$(CFG)" == "verv - Win32 Release"


"$(INTDIR)\allocsafe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"


"$(INTDIR)\allocsafe.obj"	"$(INTDIR)\allocsafe.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\autoverv.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_AUTOV=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\fileiterator.hpp"\
	".\src\polymath.h"\
	

"$(INTDIR)\autoverv.obj" : $(SOURCE) $(DEP_CPP_AUTOV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_AUTOV=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\fileiterator.hpp"\
	".\src\polymath.h"\
	

"$(INTDIR)\autoverv.obj"	"$(INTDIR)\autoverv.sbr" : $(SOURCE) $(DEP_CPP_AUTOV)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\avfileio.cpp
DEP_CPP_AVFIL=\
	".\src\avfileio.h"\
	

!IF  "$(CFG)" == "verv - Win32 Release"


"$(INTDIR)\avfileio.obj" : $(SOURCE) $(DEP_CPP_AVFIL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"


"$(INTDIR)\avfileio.obj"	"$(INTDIR)\avfileio.sbr" : $(SOURCE) $(DEP_CPP_AVFIL)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\boundary.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_BOUND=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\boundary.obj" : $(SOURCE) $(DEP_CPP_BOUND) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_BOUND=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\boundary.obj"	"$(INTDIR)\boundary.sbr" : $(SOURCE) $(DEP_CPP_BOUND)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\compute_offset.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_COMPU=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\compute_offset.obj" : $(SOURCE) $(DEP_CPP_COMPU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_COMPU=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\compute_offset.obj"	"$(INTDIR)\compute_offset.sbr" : $(SOURCE)\
 $(DEP_CPP_COMPU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\crc_setup.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_CRC_S=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\crc_setup.obj" : $(SOURCE) $(DEP_CPP_CRC_S) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_CRC_S=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\crc_setup.obj"	"$(INTDIR)\crc_setup.sbr" : $(SOURCE)\
 $(DEP_CPP_CRC_S) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\create_invariant.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_CREAT=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\create_invariant.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_CREAT=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\create_invariant.obj"	"$(INTDIR)\create_invariant.sbr" : $(SOURCE)\
 $(DEP_CPP_CREAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\decrypt.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_DECRY=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\decrypt.obj" : $(SOURCE) $(DEP_CPP_DECRY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_DECRY=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\decrypt.obj"	"$(INTDIR)\decrypt.sbr" : $(SOURCE) $(DEP_CPP_DECRY)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\derive_correction.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_DERIV=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\derive_correction.obj" : $(SOURCE) $(DEP_CPP_DERIV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_DERIV=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\derive_correction.obj"	"$(INTDIR)\derive_correction.sbr" : $(SOURCE)\
 $(DEP_CPP_DERIV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\eliminate_duplicate_keys.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_ELIMI=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\eliminate_duplicate_keys.obj" : $(SOURCE) $(DEP_CPP_ELIMI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_ELIMI=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\eliminate_duplicate_keys.obj"\
	"$(INTDIR)\eliminate_duplicate_keys.sbr" : $(SOURCE) $(DEP_CPP_ELIMI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\entry.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_ENTRY=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\entry.obj" : $(SOURCE) $(DEP_CPP_ENTRY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_ENTRY=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\entry.obj"	"$(INTDIR)\entry.sbr" : $(SOURCE) $(DEP_CPP_ENTRY)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\equivalent_keys.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_EQUIV=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\equivalent_keys.obj" : $(SOURCE) $(DEP_CPP_EQUIV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_EQUIV=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\equivalent_keys.obj"	"$(INTDIR)\equivalent_keys.sbr" : $(SOURCE)\
 $(DEP_CPP_EQUIV) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\fileiterator.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_FILEI=\
	".\src\avfileio.h"\
	".\src\fileiterator.hpp"\
	

"$(INTDIR)\fileiterator.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_FILEI=\
	".\src\avfileio.h"\
	".\src\fileiterator.hpp"\
	

"$(INTDIR)\fileiterator.obj"	"$(INTDIR)\fileiterator.sbr" : $(SOURCE)\
 $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\filesize.cpp

!IF  "$(CFG)" == "verv - Win32 Release"


"$(INTDIR)\filesize.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"


"$(INTDIR)\filesize.obj"	"$(INTDIR)\filesize.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\find_best_anchor.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_FIND_=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\find_best_anchor.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_FIND_=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\find_best_anchor.obj"	"$(INTDIR)\find_best_anchor.sbr" : $(SOURCE)\
 $(DEP_CPP_FIND_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\find_match_len.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_FIND_M=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\find_match_len.obj" : $(SOURCE) $(DEP_CPP_FIND_M) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_FIND_M=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\find_match_len.obj"	"$(INTDIR)\find_match_len.sbr" : $(SOURCE)\
 $(DEP_CPP_FIND_M) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\free_match.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_FREE_=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\free_match.obj" : $(SOURCE) $(DEP_CPP_FREE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_FREE_=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\free_match.obj"	"$(INTDIR)\free_match.sbr" : $(SOURCE)\
 $(DEP_CPP_FREE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\free_pos.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_FREE_P=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\free_pos.obj" : $(SOURCE) $(DEP_CPP_FREE_P) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_FREE_P=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\free_pos.obj"	"$(INTDIR)\free_pos.sbr" : $(SOURCE) $(DEP_CPP_FREE_P)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\get_file_type.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_GET_F=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\get_file_type.obj" : $(SOURCE) $(DEP_CPP_GET_F) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_GET_F=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\get_file_type.obj"	"$(INTDIR)\get_file_type.sbr" : $(SOURCE)\
 $(DEP_CPP_GET_F) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\greedy_cover.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_GREED=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\greedy_cover.obj" : $(SOURCE) $(DEP_CPP_GREED) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_GREED=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\greedy_cover.obj"	"$(INTDIR)\greedy_cover.sbr" : $(SOURCE)\
 $(DEP_CPP_GREED) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\index_bigrams.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_INDEX=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\index_bigrams.obj" : $(SOURCE) $(DEP_CPP_INDEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_INDEX=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\index_bigrams.obj"	"$(INTDIR)\index_bigrams.sbr" : $(SOURCE)\
 $(DEP_CPP_INDEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lcs.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_LCS_C=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\lcs.obj" : $(SOURCE) $(DEP_CPP_LCS_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_LCS_C=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\lcs.obj"	"$(INTDIR)\lcs.sbr" : $(SOURCE) $(DEP_CPP_LCS_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\make_key_list.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_MAKE_=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_key_list.obj" : $(SOURCE) $(DEP_CPP_MAKE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_MAKE_=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_key_list.obj"	"$(INTDIR)\make_key_list.sbr" : $(SOURCE)\
 $(DEP_CPP_MAKE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\make_map_list.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_MAKE_M=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_map_list.obj" : $(SOURCE) $(DEP_CPP_MAKE_M) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_MAKE_M=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_map_list.obj"	"$(INTDIR)\make_map_list.sbr" : $(SOURCE)\
 $(DEP_CPP_MAKE_M) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\make_recon_arrays.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_MAKE_R=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_recon_arrays.obj" : $(SOURCE) $(DEP_CPP_MAKE_R) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_MAKE_R=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_recon_arrays.obj"	"$(INTDIR)\make_recon_arrays.sbr" : $(SOURCE)\
 $(DEP_CPP_MAKE_R) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\make_reconstruction_list.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_MAKE_RE=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_reconstruction_list.obj" : $(SOURCE) $(DEP_CPP_MAKE_RE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_MAKE_RE=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_reconstruction_list.obj"\
	"$(INTDIR)\make_reconstruction_list.sbr" : $(SOURCE) $(DEP_CPP_MAKE_RE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\make_universal_goat.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_MAKE_U=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_universal_goat.obj" : $(SOURCE) $(DEP_CPP_MAKE_U) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_MAKE_U=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\make_universal_goat.obj"	"$(INTDIR)\make_universal_goat.sbr" : \
$(SOURCE) $(DEP_CPP_MAKE_U) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\mem_err.cpp

!IF  "$(CFG)" == "verv - Win32 Release"


"$(INTDIR)\mem_err.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"


"$(INTDIR)\mem_err.obj"	"$(INTDIR)\mem_err.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\patch_map_list.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_PATCH=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\patch_map_list.obj" : $(SOURCE) $(DEP_CPP_PATCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_PATCH=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\patch_map_list.obj"	"$(INTDIR)\patch_map_list.sbr" : $(SOURCE)\
 $(DEP_CPP_PATCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\polymath.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_POLYM=\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\polymath.obj" : $(SOURCE) $(DEP_CPP_POLYM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_POLYM=\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\polymath.obj"	"$(INTDIR)\polymath.sbr" : $(SOURCE) $(DEP_CPP_POLYM)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\reconstruct.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_RECON=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\reconstruct.obj" : $(SOURCE) $(DEP_CPP_RECON) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_RECON=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\reconstruct.obj"	"$(INTDIR)\reconstruct.sbr" : $(SOURCE)\
 $(DEP_CPP_RECON) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\reconstruct_status.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_RECONS=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\reconstruct_status.obj" : $(SOURCE) $(DEP_CPP_RECONS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_RECONS=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\reconstruct_status.obj"	"$(INTDIR)\reconstruct_status.sbr" : \
$(SOURCE) $(DEP_CPP_RECONS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\write_bytes.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_WRITE=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_bytes.obj" : $(SOURCE) $(DEP_CPP_WRITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_WRITE=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_bytes.obj"	"$(INTDIR)\write_bytes.sbr" : $(SOURCE)\
 $(DEP_CPP_WRITE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\write_bytes2.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_WRITE_=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_bytes2.obj" : $(SOURCE) $(DEP_CPP_WRITE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_WRITE_=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_bytes2.obj"	"$(INTDIR)\write_bytes2.sbr" : $(SOURCE)\
 $(DEP_CPP_WRITE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\write_graphic.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_WRITE_G=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_graphic.obj" : $(SOURCE) $(DEP_CPP_WRITE_G) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_WRITE_G=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_graphic.obj"	"$(INTDIR)\write_graphic.sbr" : $(SOURCE)\
 $(DEP_CPP_WRITE_G) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\write_vds.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_WRITE_V=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_vds.obj" : $(SOURCE) $(DEP_CPP_WRITE_V) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_WRITE_V=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_vds.obj"	"$(INTDIR)\write_vds.sbr" : $(SOURCE)\
 $(DEP_CPP_WRITE_V) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\write_vp.cpp

!IF  "$(CFG)" == "verv - Win32 Release"

DEP_CPP_WRITE_VP=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_vp.obj" : $(SOURCE) $(DEP_CPP_WRITE_VP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

DEP_CPP_WRITE_VP=\
	".\src\allocsafe.h"\
	".\src\autoverv.h"\
	".\src\avfileio.h"\
	".\src\common.h"\
	".\src\polymath.h"\
	

"$(INTDIR)\write_vp.obj"	"$(INTDIR)\write_vp.sbr" : $(SOURCE)\
 $(DEP_CPP_WRITE_VP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

