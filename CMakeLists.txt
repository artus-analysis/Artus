cmake_minimum_required (VERSION 2.6)
project (Artus)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wfatal-errors -std=c++0x -fpic -Wall -O2 -g")

# to compile with clang, use: cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang

# Load some basic macros which are needed later on
include(FindROOT.cmake)
execute_process(COMMAND ls $ENV{CMSSW_RELEASE_BASE}/../../../../$ENV{SCRAM_ARCH}/external/boost COMMAND tail -n 1 COMMAND tr -d '\n' OUTPUT_VARIABLE BOOSTVER)
set(BOOST_INCLUDE_DIR "/afs/cern.ch/cms/$ENV{SCRAM_ARCH}/external/boost/${BOOSTVER}/include/")
message("Looking for boost... - found ${BOOST_INCLUDE_DIR}")

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BOOST_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

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

target_link_libraries( artus_configuration 	
        boost_program_options)

add_library(artus_consumer SHARED
	Consumer/src/Hist1D.cc
	Consumer/src/Profile2D.cc
	Consumer/src/ValueModifier.cc
)

add_library(artus_filter SHARED
	Provider/src/ArtusFilter.cc
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
)

target_link_libraries( artusExample 	
	artus_core	
	artus_configuration
	artus_provider
	artus_consumer
	artus_utility
	${ROOT_LIBRARIES})

