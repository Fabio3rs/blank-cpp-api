# Configuração dos testes no CMake/GoogleTest

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
set(msrcdir "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(mainbinary "${CMAKE_CURRENT_SOURCE_DIR}/../bin")

file(
    GLOB ALL_TESTS_SOURCES 
    "${srcdir}/*.cpp"
)

add_executable(maintest_binary ${WEBSERVER_SOURCES} ${REGISTERCONTROLLERS} ${WEBREQUEST_SOURCES} ${WEBCONTROLLERS_SOURCES} ${ALL_TESTS_SOURCES} ${AUTHORIZATION_SOURCES} ${UTILS_SOURCES})

target_precompile_headers(maintest_binary PRIVATE "${msrcdir}/stdafx.hpp")

target_compile_options(maintest_binary PUBLIC ${TEST_COMPILE_FLAGS})
target_link_libraries(maintest_binary GTest::GTest GTest::Main ${PISTACHE_LIBRARY} "${apiname}_lib")

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
else()
    message(FATAL_ERROR "-- Poco is not found in your system.")
endif(Poco_FOUND)

gtest_discover_tests(maintest_binary)

