set(PROJECT_ROOT_DIR "${CMAKE_ARGV3}")
set(PATCH_SOURCE_DIR "${PROJECT_ROOT_DIR}/external/patches/protobuf-cpp-3.1.0.patches")

message(STATUS "Patching protobuf...")

execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PATCH_SOURCE_DIR}/cmake/CMakeLists.txt cmake/CMakeLists.txt)
