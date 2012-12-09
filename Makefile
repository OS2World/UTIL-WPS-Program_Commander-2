#===================================================================
#
#   PC/2 - Program Commander/2 Makefile
#
#   supports: 
#       Compiler: IBM C Set++ V2.1
#                 IBM VisualAge C++ V3.0
#                 EMX (unsupported, based on the work of Andrew
#                     Zabolotny)
#
#                     Ensure that you do not have VisualAge
#                     SOM headers in paths if using the SOM
#                     provided by the Toolkit
# 
#       Toolkit : IBM Developers Toolkit 2.1
#                 IBM Warp 3 Developers Toolkit
#                 IBM Warp 4 Developers Toolkit (req. for PC2Net.c)
# 
#                     DevCon 12 and DevCon V2R* versions require in 
#                     SOMOBJ.H to add:
#                     typedef somInitCtrl som3InitCtrl;
#                     typedef somAssignCtrl som3AssignCtrl;
#                     typedef somDestructCtrl som3DestructCtrl;
#                     probably due to compiler problem in SOMAPI.H
# 
#                     DevCon V2R1 RC.EXE corrupts executables
#                     DevCon V2R1 SOM compiler has internal error
#                                 (Trapped segment) can't be used
#                     DevCon V2R1 EXEHDR.EXE displays nonsense
# 
#                     DevCon V2R2 TCP/IP headers are at least, well 
#                                 say peculiar, check PC2Net.c for 
#                                 details
#===================================================================

# Get compiler & toolkit version

!ifdef VACPP_SHARED
IBMCPP30=1
!else
IBMCPP21=1
!endif # VACPP_SHARED

!ifndef BRENDER_PATH
IBMTLKT40=1
!else
!ifdef SMADDSTAR
IBMTLKT30=1
!else
IBMTLKT21=1
!endif # BRENDER_PATH
!endif # ODPGMREF

# Compiler environment

# ICC Flags
#   /Ti     Include debug information for IPMD
#   /Kb     Warning, if no prototypes found (prevents from incorrect params)
#   /c      Compile only, we link more than one ressource
#   /Se     Allow IBM C language extentions and migration
#   /Ms     Set the default linkage to _System
#   /Re     Produce Code in IBM C Set/2 Run Time environment
#   /Rn     No language environment. Allow system programming and subsystem development
#   /ss     Allow // as comments
#   /Gm     Link with multitasking libraries, because we're multithreaded
#   /Gn     Do not generate default libraries in object
#   /Ge-    Link with libraries that assume a DLL
#   /Gs     Remove stack probes
#   /O      Turn on code optimization (turns also function inlining on)
#   /Op-    Turn off stack pointer optimization
#   /Oi     Turn on function inlining
#   /Oc     Optimize for speed and size
#   /G5     Optimize for Pentium
#   /Fa     Generate assembly listing (for problem determination)

!ifndef SLOWVID
VIDEO =
!else
VIDEO = /DSLOWVIDEO
!endif # SLOWVID

# Test for IBM C Set++ V2.1 or IBM VisualAge C++ V3.0

!ifdef IBMCPP30
COMPILER = /DIBMCPP30
!else
COMPILER = /DIBMCPP21 /Kb+
!endif # IBMCPP30

# Test for IBM Toolkit V2.1 or IBM Warp Toolkit V3.0 / V4.0

!ifdef IBMTLKT40
TOOLKIT = /DIBMTLKT40
!endif # IBMTLKT40

!ifdef IBMTLKT30
TOOLKIT = /DIBMTLKT30
!endif # IBMTLKT30

!ifdef IBMTLKT21
TOOLKIT = /DIBMTLKT21
!endif # IBMTLKT21

!ifndef NODEBUG
CFLAGS =    /Ti /Oc /Tm /c /Se /Ms /Re     /ss /W3 /Gm       /G5 /Fa $(VIDEO) $(COMPILER) $(TOOLKIT) /DDEBUG
CFLAGSDLL = /Ti /Oc /Tm /c /Se /Ms     /Rn /ss /W3      /Ge- /G5 /Fa $(VIDEO) $(COMPILER) $(TOOLKIT) /DDEBUG
!else
CFLAGS =    /Ti /Oc     /c /Se /Ms /Re     /ss /W3 /Gm       /G5 /Fa $(VIDEO) $(COMPILER) $(TOOLKIT)
CFLAGSDLL = /Ti /Oc     /c /Se /Ms     /Rn /ss /W3      /Ge- /G5 /Fa $(VIDEO) $(COMPILER) $(TOOLKIT)
# PC/2 2.19c: VisualAge C++ 3 CSD8 optimizer seems to have a bug, because with option /O only 2 
#             threads will run. The bug seems near RegisterExceptionHandler() in Thread.c, it
#             probably can be localized when splitting the code into modules and see exactly
#             what's causing the bug exactly and why (using the assembly listing and/or debugger)
#      2.19e: PC2NET.DLL will trap when using option /Gs+, with further investigation it may
#             be caused by some stack shortage (at the moment I just skip that option)       
#CFLAGS =    /Gs+ /O /Oc     /c /Se /Ms /Re     /ss /W3 /Gm       /G5 /Fa $(VIDEO) $(COMPILER) $(TOOLKIT)
#CFLAGSDLL = /Gs+ /O /Oc     /c /Se /Ms     /Rn /ss /W3      /Ge- /G5 /Fa $(VIDEO) $(COMPILER) $(TOOLKIT)
!endif # NODEBUG

# LINK386 Flags
#   /DEBUG  Include debug information for IPMD
#   /NOE    NO Extented dictionary, don't assume any library dependencies
#   /NOD    NO Default library, ignore the library names in object files
#   /NOI    NO Ignore case
#   /A:16   Align on paragraph for PM programs
#   /MAP    Produce map
#   /E      Exepack to compress pages within the executable
#   /F      Farcalltranslation to optimize far calls by avoiding selector reloads
#   /PACKC  Overwrite Packcode default of 65530 to 0x40000

!ifndef NODEBUG
!ifdef IBMCPP30
LFLAGS = /DEBUG /NOE /NOD /ALIGN:8 /MAP /NOI /E    /PACKC:0x40000
LFLAGR = /ALIGN:1 /MAP
!else
LFLAGS = /DEBUG /NOE /NOD /ALIGN:8 /MAP /NOI /E /F /PACKC:0x40000
LFLAGR = /ALIGN:1 /MAP
!endif # IBMCPP30
!else
!ifdef IBMCPP30
LFLAGS = /DEBUG /NOE /NOD /ALIGN:8 /MAP /NOI /E    /PACKC:0x40000
LFLAGR = /ALIGN:1 /MAP
!else
LFLAGS = /DEBUG /NOE /NOD /ALIGN:8 /MAP /NOI /E /F /PACKC:0x40000
LFLAGR = /ALIGN:1 /MAP
!endif # IBMCPP30
!endif # NODEBUG

# MicroSoft MASM 6.0 Flags
#   /c      Compile only, we link more than one ressource
#   /Cx     Preserve case in publics, externs
#   /Cp     Preserve case of user identifiers
#   /Fl     Generate listings

!ifndef NODEBUG
MFLAGS = /c /Cx /Cp /Zd /Zi
!else
MFLAGS = /c /Cx /Cp
!endif # NODEBUG

# Libraries
#   C Set++ V2.01
#     DDE4MBS Multitasking standard library
#     DDE4NBS Subsystem library
#   VisualAge C++ V3.0
#     CPPOM30 Multitasking standard library
#     CPPON30 Subsystem library
#   OS2386  OS/2 2.0 Link library
#   SO32DLL TCP/IP Link library
#   TCP32DLL
!ifdef IBMCPP30
LIBS = CPPOM30 + OS2386
LIBSS = CPPON30 + OS2386
!else
LIBS = DDE4MBS + OS2386
LIBSS = DDE4NBS + OS2386
!endif # IBMCPP30
LIBTCP = SO32DLL + TCP32DLL

# Directories
# \.        Executables, help panels, DLLs, documentation, profiles
# \Source   Source, object modules, headers
SRC=Source

# SC Flags
# For the SOM compiler, the following environment variables are assumed
# to be set for the IBM OS/2 Developer's Toolkit 2.1:
#
# SET SMINCLUDE=.;x:\TOOLKT21\SC;
# SET SMTMP=x:\
# SET SMEMIT=ih;h;ph;psc;sc;c;def
# SOM 1 (Toolkit 2.1)
#   -v          Verbose mode
#   -r          Check that release order entries exist
#   -d          Output directory of emitted file
#
# SOM 2 (Toolkit 3.0 / 4.0)
#   Note: The path .;.\Source; must be included in the INCLUDE path
#   -maddstar   add * to C bindings for interface references
#   -mnoint     don't warn about "int" causing portability problems
#   -S128000    size of string buffer (default 49152)
#   -C128000    size of comment buffer (default 59152)
#   -v          verbose debugging move
#   -p          shorthand for -D__PRIVATE__
#   -s c        replace SMEMIT variable with <string> (creates *.C file)
#   -s def      replace SMEMIT variable with <string> (creates *.DEF file)
#
# For the SOM compiler, the following environment variables are assumed
# to be set for the IBM OS/2 Warp Developer's Toolkit 3.0 / 4.0:
#
# SET SMINCLUDE=.;x:\TOOLKT30\IDL;x:\TOOLKT30\SOM\INCLUDE;
# SET SMTMP=x:\
# SET SMEMIT=ih;h;c;def
# SET SMADDSTAR=1
# SET SMNOTC=1
#
# Ensure that INCLUDE and SMINCLUDE do NOT contain the path to the
# compiler SOM header files, if using the toolkit SOM

!ifdef IBMTLKT40
SCFLAGS = -maddstar -mnoint -S128000 -C128000 -v -p
SCFLAGS1 = -maddstar -mnoint -S128000 -C128000 -v -p -s c
SCFLAGS2 = -maddstar -mnoint -S128000 -C128000 -v -p -s def
RCFLAGS = -x1
SOMLIB = SOMTK
!endif # IBMTLKT40

!ifdef IBMTLKT30
SCFLAGS = -maddstar -mnoint -S128000 -C128000 -v -p
SCFLAGS1 = -maddstar -mnoint -S128000 -C128000 -v -p -s c
SCFLAGS2 = -maddstar -mnoint -S128000 -C128000 -v -p -s def
RCFLAGS = -x1 -n
SOMLIB = SOMTK
!endif # IBMTLKT30

!ifdef IBMTLKT21
SCFLAGS = -v -r -d $(SRC)
RCFLAGS = -x
SOMLIB = SOM
!endif # IBMTLKT21

CC = icc $(CFLAGS)
SC = sc $(SCFLAGS)
SC1 = sc $(SCFLAGS1)
SC2 = sc $(SCFLAGS2)
CCDLL = icc $(CFLAGSDLL)
ASM = ml $(MFLAGS)
RC = rc $(RCFLAGS)
!ifdef IBMCPP30
LINK = ILINK /NOFREE $(LFLAGS)
LINKRES = ILINK /NOFREE $(LFLAGR)
!else
LINK = LINK386 $(LFLAGS)
LINKRES = LINK386 $(LFLAGR)
!endif # IBMCPP30

#===================================================================
# Now include the actual build process
#===================================================================

!include Makerules
