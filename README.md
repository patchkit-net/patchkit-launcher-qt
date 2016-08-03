# patchkit-launcher-qt
PatchKit launcher made in Qt.

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
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/
```

Every header file must contain guard
``` C++
#ifndef HEADERFILENAME_H
#define HEADERFILENAME_H
...
#endif // HEADERFILENAME_H
```

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

## Using Visual Studio as editor

Install [Qt Visual Studio Add-in](https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408).

Then you will be able to open Qt project (*patchkit-launcher-qt.pro*) with Visual Studio by entering *Qt5* menu from toolbar and selecting *Open Qt Project File (.pro)* option. Project will be automaticaly generated and opened within a solution. Note that Visual Studio project (*patchkit-launcher-qt.vcxproj*) shouldn't be added to the repository. If you want make some modifications to project setup you need to modify *patchkit-launcher-qt.pro* file manually and regenerate Visual Studio project. This means that if you are adding some files you need to add them manually to *patchkit-launcher-qt.pro* file.

