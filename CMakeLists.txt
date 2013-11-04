cmake_minimum_required (VERSION 2.6)
project (Artus)

set (CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -pedantic -Wfatal-errors -std=c++0x -Wall -O2 -g -fPIC  -DSTANDALONE")

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

add_executable(artus_example
	Example/bin/ArtusExample.cpp
)

#target_link_libraries( closure_test boost )

target_link_libraries( artus_example ${ROOT_LIBRARIES})


