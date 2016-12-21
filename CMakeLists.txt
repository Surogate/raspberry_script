cmake_minimum_required (VERSION 3.7)

include (xtsslib/utility.cmake)

project (rasberry_script)

#compiler requirement
set (CMAKE_CXX_STANDARD 14)

# the version number
set (RASPBERRY_SCRIPT_VERSION_MAJOR 0)
set (RASPBERRY_SCRIPT_VERSION_MINOR 1)

find_boost_lib("program_options") #utility.cmake

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

assert_exists("${PROJECT_SOURCE_DIR}/xtsslib") #utility.cmake
include_directories("${PROJECT_SOURCE_DIR}/xtsslib")

if (${UNIX} OR ${APPLE})
	find_package(Curl REQUIRED)
else ()
	set(CURL_ROOT ${PROJECT_SOURCE_DIR} CACHE STRING "The root directory of curl")
	set(CURL_ROOT_BIN ${PROJECT_SOURCE_DIR} CACHE STRING "The root directory of curl binary")
	assert_exists("${CURL_ROOT}/include") 
	assert_exists("${CURL_ROOT_BIN}/lib")
	link_directories("${CURL_ROOT_BIN}/lib")
	set (Curl_LIBRARIES 
	debug "${CURL_ROOT_BIN}/lib/Debug/libcurl_imp.lib" 
	optimized "${CURL_ROOT_BIN}/lib/Release/libcurl_imp.lib")
endif()

include_directories("${CURL_ROOT}/include")

set (RESTART_TRANSMISSION_SOURCES 
	${PROJECT_SOURCE_DIR}/restart_transmission/main.cpp
	)

add_executable(restart_transmission ${RESTART_TRANSMISSION_SOURCES})

set (SCAN_AUTO_FETCH_SOURCES
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/main.cpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/anime_db.cpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/anime_db.hpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/configuration_input.cpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/configuration_input.hpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/curl_getter.cpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/curl_getter.hpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/error_code.cpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/error_code.hpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/fetch_image.cpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/fetch_image.hpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/source_token.cpp
	${PROJECT_SOURCE_DIR}/scan-auto-fetch/source_token.hpp
	)
	
add_executable(scan-auto-fetch ${SCAN_AUTO_FETCH_SOURCES})
target_link_libraries(scan-auto-fetch ${Boost_LIBRARIES} ${Curl_LIBRARIES})
# add_custom_command(TARGET scan-auto-fetch POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CURL_ROOT_BIN}/lib/\$\(Configuration\)/libcurl.dll" "${CMAKE_CURRENT_BINARY_DIR}/\$\(Configuration\)/libcurl.dll" )

set (WHEN_IS_ON_SOURCES 
	${PROJECT_SOURCE_DIR}/when_is_on/main.cpp
	${PROJECT_SOURCE_DIR}/when_is_on/input.cpp
	${PROJECT_SOURCE_DIR}/when_is_on/input.hpp
	)

add_executable(when_is_on ${WHEN_IS_ON_SOURCES})

