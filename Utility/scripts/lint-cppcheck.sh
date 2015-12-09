#!/bin/bash

# use cppcheck if installed
# all _ELPP* and __* lines are necessary because of the ugly macros of easylogging++
# does not check ROOT or system headers, will complain about missing headers in the end
echo "Usage: lint-cppcheck folder"
which cppcheck &> /dev/null || (echo "cppcheck not installed" && exit 1)

cppcheck --platform=unix64 --std=c++11 --language=c++ --enable=all \
-I ../CondFormats \
--suppress=missingIncludeSystem \
--suppress=*:Artus/Utility/interface/easylogging++.h \
--template="+{line} {file} ({severity}) {id}: {message}" \
-U_ELPP_AS_DLL \
-U_ELPP_BOOST_LOGGING \
-U_ELPP_COMPILER_MSVC \
-U_ELPP_DEFAULT_LOG_FILE \
-U_ELPP_DISABLE_ASSERT \
-U_ELPP_DISABLE_CONFIGURATION_FROM_PROGRAM_ARGS \
-U_ELPP_DISABLE_CUSTOM_FORMAT_SPECIFIERS \
-U_ELPP_DISABLE_DEFAULT_CRASH_HANDLING \
-U_ELPP_DISABLE_LOGGING_FLAGS_FROM_ARG \
-U_ELPP_DISABLE_LOGS \
-U_ELPP_DISABLE_LOG_FILE_FROM_ARG \
-U_ELPP_DISABLE_PERFORMANCE_TRACKING \
-U_ELPP_DISABLE_VMODULES \
-U_ELPP_DISABLE_VMODULES_EXTENSION \
-U_ELPP_ENABLE_ERRORS \
-U_ELPP_ENABLE_INFO \
-U_ELPP_HANDLE_POST_LOG_DISPATCH \
-U_ELPP_HANDLE_POST_PERFORMANCE_TRACKING \
-U_ELPP_HANDLE_SIGABRT \
-U_ELPP_LOG_STD_ARRAY \
-U_ELPP_LOG_UNORDERED_MAP \
-U_ELPP_STL_LOGGING \
-U_ELPP_MULTI_LOGGER_SUPPORT \
-U_ELPP_PERFORMANCE_DISABLE_COMPARE_CHECKPOINTS \
-U_ELPP_PERFORMANCE_MICROSECONDS \
-U_ELPP_QT_LOGGING \
-U_ELPP_STOP_ON_FIRST_ASSERTION \
-U_ELPP_STRICT_SIZE_CHECK \
-U_ELPP_SYSLOG \
-U_ELPP_THREAD_SAFE \
-U_ELPP_UNICODE \
-U_ELPP_WXWIDGETS_LOGGING \
-U_LOGGER \
-U_MSC_VER \
-U_MSC_VER=1600 \
-U_PERFORMANCE_LOGGER \
-U__ANDROID__ \
-U__APPLE__ \
-U__CYGWIN__ \
-U__FreeBSD__ \
-U__GNUC__ \
-U__GXX_EXPERIMENTAL_CXX0X__ \
-U__INTEL_COMPILER \
-U__clang__ \
-UKAPPA_STANDALONE_DICT \
-DM_PI=3.1415 \
--force \
$1
