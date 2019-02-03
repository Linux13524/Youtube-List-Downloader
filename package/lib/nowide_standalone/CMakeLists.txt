cmake_minimum_required(VERSION 2.6)

include_directories(.)

if(NOT LIBDIR)
	set(LIBDIR lib CACHE STRING "Library installation directory" FORCE)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CXX_FLAGS "-Wall -Wextra")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CXX_FLAGS "-Wall -Wextra")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	set(CXX_FLAGS "-Wall")
elseif(MSVC)
	set(CXX_FLAGS "/EHsc /W3")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	if(MSVC)
		set(NOWIDE_SUFFIX "-d")
	endif()
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

add_library(nowide-static STATIC src/iostream.cpp)
set_target_properties(nowide-static PROPERTIES
				CLEAN_DIRECT_OUTPUT 1
				OUTPUT_NAME "nowide${NOWIDE_SUFFIX}"
			)

if(MSVC)
	set_target_properties(nowide-static PROPERTIES PREFIX "lib")
endif()


install(TARGETS nowide-static EXPORT YoutubeListDownloaderTargets
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${LIBDIR}
	ARCHIVE DESTINATION ${LIBDIR})

install(DIRECTORY nowide DESTINATION include)

