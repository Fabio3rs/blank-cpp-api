set(apiname blank_cpp_api)

cmake_minimum_required(VERSION 3.6.0)
project("${apiname}_lib" VERSION 0.1.0)
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

find_package(Poco REQUIRED COMPONENTS Crypto JSON Redis Net NetSSL)
find_package(CURL REQUIRED)
find_package(PkgConfig)
pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)

include(CTest)
enable_testing()
find_package(GTest)

if(GTest_FOUND)
    include(GoogleTest)
endif()

file(
    GLOB SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/DBMigrationList.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SeedDB.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/InitWebService.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WebControllers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/routes/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/jobs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/migrations/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/models/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WebInterface/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Authorization/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WebRequest/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Database/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GluoCRM/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FormRequest/*.cpp"
)

set(INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

file(GLOB JOBS_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/jobs/*.hpp"
)

add_subdirectory(vendor/cppapiframework)

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_SOURCE_DIR}/src/autogen/jobslist.hpp
  COMMAND ${CMAKE_SOURCE_DIR}/vendor/cppapiframework/scripts/genjoblist.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
  DEPENDS ${JOBS_HEADERS}
)

ADD_CUSTOM_TARGET(GenerateJobs ALL
   DEPENDS ${CMAKE_SOURCE_DIR}/src/autogen/jobslist.hpp
)

include_directories(${INCLUDE_DIRS})

add_library("${apiname}_lib" STATIC ${SOURCES})
add_executable(${apiname} "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

target_compile_definitions("${apiname}_lib" PRIVATE "PROJECT_NAME=\"unknownapi\"")
target_link_libraries("${apiname}_lib" cppapiframework)
add_dependencies("${apiname}_lib" GenerateJobs)

target_precompile_headers("${apiname}_lib" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.hpp")

# verifica se o curl foi encontrado e faz o link com a lib
if(CURL_FOUND)
    message("-- Curl encontrado!")
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries("${apiname}_lib" ${CURL_LIBRARIES})
else()
    message(FATAL_ERROR "-- LibCurl is not found in your system.")
endif(CURL_FOUND)

# verifica se o pistache foi encontrado e faz o link com a lib
if(Pistache_FOUND)
    message("-- Pistache encontrado!")
    target_link_libraries("${apiname}_lib" PkgConfig::Pistache)
else()
    message(FATAL_ERROR "-- Pistache is not found in your system.")
endif(Pistache_FOUND)

# verifica se o poco foi encontrado e faz o link com a lib
if(Poco_FOUND)
    message("-- Poco encontrado!")
    target_link_libraries("${apiname}_lib" ${Poco_LIBRARIES})
else()
    message(FATAL_ERROR "-- Poco is not found in your system.")
endif(Poco_FOUND)

target_link_libraries("${apiname}_lib" mysqlcppconn)
target_link_libraries(${apiname} "${apiname}_lib")

if(GTest_FOUND)
    add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()
