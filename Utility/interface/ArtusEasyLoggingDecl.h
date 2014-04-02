#pragma once

#include <boost/noncopyable.hpp>
#include "Artus/Utility/interface/easylogging++.h"

/*
 * Staticly creates the easy logging system. Don't use this
 * class directly, but include ArtusLogging.h
*/
class ArtusEasyLoggingDecl : boost::noncopyable {
public:
	ArtusEasyLoggingDecl();
	~ArtusEasyLoggingDecl();

    static el::base::type::StoragePointer getEasyloggingStorage();
private:
    static int runOnceHelper;
    static int runOnce();
};

