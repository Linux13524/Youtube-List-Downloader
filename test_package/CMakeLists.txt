cmake_minimum_required(VERSION 2.8.12)
project(YoutubeListDownloaderTest CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(YoutubeListDownloaderTest test.cpp)
target_include_directories(YoutubeListDownloaderTest PRIVATE "include" ${CONAN_INCLUDE_DIRS})
target_link_libraries(YoutubeListDownloaderTest PRIVATE ${CONAN_LIBS})

if (CMAKE_VERSION VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 11)
else()
    target_compile_features(YoutubeListDownloaderTest PUBLIC cxx_std_11)
endif()

