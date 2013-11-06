cmake_minimum_required (VERSION 2.6)
project (Artus)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wfatal-errors -std=c++0x -Wall -O2 -g")

# to compile with clang, use: cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang

# Load some basic macros which are needed later on
include(FindROOT.cmake)

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
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

add_library(artus_draw STATIC
	Draw/src/Draw.cc
	Draw/src/Hist1D.cc
	Draw/src/ValueModifier.cc
)

add_library(artus_provider STATIC
	Provider/src/ArtusProvider.cc
)

add_executable(artus_example
	Example/bin/ArtusExample.cc
)

#target_link_libraries( artus_configuration boost_regex )

target_link_libraries( artus_example 	
	artus_core
	artus_configuration
	artus_provider
	artus_draw
	${ROOT_LIBRARIES})

