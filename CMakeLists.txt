cmake_minimum_required (VERSION 2.6)
project (Artus)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wfatal-errors -std=c++0x -fpic -Wall -O2 -g -Wno-unused-local-typedefs -DSTANDALONE")

# to compile with clang, use: cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang

# Load some basic macros which are needed later on
include(FindROOT.cmake)

if (DEFINED ENV{CMSSW_RELEASE_BASE} )
	set(USE_BOOST_CMSSW TRUE)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTANDALONE")
	execute_process(COMMAND ls $ENV{CMSSW_RELEASE_BASE}/../../../../$ENV{SCRAM_ARCH}/external/boost COMMAND tail -n 1 COMMAND tr -d '\n' OUTPUT_VARIABLE BOOSTVER)
	set(BOOST_INCLUDE_DIR "/afs/cern.ch/cms/$ENV{SCRAM_ARCH}/external/boost/${BOOSTVER}/include/")
	set(BOOST_LIB_DIR "/afs/cern.ch/cms/$ENV{SCRAM_ARCH}/external/boost/${BOOSTVER}/lib/")
	set(CORRECTION_DIR "..")
	message(STATUS "Looking for boost ${BOOSTVER} inc in CMSSW: found ${BOOST_INCLUDE_DIR}")
	message(STATUS "Looking for boost ${BOOSTVER} lib in CMSSW: found ${BOOST_LIB_DIR}")
else()
	set(USE_BOOST_CMSSW FALSE)
	set(BOOST_INCLUDE_DIR "")
	set(BOOST_LIB_DIR "")
	set(CORRECTION_DIR "")
	message("Expecting boost to be provided by system")
endif()

set(INCLUDE_DIRECTORIES
	${ROOT_INCLUDE_DIR}
	${BOOST_INCLUDE_DIR}
	${CORRECTION_DIR}
)

include_directories(SYSTEM ${INCLUDE_DIRECTORIES})
include_directories( ${closure_SOURCE_DIR}../ )

add_library(artus_core SHARED
	Core/src/CutFlow.cc
	Core/src/FilterResult.cc
	Core/src/Mutation.cc
	Core/src/ProgressReport.cc
	Core/src/OsSignalHandler.cc
)

add_library(artus_configuration SHARED
	Configuration/src/ArtusConfig.cc
	Configuration/src/PropertyTreeSupport.cc
	Configuration/src/RootEnvironment.cc
	Configuration/src/SettingsBase.cc
)

if ( USE_BOOST_CMSSW )
	target_link_libraries( artus_configuration
			-L${BOOST_LIB_DIR}
	        boost_program_options)
else()
	target_link_libraries(artus_configuration
	boost_program_options)
endif()

add_library(artus_consumer SHARED
	Consumer/src/Hist1D.cc
	Consumer/src/Profile2D.cc
	Consumer/src/ValueModifier.cc
	Consumer/src/LambdaNtupleConsumer.cc
)

add_library(artus_filter SHARED
	Filter/src/ArtusFilter.cc
)

add_library(artus_provider SHARED
	Provider/src/ArtusProvider.cc
)

add_library(artus_utility SHARED
	Utility/src/RootFileHelper.cc
	Utility/src/Utility.cc
	Utility/src/ArtusEasyLoggingDecl.cc
	Utility/src/DefaultValues.cc
	Utility/src/CutRange.cc
)

if(EXISTS "../CondFormats")
	message(STATUS "Looking for offline jet corrections: found ../CondFormats")
set_source_files_properties(
	../CondFormats/JetMETObjects/src/FactorizedJetCorrector.cc
	../CondFormats/JetMETObjects/src/JetCorrectionUncertainty.cc
	../CondFormats/JetMETObjects/src/JetCorrectorParameters.cc
	../CondFormats/JetMETObjects/src/JetResolution.cc
	../CondFormats/JetMETObjects/src/SimpleJetCorrectionUncertainty.cc
	../CondFormats/JetMETObjects/src/SimpleJetCorrector.cc
	PROPERTIES COMPILE_FLAGS "-Wno-conversion -Wno-float-equal -Wno-zero-as-null-pointer-constant -Wno-unsafe-loop-optimizations -Wno-double-promotion -Wno-useless-cast -Wno-shadow"
)
add_library(artus_externalcorr SHARED
	../CondFormats/JetMETObjects/src/FactorizedJetCorrector.cc
	../CondFormats/JetMETObjects/src/JetCorrectionUncertainty.cc
	../CondFormats/JetMETObjects/src/JetCorrectorParameters.cc
	../CondFormats/JetMETObjects/src/JetResolution.cc
	../CondFormats/JetMETObjects/src/SimpleJetCorrectionUncertainty.cc
	../CondFormats/JetMETObjects/src/SimpleJetCorrector.cc
)
else()
	message(STATUS "Looking for offline corrections: not found and not compiled.")
endif()

if(EXISTS "../Kappa")
	message(STATUS "Looking for Kappa: found ../Kappa")
	FILE(GLOB KappaAnalysisFiles KappaAnalysis/src/KappaFactory.cc KappaAnalysis/src/*/*.cc)
	add_library(artus_kappaanalysis SHARED
		${KappaAnalysisFiles}
	#Consumer/src/LambdaNtupleConsumer.cc
	#KappaAnalysis/src/KappaFactory.cc
	#KappaAnalysis/src/Consumers/KappaCollectionsConsumers.cc
	#KappaAnalysis/src/Consumers/KappaCutFlowHistogramConsumer.cc
	#KappaAnalysis/src/Consumers/KappaCutFlowTreeConsumer.cc
	#KappaAnalysis/src/Filters/BeamScrapingFilter.cc
	#KappaAnalysis/src/Filters/GenDiLeptonDecayModeFilter.cc
	#KappaAnalysis/src/Filters/GoodPrimaryVertexFilter.cc
	#KappaAnalysis/src/Filters/HCALNoiseFilter.cc
	#KappaAnalysis/src/Filters/HltFilter.cc
	#KappaAnalysis/src/Filters/JsonFilter.cc
	#KappaAnalysis/src/Filters/MaxObjectsCountFilters.cc
	#KappaAnalysis/src/Filters/MinObjectsCountFilters.cc
	#KappaAnalysis/src/Filters/ObjectsCountFilters.cc
	#KappaAnalysis/src/Filters/ObjectsLowerPtCutFilters.cc
	#KappaAnalysis/src/Filters/ObjectsUpperAbsEtaCutFilters.cc
	#KappaAnalysis/src/Filters/RunLumiEventFilter.cc
	#KappaAnalysis/src/Filters/TriggerMatchingFilters.cc
	#KappaAnalysis/src/Filters/ValidObjectsFilters.cc
	#KappaAnalysis/src/Producers/CrossSectionWeightProducer.cc
	#KappaAnalysis/src/Producers/ElectronCorrectionsProducer.cc
	#KappaAnalysis/src/Producers/EmbeddingWeightProducer.cc
	#KappaAnalysis/src/Producers/EventWeightProducer.cc
	#KappaAnalysis/src/Producers/GenDiLeptonDecayModeProducer.cc
	#KappaAnalysis/src/Producers/GenParticleMatchingProducers.cc
	#KappaAnalysis/src/Producers/GenParticleProducer.cc
	#KappaAnalysis/src/Producers/GenPartonCounterProducer.cc
	#KappaAnalysis/src/Producers/GenTauDecayModeProducer.cc
	#KappaAnalysis/src/Producers/GenTauDecayProducer.cc
	#KappaAnalysis/src/Producers/GeneratorWeightProducer.cc
	#KappaAnalysis/src/Producers/HltProducer.cc
	#KappaAnalysis/src/Producers/JetCorrectionsProducer.cc
	#KappaAnalysis/src/Producers/MatchedLeptonsProducer.cc
	#KappaAnalysis/src/Producers/MuonCorrectionsProducer.cc
	#KappaAnalysis/src/Producers/NicknameProducer.cc
	#KappaAnalysis/src/Producers/NumberGeneratedEventsWeightProducer.cc
	#KappaAnalysis/src/Producers/PUWeightProducer.cc
	#KappaAnalysis/src/Producers/TauCorrectionsProducer.cc
	#KappaAnalysis/src/Producers/TmvaClassificationReaderBase.cc
	#KappaAnalysis/src/Producers/TriggerMatchingProducers.cc
	#KappaAnalysis/src/Producers/ValidBTaggedJetsProducer.cc
	#KappaAnalysis/src/Producers/ValidJetsProducer.cc
	#KappaAnalysis/src/Producers/ValidLeptonsProducer.cc
	#KappaAnalysis/src/Utility/BtagSF.cc
	)
else()
	message(STATUS "Looking for Kappa: not found and not compiled")
endif()

add_executable(artus_core_test
	Test/test/Artus_t.cc
)

target_link_libraries( artus_core_test
	artus_core
	artus_configuration
	artus_utility
	${ROOT_LIBRARIES}
)

# use a capital *E*xample here and no underscore
# to be compatible of how the binary is named in the CMSSW build
add_executable(artusExample
	Example/bin/ArtusExample.cc
	Example/src/MeanPtConsumer.cc
)

target_link_libraries(artusExample
	artus_core
	artus_configuration
	artus_provider
	artus_consumer
	artus_utility
	${ROOT_LIBRARIES}
)

