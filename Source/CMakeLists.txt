cmake_minimum_required(VERSION 3.4)
project(marisa)
ENABLE_TESTING()

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES

        Marisa.hpp Marisa.cpp

        Server/Instance.hpp Server/InstanceCommon.cpp
        Server/InstanceTCP.cpp Server/InstanceSSL.cpp Server/InstanceUnix.cpp

        Server/Session.hpp Server/SessionCommon.cpp
        Server/SessionTCP.cpp Server/SessionSSL.cpp Server/SessionUnix.cpp

        Server/ConnectionContext.hpp
        Server/Config.cpp Server/Config.hpp Server/Buffer.cpp Server/Buffer.hpp

        App/App.cpp App/App.hpp App/Route.cpp App/Route.hpp App/Context.cpp App/Context.hpp

        App/Request/Request.cpp App/Request/Request.hpp
        App/Response/Response.hpp App/Response/Response.cpp
        App/Middleware.cpp App/Middleware.hpp

        App/Middlewares/Middlewares.hpp App/Middlewares/CommonIncludes.hpp
        App/Middlewares/Compression.cpp App/Middlewares/Compression.hpp
        App/Middlewares/StaticFiles.cpp App/Middlewares/StaticFiles.hpp
        App/Middlewares/Redirection.cpp App/Middlewares/Redirection.hpp
        App/Middlewares/Simple.cpp App/Middlewares/Simple.hpp
        App/Middlewares/Lambda.cpp App/Middlewares/Lambda.hpp
        App/Middlewares/Websocket.cpp App/Middlewares/Websocket.hpp

        Types/Date.cpp Types/Date.hpp
        Types/MMap.cpp Types/MMap.hpp
        Types/Mullet.cpp Types/Mullet.hpp
        Types/Base64.cpp Types/Base64.hpp

        Utilities/Utilities.cpp Utilities/Utilities.hpp
        Utilities/MimeTypes.cpp

        Log/Log.cpp Log/Log.hpp


        Protocol/Protocol.hpp

        Protocol/HTTP/HTTP.hpp

        Protocol/HTTP/CommonIncludes.hpp
        Protocol/HTTP/Generator.hpp
        Protocol/HTTP/Parser.cpp Protocol/HTTP/Parser.hpp
        Protocol/HTTP/Method.cpp Protocol/HTTP/Method.hpp
        Protocol/HTTP/Status.cpp Protocol/HTTP/Status.hpp

        Protocol/HTTP/HTTP1/HTTP1.hpp

        Protocol/HTTP/HTTP1/Parser.cpp Protocol/HTTP/HTTP1/Parser.hpp
        Protocol/HTTP/HTTP1/Generator.cpp Protocol/HTTP/HTTP1/Generator.hpp

        3rdParty/http-parser/http_parser.h 3rdParty/http-parser/http_parser.c
        )

include(GNUInstallDirs)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/base64_default_config.h ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/base64/lib/config.h COPYONLY)

set(SOURCE_FILES_BASE64
        3rdParty/base64/lib/arch/avx2/codec.c 3rdParty/base64/lib/arch/generic/codec.c
        3rdParty/base64/lib/arch/neon32/codec.c 3rdParty/base64/lib/arch/neon64/codec.c
        3rdParty/base64/lib/arch/ssse3/codec.c 3rdParty/base64/lib/arch/sse41/codec.c
        3rdParty/base64/lib/arch/sse42/codec.c 3rdParty/base64/lib/arch/avx/codec.c
        3rdParty/base64/lib/lib.c 3rdParty/base64/lib/codec_choose.c)
add_library(local_base64 STATIC ${SOURCE_FILES_BASE64})
SET_TARGET_PROPERTIES(local_base64 PROPERTIES COMPILE_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")

add_library(marisa SHARED ${SOURCE_FILES})
add_library(marisa_static STATIC ${SOURCE_FILES})
set_target_properties(marisa_static PROPERTIES OUTPUT_NAME marisa)
target_link_libraries(marisa pthread boost_system boost_thread boost_coroutine z ssl crypto local_base64)
target_link_libraries(marisa_static pthread boost_system boost_thread boost_coroutine z ssl crypto local_base64)


add_executable(test_Coverage Tests/coverage.cpp)
target_link_libraries(test_Coverage marisa gcov)

add_test(NAME Coverage COMMAND test_Coverage)

add_executable(example_Hello Examples/hello.cpp)
target_link_libraries(example_Hello marisa)

add_executable(example_HelloLambda Examples/hello_lambda.cpp)
target_link_libraries(example_HelloLambda marisa)

add_executable(example_HelloAsync Examples/hello_async.cpp)
target_link_libraries(example_HelloAsync marisa)

add_executable(example_HelloSimple Examples/hello_simple.cpp)
target_link_libraries(example_HelloSimple marisa)

add_executable(example_HelloSSL Examples/hello_ssl.cpp)
target_link_libraries(example_HelloSSL marisa)

add_executable(example_HelloUnixSocket Examples/hello_unix_socket.cpp)
target_link_libraries(example_HelloUnixSocket marisa)

add_executable(example_UsageDemo Examples/usage_demo.cpp)
target_link_libraries(example_UsageDemo marisa)

add_executable(example_SendSingleFile Examples/send_single_file.cpp)
target_link_libraries(example_SendSingleFile marisa)

add_executable(example_StaticFiles Examples/static_files.cpp)
target_link_libraries(example_StaticFiles marisa)

add_executable(example_StreamedPicUpload Examples/streamed_upload_pic.cpp)
target_link_libraries(example_StreamedPicUpload marisa)

add_executable(example_RawIO Examples/raw_io.cpp)
target_link_libraries(example_RawIO marisa)

install(TARGETS marisa
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(TARGETS marisa_static
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES Marisa.hpp DESTINATION "include/Marisa")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/App ${CMAKE_SOURCE_DIR}/Log ${CMAKE_SOURCE_DIR}/Protocol
        ${CMAKE_SOURCE_DIR}/Server ${CMAKE_SOURCE_DIR}/Tests ${CMAKE_SOURCE_DIR}/Types ${CMAKE_SOURCE_DIR}/Utilities
        DESTINATION "include/Marisa"
        FILES_MATCHING
        PATTERN "*.hpp"
        )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/3rdParty
        DESTINATION "include/Marisa"
        FILES_MATCHING
        PATTERN "*.h"
        )
