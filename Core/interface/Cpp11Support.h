#pragma once

// compiling for x86, most recent gcc 4.7 used

#define USE_CPP11


#ifdef USE_CPP11

#define ARTUS_CPP11_OVERRIDE	override
#define ARTUS_CPP11_FINAL		final

#else

#define ARTUS_CPP11_OVERRIDE
#define ARTUS_CPP11_FINAL

#endif
