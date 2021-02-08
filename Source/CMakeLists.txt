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
        VERSION 0.1.5
)

CPMAddPackage(
        NAME ReGlob
        GITHUB_REPOSITORY SudoMaker/ReGlob
        VERSION 0.0.4
)

CPMAddPackage(
        NAME threadpool11
        GITHUB_REPOSITORY tghosgor/threadpool11
        VERSION 3.0.0
        GIT_TAG 39fcc1fc51671989d4f3c02903f9d39c6323b685
        DOWNLOAD_ONLY YES
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
        VERSION 0.0.1
)

set(SOURCE_FILES

        Marisa.hpp Version.hpp

        CommonIncludes.hpp

        App/Buffer.cpp App/Buffer.hpp
        App/App.cpp App/App.hpp App/Route.cpp App/Route.hpp App/Context.cpp App/Context.hpp

        App/Request.cpp App/Request.hpp
        App/Response.hpp App/Response.cpp
        App/Middleware.cpp App/Middleware.hpp

        App/Middlewares/Middlewares.hpp App/Middlewares/CommonIncludes.hpp
        App/Middlewares/StaticFiles.cpp App/Middlewares/StaticFiles.hpp
        App/Middlewares/Redirection.cpp App/Middlewares/Redirection.hpp
        App/Middlewares/Simple.cpp App/Middlewares/Simple.hpp
        App/Middlewares/Lambda.cpp App/Middlewares/Lambda.hpp
        App/Middlewares/Websocket.cpp App/Middlewares/Websocket.hpp

        Util/Util.cpp Util/Util.hpp
        Util/MimeTypes.cpp Util/DefaultStatusPage.cpp Util/JSDate.cpp

        Util/URLEncode.cpp App/Middlewares/Dashboard.cpp App/Middlewares/Dashboard.hpp)

#include_directories(${IODash_SOURCE_DIR})
include_directories(${fmt_SOURCE_DIR}/include)
include_directories(${spdlog_SOURCE_DIR}/include)
include_directories(${threadpool11_SOURCE_DIR}/threadpool11/include/threadpool11)
include_directories(${cpp-base64X_SOURCE_DIR})

add_library(threadpool11
        ${threadpool11_SOURCE_DIR}/threadpool11/include/threadpool11/pool.hpp
        ${threadpool11_SOURCE_DIR}/threadpool11/include/threadpool11/threadpool11.hpp
        ${threadpool11_SOURCE_DIR}/threadpool11/include/threadpool11/work.hpp
        ${threadpool11_SOURCE_DIR}/threadpool11/src/pool.cpp
        )

target_include_directories(threadpool11 PUBLIC ${threadpool11_SOURCE_DIR}/threadpool11/include)

if (CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(threadpool11 pthread)
endif()

add_library(marisa ${SOURCE_FILES})


target_link_libraries(marisa pthread z ssl crypto fmt cpp-base64X IODash ReGlob microhttpd threadpool11)


#add_executable(test_Coverage Tests/coverage.cpp)
#target_link_libraries(test_Coverage marisa gcov)
#
#add_test(NAME Coverage COMMAND test_Coverage)

add_executable(example_Hello Examples/hello.cpp)
target_link_libraries(example_Hello marisa)

add_executable(example_HelloStreamed Examples/hello_streamed.cpp)
target_link_libraries(example_HelloStreamed marisa)

add_executable(example_HelloSSL Examples/hello_ssl.cpp)
target_link_libraries(example_HelloSSL marisa)

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
