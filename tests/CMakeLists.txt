# Configuração dos testes no CMake/GoogleTest

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(CMAKE_CXX_STANDARD 17)

set(srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
set(msrcdir "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(mainbinary "${CMAKE_CURRENT_SOURCE_DIR}/../bin")

file(
    GLOB AUTHORIZATION_SOURCES 
    "${msrcdir}/Authorization/*.cpp"
)

file(
    GLOB WEBSERVER_SOURCES 
    "${msrcdir}/WebInterface/*.cpp"
)

file(
    GLOB WEBCONTROLLERS_SOURCES 
    "${msrcdir}/WebControllers/*.cpp"
)

file(
    GLOB WEBREQUEST_SOURCES 
    "${msrcdir}/WebRequest/*.cpp"
)

file(
    GLOB REGISTERCONTROLLERS 
    "${msrcdir}/RegisterControllers.cpp"
)

file(
    GLOB UTILS_SOURCES 
    "${msrcdir}/utils/*.cpp"
)

file(
    GLOB ALL_TESTS_SOURCES 
    "${srcdir}/*.cpp"
)

pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)

set(TEST_COMPILE_FLAGS -Weverything -Werror -Wno-disabled-macro-expansion -Wno-global-constructors -std=c++17 -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-exit-time-destructors -Wno-global-constructors)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Werror -std=c++17 -Wno-c++98-compat -Wno-padded -Wno-date-time -Wno-c++98-compat-pedantic -Wno-exit-time-destructors -Wno-global-constructors")

add_executable(maintest_binary ${WEBSERVER_SOURCES} ${REGISTERCONTROLLERS} ${WEBREQUEST_SOURCES} ${WEBCONTROLLERS_SOURCES} ${ALL_TESTS_SOURCES} ${AUTHORIZATION_SOURCES} ${UTILS_SOURCES})

target_precompile_headers(maintest_binary PRIVATE "${msrcdir}/stdafx.hpp"
                            PRIVATE "${msrcdir}/WebInterface/webstdafx.hpp"
                            PRIVATE "${msrcdir}/WebInterface/pistache.hpp")

target_compile_options(maintest_binary PUBLIC ${TEST_COMPILE_FLAGS})
target_link_libraries(maintest_binary GTest::GTest GTest::Main PkgConfig::Pistache)

if(CURL_FOUND)
    message("-- Curl encontrado!")
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(maintest_binary ${CURL_LIBRARIES})
else()
    message(FATAL_ERROR "-- LibCurl is not found in your system.")
endif(CURL_FOUND)

if(Poco_FOUND)
    message("-- Poco encontrado!")
    target_link_libraries(maintest_binary Poco::Crypto)
    target_link_libraries(maintest_binary Poco::JSON)
    target_link_libraries(maintest_binary Poco::Net)
    target_link_libraries(maintest_binary Poco::NetSSL)
else()
    message(FATAL_ERROR "-- Poco is not found in your system.")
endif(Poco_FOUND)

gtest_discover_tests(maintest_binary)

