/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

// compiling for x86, most recent gcc 4.7 used

#define USE_CPP11


#ifdef USE_CPP11

#define ARTUS_CPP11_FINAL		final

#else

#define ARTUS_CPP11_FINAL

#endif
