# Microsoft Developer Studio Generated NMAKE File, Based on condor_vmgahp.dsp
!IF "$(CFG)" == ""
CFG=condor_vmgahp - Win32 Release
!MESSAGE No configuration specified. Defaulting to condor_vmgahp - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "condor_vmgahp - Win32 Debug" && "$(CFG)" != "condor_vmgahp - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "condor_vmgahp.mak" CFG="condor_vmgahp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "condor_vmgahp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "condor_vmgahp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "condor_vmgahp - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\..\Debug
# Begin Custom Macros
OutDir=.\..\Debug
# End Custom Macros

ALL : "$(OUTDIR)\condor_vm-gahp.exe"


CLEAN :
	-@erase "$(INTDIR)\pbuffer.obj"
	-@erase "$(INTDIR)\soap_vmgahpC.obj"
	-@erase "$(INTDIR)\soap_vmgahpServer.obj"
	-@erase "$(INTDIR)\soap_vmgahpStub.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vm_request.obj"
	-@erase "$(INTDIR)\vm_type.obj"
	-@erase "$(INTDIR)\vmgahp.obj"
	-@erase "$(INTDIR)\vmgahp_common.obj"
	-@erase "$(INTDIR)\vmgahp_config.obj"
	-@erase "$(INTDIR)\vmgahp_instantiate.obj"
	-@erase "$(INTDIR)\vmgahp_main.obj"
	-@erase "$(INTDIR)\vmware_type.obj"
	-@erase "$(OUTDIR)\condor_vm-gahp.exe"
	-@erase "$(OUTDIR)\condor_vm-gahp.ilk"
	-@erase "$(OUTDIR)\condor_vm-gahp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\condor_common.pch" /Yu"condor_common.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP $(CONDOR_INCLUDE) $(CONDOR_GSOAP_INCLUDE) $(CONDOR_GLOBUS_INCLUDE) $(CONDOR_KERB_INCLUDE) $(CONDOR_PCRE_INCLUDE) $(CONDOR_OPENSSL_INCLUDE) $(CONDOR_POSTGRESQL_INCLUDE) /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\condor_vmgahp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=../Debug/condor_common.obj ..\Debug\condor_common_c.obj $(CONDOR_LIB) $(CONDOR_LIBPATH) $(CONDOR_GSOAP_LIB) $(CONDOR_GSOAP_LIBPATH) $(CONDOR_KERB_LIB) $(CONDOR_KERB_LIBPATH) $(CONDOR_GLOBUS_LIBPATH) $(CONDOR_GLOBUS_LIB) $(CONDOR_PCRE_LIB) $(CONDOR_PCRE_LIBPATH) $(CONDOR_OPENSSL_LIB) $(CONDOR_POSTGRESQL_LIB) $(CONDOR_OPENSSL_LIBPATH) $(CONDOR_POSTGRESQL_LIBPATH) /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\condor_vm-gahp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\condor_vm-gahp.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\pbuffer.obj" \
	"$(INTDIR)\soap_vmgahpC.obj" \
	"$(INTDIR)\soap_vmgahpServer.obj" \
	"$(INTDIR)\soap_vmgahpStub.obj" \
	"$(INTDIR)\vm_request.obj" \
	"$(INTDIR)\vm_type.obj" \
	"$(INTDIR)\vmgahp.obj" \
	"$(INTDIR)\vmgahp_common.obj" \
	"$(INTDIR)\vmgahp_config.obj" \
	"$(INTDIR)\vmgahp_instantiate.obj" \
	"$(INTDIR)\vmgahp_main.obj" \
	"$(INTDIR)\vmware_type.obj"

"$(OUTDIR)\condor_vm-gahp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "condor_vmgahp - Win32 Release"

OUTDIR=.\../Release
INTDIR=.\../Release
# Begin Custom Macros
OutDir=.\../Release
# End Custom Macros

ALL : "$(OUTDIR)\condor_vm-gahp.exe"


CLEAN :
	-@erase "$(INTDIR)\pbuffer.obj"
	-@erase "$(INTDIR)\soap_vmgahpC.obj"
	-@erase "$(INTDIR)\soap_vmgahpServer.obj"
	-@erase "$(INTDIR)\soap_vmgahpStub.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vm_request.obj"
	-@erase "$(INTDIR)\vm_type.obj"
	-@erase "$(INTDIR)\vmgahp.obj"
	-@erase "$(INTDIR)\vmgahp_common.obj"
	-@erase "$(INTDIR)\vmgahp_config.obj"
	-@erase "$(INTDIR)\vmgahp_instantiate.obj"
	-@erase "$(INTDIR)\vmgahp_main.obj"
	-@erase "$(INTDIR)\vmware_type.obj"
	-@erase "$(OUTDIR)\condor_vm-gahp.exe"
	-@erase "$(OUTDIR)\condor_vm-gahp.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /Z7 /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\condor_common.pch" /Yu"condor_common.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP $(CONDOR_INCLUDE) $(CONDOR_GSOAP_INCLUDE) $(CONDOR_GLOBUS_INCLUDE) $(CONDOR_KERB_INCLUDE) $(CONDOR_PCRE_INCLUDE) $(CONDOR_OPENSSL_INCLUDE) $(CONDOR_POSTGRESQL_INCLUDE) /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\condor_vmgahp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=../Release/condor_common.obj ..\Release\condor_common_c.obj $(CONDOR_LIB) $(CONDOR_LIBPATH) $(CONDOR_GSOAP_LIB) $(CONDOR_GSOAP_LIBPATH) $(CONDOR_KERB_LIB) $(CONDOR_KERB_LIBPATH) $(CONDOR_GLOBUS_LIBPATH) $(CONDOR_GLOBUS_LIB) $(CONDOR_PCRE_LIB) $(CONDOR_PCRE_LIBPATH) $(CONDOR_OPENSSL_LIB) $(CONDOR_POSTGRESQL_LIB) $(CONDOR_OPENSSL_LIBPATH) $(CONDOR_POSTGRESQL_LIBPATH) /nologo /subsystem:console /pdb:none /map:"$(INTDIR)\condor_vm-gahp.map" /debug /machine:I386 /out:"$(OUTDIR)\condor_vm-gahp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\pbuffer.obj" \
	"$(INTDIR)\soap_vmgahpC.obj" \
	"$(INTDIR)\soap_vmgahpServer.obj" \
	"$(INTDIR)\soap_vmgahpStub.obj" \
	"$(INTDIR)\vm_request.obj" \
	"$(INTDIR)\vm_type.obj" \
	"$(INTDIR)\vmgahp.obj" \
	"$(INTDIR)\vmgahp_common.obj" \
	"$(INTDIR)\vmgahp_config.obj" \
	"$(INTDIR)\vmgahp_instantiate.obj" \
	"$(INTDIR)\vmgahp_main.obj" \
	"$(INTDIR)\vmware_type.obj"

"$(OUTDIR)\condor_vm-gahp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("condor_vmgahp.dep")
!INCLUDE "condor_vmgahp.dep"
!ELSE 
!MESSAGE Warning: cannot find "condor_vmgahp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "condor_vmgahp - Win32 Debug" || "$(CFG)" == "condor_vmgahp - Win32 Release"
SOURCE="..\src\condor_vm-gahp\pbuffer.C"

"$(INTDIR)\pbuffer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\src\condor_vm-gahp\soap_vmgahpC.C"

!IF  "$(CFG)" == "condor_vmgahp - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\condor_common.pch" /Yu"condor_common.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP $(CONDOR_INCLUDE) $(CONDOR_GSOAP_INCLUDE) $(CONDOR_GLOBUS_INCLUDE) $(CONDOR_KERB_INCLUDE) $(CONDOR_PCRE_INCLUDE) $(CONDOR_OPENSSL_INCLUDE) $(CONDOR_POSTGRESQL_INCLUDE) /c 

"$(INTDIR)\soap_vmgahpC.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "condor_vmgahp - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Z7 /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP $(CONDOR_INCLUDE) $(CONDOR_GSOAP_INCLUDE) $(CONDOR_GLOBUS_INCLUDE) $(CONDOR_KERB_INCLUDE) $(CONDOR_PCRE_INCLUDE) $(CONDOR_OPENSSL_INCLUDE) $(CONDOR_POSTGRESQL_INCLUDE) /c 

"$(INTDIR)\soap_vmgahpC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE="..\src\condor_vm-gahp\soap_vmgahpServer.C"

!IF  "$(CFG)" == "condor_vmgahp - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\condor_common.pch" /Yu"condor_common.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP $(CONDOR_INCLUDE) $(CONDOR_GSOAP_INCLUDE) $(CONDOR_GLOBUS_INCLUDE) $(CONDOR_KERB_INCLUDE) $(CONDOR_PCRE_INCLUDE) $(CONDOR_OPENSSL_INCLUDE) $(CONDOR_POSTGRESQL_INCLUDE) /c 

"$(INTDIR)\soap_vmgahpServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "condor_vmgahp - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Z7 /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP $(CONDOR_INCLUDE) $(CONDOR_GSOAP_INCLUDE) $(CONDOR_GLOBUS_INCLUDE) $(CONDOR_KERB_INCLUDE) $(CONDOR_PCRE_INCLUDE) $(CONDOR_OPENSSL_INCLUDE) $(CONDOR_POSTGRESQL_INCLUDE) /c 

"$(INTDIR)\soap_vmgahpServer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE="..\src\condor_vm-gahp\soap_vmgahpStub.C"

!IF  "$(CFG)" == "condor_vmgahp - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\condor_common.pch" /Yu"condor_common.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP $(CONDOR_INCLUDE) $(CONDOR_GSOAP_INCLUDE) $(CONDOR_GLOBUS_INCLUDE) $(CONDOR_KERB_INCLUDE) $(CONDOR_PCRE_INCLUDE) $(CONDOR_OPENSSL_INCLUDE) $(CONDOR_POSTGRESQL_INCLUDE) /c 

"$(INTDIR)\soap_vmgahpStub.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "condor_vmgahp - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Z7 /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP $(CONDOR_INCLUDE) $(CONDOR_GSOAP_INCLUDE) $(CONDOR_GLOBUS_INCLUDE) $(CONDOR_KERB_INCLUDE) $(CONDOR_PCRE_INCLUDE) $(CONDOR_OPENSSL_INCLUDE) $(CONDOR_POSTGRESQL_INCLUDE) /c 

"$(INTDIR)\soap_vmgahpStub.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE="..\src\condor_vm-gahp\vm_request.C"

"$(INTDIR)\vm_request.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\src\condor_vm-gahp\vm_type.C"

"$(INTDIR)\vm_type.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\src\condor_vm-gahp\vmgahp.C"

"$(INTDIR)\vmgahp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\src\condor_vm-gahp\vmgahp_common.C"

"$(INTDIR)\vmgahp_common.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\src\condor_vm-gahp\vmgahp_config.C"

"$(INTDIR)\vmgahp_config.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\src\condor_vm-gahp\vmgahp_instantiate.C"

"$(INTDIR)\vmgahp_instantiate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\src\condor_vm-gahp\vmgahp_main.C"

"$(INTDIR)\vmgahp_main.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\src\condor_vm-gahp\vmware_type.C"

"$(INTDIR)\vmware_type.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\condor_common.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

