include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/c9461a9b55ba954df0489bab6420eb297bed846b.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Avoid installing GoogleTest when installing this project.
option(INSTALL_GTEST "Enable installation of googletest." OFF)

FetchContent_MakeAvailable(googletest)

enable_testing()

# Main test executable.
add_executable(
    libtest 
    src/json.cpp
    src/file.cpp
)

target_link_libraries(
    libtest
    gtest_main
    gmock_main 
    millijson
)

FetchContent_Declare(
    byteme 
    GIT_REPOSITORY https://github.com/LTLA/byteme
    GIT_TAG master
)

target_link_libraries(
    libtest
    byteme
)

target_compile_options(libtest PRIVATE -Wall -Wextra -Wpedantic -Werror)

FetchContent_MakeAvailable(byteme)
set(CODE_COVERAGE OFF CACHE BOOL "Enable coverage testing")
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(libtest PRIVATE -O0 -g --coverage)
    target_link_options(libtest PRIVATE --coverage)
endif()

include(GoogleTest)
gtest_discover_tests(libtest)
