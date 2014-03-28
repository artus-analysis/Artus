
#include "Artus/Utility/interface/EasyLoggingDecl.h"

#include "Artus/Utility/interface/easylogging++.h"
_INITIALIZE_EASYLOGGINGPP

int ArtusEasyLogging::runOnceHelper = runOnce();

int ArtusEasyLogging::runOnce() {
    LOG(INFO) << "Registering logger [mylib]";
    el::Loggers::getLogger("mylib");
    return 0;
}

ArtusEasyLogging::ArtusEasyLogging() {
    LOG(INFO) << "---MyLib Constructor () ---";
}


ArtusEasyLogging::~ArtusEasyLogging() {
    LOG(INFO) << "---MyLib Destructor---";
}

el::base::type::StoragePointer ArtusEasyLogging::getEasyloggingStorage() {
        return el::Helpers::storage();
}
