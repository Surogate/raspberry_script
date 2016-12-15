cmake_minimum_required (VERSION 3.7)

project (rasberry_script)

#compiler requirement
set (CMAKE_CXX_STANDARD 14)

# the version number
set (RASPBERRY_SCRIPT_VERSION_MAJOR 0)
set (RASPBERRY_SCRIPT_VERSION_MINOR 1)

set(BOOST_ROOT_DIR ${PROJECT_SOURCE_DIR} CACHE STRING "The root directory of boost, leave it as is if you initialized subrepositories")

set(CURL_ROOT_DIR ${PROJECT_SOURCE_DIR} CACHE STRING "The root directory of boost, leave it as is if you initialized subrepositories")

# if (${BOOST_ROOT_DIR} EQUAL ${PROJECT_SOURCE_DIR})
	# include_directories("${PROJECT_SOURCE_DIR}/utility/include")
	# include_directories("${PROJECT_SOURCE_DIR}/config/include")
	# include_directories("${PROJECT_SOURCE_DIR}/throw_exception/include")
	# include_directories("${PROJECT_SOURCE_DIR}/assert/include")
# else (${BOOST_ROOT_DIR} EQUAL ${PROJECT_SOURCE_DIR})
	# include_directories("${BOOST_ROOT_DIR}")
# endif(${BOOST_ROOT_DIR} EQUAL ${PROJECT_SOURCE_DIR})

# directories where to search headers
include_directories("${BOOST_ROOT_DIR}")
link_directories("${BOOST_ROOT_DIR}/stage/lib")
include_directories("${CURL_ROOT_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/xtsslib")

# sources in the resolver_server directory
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
target_link_libraries(scan-auto-fetch libboost_program_options)

set (WHEN_IS_ON_SOURCES 
	${PROJECT_SOURCE_DIR}/when_is_on/main.cpp
	${PROJECT_SOURCE_DIR}/when_is_on/input.cpp
	${PROJECT_SOURCE_DIR}/when_is_on/input.hpp
	)

add_executable(when_is_on ${WHEN_IS_ON_SOURCES})

