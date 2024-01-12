=====================================================================
ModPlug Wrapper Input Plugin for Winamp
=====================================================================

IMPORTANT NOTES
===============
This plugin is based on the very obsolete engine. Much more advanced 
mod player engines are available for modern systems, e.g. libopenmpt 
or libXMP. This version was made to allow playback on very old 
computers/systems (and for those, whose can not live without the 
modplug-specific features), in which others can not be used.
It seems that ASM optimization has no effect (or very minor) in case 
of "No interpolation" or "Linear interpolation" on P-1 MMX machine, 
but there are some (but not huge) speed increase in case of higher 
quality interpolations. This means, that using VC6SP5+PP compiler 
have some advanteges over VC6SP6, but do not expect miracles.

SPECIAL NOTES:
Plugin compiled with VC2003, VC2005 and VC2008+LE have somewhat better 
memory management than VC6 (both SP5+PP and SP6) and VC2008+Win9xFix 
is even worse. These are prone to crash on Windows 9x on some very 
demanding MO3 files when the pagefile is set to user specified size 
(regardless of the size). VC2003 compiled seems to have a slightly 
less CPU usage than newer compilers, so this should be preferred.

Latest version + source code made by PPeti66x:
https://github.com/PPeti66x/in_modplug

Latest version + source code made by w8m and DSP 2003:
http://wks.arai-kibou.ru/

Latest libmodplug source code:
https://github.com/Konstanty/libmodplug
                                                       Sincerely
                                                  PPeti66x, Jan. 2024

How to compile
--------------
1.) Download the latest libmodplug source code and unpack it to the 
"libmodplug_0_8_8_x" subfolder.

2.) Edit file ".\libmodplug_0_8_8_x\src\libmodplug\stdafx.h":
search for (1 line):
#define NO_AGC

replace with (3 lines):
#ifndef USEEXTRAFEATURES
#define NO_AGC
#endif

3.) open the Visual Studio project file in the ".\build\vs<version>\" 
folder, configure by your needs and compile it.

NOTE: Avoid editing the resource files (main.rc, resource.h) in the
Visual Studio IDE. It will remove conditional definitions and will 
replce the pre-defined variables with immediate values.


KNOWN BUGS / LIMITATIONS (libmodplug)
=====================================
- 1st pattern of subsongs are skipped?
- Play time does not include subsongs
- Can not affect subsong playback behaviour


SPECIAL COMPILER DEFINITIONS
============================
USEEXTRAFEATURES  Enables AGC and Pre-amp (gain) options.
MFCGUI            Use MFC-based configuration dialog window (bigger 
                  and slower, so no reason to do that).
NOGUICONFIG       Disables configuration dialog window (so you must 
                  manually edit the ini file as before).
MODPLUG_STATIC    Beginning with the libmodplug 0.8.8.5 this is 
                  required for the WinAmp plugin compilation.
INFOTIMER         Enables Pattern, Row, Tempo, Voices realtime 
                  informations in the infobox.

These definitions (except "MODPLUG_STATIC") should be defined in the 
"resource.h" or in settings for both "C/C++" and "Resources"!
The MODPLUG_STATIC definition should be used via "C/C++" settings.


Version history
===============

20240108 - v0.3_1 [libmodplug 0.8.9.0-svn-20220131]:
[*] [pp6] updated the libmodplug library to 0.8.9.0-svn-20220131
[+] [pp6] format selection added to the Configuration dialog
[+] [pp6] AGC option and Pre-amp (gain) level settings added
[+] [pp6] new INI file is created (with descriptions) when necessary
[+] [pp6] fixed behaviour in cases where file does not contains title
[*] [pp6] libmodplug 0.8.9.0-svn-20220131 fixed all of the C89 compatibility issues
[*] [pp6] all previous libmodplug patches was backported from this version
[!] [pp6] unmo3.dll v2.4.2.0 no longer works on Win95, v2.4.1.2 is kept
[+] [pp6] added "SpecialBuild" text to VS_VERSION_INFO
[+] [pp6] reset button will initiate rewrite of all INI values when accepted
[+] [pp6] VS2003 project files
[+] [pp6] VS2005 project files with LegacyExtender and with BWC's Win95 fix
[+] [pp6] VS2008 project files with LegacyExtender and with BWC's Win95 fix

20231020 - v0.3 [libmodplug 0.8.9.0] (not released):
[*] [pp6] updated the libmodplug library to 0.8.9.0
[*] [pp6] fixed compilation in MSVC for libmodplug library (load_abc.cpp, load_pat.cpp)
[*] [pp6] configuration dialog is rewritten from MFC to Win32 native
[*] [pp6] MFC GUI is now obsolete, but still maintained
[+] [pp6] added tooltips with longer (up to 255 char) text support
[*] [pp6] updated to unmo3.dll v2.4.1.2 - still compatible with Win95 (at least with 95B)

20231019 - v0.2e [libmodplug 0.8.8.5] (not released):
[!] [pp6] unmo3.dll v2.4.0.5 API change - now both is supported (autodetected)
[+] [pp6] update to unmo3.dll v2.4.0.5
[*] [pp6] updated the libmodplug library to 0.8.8.5
[*] [pp6] fixed compilation in MSVC for libmodplug library (load_abc.cpp, load_pat.cpp)

20231019 - v0.2d [libmodplug 0.8.8.4] (not released):
[*] [pp6] updated the libmodplug library to 0.8.8.4
[*] [pp6] fixed compilation in MSVC for libmodplug library (stdafx.h, load_abc.cpp)
[+] [pp6] added GUI configuration (MFC version)
[+] [pp6] added limited tooltips (up to 80 char)

20231018 - v0.2c [libmodplug 0.8.8.3] (not released):
[*] [pp6] updated the libmodplug library to 0.8.8.3
[*] [pp6] fixed compilation in MSVC for libmodplug library (stdafx.h, load_abc.cpp)

20231017 - v0.2b1 [libmodplug 0.8.8.2] (not released):
[*] [pp6] recompiled with MS Visual C++ 6.0
[*] [pp6] updated unmo3.dll from v2.4.0.1 to v2.4.0.3
[+] [pp6] added VS_VERSION_INFO (version.rc, version.h)
[*] [pp6] version-specific strings are now replaced with constants declared in version.h

20120505 - v0.2b [libmodplug 0.8.8.2]:
[*] [dsp] Fixed bug with incorrect samples/instrument display (missing one field)
[*] [dsp] Added more formats to file extension list (thanks to Jojo of OpenMPT for the hint)

20110603 - v0.2a [libmodplug 0.8.8.2]:
[+] [dsp] Full Russian translation
[+] [dsp] Added order/tempo/bpm display and refined pattern/row/active voices display into more compact thing
[*] [w8m] Removed "Comment" button, changed things so the comment, instruments and samples pages are shown in a single window
[+] [w8m] Added pattern/row/active voices display

20110531 - v0.2 [libmodplug 0.8.8.2]:
[+] [w8m] Dynamically load unmo3.dll only if present/if there's attempt to load the mo3 file
[*] [w8m] Do not load the library for file info dialog again if the file is already loaded
[*] [dsp] Read-only edit controls instead of labels for module info fields
[+] [w8m] Added MO3 compressed module support (via unmo3.dll)
[+] [w8m] Added module info dialog

20110530 - v0.1 [libmodplug 0.8.8.2]:
[*] [dsp] Reverted mixing to default
[*] [w8m] Recompiled with MSVC2005
[*] [w8m] Fixed Winamp crash
[*] [w8m] Changed the mechanism of sound volume control to pass directly to ModPlug's mixer
[*] [w8m] Fixed buffer lock when seeking

20110529 - v0.0 [libmodplug 0.8.8.2]:
[*] [w8m] First prototype of wrapper plugin

[w8m] = "w8m"
[dsp] = "dsp 2003"
[pp6] = "PPeti66x"
