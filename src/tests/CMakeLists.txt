include_directories(${CMAKE_SOURCE_DIR}/src/marsyas/core/)
include_directories(${CMAKE_SOURCE_DIR}/src/marsyas/marsystems/)
include_directories(${CMAKE_SOURCE_DIR}/src/marsyas/optional/)

add_subdirectory(unit_tests)
add_subdirectory(black_box)

