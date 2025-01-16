# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Komunikator_internetowy_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Komunikator_internetowy_autogen.dir/ParseCache.txt"
  "Komunikator_internetowy_autogen"
  )
endif()
