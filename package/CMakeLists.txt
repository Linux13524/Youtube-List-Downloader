cmake_minimum_required(VERSION 3.6)
project(YoutubeListDownloader)

include(cmake/tools.cmake)
include(cmake/conan.cmake)

set(PROJECT_VERSION 1.0.1)

### Platform dependend
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static") # link everything statically so the user does not need to install MinGW
    set(CONAN_SETTINGS compiler.exception=seh compiler.threads=posix)
elseif (ANDROID)
    set(Platform_LINKAGE log)
    # May be removed?
    # set(CMAKE_CXX_COMPILER_VERSION "7.0")
endif ()

### CONAN
# Runs conan install to get dependencies
conan_cmake_run(CONANFILE ../conanfile.py
        BASIC_SETUP
        SETTINGS ${CONAN_SETTINGS})


# BOOST NOWIDE - static
add_subdirectory(lib/nowide_standalone EXCLUDE_FROM_ALL)

set(Headers
        include/YoutubeListDownloader/sql/dbs.h
        include/YoutubeListDownloader/download/downloader.h
        include/YoutubeListDownloader/download/options.h
        include/YoutubeListDownloader/utils/fs.h
        include/YoutubeListDownloader/utils/working_queue.h
        include/YoutubeListDownloader/youtube/channel.h
        include/YoutubeListDownloader/youtube/format.h
        include/YoutubeListDownloader/youtube/playlist.h
        include/YoutubeListDownloader/youtube/video.h
        include/YoutubeListDownloader/youtube/video_list.h
        include/YoutubeListDownloader/youtube/youtube.h
        include/YoutubeListDownloader/logger.h
        lib/json/json.hpp)

set(Sources
        src/YoutubeListDownloader/sql/dbs.cpp
        src/YoutubeListDownloader/download/downloader.cpp
        src/YoutubeListDownloader/utils/working_queue.cpp
        src/YoutubeListDownloader/youtube/channel.cpp
        src/YoutubeListDownloader/youtube/playlist.cpp
        src/YoutubeListDownloader/youtube/video.cpp
        src/YoutubeListDownloader/youtube/video_list.cpp)

### Youtube-List-Downloader build
add_library(YoutubeListDownloader STATIC
        ${Headers}
        ${Sources})

set_target_properties(YoutubeListDownloader PROPERTIES DEBUG_POSTFIX "-d")

if (CMAKE_VERSION VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 11)
else()
    target_compile_features(YoutubeListDownloader PUBLIC cxx_std_11)
endif()

target_include_directories(YoutubeListDownloader
        INTERFACE $<INSTALL_INTERFACE:include>
        INTERFACE $<INSTALL_INTERFACE:lib>
        PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/YoutubeDecipher/include>
        PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/nowide_standalone>
        PRIVATE $<BUILD_INTERFACE:${CONAN_INCLUDE_DIRS}>)

target_link_libraries(YoutubeListDownloader
        PRIVATE ${CONAN_LIBS}
        PRIVATE nowide-static
        PRIVATE ${Platform_LINKAGE})


### For installation of the library
install(TARGETS YoutubeListDownloader EXPORT YoutubeListDownloaderTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
install(DIRECTORY include/YoutubeListDownloader DESTINATION include)
install(DIRECTORY lib/json DESTINATION include)
install(DIRECTORY lib/nowide_standalone/nowide DESTINATION include)

configure_and_install(YoutubeListDownloader SameMajorVersion)
