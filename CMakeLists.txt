cmake_minimum_required (VERSION 2.6)
project (Artus)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wfatal-errors -std=c++0x -fpic -Wall -O2 -g")

# to compile with clang, use: cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang

# Load some basic macros which are needed later on
include(FindROOT.cmake)

# commented for now, if you will need this type of compile environment ( afs-machine + cmake compile )
# please take care of making this check safe also on non-afs systems

if (DEFINED ENV{CMSSW_RELEASE_BASE} )
	set(USE_BOOST_CMSSW TRUE)
	execute_process(COMMAND ls $ENV{CMSSW_RELEASE_BASE}/../../../../$ENV{SCRAM_ARCH}/external/boost COMMAND tail -n 1 COMMAND tr -d '\n' OUTPUT_VARIABLE BOOSTVER)
	set(BOOST_INCLUDE_DIR "/afs/cern.ch/cms/$ENV{SCRAM_ARCH}/external/boost/${BOOSTVER}/include/")
	set(BOOST_LIB_DIR "/afs/cern.ch/cms/$ENV{SCRAM_ARCH}/external/boost/${BOOSTVER}/lib/")
	message("Looking for boost in CMSSW ... - found ${BOOST_INCLUDE_DIR}")
	message("Looking for boost in CMSSW ... - found ${BOOST_LIB_DIR}")
else()
	set(USE_BOOST_CMSSW FALSE)
	set(BOOST_INCLUDE_DIR "")
	set(BOOST_LIB_DIR "")
	message("Expecting boost to be provided by system")
endif()

set(INCLUDE_DIRECTORIES
	${ROOT_INCLUDE_DIR}
	${BOOST_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories( ${closure_SOURCE_DIR}../ )

add_library(artus_core SHARED
	Core/src/CutFlow.cc
	Core/src/FilterResult.cc
	Core/src/Mutation.cc
	Core/src/ProgressReport.cc
)

add_library(artus_configuration SHARED
	Configuration/src/ArtusConfig.cc
	Configuration/src/PropertyTreeSupport.cc
	Configuration/src/RootEnvironment.cc
	Configuration/src/SettingsBase.cc
)

if ( USE_BOOST_CMSSW )
	target_link_libraries( artus_configuration
			-L${BOOST_LIB_DIR 	
	        boost_program_options)
else()
	target_link_libraries( artus_configuration 	
	        boost_program_options)
endif()

add_library(artus_consumer SHARED
	Consumer/src/Hist1D.cc
	Consumer/src/Profile2D.cc
	Consumer/src/ValueModifier.cc
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

add_executable(artus_core_test
	Test/test/Artus_t.cc	
)

target_link_libraries( artus_core_test 	
	artus_core	
	artus_configuration
	artus_utility
	${ROOT_LIBRARIES})

# use a capitaal *E*xample here and no underscore
# to be compatible of how the binary is named in the CMSSW build
add_executable(artusExample
	Example/bin/ArtusExample.cc
	Example/src/MeanPtConsumer.cc
)

target_link_libraries( artusExample 	
	artus_core	
	artus_configuration
	artus_provider
	artus_consumer
	artus_utility
	${ROOT_LIBRARIES})

