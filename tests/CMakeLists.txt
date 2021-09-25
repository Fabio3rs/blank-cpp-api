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
    GLOB WEBREQUEST_SOURCES 
    "${msrcdir}/WebRequest/*.cpp"
)

file(
    GLOB UTILS_SOURCES 
    "${msrcdir}/utils/*.cpp"
)

set(TEST_COMPILE_FLAGS -Weverything -Werror -Wno-disabled-macro-expansion -Wno-global-constructors -std=c++17 -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-exit-time-destructors -Wno-global-constructors)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Werror -std=c++17 -Wno-c++98-compat -Wno-padded -Wno-date-time -Wno-c++98-compat-pedantic -Wno-exit-time-destructors -Wno-global-constructors")

add_executable(test_nothing_really test_nothing.cpp)
add_executable(test_inputvalidator test_inputvalidators.cpp ${UTILS_SOURCES})
add_executable(test_ApiAccessID test_ApiAccessID.cpp ${AUTHORIZATION_SOURCES})

target_compile_options(test_nothing_really PUBLIC ${TEST_COMPILE_FLAGS})
target_compile_options(test_inputvalidator PUBLIC ${TEST_COMPILE_FLAGS})
target_compile_options(test_ApiAccessID PUBLIC ${TEST_COMPILE_FLAGS})

target_link_libraries(test_nothing_really GTest::GTest GTest::Main)
target_link_libraries(test_inputvalidator GTest::GTest GTest::Main PkgConfig::Pistache)
target_link_libraries(test_ApiAccessID GTest::GTest GTest::Main PkgConfig::Pistache)

if(Poco_FOUND)
    message("-- Poco encontrado!")
    target_link_libraries(test_inputvalidator Poco::JSON)
    target_link_libraries(test_ApiAccessID Poco::Crypto Poco::JSON)
else()
    message(FATAL_ERROR "-- Poco is not found in your system.")
endif(Poco_FOUND)

gtest_discover_tests(test_nothing_really)
gtest_discover_tests(test_inputvalidator)
gtest_discover_tests(test_ApiAccessID)

