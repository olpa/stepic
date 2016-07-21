cmake_minimum_required (VERSION 2.6)
add_definitions(-std=c++11)
project (final)
add_executable(final httpserver.cpp)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
TARGET_LINK_LIBRARIES(final
  pthread
)
