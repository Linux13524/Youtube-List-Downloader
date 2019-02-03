[![Download](https://api.bintray.com/packages/linux13524/conan/youtube_list_downloader%3Alinux13524/images/download.svg) ](https://bintray.com/linux13524/conan/youtube_list_downloader%3Alinux13524/_latestVersion)
[![Build Status Travis](https://travis-ci.com/Linux13524/YoutubeListDownloader.svg?branch=testing%2F1.0.0)](https://travis-ci.com/Linux13524/YoutubeListDownloader)
[![Build Status AppVeyor](https://ci.appveyor.com/api/projects/status/github/linux13524/youtubelistdownloader?branch=testing%2F1.0.0&svg=true)](https://ci.appveyor.com/project/linux13524/youtubelistdownloader)

## Conan package recipe for *youtube_list_downloader*

Multiplatform library for downloading Youtube content

The packages generated with this **conanfile** can be found on [Bintray](https://bintray.com/youtube_list_downloader/conan/youtube_list_downloader%3Ayoutube_list_downloader).



## For Users

### Basic setup

    $ conan install youtube_list_downloader/1.0.0@linux13524/testing

### Project setup

If you handle multiple dependencies in your project is better to add a *conanfile.txt*

    [requires]
    youtube_list_downloader/1.0.0@linux13524/testing

    [generators]
    cmake

Complete the installation of requirements for your project running:

    $ mkdir build && cd build && conan install ..

Note: It is recommended that you run conan install from a build directory and not the root of the project directory.  This is because conan generates *conanbuildinfo* files specific to a single build configuration which by default comes from an autodetected default profile located in ~/.conan/profiles/default .  If you pass different build configuration options to conan install, it will generate different *conanbuildinfo* files.  Thus, they should not be added to the root of the project, nor committed to git.


## Build and package

The following command both runs all the steps of the conan file, and publishes the package to the local system cache.  This includes downloading dependencies from "build_requires" and "requires" , and then running the build() method.

    $ conan create . linux13524/testing


### Available Options
| Option        | Default | Possible Values  |
| ------------- |:----------------- |:------------:|
| shared      | False |  [True, False] |
| fPIC      | True |  [True, False] |


## Add Remote

    $ conan remote add linux13524 "https://api.bintray.com/conan/linux13524/conan"


## License

[MIT](https://github.com/Linux13524/Youtube-List-Downloader/blob/testing/1.0.0/LICENSE.md)
