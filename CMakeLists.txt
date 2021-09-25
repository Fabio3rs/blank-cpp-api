set(apiname unknownapi)

cmake_minimum_required(VERSION 3.6.0)
project(${apiname} VERSION 0.1.0)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Werror -std=c++17 -Wno-c++98-compat -Wno-padded -Wno-date-time -Wno-c++98-compat-pedantic -Wno-exit-time-destructors -Wno-global-constructors")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_DEFAULT 17)
set(CXX17 ON)
set(CMAKE_CXX_EXTENSIONS Off)
set(CMAKE_C_EXTENSIONS Off)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(CPack)

# procura as libs necessárias
find_package(CURL REQUIRED)
find_package(Poco REQUIRED COMPONENTS Crypto JSON Net NetSSL)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig)
pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)

include(CTest)
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_subdirectory(${CMAKE_SOURCE_DIR}/tests)

# setando todos arquivos fontes que serão compilados
file(
    GLOB SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WebControllers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/models/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WebInterface/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Authorization/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WebRequest/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Database/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GluoCRM/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FormRequest/*.cpp"
)

# setando todos arquivos include
set(INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

# adicionando os caminhos a seres procurados os arquivos .hpp
include_directories(${INCLUDE_DIRS})

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-header-filter=${CMAKE_CURRENT_SOURCE_DIR}/src;--warnings-as-errors=\"*\";-extra-arg=-std=c++17;-export-fixes;-checks=read*,modern*,hicpp*,performance*")
add_executable(${apiname} ${SOURCES})

target_compile_definitions(${apiname} PRIVATE "PROJECT_NAME=\"unknownapi\"")

# Compiler-specific compile flags
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    message(STATUS "Setting G++ flags")
    # G++
    target_compile_options(${apiname} PRIVATE -Wall -Werror -Wextra -O2 -std=c++17 -Wformat-security -Wconversion -Wsign-conversion)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    message(STATUS "Setting MSVC flags")
    # MSVC
    target_compile_options(${apiname} PRIVATE /EHsc /W2 /c)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    message(STATUS "Setting Clang flags")
    # Activate all clang warnings except those that don't really affect much
    target_compile_options(${apiname} PRIVATE -O2 -Weverything -Werror -Wno-date-time -Wno-disabled-macro-expansion -Wno-global-constructors -std=c++17 -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-exit-time-destructors)
endif()

target_precompile_headers(${apiname} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.hpp"
                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/WebInterface/webstdafx.hpp"
                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/WebInterface/pistache.hpp")

# verifica se o curl foi encontrado e faz o link com a lib
if(CURL_FOUND)
    message("-- Curl encontrado!")
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(${apiname} ${CURL_LIBRARIES})
else()
    message(FATAL_ERROR "-- LibCurl is not found in your system.")
endif(CURL_FOUND)

# verifica se o pistache foi encontrado e faz o link com a lib
if(Pistache_FOUND)
    message("-- Pistache encontrado!")
    target_link_libraries(${apiname} PkgConfig::Pistache)
else()
    message(FATAL_ERROR "-- Pistache is not found in your system.")
endif(Pistache_FOUND)

# verifica se o poco foi encontrado e faz o link com a lib
if(Poco_FOUND)
    message("-- Poco encontrado!")
    target_link_libraries(${apiname} Poco::Crypto)
    target_link_libraries(${apiname} Poco::JSON)
    target_link_libraries(${apiname} Poco::Net)
    target_link_libraries(${apiname} Poco::NetSSL)
else()
    message(FATAL_ERROR "-- Poco is not found in your system.")
endif(Poco_FOUND)

target_link_libraries(${apiname} mysqlcppconn)


