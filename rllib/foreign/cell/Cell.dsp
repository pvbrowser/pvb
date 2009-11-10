# Microsoft Developer Studio Project File - Name="Cell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Cell - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cell.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cell - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CELL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CELL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "Cell - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CELL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CELL_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ws2_32.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Cell - Win32 Release"
# Name "Cell - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\lib\aliascheck.c
# End Source File
# Begin Source File

SOURCE=.\lib\attach.c
# End Source File
# Begin Source File

SOURCE=.\lib\cell_perror.c
# End Source File
# Begin Source File

SOURCE=.\lib\cpf.c
# End Source File
# Begin Source File

SOURCE=.\lib\cpf_data.c
# End Source File
# Begin Source File

SOURCE=.\lib\defs.c
# End Source File
# Begin Source File

SOURCE=.\lib\establish_connection.c
# End Source File
# Begin Source File

SOURCE=.\lib\fill_header.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_backplane_data.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_device_data.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_object_config.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_object_config_list.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_object_details.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_object_details_list.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_port_data.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_program_details.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_program_list.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_struct_config.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_struct_details.c
# End Source File
# Begin Source File

SOURCE=.\lib\get_struct_list.c
# End Source File
# Begin Source File

SOURCE=.\lib\getsize.c
# End Source File
# Begin Source File

SOURCE=.\lib\ioi.c
# End Source File
# Begin Source File

SOURCE=.\lib\list_interfaces.c
# End Source File
# Begin Source File

SOURCE=.\lib\list_ports.c
# End Source File
# Begin Source File

SOURCE=.\lib\list_services.c
# End Source File
# Begin Source File

SOURCE=.\lib\makepath.c
# End Source File
# Begin Source File

SOURCE=.\lib\nop.c
# End Source File
# Begin Source File

SOURCE=.\lib\read_multi_object_value.c
# End Source File
# Begin Source File

SOURCE=.\lib\read_object_value.c
# End Source File
# Begin Source File

SOURCE=.\lib\readdata.c
# End Source File
# Begin Source File

SOURCE=.\lib\register_session.c
# End Source File
# Begin Source File

SOURCE=.\lib\senddata.c
# End Source File
# Begin Source File

SOURCE=.\lib\sendRRdata.c
# End Source File
# Begin Source File

SOURCE=.\lib\settimeout.c
# End Source File
# Begin Source File

SOURCE=.\lib\unconnected_send.c
# End Source File
# Begin Source File

SOURCE=.\lib\unregister_session.c
# End Source File
# Begin Source File

SOURCE=.\lib\who.c
# End Source File
# Begin Source File

SOURCE=.\lib\write_object_value.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\lib\libcell.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
