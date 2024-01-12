# Microsoft Developer Studio Project File - Name="in_modplug" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=in_modplug - Win32 DebugNOCONFIG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "in_modplug.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "in_modplug.mak" CFG="in_modplug - Win32 DebugNOCONFIG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "in_modplug - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_modplug - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_modplug - Win32 ReleaseMFCGUI" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_modplug - Win32 DebugMFCGUI" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_modplug - Win32 ReleaseNOCONFIG" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_modplug - Win32 DebugNOCONFIG" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "in_modplug - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Obj_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D WINVER=0x0400 /D "MODPLUG_STATIC" /D "USEEXTRAFEATURES" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41b /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\\" /d "NDEBUG" /d _MSC_VER=1200 /d "USEEXTRAFEATURES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 libcmt.lib kernel32.lib user32.lib oldnames.lib /nologo /dll /machine:I386 /nodefaultlib /RELEASE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "in_modplug - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Obj_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D WINVER=0x0400 /D "MODPLUG_STATIC" /D "USEEXTRAFEATURES" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41b /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\\" /d "_DEBUG" /d _MSC_VER=1200 /d "USEEXTRAFEATURES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libcmtd.lib kernel32.lib user32.lib oldnames.lib /nologo /dll /debug /machine:I386 /nodefaultlib /pdbtype:sept

!ELSEIF  "$(CFG)" == "in_modplug - Win32 ReleaseMFCGUI"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "in_modplug___Win32_ReleaseMFCGUI"
# PROP BASE Intermediate_Dir "in_modplug___Win32_ReleaseMFCGUI"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMFCGUI"
# PROP Intermediate_Dir "Obj_ReleaseMFCGUI"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D "MODPLUG_STATIC" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D WINVER=0x0400 /D "MODPLUG_STATIC" /D "MFCGUI" /D "USEEXTRAFEATURES" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /D "_WINDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\\" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\\" /d "NDEBUG" /d _MSC_VER=1200 /d "MFCGUI" /d "USEEXTRAFEATURES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib libcmt.lib oldnames.lib /nologo /dll /machine:I386 /nodefaultlib
# ADD LINK32 /nologo /dll /machine:I386 /RELEASE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "in_modplug - Win32 DebugMFCGUI"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "in_modplug___Win32_DebugMFCGUI"
# PROP BASE Intermediate_Dir "in_modplug___Win32_DebugMFCGUI"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMFCGUI"
# PROP Intermediate_Dir "Obj_DebugMFCGUI"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D "MODPLUG_STATIC" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D WINVER=0x0400 /D "MODPLUG_STATIC" /D "MFCGUI" /D "USEEXTRAFEATURES" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /D "_WINDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\\" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\\" /d "_DEBUG" /d _MSC_VER=1200 /d "MFCGUI" /d "USEEXTRAFEATURES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib libcmtd.lib oldnames.lib /nologo /dll /debug /machine:I386 /nodefaultlib /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "in_modplug - Win32 ReleaseNOCONFIG"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "in_modplug___Win32_ReleaseNOCONFIG"
# PROP BASE Intermediate_Dir "in_modplug___Win32_ReleaseNOCONFIG"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseNOCONFIG"
# PROP Intermediate_Dir "Obj_ReleaseNOCONFIG"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D "MODPLUG_STATIC" /D "USEEXTRAFEATURES" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D WINVER=0x0400 /D "MODPLUG_STATIC" /D "NOGUICONFIG" /D "USEEXTRAFEATURES" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\\" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\\" /d "NDEBUG" /d _MSC_VER=1200 /d "NOGUICONFIG" /d "USEEXTRAFEATURES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmt.lib kernel32.lib user32.lib oldnames.lib /nologo /dll /machine:I386 /nodefaultlib
# ADD LINK32 libcmt.lib kernel32.lib user32.lib oldnames.lib /nologo /dll /machine:I386 /nodefaultlib /RELEASE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "in_modplug - Win32 DebugNOCONFIG"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "in_modplug___Win32_DebugNOCONFIG"
# PROP BASE Intermediate_Dir "in_modplug___Win32_DebugNOCONFIG"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugNOCONFIG"
# PROP Intermediate_Dir "Obj_DebugNOCONFIG"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D "MODPLUG_STATIC" /D "USEEXTRAFEATURES" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\..\.." /I ".\..\..\libmodplug_0_8_8_x\src" /I ".\..\..\libmodplug_0_8_8_x\src\libmodplug" /D WINVER=0x0400 /D "MODPLUG_STATIC" /D "NOGUICONFIG" /D "USEEXTRAFEATURES" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IN_MODPLUG_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\\" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\\" /d "_DEBUG" /d _MSC_VER=1200 /d "NOGUICONFIG" /d "USEEXTRAFEATURES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmtd.lib kernel32.lib user32.lib oldnames.lib /nologo /dll /debug /machine:I386 /nodefaultlib /pdbtype:sept
# ADD LINK32 libcmtd.lib kernel32.lib user32.lib oldnames.lib /nologo /dll /debug /machine:I386 /nodefaultlib /pdbtype:sept

!ENDIF 

# Begin Target

# Name "in_modplug - Win32 Release"
# Name "in_modplug - Win32 Debug"
# Name "in_modplug - Win32 ReleaseMFCGUI"
# Name "in_modplug - Win32 DebugMFCGUI"
# Name "in_modplug - Win32 ReleaseNOCONFIG"
# Name "in_modplug - Win32 DebugNOCONFIG"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\config_mfc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\config_native.cpp
# End Source File
# Begin Source File

SOURCE=..\..\main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\stdint.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\main.rc
# End Source File
# Begin Source File

SOURCE=..\..\resource.h

!IF  "$(CFG)" == "in_modplug - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\resource.h

"..\..\vc6ver.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\getvc6build.exe /of="vc6ver.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "in_modplug - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\resource.h

"..\..\vc6ver.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\getvc6build.exe /of="vc6ver.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "in_modplug - Win32 ReleaseMFCGUI"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\resource.h

"..\..\vc6ver.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\getvc6build.exe /of="vc6ver.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "in_modplug - Win32 DebugMFCGUI"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\resource.h

"..\..\vc6ver.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\getvc6build.exe /of="vc6ver.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "in_modplug - Win32 ReleaseNOCONFIG"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\resource.h

"..\..\vc6ver.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\getvc6build.exe /of="vc6ver.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "in_modplug - Win32 DebugNOCONFIG"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\resource.h

"..\..\vc6ver.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\getvc6build.exe /of="vc6ver.h"

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "WinAmp SDK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Winamp\DSP.H
# End Source File
# Begin Source File

SOURCE=..\..\Winamp\GEN.H
# End Source File
# Begin Source File

SOURCE=..\..\Winamp\IN2.H
# End Source File
# Begin Source File

SOURCE=..\..\Winamp\ipc_pe.h
# End Source File
# Begin Source File

SOURCE=..\..\Winamp\OUT.H
# End Source File
# Begin Source File

SOURCE=..\..\Winamp\wa_dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Winamp\wa_ipc.h
# End Source File
# End Group
# Begin Group "libmodplug_0_8_8_x"

# PROP Default_Filter ""
# Begin Group "src"

# PROP Default_Filter "cpp;h"
# Begin Group "libmodplug"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\libmodplug\it_defs.h
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\libmodplug\sndfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\libmodplug\stdafx.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\fastmix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_669.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_abc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_amf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_ams.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_dbm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_dmf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_dsm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_far.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_it.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_j2b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_mdl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_med.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_mid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_mod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_mt2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_mtm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_okt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_pat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_pat.h
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_psm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_ptm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_s3m.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_stm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_ult.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_umx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_wav.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\load_xm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\mmcmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\modplug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\modplug.h
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\snd_dsp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\snd_flt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\snd_fx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\sndfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\sndmix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libmodplug_0_8_8_x\src\tables.h
# End Source File
# End Group
# End Group
# End Target
# End Project
