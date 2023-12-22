# Microsoft Developer Studio Project File - Name="KlockPlusCE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (WCE x86em) Application" 0x0b01
# TARGTYPE "Win32 (WCE MIPS) Application" 0x0a01
# TARGTYPE "Win32 (WCE SH) Application" 0x0901

CFG=KlockPlusCE - Win32 (WCE x86em) Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KlockPlusCE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KlockPlusCE.mak" CFG="KlockPlusCE - Win32 (WCE x86em) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KlockPlusCE - Win32 (WCE x86em) Release" (based on\
 "Win32 (WCE x86em) Application")
!MESSAGE "KlockPlusCE - Win32 (WCE x86em) Debug" (based on\
 "Win32 (WCE x86em) Application")
!MESSAGE "KlockPlusCE - Win32 (WCE MIPS) Release" (based on\
 "Win32 (WCE MIPS) Application")
!MESSAGE "KlockPlusCE - Win32 (WCE MIPS) Debug" (based on\
 "Win32 (WCE MIPS) Application")
!MESSAGE "KlockPlusCE - Win32 (WCE SH) Release" (based on\
 "Win32 (WCE SH) Application")
!MESSAGE "KlockPlusCE - Win32 (WCE SH) Debug" (based on\
 "Win32 (WCE SH) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
# PROP WCE_Configuration "H/PC Ver. 2.00"

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "x86emRel"
# PROP BASE Intermediate_Dir "x86emRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x86emRel"
# PROP Intermediate_Dir "x86emRel"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /O2 /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "_WIN32_WCE_EMULATION" /D "INTERNATIONAL" /D "USA" /D "INTLMSG_CODEPAGE" /D "NDEBUG" /D "x86" /D "i486" /D "_x86_" /YX /c
# ADD CPP /nologo /ML /W3 /O2 /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "_WIN32_WCE_EMULATION" /D "INTERNATIONAL" /D "USA" /D "INTLMSG_CODEPAGE" /D "NDEBUG" /D "x86" /D "i486" /D "_x86_" /YX /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d UNDER_CE=$(CEVersion) /d "UNICODE" /d "_UNICODE" /d "WIN32" /d "STRICT" /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "_WIN32_WCE_EMULATION" /d "INTERNATIONAL" /d "USA" /d "INTLMSG_CODEPAGE" /d "NDEBUG"
# ADD RSC /l 0x409 /d UNDER_CE=$(CEVersion) /d "UNICODE" /d "_UNICODE" /d "WIN32" /d "STRICT" /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "_WIN32_WCE_EMULATION" /d "INTERNATIONAL" /d "USA" /d "INTLMSG_CODEPAGE" /d "NDEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /subsystem:windows /machine:I386 /windowsce:emulation
# ADD LINK32 commctrl.lib coredll.lib /nologo /subsystem:windows /machine:I386 /windowsce:emulation
EMPFILE=empfile.exe
# ADD BASE EMPFILE -COPY
# ADD EMPFILE -COPY

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "x86emDbg"
# PROP BASE Intermediate_Dir "x86emDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x86emDbg"
# PROP Intermediate_Dir "x86emDbg"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MLd /W3 /Gm /Zi /Od /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "_WIN32_WCE_EMULATION" /D "INTERNATIONAL" /D "USA" /D "INTLMSG_CODEPAGE" /D "_DEBUG" /D "x86" /D "i486" /D "_x86_" /YX /c
# ADD CPP /nologo /MLd /W3 /Gm /Zi /Od /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "_WIN32_WCE_EMULATION" /D "INTERNATIONAL" /D "USA" /D "INTLMSG_CODEPAGE" /D "_DEBUG" /D "x86" /D "i486" /D "_x86_" /YX /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d UNDER_CE=$(CEVersion) /d "UNICODE" /d "_UNICODE" /d "WIN32" /d "STRICT" /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "_WIN32_WCE_EMULATION" /d "INTERNATIONAL" /d "USA" /d "INTLMSG_CODEPAGE" /d "_DEBUG" /d "x86" /d "i486" /d "_x86_"
# ADD RSC /l 0x409 /d UNDER_CE=$(CEVersion) /d "UNICODE" /d "_UNICODE" /d "WIN32" /d "STRICT" /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "_WIN32_WCE_EMULATION" /d "INTERNATIONAL" /d "USA" /d "INTLMSG_CODEPAGE" /d "_DEBUG" /d "x86" /d "i486" /d "_x86_"
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /subsystem:windows /debug /machine:I386 /windowsce:emulation
# ADD LINK32 commctrl.lib coredll.lib /nologo /subsystem:windows /debug /machine:I386 /windowsce:emulation
EMPFILE=empfile.exe
# ADD BASE EMPFILE -COPY
# ADD EMPFILE -COPY

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WMIPSRel"
# PROP BASE Intermediate_Dir "WMIPSRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WMIPSRel"
# PROP Intermediate_Dir "WMIPSRel"
# PROP Target_Dir ""
CPP=clmips.exe
# ADD BASE CPP /nologo /ML /W3 /O2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
# ADD CPP /nologo /ML /W3 /O2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WMIPSDbg"
# PROP BASE Intermediate_Dir "WMIPSDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WMIPSDbg"
# PROP Intermediate_Dir "WMIPSDbg"
# PROP Target_Dir ""
CPP=clmips.exe
# ADD BASE CPP /nologo /MLd /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
# ADD CPP /nologo /MLd /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
# ADD RSC /l 0x409 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /debug /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /debug /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WCESHRel"
# PROP BASE Intermediate_Dir "WCESHRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WCESHRel"
# PROP Intermediate_Dir "WCESHRel"
# PROP Target_Dir ""
CPP=shcl.exe
# ADD BASE CPP /nologo /ML /W3 /O2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
# ADD CPP /nologo /ML /W3 /O2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WCESHDbg"
# PROP BASE Intermediate_Dir "WCESHDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WCESHDbg"
# PROP Intermediate_Dir "WCESHDbg"
# PROP Target_Dir ""
CPP=shcl.exe
# ADD BASE CPP /nologo /MLd /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
# ADD CPP /nologo /MLd /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
# ADD RSC /l 0x409 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /debug /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /debug /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY

!ENDIF 

# Begin Target

# Name "KlockPlusCE - Win32 (WCE x86em) Release"
# Name "KlockPlusCE - Win32 (WCE x86em) Debug"
# Name "KlockPlusCE - Win32 (WCE MIPS) Release"
# Name "KlockPlusCE - Win32 (WCE MIPS) Debug"
# Name "KlockPlusCE - Win32 (WCE SH) Release"
# Name "KlockPlusCE - Win32 (WCE SH) Debug"
# Begin Group "KlockPlusCE"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\ce-utils.c"

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_CE_UT=\
	"..\ce-utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_CE_UT=\
	"..\ce-utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_CE_UT=\
	"..\ce-utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\ce-utils.h"
# End Source File
# Begin Source File

SOURCE=..\device.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_DEVIC=\
	"..\ce-utils.h"\
	"..\device.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_DEVIC=\
	"..\ce-utils.h"\
	"..\device.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_DEVIC=\
	"..\ce-utils.h"\
	"..\device.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\device.h
# End Source File
# Begin Source File

SOURCE=..\input.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_INPUT=\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_INPUT=\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_INPUT=\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\input.h
# End Source File
# Begin Source File

SOURCE=..\main.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_MAIN_=\
	"..\ce-utils.h"\
	"..\device.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_MAIN_=\
	"..\ce-utils.h"\
	"..\device.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_MAIN_=\
	"..\ce-utils.h"\
	"..\device.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\resource.h
# End Source File
# End Group
# Begin Group "Graphics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\graphics\assets.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_ASSET=\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.c"\
	"..\graphics\generated\Font_canele_12.c"\
	"..\graphics\generated\Osb_a.c"\
	"..\graphics\generated\Osb_arrow_down.c"\
	"..\graphics\generated\Osb_arrow_left.c"\
	"..\graphics\generated\Osb_arrow_right.c"\
	"..\graphics\generated\Osb_arrow_up.c"\
	"..\graphics\generated\Osb_b.c"\
	"..\graphics\generated\Osb_close.c"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_ASSET=\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.c"\
	"..\graphics\generated\Font_canele_12.c"\
	"..\graphics\generated\Osb_a.c"\
	"..\graphics\generated\Osb_arrow_down.c"\
	"..\graphics\generated\Osb_arrow_left.c"\
	"..\graphics\generated\Osb_arrow_right.c"\
	"..\graphics\generated\Osb_arrow_up.c"\
	"..\graphics\generated\Osb_b.c"\
	"..\graphics\generated\Osb_close.c"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_ASSET=\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.c"\
	"..\graphics\generated\Font_canele_12.c"\
	"..\graphics\generated\Osb_a.c"\
	"..\graphics\generated\Osb_arrow_down.c"\
	"..\graphics\generated\Osb_arrow_left.c"\
	"..\graphics\generated\Osb_arrow_right.c"\
	"..\graphics\generated\Osb_arrow_up.c"\
	"..\graphics\generated\Osb_b.c"\
	"..\graphics\generated\Osb_close.c"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\graphics\assets.h
# End Source File
# Begin Source File

SOURCE=..\graphics\defs.h
# End Source File
# Begin Source File

SOURCE=..\graphics\graph.h
# End Source File
# Begin Source File

SOURCE=..\graphics\graph_provider.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_GRAPH=\
	"..\ce-utils.h"\
	"..\device.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_GRAPH=\
	"..\ce-utils.h"\
	"..\device.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_GRAPH=\
	"..\ce-utils.h"\
	"..\device.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\graphics\pixel.h
# End Source File
# End Group
# Begin Group "Scene"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\scene\scene.h
# End Source File
# Begin Source File

SOURCE=..\scene\scene_app.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_SCENE=\
	"..\..\core\kommon.h"\
	"..\..\core\krt.h"\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_SCENE=\
	"..\..\core\kommon.h"\
	"..\..\core\krt.h"\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_SCENE=\
	"..\..\core\kommon.h"\
	"..\..\core\krt.h"\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\scene\scene_basic.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_SCENE_=\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_SCENE_=\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_SCENE_=\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\scene\scene_menu.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_SCENE_M=\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_SCENE_M=\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_SCENE_M=\
	"..\ce-utils.h"\
	"..\graphics\assets.h"\
	"..\graphics\defs.h"\
	"..\graphics\generated\_graphics_list.h"\
	"..\graphics\graph.h"\
	"..\graphics\pixel.h"\
	"..\input.h"\
	"..\scene\scene.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\core\kasset.h
# End Source File
# Begin Source File

SOURCE=..\..\core\kommon.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_KOMMO=\
	"..\..\core\kommon.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_KOMMO=\
	"..\..\core\kommon.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_KOMMO=\
	"..\..\core\kommon.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\core\kommon.h
# End Source File
# Begin Source File

SOURCE=..\..\core\konstant.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_KONST=\
	"..\..\core\kommon.h"\
	"..\..\core\konstant.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_KONST=\
	"..\..\core\kommon.h"\
	"..\..\core\konstant.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_KONST=\
	"..\..\core\kommon.h"\
	"..\..\core\konstant.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\core\konstant.h
# End Source File
# Begin Source File

SOURCE=..\..\core\krt.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_KRT_C=\
	"..\..\core\kommon.h"\
	"..\..\core\konstant.h"\
	"..\..\core\krt.h"\
	"..\..\core\utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_KRT_C=\
	"..\..\core\kommon.h"\
	"..\..\core\konstant.h"\
	"..\..\core\krt.h"\
	"..\..\core\utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_KRT_C=\
	"..\..\core\kommon.h"\
	"..\..\core\konstant.h"\
	"..\..\core\krt.h"\
	"..\..\core\utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\core\krt.h
# End Source File
# Begin Source File

SOURCE=..\..\core\utils.c

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

DEP_CPP_UTILS=\
	"..\..\core\utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

DEP_CPP_UTILS=\
	"..\..\core\utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

DEP_CPP_UTILS=\
	"..\..\core\utils.h"\
	

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\core\utils.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\klockplus.rc

!IF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE x86em) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "KlockPlusCE - Win32 (WCE SH) Debug"

!ENDIF 

# End Source File
# End Target
# End Project
