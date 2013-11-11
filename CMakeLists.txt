cmake_minimum_required (VERSION 2.6)
project (Artus)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wfatal-errors -std=c++0x -Wall -O2 -g")

# to compile with clang, use: cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang

# Load some basic macros which are needed later on
include(FindROOT.cmake)

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
/afs/cern.ch/cms/slc6_amd64_gcc472/external/boost/1.51.0-cms5/include/ # BOOST: example paths (use one of those)
/afs/cern.ch/cms/slc5_amd64_gcc462/external/boost/1.51.0/include/
/afs/cern.ch/cms/slc5_amd64_gcc462/external/boost/1.49.0-cms2/include/
/wlcg/sw/boost/current/include/
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

include_directories( ${closure_SOURCE_DIR}../ )

add_library(artus_core STATIC
	Core/src/ArtusCore.cc
)

add_library(artus_configuration STATIC
	Configuration/src/ArtusConfig.cc
)

add_library(artus_consumer STATIC
	Consumer/src/Hist1D.cc
	Consumer/src/ValueModifier.cc
)

add_library(artus_provider STATIC
	Provider/src/ArtusProvider.cc
)

add_executable(artus_core_test
	Core/test/Artus_t.cc
)

add_executable(artus_example
	Example/bin/ArtusExample.cc
)

target_link_libraries( artus_example 	
	artus_core	
	artus_configuration
	artus_provider
	artus_consumer
	${ROOT_LIBRARIES})

