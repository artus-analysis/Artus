
#define _ELPP_NO_DEFAULT_LOG_FILE
#define _ELPP_DISABLE_VERBOSE_LOGS
#define _ELPP_DISABLE_TRACE_LOGS

#include "Artus/Utility/interface/ArtusEasyLoggingDecl.h"
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
