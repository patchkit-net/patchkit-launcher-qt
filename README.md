# patchkit-launcher-qt
PatchKit launcher made in Qt.

## Requirements
* [Qt Framework](https://www.qt.io/download/)
* Microsoft Visual Studio (only for Windows)

### Building statically linked Qt

Refer to [PatchKit Development Kit](https://github.com/patchkit-net/patchkit-development-kit).

## Building

### Building from Qt Creator (recommended)

Open launcher-qt.pro project in Qt Creator and compile.

### Building from command line

#### Windows

First of all you need to setup the envirnonment.

**For x86 envirnonment**
``` Batch
[X86_QT_INSTALLATION_ROOT]/bin/qtenv2.bat
[VS_ROOT]/VC/bin/vcvarsall.bat
```

**For x64 envirnonment**
``` Batch
[X64_QT_INSTALLATION_ROOT]/bin/qtenv2.bat
[VS_ROOT]/VC/bin/vcvarsall.bat x64
```

Now generate Makefile with qmake.
``` Batch
qmake -r -spec win32-msvc2015
```

Compile.

**Release**
``` Batch
nmake -f Makefile.Release
```

**Debug**
``` Batch
nmake -f Makefile.Debug
```

## Launcher data file

Launcher data file is storing information about patcher and application secret. Both secrets are encrypted and written in raw format.
Data file location depends on the platform:

* Windows - launcher data file is stored in launcher executable resources under `3151` id as resource of type `RT_RCDATA` (type id - `10`). [Read more](https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms648009(v=vs.85).aspx). If the resource is missing or corrupted, the launcher is searching for data file next to launcher location (the same method as Linux does).
* Mac OSX - it's placed in the Resources directory of bundle.
* Linux - it's placed next to the launcher executable .

## Log file

Launcher produces a log file during it's execution named `Launcher-log.txt`. It's location depends on the platform and permissions.

If the Launcher has necessary write permissions.

* Windows - log file is located next to executable.
* Mac OSX - log file is located in bundle `Resources` directory.
* Linux - log file is located next to executable.

Otherwise:

* Windows - `C:/Users/<USER>/AppData/Local/<APPNAME>`
* Mac OSX - `~/Library/Application Support/<APPNAME>`
* Linux - `~/.local/share/<APPNAME>`

## Project structure

The Launcher project is separated into 3 subprojects:
* Src - which is compiled into a static library to be used by other subprojects.
* App - which runs the window of the application.
* Tests - which runs the tests using the [Catch testing library](https://github.com/philsquared/Catch).

## Using Visual Studio as editor

Install [Qt Visual Studio Add-in](https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408).

Then you will be able to open Qt project (*patchkit-launcher-qt.pro*) with Visual Studio by entering *Qt5* menu from toolbar and selecting *Open Qt Project File (.pro)* option. Project will be automaticaly generated and opened within a solution. Note that Visual Studio project (*patchkit-launcher-qt.vcxproj*) shouldn't be added to the repository. If you want make some modifications to project setup you need to modify *patchkit-launcher-qt.pro* file manually and regenerate Visual Studio project.

## Code style

### Files

* Header files - `headerfile.h`
* Source files - `sourcefile.cpp`

### Classes

* Classes - `class UpperCamelCase`
* Constructors - `ClassName(int t_lowerCamelCase)`
* Methods - `void lowerCamelCase(int t_lowerCamelCase)`
* Public field - `int lowerCamelCase;`
* Private field - `int m_lowerCamelCase;`
* Consts - `const int lowerCamelCase = 0;`

### Other
* Variable `int lowerCamelCase = 0;`
* Qt Signals - `void lowerCamelCase()`
* Qt Slots - `void lowerCamelCase()`

Every file must begin with license header
``` C++
/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/
```

Every header file must contain a compilation guard `#pragma once` at the top of the file.

## Errors

Launcher code is using exceptions to handle errors.
If something in launcher fails it should throw an exception. Exception and it's context will be saved to log file and will make debugging process easier.

## Logging

### Functions

There are five functions that can be used for creating log:

* `logDebug` - very deep informations, mostly contains raw data
* `logInfo` - describing launcher operations
* `logWarning` - describing failures from which launcher can be rescued
* `logCritical` - describing failures that makes launcher unable to work

Every function can format string. For example
``` C++
logInfo("Formatted log %1 with some data %2", .arg(QString("someData"), QString::number(5)));
```

### Rules

* Every important operation should start with log describing it's purpose but it shouldn't inform about it's result. For example:
``` C++
bool loadDataFile()
{
	logInfo("Loading data file from %1", .args(m_dataFilePath));
	...
}
```
* Log can display result of function. For example:
``` C++
if(loadDataFile())
{
	logInfo("Loaded data file.");
	...
}
else
{
	logWarning("Failed to load data file.");
	...
}
```
* Use `logCritical` to display exception description. For example:
``` C++
try
{
	...
}
catch(LauncherException &exception)
{
	logCritical(exception.what());
	...
}
```
