# Microsoft Developer Studio Project File - Name="OublietteDump" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=OublietteDump - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "OublietteDump.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "OublietteDump.mak" CFG="OublietteDump - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "OublietteDump - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "OublietteDump - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OublietteDump - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Binary\Debug"
# PROP BASE Intermediate_Dir "Binary\Debug\Temp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Binary\Debug"
# PROP Intermediate_Dir "Binary\Debug\Temp"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /GZ /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cryptlibd.lib /nologo /subsystem:console /debug /machine:IX86 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cryptlibd.lib /nologo /subsystem:console /debug /machine:IX86 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OublietteDump - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Binary\Release"
# PROP BASE Intermediate_Dir "Binary\Release\Temp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Binary\Release"
# PROP Intermediate_Dir "Binary\Release\Temp"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MT /W4 /GX /Zi /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /c
# ADD CPP /nologo /MT /W3 /GX /Zi /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cryptlib.lib /nologo /subsystem:console /debug /machine:IX86 /pdbtype:sept /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cryptlib.lib /nologo /subsystem:console /debug /machine:IX86 /pdbtype:sept /opt:ref /opt:icf

!ENDIF

# Begin Target

# Name "OublietteDump - Win32 Debug"
# Name "OublietteDump - Win32 Release"
# Begin Group "Delphi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Delphi\DateTime.h
# End Source File
# Begin Source File

SOURCE=.\Delphi\ShortString.h
# End Source File
# End Group
# Begin Group "Oubliette"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Oubliette\DecryptMethod.h
# End Source File
# Begin Source File

SOURCE=.\Oubliette\OublietteFile.cpp
DEP_CPP_OUBLI=\
	"..\..\StdAfx.h"\
	".\Delphi\DateTime.h"\
	".\Delphi\ShortString.h"\
	".\Oubliette\DecryptMethod.h"\
	".\Oubliette\OublietteFile.h"\
	".\Oubliette\OublietteHeader.h"\
	".\Oubliette\OubliettePlainHeader.h"\
	{$(INCLUDE)}"algparam.h"\
	{$(INCLUDE)}"argnames.h"\
	{$(INCLUDE)}"blowfish.h"\
	{$(INCLUDE)}"config.h"\
	{$(INCLUDE)}"cryptlib.h"\
	{$(INCLUDE)}"idea.h"\
	{$(INCLUDE)}"iterhash.h"\
	{$(INCLUDE)}"misc.h"\
	{$(INCLUDE)}"modes.h"\
	{$(INCLUDE)}"secblock.h"\
	{$(INCLUDE)}"seckey.h"\
	{$(INCLUDE)}"sha.h"\
	{$(INCLUDE)}"simple.h"\
	{$(INCLUDE)}"smartptr.h"\
	{$(INCLUDE)}"stdcpp.h"\
	{$(INCLUDE)}"strciphr.h"\

# End Source File
# Begin Source File

SOURCE=.\Oubliette\OublietteFile.h
# End Source File
# Begin Source File

SOURCE=.\Oubliette\OublietteHeader.h
# End Source File
# Begin Source File

SOURCE=.\Oubliette\OubliettePlainHeader.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\main.cpp
DEP_CPP_MAIN_=\
	".\Delphi\DateTime.h"\
	".\Delphi\ShortString.h"\
	".\Oubliette\OublietteFile.h"\
	".\Oubliette\OublietteHeader.h"\
	".\Oubliette\OubliettePlainHeader.h"\

# End Source File
# End Target
# End Project
