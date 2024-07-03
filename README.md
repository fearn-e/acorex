# ACorEx
3D Audio Corpus Explorer

# Developers 
### Build Requirements

* CMake

* Something to run shell scripts in (e.g. Git Bash)

* IDE (VS for Windows, Xcode for Mac)

### Build Steps

* Clone the repo

* Run ./build-deps.sh - this will download all dependencies and OpenFrameworks

  * Optional arguments:

    * -c | force recompile

    * -d | force re-download (also forces recompile)

* Once the script is done, navigate to openframeworks/apps/myApps/acorex, run the VS or XCode solutions and build from there

# Attribution

ACorEx pulls in several other libraries during the build process:
* [openframeworks/OpenFrameworks](https://github.com/openframeworks/openframeworks)
* [roymacdonald/ofxDropdown](https://github.com/roymacdonald/ofxDropdown) - (modified at [fearn-e/ofxDropdown](https://github.com/fearn-e/ofxDropdown))
* [npisanti/ofxAudioFile](https://github.com/npisanti/ofxAudioFile)
* [nlohmann/json](https://github.com/nlohmann/json)
* [flucoma/flucoma-core](https://github.com/flucoma/flucoma-core) - (modified at [fearn-e/flucoma-core](https://github.com/fearn-e/flucoma-core))
  * [libeigen/eigen](https://gitlab.com/libeigen/eigen)
  * [alexharker/HISSTools_Library](https://github.com/alexharker/hisstools_library)
  * [foonathan/memory](https://github.com/foonathan/memory)
  * [yixuan/spectra](https://github.com/yixuan/spectra)
