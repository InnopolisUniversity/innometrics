file(REMOVE_RECURSE
  "runUnitTests.pdb"
  "runUnitTests"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/runUnitTests.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
