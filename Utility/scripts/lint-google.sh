#!/bin/bash

# use the Google linter cpplint without these filters:
# - header_guard: we use pragma once
# - include_what_you_use: we have global includes for map, string, vector
# - legal: we have no copyright notice
# - braces/newline: we use a different brace style (Linux) than google (Attach)
# - indent: we do not use a 2 spaces indent
which cpplint.py &> /dev/null || (echo "cpplint.py not available" && exit 1)

find $1 -name "*\.cc" -or -name "*\.h" | xargs cpplint.py --linelength=100 \
  --filter=-build/header_guard,-build/include_what_you_use,-legal,-readability/braces,-runtime/int,-runtime/references,-whitespace/braces,-whitespace/indent,-whitespace/newline,-whitespace

