cmake_minimum_required(VERSION 3.14)
project(marisa)
ENABLE_TESTING()

set(CMAKE_CXX_STANDARD 17)

set(CPM_DOWNLOAD_VERSION 0.27.5)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
        NAME IODash
        GITHUB_REPOSITORY YukiWorkshop/IODash
        VERSION 0.1.7
)

CPMAddPackage(
        NAME ReGlob
        GITHUB_REPOSITORY SudoMaker/ReGlob
        VERSION 0.0.5
)

CPMAddPackage(
        NAME ThreadPool
        GITHUB_REPOSITORY progschj/ThreadPool
        VERSION 1.0.0
        GIT_TAG 9a42ec1329f259a5f4881a291db1dcb8f2ad9040
)

CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        VERSION 7.1.3
        GIT_TAG 7bdf0628b1276379886c7f6dda2cef2b3b374f0b
)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.8.2
)

CPMAddPackage(
        NAME cpp-base64X
        GITHUB_REPOSITORY YukiWorkshop/cpp-base64X
        VERSION 0.0.2
)

include(CMake/FindMHD.cmake)

file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp" "Source/*.hpp")

include_directories(${LIBMICROHTTPD_INCLUDE_DIR})
include_directories(${fmt_SOURCE_DIR}/include)
include_directories(${spdlog_SOURCE_DIR}/include)
include_directories(${ThreadPool_SOURCE_DIR})
include_directories(${cpp-base64X_SOURCE_DIR})

add_library(marisa ${SOURCE_FILES})
target_include_directories(marisa INTERFACE Source/)
target_link_libraries(marisa pthread z fmt cpp-base64X IODash ReGlob ${LIBMICROHTTPD_LIBRARIES})

add_executable(CoverageTest Tests/coverage.cpp)
target_link_libraries(CoverageTest marisa gcov)

add_test(NAME Coverage COMMAND CoverageTest)

add_executable(example_Hello Examples/hello.cpp)
target_link_libraries(example_Hello marisa)

add_executable(example_HelloStreamed Examples/hello_streamed.cpp)
target_link_libraries(example_HelloStreamed marisa)

add_executable(example_HelloSSL Examples/hello_ssl.cpp)
target_link_libraries(example_HelloSSL marisa)

add_executable(example_AsyncSuspendResume Examples/async_suspend_resume.cpp)
target_link_libraries(example_AsyncSuspendResume marisa)

add_executable(example_UsageDemo Examples/usage_demo.cpp)
target_link_libraries(example_UsageDemo marisa)

add_executable(example_Dashboard Examples/dashboard.cpp)
target_link_libraries(example_Dashboard marisa)

add_executable(example_SendSingleFile Examples/send_single_file.cpp)
target_link_libraries(example_SendSingleFile marisa)

add_executable(example_StaticFiles Examples/static_files.cpp)
target_link_libraries(example_StaticFiles marisa)

add_executable(example_StreamedPicUpload Examples/streamed_upload_pic.cpp)
target_link_libraries(example_StreamedPicUpload marisa)
