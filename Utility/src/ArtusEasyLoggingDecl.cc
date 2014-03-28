
#include "Artus/Utility/interface/ArtusEasyLoggingDecl.h"

#include "Artus/Utility/interface/easylogging++.h"
_INITIALIZE_EASYLOGGINGPP

int ArtusEasyLoggingDecl::runOnceHelper = runOnce();

int ArtusEasyLoggingDecl::runOnce() {
    el::Loggers::getLogger("artus");
    return 0;
}

ArtusEasyLoggingDecl::ArtusEasyLoggingDecl() {
}


ArtusEasyLoggingDecl::~ArtusEasyLoggingDecl() {
}

el::base::type::StoragePointer ArtusEasyLoggingDecl::getEasyloggingStorage() {
        return el::Helpers::storage();
}
