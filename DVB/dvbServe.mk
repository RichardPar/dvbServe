##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=dvbServe
ConfigurationName      :=Debug
WorkspacePath          :=/home/richard/.codelite/workspace/dvb
ProjectPath            :=/home/richard/Source/DVB
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Richard
Date                   :=08/12/18
CodeLitePath           :=/home/richard/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
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
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)/usr/include/glib-2.0 $(IncludeSwitch)/usr/lib/x86_64-linux-gnu/glib-2.0/include $(IncludeSwitch)/usr/include/gstreamer-0.10 $(IncludeSwitch)/usr/include/glib-2.0 $(IncludeSwitch)/usr/lib/x86_64-linux-gnu/glib-2.0/include $(IncludeSwitch)/usr/include/libxml2 $(IncludeSwitch)/usr/include/clutter-1.0 $(IncludeSwitch)/usr/include/pango-1.0 $(IncludeSwitch)/usr/include/cogl $(IncludeSwitch)/usr/include/cairo $(IncludeSwitch)/usr/include/atk-1.0 $(IncludeSwitch)/usr/include/json-glib-1.0 $(IncludeSwitch)/usr/include/gtk-3.0 $(IncludeSwitch)/usr/include/harfbuzz $(IncludeSwitch)/usr/include/freetype2 $(IncludeSwitch)/usr/include/glib-2.0 $(IncludeSwitch)/usr/lib/x86_64-linux-gnu/glib-2.0/include $(IncludeSwitch)/usr/include/gdk-pixbuf-2.0 $(IncludeSwitch)/usr/include/libdrm $(IncludeSwitch)/usr/include/libpng12 $(IncludeSwitch)/usr/include/pixman-1 $(IncludeSwitch)/usr/include/gio-unix-2.0/ 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)pthread $(LibrarySwitch)dl $(LibrarySwitch)dvbpsi $(LibrarySwitch)microhttpd 
ArLibs                 :=  "pthread" "dl" "dvbpsi" "microhttpd" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0  $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/imageConvert.c$(ObjectSuffix) $(IntermediateDirectory)/destination.c$(ObjectSuffix) $(IntermediateDirectory)/configuration.c$(ObjectSuffix) $(IntermediateDirectory)/utils.c$(ObjectSuffix) $(IntermediateDirectory)/session.c$(ObjectSuffix) $(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/linkedlist.c$(ObjectSuffix) $(IntermediateDirectory)/webserv.c$(ObjectSuffix) $(IntermediateDirectory)/socketserver.c$(ObjectSuffix) $(IntermediateDirectory)/dvbhardware.c$(ObjectSuffix) \
	$(IntermediateDirectory)/dictionary.c$(ObjectSuffix) $(IntermediateDirectory)/iniparser.c$(ObjectSuffix) $(IntermediateDirectory)/sqlite3.c$(ObjectSuffix) $(IntermediateDirectory)/database.c$(ObjectSuffix) $(IntermediateDirectory)/telnetservermanager.c$(ObjectSuffix) $(IntermediateDirectory)/channels.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/imageConvert.c$(ObjectSuffix): imageConvert.c $(IntermediateDirectory)/imageConvert.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/imageConvert.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/imageConvert.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/imageConvert.c$(DependSuffix): imageConvert.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/imageConvert.c$(ObjectSuffix) -MF$(IntermediateDirectory)/imageConvert.c$(DependSuffix) -MM imageConvert.c

$(IntermediateDirectory)/imageConvert.c$(PreprocessSuffix): imageConvert.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/imageConvert.c$(PreprocessSuffix) imageConvert.c

$(IntermediateDirectory)/destination.c$(ObjectSuffix): destination.c $(IntermediateDirectory)/destination.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/destination.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/destination.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/destination.c$(DependSuffix): destination.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/destination.c$(ObjectSuffix) -MF$(IntermediateDirectory)/destination.c$(DependSuffix) -MM destination.c

$(IntermediateDirectory)/destination.c$(PreprocessSuffix): destination.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/destination.c$(PreprocessSuffix) destination.c

$(IntermediateDirectory)/configuration.c$(ObjectSuffix): configuration.c $(IntermediateDirectory)/configuration.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/configuration.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/configuration.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/configuration.c$(DependSuffix): configuration.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/configuration.c$(ObjectSuffix) -MF$(IntermediateDirectory)/configuration.c$(DependSuffix) -MM configuration.c

$(IntermediateDirectory)/configuration.c$(PreprocessSuffix): configuration.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/configuration.c$(PreprocessSuffix) configuration.c

$(IntermediateDirectory)/utils.c$(ObjectSuffix): utils.c $(IntermediateDirectory)/utils.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/utils.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/utils.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/utils.c$(DependSuffix): utils.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/utils.c$(ObjectSuffix) -MF$(IntermediateDirectory)/utils.c$(DependSuffix) -MM utils.c

$(IntermediateDirectory)/utils.c$(PreprocessSuffix): utils.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/utils.c$(PreprocessSuffix) utils.c

$(IntermediateDirectory)/session.c$(ObjectSuffix): session.c $(IntermediateDirectory)/session.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/session.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/session.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/session.c$(DependSuffix): session.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/session.c$(ObjectSuffix) -MF$(IntermediateDirectory)/session.c$(DependSuffix) -MM session.c

$(IntermediateDirectory)/session.c$(PreprocessSuffix): session.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/session.c$(PreprocessSuffix) session.c

$(IntermediateDirectory)/main.c$(ObjectSuffix): main.c $(IntermediateDirectory)/main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/main.c$(DependSuffix) -MM main.c

$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) main.c

$(IntermediateDirectory)/linkedlist.c$(ObjectSuffix): linkedlist.c $(IntermediateDirectory)/linkedlist.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/linkedlist.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/linkedlist.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/linkedlist.c$(DependSuffix): linkedlist.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/linkedlist.c$(ObjectSuffix) -MF$(IntermediateDirectory)/linkedlist.c$(DependSuffix) -MM linkedlist.c

$(IntermediateDirectory)/linkedlist.c$(PreprocessSuffix): linkedlist.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/linkedlist.c$(PreprocessSuffix) linkedlist.c

$(IntermediateDirectory)/webserv.c$(ObjectSuffix): webserv.c $(IntermediateDirectory)/webserv.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/webserv.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/webserv.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/webserv.c$(DependSuffix): webserv.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/webserv.c$(ObjectSuffix) -MF$(IntermediateDirectory)/webserv.c$(DependSuffix) -MM webserv.c

$(IntermediateDirectory)/webserv.c$(PreprocessSuffix): webserv.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/webserv.c$(PreprocessSuffix) webserv.c

$(IntermediateDirectory)/socketserver.c$(ObjectSuffix): socketserver.c $(IntermediateDirectory)/socketserver.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/socketserver.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/socketserver.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/socketserver.c$(DependSuffix): socketserver.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/socketserver.c$(ObjectSuffix) -MF$(IntermediateDirectory)/socketserver.c$(DependSuffix) -MM socketserver.c

$(IntermediateDirectory)/socketserver.c$(PreprocessSuffix): socketserver.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/socketserver.c$(PreprocessSuffix) socketserver.c

$(IntermediateDirectory)/dvbhardware.c$(ObjectSuffix): dvbhardware.c $(IntermediateDirectory)/dvbhardware.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/dvbhardware.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dvbhardware.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dvbhardware.c$(DependSuffix): dvbhardware.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dvbhardware.c$(ObjectSuffix) -MF$(IntermediateDirectory)/dvbhardware.c$(DependSuffix) -MM dvbhardware.c

$(IntermediateDirectory)/dvbhardware.c$(PreprocessSuffix): dvbhardware.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dvbhardware.c$(PreprocessSuffix) dvbhardware.c

$(IntermediateDirectory)/dictionary.c$(ObjectSuffix): dictionary.c $(IntermediateDirectory)/dictionary.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/dictionary.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dictionary.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dictionary.c$(DependSuffix): dictionary.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dictionary.c$(ObjectSuffix) -MF$(IntermediateDirectory)/dictionary.c$(DependSuffix) -MM dictionary.c

$(IntermediateDirectory)/dictionary.c$(PreprocessSuffix): dictionary.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dictionary.c$(PreprocessSuffix) dictionary.c

$(IntermediateDirectory)/iniparser.c$(ObjectSuffix): iniparser.c $(IntermediateDirectory)/iniparser.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/iniparser.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/iniparser.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/iniparser.c$(DependSuffix): iniparser.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/iniparser.c$(ObjectSuffix) -MF$(IntermediateDirectory)/iniparser.c$(DependSuffix) -MM iniparser.c

$(IntermediateDirectory)/iniparser.c$(PreprocessSuffix): iniparser.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/iniparser.c$(PreprocessSuffix) iniparser.c

$(IntermediateDirectory)/sqlite3.c$(ObjectSuffix): sqlite3.c $(IntermediateDirectory)/sqlite3.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/sqlite3.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sqlite3.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sqlite3.c$(DependSuffix): sqlite3.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sqlite3.c$(ObjectSuffix) -MF$(IntermediateDirectory)/sqlite3.c$(DependSuffix) -MM sqlite3.c

$(IntermediateDirectory)/sqlite3.c$(PreprocessSuffix): sqlite3.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sqlite3.c$(PreprocessSuffix) sqlite3.c

$(IntermediateDirectory)/database.c$(ObjectSuffix): database.c $(IntermediateDirectory)/database.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/database.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/database.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/database.c$(DependSuffix): database.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/database.c$(ObjectSuffix) -MF$(IntermediateDirectory)/database.c$(DependSuffix) -MM database.c

$(IntermediateDirectory)/database.c$(PreprocessSuffix): database.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/database.c$(PreprocessSuffix) database.c

$(IntermediateDirectory)/telnetservermanager.c$(ObjectSuffix): telnetservermanager.c $(IntermediateDirectory)/telnetservermanager.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/telnetservermanager.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/telnetservermanager.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/telnetservermanager.c$(DependSuffix): telnetservermanager.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/telnetservermanager.c$(ObjectSuffix) -MF$(IntermediateDirectory)/telnetservermanager.c$(DependSuffix) -MM telnetservermanager.c

$(IntermediateDirectory)/telnetservermanager.c$(PreprocessSuffix): telnetservermanager.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/telnetservermanager.c$(PreprocessSuffix) telnetservermanager.c

$(IntermediateDirectory)/channels.c$(ObjectSuffix): channels.c $(IntermediateDirectory)/channels.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/richard/Source/DVB/channels.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/channels.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/channels.c$(DependSuffix): channels.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/channels.c$(ObjectSuffix) -MF$(IntermediateDirectory)/channels.c$(DependSuffix) -MM channels.c

$(IntermediateDirectory)/channels.c$(PreprocessSuffix): channels.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/channels.c$(PreprocessSuffix) channels.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


