project(TestIdl LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.5)

find_package(Idlpp-cxx REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../../../..")
if (NOT TARGET CycloneDDS-CXX::ddscxx)
  find_package(CycloneDDS-CXX REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../../../..")
endif()

# Convenience function, provided by the Idlpp-cxx that generates a CMake
# target for the given IDL file. The function calls Idlcpp-cxx to generate
# source files and compiles them into a library.
idl_ddscxx_generate(ddscxxTestCommand_lib "TestCommand.idl")

add_executable(ddscxxTestCommandPublisher publisher.cpp)
add_executable(ddscxxTestCommandSubscriber subscriber.cpp)

# Link both executables to idl data type library and ddscxx.
target_link_libraries(ddscxxTestCommandPublisher ddscxxTestCommand_lib CycloneDDS-CXX::ddscxx)
target_link_libraries(ddscxxTestCommandSubscriber ddscxxTestCommand_lib CycloneDDS-CXX::ddscxx)

set_property(TARGET ddscxxTestCommandPublisher PROPERTY CXX_STANDARD 11)
set_property(TARGET ddscxxTestCommandSubscriber PROPERTY CXX_STANDARD 11)
