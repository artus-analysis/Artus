# Code checks

The code is compiled using [gcc warnings](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html "Options to Request or Suppress Warnings"): `-Wall -Wextra -Wpedantic` and some more using [cmake](https://github.com/artus-analysis/Artus/blob/master/CMakeLists.txt#L4) and [scram](https://github.com/artus-analysis/Artus/blob/master/Core/BuildFile.xml#L1). Some parts of external code must be excluded because we can not change these parts. This comprises:

- external code copied into the repository is ignored using `#pragma GCC system_header` as done in [easylogging](https://github.com/artus-analysis/Artus/blob/master/Utility/interface/easylogging%2B%2B.h#L20)
- system libraries like boost must be [handed over](http://stackoverflow.com/questions/1867065/how-to-suppress-gcc-warnings-from-library-headers "Stack overflow") to the compiler with `-isystem` instead of `-I`. This is done via the `SYSTEM` [keyword](https://github.com/artus-analysis/Artus/blob/master/CMakeLists.txt#L35) in cmake.

More warnings could be added once they are understood and existing occurances are fixed.

In addition to these quality checks during compile time, static code analysis (linting) can be done via these scripts:


- **own regex based checks** to spot most common issues. This is directly implemented in `lint.sh`. Additional checks can be added.
*Usage*: `lint.sh -r <folder>` or `lint.sh <file>`
- **google linter** (cpplint): Uses cpplint from google. This checks for the google style guide. As long as we do not agree with all points (in particular code formatting), some checks are disabled. 
*Usage*: `lint-google.sh <folder>`
- **cppcheck**: This is another linter tool available on Linux.
*Usage*: `lint-cppcheck.sh <folder>`

All these tools show false positives because the analyser does not know the full context in which the code is compiled and used. Therefore, not all warnings can or should be solved. But nonetheless they give some handle where code quality can be improved.