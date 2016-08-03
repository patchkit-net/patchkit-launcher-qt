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

## Using Visual Studio as editor

Install [Qt Visual Studio Add-in](https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408).

Then you will be able to open Qt project (*patchkit-launcher-qt.pro*) with Visual Studio by entering *Qt5* menu from toolbar and selecting *Open Qt Project File (.pro)* option. Project will be automaticaly generated and opened within a solution. Note that Visual Studio project (*patchkit-launcher-qt.vcxproj*) shouldn't be added to the repository. If you want make some modifications to project setup you need to modify *patchkit-launcher-qt.pro* file manually and regenerate Visual Studio project. This means that if you are adding some files you need to add them manually to *patchkit-launcher-qt.pro* file.

