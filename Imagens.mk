##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Imagens
ConfigurationName      :=Debug
WorkspacePath          :=/Users/Pinho/Documents/CG/OpenGL/MediaClass/ImageClass
ProjectPath            :=/Users/Pinho/Documents/CG/OpenGL/MediaClass/ImageClass
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Marcio Sarroglia Pinho
Date                   :=30/09/2017
CodeLitePath           :="/Users/Pinho/Library/Application Support/codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -dynamiclib -fPIC
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
ObjectsFileList        :="Imagens.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -framework CoreFoundation -framework OpenGL -framework GLUT
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/Applications/codelite2.app/Contents/SharedSupport/
Objects0=$(IntermediateDirectory)/ImageClass.cpp$(ObjectSuffix) $(IntermediateDirectory)/GeracaoHistograma.cpp$(ObjectSuffix) $(IntermediateDirectory)/SOIL_image_DXT.cpp$(ObjectSuffix) $(IntermediateDirectory)/SOIL_image_helper.cpp$(ObjectSuffix) $(IntermediateDirectory)/SOIL_SOIL.cpp$(ObjectSuffix) $(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(ObjectSuffix) 



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
$(IntermediateDirectory)/ImageClass.cpp$(ObjectSuffix): ImageClass.cpp $(IntermediateDirectory)/ImageClass.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/Pinho/Documents/CG/OpenGL/MediaClass/ImageClass/ImageClass.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ImageClass.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ImageClass.cpp$(DependSuffix): ImageClass.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ImageClass.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ImageClass.cpp$(DependSuffix) -MM ImageClass.cpp

$(IntermediateDirectory)/ImageClass.cpp$(PreprocessSuffix): ImageClass.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ImageClass.cpp$(PreprocessSuffix) ImageClass.cpp

$(IntermediateDirectory)/GeracaoHistograma.cpp$(ObjectSuffix): GeracaoHistograma.cpp $(IntermediateDirectory)/GeracaoHistograma.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/Pinho/Documents/CG/OpenGL/MediaClass/ImageClass/GeracaoHistograma.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GeracaoHistograma.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GeracaoHistograma.cpp$(DependSuffix): GeracaoHistograma.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GeracaoHistograma.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GeracaoHistograma.cpp$(DependSuffix) -MM GeracaoHistograma.cpp

$(IntermediateDirectory)/GeracaoHistograma.cpp$(PreprocessSuffix): GeracaoHistograma.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GeracaoHistograma.cpp$(PreprocessSuffix) GeracaoHistograma.cpp

$(IntermediateDirectory)/SOIL_image_DXT.cpp$(ObjectSuffix): SOIL/image_DXT.cpp $(IntermediateDirectory)/SOIL_image_DXT.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/Pinho/Documents/CG/OpenGL/MediaClass/ImageClass/SOIL/image_DXT.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SOIL_image_DXT.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SOIL_image_DXT.cpp$(DependSuffix): SOIL/image_DXT.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SOIL_image_DXT.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SOIL_image_DXT.cpp$(DependSuffix) -MM SOIL/image_DXT.cpp

$(IntermediateDirectory)/SOIL_image_DXT.cpp$(PreprocessSuffix): SOIL/image_DXT.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SOIL_image_DXT.cpp$(PreprocessSuffix) SOIL/image_DXT.cpp

$(IntermediateDirectory)/SOIL_image_helper.cpp$(ObjectSuffix): SOIL/image_helper.cpp $(IntermediateDirectory)/SOIL_image_helper.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/Pinho/Documents/CG/OpenGL/MediaClass/ImageClass/SOIL/image_helper.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SOIL_image_helper.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SOIL_image_helper.cpp$(DependSuffix): SOIL/image_helper.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SOIL_image_helper.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SOIL_image_helper.cpp$(DependSuffix) -MM SOIL/image_helper.cpp

$(IntermediateDirectory)/SOIL_image_helper.cpp$(PreprocessSuffix): SOIL/image_helper.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SOIL_image_helper.cpp$(PreprocessSuffix) SOIL/image_helper.cpp

$(IntermediateDirectory)/SOIL_SOIL.cpp$(ObjectSuffix): SOIL/SOIL.cpp $(IntermediateDirectory)/SOIL_SOIL.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/Pinho/Documents/CG/OpenGL/MediaClass/ImageClass/SOIL/SOIL.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SOIL_SOIL.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SOIL_SOIL.cpp$(DependSuffix): SOIL/SOIL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SOIL_SOIL.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SOIL_SOIL.cpp$(DependSuffix) -MM SOIL/SOIL.cpp

$(IntermediateDirectory)/SOIL_SOIL.cpp$(PreprocessSuffix): SOIL/SOIL.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SOIL_SOIL.cpp$(PreprocessSuffix) SOIL/SOIL.cpp

$(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(ObjectSuffix): SOIL/stb_image_aug.cpp $(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/Pinho/Documents/CG/OpenGL/MediaClass/ImageClass/SOIL/stb_image_aug.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(DependSuffix): SOIL/stb_image_aug.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(DependSuffix) -MM SOIL/stb_image_aug.cpp

$(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(PreprocessSuffix): SOIL/stb_image_aug.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SOIL_stb_image_aug.cpp$(PreprocessSuffix) SOIL/stb_image_aug.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


