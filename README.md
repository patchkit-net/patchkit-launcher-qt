# patchkit-launcher-qt
PatchKit launcher made in Qt.

## [Changelog](/CHANGELOG.md)

## Requirements
* [Qt Framework](https://www.qt.io/download/)
* Microsoft Visual Studio (only for Windows)

## [Code Style](/CODE_STYLE.md)
## [Building](/BUILDING.md)

## Launcher data file

Launcher data file is storing information about patcher and application secret. Both secrets are encrypted and written in raw format.
Data file location depends on the platform:

* Windows - launcher data file is stored in launcher executable resources under `3151` id as resource of type `RT_RCDATA` (type id - `10`). [Read more](https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms648009(v=vs.85).aspx). If the resource is missing or corrupted, the launcher is searching for data file next to launcher location (the same method as Linux does).
* Mac OSX - it's placed in the Resources directory of bundle.
* Linux - it's placed next to the launcher executable.

## Log file

Launcher produces a log file during it's execution named `Launcher-log.txt`. It's location depends on the platform and permissions.

If the Launcher has necessary write permissions.

* Windows - log file is located next to executable.
* Linux - log file is located next to executable.
* Mac OSX - log file is located in `~/Library/Application Support/PatchKit/Apps`

Otherwise:

* Windows - `C:/Users/<USER>/AppData/Local/<APPNAME>`
* Linux - `~/.local/share/<APPNAME>`
* Mac OSX - unchanged

## Project structure

The Launcher project is separated into 3 subprojects:
* Src - which is compiled into a static library to be used by other subprojects.
* App - which runs the window of the application.
* Tests - which runs the tests using the [Catch testing library](https://github.com/philsquared/Catch).

## Using Visual Studio as editor

Install [Qt Visual Studio Add-in](https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408).

Then you will be able to open Qt project (`patchkit-launcher-qt.pro`) with Visual Studio by entering *Qt5* menu from toolbar and selecting **Open Qt Project File (.pro)** option. Project will be automaticaly generated and opened within a solution. Note that Visual Studio project (`patchkit-launcher-qt.vcxproj`) shouldn't be added to the repository. If you want make some modifications to project setup you need to modify `patchkit-launcher-qt.pro` file manually and regenerate Visual Studio project.