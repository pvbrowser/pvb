# Microsoft Developer Studio Generated NMAKE File, Based on Cell.dsp
!IF "$(CFG)" == ""
CFG=Cell - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Cell - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Cell - Win32 Release" && "$(CFG)" != "Cell - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cell.mak" CFG="Cell - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cell - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Cell - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cell - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Cell.dll"


CLEAN :
	-@erase "$(INTDIR)\aliascheck.obj"
	-@erase "$(INTDIR)\attach.obj"
	-@erase "$(INTDIR)\cell_perror.obj"
	-@erase "$(INTDIR)\cpf.obj"
	-@erase "$(INTDIR)\cpf_data.obj"
	-@erase "$(INTDIR)\defs.obj"
	-@erase "$(INTDIR)\establish_connection.obj"
	-@erase "$(INTDIR)\fill_header.obj"
	-@erase "$(INTDIR)\get_backplane_data.obj"
	-@erase "$(INTDIR)\get_device_data.obj"
	-@erase "$(INTDIR)\get_object_config.obj"
	-@erase "$(INTDIR)\get_object_config_list.obj"
	-@erase "$(INTDIR)\get_object_details.obj"
	-@erase "$(INTDIR)\get_object_details_list.obj"
	-@erase "$(INTDIR)\get_port_data.obj"
	-@erase "$(INTDIR)\get_program_details.obj"
	-@erase "$(INTDIR)\get_program_list.obj"
	-@erase "$(INTDIR)\get_struct_config.obj"
	-@erase "$(INTDIR)\get_struct_details.obj"
	-@erase "$(INTDIR)\get_struct_list.obj"
	-@erase "$(INTDIR)\getsize.obj"
	-@erase "$(INTDIR)\ioi.obj"
	-@erase "$(INTDIR)\list_interfaces.obj"
	-@erase "$(INTDIR)\list_ports.obj"
	-@erase "$(INTDIR)\list_services.obj"
	-@erase "$(INTDIR)\makepath.obj"
	-@erase "$(INTDIR)\nop.obj"
	-@erase "$(INTDIR)\read_multi_object_value.obj"
	-@erase "$(INTDIR)\read_object_value.obj"
	-@erase "$(INTDIR)\readdata.obj"
	-@erase "$(INTDIR)\register_session.obj"
	-@erase "$(INTDIR)\senddata.obj"
	-@erase "$(INTDIR)\sendRRdata.obj"
	-@erase "$(INTDIR)\settimeout.obj"
	-@erase "$(INTDIR)\unconnected_send.obj"
	-@erase "$(INTDIR)\unregister_session.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\who.obj"
	-@erase "$(INTDIR)\write_object_value.obj"
	-@erase "$(OUTDIR)\Cell.dll"
	-@erase "$(OUTDIR)\Cell.exp"
	-@erase "$(OUTDIR)\Cell.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CELL_EXPORTS" /Fp"$(INTDIR)\Cell.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Cell.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\Cell.pdb" /machine:I386 /out:"$(OUTDIR)\Cell.dll" /implib:"$(OUTDIR)\Cell.lib" 
LINK32_OBJS= \
	"$(INTDIR)\aliascheck.obj" \
	"$(INTDIR)\attach.obj" \
	"$(INTDIR)\cell_perror.obj" \
	"$(INTDIR)\cpf.obj" \
	"$(INTDIR)\cpf_data.obj" \
	"$(INTDIR)\defs.obj" \
	"$(INTDIR)\establish_connection.obj" \
	"$(INTDIR)\fill_header.obj" \
	"$(INTDIR)\get_backplane_data.obj" \
	"$(INTDIR)\get_device_data.obj" \
	"$(INTDIR)\get_object_config.obj" \
	"$(INTDIR)\get_object_config_list.obj" \
	"$(INTDIR)\get_object_details.obj" \
	"$(INTDIR)\get_object_details_list.obj" \
	"$(INTDIR)\get_port_data.obj" \
	"$(INTDIR)\get_program_details.obj" \
	"$(INTDIR)\get_program_list.obj" \
	"$(INTDIR)\get_struct_config.obj" \
	"$(INTDIR)\get_struct_details.obj" \
	"$(INTDIR)\get_struct_list.obj" \
	"$(INTDIR)\getsize.obj" \
	"$(INTDIR)\ioi.obj" \
	"$(INTDIR)\list_interfaces.obj" \
	"$(INTDIR)\list_ports.obj" \
	"$(INTDIR)\list_services.obj" \
	"$(INTDIR)\makepath.obj" \
	"$(INTDIR)\nop.obj" \
	"$(INTDIR)\read_multi_object_value.obj" \
	"$(INTDIR)\read_object_value.obj" \
	"$(INTDIR)\readdata.obj" \
	"$(INTDIR)\register_session.obj" \
	"$(INTDIR)\senddata.obj" \
	"$(INTDIR)\sendRRdata.obj" \
	"$(INTDIR)\settimeout.obj" \
	"$(INTDIR)\unconnected_send.obj" \
	"$(INTDIR)\unregister_session.obj" \
	"$(INTDIR)\who.obj" \
	"$(INTDIR)\write_object_value.obj"

"$(OUTDIR)\Cell.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Cell - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Cell.dll"


CLEAN :
	-@erase "$(INTDIR)\aliascheck.obj"
	-@erase "$(INTDIR)\attach.obj"
	-@erase "$(INTDIR)\cell_perror.obj"
	-@erase "$(INTDIR)\cpf.obj"
	-@erase "$(INTDIR)\cpf_data.obj"
	-@erase "$(INTDIR)\defs.obj"
	-@erase "$(INTDIR)\establish_connection.obj"
	-@erase "$(INTDIR)\fill_header.obj"
	-@erase "$(INTDIR)\get_backplane_data.obj"
	-@erase "$(INTDIR)\get_device_data.obj"
	-@erase "$(INTDIR)\get_object_config.obj"
	-@erase "$(INTDIR)\get_object_config_list.obj"
	-@erase "$(INTDIR)\get_object_details.obj"
	-@erase "$(INTDIR)\get_object_details_list.obj"
	-@erase "$(INTDIR)\get_port_data.obj"
	-@erase "$(INTDIR)\get_program_details.obj"
	-@erase "$(INTDIR)\get_program_list.obj"
	-@erase "$(INTDIR)\get_struct_config.obj"
	-@erase "$(INTDIR)\get_struct_details.obj"
	-@erase "$(INTDIR)\get_struct_list.obj"
	-@erase "$(INTDIR)\getsize.obj"
	-@erase "$(INTDIR)\ioi.obj"
	-@erase "$(INTDIR)\list_interfaces.obj"
	-@erase "$(INTDIR)\list_ports.obj"
	-@erase "$(INTDIR)\list_services.obj"
	-@erase "$(INTDIR)\makepath.obj"
	-@erase "$(INTDIR)\nop.obj"
	-@erase "$(INTDIR)\read_multi_object_value.obj"
	-@erase "$(INTDIR)\read_object_value.obj"
	-@erase "$(INTDIR)\readdata.obj"
	-@erase "$(INTDIR)\register_session.obj"
	-@erase "$(INTDIR)\senddata.obj"
	-@erase "$(INTDIR)\sendRRdata.obj"
	-@erase "$(INTDIR)\settimeout.obj"
	-@erase "$(INTDIR)\unconnected_send.obj"
	-@erase "$(INTDIR)\unregister_session.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\who.obj"
	-@erase "$(INTDIR)\write_object_value.obj"
	-@erase "$(OUTDIR)\Cell.dll"
	-@erase "$(OUTDIR)\Cell.exp"
	-@erase "$(OUTDIR)\Cell.ilk"
	-@erase "$(OUTDIR)\Cell.lib"
	-@erase "$(OUTDIR)\Cell.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CELL_EXPORTS" /Fp"$(INTDIR)\Cell.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Cell.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ws2_32.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Cell.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Cell.dll" /implib:"$(OUTDIR)\Cell.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\aliascheck.obj" \
	"$(INTDIR)\attach.obj" \
	"$(INTDIR)\cell_perror.obj" \
	"$(INTDIR)\cpf.obj" \
	"$(INTDIR)\cpf_data.obj" \
	"$(INTDIR)\defs.obj" \
	"$(INTDIR)\establish_connection.obj" \
	"$(INTDIR)\fill_header.obj" \
	"$(INTDIR)\get_backplane_data.obj" \
	"$(INTDIR)\get_device_data.obj" \
	"$(INTDIR)\get_object_config.obj" \
	"$(INTDIR)\get_object_config_list.obj" \
	"$(INTDIR)\get_object_details.obj" \
	"$(INTDIR)\get_object_details_list.obj" \
	"$(INTDIR)\get_port_data.obj" \
	"$(INTDIR)\get_program_details.obj" \
	"$(INTDIR)\get_program_list.obj" \
	"$(INTDIR)\get_struct_config.obj" \
	"$(INTDIR)\get_struct_details.obj" \
	"$(INTDIR)\get_struct_list.obj" \
	"$(INTDIR)\getsize.obj" \
	"$(INTDIR)\ioi.obj" \
	"$(INTDIR)\list_interfaces.obj" \
	"$(INTDIR)\list_ports.obj" \
	"$(INTDIR)\list_services.obj" \
	"$(INTDIR)\makepath.obj" \
	"$(INTDIR)\nop.obj" \
	"$(INTDIR)\read_multi_object_value.obj" \
	"$(INTDIR)\read_object_value.obj" \
	"$(INTDIR)\readdata.obj" \
	"$(INTDIR)\register_session.obj" \
	"$(INTDIR)\senddata.obj" \
	"$(INTDIR)\sendRRdata.obj" \
	"$(INTDIR)\settimeout.obj" \
	"$(INTDIR)\unconnected_send.obj" \
	"$(INTDIR)\unregister_session.obj" \
	"$(INTDIR)\who.obj" \
	"$(INTDIR)\write_object_value.obj"

"$(OUTDIR)\Cell.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Cell.dep")
!INCLUDE "Cell.dep"
!ELSE 
!MESSAGE Warning: cannot find "Cell.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Cell - Win32 Release" || "$(CFG)" == "Cell - Win32 Debug"
SOURCE=.\lib\aliascheck.c

"$(INTDIR)\aliascheck.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\attach.c

"$(INTDIR)\attach.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\cell_perror.c

"$(INTDIR)\cell_perror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\cpf.c

"$(INTDIR)\cpf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\cpf_data.c

"$(INTDIR)\cpf_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\defs.c

"$(INTDIR)\defs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\establish_connection.c

"$(INTDIR)\establish_connection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\fill_header.c

"$(INTDIR)\fill_header.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_backplane_data.c

"$(INTDIR)\get_backplane_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_device_data.c

"$(INTDIR)\get_device_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_object_config.c

"$(INTDIR)\get_object_config.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_object_config_list.c

"$(INTDIR)\get_object_config_list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_object_details.c

"$(INTDIR)\get_object_details.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_object_details_list.c

"$(INTDIR)\get_object_details_list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_port_data.c

"$(INTDIR)\get_port_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_program_details.c

"$(INTDIR)\get_program_details.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_program_list.c

"$(INTDIR)\get_program_list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_struct_config.c

"$(INTDIR)\get_struct_config.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_struct_details.c

"$(INTDIR)\get_struct_details.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\get_struct_list.c

"$(INTDIR)\get_struct_list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\getsize.c

"$(INTDIR)\getsize.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\ioi.c

"$(INTDIR)\ioi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\list_interfaces.c

"$(INTDIR)\list_interfaces.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\list_ports.c

"$(INTDIR)\list_ports.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\list_services.c

"$(INTDIR)\list_services.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\makepath.c

"$(INTDIR)\makepath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\nop.c

"$(INTDIR)\nop.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\read_multi_object_value.c

"$(INTDIR)\read_multi_object_value.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\read_object_value.c

"$(INTDIR)\read_object_value.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\readdata.c

"$(INTDIR)\readdata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\register_session.c

"$(INTDIR)\register_session.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\senddata.c

"$(INTDIR)\senddata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\sendRRdata.c

"$(INTDIR)\sendRRdata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\settimeout.c

"$(INTDIR)\settimeout.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\unconnected_send.c

"$(INTDIR)\unconnected_send.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\unregister_session.c

"$(INTDIR)\unregister_session.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\who.c

"$(INTDIR)\who.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lib\write_object_value.c

"$(INTDIR)\write_object_value.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

