# patchkit-launcher-qt
PatchKit launcher made in Qt.

## [Changelog](/CHANGELOG.md)

## Requirements
* [Qt Framework](https://www.qt.io/download/)
* Microsoft Visual Studio (only for Windows)

## [Code Style](/CODE_STYLE.md)
## [Building](/BUILDING.md)
## [Running Tests](/RUNNING_TESTS.md)

## Launcher data file

Launcher data file is storing information about patcher and application secret. Both secrets are encrypted and written in raw format.
Data file location depends on the platform:

* Windows - launcher data file is stored in launcher executable resources under `3151` id as resource of type `RT_RCDATA` (type id - `10`). [Read more](https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms648009(v=vs.85).aspx). If the resource is missing or corrupted, the launcher is searching for data file next to launcher location (the same method as Linux does).
* Mac OSX - it's placed in the Resources directory of bundle.
* Linux - it's placed next to the launcher executable.

## Runtime locations
Launcher uses a concept of working directory during it's runtime. The location of this directory platform dependent:
1. Windows: next to the executable
2. Linux: next to the executable
3. OSX: `~/Library/Application Support/PatchKit/Apps/<first 8 characters of app secret>`

The working directory will **always** contain the following files:
1. `.locations` the file describing where the patcher and the app are installed
2. `.lock` the lock file preventing another launcher from being executed at the same time

The patcher and application will be located in the directories pointed to by the `.locations` file. On Windows and Linux these locations will be in the working directory, in the `patcher` and `app` directories. On OSX the user will be asked where to install the game with the default location being the working directory.

The log file `launcher-log.txt` will be located next to the executable on Windows and Linux platforms. On OSX it will be located in the ` ~/Library/Application Support/PatchKit/Apps` directory.

The data file `launcher.dat` is required on most platforms for proper functioning of the launcher. It's expected to be next to the executable. On Windows the same data can be provided through the use of an executable resource.

## Environment variables
`PK_LAUNCHER_API_URL` Overrides the main api url \
`PK_LAUNCHER_API_CACHE_URL` Overrides **all** the cache api urls with a single cache url \
`PK_LAUNCHER_SELECT_INSTALLATION_LOCATION` If set to 1, the launcher will ask the user for the installation location. Otherwise the launcher will assume platform default behaviour.

## Using Visual Studio as editor

Install [Qt Visual Studio Add-in](https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408).

Then you will be able to open Qt project (`patchkit-launcher-qt.pro`) with Visual Studio by entering *Qt5* menu from toolbar and selecting **Open Qt Project File (.pro)** option. Project will be automaticaly generated and opened within a solution. Note that Visual Studio project (`patchkit-launcher-qt.vcxproj`) shouldn't be added to the repository. If you want make some modifications to project setup you need to modify `patchkit-launcher-qt.pro` file manually and regenerate Visual Studio project.
