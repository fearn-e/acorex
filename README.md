# ACorEx
[3D Audio Corpus Explorer](https://acorex.fearne.net) is a tool for exploring audio corpora, similar to existing software like AudioStellar and CataRT. It was created to investigate ideas of 3D navigation and time series analysis in this space, and to help researchers and musicians to navigate through large collections of audio files. The tool provides an interface for analysing, browsing, and listening to audio files.

## Important Note
I recently cleaned the repo history of some large collections of mp3 files that briefly existed all the way back at the start. This cut the full repo clone size from ~140MB to just under 4MB, so I feel it was completely worth it. That being said, it more or less ended up changing the hash of every single commit in the repo. If you have the repo cloned and try to pull in any changes, you will probably get errors that you will either have to fix manually or just transfer any local changes to a fresh clone of the repo, apologies.

# Developers 
### Current State


### Build Requirements

* CMake

* Something to run shell scripts in (e.g. Git Bash)

* IDE (VS for Windows, Xcode for Mac)

### Build Steps

* Clone the repo

* (Recommended) Checkout the dev branch -- there is a good chance that the main branch no longer builds as expected until v2.0.0 is released as modified dependencies have been updated and changed and may no longer build correctly on the 2 year old version of the codebase.

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
