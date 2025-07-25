cmake_minimum_required(VERSION 3.16)
project(UserManagementAPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(Threads REQUIRED)

# Windows-specific settings
if(WIN32)
    # Use static runtime on Windows for easier deployment
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # Windows sockets
    set(ADDITIONAL_LIBS ws2_32)
else()
    set(ADDITIONAL_LIBS "")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# FetchContent for dependencies
include(FetchContent)

# Fetch cpp-httplib (header-only, cross-platform)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.3
)

# Fetch nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

# Fetch Catch2 for testing
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(httplib json Catch2)

# Main executable
add_executable(user_api src/main.cpp)
target_link_libraries(user_api 
    PRIVATE 
    Threads::Threads
    nlohmann_json::nlohmann_json
    httplib::httplib
    ${ADDITIONAL_LIBS}
)

# Test executable
add_executable(test_api tests/test_main.cpp)
target_link_libraries(test_api 
    PRIVATE 
    Catch2::Catch2WithMain
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# Enable testing
enable_testing()
add_test(NAME UserAPITests COMMAND test_api)