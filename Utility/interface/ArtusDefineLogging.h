#pragma once

/*
 * This includes setups the artus logging system for your analysis
 * Include it only once in your project, preferably in the .cc file which
 * also contains your main () functon.
 *
 * In all places you want to use the logging output, include "ArtusLogging.h"
 */


#include "Artus/Utility/interface/ArtusEasyLoggingDecl.h"

SHARE_EASYLOGGINGPP(ArtusEasyLoggingDecl::getEasyloggingStorage())

