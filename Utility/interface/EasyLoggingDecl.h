#pragma once

#include <boost/noncopyable.hpp>

#include "Artus/Utility/interface/easylogging++.h"

class ArtusEasyLogging : boost::noncopyable {
public:
	ArtusEasyLogging();
	~ArtusEasyLogging();

    static el::base::type::StoragePointer getEasyloggingStorage();
private:
    static int runOnceHelper;
    static int runOnce();
};

