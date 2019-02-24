##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=dvbServe
ConfigurationName      :=Debug
WorkspacePath          := "/home/richard/codelite/DVB"
ProjectPath            := /home/richard/Source/DVB
IntermediateDirectory  :=./Debug_ARM
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=richard
Date                   :=01/28/14
CodeLitePath           :="/home/richard/.codelite"
LinkerName             :=armv7-linux-gcc
SharedObjectLinkerName :=armv7-linux-gcc -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="dvbServe.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). -I /home/richard/Source/DVB/Packages/libhttpd_ARM/include -I /home/richard/Source/DVB/Packages/libdvbpsi_ARM/include
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)pthread $(LibrarySwitch)dl $(LibrarySwitch)dvbpsi $(LibrarySwitch)microhttpd -L /home/richard/Source/DVB/Packages/libdvbpsi_ARM/lib
ArLibs                 :=  "pthread" "dl" "dvbpsi" "microhttpd" 
LibPath                := $(LibraryPathSwitch). -L /home/richard/Source/DVB/Packages/libhttpd_ARM/lib

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := armv7-linux-ar rcus
CXX      := armv7-linux-gcc
CC       := armv7-linux-gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := armv7-linux-as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main$(ObjectSuffix) $(IntermediateDirectory)/linkedlist$(ObjectSuffix) $(IntermediateDirectory)/socketserver$(ObjectSuffix) $(IntermediateDirectory)/telnetservermanager$(ObjectSuffix) $(IntermediateDirectory)/utils$(ObjectSuffix) $(IntermediateDirectory)/configuration$(ObjectSuffix) $(IntermediateDirectory)/dictionary$(ObjectSuffix) $(IntermediateDirectory)/iniparser$(ObjectSuffix) $(IntermediateDirectory)/sqlite3$(ObjectSuffix) $(IntermediateDirectory)/dvbhardware$(ObjectSuffix) \
	$(IntermediateDirectory)/channels$(ObjectSuffix) $(IntermediateDirectory)/session$(ObjectSuffix) $(IntermediateDirectory)/destination$(ObjectSuffix) $(IntermediateDirectory)/webserv$(ObjectSuffix) $(IntermediateDirectory)/database$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main$(ObjectSuffix): main.c $(IntermediateDirectory)/main$(DependSuffix)
	$(CC) $(SourceSwitch) "main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main$(ObjectSuffix) -MF$(IntermediateDirectory)/main$(DependSuffix) -MM "main.c"

$(IntermediateDirectory)/main$(PreprocessSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main$(PreprocessSuffix) "main.c"

$(IntermediateDirectory)/linkedlist$(ObjectSuffix): linkedlist.c $(IntermediateDirectory)/linkedlist$(DependSuffix)
	$(CC) $(SourceSwitch) "linkedlist.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/linkedlist$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/linkedlist$(DependSuffix): linkedlist.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/linkedlist$(ObjectSuffix) -MF$(IntermediateDirectory)/linkedlist$(DependSuffix) -MM "linkedlist.c"

$(IntermediateDirectory)/linkedlist$(PreprocessSuffix): linkedlist.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/linkedlist$(PreprocessSuffix) "linkedlist.c"

$(IntermediateDirectory)/socketserver$(ObjectSuffix): socketserver.c $(IntermediateDirectory)/socketserver$(DependSuffix)
	$(CC) $(SourceSwitch) "socketserver.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketserver$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketserver$(DependSuffix): socketserver.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketserver$(ObjectSuffix) -MF$(IntermediateDirectory)/socketserver$(DependSuffix) -MM "socketserver.c"

$(IntermediateDirectory)/socketserver$(PreprocessSuffix): socketserver.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketserver$(PreprocessSuffix) "socketserver.c"

$(IntermediateDirectory)/telnetservermanager$(ObjectSuffix): telnetservermanager.c $(IntermediateDirectory)/telnetservermanager$(DependSuffix)
	$(CC) $(SourceSwitch) "telnetservermanager.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/telnetservermanager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/telnetservermanager$(DependSuffix): telnetservermanager.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/telnetservermanager$(ObjectSuffix) -MF$(IntermediateDirectory)/telnetservermanager$(DependSuffix) -MM "telnetservermanager.c"

$(IntermediateDirectory)/telnetservermanager$(PreprocessSuffix): telnetservermanager.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/telnetservermanager$(PreprocessSuffix) "telnetservermanager.c"

$(IntermediateDirectory)/utils$(ObjectSuffix): utils.c $(IntermediateDirectory)/utils$(DependSuffix)
	$(CC) $(SourceSwitch) "utils.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/utils$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/utils$(DependSuffix): utils.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/utils$(ObjectSuffix) -MF$(IntermediateDirectory)/utils$(DependSuffix) -MM "utils.c"

$(IntermediateDirectory)/utils$(PreprocessSuffix): utils.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/utils$(PreprocessSuffix) "utils.c"

$(IntermediateDirectory)/configuration$(ObjectSuffix): configuration.c $(IntermediateDirectory)/configuration$(DependSuffix)
	$(CC) $(SourceSwitch) "configuration.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/configuration$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/configuration$(DependSuffix): configuration.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/configuration$(ObjectSuffix) -MF$(IntermediateDirectory)/configuration$(DependSuffix) -MM "configuration.c"

$(IntermediateDirectory)/configuration$(PreprocessSuffix): configuration.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/configuration$(PreprocessSuffix) "configuration.c"

$(IntermediateDirectory)/dictionary$(ObjectSuffix): dictionary.c $(IntermediateDirectory)/dictionary$(DependSuffix)
	$(CC) $(SourceSwitch) "dictionary.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dictionary$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dictionary$(DependSuffix): dictionary.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dictionary$(ObjectSuffix) -MF$(IntermediateDirectory)/dictionary$(DependSuffix) -MM "dictionary.c"

$(IntermediateDirectory)/dictionary$(PreprocessSuffix): dictionary.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dictionary$(PreprocessSuffix) "dictionary.c"

$(IntermediateDirectory)/iniparser$(ObjectSuffix): iniparser.c $(IntermediateDirectory)/iniparser$(DependSuffix)
	$(CC) $(SourceSwitch) "iniparser.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/iniparser$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/iniparser$(DependSuffix): iniparser.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/iniparser$(ObjectSuffix) -MF$(IntermediateDirectory)/iniparser$(DependSuffix) -MM "iniparser.c"

$(IntermediateDirectory)/iniparser$(PreprocessSuffix): iniparser.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/iniparser$(PreprocessSuffix) "iniparser.c"

$(IntermediateDirectory)/sqlite3$(ObjectSuffix): sqlite3.c $(IntermediateDirectory)/sqlite3$(DependSuffix)
	$(CC) $(SourceSwitch) "sqlite3.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sqlite3$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sqlite3$(DependSuffix): sqlite3.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sqlite3$(ObjectSuffix) -MF$(IntermediateDirectory)/sqlite3$(DependSuffix) -MM "sqlite3.c"

$(IntermediateDirectory)/sqlite3$(PreprocessSuffix): sqlite3.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sqlite3$(PreprocessSuffix) "sqlite3.c"

$(IntermediateDirectory)/dvbhardware$(ObjectSuffix): dvbhardware.c $(IntermediateDirectory)/dvbhardware$(DependSuffix)
	$(CC) $(SourceSwitch) "dvbhardware.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dvbhardware$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dvbhardware$(DependSuffix): dvbhardware.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dvbhardware$(ObjectSuffix) -MF$(IntermediateDirectory)/dvbhardware$(DependSuffix) -MM "dvbhardware.c"

$(IntermediateDirectory)/dvbhardware$(PreprocessSuffix): dvbhardware.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dvbhardware$(PreprocessSuffix) "dvbhardware.c"

$(IntermediateDirectory)/channels$(ObjectSuffix): channels.c $(IntermediateDirectory)/channels$(DependSuffix)
	$(CC) $(SourceSwitch) "channels.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/channels$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/channels$(DependSuffix): channels.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/channels$(ObjectSuffix) -MF$(IntermediateDirectory)/channels$(DependSuffix) -MM "channels.c"

$(IntermediateDirectory)/channels$(PreprocessSuffix): channels.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/channels$(PreprocessSuffix) "channels.c"

$(IntermediateDirectory)/session$(ObjectSuffix): session.c $(IntermediateDirectory)/session$(DependSuffix)
	$(CC) $(SourceSwitch) "session.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/session$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/session$(DependSuffix): session.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/session$(ObjectSuffix) -MF$(IntermediateDirectory)/session$(DependSuffix) -MM "session.c"

$(IntermediateDirectory)/session$(PreprocessSuffix): session.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/session$(PreprocessSuffix) "session.c"

$(IntermediateDirectory)/destination$(ObjectSuffix): destination.c $(IntermediateDirectory)/destination$(DependSuffix)
	$(CC) $(SourceSwitch) "destination.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/destination$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/destination$(DependSuffix): destination.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/destination$(ObjectSuffix) -MF$(IntermediateDirectory)/destination$(DependSuffix) -MM "destination.c"

$(IntermediateDirectory)/destination$(PreprocessSuffix): destination.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/destination$(PreprocessSuffix) "destination.c"

$(IntermediateDirectory)/webserv$(ObjectSuffix): webserv.c $(IntermediateDirectory)/webserv$(DependSuffix)
	$(CC) $(SourceSwitch) "webserv.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/webserv$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/webserv$(DependSuffix): webserv.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/webserv$(ObjectSuffix) -MF$(IntermediateDirectory)/webserv$(DependSuffix) -MM "webserv.c"

$(IntermediateDirectory)/webserv$(PreprocessSuffix): webserv.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/webserv$(PreprocessSuffix) "webserv.c"

$(IntermediateDirectory)/database$(ObjectSuffix): database.c $(IntermediateDirectory)/database$(DependSuffix)
	$(CC) $(SourceSwitch) "database.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/database$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/database$(DependSuffix): database.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/database$(ObjectSuffix) -MF$(IntermediateDirectory)/database$(DependSuffix) -MM "database.c"

$(IntermediateDirectory)/database$(PreprocessSuffix): database.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/database$(PreprocessSuffix) "database.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/linkedlist$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/linkedlist$(DependSuffix)
	$(RM) $(IntermediateDirectory)/linkedlist$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/socketserver$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/socketserver$(DependSuffix)
	$(RM) $(IntermediateDirectory)/socketserver$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/telnetservermanager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/telnetservermanager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/telnetservermanager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/utils$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/utils$(DependSuffix)
	$(RM) $(IntermediateDirectory)/utils$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/configuration$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/configuration$(DependSuffix)
	$(RM) $(IntermediateDirectory)/configuration$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/dictionary$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/dictionary$(DependSuffix)
	$(RM) $(IntermediateDirectory)/dictionary$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/iniparser$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/iniparser$(DependSuffix)
	$(RM) $(IntermediateDirectory)/iniparser$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/sqlite3$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/sqlite3$(DependSuffix)
	$(RM) $(IntermediateDirectory)/sqlite3$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/dvbhardware$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/dvbhardware$(DependSuffix)
	$(RM) $(IntermediateDirectory)/dvbhardware$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/selectmanager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/selectmanager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/selectmanager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/channels$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/channels$(DependSuffix)
	$(RM) $(IntermediateDirectory)/channels$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/session$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/session$(DependSuffix)
	$(RM) $(IntermediateDirectory)/session$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/destination$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/destination$(DependSuffix)
	$(RM) $(IntermediateDirectory)/destination$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/webserv$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/webserv$(DependSuffix)
	$(RM) $(IntermediateDirectory)/webserv$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/database$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/database$(DependSuffix)
	$(RM) $(IntermediateDirectory)/database$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "../.build-debug/dvbServe"


