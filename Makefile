##
## Makefile
##
ProjectName            :=iio
ConfigurationName      :=Release
IntermediateDirectory  :=./bin
OutDir                 := $(IntermediateDirectory)
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="iio.txt"
MakeDirCommand         :=mkdir -p
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar crs
CXX      := g++
CC       := gcc
CXXFLAGS :=  -O3 -fexpensive-optimizations -std=c++14 -Wall -Wno-sign-compare -Wno-reorder -Wno-deprecated-declarations -m64 -fPIC -fno-strict-aliasing -fexceptions -fno-signed-zeros -fno-trapping-math -frename-registers -funroll-loops -DSKIPTHETA0 -DLAZYCLUST -DREDINST -DINSVS -DCOUTRLOG $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
Objects0=$(IntermediateDirectory)/src_TSimplex.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TpInstance.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_MyLog.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TPHeuristics.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_optcfg.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_optresult.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Shielding.cpp$(ObjectSuffix) 



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
	@test -d ./bin || $(MakeDirCommand) ./bin


$(IntermediateDirectory)/.d:
	@test -d ./bin || $(MakeDirCommand) ./bin

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_TSimplex.cpp$(ObjectSuffix): src/TSimplex.cpp $(IntermediateDirectory)/src_TSimplex.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/TSimplex.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TSimplex.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TSimplex.cpp$(DependSuffix): src/TSimplex.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TSimplex.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TSimplex.cpp$(DependSuffix) -MM src/TSimplex.cpp

$(IntermediateDirectory)/src_TSimplex.cpp$(PreprocessSuffix): src/TSimplex.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TSimplex.cpp$(PreprocessSuffix) src/TSimplex.cpp

$(IntermediateDirectory)/src_TpInstance.cpp$(ObjectSuffix): src/TpInstance.cpp $(IntermediateDirectory)/src_TpInstance.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/TpInstance.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TpInstance.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TpInstance.cpp$(DependSuffix): src/TpInstance.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TpInstance.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TpInstance.cpp$(DependSuffix) -MM src/TpInstance.cpp

$(IntermediateDirectory)/src_TpInstance.cpp$(PreprocessSuffix): src/TpInstance.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TpInstance.cpp$(PreprocessSuffix) src/TpInstance.cpp

$(IntermediateDirectory)/src_MyLog.cpp$(ObjectSuffix): src/MyLog.cpp $(IntermediateDirectory)/src_MyLog.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/MyLog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_MyLog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MyLog.cpp$(DependSuffix): src/MyLog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MyLog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MyLog.cpp$(DependSuffix) -MM src/MyLog.cpp

$(IntermediateDirectory)/src_MyLog.cpp$(PreprocessSuffix): src/MyLog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MyLog.cpp$(PreprocessSuffix) src/MyLog.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp

$(IntermediateDirectory)/src_TPHeuristics.cpp$(ObjectSuffix): src/TPHeuristics.cpp $(IntermediateDirectory)/src_TPHeuristics.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/TPHeuristics.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TPHeuristics.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TPHeuristics.cpp$(DependSuffix): src/TPHeuristics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TPHeuristics.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TPHeuristics.cpp$(DependSuffix) -MM src/TPHeuristics.cpp

$(IntermediateDirectory)/src_TPHeuristics.cpp$(PreprocessSuffix): src/TPHeuristics.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TPHeuristics.cpp$(PreprocessSuffix) src/TPHeuristics.cpp

$(IntermediateDirectory)/src_optcfg.cpp$(ObjectSuffix): src/optcfg.cpp $(IntermediateDirectory)/src_optcfg.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/optcfg.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_optcfg.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_optcfg.cpp$(DependSuffix): src/optcfg.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_optcfg.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_optcfg.cpp$(DependSuffix) -MM src/optcfg.cpp

$(IntermediateDirectory)/src_optcfg.cpp$(PreprocessSuffix): src/optcfg.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_optcfg.cpp$(PreprocessSuffix) src/optcfg.cpp

$(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(ObjectSuffix): src/SpanningTreeImpl.cpp $(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/SpanningTreeImpl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(DependSuffix): src/SpanningTreeImpl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(DependSuffix) -MM src/SpanningTreeImpl.cpp

$(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(PreprocessSuffix): src/SpanningTreeImpl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_SpanningTreeImpl.cpp$(PreprocessSuffix) src/SpanningTreeImpl.cpp

$(IntermediateDirectory)/src_optresult.cpp$(ObjectSuffix): src/optresult.cpp $(IntermediateDirectory)/src_optresult.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/optresult.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_optresult.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_optresult.cpp$(DependSuffix): src/optresult.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_optresult.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_optresult.cpp$(DependSuffix) -MM src/optresult.cpp

$(IntermediateDirectory)/src_optresult.cpp$(PreprocessSuffix): src/optresult.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_optresult.cpp$(PreprocessSuffix) src/optresult.cpp

$(IntermediateDirectory)/src_Shielding.cpp$(ObjectSuffix): src/Shielding.cpp $(IntermediateDirectory)/src_Shielding.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "src/Shielding.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Shielding.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Shielding.cpp$(DependSuffix): src/Shielding.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Shielding.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Shielding.cpp$(DependSuffix) -MM src/Shielding.cpp

$(IntermediateDirectory)/src_Shielding.cpp$(PreprocessSuffix): src/Shielding.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Shielding.cpp$(PreprocessSuffix) src/Shielding.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./bin/


